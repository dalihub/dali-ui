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

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/integration-api/debug.h>

#include <algorithm>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/property-registration-helper.h>
#include <dali-ui-foundation/integration-api/text-visualizer-impl.h>
#include <dali-ui-foundation/integration-api/text-visualizer-property-handler.h>
#include <dali-ui-foundation/integration-api/view-integration.h>
#include <dali-ui-foundation/internal/text/text-visualizer/atlas-renderer-bridge.h>
#include <dali-ui-foundation/internal/text/text-visualizer/atlas-view-adapter.h>
#include <dali-ui-foundation/internal/text/text-visualizer/layout-engine.h>
#include <dali-ui-foundation/internal/text/text-visualizer/text-preparer.h>

#include <cmath>

namespace Dali
{

namespace Ui
{

namespace Integration
{

namespace
{
constexpr bool ENABLE_TEXT_VISUALIZER_RENDER_DIAGNOSTICS = false;

BaseHandle Create()
{
  return BaseHandle();
}

#define TEXT_VISUALIZER_PROPERTY_REGISTRATION(text, valueType, enumIndex) \
  DALI_PROPERTY_REGISTRATION_EXTERNAL(Ui::Text, TextVisualizerPropertyIndex, Ui::Integration, TextVisualizerImpl, text, valueType, enumIndex)

// Type Registration
DALI_TYPE_REGISTRATION_BEGIN(TextVisualizerImpl, ViewImpl, Create)
TEXT_VISUALIZER_PROPERTY_REGISTRATION("text", STRING, TEXT)
TEXT_VISUALIZER_PROPERTY_REGISTRATION("fontFamily", STRING, FONT_FAMILY)
TEXT_VISUALIZER_PROPERTY_REGISTRATION("fontSize", FLOAT, FONT_SIZE)
TEXT_VISUALIZER_PROPERTY_REGISTRATION("textColor", VECTOR4, TEXT_COLOR)
TEXT_VISUALIZER_PROPERTY_REGISTRATION("lineHeight", FLOAT, LINE_HEIGHT)
DALI_TYPE_REGISTRATION_END()

} // namespace

TextVisualizerImplPtr TextVisualizerImpl::New()
{
  return TextVisualizerImplPtr(new TextVisualizerImpl());
}

TextVisualizerImpl::TextVisualizerImpl()
: ViewImpl(),
  mText(),
  mFontFamily(),
  mFontSize(0.0f),
  mLineHeight(Text::LINE_HEIGHT_AUTO),
  mTextColor(Color::BLACK),
  mExclusionRegions(),
  mExclusionLayoutCache(),
  mPreparedText(),
  mLayoutResult(),
  mMeasuredLayoutCache(),
  mAtlasViewAdapter(),
  mAtlasRendererBridge(),
  mRenderHost(),
  mLastLayoutSize(Vector2::ZERO),
  mMeasuredLayoutWidth(0.0f),
  mLastRenderedLayoutSignature(0u),
  mPrepareDirty(true),
  mLayoutDirty(true),
  mRenderDirty(true),
  mForceRenderDirty(true),
  mHasLastRenderedLayoutSignature(false),
  mHasMeasuredLayoutCache(false),
  mRenderDiagnosticsLogged(false)
{
}

TextVisualizerImpl::~TextVisualizerImpl()
{
  ClearRenderHost();
}

void TextVisualizerImpl::SetText(const Dali::String& text)
{
  if(mText != text)
  {
    mText = text;
    MarkPrepareDirty();
    RelayoutRequest();
    InvalidateMeasure();
  }
}

Dali::String TextVisualizerImpl::GetText() const
{
  return mText;
}

void TextVisualizerImpl::SetFontFamily(const Dali::String& fontFamily)
{
  if(mFontFamily != fontFamily)
  {
    mFontFamily = fontFamily;
    MarkPrepareDirty();
    RelayoutRequest();
    InvalidateMeasure();
  }
}

Dali::String TextVisualizerImpl::GetFontFamily() const
{
  return mFontFamily;
}

void TextVisualizerImpl::SetFontSize(float fontSize)
{
  if(mFontSize != fontSize)
  {
    mFontSize = fontSize;
    MarkPrepareDirty();
    RelayoutRequest();
    InvalidateMeasure();
  }
}

float TextVisualizerImpl::GetFontSize() const
{
  return mFontSize;
}

void TextVisualizerImpl::SetLineHeight(float lineHeight)
{
  const float normalizedLineHeight = (lineHeight > 0.0f) ? lineHeight : Text::LINE_HEIGHT_AUTO;

  if(mLineHeight != normalizedLineHeight)
  {
    mLineHeight = normalizedLineHeight;
    ClearMeasuredLayoutCache();
    MarkLayoutDirty();
    MarkPlacementRenderDirty();
    RelayoutRequest();
    InvalidateMeasure();
  }
}

float TextVisualizerImpl::GetLineHeight() const
{
  return mLineHeight;
}

void TextVisualizerImpl::ClearLineHeight()
{
  SetLineHeight(Text::LINE_HEIGHT_AUTO);
}

void TextVisualizerImpl::SetTextColor(const UiColor& color)
{
  if(mTextColor.Resolve() != color.Resolve() || mTextColor.GetColorId() != color.GetColorId())
  {
    mTextColor = color;
    MarkRenderDirty();
    RelayoutRequest();
  }
}

UiColor TextVisualizerImpl::GetTextColor()
{
  return mTextColor;
}

void TextVisualizerImpl::Prepare()
{
  ClearMeasuredLayoutCache();

  Internal::TextVisualizer::TextPreparer::Input input;
  input.text       = mText;
  input.fontFamily = mFontFamily;
  input.fontSize   = mFontSize;

  mPreparedText = Internal::TextVisualizer::TextPreparer::Prepare(input);
  mAtlasViewAdapter.Clear();
  mAtlasRendererBridge.Clear();
  ClearPrepareDirty();
  MarkLayoutDirty();
  MarkRenderDirty();
}

void TextVisualizerImpl::SetExclusionRegions(const Dali::Vector<Rect<float>>& regions)
{
  if(!AreExclusionRegionsEqual(regions))
  {
    UpdateStoredExclusionRegions(regions);
    mExclusionLayoutCache.SetRegions(mExclusionRegions);
    ClearMeasuredLayoutCache();
    MarkLayoutDirty();
    MarkPlacementRenderDirty();
    RelayoutRequest();
    InvalidateMeasure();
  }
}

Dali::Vector<Rect<float>> TextVisualizerImpl::GetExclusionRegions() const
{
  return mExclusionRegions;
}

void TextVisualizerImpl::ClearExclusionRegions()
{
  if(!mExclusionRegions.Empty())
  {
    mExclusionRegions.Clear();
    mExclusionLayoutCache.Clear();
    ClearMeasuredLayoutCache();
    MarkLayoutDirty();
    MarkPlacementRenderDirty();
    RelayoutRequest();
    InvalidateMeasure();
  }
}

void TextVisualizerImpl::OnInitialize()
{
  ViewImpl::OnInitialize();

  Actor self = Self();

  self.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
  self.SetResizePolicy(ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT);
}

void TextVisualizerImpl::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  (void)container;

