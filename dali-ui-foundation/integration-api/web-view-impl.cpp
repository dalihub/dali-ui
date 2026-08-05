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
#include <dali/devel-api/object/property-map-devel.h>
#include <dali/devel-api/object/property-value-devel.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/adaptor-framework/native-image.h>
#include <dali/public-api/object/property-conditions.h>
#include <algorithm>
#include <functional>
#include <memory>
#include <mutex>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/integration-api/visuals/visual-actions-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/web-profile-impl.h>
#include <dali-ui-foundation/integration-api/web-settings-impl.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-foundation/public-api/image-loader/image-url-utils.h>
#include <dali-ui-foundation/public-api/image-loader/image-url.h>
#include <dali-ui-foundation/public-api/visuals/color-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/image-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

#if defined(_MSC_VER)
#include <dali/integration-api/scene.h>
#include <dali/public-api/events/point-state.h>

#include <dali-ui-foundation/internal/focus-manager/keyinput-focus-manager.h>
#include <dali-ui-foundation/public-api/views/view.h>

// TEMP DEBUG (Windows LWE WebView bring-up): file logging kept while an
// arrow-key input issue is under investigation. Remove once resolved.
#include <cstdarg>
#include <cstdio>
static void WriteWebViewDebugLog(const char* format, ...)
{
  FILE* outputFile = nullptr;
  if(fopen_s(&outputFile, "d:\\lwe_webview.log", "a") != 0 || outputFile == nullptr)
  {
    return;
  }

  va_list arguments;
  va_start(arguments, format);
  vfprintf(outputFile, format, arguments);
  va_end(arguments);
  fputc('\n', outputFile);
  fclose(outputFile);
}
#endif

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
  map.Insert(Dali::Ui::VisualBasePropertyIndex::TYPE, Dali::Ui::Integration::InternalVisualType::COLOR);
  map.Insert(Dali::Ui::VisualBasePropertyIndex::MIX_COLOR, Color::TRANSPARENT);
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
BoundsInteger CalculateDisplayArea(Dali::Actor self)
{
  bool    positionUsesAnchorPoint = self.GetProperty<bool>(Actor::Property::POSITION_USES_PIVOT);
  Vector3 actorSize               = self.GetCurrentProperty<Vector3>(Actor::Property::SIZE) * self.GetCurrentProperty<Vector3>(Actor::Property::SCALE);
  Vector3 anchorOff               = actorSize * (positionUsesAnchorPoint ? self.GetCurrentProperty<Vector3>(Actor::Property::PIVOT) : Pivot::TOP_LEFT);
  Vector2 screenPos               = self.GetProperty<Vector2>(Actor::Property::SCREEN_POSITION);

  Dali::BoundsInteger area;
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
  mWebViewSize(Vector2(1.0f, 1.0f)),
  mWebViewArea(0, 0, static_cast<int32_t>(mWebViewSize.width), static_cast<int32_t>(mWebViewSize.height)),
  mLastRenderedNativeImageWidth(0u),
  mLastRenderedNativeImageHeight(0u),
  mVisualChangeRequired(false),
  mVideoHoleEnabled(false),
  mMouseEventsEnabled(true),
  mKeyEventsEnabled(true)
{
  SetArrangePolicy(ArrangePolicy::ALWAYS);
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
  DALI_ASSERT_ALWAYS(UiConfig::HasCurrent() && "UiConfig::Apply() must be called before WebView::New()");

  const WebEngineType webEngineType = UiConfig::GetCurrent().GetWebEngineType();
  const char* const   webEngineName = webEngineType == WebEngineType::LWE ? "LWE" : "Chromium";

  static std::once_flag webEngineLogFlag;
  std::call_once(webEngineLogFlag, [webEngineName]()
  {
    DALI_LOG_RELEASE_INFO("[WebView] Using %s web engine as configured in the applied UiConfig.\n", webEngineName);
  });

  auto* impl       = new WebViewImpl();
  impl->mWebEngine = Dali::WebEngine::New(static_cast<int32_t>(webEngineType));
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
  self.SetProperty(Actor::Property::FOCUSABLE, true);
  self.SetProperty(Actor::Property::FOCUS_ON_TOUCH, true);

  // Key events are now handled via OnKeyEvent() virtual override.
  // Touch events are handled via signal connection.
  self.TouchEventSignal().Connect(this, &WebViewImpl::OnTouchEvent);

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
    Ui::Integration::Visual::Base placeholderVisual = Ui::Integration::VisualFactory::Get().CreateVisual(TRANSPARENT_COLOR_VISUAL_PROPERTIES);
    if(placeholderVisual)
    {
      viewData.RegisterVisual(WebViewImpl::Property::URL, placeholderVisual);
    }
  }

  // --- Bridge web-engine callbacks to public signals ---
  // The dali-adaptor WebEngine is callback-based (it exposes no signals of its own), so each
  // engine event is forwarded here to the matching WebView signal through an Emit* helper.
  if(mWebEngine)
  {
    mWebEngine.RegisterFrameRenderedCallback(std::bind(&WebViewImpl::OnFrameRendered, this));

    mWebEngine.RegisterPageLoadStartedCallback([this](const std::string& url)
    { EmitPageLoadStarted(ToDaliString(url)); });
    mWebEngine.RegisterPageLoadInProgressCallback([this](const std::string& url)
    { EmitPageLoadInProgress(ToDaliString(url)); });
    mWebEngine.RegisterPageLoadFinishedCallback([this](const std::string& url)
    { EmitPageLoadFinished(ToDaliString(url)); });
    mWebEngine.RegisterPageLoadErrorCallback([this](std::unique_ptr<Dali::WebEngineLoadError> error)
    {
      if(!error)
      {
        return;
      }
      WebViewPageLoadError uiError;
      uiError.url         = ToDaliString(error->GetUrl());
      uiError.code        = ToUiPageLoadErrorCode(error->GetCode());
      uiError.description = ToDaliString(error->GetDescription());
      uiError.type        = ToUiPageLoadErrorType(error->GetType());
      EmitPageLoadError(uiError);
    });
    mWebEngine.RegisterUrlChangedCallback([this](const std::string& url)
    { EmitUrlChanged(ToDaliString(url)); });
    mWebEngine.RegisterScrollEdgeReachedCallback([this](Dali::WebEnginePlugin::ScrollEdge edge)
    { EmitScrollEdgeReached(ToUiScrollEdge(edge)); });
    mWebEngine.RegisterOverScrolledCallback([this](Dali::WebEnginePlugin::OverScrolled overScrolled)
    { EmitOverScrolled(ToUiOverScrolled(overScrolled)); });
    mWebEngine.RegisterFullscreenEnteredCallback([this]()
    { EmitFullscreenEntered(); });
    mWebEngine.RegisterFullscreenExitedCallback([this]()
    { EmitFullscreenExited(); });
    mWebEngine.RegisterTextFoundCallback([this](uint32_t count)
    { EmitTextFound(count); });
    mWebEngine.RegisterGeolocationPermissionCallback([this](const std::string& host, const std::string& protocol) -> bool
    { return EmitGeolocationPermission(ToDaliString(host), ToDaliString(protocol)); });
    mWebEngine.RegisterWebProcessCrashedCallback([this]()
    { EmitWebProcessCrashed(); });
  }
}

