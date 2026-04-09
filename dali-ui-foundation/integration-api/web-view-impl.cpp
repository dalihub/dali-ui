/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// CLASS HEADER
#include "web-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/adaptor-framework/native-image.h>
#include <dali/public-api/object/property-conditions.h>
#include <algorithm>
#include <functional>
#include <memory>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/devel-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-ui-foundation/devel-api/visuals/visual-actions-devel.h>
#include <dali-ui-foundation/devel-api/visuals/visual-properties-devel.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/image-loader/image-url-utils.h>
#include <dali-ui-foundation/public-api/image-loader/image-url.h>
#include <dali-ui-foundation/public-api/visuals/color-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/image-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

namespace
{

// ---------------------------------------------------------------------------
// Type registration factory function
// ---------------------------------------------------------------------------

BaseHandle CreateWebView()
{
  WebViewImplPtr impl = WebViewImpl::New();
  Ui::View       view(*impl);
  impl->Initialize();
  return view;
}

// clang-format off
DALI_TYPE_REGISTRATION_BEGIN(WebViewImpl, ViewImpl, CreateWebView)

DALI_PROPERTY_REGISTRATION(Ui::Integration, WebViewImpl, "url",                     STRING,  URL                       )
DALI_PROPERTY_REGISTRATION(Ui::Integration, WebViewImpl, "userAgent",               STRING,  USER_AGENT                )
DALI_PROPERTY_REGISTRATION(Ui::Integration, WebViewImpl, "scrollPosition",          VECTOR2, SCROLL_POSITION           )
DALI_PROPERTY_REGISTRATION(Ui::Integration, WebViewImpl, "scrollSize",              VECTOR2, SCROLL_SIZE               )
DALI_PROPERTY_REGISTRATION(Ui::Integration, WebViewImpl, "contentSize",             VECTOR2, CONTENT_SIZE              )
DALI_PROPERTY_REGISTRATION(Ui::Integration, WebViewImpl, "title",                   STRING,  TITLE                     )
DALI_PROPERTY_REGISTRATION(Ui::Integration, WebViewImpl, "videoHoleEnabled",        BOOLEAN, VIDEO_HOLE_ENABLED        )
DALI_PROPERTY_REGISTRATION(Ui::Integration, WebViewImpl, "mouseEventsEnabled",      BOOLEAN, MOUSE_EVENTS_ENABLED      )
DALI_PROPERTY_REGISTRATION(Ui::Integration, WebViewImpl, "keyEventsEnabled",        BOOLEAN, KEY_EVENTS_ENABLED        )
DALI_PROPERTY_REGISTRATION(Ui::Integration, WebViewImpl, "documentBackgroundColor", VECTOR4, DOCUMENT_BACKGROUND_COLOR )
DALI_PROPERTY_REGISTRATION(Ui::Integration, WebViewImpl, "tilesClearedWhenHidden",  BOOLEAN, TILES_CLEARED_WHEN_HIDDEN )
DALI_PROPERTY_REGISTRATION(Ui::Integration, WebViewImpl, "tileCoverAreaMultiplier", FLOAT,   TILE_COVER_AREA_MULTIPLIER)
DALI_PROPERTY_REGISTRATION(Ui::Integration, WebViewImpl, "cursorEnabledByClient",   BOOLEAN, CURSOR_ENABLED_BY_CLIENT  )
DALI_PROPERTY_REGISTRATION(Ui::Integration, WebViewImpl, "selectedText",            STRING,  SELECTED_TEXT             )
DALI_PROPERTY_REGISTRATION(Ui::Integration, WebViewImpl, "pageZoomFactor",          FLOAT,   PAGE_ZOOM_FACTOR          )
DALI_PROPERTY_REGISTRATION(Ui::Integration, WebViewImpl, "textZoomFactor",          FLOAT,   TEXT_ZOOM_FACTOR          )
DALI_PROPERTY_REGISTRATION(Ui::Integration, WebViewImpl, "loadProgressPercentage",  FLOAT,   LOAD_PROGRESS_PERCENTAGE  )

DALI_TYPE_REGISTRATION_END()
// clang-format on

// ---------------------------------------------------------------------------
// Visual property defaults
// ---------------------------------------------------------------------------

Dali::Property::Map CreateTransparentColorVisualProperties()
{
  Dali::Property::Map map;
  map.Insert(Dali::Ui::Visual::Property::TYPE, Dali::Ui::Visual::Type::COLOR);
  map.Insert(Dali::Ui::ColorVisual::Property::MIX_COLOR, Color::TRANSPARENT);
  return map;
}

const Dali::Property::Map TRANSPARENT_COLOR_VISUAL_PROPERTIES = CreateTransparentColorVisualProperties();

constexpr Vector4 FULL_TEXTURE_RECT(0.0f, 0.0f, 1.0f, 1.0f);

// ---------------------------------------------------------------------------
// Helper: calculate the exact texture ratio for partial-update pixel area
// ---------------------------------------------------------------------------
Vector2 CalculateTextureRatio(const Size& viewSize, uint32_t textureWidth, uint32_t textureHeight)
{
  float widthRatio  = (textureWidth == 0u) ? 1.0f : viewSize.width / static_cast<float>(textureWidth);
  float heightRatio = (textureHeight == 0u) ? 1.0f : viewSize.height / static_cast<float>(textureHeight);
  return Vector2(widthRatio, heightRatio);
}

// ---------------------------------------------------------------------------
// Helper: calculate screen display area from actor properties
// ---------------------------------------------------------------------------
Rect<int32_t> CalculateDisplayArea(Dali::Actor self)
{
  bool    positionUsesAnchorPoint = self.GetProperty<bool>(Actor::Property::POSITION_USES_PIVOT);
  Vector3 actorSize               = self.GetCurrentProperty<Vector3>(Actor::Property::SIZE) * self.GetCurrentProperty<Vector3>(Actor::Property::SCALE);
  Vector3 anchorOff               = actorSize * (positionUsesAnchorPoint ? self.GetCurrentProperty<Vector3>(Actor::Property::PIVOT) : Pivot::TOP_LEFT);
  Vector2 screenPos               = self.GetProperty<Vector2>(Actor::Property::SCREEN_POSITION);

  Dali::Rect<int32_t> area;
  area.x      = static_cast<int32_t>(screenPos.x - anchorOff.x);
  area.y      = static_cast<int32_t>(screenPos.y - anchorOff.y);
  area.width  = static_cast<int32_t>(actorSize.x);
  area.height = static_cast<int32_t>(actorSize.y);
  return area;
}

} // namespace