  bool updateRenderDataResult = false;
  bool attachResult           = false;

  if(mPrepareDirty)
  {
    Prepare();
  }

  const float   layoutWidth = (size.x > 0.0f) ? size.x : GetNaturalTextWidth();
  const Vector2 layoutSize(layoutWidth, size.y);

  if(mLastLayoutSize.x != layoutSize.x || (size.y > 0.0f && mLastLayoutSize.y != size.y))
  {
    MarkLayoutDirty();
    MarkPlacementRenderDirty();
  }

  if(mLayoutDirty)
  {
    if(!TryUseMeasuredLayoutCache(layoutWidth, mLayoutResult))
    {
      UpdateLayout(layoutWidth, mLayoutResult);
    }
    mAtlasViewAdapter.SetPreparedText(&mPreparedText);
    mAtlasViewAdapter.SetLayoutResult(&mLayoutResult);
    mAtlasRendererBridge.SetAdapter(&mAtlasViewAdapter);
    mAtlasRendererBridge.SetTextControlActor(Self());
    ClearLayoutDirty();
  }

  const Vector2 renderSize(layoutWidth, size.y > 0.0f ? size.y : mLayoutResult.height);

  SyncRenderStateToAdapter(renderSize);
  mAtlasRendererBridge.SetLightweightRendererAllowed(!mExclusionRegions.Empty());
  SyncRenderHostSize(renderSize);