MeasuredSize WebViewImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  // widthConstraint/heightConstraint are visual sizes.
  // WebView fills whatever size the parent provides.
  // If constraints are unconstrained (INFINITY), fall back to mWebViewSize which is always in
  // visual units: initialized to stage size (visual at scale 1.0) and updated to the actor's
  // visual SIZE via SetDisplayArea after each OnArrange. No further scaling is needed.
  float w = std::isinf(widthConstraint) ? mWebViewSize.width : widthConstraint;
  float h = std::isinf(heightConstraint) ? mWebViewSize.height : heightConstraint;

  DALI_LOG_DEBUG_INFO("[WebViewImpl] OnMeasure: constraint=(%.0f,%.0f) -> (%.0f,%.0f)\n",
                      widthConstraint, heightConstraint, w, h);
  return MeasuredSize(w, h);
}

// ArrangePolicy::ALWAYS is required here and is set in the constructor. CalculateDisplayArea() reads
// Actor::Property::SCREEN_POSITION, a function of the whole ancestor chain, and
// SetDisplayArea() pushes it to a surface outside the actor tree
// (mWebEngine.UpdateDisplayArea). Neither input is in the arrange cache key, and an
// ancestor move invalidates nothing here, so serving this view a cached arrange would
// strand the web engine's surface at a stale offset. The WORLD_POSITION
// StepCondition(1.0f, 1.0f) notification is a coarse backstop, not an equivalent: it
// cannot see sub-pixel-per-frame drift.
// Pinned by UtcDaliArrangeCacheHitAlwaysFirstPartyLeavesNeverCacheP.
LayoutRect WebViewImpl::OnArrange(const LayoutRect& bounds)
{
  DALI_LOG_DEBUG_INFO("[WebViewImpl] OnArrange: bounds=(x=%.0f,y=%.0f,w=%.0f,h=%.0f)\n",
                      bounds.x, bounds.y, bounds.width, bounds.height);

  LayoutRect result = ViewImpl::OnArrange(bounds);

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
  // Notify listeners that the web engine produced a new frame.
  EmitFrameRendered();

#if defined(_MSC_VER)
  WriteWebViewDebugLog("[WV] OnFrameRendered visualChangeReq=%d hasVisual=%d", static_cast<int>(mVisualChangeRequired), static_cast<int>(static_cast<bool>(mVisual)));

  // The web engine delivers frames asynchronously (via EventThreadCallback) rather than in
  // response to a scene change, so the DALi scene is not marked dirty and, under on-demand
  // rendering, no update/render cycle is scheduled - the freshly uploaded native-image texture
  // is never drawn until some unrelated event (e.g. a mouse click) wakes the pipeline. Request
  // a render for each incoming frame so web content appears (and keeps updating) on its own.
  Dali::Integration::Scene scene = Dali::Integration::Scene::Get(Self());
  if(scene)
  {
    scene.KeepRendering(0.0f);
  }
#endif

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
  imageVisualMap.Insert(Dali::Ui::VisualBasePropertyIndex::TYPE, Dali::Ui::Integration::InternalVisualType::IMAGE);
  imageVisualMap.Insert(Dali::Ui::ImageVisualPropertyIndex::URL, nativeImageUrl.GetUrl());
  imageVisualMap.Insert(Dali::Ui::ImageVisualPropertyIndex::PIXEL_AREA, FULL_TEXTURE_RECT);
  imageVisualMap.Insert(Dali::Ui::ImageVisualPropertyIndex::WRAP_MODE_U, static_cast<int>(WrapMode::CLAMP_TO_EDGE));
  imageVisualMap.Insert(Dali::Ui::ImageVisualPropertyIndex::WRAP_MODE_V, static_cast<int>(WrapMode::CLAMP_TO_EDGE));

  mVisual = Ui::Integration::VisualFactory::Get().CreateVisual(imageVisualMap);
#if defined(_MSC_VER)
  {
    Vector3 selfSize = Self().GetCurrentProperty<Vector3>(Dali::Actor::Property::SIZE);
    WriteWebViewDebugLog("[WV] created visual=%d nativeImg=%ux%u selfSize=%.0fx%.0f",
          (int)(bool)mVisual, mLastRenderedNativeImageWidth, mLastRenderedNativeImageHeight,
          selfSize.width, selfSize.height);
  }
#endif
  if(mVisual)
  {
    auto& viewData = Internal::ViewDataImpl::Get(*this);
    viewData.RegisterVisual(WebViewImpl::Property::URL, mVisual, Dali::Ui::Integration::DepthIndex::CONTENT);
    EnableBlendMode(!mVideoHoleEnabled);
  }
  else
  {
    DALI_LOG_ERROR("[WebViewImpl] OnFrameRendered: failed to create image visual\n");
  }
}