// ===========================================================================
// Construction & Destruction
// ===========================================================================

WebViewImpl::WebViewImpl()
: ViewImpl(),
  mVisual(),
  mWebViewSize(Stage::GetCurrent().GetSize()),
  mWebViewArea(0, 0, static_cast<int32_t>(mWebViewSize.width), static_cast<int32_t>(mWebViewSize.height)),
  mLastRenderedNativeImageWidth(0u),
  mLastRenderedNativeImageHeight(0u),
  mVisualChangeRequired(false),
  mVideoHoleEnabled(false),
  mMouseEventsEnabled(true),
  mKeyEventsEnabled(true)
{
  // WebEngine instance is created in the factory (New()), not here,
  // so that the caller can choose type/argc/argv.
}

WebViewImpl::~WebViewImpl()
{
  if(mWebEngine)
  {
    mWebEngine.Destroy();
  }
}

// ===========================================================================
// Factory methods
// ===========================================================================

WebViewImplPtr WebViewImpl::New()
{
  auto* impl       = new WebViewImpl();
  impl->mWebEngine = Dali::WebEngine::New();
  if(impl->mWebEngine)
  {
    impl->mWebEngine.Create(
      static_cast<uint32_t>(impl->mWebViewSize.width),
      static_cast<uint32_t>(impl->mWebViewSize.height),
      /*locale=*/"",
      /*timezoneId=*/"");
  }
  return impl;
}

// ===========================================================================
// Property system
// ===========================================================================

void WebViewImpl::SetProperty(Dali::BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value)
{
  Ui::View view = Ui::View::DownCast(Dali::BaseHandle(object));
  if(!view)
  {
    return;
  }
  WebViewImpl& impl = static_cast<WebViewImpl&>(GetImpl(view));

  switch(index)
  {
    case Property::URL:
    {
      Dali::String url;
      if(value.Get(url))
      {
        impl.LoadUrl(url);
      }
      break;
    }
    case Property::USER_AGENT:
    {
      Dali::String agent;
      if(value.Get(agent))
      {
        impl.SetUserAgent(agent);
      }
      break;
    }
    case Property::SCROLL_POSITION:
    {
      Vector2 pos;
      if(value.Get(pos) && impl.mWebEngine)
      {
        impl.mWebEngine.SetScrollPosition(static_cast<int32_t>(pos.x), static_cast<int32_t>(pos.y));
      }
      break;
    }
    case Property::VIDEO_HOLE_ENABLED:
    {
      bool enabled;
      if(value.Get(enabled))
      {
        impl.SetVideoHoleEnabled(enabled);
      }
      break;
    }
    case Property::MOUSE_EVENTS_ENABLED:
    {
      bool enabled;
      if(value.Get(enabled))
      {
        impl.SetMouseEventsEnabled(enabled);
      }
      break;
    }
    case Property::KEY_EVENTS_ENABLED:
    {
      bool enabled;
      if(value.Get(enabled))
      {
        impl.SetKeyEventsEnabled(enabled);
      }
      break;
    }
    case Property::DOCUMENT_BACKGROUND_COLOR:
    {
      Vector4 color;
      if(value.Get(color))
      {
        impl.SetDocumentBackgroundColor(color);
      }
      break;
    }
    case Property::TILES_CLEARED_WHEN_HIDDEN:
    {
      bool cleared;
      if(value.Get(cleared))
      {
        impl.SetTilesClearedWhenHidden(cleared);
      }
      break;
    }
    case Property::TILE_COVER_AREA_MULTIPLIER:
    {
      float multiplier;
      if(value.Get(multiplier))
      {
        impl.SetTileCoverAreaMultiplier(multiplier);
      }
      break;
    }
    case Property::CURSOR_ENABLED_BY_CLIENT:
    {
      bool enabled;
      if(value.Get(enabled))
      {
        impl.SetCursorEnabledByClient(enabled);
      }
      break;
    }
    case Property::PAGE_ZOOM_FACTOR:
    {
      float zoom;
      if(value.Get(zoom))
      {
        impl.SetPageZoomFactor(zoom);
      }
      break;
    }
    case Property::TEXT_ZOOM_FACTOR:
    {
      float zoom;
      if(value.Get(zoom))
      {
        impl.SetTextZoomFactor(zoom);
      }
      break;
    }
    default:
      break;
  }
}

Dali::Property::Value WebViewImpl::GetProperty(Dali::BaseObject* object, Dali::Property::Index index)
{
  Dali::Property::Value value;
  Ui::View              view = Ui::View::DownCast(Dali::BaseHandle(object));
  if(!view)
  {
    return value;
  }
  WebViewImpl& impl = static_cast<WebViewImpl&>(GetImpl(view));

  switch(index)
  {
    case Property::URL:
      value = impl.GetUrl();
      break;
    case Property::USER_AGENT:
      value = impl.GetUserAgent();
      break;
    case Property::SCROLL_POSITION:
      value = impl.GetScrollPosition();
      break;
    case Property::SCROLL_SIZE:
      value = impl.GetScrollSize();
      break;
    case Property::CONTENT_SIZE:
      value = impl.GetContentSize();
      break;
    case Property::TITLE:
      value = impl.GetTitle();
      break;
    case Property::VIDEO_HOLE_ENABLED:
      value = impl.mVideoHoleEnabled;
      break;
    case Property::MOUSE_EVENTS_ENABLED:
      value = impl.mMouseEventsEnabled;
      break;
    case Property::KEY_EVENTS_ENABLED:
      value = impl.mKeyEventsEnabled;
      break;
    case Property::SELECTED_TEXT:
      value = impl.GetSelectedText();
      break;
    case Property::PAGE_ZOOM_FACTOR:
      value = impl.GetPageZoomFactor();
      break;
    case Property::TEXT_ZOOM_FACTOR:
      value = impl.GetTextZoomFactor();
      break;
    case Property::LOAD_PROGRESS_PERCENTAGE:
      value = impl.GetLoadProgressPercentage();
      break;
    default:
      break;
  }
  return value;
}