  const bool     hasLayoutSignature        = !mLayoutResult.Empty();
  const uint64_t currentLayoutSignature    = hasLayoutSignature ? mLayoutResult.CalculateSignature() : 0u;
  const bool     skipUnchangedLayoutRender = hasLayoutSignature && CanSkipRenderForUnchangedLayout(currentLayoutSignature);

  if(skipUnchangedLayoutRender)
  {
    ClearRenderDirty();
  }

  if(mRenderDirty && mAtlasRendererBridge.HasRenderableGlyphs())
  {
    updateRenderDataResult = mAtlasRendererBridge.UpdateRenderData();
    if(updateRenderDataResult)
    {
      EnsureRenderHost();
      SyncRenderHostSize(renderSize);

      attachResult = mAtlasRendererBridge.AttachRendererToHost();
      if(IsRenderUpdateSuccessful(updateRenderDataResult, attachResult))
      {
        if(hasLayoutSignature)
        {
          StoreLastRenderedLayoutSignature(currentLayoutSignature);
        }
        ClearRenderDirty();
      }
    }
  }

  LogRenderDiagnostics(renderSize, updateRenderDataResult, attachResult);

  mLastLayoutSize = renderSize;
}

MeasuredSize TextVisualizerImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  if(mPrepareDirty)
  {
    Prepare();
  }

  const float requestedWidth  = GetRequestedWidth();
  const float requestedHeight = GetRequestedHeight();
  const float minWidth        = GetMinimumWidth();
  const float maxWidth        = GetMaximumWidth();
  const float minHeight       = GetMinimumHeight();
  const float maxHeight       = GetMaximumHeight();
  const float naturalWidth    = GetNaturalTextWidth();

  float measuredWidth  = 0.0f;
  float measuredHeight = 0.0f;

  if(requestedWidth >= 0.0f)
  {
    measuredWidth = std::max(std::min(requestedWidth, maxWidth), minWidth);
  }
  else if(requestedWidth == MATCH_PARENT)
  {
    measuredWidth = (widthConstraint > 0.0f) ? widthConstraint : minWidth;
    measuredWidth = std::max(std::min(measuredWidth, maxWidth), minWidth);
  }
  else
  {
    const float allowedMaxWidth = (widthConstraint > 0.0f) ? std::min(maxWidth, widthConstraint) : maxWidth;
    measuredWidth               = std::max(std::min(naturalWidth, allowedMaxWidth), minWidth);
  }

  if(requestedHeight >= 0.0f)
  {
    measuredHeight = std::max(std::min(requestedHeight, maxHeight), minHeight);
  }
  else if(requestedHeight == MATCH_PARENT)
  {
    measuredHeight = (heightConstraint > 0.0f) ? heightConstraint : minHeight;
    measuredHeight = std::max(std::min(measuredHeight, maxHeight), minHeight);
  }
  else
  {
    const float allowedMaxHeight = (heightConstraint > 0.0f) ? std::min(maxHeight, heightConstraint) : maxHeight;
    const float layoutWidth      = (requestedWidth == MATCH_PARENT && widthConstraint > 0.0f) ? widthConstraint : measuredWidth;
    const float naturalHeight    = MeasureNaturalTextHeightForWidth(layoutWidth);
    measuredHeight               = std::max(std::min(naturalHeight, allowedMaxHeight), minHeight);
  }