void WebViewImpl::OnDisplayAreaUpdated(Dali::PropertyNotification /*source*/)
{
  if(!mWebEngine)
  {
    return;
  }
  auto displayArea = CalculateDisplayArea(Self());
  SetDisplayArea(displayArea);
}

void WebViewImpl::SetDisplayArea(const Dali::BoundsInteger& displayArea)
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
      updateMap.Insert(Dali::Ui::ImageVisualPropertyIndex::PIXEL_AREA, pixelArea);
      updateMap.Insert(Dali::Ui::VisualBasePropertyIndex::TRANSFORM,
                       Dali::CreatePropertyMap({{Dali::Ui::Visual::Transform::Property::SIZE, transformSize}}));

      auto& viewData = Internal::ViewDataImpl::Get(*this);
      viewData.DoAction(WebViewImpl::Property::URL, Dali::Ui::Integration::Visual::Action::UPDATE_PROPERTY, updateMap);
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

WebViewPageLoadErrorCode WebViewImpl::ToUiPageLoadErrorCode(Dali::WebEngineLoadError::ErrorCode code)
{
  // WebViewPageLoadErrorCode mirrors WebEngineLoadError::ErrorCode value-for-value.
  return static_cast<WebViewPageLoadErrorCode>(static_cast<int>(code));
}