// ===========================================================================
// ViewImpl overrides
// ===========================================================================

void WebViewImpl::OnInitialize()
{
  DALI_LOG_DEBUG_INFO("[WebViewImpl] OnInitialize()\n");

  ViewImpl::OnInitialize();

  Actor self = Self();

  // Make the view keyboard-focusable so it can receive key events.
  self.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
  self.SetProperty(DevelActor::Property::TOUCH_FOCUSABLE, true);

  // --- Property notifications for display-area tracking ---
  // Fire when world position, size, or scale change by at least 1 unit / 0.1 scale step.
  mPositionUpdateNotification = self.AddPropertyNotification(Actor::Property::WORLD_POSITION, StepCondition(1.0f, 1.0f));
  mSizeUpdateNotification     = self.AddPropertyNotification(Actor::Property::SIZE, StepCondition(1.0f, 1.0f));
  mScaleUpdateNotification    = self.AddPropertyNotification(Actor::Property::WORLD_SCALE, StepCondition(0.1f, 1.0f));

  mPositionUpdateNotification.NotifySignal().Connect(this, &WebViewImpl::OnDisplayAreaUpdated);
  mSizeUpdateNotification.NotifySignal().Connect(this, &WebViewImpl::OnDisplayAreaUpdated);
  mScaleUpdateNotification.NotifySignal().Connect(this, &WebViewImpl::OnDisplayAreaUpdated);

  // --- Initial placeholder visual (transparent color) ---
  // This ensures the actor is renderable even before the first frame arrives.
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  {
    Ui::Visual::Base placeholderVisual = Ui::VisualFactory::Get().CreateVisual(TRANSPARENT_COLOR_VISUAL_PROPERTIES);
    if(placeholderVisual)
    {
      viewData.RegisterVisual(WebViewImpl::Property::URL, placeholderVisual);
    }
  }

  // --- Register frame-rendered callback ---
  if(mWebEngine)
  {
    mWebEngine.RegisterFrameRenderedCallback(std::bind(&WebViewImpl::OnFrameRendered, this));
  }
}

MeasuredSize WebViewImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  // WebView fills whatever size the parent provides.
  // If constraints are unconstrained (INFINITY), fall back to the initial stage size.
  float w = std::isinf(widthConstraint) ? mWebViewSize.width : widthConstraint;
  float h = std::isinf(heightConstraint) ? mWebViewSize.height : heightConstraint;

  DALI_LOG_DEBUG_INFO("[WebViewImpl] OnMeasure: constraint=(%.0f,%.0f) -> (%.0f,%.0f)\n",
                      widthConstraint, heightConstraint, w, h);
  return MeasuredSize(w, h);
}

MeasuredSize WebViewImpl::OnArrange(const LayoutRect& bounds)
{
  DALI_LOG_DEBUG_INFO("[WebViewImpl] OnArrange: bounds=(x=%.0f,y=%.0f,w=%.0f,h=%.0f)\n",
                      bounds.x, bounds.y, bounds.width, bounds.height);

  MeasuredSize result = ViewImpl::OnArrange(bounds);

  // Recalculate and apply the new display area.
  auto displayArea = CalculateDisplayArea(Self());
  SetDisplayArea(displayArea);

  return result;
}

// ===========================================================================
// Internal rendering helpers
// ===========================================================================

void WebViewImpl::OnFrameRendered()
{
  // Forward to any user-registered frame-rendered callback first.
  if(mFrameRenderedCallback)
  {
    mFrameRenderedCallback();
  }

  // Only rebuild the visual when it hasn't been created yet or a size change occurred.
  if(!mVisualChangeRequired && mVisual)
  {
    return;
  }
  mVisualChangeRequired = false;

  if(!mWebEngine)
  {
    return;
  }

  auto nativeImagePtr = mWebEngine.GetNativeImage();
  if(!nativeImagePtr)
  {
    DALI_LOG_ERROR("[WebViewImpl] GetNativeImage() returned null\n");
    return;
  }

  mLastRenderedNativeImageWidth  = nativeImagePtr->GetWidth();
  mLastRenderedNativeImageHeight = nativeImagePtr->GetHeight();

  DALI_LOG_DEBUG_INFO("[WebViewImpl] OnFrameRendered: nativeImage=%ux%u\n",
                      mLastRenderedNativeImageWidth, mLastRenderedNativeImageHeight);

  // Convert the native image into a URL understood by the ImageVisual.
  Dali::Ui::ImageUrl nativeImageUrl = Dali::Ui::ImageUrlUtils::GenerateUrl(nativeImagePtr, /*useNativeImage=*/true);

  Dali::Property::Map imageVisualMap;
  imageVisualMap.Insert(Dali::Ui::Visual::Property::TYPE, static_cast<int>(Dali::Ui::Visual::Type::IMAGE));
  imageVisualMap.Insert(Dali::Ui::ImageVisual::Property::URL, nativeImageUrl.GetUrl());
  imageVisualMap.Insert(Dali::Ui::ImageVisual::Property::PIXEL_AREA, FULL_TEXTURE_RECT);
  imageVisualMap.Insert(Dali::Ui::ImageVisual::Property::WRAP_MODE_U, static_cast<int>(WrapMode::CLAMP_TO_EDGE));
  imageVisualMap.Insert(Dali::Ui::ImageVisual::Property::WRAP_MODE_V, static_cast<int>(WrapMode::CLAMP_TO_EDGE));

  mVisual = Ui::VisualFactory::Get().CreateVisual(imageVisualMap);
  if(mVisual)
  {
    auto& viewData = Internal::ViewDataImpl::Get(*this);
    viewData.RegisterVisual(WebViewImpl::Property::URL, mVisual, DepthIndex::CONTENT);
    EnableBlendMode(!mVideoHoleEnabled);
  }
  else
  {
    DALI_LOG_ERROR("[WebViewImpl] OnFrameRendered: failed to create image visual\n");
  }
}