  return MeasuredSize(measuredWidth, measuredHeight);
}

void TextVisualizerImpl::OnPropertySet(Dali::Property::Index index, const Dali::Property::Value& propertyValue)
{
  (void)propertyValue;

  switch(index)
  {
    case Text::TextVisualizerPropertyIndex::TEXT:
    case Text::TextVisualizerPropertyIndex::FONT_FAMILY:
    case Text::TextVisualizerPropertyIndex::FONT_SIZE:
    case Text::TextVisualizerPropertyIndex::TEXT_COLOR:
    case Text::TextVisualizerPropertyIndex::LINE_HEIGHT:
    {
      break;
    }
    default:
    {
      ViewImpl::OnPropertySet(index, propertyValue);
      break;
    }
  }
}

void TextVisualizerImpl::SetProperty(BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value)
{
  Ui::View view = Ui::View::DownCast(BaseHandle(object));
  if(view)
  {
    PropertyHandler::SetProperty(view, index, value);
  }
}

Dali::Property::Value TextVisualizerImpl::GetProperty(BaseObject* object, Dali::Property::Index index)
{
  Property::Value value;
  Ui::View        view = Ui::View::DownCast(BaseHandle(object));
  if(view)
  {
    value = PropertyHandler::GetProperty(view, index);
  }
  return value;
}

void TextVisualizerImpl::MarkPrepareDirty()
{
  mPreparedText.Clear();
  mLayoutResult.Clear();
  ClearMeasuredLayoutCache();
  mAtlasViewAdapter.Clear();
  mAtlasRendererBridge.Clear();
  mPrepareDirty            = true;
  mLayoutDirty             = true;
  mRenderDirty             = true;
  mForceRenderDirty        = true;
  mRenderDiagnosticsLogged = false;
  ResetLastRenderedLayoutSignature();
}

void TextVisualizerImpl::MarkLayoutDirty()
{
  mLayoutDirty             = true;
  mRenderDiagnosticsLogged = false;
}

void TextVisualizerImpl::MarkRenderDirty()
{
  mRenderDirty             = true;
  mForceRenderDirty        = true;
  mRenderDiagnosticsLogged = false;
}

void TextVisualizerImpl::MarkPlacementRenderDirty()
{
  mRenderDirty             = true;
  mForceRenderDirty        = false;
  mRenderDiagnosticsLogged = false;
}

void TextVisualizerImpl::ClearPrepareDirty()
{
  mPrepareDirty = false;
}

void TextVisualizerImpl::ClearLayoutDirty()
{
  mLayoutDirty = false;
}

void TextVisualizerImpl::ClearRenderDirty()
{
  mRenderDirty      = false;
  mForceRenderDirty = false;
}

void TextVisualizerImpl::EnsureRenderHost()
{
  if(!mRenderHost)
  {
    mRenderHost = Actor::New();
    mRenderHost.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    mRenderHost.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
    mRenderHost.SetProperty(Actor::Property::POSITION, Vector3::ZERO);
    mRenderHost.SetProperty(Actor::Property::VISIBLE, true);
    mRenderHost.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);
    mRenderHost.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    IntegrationView::AddActorChild(Ui::View::DownCast(Self()), mRenderHost);
  }

  mAtlasRendererBridge.SetRenderHost(mRenderHost);
  mAtlasRendererBridge.SetTextControlActor(Self());
}

void TextVisualizerImpl::SyncRenderHostSize(const Vector2& size)
{
  if(!mRenderHost)
  {
    return;
  }

  mRenderHost.SetProperty(Actor::Property::SIZE, Vector3(size.x, size.y, 0.0f));
}

void TextVisualizerImpl::ClearRenderHost()
{
  mAtlasRendererBridge.DetachRendererFromHost();
  mAtlasRendererBridge.SetRenderHost(Actor());
  mAtlasRendererBridge.SetTextControlActor(Actor());
  ResetLastRenderedLayoutSignature();

  if(mRenderHost)
  {
    mRenderHost.Unparent();
    mRenderHost.Reset();
  }
}