WebViewPageLoadErrorType WebViewImpl::ToUiPageLoadErrorType(Dali::WebEngineLoadError::ErrorType type)
{
  // WebViewPageLoadErrorType mirrors WebEngineLoadError::ErrorType value-for-value.
  return static_cast<WebViewPageLoadErrorType>(static_cast<int>(type));
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
  // The web engine reports progress as a 0.0-1.0 ratio; the public API contract is a
  // 0.0-100.0 percentage, so scale it here.
  return (mWebEngine && mWebEngine.GetPlugin()) ? mWebEngine.GetPlugin()->GetLoadProgressPercentage() * 100.0f : 0.0f;
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

bool WebViewImpl::CanGoForward() const
{
  // WebEngine::CanGoForward() is not declared const in dali-adaptor, but the query does not
  // modify observable state, so the const_cast keeps this public query const-correct.
  return mWebEngine && const_cast<Dali::WebEngine&>(mWebEngine).CanGoForward();
}

void WebViewImpl::GoForward()
{
  if(mWebEngine)
  {
    mWebEngine.GoForward();
  }
}

bool WebViewImpl::CanGoBack() const
{
  // WebEngine::CanGoBack() is not declared const in dali-adaptor, but the query does not
  // modify observable state, so the const_cast keeps this public query const-correct.
  return mWebEngine && const_cast<Dali::WebEngine&>(mWebEngine).CanGoBack();
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

void WebViewImpl::ClearCache()
{
  GetProfile().ClearCache();
}

void WebViewImpl::ClearCookies()
{
  GetProfile().GetCookieManager().ClearAllCookies();
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

void WebViewImpl::RemoveJavaScriptMessageHandler(const Dali::String& exposedObjectName)
{
  if(mWebEngine)
  {
    mWebEngine.RemoveJavaScriptMessageHandler(ToStdString(exposedObjectName));
  }
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

WebProfile WebViewImpl::GetProfile() const
{
  if(!mWebProfile)
  {
    const bool        isIncognito = mWebEngine && mWebEngine.GetPlugin() && mWebEngine.IsIncognito();
    WebProfileImplPtr impl        = WebProfileImpl::New(Dali::WebEngine::GetContext(isIncognito), Dali::WebEngine::GetCookieManager(isIncognito));
    mWebProfile                   = WebProfile(*impl);
  }
  return mWebProfile;
}

WebSettings WebViewImpl::GetSettings() const
{
  if(!mWebSettings)
  {
    WebSettingsImplPtr impl = WebSettingsImpl::New(mWebEngine);
    mWebSettings            = WebSettings(*impl);
  }
  return mWebSettings;
}

float WebViewImpl::GetPageZoomFactor() const
{
  return (mWebEngine && mWebEngine.GetPlugin()) ? mWebEngine.GetPlugin()->GetPageZoomFactor() : 1.0f;
}

void WebViewImpl::SetPageZoomFactor(float zoomFactor)
{
  if(mWebEngine && mWebEngine.GetPlugin())
  {
    mWebEngine.GetPlugin()->SetPageZoomFactor(zoomFactor);
  }
}

float WebViewImpl::GetTextZoomFactor() const
{
  return (mWebEngine && mWebEngine.GetPlugin()) ? mWebEngine.GetPlugin()->GetTextZoomFactor() : 1.0f;
}

void WebViewImpl::SetTextZoomFactor(float zoomFactor)
{
  if(mWebEngine && mWebEngine.GetPlugin())
  {
    mWebEngine.GetPlugin()->SetTextZoomFactor(zoomFactor);
  }
}

// ===========================================================================
// Screenshot
// ===========================================================================

Dali::Ui::ImageView WebViewImpl::GetScreenshot(Dali::BoundsInteger viewArea, float scaleFactor)
{
  if(!mWebEngine)
  {
    return {};
  }
  Dali::PixelData pixelData = mWebEngine.GetScreenshot(viewArea, scaleFactor);
  return CreateImageViewFromPixelData(pixelData);
}

bool WebViewImpl::GetScreenshotAsynchronously(Dali::BoundsInteger viewArea, float scaleFactor, WebView::ScreenshotCapturedCallback callback)
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
  return (mWebEngine && mWebEngine.GetPlugin()) ? ToDaliString(mWebEngine.GetPlugin()->GetSelectedText()) : Dali::String();
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

bool WebViewImpl::FindText(const Dali::String& text, WebViewFindOption options, uint32_t maxMatchCount)
{
  // dali-adaptor exposes find-and-highlight as HighlightText(); the match count arrives via the
  // text-found callback wired in OnInitialize(), which emits TextFoundSignal.
  return mWebEngine && mWebEngine.HighlightText(ToStdString(text), ToEngineFindOption(options), maxMatchCount);
}

// ===========================================================================
// Document Appearance
// ===========================================================================

void WebViewImpl::SetDocumentBackgroundColor(const Dali::Vector4& color)
{
  if(mWebEngine && mWebEngine.GetPlugin())
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
  if(mWebEngine && mWebEngine.GetPlugin())
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

bool WebViewImpl::IsMouseEventsEnabled() const
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

bool WebViewImpl::IsKeyEventsEnabled() const
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

bool WebViewImpl::FeedKeyEvent(const KeyEvent& keyEvent)
{
  if(mWebEngine)
  {
    return mWebEngine.SendKeyEvent(keyEvent);
  }
  return false;
}

bool WebViewImpl::FeedTouchEvent(const TouchEvent& touchEvent)
{
  if(mWebEngine)
  {
    return mWebEngine.SendTouchEvent(touchEvent);
  }
  return false;
}

bool WebViewImpl::OnKeyEvent(const Dali::KeyEvent& event)
{
  if(mKeyEventsEnabled && mWebEngine)
  {
    return mWebEngine.SendKeyEvent(event);
  }
  return false;
}

bool WebViewImpl::OnTouchEvent(Dali::Actor /*actor*/, Dali::TouchEvent touch)
{
  if(mMouseEventsEnabled && mWebEngine)
  {
#if defined(_MSC_VER)
    {
      Vector3 sz = Self().GetCurrentProperty<Vector3>(Dali::Actor::Property::SIZE);
      Vector2 lp = touch.GetPointCount() ? touch.GetLocalPosition(0) : Vector2::ZERO;
      Vector2 sp = touch.GetPointCount() ? touch.GetScreenPosition(0) : Vector2::ZERO;
      WriteWebViewDebugLog("[TOUCH] actorSize=%.0fx%.0f local=(%.0f,%.0f) screen=(%.0f,%.0f) lastImg=%ux%u",
            sz.width, sz.height, lp.x, lp.y, sp.x, sp.y,
            mLastRenderedNativeImageWidth, mLastRenderedNativeImageHeight);
    }
    // Clicking the page must also give it keyboard focus (like a real browser), otherwise
    // OnKeyEvent() never fires for this control and key events never reach the web engine.
    // Route key input focus here on press, mirroring InputEditorImpl's tap-to-focus.
    if(touch.GetPointCount() > 0 && touch.GetState(0) == Dali::PointState::DOWN)
    {
      Ui::View self = Ui::View::DownCast(Self());
      if(self && self.IsConnectedToScene())
      {
        Internal::KeyInputFocusManager::Get().SetFocus(self);
      }
      mWebEngine.SetFocus(true);
    }
#endif
    return mWebEngine.SendTouchEvent(touch);
  }
  return false;
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

bool WebViewImpl::IsVideoHoleEnabled() const
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
// Signal emission helpers
//
// Each helper bridges a web-engine callback to the matching public WebView signal.
// The signal's first argument is the emitting WebView handle, obtained from Self().
// Empty() is checked first so no handle is constructed when nothing is connected.
// ===========================================================================

void WebViewImpl::EmitPageLoadStarted(const Dali::String& url)
{
  if(mPageLoadStartedSignal.Empty()) return;
  mPageLoadStartedSignal.Emit(WebView::DownCast(Self()), url);
}

void WebViewImpl::EmitPageLoadInProgress(const Dali::String& url)
{
  if(mPageLoadInProgressSignal.Empty()) return;
  mPageLoadInProgressSignal.Emit(WebView::DownCast(Self()), url);
}

void WebViewImpl::EmitPageLoadFinished(const Dali::String& url)
{
  if(mPageLoadFinishedSignal.Empty()) return;
  mPageLoadFinishedSignal.Emit(WebView::DownCast(Self()), url);
}

void WebViewImpl::EmitPageLoadError(const WebViewPageLoadError& error)
{
  if(mPageLoadErrorSignal.Empty()) return;
  mPageLoadErrorSignal.Emit(WebView::DownCast(Self()), error);
}

void WebViewImpl::EmitScrollEdgeReached(WebViewScrollEdge edge)
{
  if(mScrollEdgeReachedSignal.Empty()) return;
  mScrollEdgeReachedSignal.Emit(WebView::DownCast(Self()), edge);
}

void WebViewImpl::EmitOverScrolled(WebViewOverScrolled overScrolled)
{
  if(mOverScrolledSignal.Empty()) return;
  mOverScrolledSignal.Emit(WebView::DownCast(Self()), overScrolled);
}

void WebViewImpl::EmitUrlChanged(const Dali::String& url)
{
  if(mUrlChangedSignal.Empty()) return;
  mUrlChangedSignal.Emit(WebView::DownCast(Self()), url);
}

void WebViewImpl::EmitFrameRendered()
{
  if(mFrameRenderedSignal.Empty()) return;
  mFrameRenderedSignal.Emit(WebView::DownCast(Self()));
}

void WebViewImpl::EmitFullscreenEntered()
{
  if(mFullscreenEnteredSignal.Empty()) return;
  mFullscreenEnteredSignal.Emit(WebView::DownCast(Self()));
}

void WebViewImpl::EmitFullscreenExited()
{
  if(mFullscreenExitedSignal.Empty()) return;
  mFullscreenExitedSignal.Emit(WebView::DownCast(Self()));
}

void WebViewImpl::EmitTextFound(uint32_t count)
{
  if(mTextFoundSignal.Empty()) return;
  mTextFoundSignal.Emit(WebView::DownCast(Self()), count);
}

bool WebViewImpl::EmitGeolocationPermission(const Dali::String& host, const Dali::String& protocol)
{
  if(mGeolocationPermissionSignal.Empty()) return false;
  return mGeolocationPermissionSignal.Emit(WebView::DownCast(Self()), host, protocol);
}

void WebViewImpl::EmitWebProcessCrashed()
{
  if(mWebProcessCrashedSignal.Empty()) return;
  mWebProcessCrashedSignal.Emit(WebView::DownCast(Self()));
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