void WebViewImpl::OnDisplayAreaUpdated(Dali::PropertyNotification& /*source*/)
{
  if(!mWebEngine)
  {
    return;
  }
  auto displayArea = CalculateDisplayArea(Self());
  SetDisplayArea(displayArea);
}

void WebViewImpl::SetDisplayArea(const Dali::Rect<int32_t>& displayArea)
{
  Size displaySize(static_cast<float>(displayArea.width), static_cast<float>(displayArea.height));

  if(mWebViewSize != displaySize)
  {
    mWebViewSize = displaySize;
  }

  if(mWebViewArea != displayArea)
  {
    // A size/position change requires the visual to be re-created on the next frame.
    mVisualChangeRequired = true;

    DALI_LOG_DEBUG_INFO("[WebViewImpl] SetDisplayArea: (%d,%d)%dx%d -> (%d,%d)%dx%d\n",
                        mWebViewArea.x, mWebViewArea.y, mWebViewArea.width, mWebViewArea.height,
                        displayArea.x, displayArea.y, displayArea.width, displayArea.height);

    // Until the new frame arrives, adjust the existing visual's pixel area to avoid
    // showing the wrong portion of the old texture.
    if(mVisual && mLastRenderedNativeImageWidth > 0u && mLastRenderedNativeImageHeight > 0u)
    {
      const Vector2 textureRatio = CalculateTextureRatio(mWebViewSize,
                                                         mLastRenderedNativeImageWidth,
                                                         mLastRenderedNativeImageHeight);
      const Vector4 pixelArea(0.0f, 0.0f,
                              std::min(1.0f, textureRatio.x),
                              std::min(1.0f, textureRatio.y));
      const Vector2 transformSize(
        Dali::EqualsZero(textureRatio.x) ? 1.0f : std::min(1.0f, 1.0f / textureRatio.x),
        Dali::EqualsZero(textureRatio.y) ? 1.0f : std::min(1.0f, 1.0f / textureRatio.y));

      Dali::Property::Map updateMap;
      updateMap.Insert(Dali::Ui::ImageVisual::Property::PIXEL_AREA, pixelArea);
      updateMap.Insert(Dali::Ui::Visual::Property::TRANSFORM,
                       Dali::Property::Map{{Dali::Ui::Visual::Transform::Property::SIZE, transformSize}});

      auto& viewData = Internal::ViewDataImpl::Get(*this);
      viewData.DoAction(WebViewImpl::Property::URL, DevelVisual::Action::UPDATE_PROPERTY, updateMap);
    }

    mWebViewArea = displayArea;

    if(mWebEngine)
    {
      mWebEngine.UpdateDisplayArea(mWebViewArea);
    }
  }
}

void WebViewImpl::EnableBlendMode(bool blend)
{
  Actor self = Self();
  if(self)
  {
    Renderer renderer = self.GetRendererAt(0u);
    if(renderer)
    {
      renderer.SetProperty(Renderer::Property::BLEND_MODE,
                           blend ? BlendMode::ON_WITHOUT_CULL : BlendMode::OFF);
    }
  }
}

Dali::Ui::ImageView WebViewImpl::CreateImageViewFromPixelData(Dali::PixelData pixelData)
{
  if(!pixelData)
  {
    return {};
  }
  Dali::Ui::ImageUrl imageUrl = Dali::Ui::ImageUrlUtils::GenerateUrl(pixelData);
  return Dali::Ui::ImageView::New(imageUrl.GetUrl());
}

// ===========================================================================
// Conversion helpers
// ===========================================================================

std::string WebViewImpl::ToStdString(const Dali::String& str)
{
  return std::string(str.CStr());
}

Dali::String WebViewImpl::ToDaliString(const std::string& str)
{
  return Dali::String(str.c_str());
}

WebViewScrollEdge WebViewImpl::ToUiScrollEdge(Dali::WebEnginePlugin::ScrollEdge edge)
{
  switch(edge)
  {
    case Dali::WebEnginePlugin::ScrollEdge::LEFT:
      return WebViewScrollEdge::LEFT;
    case Dali::WebEnginePlugin::ScrollEdge::RIGHT:
      return WebViewScrollEdge::RIGHT;
    case Dali::WebEnginePlugin::ScrollEdge::TOP:
      return WebViewScrollEdge::TOP;
    case Dali::WebEnginePlugin::ScrollEdge::BOTTOM:
      return WebViewScrollEdge::BOTTOM;
    default:
      return WebViewScrollEdge::BOTTOM;
  }
}

WebViewOverScrolled WebViewImpl::ToUiOverScrolled(Dali::WebEnginePlugin::OverScrolled overScrolled)
{
  switch(overScrolled)
  {
    case Dali::WebEnginePlugin::OverScrolled::LEFT:
      return WebViewOverScrolled::LEFT;
    case Dali::WebEnginePlugin::OverScrolled::RIGHT:
      return WebViewOverScrolled::RIGHT;
    case Dali::WebEnginePlugin::OverScrolled::TOP:
      return WebViewOverScrolled::TOP;
    case Dali::WebEnginePlugin::OverScrolled::BOTTOM:
      return WebViewOverScrolled::BOTTOM;
    default:
      return WebViewOverScrolled::BOTTOM;
  }
}