void TextVisualizerImpl::LogRenderDiagnostics(const Vector2& size, bool updateRenderDataResult, bool attachResult) const
{
  if(!ENABLE_TEXT_VISUALIZER_RENDER_DIAGNOSTICS || mRenderDiagnosticsLogged)
  {
    return;
  }

  const Vector3  renderHostSize      = mAtlasRendererBridge.GetRenderHostSize();
  const Vector3  rendererOutputSize  = mAtlasRendererBridge.GetRendererOutputSize();
  const Vector3  firstChildSize      = mAtlasRendererBridge.GetFirstRendererOutputChildSize();
  const uint32_t glyphPlacementCount = mLayoutResult.glyphPlacements.Count();

  DALI_LOG_RELEASE_INFO(
    "[TextVisualizer][%p] size=(%.2f,%.2f) glyphPlacements=%u hasRenderable=%d updateRenderData=%d attach=%d "
    "renderReady=%d renderCalls=%u attachCalls=%u getGlyphsCalls=%u lastRequested=%u lastReturned=%u lastStart=%u "
    "outputChildren=%u outputDescendants=%u outputRenderers=%u outputTotalRenderers=%u hasRenderableDescendant=%d "
    "outputParentedToHost=%d renderHostSize=(%.2f,%.2f,%.2f) outputSize=(%.2f,%.2f,%.2f) "
    "firstChildSize=(%.2f,%.2f,%.2f) firstChildVisible=%d\n",
    this,
    size.x,
    size.y,
    glyphPlacementCount,
    mAtlasRendererBridge.HasRenderableGlyphs(),
    updateRenderDataResult,
    attachResult,
    mAtlasRendererBridge.IsRenderReady(),
    mAtlasRendererBridge.GetRenderCallCount(),
    mAtlasRendererBridge.GetAttachCallCount(),
    mAtlasRendererBridge.GetViewInterfaceGetGlyphsCallCount(),
    mAtlasRendererBridge.GetLastRequestedGlyphCount(),
    mAtlasRendererBridge.GetLastReturnedGlyphCount(),
    mAtlasRendererBridge.GetLastGlyphStartIndex(),
    mAtlasRendererBridge.GetRendererOutputChildCount(),
    mAtlasRendererBridge.GetRendererOutputDescendantCount(),
    mAtlasRendererBridge.GetRendererOutputRendererCount(),
    mAtlasRendererBridge.GetRendererOutputTotalRendererCount(),
    mAtlasRendererBridge.HasRendererOutputRenderableDescendant(),
    mAtlasRendererBridge.IsRendererOutputParentedToHost(),
    renderHostSize.x,
    renderHostSize.y,
    renderHostSize.z,
    rendererOutputSize.x,
    rendererOutputSize.y,
    rendererOutputSize.z,
    firstChildSize.x,
    firstChildSize.y,
    firstChildSize.z,
    mAtlasRendererBridge.IsFirstRendererOutputChildVisible());

  DALI_LOG_RELEASE_INFO(
    "[TextVisualizer][%p] lightweight attempts=%u successes=%u fallbacks=%u fullRebuilds=%u geometryOnly=%u "
    "glyphCacheEntries=%u meshRecords=%u hasTopology=%d topologySignature=%llu\n",
    this,
    mAtlasRendererBridge.GetLightweightRenderAttemptCount(),
    mAtlasRendererBridge.GetLightweightRenderSuccessCount(),
    mAtlasRendererBridge.GetLightweightRenderFallbackCount(),
    mAtlasRendererBridge.GetLightweightFullMeshRebuildCount(),
    mAtlasRendererBridge.GetLightweightGeometryOnlyUpdateCount(),
    mAtlasRendererBridge.GetLightweightGlyphCacheEntryCount(),
    mAtlasRendererBridge.GetLightweightMeshRecordCount(),
    mAtlasRendererBridge.HasLightweightMeshTopologySignature(),
    static_cast<unsigned long long>(mAtlasRendererBridge.GetLightweightMeshTopologySignature()));

  DALI_LOG_RELEASE_INFO(
    "[TextVisualizer][%p] lightweightFailure cacheMiss=%u emptyMesh=%u placement=%u glyphInfo=%u position=%u "
    "input=%u noRenderable=%u noPositionCache=%u signature=%u noGlyphManager=%u noOutput=%u noTexture=%u "
    "lastReason=%u lastPlacement=%u lastGlyphIndex=%u lastFontId=%u lastGlyphId=%u "
    "lastAdvance=%.2f lastWidth=%.2f lastHeight=%.2f\n",
    this,
    mAtlasRendererBridge.GetLightweightFailureCacheMissCount(),
    mAtlasRendererBridge.GetLightweightFailureEmptyMeshCount(),
    mAtlasRendererBridge.GetLightweightFailureGlyphPlacementCount(),
    mAtlasRendererBridge.GetLightweightFailureGlyphInfoCount(),
    mAtlasRendererBridge.GetLightweightFailureGlyphPositionCount(),
    mAtlasRendererBridge.GetLightweightFailureEmptyInputCount(),
    mAtlasRendererBridge.GetLightweightFailureNoRenderableGlyphsCount(),
    mAtlasRendererBridge.GetLightweightFailureNoPositionCacheCount(),
    mAtlasRendererBridge.GetLightweightFailureSignatureCount(),
    mAtlasRendererBridge.GetLightweightFailureNoGlyphManagerCount(),
    mAtlasRendererBridge.GetLightweightFailureNoOutputActorCount(),
    mAtlasRendererBridge.GetLightweightFailureNoTextureSetCount(),
    static_cast<uint32_t>(mAtlasRendererBridge.GetLightweightLastFailureReason()),
    mAtlasRendererBridge.GetLightweightLastFailedPlacementIndex(),
    mAtlasRendererBridge.GetLightweightLastFailedGlyphIndex(),
    mAtlasRendererBridge.GetLightweightLastFailedFontId(),
    mAtlasRendererBridge.GetLightweightLastFailedGlyphId(),
    mAtlasRendererBridge.GetLightweightLastFailedGlyphAdvance(),
    mAtlasRendererBridge.GetLightweightLastFailedGlyphWidth(),
    mAtlasRendererBridge.GetLightweightLastFailedGlyphHeight());

  mRenderDiagnosticsLogged = true;
}