Dali::WebEnginePlugin::FindOption WebViewImpl::ToEngineFindOption(WebViewFindOption options)
{
  // Both enums have identical bit layout — cast directly.
  return static_cast<Dali::WebEnginePlugin::FindOption>(static_cast<uint32_t>(options));
}

// ===========================================================================
// URL & User Agent
// ===========================================================================

Dali::String WebViewImpl::GetUrl() const
{
  return mWebEngine ? ToDaliString(mWebEngine.GetUrl()) : Dali::String();
}

void WebViewImpl::SetUserAgent(const Dali::String& userAgent)
{
  if(mWebEngine)
  {
    mWebEngine.SetUserAgent(ToStdString(userAgent));
  }
}

Dali::String WebViewImpl::GetUserAgent() const
{
  return mWebEngine ? ToDaliString(mWebEngine.GetUserAgent()) : Dali::String();
}

// ===========================================================================
// Page Loading
// ===========================================================================

void WebViewImpl::LoadUrl(const Dali::String& url)
{
  if(mWebEngine)
  {
    DALI_LOG_DEBUG_INFO("[WebViewImpl] LoadUrl: %s\n", url.CStr());
    mWebEngine.LoadUrl(ToStdString(url));
  }
}

void WebViewImpl::LoadHtmlString(const Dali::String& htmlString)
{
  if(mWebEngine)
  {
    mWebEngine.LoadHtmlString(ToStdString(htmlString));
  }
}

bool WebViewImpl::LoadHtmlStringOverrideCurrentEntry(const Dali::String& html, const Dali::String& basicUri, const Dali::String& unreachableUrl)
{
  return mWebEngine ? mWebEngine.LoadHtmlStringOverrideCurrentEntry(ToStdString(html), ToStdString(basicUri), ToStdString(unreachableUrl)) : false;
}

bool WebViewImpl::LoadContents(const int8_t* contents, uint32_t contentSize, const Dali::String& mimeType, const Dali::String& encoding, const Dali::String& baseUri)
{
  return mWebEngine ? mWebEngine.LoadContents(contents, contentSize, ToStdString(mimeType), ToStdString(encoding), ToStdString(baseUri)) : false;
}

void WebViewImpl::Reload()
{
  if(mWebEngine)
  {
    mWebEngine.Reload();
  }
}

bool WebViewImpl::ReloadWithoutCache()
{
  return mWebEngine ? mWebEngine.ReloadWithoutCache() : false;
}

void WebViewImpl::StopLoading()
{
  if(mWebEngine)
  {
    mWebEngine.StopLoading();
  }
}

void WebViewImpl::Suspend()
{
  if(mWebEngine)
  {
    mWebEngine.Suspend();
  }
}

void WebViewImpl::Resume()
{
  if(mWebEngine)
  {
    mWebEngine.Resume();
  }
}

void WebViewImpl::SuspendNetworkLoading()
{
  if(mWebEngine)
  {
    mWebEngine.SuspendNetworkLoading();
  }
}

void WebViewImpl::ResumeNetworkLoading()
{
  if(mWebEngine)
  {
    mWebEngine.ResumeNetworkLoading();
  }
}

float WebViewImpl::GetLoadProgressPercentage() const
{
  return mWebEngine ? mWebEngine.GetPlugin()->GetLoadProgressPercentage() : 0.0f;
}

// ===========================================================================
// Custom Headers
// ===========================================================================

bool WebViewImpl::AddCustomHeader(const Dali::String& name, const Dali::String& value)
{
  return mWebEngine ? mWebEngine.AddCustomHeader(ToStdString(name), ToStdString(value)) : false;
}

bool WebViewImpl::RemoveCustomHeader(const Dali::String& name)
{
  return mWebEngine ? mWebEngine.RemoveCustomHeader(ToStdString(name)) : false;
}

// ===========================================================================
// Scroll
// ===========================================================================

Dali::Vector2 WebViewImpl::GetScrollPosition() const
{
  if(!mWebEngine)
  {
    return Vector2::ZERO;
  }
  return mWebEngine.GetScrollPosition();
}

Dali::Vector2 WebViewImpl::GetScrollSize() const
{
  if(!mWebEngine)
  {
    return Vector2::ZERO;
  }
  return mWebEngine.GetScrollSize();
}

Dali::Vector2 WebViewImpl::GetContentSize() const
{
  if(!mWebEngine)
  {
    return Vector2::ZERO;
  }
  return mWebEngine.GetContentSize();
}

void WebViewImpl::ScrollBy(int32_t deltaX, int32_t deltaY)
{
  if(mWebEngine)
  {
    mWebEngine.ScrollBy(deltaX, deltaY);
  }
}

bool WebViewImpl::ScrollEdgeBy(int32_t deltaX, int32_t deltaY)
{
  return mWebEngine ? mWebEngine.ScrollEdgeBy(deltaX, deltaY) : false;
}

// ===========================================================================
// Navigation
// ===========================================================================

bool WebViewImpl::CanGoForward()
{
  return mWebEngine && mWebEngine.CanGoForward();
}

void WebViewImpl::GoForward()
{
  if(mWebEngine)
  {
    mWebEngine.GoForward();
  }
}

bool WebViewImpl::CanGoBack()
{
  return mWebEngine && mWebEngine.CanGoBack();
}

void WebViewImpl::GoBack()
{
  if(mWebEngine)
  {
    mWebEngine.GoBack();
  }
}

void WebViewImpl::ClearHistory()
{
  if(mWebEngine)
  {
    mWebEngine.ClearHistory();
  }
}

void WebViewImpl::ClearAllTilesResources()
{
  if(mWebEngine)
  {
    mWebEngine.ClearAllTilesResources();
  }
}