bool TextVisualizerImpl::HasRenderHost() const
{
  return static_cast<bool>(mRenderHost);
}

bool TextVisualizerImpl::CanSkipRenderForUnchangedLayout(uint64_t layoutSignature) const
{
  return mRenderDirty &&
         !mForceRenderDirty &&
         mHasLastRenderedLayoutSignature &&
         layoutSignature == mLastRenderedLayoutSignature &&
         mAtlasRendererBridge.IsRenderReady() &&
         mAtlasRendererBridge.GetRendererOutput() &&
         mAtlasRendererBridge.IsRendererOutputParentedToHost();
}

bool TextVisualizerImpl::IsRenderUpdateSuccessful(bool updateRenderDataResult, bool attachResult) const
{
  if(!mRenderDirty ||
     !mAtlasRendererBridge.HasRenderableGlyphs() ||
     !updateRenderDataResult ||
     !attachResult ||
     !mAtlasRendererBridge.IsRenderReady() ||
     !mAtlasRendererBridge.GetRendererOutput() ||
     !mAtlasRendererBridge.IsRendererOutputParentedToHost())
  {
    return false;
  }

  const uint32_t lastRequestedGlyphCount = mAtlasRendererBridge.GetLastRequestedGlyphCount();
  const uint32_t lastReturnedGlyphCount  = mAtlasRendererBridge.GetLastReturnedGlyphCount();

  return lastReturnedGlyphCount > 0u &&
         lastReturnedGlyphCount <= lastRequestedGlyphCount;
}

void TextVisualizerImpl::ResetLastRenderedLayoutSignature()
{
  mLastRenderedLayoutSignature    = 0u;
  mHasLastRenderedLayoutSignature = false;
}

void TextVisualizerImpl::StoreLastRenderedLayoutSignature(uint64_t layoutSignature)
{
  mLastRenderedLayoutSignature    = layoutSignature;
  mHasLastRenderedLayoutSignature = true;
}

float TextVisualizerImpl::CalculateEffectiveLineHeight() const
{
  if(mLineHeight > 0.0f && mFontSize > 0.0f)
  {
    return mFontSize * mLineHeight;
  }

  return 0.0f;
}

void TextVisualizerImpl::UpdateStoredExclusionRegions(const Dali::Vector<Rect<float>>& regions)
{
  const uint32_t count = regions.Count();
  if(mExclusionRegions.Count() == count)
  {
    for(uint32_t index = 0u; index < count; ++index)
    {
      mExclusionRegions[index] = regions[index];
    }
    return;
  }

  mExclusionRegions.Clear();
  mExclusionRegions.Reserve(count);
  for(uint32_t index = 0u; index < count; ++index)
  {
    mExclusionRegions.PushBack(regions[index]);
  }
}

bool TextVisualizerImpl::AreExclusionRegionsEqual(const Dali::Vector<Rect<float>>& regions) const
{
  if(mExclusionRegions.Count() != regions.Count())
  {
    return false;
  }

  const uint32_t count = regions.Count();
  for(uint32_t index = 0u; index < count; ++index)
  {
    if(mExclusionRegions[index] != regions[index])
    {
      return false;
    }
  }

  return true;
}

void TextVisualizerImpl::ClearMeasuredLayoutCache()
{
  mMeasuredLayoutCache.Clear();
  mMeasuredLayoutWidth    = 0.0f;
  mHasMeasuredLayoutCache = false;
}

void TextVisualizerImpl::StoreMeasuredLayoutCache(float layoutWidth, const Internal::TextVisualizer::LayoutResult& result)
{
  mMeasuredLayoutCache    = result;
  mMeasuredLayoutWidth    = layoutWidth;
  mHasMeasuredLayoutCache = true;
}

bool TextVisualizerImpl::TryUseMeasuredLayoutCache(float layoutWidth, Internal::TextVisualizer::LayoutResult& result)
{
  if(!mHasMeasuredLayoutCache || std::fabs(mMeasuredLayoutWidth - layoutWidth) > 0.001f)
  {
    return false;
  }

  result = mMeasuredLayoutCache;
  ClearMeasuredLayoutCache();
  return true;
}

float TextVisualizerImpl::GetNaturalTextWidth() const
{
  if(mPreparedText.Empty() || mPreparedText.GetClusterCount() == 0u)
  {
    return 0.0f;
  }

  if(mPreparedText.HasGlyphData() && mPreparedText.HasGlyphMetrics())
  {
    return std::max(0.0f, mPreparedText.GetTotalGlyphAdvance());
  }

  const float clusterAdvance = Internal::TextVisualizer::LayoutEngine::GetPlaceholderClusterAdvance(mPreparedText);
  return std::max(0.0f, static_cast<float>(mPreparedText.GetClusterCount()) * clusterAdvance);
}

float TextVisualizerImpl::MeasureNaturalTextHeightForWidth(float layoutWidth)
{
  if(layoutWidth <= 0.0f || mPreparedText.Empty() || mPreparedText.GetClusterCount() == 0u)
  {
    return 0.0f;
  }

  Internal::TextVisualizer::LayoutResult measuredLayoutResult;
  UpdateLayout(layoutWidth, measuredLayoutResult);
  StoreMeasuredLayoutCache(layoutWidth, measuredLayoutResult);
  return measuredLayoutResult.Empty() ? 0.0f : measuredLayoutResult.height;
}

void TextVisualizerImpl::UpdateLayout(float layoutWidth, Internal::TextVisualizer::LayoutResult& result)
{
  result.Clear();

  if(mPreparedText.Empty() || mPreparedText.GetClusterCount() == 0u)
  {
    return;
  }

  const float effectiveLineHeight = CalculateEffectiveLineHeight();

  if(mPreparedText.HasGlyphData() && mPreparedText.HasGlyphMetrics())
  {
    Internal::TextVisualizer::LayoutEngine::LayoutGlyphs(mPreparedText, layoutWidth, effectiveLineHeight, mExclusionLayoutCache, result);
  }
  else
  {
    Internal::TextVisualizer::LayoutEngine::LayoutPlaceholder(mPreparedText, layoutWidth, effectiveLineHeight, mExclusionLayoutCache, result);
  }
}

void TextVisualizerImpl::SyncRenderStateToAdapter(const Vector2& controlSize)
{
  mAtlasViewAdapter.SetControlSize(controlSize);
  mAtlasViewAdapter.SetTextColor(mTextColor.Resolve());
}

} // namespace Integration

} // namespace Ui

} // namespace Dali