void WebViewImpl::ExitFullscreen()
{
  if(mWebEngine)
  {
    mWebEngine.ExitFullscreen();
  }
}

// ===========================================================================
// JavaScript
// ===========================================================================

void WebViewImpl::EvaluateJavaScript(const Dali::String& script)
{
  if(mWebEngine)
  {
    mWebEngine.EvaluateJavaScript(ToStdString(script), nullptr);
  }
}

void WebViewImpl::EvaluateJavaScript(const Dali::String& script, WebView::JavaScriptCallback callback)
{
  if(!mWebEngine)
  {
    return;
  }
  auto sharedCb = std::shared_ptr<CallbackBase>(callback.Release());
  mWebEngine.EvaluateJavaScript(ToStdString(script),
                                [sharedCb](const std::string& result)
  {
    Dali::String daliResult(result.c_str());
    CallbackBase::Execute<Dali::String>(*sharedCb, daliResult);
  });
}

void WebViewImpl::AddJavaScriptMessageHandler(const Dali::String& exposedObjectName, WebView::JavaScriptCallback callback)
{
  if(!mWebEngine)
  {
    return;
  }
  auto sharedCb = std::shared_ptr<CallbackBase>(callback.Release());
  mWebEngine.AddJavaScriptMessageHandler(ToStdString(exposedObjectName),
                                         [sharedCb](const std::string& message)
  {
    Dali::String daliMsg(message.c_str());
    CallbackBase::Execute<Dali::String>(*sharedCb, daliMsg);
  });
}

void WebViewImpl::RegisterJavaScriptAlertCallback(WebView::JavaScriptAlertCallback callback)
{
  if(!mWebEngine)
  {
    return;
  }
  auto sharedCb            = std::shared_ptr<CallbackBase>(callback.Release());
  mJavaScriptAlertCallback = [sharedCb](const std::string& msg) -> bool
  {
    Dali::String daliMsg(msg.c_str());
    return CallbackBase::ExecuteReturn<bool, Dali::String>(*sharedCb, daliMsg);
  };
  mWebEngine.RegisterJavaScriptAlertCallback(mJavaScriptAlertCallback);
}

void WebViewImpl::JavaScriptAlertReply()
{
  if(mWebEngine)
  {
    mWebEngine.JavaScriptAlertReply();
  }
}

void WebViewImpl::RegisterJavaScriptConfirmCallback(WebView::JavaScriptConfirmCallback callback)
{
  if(!mWebEngine)
  {
    return;
  }
  auto sharedCb              = std::shared_ptr<CallbackBase>(callback.Release());
  mJavaScriptConfirmCallback = [sharedCb](const std::string& msg) -> bool
  {
    Dali::String daliMsg(msg.c_str());
    return CallbackBase::ExecuteReturn<bool, Dali::String>(*sharedCb, daliMsg);
  };
  mWebEngine.RegisterJavaScriptConfirmCallback(mJavaScriptConfirmCallback);
}

void WebViewImpl::JavaScriptConfirmReply(bool confirmed)
{
  if(mWebEngine)
  {
    mWebEngine.JavaScriptConfirmReply(confirmed);
  }
}

void WebViewImpl::RegisterJavaScriptPromptCallback(WebView::JavaScriptPromptCallback callback)
{
  if(!mWebEngine)
  {
    return;
  }
  auto sharedCb             = std::shared_ptr<CallbackBase>(callback.Release());
  mJavaScriptPromptCallback = [sharedCb](const std::string& msg, const std::string& defVal) -> bool
  {
    Dali::String daliMsg(msg.c_str());
    Dali::String daliDef(defVal.c_str());
    return CallbackBase::ExecuteReturn<bool, Dali::String, Dali::String>(*sharedCb, daliMsg, daliDef);
  };
  mWebEngine.RegisterJavaScriptPromptCallback(mJavaScriptPromptCallback);
}

void WebViewImpl::JavaScriptPromptReply(const Dali::String& result)
{
  if(mWebEngine)
  {
    mWebEngine.JavaScriptPromptReply(ToStdString(result));
  }
}

// ===========================================================================
// Scale & Zoom
// ===========================================================================

void WebViewImpl::SetScaleFactor(float scaleFactor, Dali::Vector2 point)
{
  if(mWebEngine)
  {
    mWebEngine.SetScaleFactor(scaleFactor, point);
  }
}

float WebViewImpl::GetScaleFactor() const
{
  return mWebEngine ? mWebEngine.GetScaleFactor() : 1.0f;
}

float WebViewImpl::GetPageZoomFactor() const
{
  return mWebEngine ? mWebEngine.GetPlugin()->GetPageZoomFactor() : 1.0f;
}

void WebViewImpl::SetPageZoomFactor(float zoomFactor)
{
  if(mWebEngine)
  {
    mWebEngine.GetPlugin()->SetPageZoomFactor(zoomFactor);
  }
}

float WebViewImpl::GetTextZoomFactor() const
{
  return mWebEngine ? mWebEngine.GetPlugin()->GetTextZoomFactor() : 1.0f;
}

void WebViewImpl::SetTextZoomFactor(float zoomFactor)
{
  if(mWebEngine)
  {
    mWebEngine.GetPlugin()->SetTextZoomFactor(zoomFactor);
  }
}

// ===========================================================================
// Screenshot
// ===========================================================================

Dali::Ui::ImageView WebViewImpl::GetScreenshot(Dali::Rect<int32_t> viewArea, float scaleFactor)
{
  if(!mWebEngine)
  {
    return {};
  }
  Dali::PixelData pixelData = mWebEngine.GetScreenshot(viewArea, scaleFactor);
  return CreateImageViewFromPixelData(pixelData);
}

bool WebViewImpl::GetScreenshotAsynchronously(Dali::Rect<int32_t> viewArea, float scaleFactor, WebView::ScreenshotCapturedCallback callback)
{
  if(!mWebEngine)
  {
    return false;
  }
  auto sharedCb              = std::shared_ptr<CallbackBase>(callback.Release());
  mPendingScreenshotCallback = [sharedCb](Dali::Ui::ImageView imageView)
  {
    CallbackBase::Execute<Dali::Ui::ImageView>(*sharedCb, imageView);
  };
  return mWebEngine.GetScreenshotAsynchronously(viewArea, scaleFactor,
                                                [this](Dali::PixelData pixelData)
  {
    if(mPendingScreenshotCallback)
    {
      mPendingScreenshotCallback(CreateImageViewFromPixelData(pixelData));
      mPendingScreenshotCallback = nullptr;
    }
  });
}

// ===========================================================================
// Page Info
// ===========================================================================

Dali::String WebViewImpl::GetTitle() const
{
  return mWebEngine ? ToDaliString(mWebEngine.GetTitle()) : Dali::String();
}

Dali::Ui::ImageView WebViewImpl::GetFavicon() const
{
  if(!mWebEngine)
  {
    return {};
  }
  return CreateImageViewFromPixelData(mWebEngine.GetFavicon());
}

Dali::String WebViewImpl::GetSelectedText() const
{
  return mWebEngine ? ToDaliString(mWebEngine.GetPlugin()->GetSelectedText()) : Dali::String();
}

void WebViewImpl::GetPlainTextAsynchronously(WebView::PlainTextCallback callback)
{
  if(!mWebEngine)
  {
    return;
  }
  auto sharedCb = std::shared_ptr<CallbackBase>(callback.Release());
  mWebEngine.GetPlainTextAsynchronously(
    [sharedCb](const std::string& text)
  {
    Dali::String daliText(text.c_str());
    CallbackBase::Execute<Dali::String>(*sharedCb, daliText);
  });
}

// ===========================================================================
// Document Appearance
// ===========================================================================

void WebViewImpl::SetDocumentBackgroundColor(const Dali::Vector4& color)
{
  if(mWebEngine)
  {
    mWebEngine.GetPlugin()->SetDocumentBackgroundColor(color);
  }
}

void WebViewImpl::SetTilesClearedWhenHidden(bool cleared)
{
  if(mWebEngine)
  {
    mWebEngine.ClearTilesWhenHidden(cleared);
  }
}

void WebViewImpl::SetTileCoverAreaMultiplier(float multiplier)
{
  if(mWebEngine)
  {
    mWebEngine.GetPlugin()->SetTileCoverAreaMultiplier(multiplier);
  }
}

void WebViewImpl::SetCursorEnabledByClient(bool enabled)
{
  if(mWebEngine)
  {
    mWebEngine.EnableCursorByClient(enabled);
  }
}

// ===========================================================================
// Input & Video
// ===========================================================================

void WebViewImpl::SetMouseEventsEnabled(bool enabled)
{
  mMouseEventsEnabled = enabled;
  if(mWebEngine)
  {
    mWebEngine.EnableMouseEvents(enabled);
  }
}

bool WebViewImpl::GetMouseEventsEnabled() const
{
  return mMouseEventsEnabled;
}

void WebViewImpl::SetKeyEventsEnabled(bool enabled)
{
  mKeyEventsEnabled = enabled;
  if(mWebEngine)
  {
    mWebEngine.EnableKeyEvents(enabled);
  }
}

bool WebViewImpl::GetKeyEventsEnabled() const
{
  return mKeyEventsEnabled;
}

void WebViewImpl::FeedMouseWheel(bool yDirection, int step, int x, int y)
{
  if(mWebEngine)
  {
    mWebEngine.FeedMouseWheel(yDirection, step, x, y);
  }
}

void WebViewImpl::SetVideoHole(bool enabled, bool isWaylandWindow)
{
  mVideoHoleEnabled = enabled;
  EnableBlendMode(!mVideoHoleEnabled);
  if(mWebEngine)
  {
    mWebEngine.SetVideoHole(mVideoHoleEnabled, isWaylandWindow);
  }
}

void WebViewImpl::SetVideoHoleEnabled(bool enabled)
{
  SetVideoHole(enabled, /*isWaylandWindow=*/true);
}

bool WebViewImpl::GetVideoHoleEnabled() const
{
  return mVideoHoleEnabled;
}

bool WebViewImpl::CheckVideoPlayingAsynchronously(WebView::VideoPlayingCallback callback)
{
  if(!mWebEngine)
  {
    return false;
  }
  auto sharedCb = std::shared_ptr<CallbackBase>(callback.Release());
  return mWebEngine.CheckVideoPlayingAsynchronously(
    [sharedCb](bool isPlaying)
  {
    CallbackBase::Execute<bool>(*sharedCb, isPlaying);
  });
}

// ===========================================================================
// Callback Registration — Page Loading
// ===========================================================================

void WebViewImpl::RegisterPageLoadStartedCallback(PageLoadCallback callback)
{
  if(!mWebEngine) return;
  auto sharedCb            = std::shared_ptr<CallbackBase>(callback.Release());
  mPageLoadStartedCallback = [sharedCb](const std::string& url)
  {
    Dali::String daliUrl(url.c_str());
    CallbackBase::Execute<Dali::String>(*sharedCb, daliUrl);
  };
  mWebEngine.RegisterPageLoadStartedCallback(mPageLoadStartedCallback);
}

void WebViewImpl::RegisterPageLoadInProgressCallback(PageLoadCallback callback)
{
  if(!mWebEngine) return;
  auto sharedCb               = std::shared_ptr<CallbackBase>(callback.Release());
  mPageLoadInProgressCallback = [sharedCb](const std::string& url)
  {
    Dali::String daliUrl(url.c_str());
    CallbackBase::Execute<Dali::String>(*sharedCb, daliUrl);
  };
  mWebEngine.RegisterPageLoadInProgressCallback(mPageLoadInProgressCallback);
}

void WebViewImpl::RegisterPageLoadFinishedCallback(PageLoadCallback callback)
{
  if(!mWebEngine) return;
  auto sharedCb             = std::shared_ptr<CallbackBase>(callback.Release());
  mPageLoadFinishedCallback = [sharedCb](const std::string& url)
  {
    Dali::String daliUrl(url.c_str());
    CallbackBase::Execute<Dali::String>(*sharedCb, daliUrl);
  };
  mWebEngine.RegisterPageLoadFinishedCallback(mPageLoadFinishedCallback);
}

// ===========================================================================
// Callback Registration — Scroll
// ===========================================================================

void WebViewImpl::RegisterScrollEdgeReachedCallback(ScrollEdgeReachedCallback callback)
{
  if(!mWebEngine) return;
  auto sharedCb              = std::shared_ptr<CallbackBase>(callback.Release());
  mScrollEdgeReachedCallback = [sharedCb](const Dali::WebEnginePlugin::ScrollEdge edge)
  {
    WebViewScrollEdge uiEdge = ToUiScrollEdge(edge);
    CallbackBase::Execute<WebViewScrollEdge>(*sharedCb, uiEdge);
  };
  mWebEngine.RegisterScrollEdgeReachedCallback(mScrollEdgeReachedCallback);
}

void WebViewImpl::RegisterOverScrolledCallback(OverScrolledCallback callback)
{
  if(!mWebEngine) return;
  auto sharedCb         = std::shared_ptr<CallbackBase>(callback.Release());
  mOverScrolledCallback = [sharedCb](const Dali::WebEnginePlugin::OverScrolled overScrolled)
  {
    WebViewOverScrolled uiOs = ToUiOverScrolled(overScrolled);
    CallbackBase::Execute<WebViewOverScrolled>(*sharedCb, uiOs);
  };
  mWebEngine.RegisterOverScrolledCallback(mOverScrolledCallback);
}

// ===========================================================================
// Callback Registration — Navigation
// ===========================================================================

void WebViewImpl::RegisterUrlChangedCallback(UrlChangedCallback callback)
{
  if(!mWebEngine) return;
  auto sharedCb       = std::shared_ptr<CallbackBase>(callback.Release());
  mUrlChangedCallback = [sharedCb](const std::string& url)
  {
    Dali::String daliUrl(url.c_str());
    CallbackBase::Execute<Dali::String>(*sharedCb, daliUrl);
  };
  mWebEngine.RegisterUrlChangedCallback(mUrlChangedCallback);
}

// ===========================================================================
// Callback Registration — Rendering
// ===========================================================================

void WebViewImpl::RegisterFrameRenderedCallback(FrameRenderedCallback callback)
{
  // Store alongside our internal frame-rendered logic.
  // We wrap the user callback and call it from OnFrameRendered().
  auto sharedCb          = std::shared_ptr<CallbackBase>(callback.Release());
  mFrameRenderedCallback = [sharedCb]()
  {
    CallbackBase::Execute(*sharedCb);
  };
  // Note: the web engine's frame-rendered callback is already registered in OnInitialize()
  // pointing to our OnFrameRendered() method, which internally calls mFrameRenderedCallback.
}

// ===========================================================================
// Callback Registration — Fullscreen
// ===========================================================================

void WebViewImpl::RegisterFullscreenEnteredCallback(FullscreenCallback callback)
{
  if(!mWebEngine) return;
  auto sharedCb              = std::shared_ptr<CallbackBase>(callback.Release());
  mFullscreenEnteredCallback = [sharedCb]()
  {
    CallbackBase::Execute(*sharedCb);
  };
  mWebEngine.RegisterFullscreenEnteredCallback(mFullscreenEnteredCallback);
}

void WebViewImpl::RegisterFullscreenExitedCallback(FullscreenCallback callback)
{
  if(!mWebEngine) return;
  auto sharedCb             = std::shared_ptr<CallbackBase>(callback.Release());
  mFullscreenExitedCallback = [sharedCb]()
  {
    CallbackBase::Execute(*sharedCb);
  };
  mWebEngine.RegisterFullscreenExitedCallback(mFullscreenExitedCallback);
}

// ===========================================================================
// Callback Registration — Text Search & Geolocation
// ===========================================================================

void WebViewImpl::RegisterTextFoundCallback(TextFoundCallback callback)
{
  if(!mWebEngine) return;
  auto sharedCb      = std::shared_ptr<CallbackBase>(callback.Release());
  mTextFoundCallback = [sharedCb](uint32_t count)
  {
    CallbackBase::Execute<uint32_t>(*sharedCb, count);
  };
  mWebEngine.RegisterTextFoundCallback(mTextFoundCallback);
}

void WebViewImpl::RegisterGeolocationPermissionCallback(GeolocationPermissionCallback callback)
{
  if(!mWebEngine) return;
  auto sharedCb                  = std::shared_ptr<CallbackBase>(callback.Release());
  mGeolocationPermissionCallback = [sharedCb](const std::string& host, const std::string& protocol) -> bool
  {
    Dali::String daliHost(host.c_str());
    Dali::String daliProtocol(protocol.c_str());
    return CallbackBase::ExecuteReturn<bool, Dali::String, Dali::String>(*sharedCb, daliHost, daliProtocol);
  };
  mWebEngine.RegisterGeolocationPermissionCallback(mGeolocationPermissionCallback);
}

// ===========================================================================
// Callback Registration — Process Events
// ===========================================================================

void WebViewImpl::RegisterWebProcessCrashedCallback(WebProcessCrashedCallback callback)
{
  if(!mWebEngine) return;
  auto sharedCb              = std::shared_ptr<CallbackBase>(callback.Release());
  mWebProcessCrashedCallback = [sharedCb]()
  {
    CallbackBase::Execute(*sharedCb);
  };
  mWebEngine.GetPlugin()->RegisterWebProcessCrashedCallback(mWebProcessCrashedCallback);
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
