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
#include <dali/devel-api/adaptor-framework/image-loading-devel.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/devel-api/rendering/texture-devel.h>
#include <dali/devel-api/text-abstraction/text-abstraction-definitions.h>
#include <dali/integration-api/constraint-integ.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/pixel-data-integ.h>
#include <dali/integration-api/string-utils.h>
#include <dali/integration-api/texture-integ.h>
#include <dali/integration-api/trace.h>
#include <string.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-ui-foundation/internal/text/color-glyph-helper.h>
#include <dali-ui-foundation/internal/text/script-run.h>
#include <dali-ui-foundation/internal/text/text-effects-style.h>
#include <dali-ui-foundation/internal/text/text-enumerations-impl.h>
#include <dali-ui-foundation/internal/text/text-enumerations.h>
#include <dali-ui-foundation/internal/text/text-font-style.h>
#include <dali-ui-foundation/internal/text/text-gradient-bounds.h>
#include <dali-ui-foundation/internal/text/text-gradient-helper.h>
#include <dali-ui-foundation/internal/visuals/text/text-visual.h>
#include <dali-ui-foundation/internal/visuals/visual-base-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-string-constants.h>
#include <dali-ui-foundation/public-api/types/ui-constraint-tag-ranges.h>
#include <dali-ui-foundation/public-api/visuals/text-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

using Dali::Integration::ToDaliString;
using Dali::Integration::ToPropertyValue;
using Dali::Integration::ToStdString;

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_TEXT_PERFORMANCE_MARKER, false);
DALI_INIT_TRACE_FILTER(gTraceFilter2, DALI_TRACE_TEXT_ASYNC, false);

const int CUSTOM_PROPERTY_COUNT(24); // uTextColorAnimatable, uHasMultipleTextColors, requireRender, gradient uniforms

static constexpr uint32_t TEXT_VISUAL_COLOR_CONSTRAINT_TAG(Dali::Ui::ConstraintTagRanges::UI_CONSTRAINT_TAG_START + 21);
static constexpr uint32_t TEXT_VISUAL_OPACITY_CONSTRAINT_TAG(Dali::Ui::ConstraintTagRanges::UI_CONSTRAINT_TAG_START +
                                                             22);
static constexpr uint32_t TEXT_VISUAL_GRADIENT_START_OFFSET_CONSTRAINT_TAG(Dali::Ui::ConstraintTagRanges::UI_CONSTRAINT_TAG_START + 23);
static constexpr uint32_t TEXT_VISUAL_GRADIENT_OVERLAY_START_OFFSET_CONSTRAINT_TAG(Dali::Ui::ConstraintTagRanges::UI_CONSTRAINT_TAG_START + 24);
static constexpr uint32_t TEXT_VISUAL_REVEAL_PROGRESS_CONSTRAINT_TAG(Dali::Ui::ConstraintTagRanges::UI_CONSTRAINT_TAG_START + 25);

const float VERTICAL_ALIGNMENT_TABLE[static_cast<int>(Text::Alignment::END) + 1] = {
  0.0f, // Text::Alignment::START
  0.5f, // Text::Alignment::CENTER
  1.0f  // Text::Alignment::END
};

constexpr const char* UNIFORM_TEXT_GRADIENT_START_POSITION_NAME    = "uTextGradientStartPosition";
constexpr const char* UNIFORM_TEXT_GRADIENT_END_POSITION_NAME      = "uTextGradientEndPosition";
constexpr const char* UNIFORM_TEXT_GRADIENT_START_OFFSET_NAME      = "uTextGradientStartOffset";
constexpr const char* UNIFORM_TEXT_GRADIENT_BOUNDS_NAME            = "uTextGradientBounds";
constexpr const char* UNIFORM_TEXT_GRADIENT_TYPE_NAME              = "uTextGradientType";
constexpr const char* UNIFORM_TEXT_GRADIENT_RADIAL_CENTER_NAME     = "uTextGradientRadialCenter";
constexpr const char* UNIFORM_TEXT_GRADIENT_RADIAL_SCALE_NAME      = "uTextGradientRadialScale";
constexpr const char* UNIFORM_TEXT_GRADIENT_CONIC_CENTER_NAME      = "uTextGradientConicCenter";
constexpr const char* UNIFORM_TEXT_GRADIENT_CONIC_SCALE_NAME       = "uTextGradientConicScale";
constexpr const char* UNIFORM_TEXT_GRADIENT_CONIC_START_ANGLE_NAME = "uTextGradientConicStartAngle";

constexpr const char* UNIFORM_TEXT_GRADIENT_OVERLAY_START_POSITION_NAME    = "uTextGradientOverlayStartPosition";
constexpr const char* UNIFORM_TEXT_GRADIENT_OVERLAY_END_POSITION_NAME      = "uTextGradientOverlayEndPosition";
constexpr const char* UNIFORM_TEXT_GRADIENT_OVERLAY_START_OFFSET_NAME      = "uTextGradientOverlayStartOffset";
constexpr const char* UNIFORM_TEXT_GRADIENT_OVERLAY_BOUNDS_NAME            = "uTextGradientOverlayBounds";
constexpr const char* UNIFORM_TEXT_GRADIENT_OVERLAY_TYPE_NAME              = "uTextGradientOverlayType";
constexpr const char* UNIFORM_TEXT_GRADIENT_OVERLAY_RADIAL_CENTER_NAME     = "uTextGradientOverlayRadialCenter";
constexpr const char* UNIFORM_TEXT_GRADIENT_OVERLAY_RADIAL_SCALE_NAME      = "uTextGradientOverlayRadialScale";
constexpr const char* UNIFORM_TEXT_GRADIENT_OVERLAY_CONIC_CENTER_NAME      = "uTextGradientOverlayConicCenter";
constexpr const char* UNIFORM_TEXT_GRADIENT_OVERLAY_CONIC_SCALE_NAME       = "uTextGradientOverlayConicScale";
constexpr const char* UNIFORM_TEXT_GRADIENT_OVERLAY_CONIC_START_ANGLE_NAME = "uTextGradientOverlayConicStartAngle";
constexpr const char* UNIFORM_TEXT_GRADIENT_OVERLAY_MODE_NAME              = "uTextGradientOverlayMode";
constexpr const char* UNIFORM_TEXT_REVEAL_PROGRESS_NAME                    = "uTextRevealProgress";
constexpr const char* UNIFORM_TEXT_REVEAL_FADE_DURATION_NAME               = "uTextRevealFadeDuration";

bool IsAsyncRenderRequest(Ui::Integration::Text::Async::RequestType requestType)
{
  switch(requestType)
  {
    case Ui::Integration::Text::Async::RENDER_FIXED_SIZE:
    case Ui::Integration::Text::Async::RENDER_FIXED_WIDTH:
    case Ui::Integration::Text::Async::RENDER_FIXED_HEIGHT:
    case Ui::Integration::Text::Async::RENDER_CONSTRAINT:
      return true;
    case Ui::Integration::Text::Async::COMPUTE_NATURAL_SIZE:
    case Ui::Integration::Text::Async::COMPUTE_HEIGHT_FOR_WIDTH:
      return false;
  }
  return false;
}

bool HasCompleteTextRevealMetadata(const Text::AsyncTextRenderInfo& renderInfo, int maxTextureSize)
{
  if(!renderInfo.isTextRevealEnabled || !renderInfo.textPixelData || maxTextureSize <= 0)
  {
    return false;
  }

  const uint32_t metadataWidth  = renderInfo.textPixelData.GetWidth();
  const uint32_t metadataHeight = renderInfo.textPixelData.GetHeight();
  if(metadataWidth == 0u || metadataHeight == 0u)
  {
    return false;
  }

  // Renderer tiling uses the reported logical projection. Render scale can
  // make the worker raster larger, so tiled metadata follows the exact region
  // uploaded by each renderer rather than the full raster dimensions.
  const int32_t rendererHeight = static_cast<int32_t>(renderInfo.size.height);
  const int32_t rendererWidth  = static_cast<int32_t>(renderInfo.size.width);
  if(rendererWidth <= 0 || rendererHeight <= 0)
  {
    return false;
  }
  const size_t expectedRendererTiles =
    1u + static_cast<size_t>(rendererHeight - 1) / static_cast<size_t>(maxTextureSize);
  if(renderInfo.revealMetadataTiles.size() != expectedRendererTiles)
  {
    return false;
  }

  const bool isHeightTiling = expectedRendererTiles > 1u;
  if(isHeightTiling &&
     (metadataWidth < static_cast<uint32_t>(rendererWidth) ||
      metadataHeight < static_cast<uint32_t>(rendererHeight)))
  {
    return false;
  }

  int32_t rendererOffsetY = 0;
  for(const PixelData& metadata : renderInfo.revealMetadataTiles)
  {
    const uint32_t expectedWidth          = isHeightTiling ? static_cast<uint32_t>(rendererWidth) : metadataWidth;
    const uint32_t expectedRendererHeight = static_cast<uint32_t>(std::min(maxTextureSize, rendererHeight - rendererOffsetY));
    const uint32_t expectedHeight         = isHeightTiling ? expectedRendererHeight : metadataHeight;
    if(!metadata || metadata.GetPixelFormat() != Pixel::RGBA8888 ||
       metadata.GetWidth() != expectedWidth || metadata.GetHeight() != expectedHeight)
    {
      return false;
    }
    rendererOffsetY += static_cast<int32_t>(expectedRendererHeight);
  }

  return rendererOffsetY == rendererHeight;
}

#ifdef TRACE_ENABLED
constexpr const char* ASYNC_REQUEST_TYPE_NAME[] = {"RENDER_FIXED_SIZE", "RENDER_FIXED_WIDTH", "RENDER_FIXED_HEIGHT",
                                                   "RENDER_CONSTRAINT", "COMPUTE_NATURAL_SIZE", "COMPUTE_HEIGHT_FOR_WIDTH"};

const char* GetRequestTypeName(Ui::Integration::Text::Async::RequestType type)
{
  if(type < Ui::Integration::Text::Async::RENDER_FIXED_SIZE || type > Ui::Integration::Text::Async::COMPUTE_HEIGHT_FOR_WIDTH)
  {
    return "INVALID_REQUEST_TYPE";
  }
  return ASYNC_REQUEST_TYPE_NAME[type];
}
#endif

struct NameIndexMatch
{
  const char* const name;
  Property::Index   index;
};

const NameIndexMatch NAME_INDEX_MATCH_TABLE[] = {
  {TEXT_PROPERTY, Ui::TextVisualPropertyIndex::TEXT},
  {FONT_FAMILY_PROPERTY, Ui::TextVisualPropertyIndex::FONT_FAMILY},
  {FONT_SIZE_PROPERTY, Ui::TextVisualPropertyIndex::FONT_SIZE},
  {FONT_WEIGHT_PROPERTY, Ui::TextVisualPropertyIndex::FONT_WEIGHT},
  {FONT_WIDTH_PROPERTY, Ui::TextVisualPropertyIndex::FONT_WIDTH},
  {FONT_SLANT_PROPERTY, Ui::TextVisualPropertyIndex::FONT_SLANT},
  {MULTI_LINE_PROPERTY, Ui::TextVisualPropertyIndex::MULTI_LINE},
  {LINE_WRAP_MODE_PROPERTY, Ui::TextVisualPropertyIndex::LINE_WRAP_MODE},
  {HORIZONTAL_ALIGNMENT_PROPERTY, Ui::TextVisualPropertyIndex::HORIZONTAL_ALIGNMENT},
  {VERTICAL_ALIGNMENT_PROPERTY, Ui::TextVisualPropertyIndex::VERTICAL_ALIGNMENT},
  {OVERFLOW_MODE_PROPERTY, Ui::TextVisualPropertyIndex::OVERFLOW_MODE},
  {LINE_HEIGHT_PROPERTY, Ui::TextVisualPropertyIndex::LINE_HEIGHT},
  {LINE_HEIGHT_MODE_PROPERTY, Ui::TextVisualPropertyIndex::LINE_HEIGHT_MODE},
  {TEXT_COLOR_PROPERTY, Ui::TextVisualPropertyIndex::TEXT_COLOR},
};
const int NAME_INDEX_MATCH_TABLE_SIZE = sizeof(NAME_INDEX_MATCH_TABLE) / sizeof(NAME_INDEX_MATCH_TABLE[0]);

void TextColorConstraint(Vector4& current, const PropertyInputContainer& inputs)
{
  Vector4 color = inputs[0]->GetVector4();
  current.r     = color.r * color.a;
  current.g     = color.g * color.a;
  current.b     = color.b * color.a;
  current.a     = color.a;
}

void OpacityConstraint(float& current, const PropertyInputContainer& inputs)
{
  // Make zero if the alpha value of text color is zero to skip rendering text
  if(EqualsZero(inputs[0]->GetVector4().a) && !inputs[1]->GetBoolean())
  {
    current = 0.0f;
  }
  else
  {
    current = 1.0f;
  }
}

void GradientOffsetConstraint(float& current, const PropertyInputContainer& inputs)
{
  current = inputs[0]->GetFloat();
}

} // unnamed namespace

TextVisualPtr TextVisual::New(VisualFactoryCache& factoryCache, TextVisualShaderFactory& shaderFactory,
                              const Property::Map& properties)
{
  TextVisualPtr textVisualPtr(new TextVisual(factoryCache, shaderFactory));
  textVisualPtr->SetProperties(properties);
  textVisualPtr->Initialize();
  return textVisualPtr;
}

float TextVisual::GetHeightForWidth(float width)
{
  return mController->GetHeightForWidth(width);
}

void TextVisual::GetNaturalSize(Vector2& naturalSize)
{
  naturalSize = mController->GetNaturalSize().GetVectorXY();
}

void TextVisual::DoCreatePropertyMap(Property::Map& map) const
{
  Property::Value value;

  map.Clear();
  map.Insert(Ui::VisualBasePropertyIndex::TYPE, Ui::Integration::InternalVisualType::TEXT);

  std::string text;
  mController->GetText(text);
  map.Insert(Ui::TextVisualPropertyIndex::TEXT, ToPropertyValue(text));

  map.Insert(Ui::TextVisualPropertyIndex::FONT_FAMILY, ToPropertyValue(mController->GetDefaultFontFamily()));
  map.Insert(Ui::TextVisualPropertyIndex::FONT_SIZE, mController->GetDefaultFontSize(Text::Controller::PIXEL_SIZE));
  map.Insert(Ui::TextVisualPropertyIndex::FONT_WEIGHT, Text::ToFontWeight(mController->GetDefaultFontWeight()));
  map.Insert(Ui::TextVisualPropertyIndex::FONT_WIDTH, Text::ToFontWidth(mController->GetDefaultFontWidth()));
  map.Insert(Ui::TextVisualPropertyIndex::FONT_SLANT, Text::ToFontSlant(mController->GetDefaultFontSlant()));

  map.Insert(Ui::TextVisualPropertyIndex::MULTI_LINE, mController->IsMultiLineEnabled());
  map.Insert(Ui::TextVisualPropertyIndex::LINE_WRAP_MODE, mController->GetLineWrapMode());

  map.Insert(Ui::TextVisualPropertyIndex::HORIZONTAL_ALIGNMENT, mController->GetHorizontalAlignment());
  map.Insert(Ui::TextVisualPropertyIndex::VERTICAL_ALIGNMENT, mController->GetVerticalAlignment());

  map.Insert(Ui::TextVisualPropertyIndex::OVERFLOW_MODE, mOverflowMode);
  map.Insert(Ui::TextVisualPropertyIndex::LINE_HEIGHT, mLineHeight);
  map.Insert(Ui::TextVisualPropertyIndex::LINE_HEIGHT_MODE, mLineHeightMode);

  map.Insert(Ui::TextVisualPropertyIndex::TEXT_COLOR, mController->GetDefaultColor());
}

void TextVisual::DoCreateInstancePropertyMap(Property::Map& map) const
{
  map.Clear();
  map.Insert(Ui::VisualBasePropertyIndex::TYPE, Ui::Integration::InternalVisualType::TEXT);
  std::string text;
  mController->GetText(text);
  map.Insert(Ui::TextVisualPropertyIndex::TEXT, ToPropertyValue(text));
}

TextVisual::TextVisual(VisualFactoryCache& factoryCache, TextVisualShaderFactory& shaderFactory)
: Visual::Base(factoryCache, Ui::Integration::InternalVisualType::TEXT),
  mController(Text::Controller::New()),
  mTypesetter(Text::Typesetter::New(mController->GetRenderTextModel())),
  mAsyncTextInterface(nullptr),
  mGradientData(),
  mRevealData(),
  mTextVisualShaderFactory(shaderFactory),
  mTextShaderFeatureCache(),
  mHasMultipleTextColorsIndex(Property::INVALID_INDEX),
  mAnimatableTextColorPropertyIndex(Property::INVALID_INDEX),
  mTextColorAnimatableIndex(Property::INVALID_INDEX),
  mTextRequireRenderPropertyIndex(Property::INVALID_INDEX),
  mLineHeight(Text::LINE_HEIGHT_AUTO),
  mLineHeightMode(Text::LineHeightMode::RELATIVE),
  mOverflowMode(Text::OverflowMode::ELLIPSIS),
  mTextLoadingTaskId(0u),
  mNaturalSizeTaskId(0u),
  mHeightForWidthTaskId(0u),
  mRendererUpdateNeeded(false),
  mApplyingFittingMode(false),
  mTextRequireRender(false),
  mIsConstraintAppliedAlways(false),
  mIsTextLoadingTaskRunning(false),
  mIsNaturalSizeTaskRunning(false),
  mIsHeightForWidthTaskRunning(false)
{
  // Enable the pre-multiplied alpha to improve the text quality
  mImpl->mFlags |= Impl::IS_PRE_MULTIPLIED_ALPHA;

  // Enable fitting mode, to acquire effectiveScale value.
  mImpl->mFittingModeRequired = true;
}

TextVisual::~TextVisual()
{
}

void TextVisual::OnInitialize()
{
  Geometry geometry       = mFactoryCache.GetGeometry(VisualFactoryCache::QUAD_GEOMETRY);
  auto     featureBuilder = TextVisualShaderFeature::FeatureBuilder();
  Shader   shader         = GetTextShader(mFactoryCache, featureBuilder);

  mImpl->mRenderer = VisualRenderer::New(geometry, shader);
  mImpl->mRenderer.ReserveCustomProperties(CUSTOM_PROPERTY_COUNT);
  mTextRequireRenderPropertyIndex = mImpl->mRenderer.RegisterUniqueProperty("requireRender", mTextRequireRender);
  mHasMultipleTextColorsIndex =
    mImpl->mRenderer.RegisterUniqueProperty("uHasMultipleTextColors", static_cast<float>(false));

  // Apply overflow mode now. (Because contorller's text elide disabled as default)
  switch(mOverflowMode)
  {
    case Text::OverflowMode::CLIP:
    {
      mController->SetTextElideEnabled(false);
      break;
    }
    case Text::OverflowMode::ELLIPSIS:
    {
      mController->SetTextElideEnabled(true);
      break;
    }
  }

  // Retrieve the layout engine to set the cursor's width.
  Text::Layout::Engine& engine = mController->GetLayoutEngine();

  // Sets 0 as cursor's width.
  engine.SetCursorWidth(0u); // Do not layout space for the cursor.

  // Register transform properties
  mImpl->SetTransformUniforms(mImpl->mRenderer, static_cast<Ui::Integration::Direction::Type>(Text::Direction::LEFT_TO_RIGHT));
}

void TextVisual::DoSetProperties(const Property::Map& propertyMap)
{
  for(Property::Map::SizeType index = 0u, count = propertyMap.Count(); index < count; ++index)
  {
    const KeyValuePair& keyValue = propertyMap.GetKeyValue(index);
    if(keyValue.first.type == Property::Key::INDEX)
    {
      DoSetProperty(keyValue.first.indexKey, keyValue.second);
    }
    else
    {
      for(int i = 0; i < NAME_INDEX_MATCH_TABLE_SIZE; ++i)
      {
        if(keyValue.first == NAME_INDEX_MATCH_TABLE[i].name)
        {
          DoSetProperty(NAME_INDEX_MATCH_TABLE[i].index, keyValue.second);
          break;
        }
      }
    }
  }

  if(IsOnScene())
  {
    mRendererUpdateNeeded = true;

    // TODO : Need to trigger the owner to call OnSetTransform()
    // TODO : RelayoutRequest might not be works for dali-ui.
    // if(mImpl->mEventObserver)
    // {
    //   mImpl->mEventObserver->RelayoutRequest(*this);
    // }
    UpdateRenderer();
  }
}

void TextVisual::DoSetOnScene(Actor& actor)
{
  mControl = actor;

  const Vector4& defaultColor = mController->GetRenderTextModel()->GetDefaultColor();
  if(mTextColorAnimatableIndex == Property::INVALID_INDEX)
  {
    mTextColorAnimatableIndex = mImpl->mRenderer.RegisterUniqueProperty("uTextColorAnimatable", defaultColor);
  }
  else
  {
    mImpl->mRenderer.SetProperty(mTextColorAnimatableIndex, defaultColor);
  }

  if(mAnimatableTextColorPropertyIndex != Property::INVALID_INDEX)
  {
    // Create constraint for the animatable text's color Property with uTextColorAnimatable in the renderer.
    if(mTextColorAnimatableIndex != Property::INVALID_INDEX)
    {
      if(!mColorConstraint)
      {
        mColorConstraint = Constraint::New<Vector4>(mImpl->mRenderer, mTextColorAnimatableIndex, TextColorConstraint);
        mColorConstraint.AddSource(Source(actor, mAnimatableTextColorPropertyIndex));
        Dali::Integration::ConstraintSetInternalTag(mColorConstraint, TEXT_VISUAL_COLOR_CONSTRAINT_TAG);
        mColorConstraint.Apply();
      }
      mColorConstraint.SetApplyRate(mIsConstraintAppliedAlways ? Dali::Constraint::APPLY_ALWAYS
                                                               : Dali::Constraint::APPLY_ONCE);

      mColorConstraintList.push_back(mColorConstraint);
    }

    // Make zero if the alpha value of text color is zero to skip rendering text
    if(!mOpacityConstraint)
    {
      // VisualRenderer::Property::OPACITY uses same animatable property internally.
      mOpacityConstraint =
        Constraint::New<float>(mImpl->mRenderer, Dali::DevelRenderer::Property::OPACITY, OpacityConstraint);
      mOpacityConstraint.AddSource(Source(actor, mAnimatableTextColorPropertyIndex));
      mOpacityConstraint.AddSource(Source(mImpl->mRenderer, mTextRequireRenderPropertyIndex));
      Dali::Integration::ConstraintSetInternalTag(mOpacityConstraint, TEXT_VISUAL_OPACITY_CONSTRAINT_TAG);
      mOpacityConstraint.Apply();
    }
    mOpacityConstraint.SetApplyRate(mIsConstraintAppliedAlways ? Dali::Constraint::APPLY_ALWAYS
                                                               : Dali::Constraint::APPLY_ONCE);

    mOpacityConstraintList.push_back(mOpacityConstraint);
  }

  // Renderer needs textures and to be added to control
  mRendererUpdateNeeded = true;

  UpdateRenderer();
}

void TextVisual::RemoveRenderer(Actor& actor, bool removeDefaultRenderer)
{
  for(RendererContainer::iterator iter = mRendererList.begin(); iter != mRendererList.end(); ++iter)
  {
    Renderer renderer = (*iter);
    if(renderer && (removeDefaultRenderer || (renderer != mImpl->mRenderer)))
    {
      // Removes the renderer from the actor.
      actor.RemoveRenderer(renderer);
    }
  }
  // Clear the renderer list
  mRendererList.clear();
  if(auto* gradientData = GetTextVisualGradientData(mGradientData))
  {
    gradientData->mTextGradientMaskPixelData = PixelData();
    gradientData->mGradientRenderer          = VisualRenderer();
    gradientData->mHasGradientContext        = false;
    gradientData->mGradientOverlayRenderer   = VisualRenderer();
    gradientData->mHasGradientOverlayContext = false;
  }
  RemoveGradientAnimConstraints();
  RemoveGradientOverlayAnimConstraints();
  RemoveTextRevealConstraints();

  if(removeDefaultRenderer)
  {
    // Remove default renderer's textureset
    mImpl->mRenderer.RemoveTextures();
  }

  // Clear constraint, and keep default renderer's constraint only.
  if(mColorConstraint)
  {
    for(auto& constraint : mColorConstraintList)
    {
      if(constraint && (constraint != mColorConstraint))
      {
        constraint.Remove();
      }
    }
    mColorConstraintList.clear();
    mColorConstraintList.push_back(mColorConstraint);
  }
  if(mOpacityConstraint)
  {
    for(auto& constraint : mOpacityConstraintList)
    {
      if(constraint && (constraint != mOpacityConstraint))
      {
        constraint.Remove();
      }
    }
    mOpacityConstraintList.clear();
    mOpacityConstraintList.push_back(mOpacityConstraint);
  }
}

void TextVisual::DoSetOffScene(Actor& actor)
{
  if(mController->IsAsyncRendering() && mIsTextLoadingTaskRunning)
  {
    Text::AsyncTextManager::Get().RequestCancel(mTextLoadingTaskId);
    mIsTextLoadingTaskRunning = false;
  }

  RemoveRenderer(actor, true);

  // Change the constraint as APPLY_ONCE if apply rate was always.
  if(mIsConstraintAppliedAlways)
  {
    mColorConstraint.SetApplyRate(Dali::Constraint::APPLY_ONCE);
    mOpacityConstraint.SetApplyRate(Dali::Constraint::APPLY_ONCE);
  }

  // Resets the control handle.
  mControl.Reset();
}

void TextVisual::SetFittingMode(Ui::Image::FittingMode fittingMode)
{
  // Do nothing
}

void TextVisual::OnApplyFittingMode(const Vector2& controlSize, const Insets& padding, float effectiveScale)
{
  // Apply UiScale to controller
  const bool uiScaleChanged = mController->SetUiScale(effectiveScale);
  if(uiScaleChanged || controlSize != mImpl->mControlSize)
  {
    if(uiScaleChanged)
    {
      mController->InvalidateFontData();
    }

    // Renderer needs textures and to be added to control
    mRendererUpdateNeeded = true;
  }
  // Track the active fitting update so resource reuse is limited to this call.
  mApplyingFittingMode = true;
  Visual::Base::OnApplyFittingMode(controlSize, padding, effectiveScale);
  mApplyingFittingMode = false;
}

Vector4 TextVisual::CalculateGradientViewBounds(Ui::Integration::Visual::Base visual, const Vector2& coordinateSize)
{
  TextVisual& visualObject = GetVisualObject(visual);
  Vector2     visualOffset = Vector2::ZERO;
  if(visualObject.mImpl->mTransform)
  {
    visualOffset = visualObject.mImpl->mTransform->mOffset;
  }
  return Text::Internal::CalculateGradientViewBounds(coordinateSize, visualObject.mImpl->mControlSize, visualOffset);
}

Vector2 TextVisual::GetGradientViewCoordinateSize(Ui::Integration::Visual::Base visual)
{
  TextVisual& visualObject = GetVisualObject(visual);
  return visualObject.mImpl->GetTransformVisualSize(visualObject.mImpl->mControlSize);
}

PixelData TextVisual::RenderMarqueeText(Ui::Integration::Visual::Base     visual,
                                        const Vector2&                    size,
                                        Text::Direction                   textDirection,
                                        Text::Typesetter::RenderBehaviour behaviour,
                                        bool                              ignoreHorizontalAlignment,
                                        Pixel::Format                     pixelFormat,
                                        const Vector2&                    originSize)
{
  TextVisual& visualObject = GetVisualObject(visual);
  visualObject.mTypesetter->SetModel(visualObject.mController->GetRenderTextModel());
  if(visualObject.mController->HasValidReplacementSource())
  {
    visualObject.mTypesetter->SetFinalElisionResult(visualObject.mController->GetFinalElisionResult());
  }
  return visualObject.mTypesetter->Render(size, textDirection, behaviour, ignoreHorizontalAlignment, pixelFormat, originSize);
}

PixelData TextVisual::RenderMarqueeTextGradientPreserved(Ui::Integration::Visual::Base visual,
                                                         const Vector2&                size,
                                                         Text::Direction               textDirection,
                                                         bool                          ignoreHorizontalAlignment,
                                                         Pixel::Format                 pixelFormat,
                                                         const Vector2&                originSize)
{
  TextVisual& visualObject = GetVisualObject(visual);
  visualObject.mTypesetter->SetModel(visualObject.mController->GetRenderTextModel());
  if(visualObject.mController->HasValidReplacementSource())
  {
    visualObject.mTypesetter->SetFinalElisionResult(visualObject.mController->GetFinalElisionResult());
  }
  return visualObject.mTypesetter->RenderTextGradientPreserved(size, textDirection, ignoreHorizontalAlignment, pixelFormat, originSize);
}

PixelData TextVisual::RenderMarqueeTextGradientMask(Ui::Integration::Visual::Base visual,
                                                    const Vector2&                size,
                                                    Text::Direction               textDirection,
                                                    bool                          ignoreHorizontalAlignment,
                                                    Pixel::Format                 pixelFormat,
                                                    const Vector2&                originSize)
{
  TextVisual& visualObject = GetVisualObject(visual);
  visualObject.mTypesetter->SetModel(visualObject.mController->GetRenderTextModel());
  if(visualObject.mController->HasValidReplacementSource())
  {
    visualObject.mTypesetter->SetFinalElisionResult(visualObject.mController->GetFinalElisionResult());
  }
  return visualObject.mTypesetter->RenderTextGradientMask(size, textDirection, ignoreHorizontalAlignment, pixelFormat, originSize);
}

void TextVisual::OnSetTransform()
{
  UpdateRenderer();
}

void TextVisual::DoSetProperty(Dali::Property::Index index, const Dali::Property::Value& propertyValue)
{
  switch(index)
  {
    case Ui::TextVisualPropertyIndex::TEXT:
    {
      mController->SetText(ToStdString(propertyValue));
      break;
    }
    case Ui::TextVisualPropertyIndex::FONT_FAMILY:
    {
      SetFontFamilyProperty(mController, propertyValue);
      break;
    }
    case Ui::TextVisualPropertyIndex::FONT_SIZE:
    {
      const float fontSize = propertyValue.Get<float>();
      if(!Equals(mController->GetDefaultFontSize(Text::Controller::PIXEL_SIZE), fontSize))
      {
        mController->SetDefaultFontSize(fontSize, Text::Controller::PIXEL_SIZE);
      }
      break;
    }
    case Ui::TextVisualPropertyIndex::FONT_WEIGHT:
    {
      Text::FontWeight weight(static_cast<Text::FontWeight>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Text::GetFontWeightEnumeration(propertyValue, weight))
      {
        mController->SetDefaultFontWeight(Text::ToTextAbstractionFontWeight(weight));
      }
      break;
    }
    case Ui::TextVisualPropertyIndex::FONT_WIDTH:
    {
      Text::FontWidth width(static_cast<Text::FontWidth>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Text::GetFontWidthEnumeration(propertyValue, width))
      {
        mController->SetDefaultFontWidth(Text::ToTextAbstractionFontWidth(width));
      }
      break;
    }
    case Ui::TextVisualPropertyIndex::FONT_SLANT:
    {
      Text::FontSlant slant(static_cast<Text::FontSlant>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Text::GetFontSlantEnumeration(propertyValue, slant))
      {
        mController->SetDefaultFontSlant(Text::ToTextAbstractionFontSlant(slant));
      }
      break;
    }
    case Ui::TextVisualPropertyIndex::MULTI_LINE:
    {
      mController->SetMultiLineEnabled(propertyValue.Get<bool>());
      break;
    }
    case Ui::TextVisualPropertyIndex::LINE_WRAP_MODE:
    {
      Text::LineWrapMode mode(static_cast<Text::LineWrapMode>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Text::GetLineWrapModeEnumeration(propertyValue, mode))
      {
        mController->SetLineWrapMode(mode);
      }
      break;
    }
    case Ui::TextVisualPropertyIndex::HORIZONTAL_ALIGNMENT:
    {
      if(mController)
      {
        Text::Alignment alignment(static_cast<Text::Alignment>(-1)); // Set to invalid value to ensure a valid mode does get set
        if(Ui::Text::GetHorizontalAlignmentEnumeration(propertyValue, alignment))
        {
          mController->SetHorizontalAlignment(alignment);
        }
      }
      break;
    }
    case Ui::TextVisualPropertyIndex::VERTICAL_ALIGNMENT:
    {
      if(mController)
      {
        Text::Alignment alignment(static_cast<Text::Alignment>(-1)); // Set to invalid value to ensure a valid mode does get set
        if(Ui::Text::GetVerticalAlignmentEnumeration(propertyValue, alignment))
        {
          mController->SetVerticalAlignment(alignment);
        }
      }
      break;
    }
    case Ui::TextVisualPropertyIndex::OVERFLOW_MODE:
    {
      Text::OverflowMode mode(static_cast<Text::OverflowMode>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Text::GetOverflowModeEnumeration(propertyValue, mode))
      {
        if(mOverflowMode != mode)
        {
          mOverflowMode = mode;
          switch(mode)
          {
            case Text::OverflowMode::CLIP:
            {
              mController->SetTextElideEnabled(false);
              break;
            }
            case Text::OverflowMode::ELLIPSIS:
            {
              mController->SetTextElideEnabled(true);
              break;
            }
          }
        }
      }
      break;
    }
    case Ui::TextVisualPropertyIndex::LINE_HEIGHT:
    {
      float lineHeight = 0.0f;
      if(propertyValue.Get(lineHeight))
      {
        if(!Dali::Equals(mLineHeight, lineHeight))
        {
          mLineHeight = lineHeight;
          UpdateLineHeight();
        }
      }
      break;
    }
    case Ui::TextVisualPropertyIndex::LINE_HEIGHT_MODE:
    {
      Text::LineHeightMode mode(static_cast<Text::LineHeightMode>(-1)); // Set to invalid value to ensure a valid value does get set
      if(Text::GetLineHeightModeEnumeration(propertyValue, mode))
      {
        if(mLineHeightMode != mode)
        {
          mLineHeightMode = mode;
          UpdateLineHeight();
        }
      }
      break;
    }
    case Ui::TextVisualPropertyIndex::TEXT_COLOR:
    {
      const Vector4& textColor = propertyValue.Get<Vector4>();
      if(mController->GetDefaultColor() != textColor)
      {
        mController->SetDefaultColor(textColor);
      }
      break;
    }
  }
}

void TextVisual::UpdateLineHeight()
{
  if(Equals(mLineHeight, Text::LINE_HEIGHT_AUTO, Math::MACHINE_EPSILON_1000))
  {
    // clear explicit line height and use the natural line height.
    mController->SetRelativeLineSize(-1.0f);
    mController->SetDefaultLineSize(0.0f);
  }
  else if(mLineHeightMode == Text::LineHeightMode::RELATIVE)
  {
    mController->SetDefaultLineSize(0.0f);
    mController->SetRelativeLineSize(mLineHeight);
  }
  else // LineHeightMode::ABSOLUTE
  {
    mController->SetRelativeLineSize(-1.0f);
    mController->SetDefaultLineSize(mLineHeight);
  }
}

void TextVisual::UpdateRenderer()
{
  if(mController->IsAsyncRendering())
  {
    return;
  }

  Actor control = mControl.GetHandle();
  if(!control)
  {
    // Nothing to do.
    return;
  }

  // Calculates the size to be used to relayout.
  Vector2 relayoutSize = mImpl->GetTransformVisualSize(mImpl->mControlSize);
  // TODO : Round the size and offset to avoid pixel alignment issues.

  auto textLengthUtf32 = mController->GetNumberOfCharacters();

  if((fabsf(relayoutSize.width) < Math::MACHINE_EPSILON_1000) ||
     (fabsf(relayoutSize.height) < Math::MACHINE_EPSILON_1000) || textLengthUtf32 == 0u)
  {
    // Remove the texture set and any renderer previously set.
    RemoveRenderer(control, true);

    // Nothing else to do if the relayout size is zero.
    ResourceReady(Ui::Visual::ResourceStatus::READY);
    return;
  }

  Vector2 layoutConstraintSize = relayoutSize;
  if(mController->GetFinalElisionResult())
  {
    // Keep replacement relayout constrained by the control area that produced
    // the final result, not by the reduced render-texture height.
    layoutConstraintSize = mController->GetRenderTextModel()->GetControlSize();
  }

  Dali::LayoutDirection::Type layoutDirection = mController->GetLayoutDirection(control);

  const Text::Controller::UpdateTextType updateTextType = mController->Relayout(layoutConstraintSize, layoutDirection);
  mTypesetter->SetModel(mController->GetRenderTextModel());
  if(mController->HasValidReplacementSource())
  {
    mTypesetter->SetFinalElisionResult(mController->GetFinalElisionResult());
  }

  const bool textModelUpdated =
    Text::Controller::NONE_UPDATED != (Text::Controller::MODEL_UPDATED & updateTextType);
  if(textModelUpdated || mRendererUpdateNeeded)
  {
    bool hasPublishedTextResource =
      GetResourceStatus() == Ui::Visual::ResourceStatus::READY && !mRendererList.empty();
    for(const Renderer& renderer : mRendererList)
    {
      hasPublishedTextResource =
        hasPublishedTextResource && renderer && renderer.GetTextures().GetTextureCount() > 0u;
    }

    // Fitting must finalize controller layout, but may reapply an already-published
    // result. Reuse it only when no renderer or transform invalidation is pending.
    const bool reusePublishedFittingResource =
      textModelUpdated &&
      !mRendererUpdateNeeded &&
      mApplyingFittingMode &&
      IsTransformMapSetForFittingMode() &&
      !mImpl->mTransformMapChanged &&
      hasPublishedTextResource;

    mRendererUpdateNeeded = false;

    if(reusePublishedFittingResource)
    {
      ResourceReady(Ui::Visual::ResourceStatus::READY);
      return;
    }

    // Remove the texture set and any renderer previously set.
    // Note, we don't need to remove the mImpl->Renderer, since it will be added again after AddRenderer call.
    RemoveRenderer(control, false);

    if((relayoutSize.width > Math::MACHINE_EPSILON_1000) && (relayoutSize.height > Math::MACHINE_EPSILON_1000))
    {
      const Text::ModelInterface* const renderModel = mController->GetRenderTextModel();

      // Check whether it is a markup text with multiple text colors
      const Vector4* const          colorsBuffer = renderModel->GetColors();
      const Text::ColorIndex* const colorIndices = renderModel->GetColorIndices();

      TextAbstraction::FontClient  fontClient            = TextAbstraction::FontClient::Get();
      const Text::GlyphInfo* const glyphsBuffer          = renderModel->GetGlyphs();
      const Text::Length           numberOfGlyphs        = renderModel->GetNumberOfGlyphs();
      const bool                   hasColorIndexBuffer   = nullptr != colorsBuffer && nullptr != colorIndices;
      bool                         hasMultipleTextColors = false;
      bool                         containsColorGlyph    = false;
      for(Text::Length glyphIndex = 0; glyphIndex < numberOfGlyphs; glyphIndex++)
      {
        if(hasColorIndexBuffer && *(colorIndices + glyphIndex) > 0u)
        {
          hasMultipleTextColors = true;
        }

        if(!containsColorGlyph)
        {
          const Text::GlyphInfo* const glyphInfo = glyphsBuffer + glyphIndex;
          if(Text::Internal::IsRenderableColorGlyph(fontClient, glyphInfo->fontId, glyphInfo->index))
          {
            containsColorGlyph = true;
          }
        }

        if(hasMultipleTextColors && containsColorGlyph)
        {
          break;
        }
      }

      // Check whether the text contains any style colors (e.g. underline color, shadow color, etc.)

      const bool shadowEnabled = renderModel->IsShadowEnabled();

      const bool outlineEnabled    = renderModel->IsOutlineEnabled();
      const bool backgroundEnabled = renderModel->IsBackgroundEnabled();
      // Legacy "Markup" accessors also report range decoration runs produced by StyledText spans.
      const bool underlineRunEnabled         = renderModel->IsMarkupUnderlineSet();
      const bool strikethroughRunEnabled     = renderModel->IsMarkupStrikethroughSet();
      const bool underlineEnabled            = renderModel->IsUnderlineEnabled() || underlineRunEnabled;
      const bool strikethroughEnabled        = renderModel->IsStrikethroughEnabled() || strikethroughRunEnabled;
      const bool backgroundMarkupSet         = renderModel->IsMarkupBackgroundColorSet();
      const bool cutoutEnabled               = mController->IsTextCutout();
      const bool backgroundWithCutoutEnabled = renderModel->IsBackgroundWithCutoutEnabled();
      const bool styleTextureEnabled         = shadowEnabled || outlineEnabled || backgroundEnabled || backgroundMarkupSet;
      const bool styleBlocksTextGradient     = cutoutEnabled || backgroundWithCutoutEnabled;
      const bool styleEnabled                = styleTextureEnabled || styleBlocksTextGradient;
      const bool isOverlayStyle              = underlineEnabled || strikethroughEnabled;
      const bool embossEnabled               = mController->IsEmbossEnabled();

      // if background with cutout is enabled, This text visual must render the entire control size.

      if(cutoutEnabled)
      {
        if(mImpl->mTransformMapUsingDefault)
        {
          relayoutSize = mImpl->mControlSize;
          mController->SetOffsetWithCutout(Vector2::ZERO);
        }
        else
        {
          // mTransform stores the size and offset of the current visual.
          // padding and alignment information is stored in mOffset.
          // When Cutout Enabled, the current visual must draw the entire control.
          // so set the size to controlSize and offset to 0.

          relayoutSize = mImpl->mControlSize;

          // Note : TextVisual which is not created by Label should not support cutout feature!
          // We can assume that current transform is absolute offset / size which is effectScale applied already
          // and the visualTransform will be change for next layout (mean, we can reset it as initial value freely).

          auto& visualTransform = mImpl->GetOrCreateTransform();

          // Relayout to the original size has been completed, so save only the offset information and use it in
          // typesetter.
          // Note : We reset OffsetWithCutout whenever visual trasnform updated. So we need to "append" the offset
          // if UpdateRenderer() called without visual transform changed.
          const Vector2& previousCutoutOffset = renderModel->GetOffsetWithCutout();
          mController->SetOffsetWithCutout(visualTransform.mOffset + previousCutoutOffset);

          visualTransform.SetPropertyMap(Property::Map());
        }
      }

      AddRenderer(control, relayoutSize, hasMultipleTextColors, containsColorGlyph, styleEnabled,
                  styleTextureEnabled, styleBlocksTextGradient, isOverlayStyle, embossEnabled);

      // Text rendered and ready to display
      ResourceReady(Ui::Visual::ResourceStatus::READY);
    }
  }
}

void TextVisual::AddTexture(TextureSet& textureSet, PixelData& data, Sampler& sampler, unsigned int textureSetIndex)
{
  Texture texture =
    Texture::New(Dali::TextureType::TEXTURE_2D, data.GetPixelFormat(), data.GetWidth(), data.GetHeight());
#if defined(GPU_MEMORY_PROFILE_ENABLED)
  {
    std::string text;
    mController->GetText(text);
    Dali::Integration::TextureUploadWithContent(texture, data, ToDaliString(std::move(text)), Dali::Integration::TextureContextTypeHint::TEXT_SIMPLE_LABEL);
  }
#else
  texture.Upload(data);
#endif

  textureSet.SetTexture(textureSetIndex, texture);
  textureSet.SetSampler(textureSetIndex, sampler);
}

void TextVisual::AddTilingTexture(TextureSet& textureSet, TilingInfo& tilingInfo, PixelData& data, Sampler& sampler,
                                  unsigned int textureSetIndex)
{
  Texture texture =
    Texture::New(Dali::TextureType::TEXTURE_2D, data.GetPixelFormat(), tilingInfo.width, tilingInfo.height);
  DevelTexture::UploadSubPixelData(texture, data, 0u, tilingInfo.offsetHeight, tilingInfo.width, tilingInfo.height);

  textureSet.SetTexture(textureSetIndex, texture);
  textureSet.SetSampler(textureSetIndex, sampler);
}

void TextVisual::CreateTextureSet(TilingInfo& info, VisualRenderer& renderer, Sampler& sampler)
{
  TextureSet textureSet      = TextureSet::New();
  uint32_t   textureSetIndex = 0u;

  // Convert the buffer to pixel data to make it a texture.

  if(info.textPixelData)
  {
    AddTilingTexture(textureSet, info, info.textPixelData, sampler, textureSetIndex);
    ++textureSetIndex;
  }

  if(mTextShaderFeatureCache.IsEnabledStyle() && info.stylePixelData)
  {
    AddTilingTexture(textureSet, info, info.stylePixelData, sampler, textureSetIndex);
    ++textureSetIndex;
  }

  if(mTextShaderFeatureCache.IsEnabledOverlay() && info.overlayStylePixelData)
  {
    AddTilingTexture(textureSet, info, info.overlayStylePixelData, sampler, textureSetIndex);
    ++textureSetIndex;
  }

  if(mTextShaderFeatureCache.IsEnabledEmoji() && !mTextShaderFeatureCache.IsEnabledMultiColor() && info.maskPixelData)
  {
    AddTilingTexture(textureSet, info, info.maskPixelData, sampler, textureSetIndex);
    ++textureSetIndex;
  }

  if(mTextShaderFeatureCache.IsEnabledTextReveal())
  {
    // Reveal metadata is an all-or-nothing renderer input. Async publication
    // validates the complete tile set; synchronous rendering creates each tile
    // immediately before this call.
    DALI_ASSERT_ALWAYS(info.revealPixelData && info.textPixelData &&
                       info.revealPixelData.GetPixelFormat() == Pixel::RGBA8888 &&
                       info.revealPixelData.GetWidth() == static_cast<uint32_t>(info.width) &&
                       info.revealPixelData.GetHeight() == static_cast<uint32_t>(info.height));
    Sampler nearestSampler = Sampler::New();
    nearestSampler.SetFilterMode(FilterMode::NEAREST, FilterMode::NEAREST);
    AddTexture(textureSet, info.revealPixelData, nearestSampler, textureSetIndex);
  }

  renderer.SetTextures(textureSet);

  // Enable the pre-multiplied alpha to improve the text quality
  renderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, true);

  // Set size and offset for the tiling.
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_SIZE,
                       Vector2(static_cast<float>(info.width), static_cast<float>(info.height)));
  renderer.SetProperty(VisualRenderer::Property::TRANSFORM_OFFSET, info.transformOffset);
  renderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);
  renderer.RegisterProperty("uHasMultipleTextColors",
                            static_cast<float>(mTextShaderFeatureCache.IsEnabledMultiColor()));

  mRendererList.push_back(renderer);
}

// From async text manager
void TextVisual::LoadComplete(bool loadingSuccess, const TextInformation& textInformation)
{
  Text::AsyncTextParameters parameters = textInformation.parameters;

#ifdef TRACE_ENABLED
  if(gTraceFilter2 && gTraceFilter2->IsTraceEnabled())
  {
    DALI_LOG_RELEASE_INFO("LoadComplete, success:%d, type:%s\n", loadingSuccess,
                          GetRequestTypeName(parameters.requestType));
  }
#endif

  const bool isRenderRequest = IsAsyncRenderRequest(parameters.requestType);
  if(isRenderRequest && !mController->IsAsyncRendering())
  {
    // A render that completed after switching back to the synchronous path is
    // no longer allowed to replace that path's renderer.
    mIsTextLoadingTaskRunning = false;
    return;
  }
  auto* revealData = GetTextVisualRevealData(mRevealData);
  if(isRenderRequest && parameters.textRevealRevision != (revealData ? revealData->revision : 0u))
  {
    // Do not clear the running flag here: a newer request may already own it.
    return;
  }
  if(isRenderRequest && parameters.isTextRevealEnabled &&
     (mController->IsMarqueeEnabled() || mController->IsTextCutout()))
  {
    // Marquee and cutout do not consume Reveal metadata. A result requested
    // before either mode was enabled must not replace their current renderer.
    return;
  }

  switch(parameters.requestType)
  {
    case Ui::Integration::Text::Async::RENDER_FIXED_SIZE:
    case Ui::Integration::Text::Async::RENDER_FIXED_WIDTH:
    case Ui::Integration::Text::Async::RENDER_FIXED_HEIGHT:
    case Ui::Integration::Text::Async::RENDER_CONSTRAINT:
    {
      mIsTextLoadingTaskRunning = false;
      break;
    }
    case Ui::Integration::Text::Async::COMPUTE_NATURAL_SIZE:
    {
      mIsNaturalSizeTaskRunning = false;
      break;
    }
    case Ui::Integration::Text::Async::COMPUTE_HEIGHT_FOR_WIDTH:
    {
      mIsHeightForWidthTaskRunning = false;
      break;
    }
    default:
    {
      DALI_LOG_ERROR("Unexpected request type : %d\n", parameters.requestType);
      break;
    }
  }

  Ui::Visual::ResourceStatus resourceStatus;

  if(loadingSuccess)
  {
    resourceStatus = Ui::Visual::ResourceStatus::READY;

    Text::AsyncTextRenderInfo renderInfo = textInformation.renderInfo;

    if(parameters.requestType == Ui::Integration::Text::Async::COMPUTE_NATURAL_SIZE ||
       parameters.requestType == Ui::Integration::Text::Async::COMPUTE_HEIGHT_FOR_WIDTH)
    {
      if(mAsyncTextInterface)
      {
        mAsyncTextInterface->AsyncSizeComputed(renderInfo);
        return;
      }
    }

    Actor control = mControl.GetHandle();
    if(!control)
    {
      // Nothing to do.
      ResourceReady(Ui::Visual::ResourceStatus::READY);
      return;
    }

    // Calculate the size of the visual that can fit the text.
    // The size of the text after it has been laid-out, size of pixel data buffer.
    Size layoutSize = renderInfo.size;

    // Set textWidth, textHeight to the original size requested for rendering.
    bool isRenderScale = parameters.renderScale > 1.0f ? true : false;
    if(isRenderScale)
    {
      parameters.textWidth  = parameters.renderScaleWidth;
      parameters.textHeight = parameters.renderScaleHeight;
    }

    // Size of the text control including padding.
    Vector2 textControlSize(parameters.textWidth + (parameters.padding.start + parameters.padding.end),
                            parameters.textHeight + (parameters.padding.top + parameters.padding.bottom));

    bool isVerticalScroll = false;
    if(parameters.isMarqueeEnabled)
    {
      // In case of marquee, the layout width (renderInfo's width) is the natural size of the text.
      // Since the layout size is the size of the visual transform, it should be reset to the text area excluding
      // padding.
      if(parameters.marqueeOrientation == Text::MarqueeOrientation::HORIZONTAL)
      {
        layoutSize.width = parameters.textWidth;
      }
      else
      {
        layoutSize.height = parameters.textHeight;
        isVerticalScroll  = true;
      }
    }

    // Calculate the offset for vertical alignment only, as the layout engine will do the horizontal alignment.
    Vector2 alignmentOffset;
    alignmentOffset.x = 0.0f;
    alignmentOffset.y = isVerticalScroll ? 0.0f
                                         : (parameters.textHeight - layoutSize.y) *
                                             VERTICAL_ALIGNMENT_TABLE[static_cast<int>(parameters.verticalAlignment)];

    Vector2 visualTransformOffset;
    if(renderInfo.isCutoutEnabled)
    {
      // When Cutout Enabled, the current visual must draw the entire control.
      // so set the size to controlSize and offset to 0.
      visualTransformOffset.x = 0.0f;
      visualTransformOffset.y = 0.0f;

      // The layout size is set to the text control size including padding.
      layoutSize = textControlSize;
    }
    else
    {
      // This affects font rendering quality.
      // It need to be integerized.
      visualTransformOffset.x = roundf(parameters.padding.start + alignmentOffset.x);
      visualTransformOffset.y =
        isRenderScale
          ? roundf((layoutSize.y + parameters.padding.top + alignmentOffset.y) * 2.0f) * 0.5f - layoutSize.y
          : roundf(parameters.padding.top + alignmentOffset.y);
    }

    SetRequireRender(renderInfo.isCutoutEnabled);

    // Mark that we don't use viewEffectiveScale at transform's size & offset for this visual.
    // (Because visual transform size and polic already apply viewEffectiveScale).
    SetTransformMapUsageForFittingMode(true);

    // Reset cutout offset before call SetTransformAndSize.
    // (cutout offset will be set at OnSetTransform)
    if(mController->IsTextCutout())
    {
      mController->SetOffsetWithCutout(Vector2::ZERO);
    }

    // Transform offset is used for subpixel data upload in text tiling.
    // We should set the transform before creating a tiling texture.
    Property::Map visualTransform;
    visualTransform.Add(Ui::Visual::Transform::Property::SIZE, layoutSize)
      .Add(Ui::Visual::Transform::Property::SIZE_POLICY,
           Vector2(Ui::Visual::Transform::Policy::ABSOLUTE, Ui::Visual::Transform::Policy::ABSOLUTE))
      .Add(Ui::Visual::Transform::Property::OFFSET, visualTransformOffset)
      .Add(Ui::Visual::Transform::Property::OFFSET_POLICY,
           Vector2(Ui::Visual::Transform::Policy::ABSOLUTE, Ui::Visual::Transform::Policy::ABSOLUTE))
      .Add(Ui::Visual::Transform::Property::ORIGIN, Ui::Align::TOP_BEGIN)
      .Add(Ui::Visual::Transform::Property::PIVOT, Ui::Align::TOP_BEGIN);
    SetTransformAndSize(visualTransform, textControlSize, parameters.effectiveTextScale);

    // Get the maximum texture size.
    const int  maxTextureSize   = Dali::GetMaxTextureSize();
    const bool isHeightTiling   = renderInfo.size.height > static_cast<float>(maxTextureSize);
    const bool isCutoutEnabled  = renderInfo.isCutoutEnabled || parameters.isCutoutEnabled;
    const bool isMarqueeEnabled = parameters.isMarqueeEnabled;
    const bool textGradientStyleBlocksComposition =
      renderInfo.styleBlocksTextGradient ||
      (isMarqueeEnabled && (renderInfo.styleTextureEnabled || renderInfo.isOverlayStyle));
    const bool textGradientCompositionEnabled =
      IsTextGradientCompositionSupported(renderInfo.size, renderInfo.hasMultipleTextColors,
                                         renderInfo.containsColorGlyph, textGradientStyleBlocksComposition,
                                         renderInfo.isOverlayStyle, renderInfo.isEmbossEnabled, isHeightTiling,
                                         isMarqueeEnabled, isCutoutEnabled);
    const bool textGradientMixedPixelDataAvailable =
      renderInfo.textGradientPreservedPixelData && renderInfo.textGradientMaskPixelData;
    const bool textGradientMixedCompositionEnabled =
      IsTextGradientMixedCompositionSupported(renderInfo.size, renderInfo.hasMultipleTextColors,
                                              renderInfo.containsColorGlyph, textGradientStyleBlocksComposition,
                                              renderInfo.styleTextureEnabled, renderInfo.isOverlayStyle,
                                              renderInfo.isEmbossEnabled, isHeightTiling,
                                              isMarqueeEnabled, isCutoutEnabled) &&
      textGradientMixedPixelDataAvailable;
    const bool textGradientOverlayCompositionEnabled =
      IsTextGradientOverlayCompositionSupported(renderInfo.size, isHeightTiling, isMarqueeEnabled, isCutoutEnabled);
    const bool featureStyleEnabled =
      (textGradientCompositionEnabled || textGradientMixedCompositionEnabled) ? renderInfo.styleTextureEnabled
                                                                              : renderInfo.styleEnabled;
    const bool textRevealEnabled =
      revealData && revealData->unit != Text::Internal::Reveal::Unit::DISABLED &&
      HasCompleteTextRevealMetadata(renderInfo, maxTextureSize) &&
      !isMarqueeEnabled && !isCutoutEnabled;
    if(revealData)
    {
      revealData->fadeDuration = textRevealEnabled ? renderInfo.textRevealFadeDuration : 0.0f;
    }

    TextVisualShaderFeature::FeatureBuilder featureBuilder;
    featureBuilder.EnableMultiColor(renderInfo.hasMultipleTextColors)
      .EnableEmoji(renderInfo.containsColorGlyph)
      .EnableStyle(featureStyleEnabled)
      .EnableOverlay(renderInfo.isOverlayStyle)
      .EnableEmboss(renderInfo.isEmbossEnabled)
      .EnableTextGradient(textGradientCompositionEnabled)
      .EnableTextGradientMixed(textGradientMixedCompositionEnabled)
      .EnableTextGradientOverlay(textGradientOverlayCompositionEnabled)
      .EnableTextReveal(textRevealEnabled);

    Shader shader = GetTextShader(mFactoryCache, featureBuilder);
    mImpl->mRenderer.SetShader(shader);

    // Remove the texture set and any renderer previously set.
    RemoveRenderer(control, false);

    // No tiling required. Use the default renderer.
    if(renderInfo.size.height <= static_cast<float>(maxTextureSize))
    {
      // Filter mode needs to be set to linear to produce better quality while scaling.
      Sampler sampler = Sampler::New();
      sampler.SetFilterMode(FilterMode::LINEAR, FilterMode::LINEAR);

      TextureSet textureSet = TextureSet::New();

      uint32_t textureSetIndex = 0u;
      if(mTextShaderFeatureCache.IsEnabledTextGradientMixed())
      {
        auto& gradientData = GetOrCreateTextVisualGradientData(mGradientData);
        AddTexture(textureSet, renderInfo.textGradientPreservedPixelData, sampler, textureSetIndex);
        ++textureSetIndex;
        gradientData.mTextGradientMaskPixelData = renderInfo.textGradientMaskPixelData;
        AddTexture(textureSet, renderInfo.textGradientMaskPixelData, sampler, textureSetIndex);
        ++textureSetIndex;
        Text::Internal::Gradient::AddLookupTexture(textureSet, textureSetIndex, gradientData.mTextGradientStyle);
        if(mTextShaderFeatureCache.IsEnabledTextGradientOverlay())
        {
          Text::Internal::Gradient::AddLookupTexture(textureSet, textureSetIndex, gradientData.mTextGradientOverlayStyle);
        }
      }
      else
      {
        AddTexture(textureSet, renderInfo.textPixelData, sampler, textureSetIndex);
        ++textureSetIndex;

        if(mTextShaderFeatureCache.IsEnabledTextGradient())
        {
          auto& gradientData = GetOrCreateTextVisualGradientData(mGradientData);
          Text::Internal::Gradient::AddLookupTexture(textureSet, textureSetIndex, gradientData.mTextGradientStyle);
        }
        if(mTextShaderFeatureCache.IsEnabledTextGradientOverlay())
        {
          auto& gradientData = GetOrCreateTextVisualGradientData(mGradientData);
          Text::Internal::Gradient::AddLookupTexture(textureSet, textureSetIndex, gradientData.mTextGradientOverlayStyle);
        }
      }

      if(mTextShaderFeatureCache.IsEnabledStyle())
      {
        // Create RGBA texture for all the text styles that render in the background (without the text itself)
        AddTexture(textureSet, renderInfo.stylePixelData, sampler, textureSetIndex);
        ++textureSetIndex;
      }
      if(mTextShaderFeatureCache.IsEnabledOverlay())
      {
        // Create RGBA texture for overlay styles such as underline and strikethrough (without the text itself)
        AddTexture(textureSet, renderInfo.overlayStylePixelData, sampler, textureSetIndex);
        ++textureSetIndex;
      }

      if(mTextShaderFeatureCache.IsEnabledEmoji() &&
         !mTextShaderFeatureCache.IsEnabledMultiColor() &&
         !mTextShaderFeatureCache.IsEnabledAnyTextGradient())
      {
        // Create a L8 texture as a mask to avoid color glyphs (e.g. emojis) to be affected by text color animation
        AddTexture(textureSet, renderInfo.maskPixelData, sampler, textureSetIndex);
        ++textureSetIndex;
      }

      if(textRevealEnabled)
      {
        Sampler nearestSampler = Sampler::New();
        nearestSampler.SetFilterMode(FilterMode::NEAREST, FilterMode::NEAREST);
        AddTexture(textureSet, renderInfo.revealMetadataTiles.front(), nearestSampler, textureSetIndex);
      }

      mImpl->mRenderer.SetTextures(textureSet);
      mImpl->mRenderer.SetProperty(mHasMultipleTextColorsIndex,
                                   static_cast<float>(mTextShaderFeatureCache.IsEnabledMultiColor()));
      mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);

      if(mTextShaderFeatureCache.IsEnabledAnyTextGradient() || mTextShaderFeatureCache.IsEnabledTextGradientOverlay())
      {
        if(mTextShaderFeatureCache.IsEnabledAnyTextGradient())
        {
          const Vector4 textBounds = ResolveTextGradientBounds(renderInfo.size, renderInfo.textLogicalBounds);
          ApplyTextGradientUniforms(mImpl->mRenderer, renderInfo.size, textBounds);
        }
        if(mTextShaderFeatureCache.IsEnabledTextGradientOverlay())
        {
          const Vector4 overlayBounds = ResolveTextGradientOverlayBounds(renderInfo.size, renderInfo.textLogicalBounds);
          ApplyTextGradientOverlayUniforms(mImpl->mRenderer, renderInfo.size, overlayBounds);
        }
      }

      if(textRevealEnabled)
      {
        BindTextRevealConstraint(mImpl->mRenderer);
      }

      mRendererList.push_back(mImpl->mRenderer);
    }
    else
    {
      // Filter mode needs to be set to linear to produce better quality while scaling.
      Sampler sampler = Sampler::New();
      sampler.SetFilterMode(FilterMode::LINEAR, FilterMode::LINEAR);

      int verifiedWidth  = static_cast<int>(renderInfo.size.width);
      int verifiedHeight = static_cast<int>(renderInfo.size.height);

      // Set information for creating textures.
      TilingInfo info(verifiedWidth, maxTextureSize);

      // Get the pixel data of text.
      info.textPixelData = renderInfo.textPixelData;

      if(mTextShaderFeatureCache.IsEnabledStyle())
      {
        info.stylePixelData = renderInfo.stylePixelData;
      }

      if(mTextShaderFeatureCache.IsEnabledOverlay())
      {
        info.overlayStylePixelData = renderInfo.overlayStylePixelData;
      }

      if(mTextShaderFeatureCache.IsEnabledEmoji() && !mTextShaderFeatureCache.IsEnabledMultiColor())
      {
        info.maskPixelData = renderInfo.maskPixelData;
      }

      size_t revealTileIndex = 0u;
      if(textRevealEnabled)
      {
        info.revealPixelData = renderInfo.revealMetadataTiles[revealTileIndex++];
      }

      // Get the current offset for recalculate the offset when tiling.
      Property::Map retMap;
      if(mImpl->mTransform)
      {
        mImpl->mTransform->GetPropertyMap(retMap);
        Property::Value* offsetValue = retMap.Find(Dali::Ui::Visual::Transform::Property::OFFSET);
        if(offsetValue)
        {
          offsetValue->Get(info.transformOffset);
        }
      }

      // Create a textureset in the default renderer.
      CreateTextureSet(info, mImpl->mRenderer, sampler);
      if(textRevealEnabled)
      {
        BindTextRevealConstraint(mImpl->mRenderer);
      }

      verifiedHeight -= maxTextureSize;

      Geometry geometry = mFactoryCache.GetGeometry(VisualFactoryCache::QUAD_GEOMETRY);

      // Create a renderer by cutting maxTextureSize.
      while(verifiedHeight > 0)
      {
        VisualRenderer tilingRenderer = VisualRenderer::New(geometry, shader);
        tilingRenderer.SetProperty(Dali::Renderer::Property::DEPTH_INDEX, Ui::Integration::DepthIndex::CONTENT);
        // New offset position of buffer for tiling.
        info.offsetHeight += static_cast<uint32_t>(maxTextureSize);
        // New height for tiling.
        info.height = (verifiedHeight - maxTextureSize) > 0 ? maxTextureSize : verifiedHeight;
        // New offset for tiling.
        info.transformOffset.y += static_cast<float>(maxTextureSize);

        if(textRevealEnabled)
        {
          info.revealPixelData = renderInfo.revealMetadataTiles[revealTileIndex++];
        }

        // Create a textureset int the new tiling renderer.
        CreateTextureSet(info, tilingRenderer, sampler);
        if(textRevealEnabled)
        {
          BindTextRevealConstraint(tilingRenderer);
        }

        verifiedHeight -= maxTextureSize;
      }
    }

    const Vector4& defaultColor = parameters.textColor;

    for(RendererContainer::iterator iter = mRendererList.begin(); iter != mRendererList.end(); ++iter)
    {
      VisualRenderer renderer = (*iter);
      if(renderer)
      {
        // Register transform properties
        mImpl->SetTransformUniforms(renderer, static_cast<Ui::Integration::Direction::Type>(Text::Direction::LEFT_TO_RIGHT));

        control.AddRenderer(renderer);

        if(renderInfo.isEmbossEnabled)
        {
          float          sizeX             = std::max(layoutSize.x, Math::MACHINE_EPSILON_100);
          float          sizeY             = std::max(std::min((float)maxTextureSize, layoutSize.y), Math::MACHINE_EPSILON_100);
          const Vector2& embossSize        = Vector2(1.0f / sizeX, 1.0f / sizeY);
          const Vector2& embossDirection   = parameters.embossDirection;
          const float    embossStrength    = parameters.embossStrength;
          const Vector4& embossLightColor  = parameters.embossLightColor;
          const Vector4& embossShadowColor = parameters.embossShadowColor;

          renderer.RegisterProperty("uEmbossSize", embossSize);
          renderer.RegisterProperty("uEmbossDirection", embossDirection);
          renderer.RegisterProperty("uEmbossStrength", embossStrength);
          renderer.RegisterProperty("uEmbossLightColor", embossLightColor);
          renderer.RegisterProperty("uEmbossShadowColor", embossShadowColor);
        }

        if(renderer != mImpl->mRenderer)
        {
          // Set constraint for text label's color for non-default renderers.
          if(mAnimatableTextColorPropertyIndex != Property::INVALID_INDEX)
          {
            // Register unique property, or get property for default renderer.
            Property::Index index = renderer.RegisterUniqueProperty("uTextColorAnimatable", defaultColor);

            // Create constraint for the animatable text's color Property with uTextColorAnimatable in the renderer.
            if(index != Property::INVALID_INDEX)
            {
              Constraint colorConstraint = Constraint::New<Vector4>(renderer, index, TextColorConstraint);
              colorConstraint.AddSource(Source(control, mAnimatableTextColorPropertyIndex));
              colorConstraint.SetApplyRate(mIsConstraintAppliedAlways ? Dali::Constraint::APPLY_ALWAYS
                                                                      : Dali::Constraint::APPLY_ONCE);
              Dali::Integration::ConstraintSetInternalTag(colorConstraint, TEXT_VISUAL_COLOR_CONSTRAINT_TAG);
              colorConstraint.Apply();

              mColorConstraintList.push_back(colorConstraint);
            }

            // Make zero if the alpha value of text color is zero to skip rendering text
            // VisualRenderer::Property::OPACITY uses same animatable property internally.
            Constraint opacityConstraint =
              Constraint::New<float>(renderer, Dali::DevelRenderer::Property::OPACITY, OpacityConstraint);
            opacityConstraint.AddSource(Source(control, mAnimatableTextColorPropertyIndex));
            opacityConstraint.AddSource(Source(mImpl->mRenderer, mTextRequireRenderPropertyIndex));
            opacityConstraint.SetApplyRate(mIsConstraintAppliedAlways ? Dali::Constraint::APPLY_ALWAYS
                                                                      : Dali::Constraint::APPLY_ONCE);
            Dali::Integration::ConstraintSetInternalTag(opacityConstraint, TEXT_VISUAL_OPACITY_CONSTRAINT_TAG);
            opacityConstraint.Apply();

            mOpacityConstraintList.push_back(opacityConstraint);
          }
        }
      }
    }

    if(mAsyncTextInterface && parameters.isMarqueeEnabled)
    {
      mAsyncTextInterface->AsyncInitializeMarquee(renderInfo);
    }

    if(mAsyncTextInterface && parameters.isTextFitEnabled)
    {
      mAsyncTextInterface->AsyncTextFitChanged(parameters.fontSize);
    }

    if(mAsyncTextInterface)
    {
      mAsyncTextInterface->AsyncRenderFinished(std::move(renderInfo));
    }

    // A completion callback may request the next render. The result committed
    // above remains visible until that request publishes its own valid result.
    SetConstraintApplyAlways(mIsConstraintAppliedAlways, true);
    if(const auto* gradientData = GetTextVisualGradientData(mGradientData))
    {
      SetGradientAnimApplyAlways(gradientData->mGradientAnimApplyAlways, true);
      SetGradientOverlayAnimApplyAlways(gradientData->mGradientOverlayAnimApplyAlways, true);
    }
  }
  else
  {
    resourceStatus = Ui::Visual::ResourceStatus::FAILED;
  }

  // Signal to observers ( control ) that resources are ready. Must be all resources.
  ResourceReady(resourceStatus);
}

void TextVisual::SetAsyncTextInterface(Ui::Integration::Text::AsyncTextInterface* asyncTextInterface)
{
  mAsyncTextInterface = asyncTextInterface;
}

void TextVisual::SetTextGradientStyle(const Text::Internal::Gradient::Style& style)
{
  auto* gradientData = GetTextVisualGradientData(mGradientData);
  if(!gradientData && !Text::Internal::Gradient::IsRenderable(style))
  {
    return;
  }

  gradientData                             = &GetOrCreateTextVisualGradientData(mGradientData);
  gradientData->mTextGradientStyle         = style;
  gradientData->mTextGradientMaskPixelData = PixelData();
  gradientData->mGradientRenderer          = VisualRenderer();
  gradientData->mHasGradientContext        = false;
  mRendererUpdateNeeded                    = true;

  if(IsOnScene())
  {
    UpdateRenderer();
  }
}

void TextVisual::SetTextGradientBoundsMode(Text::GradientBoundsMode mode)
{
  auto* gradientData = GetTextVisualGradientData(mGradientData);
  if(!gradientData && mode == Text::GradientBoundsMode::CONTENT_BOUND)
  {
    return;
  }

  gradientData                          = &GetOrCreateTextVisualGradientData(mGradientData);
  gradientData->mTextGradientBoundsMode = mode;
  gradientData->mGradientRenderer       = VisualRenderer();
  gradientData->mHasGradientContext     = false;
  mRendererUpdateNeeded                 = true;

  if(IsOnScene())
  {
    UpdateRenderer();
  }
}

void TextVisual::SetTextGradientOverlayStyle(const Text::Internal::Gradient::Style& style)
{
  auto* gradientData = GetTextVisualGradientData(mGradientData);
  if(!gradientData && !Text::Internal::Gradient::IsRenderable(style))
  {
    return;
  }

  gradientData = &GetOrCreateTextVisualGradientData(mGradientData);
  RemoveGradientOverlayAnimConstraints();
  gradientData->mTextGradientOverlayStyle  = style;
  gradientData->mGradientOverlayRenderer   = VisualRenderer();
  gradientData->mHasGradientOverlayContext = false;
  mRendererUpdateNeeded                    = true;

  if(IsOnScene())
  {
    UpdateRenderer();
  }
}

void TextVisual::SetTextGradientOverlayBoundsMode(Text::GradientBoundsMode mode)
{
  auto* gradientData = GetTextVisualGradientData(mGradientData);
  if(!gradientData && mode == Text::GradientBoundsMode::CONTENT_BOUND)
  {
    return;
  }

  gradientData                                 = &GetOrCreateTextVisualGradientData(mGradientData);
  gradientData->mTextGradientOverlayBoundsMode = mode;
  gradientData->mGradientOverlayRenderer       = VisualRenderer();
  gradientData->mHasGradientOverlayContext     = false;
  mRendererUpdateNeeded                        = true;

  if(IsOnScene())
  {
    UpdateRenderer();
  }
}

void TextVisual::SetTextGradientOverlayMode(Text::GradientOverlayMode mode)
{
  auto* gradientData = GetTextVisualGradientData(mGradientData);
  if(!gradientData && mode == Text::GradientOverlayMode::SRC_OVER)
  {
    return;
  }

  gradientData                           = &GetOrCreateTextVisualGradientData(mGradientData);
  gradientData->mTextGradientOverlayMode = mode;
  mRendererUpdateNeeded                  = true;

  if(IsOnScene())
  {
    UpdateRenderer();
  }
}

PixelData TextVisual::GetTextGradientMaskPixelData() const
{
  const auto* gradientData = GetTextVisualGradientData(mGradientData);
  return gradientData ? gradientData->mTextGradientMaskPixelData : PixelData();
}

void TextVisual::SetGradientAnimProperties(Property::Index startOffsetPropertyIndex)
{
  auto* gradientData = GetTextVisualGradientData(mGradientData);
  if(!gradientData && startOffsetPropertyIndex == Property::INVALID_INDEX)
  {
    return;
  }

  gradientData       = &GetOrCreateTextVisualGradientData(mGradientData);
  const bool changed = gradientData->mGradientAnimOffsetIndex != startOffsetPropertyIndex;

  gradientData->mGradientAnimOffsetIndex = startOffsetPropertyIndex;

  if(changed)
  {
    RebindGradientAnimConstraints();
  }
}

void TextVisual::SetGradientOverlayAnimProperties(Property::Index startOffsetPropertyIndex)
{
  auto* gradientData = GetTextVisualGradientData(mGradientData);
  if(!gradientData && startOffsetPropertyIndex == Property::INVALID_INDEX)
  {
    return;
  }

  gradientData       = &GetOrCreateTextVisualGradientData(mGradientData);
  const bool changed = gradientData->mGradientOverlayAnimOffsetIndex != startOffsetPropertyIndex;

  gradientData->mGradientOverlayAnimOffsetIndex = startOffsetPropertyIndex;

  if(changed)
  {
    RebindGradientOverlayAnimConstraints();
  }
}

void TextVisual::SetConstraintApplyAlways(bool applyAlways, bool notifyToConstraint)
{
  if(mIsConstraintAppliedAlways != applyAlways || notifyToConstraint)
  {
    mIsConstraintAppliedAlways = applyAlways;

    // Change apply rate only if it is scene on.
    if(mAnimatableTextColorPropertyIndex != Property::INVALID_INDEX && mControl.GetHandle())
    {
      for(auto& constraint : mColorConstraintList)
      {
        if(constraint)
        {
          constraint.SetApplyRate(mIsConstraintAppliedAlways ? Dali::Constraint::APPLY_ALWAYS
                                                             : Dali::Constraint::APPLY_ONCE);
        }
      }
      for(auto& constraint : mOpacityConstraintList)
      {
        if(constraint)
        {
          constraint.SetApplyRate(mIsConstraintAppliedAlways ? Dali::Constraint::APPLY_ALWAYS
                                                             : Dali::Constraint::APPLY_ONCE);
        }
      }
    }
  }
}

void TextVisual::SetGradientAnimApplyAlways(bool applyAlways, bool notifyToConstraint)
{
  auto* gradientData = GetTextVisualGradientData(mGradientData);
  if(!gradientData && !applyAlways)
  {
    return;
  }

  gradientData = &GetOrCreateTextVisualGradientData(mGradientData);
  if(gradientData->mGradientAnimApplyAlways != applyAlways || notifyToConstraint)
  {
    gradientData->mGradientAnimApplyAlways = applyAlways;

    if(mControl.GetHandle())
    {
      for(auto& constraint : gradientData->mGradientAnimConstraints)
      {
        if(constraint)
        {
          constraint.SetApplyRate(gradientData->mGradientAnimApplyAlways ? Dali::Constraint::APPLY_ALWAYS
                                                                         : Dali::Constraint::APPLY_ONCE);
        }
      }
    }
  }
}

void TextVisual::SetGradientOverlayAnimApplyAlways(bool applyAlways, bool notifyToConstraint)
{
  auto* gradientData = GetTextVisualGradientData(mGradientData);
  if(!gradientData && !applyAlways)
  {
    return;
  }

  gradientData = &GetOrCreateTextVisualGradientData(mGradientData);
  if(gradientData->mGradientOverlayAnimApplyAlways != applyAlways || notifyToConstraint)
  {
    gradientData->mGradientOverlayAnimApplyAlways = applyAlways;

    if(mControl.GetHandle())
    {
      for(auto& constraint : gradientData->mGradientOverlayAnimConstraints)
      {
        if(constraint)
        {
          constraint.SetApplyRate(gradientData->mGradientOverlayAnimApplyAlways ? Dali::Constraint::APPLY_ALWAYS
                                                                                : Dali::Constraint::APPLY_ONCE);
        }
      }
    }
  }
}

void TextVisual::RemoveGradientAnimConstraints()
{
  auto* gradientData = GetTextVisualGradientData(mGradientData);
  if(!gradientData)
  {
    return;
  }

  for(auto& constraint : gradientData->mGradientAnimConstraints)
  {
    if(constraint)
    {
      constraint.Remove();
    }
  }
  gradientData->mGradientAnimConstraints.clear();
}

void TextVisual::RemoveGradientOverlayAnimConstraints()
{
  auto* gradientData = GetTextVisualGradientData(mGradientData);
  if(!gradientData)
  {
    return;
  }

  for(auto& constraint : gradientData->mGradientOverlayAnimConstraints)
  {
    if(constraint)
    {
      constraint.Remove();
    }
  }
  gradientData->mGradientOverlayAnimConstraints.clear();
}

void TextVisual::RebindGradientAnimConstraints()
{
  RemoveGradientAnimConstraints();

  auto* gradientData = GetTextVisualGradientData(mGradientData);
  if(!gradientData ||
     !gradientData->mHasGradientContext ||
     !mTextShaderFeatureCache.IsEnabledAnyTextGradient() ||
     !gradientData->mGradientRenderer ||
     gradientData->mGradientAnimOffsetIndex == Property::INVALID_INDEX)
  {
    return;
  }

  const Text::Internal::Gradient::RenderData renderData =
    Text::Internal::Gradient::ResolveRenderData(gradientData->mTextGradientStyle,
                                                gradientData->mLastGradientBounds,
                                                gradientData->mLastGradientCoordSize);
  if(!renderData.enabled)
  {
    return;
  }

  Text::Internal::Gradient::SetRendererProperty(gradientData->mGradientRenderer, UNIFORM_TEXT_GRADIENT_START_POSITION_NAME, renderData.startPosition);
  Text::Internal::Gradient::SetRendererProperty(gradientData->mGradientRenderer, UNIFORM_TEXT_GRADIENT_END_POSITION_NAME, renderData.endPosition);
  const Property::Index startOffsetIndex =
    Text::Internal::Gradient::SetRendererProperty(gradientData->mGradientRenderer, UNIFORM_TEXT_GRADIENT_START_OFFSET_NAME, renderData.startOffset);
  Text::Internal::Gradient::SetRendererProperty(gradientData->mGradientRenderer, UNIFORM_TEXT_GRADIENT_BOUNDS_NAME, renderData.bounds);
  Text::Internal::Gradient::SetRendererProperty(gradientData->mGradientRenderer, UNIFORM_TEXT_GRADIENT_TYPE_NAME, static_cast<float>(renderData.type));
  Text::Internal::Gradient::SetRendererProperty(gradientData->mGradientRenderer, UNIFORM_TEXT_GRADIENT_RADIAL_CENTER_NAME, renderData.radialCenter);
  Text::Internal::Gradient::SetRendererProperty(gradientData->mGradientRenderer, UNIFORM_TEXT_GRADIENT_RADIAL_SCALE_NAME, renderData.radialScale);
  Text::Internal::Gradient::SetRendererProperty(gradientData->mGradientRenderer, UNIFORM_TEXT_GRADIENT_CONIC_CENTER_NAME, renderData.conicCenter);
  Text::Internal::Gradient::SetRendererProperty(gradientData->mGradientRenderer, UNIFORM_TEXT_GRADIENT_CONIC_SCALE_NAME, renderData.conicScale);
  Text::Internal::Gradient::SetRendererProperty(gradientData->mGradientRenderer, UNIFORM_TEXT_GRADIENT_CONIC_START_ANGLE_NAME, renderData.conicStartAngle);

  BindGradientAnimConstraints(gradientData->mGradientRenderer, startOffsetIndex);
}

void TextVisual::RebindGradientOverlayAnimConstraints()
{
  RemoveGradientOverlayAnimConstraints();

  auto* gradientData = GetTextVisualGradientData(mGradientData);
  if(!gradientData ||
     !gradientData->mHasGradientOverlayContext ||
     !mTextShaderFeatureCache.IsEnabledTextGradientOverlay() ||
     !gradientData->mGradientOverlayRenderer ||
     gradientData->mGradientOverlayAnimOffsetIndex == Property::INVALID_INDEX)
  {
    return;
  }

  const Text::Internal::Gradient::RenderData renderData =
    Text::Internal::Gradient::ResolveRenderData(gradientData->mTextGradientOverlayStyle,
                                                gradientData->mLastGradientOverlayBounds,
                                                gradientData->mLastGradientOverlayCoordSize);
  if(!renderData.enabled)
  {
    return;
  }

  Text::Internal::Gradient::SetRendererProperty(gradientData->mGradientOverlayRenderer, UNIFORM_TEXT_GRADIENT_OVERLAY_START_POSITION_NAME, renderData.startPosition);
  Text::Internal::Gradient::SetRendererProperty(gradientData->mGradientOverlayRenderer, UNIFORM_TEXT_GRADIENT_OVERLAY_END_POSITION_NAME, renderData.endPosition);
  const Property::Index startOffsetIndex =
    Text::Internal::Gradient::SetRendererProperty(gradientData->mGradientOverlayRenderer, UNIFORM_TEXT_GRADIENT_OVERLAY_START_OFFSET_NAME, renderData.startOffset);
  Text::Internal::Gradient::SetRendererProperty(gradientData->mGradientOverlayRenderer, UNIFORM_TEXT_GRADIENT_OVERLAY_BOUNDS_NAME, renderData.bounds);
  Text::Internal::Gradient::SetRendererProperty(gradientData->mGradientOverlayRenderer, UNIFORM_TEXT_GRADIENT_OVERLAY_TYPE_NAME, static_cast<float>(renderData.type));
  Text::Internal::Gradient::SetRendererProperty(gradientData->mGradientOverlayRenderer, UNIFORM_TEXT_GRADIENT_OVERLAY_RADIAL_CENTER_NAME, renderData.radialCenter);
  Text::Internal::Gradient::SetRendererProperty(gradientData->mGradientOverlayRenderer, UNIFORM_TEXT_GRADIENT_OVERLAY_RADIAL_SCALE_NAME, renderData.radialScale);
  Text::Internal::Gradient::SetRendererProperty(gradientData->mGradientOverlayRenderer, UNIFORM_TEXT_GRADIENT_OVERLAY_CONIC_CENTER_NAME, renderData.conicCenter);
  Text::Internal::Gradient::SetRendererProperty(gradientData->mGradientOverlayRenderer, UNIFORM_TEXT_GRADIENT_OVERLAY_CONIC_SCALE_NAME, renderData.conicScale);
  Text::Internal::Gradient::SetRendererProperty(gradientData->mGradientOverlayRenderer, UNIFORM_TEXT_GRADIENT_OVERLAY_CONIC_START_ANGLE_NAME, renderData.conicStartAngle);
  Text::Internal::Gradient::SetRendererProperty(gradientData->mGradientOverlayRenderer, UNIFORM_TEXT_GRADIENT_OVERLAY_MODE_NAME, static_cast<float>(gradientData->mTextGradientOverlayMode));

  BindGradientOverlayAnimConstraints(gradientData->mGradientOverlayRenderer, startOffsetIndex);
}

void TextVisual::BindGradientAnimConstraints(VisualRenderer& renderer,
                                             Property::Index startOffsetIndex)
{
  auto* gradientData = GetTextVisualGradientData(mGradientData);
  Actor control      = mControl.GetHandle();
  if(!gradientData || !control || gradientData->mGradientAnimOffsetIndex == Property::INVALID_INDEX)
  {
    return;
  }

  const auto applyRate = gradientData->mGradientAnimApplyAlways ? Dali::Constraint::APPLY_ALWAYS
                                                                : Dali::Constraint::APPLY_ONCE;

  if(gradientData->mGradientAnimOffsetIndex != Property::INVALID_INDEX)
  {
    Constraint constraint = Constraint::New<float>(renderer, startOffsetIndex, GradientOffsetConstraint);
    constraint.AddSource(Source(control, gradientData->mGradientAnimOffsetIndex));
    constraint.SetApplyRate(applyRate);
    Dali::Integration::ConstraintSetInternalTag(constraint, TEXT_VISUAL_GRADIENT_START_OFFSET_CONSTRAINT_TAG);
    constraint.Apply();
    gradientData->mGradientAnimConstraints.push_back(constraint);
  }
}

void TextVisual::BindGradientOverlayAnimConstraints(VisualRenderer& renderer,
                                                    Property::Index startOffsetIndex)
{
  auto* gradientData = GetTextVisualGradientData(mGradientData);
  Actor control      = mControl.GetHandle();
  if(!gradientData || !control || gradientData->mGradientOverlayAnimOffsetIndex == Property::INVALID_INDEX)
  {
    return;
  }

  const auto applyRate = gradientData->mGradientOverlayAnimApplyAlways ? Dali::Constraint::APPLY_ALWAYS
                                                                       : Dali::Constraint::APPLY_ONCE;

  if(gradientData->mGradientOverlayAnimOffsetIndex != Property::INVALID_INDEX)
  {
    Constraint constraint = Constraint::New<float>(renderer, startOffsetIndex, GradientOffsetConstraint);
    constraint.AddSource(Source(control, gradientData->mGradientOverlayAnimOffsetIndex));
    constraint.SetApplyRate(applyRate);
    Dali::Integration::ConstraintSetInternalTag(constraint, TEXT_VISUAL_GRADIENT_OVERLAY_START_OFFSET_CONSTRAINT_TAG);
    constraint.Apply();
    gradientData->mGradientOverlayAnimConstraints.push_back(constraint);
  }
}

void TextVisual::ConfigureTextReveal(Text::Internal::Reveal::Unit unit,
                                     float                        fadeDurationRatio,
                                     Property::Index              progressPropertyIndex,
                                     uint64_t                     revision)
{
  auto* data = GetTextVisualRevealData(mRevealData);
  if(!data)
  {
    if(unit == Text::Internal::Reveal::Unit::DISABLED)
    {
      return;
    }
  }
  else if(data->unit == unit &&
          Equals(data->fadeDurationRatio, fadeDurationRatio) &&
          data->progressPropertyIndex == progressPropertyIndex &&
          data->revision == revision)
  {
    return;
  }

  data                        = &GetOrCreateTextVisualRevealData(mRevealData);
  data->unit                  = unit;
  data->fadeDurationRatio     = fadeDurationRatio;
  data->progressPropertyIndex = progressPropertyIndex;
  data->revision              = revision;
  mRendererUpdateNeeded       = true;
  if(unit == Text::Internal::Reveal::Unit::DISABLED)
  {
    RemoveTextRevealConstraints();
    data->fadeDuration = 0.0f;
  }

  if(IsOnScene())
  {
    UpdateRenderer();
  }
}

void TextVisual::RemoveTextRevealConstraints()
{
  auto* data = GetTextVisualRevealData(mRevealData);
  if(!data)
  {
    return;
  }

  for(auto& constraint : data->constraints)
  {
    if(constraint)
    {
      constraint.Remove();
      constraint.Reset();
    }
  }
  data->constraints.clear();
}

void TextVisual::BindTextRevealConstraint(VisualRenderer& renderer)
{
  auto* data    = GetTextVisualRevealData(mRevealData);
  Actor control = mControl.GetHandle();
  if(!data || !control || data->progressPropertyIndex == Property::INVALID_INDEX || !renderer)
  {
    return;
  }

  // RegisterUniqueProperty deliberately skips the name lookup and would add a
  // duplicate uniform on every renderer rebuild. Reuse the named properties so
  // the constraint and shader always address the same index. Initializing from
  // the source also prevents a rebuilt renderer from flashing progress zero.
  const Property::Index progressIndex =
    renderer.RegisterProperty(UNIFORM_TEXT_REVEAL_PROGRESS_NAME,
                              control.GetCurrentProperty<float>(data->progressPropertyIndex));
  renderer.RegisterProperty(UNIFORM_TEXT_REVEAL_FADE_DURATION_NAME, data->fadeDuration);
  if(progressIndex == Property::INVALID_INDEX)
  {
    return;
  }

  Constraint constraint = Constraint::New<float>(renderer, progressIndex, GradientOffsetConstraint);
  constraint.AddSource(Source(control, data->progressPropertyIndex));
  constraint.SetApplyRate(Dali::Constraint::APPLY_ALWAYS);
  Dali::Integration::ConstraintSetInternalTag(constraint, TEXT_VISUAL_REVEAL_PROGRESS_CONSTRAINT_TAG);
  constraint.Apply();
  data->constraints.push_back(constraint);
}

Text::Internal::Reveal::Plan TextVisual::BuildTextRevealSourcePlan()
{
  auto* data = GetTextVisualRevealData(mRevealData);
  DALI_ASSERT_ALWAYS(data && data->unit != Text::Internal::Reveal::Unit::DISABLED);

  const Text::ModelInterface& model = *mController->GetRenderTextModel();
  if(data->unit == Text::Internal::Reveal::Unit::WORD)
  {
    if(!data->segmentation)
    {
      // Segmentation::Get() depends on the UI-thread SingletonService and is
      // invalid on adaptor worker threads. Own an explicit instance instead.
      data->segmentation = TextAbstraction::Segmentation::New();
    }
    return Text::Internal::Reveal::BuildPlan(model, data->unit, data->fadeDurationRatio, data->segmentation);
  }
  return Text::Internal::Reveal::BuildCharacterPlan(model, data->fadeDurationRatio);
}

void TextVisual::RequestAsyncSizeComputation(Text::AsyncTextParameters& parameters)
{
#ifdef TRACE_ENABLED
  if(gTraceFilter2 && gTraceFilter2->IsTraceEnabled())
  {
    DALI_LOG_RELEASE_INFO("Request size computation, type:%s\n", GetRequestTypeName(parameters.requestType));
  }
#endif

  switch(parameters.requestType)
  {
    case Ui::Integration::Text::Async::COMPUTE_NATURAL_SIZE:
    {
      if(mIsNaturalSizeTaskRunning)
      {
        Text::AsyncTextManager::Get().RequestCancel(mNaturalSizeTaskId);
      }
      mIsNaturalSizeTaskRunning = true;

      TextLoadObserver* textLoadObserver = this;
      mNaturalSizeTaskId                 = Text::AsyncTextManager::Get().RequestLoad(parameters, textLoadObserver);
      break;
    }
    case Ui::Integration::Text::Async::COMPUTE_HEIGHT_FOR_WIDTH:
    {
      if(mIsHeightForWidthTaskRunning)
      {
        Text::AsyncTextManager::Get().RequestCancel(mHeightForWidthTaskId);
      }
      mIsHeightForWidthTaskRunning = true;

      TextLoadObserver* textLoadObserver = this;
      mHeightForWidthTaskId              = Text::AsyncTextManager::Get().RequestLoad(parameters, textLoadObserver);
      break;
    }
    default:
    {
      DALI_LOG_ERROR("Unexpected request type : %d\n", parameters.requestType);
      break;
    }
  }
}

bool TextVisual::UpdateAsyncRenderer(Text::AsyncTextParameters& parameters)
{
  Actor control = mControl.GetHandle();
  if(!control)
  {
    // Nothing to do.
    ResourceReady(Ui::Visual::ResourceStatus::READY);
    return false;
  }

  if((fabsf(parameters.textWidth) < Math::MACHINE_EPSILON_1000) ||
     (fabsf(parameters.textHeight) < Math::MACHINE_EPSILON_1000) || parameters.text.empty())
  {
    if(mIsTextLoadingTaskRunning)
    {
      Text::AsyncTextManager::Get().RequestCancel(mTextLoadingTaskId);
      mIsTextLoadingTaskRunning = false;
    }

    // Remove the texture set and any renderer previously set.
    RemoveRenderer(control, true);

    // Nothing else to do if the relayout size is zero.
    ResourceReady(Ui::Visual::ResourceStatus::READY);

    if(mAsyncTextInterface)
    {
      Text::AsyncTextRenderInfo renderInfo;
      if(parameters.requestType == Ui::Integration::Text::Async::RENDER_FIXED_SIZE)
      {
        renderInfo.renderedSize = Size(parameters.textWidth, parameters.textHeight);
      }
      else if(parameters.requestType == Ui::Integration::Text::Async::RENDER_FIXED_WIDTH)
      {
        renderInfo.renderedSize = Size(parameters.textWidth, 0.0f);
      }
      else if(parameters.requestType == Ui::Integration::Text::Async::RENDER_FIXED_HEIGHT)
      {
        renderInfo.renderedSize = Size(0.0f, parameters.textHeight);
      }
      else
      {
        renderInfo.renderedSize = Size::ZERO;
      }

      mAsyncTextInterface->AsyncRenderFinished(std::move(renderInfo));
    }

    return true;
  }

  // Get the maximum texture size.
  const int maxTextureSize = Dali::GetMaxTextureSize();

  if(parameters.textWidth > maxTextureSize)
  {
    DALI_LOG_DEBUG_INFO(
      "layoutSize(%f) > maxTextureSize(%d): To guarantee the behavior of Texture::New, layoutSize must not be bigger "
      "than maxTextureSize\n",
      parameters.textWidth, maxTextureSize);
    parameters.textWidth = static_cast<float>(maxTextureSize);
  }

  // This does not mean whether task is actually running or waiting.
  // It is whether text visual received a completion callback after requesting a task.
  if(mIsTextLoadingTaskRunning)
  {
    Text::AsyncTextManager::Get().RequestCancel(mTextLoadingTaskId);
  }

#ifdef TRACE_ENABLED
  if(gTraceFilter2 && gTraceFilter2->IsTraceEnabled())
  {
    DALI_LOG_RELEASE_INFO("Request render, type:%s\n", GetRequestTypeName(parameters.requestType));
  }
#endif

  mIsTextLoadingTaskRunning          = true;
  TextLoadObserver* textLoadObserver = this;
  mTextLoadingTaskId                 = Text::AsyncTextManager::Get().RequestLoad(parameters, textLoadObserver);

  return true;
}

bool TextVisual::IsTextGradientCompositionSupported(const Vector2& size, bool hasMultipleTextColors,
                                                    bool containsColorGlyph, bool styleBlocksTextGradient,
                                                    bool isOverlayStyle, bool embossEnabled, bool isHeightTiling,
                                                    bool isMarqueeEnabled, bool isCutoutEnabled) const
{
  (void)isOverlayStyle;
  (void)isMarqueeEnabled;

  const auto* gradientData = GetTextVisualGradientData(mGradientData);
  if(!gradientData || !Text::Internal::Gradient::IsRenderable(gradientData->mTextGradientStyle))
  {
    return false;
  }

  if(isHeightTiling || isCutoutEnabled)
  {
    return false;
  }

  if(hasMultipleTextColors || containsColorGlyph || styleBlocksTextGradient || embossEnabled)
  {
    return false;
  }

  if(size.width < Math::MACHINE_EPSILON_1000 || size.height < Math::MACHINE_EPSILON_1000)
  {
    return false;
  }

  return true;
}

bool TextVisual::IsTextGradientMixedCompositionSupported(const Vector2& size, bool hasMultipleTextColors,
                                                         bool containsColorGlyph, bool styleBlocksTextGradient,
                                                         bool styleTextureEnabled, bool isOverlayStyle,
                                                         bool embossEnabled, bool isHeightTiling,
                                                         bool isMarqueeEnabled, bool isCutoutEnabled) const
{
  (void)styleTextureEnabled;
  (void)isOverlayStyle;

  const auto* gradientData = GetTextVisualGradientData(mGradientData);
  if(!gradientData || !Text::Internal::Gradient::IsRenderable(gradientData->mTextGradientStyle))
  {
    return false;
  }

  if(isHeightTiling || isMarqueeEnabled || isCutoutEnabled)
  {
    return false;
  }

  if(!(hasMultipleTextColors || containsColorGlyph) ||
     styleBlocksTextGradient ||
     embossEnabled)
  {
    return false;
  }

  if(size.width < Math::MACHINE_EPSILON_1000 || size.height < Math::MACHINE_EPSILON_1000)
  {
    return false;
  }

  return true;
}

bool TextVisual::IsTextGradientOverlayCompositionSupported(const Vector2& size, bool isHeightTiling,
                                                           bool isMarqueeEnabled, bool isCutoutEnabled) const
{
  const auto* gradientData = GetTextVisualGradientData(mGradientData);
  if(!gradientData || !Text::Internal::Gradient::IsRenderable(gradientData->mTextGradientOverlayStyle))
  {
    return false;
  }

  if(isHeightTiling || isMarqueeEnabled || isCutoutEnabled)
  {
    return false;
  }

  if(size.width < Math::MACHINE_EPSILON_1000 || size.height < Math::MACHINE_EPSILON_1000)
  {
    return false;
  }

  return true;
}

Vector4 TextVisual::CalculateGradientContentBounds(const Vector2& textureSize) const
{
  const Text::ModelInterface* const textModel = mController->GetRenderTextModel();
  return Text::Internal::CalculateGradientContentBounds(textureSize,
                                                        textModel->GetLayoutSize(),
                                                        textModel->GetLines(),
                                                        textModel->GetNumberOfLines(),
                                                        textModel->GetVerticalAlignment());
}

Vector4 TextVisual::ResolveTextGradientBounds(const Vector2& textureSize, const Vector4& contentBounds) const
{
  const auto* gradientData = GetTextVisualGradientData(mGradientData);
  if(gradientData && gradientData->mTextGradientBoundsMode == Text::GradientBoundsMode::VIEW_BOUND)
  {
    Vector2 visualOffset = Vector2::ZERO;
    if(mImpl->mTransform)
    {
      visualOffset = mImpl->mTransform->mOffset;
    }

    return Text::Internal::CalculateGradientViewBounds(textureSize, mImpl->mControlSize, visualOffset);
  }

  return contentBounds;
}

Vector4 TextVisual::ResolveTextGradientOverlayBounds(const Vector2& textureSize, const Vector4& contentBounds) const
{
  const auto* gradientData = GetTextVisualGradientData(mGradientData);
  if(gradientData && gradientData->mTextGradientOverlayBoundsMode == Text::GradientBoundsMode::VIEW_BOUND)
  {
    Vector2 visualOffset = Vector2::ZERO;
    if(mImpl->mTransform)
    {
      visualOffset = mImpl->mTransform->mOffset;
    }

    return Text::Internal::CalculateGradientViewBounds(textureSize, mImpl->mControlSize, visualOffset);
  }

  return contentBounds;
}

void TextVisual::ApplyTextGradientUniforms(VisualRenderer& renderer, const Vector2& textureSize, const Vector4& textBounds)
{
  auto* gradientData = GetTextVisualGradientData(mGradientData);
  if(!gradientData)
  {
    return;
  }

  const Text::Internal::Gradient::RenderData renderData =
    Text::Internal::Gradient::ResolveRenderData(gradientData->mTextGradientStyle, textBounds, textureSize);
  if(!renderData.enabled)
  {
    return;
  }

  Text::Internal::Gradient::SetRendererProperty(renderer, UNIFORM_TEXT_GRADIENT_START_POSITION_NAME, renderData.startPosition);
  Text::Internal::Gradient::SetRendererProperty(renderer, UNIFORM_TEXT_GRADIENT_END_POSITION_NAME, renderData.endPosition);
  const Property::Index startOffsetIndex =
    Text::Internal::Gradient::SetRendererProperty(renderer, UNIFORM_TEXT_GRADIENT_START_OFFSET_NAME, renderData.startOffset);
  Text::Internal::Gradient::SetRendererProperty(renderer, UNIFORM_TEXT_GRADIENT_BOUNDS_NAME, renderData.bounds);
  Text::Internal::Gradient::SetRendererProperty(renderer, UNIFORM_TEXT_GRADIENT_TYPE_NAME, static_cast<float>(renderData.type));
  Text::Internal::Gradient::SetRendererProperty(renderer, UNIFORM_TEXT_GRADIENT_RADIAL_CENTER_NAME, renderData.radialCenter);
  Text::Internal::Gradient::SetRendererProperty(renderer, UNIFORM_TEXT_GRADIENT_RADIAL_SCALE_NAME, renderData.radialScale);
  Text::Internal::Gradient::SetRendererProperty(renderer, UNIFORM_TEXT_GRADIENT_CONIC_CENTER_NAME, renderData.conicCenter);
  Text::Internal::Gradient::SetRendererProperty(renderer, UNIFORM_TEXT_GRADIENT_CONIC_SCALE_NAME, renderData.conicScale);
  Text::Internal::Gradient::SetRendererProperty(renderer, UNIFORM_TEXT_GRADIENT_CONIC_START_ANGLE_NAME, renderData.conicStartAngle);

  gradientData->mGradientRenderer      = renderer;
  gradientData->mLastGradientCoordSize = textureSize;
  gradientData->mLastGradientBounds    = renderData.bounds;
  gradientData->mHasGradientContext    = true;

  RemoveGradientAnimConstraints();
  BindGradientAnimConstraints(renderer, startOffsetIndex);
}

void TextVisual::ApplyTextGradientOverlayUniforms(VisualRenderer& renderer, const Vector2& textureSize, const Vector4& textBounds)
{
  auto* gradientData = GetTextVisualGradientData(mGradientData);
  if(!gradientData)
  {
    return;
  }

  const Text::Internal::Gradient::RenderData renderData =
    Text::Internal::Gradient::ResolveRenderData(gradientData->mTextGradientOverlayStyle, textBounds, textureSize);
  if(!renderData.enabled)
  {
    return;
  }

  Text::Internal::Gradient::SetRendererProperty(renderer, UNIFORM_TEXT_GRADIENT_OVERLAY_START_POSITION_NAME, renderData.startPosition);
  Text::Internal::Gradient::SetRendererProperty(renderer, UNIFORM_TEXT_GRADIENT_OVERLAY_END_POSITION_NAME, renderData.endPosition);
  const Property::Index startOffsetIndex =
    Text::Internal::Gradient::SetRendererProperty(renderer, UNIFORM_TEXT_GRADIENT_OVERLAY_START_OFFSET_NAME, renderData.startOffset);
  Text::Internal::Gradient::SetRendererProperty(renderer, UNIFORM_TEXT_GRADIENT_OVERLAY_BOUNDS_NAME, renderData.bounds);
  Text::Internal::Gradient::SetRendererProperty(renderer, UNIFORM_TEXT_GRADIENT_OVERLAY_TYPE_NAME, static_cast<float>(renderData.type));
  Text::Internal::Gradient::SetRendererProperty(renderer, UNIFORM_TEXT_GRADIENT_OVERLAY_RADIAL_CENTER_NAME, renderData.radialCenter);
  Text::Internal::Gradient::SetRendererProperty(renderer, UNIFORM_TEXT_GRADIENT_OVERLAY_RADIAL_SCALE_NAME, renderData.radialScale);
  Text::Internal::Gradient::SetRendererProperty(renderer, UNIFORM_TEXT_GRADIENT_OVERLAY_CONIC_CENTER_NAME, renderData.conicCenter);
  Text::Internal::Gradient::SetRendererProperty(renderer, UNIFORM_TEXT_GRADIENT_OVERLAY_CONIC_SCALE_NAME, renderData.conicScale);
  Text::Internal::Gradient::SetRendererProperty(renderer, UNIFORM_TEXT_GRADIENT_OVERLAY_CONIC_START_ANGLE_NAME, renderData.conicStartAngle);
  Text::Internal::Gradient::SetRendererProperty(renderer, UNIFORM_TEXT_GRADIENT_OVERLAY_MODE_NAME, static_cast<float>(gradientData->mTextGradientOverlayMode));

  gradientData->mGradientOverlayRenderer      = renderer;
  gradientData->mLastGradientOverlayCoordSize = textureSize;
  gradientData->mLastGradientOverlayBounds    = renderData.bounds;
  gradientData->mHasGradientOverlayContext    = true;

  RemoveGradientOverlayAnimConstraints();
  BindGradientOverlayAnimConstraints(renderer, startOffsetIndex);
}

void TextVisual::AddRenderer(Actor& actor, const Vector2& size, bool hasMultipleTextColors, bool containsColorGlyph,
                             bool styleEnabled, bool styleTextureEnabled, bool styleBlocksTextGradient,
                             bool isOverlayStyle, bool embossEnabled)
{
  auto* revealData = GetTextVisualRevealData(mRevealData);

  // Get the maximum size.
  const int  maxTextureSize   = Dali::GetMaxTextureSize();
  const bool isHeightTiling   = size.height >= static_cast<float>(maxTextureSize);
  const bool isMarqueeEnabled = mController->IsMarqueeEnabled();
  const bool textGradientStyleBlocksComposition =
    styleBlocksTextGradient ||
    (isMarqueeEnabled && (styleTextureEnabled || isOverlayStyle));
  const bool textGradientCompositionEnabled =
    IsTextGradientCompositionSupported(size, hasMultipleTextColors, containsColorGlyph, textGradientStyleBlocksComposition,
                                       isOverlayStyle, embossEnabled, isHeightTiling,
                                       isMarqueeEnabled, mController->IsTextCutout());
  const bool textGradientMixedCompositionEnabled =
    IsTextGradientMixedCompositionSupported(size, hasMultipleTextColors, containsColorGlyph, textGradientStyleBlocksComposition,
                                            styleTextureEnabled, isOverlayStyle, embossEnabled, isHeightTiling,
                                            isMarqueeEnabled, mController->IsTextCutout());
  const bool textGradientOverlayCompositionEnabled =
    IsTextGradientOverlayCompositionSupported(size, isHeightTiling, isMarqueeEnabled, mController->IsTextCutout());
  const bool featureStyleEnabled =
    (textGradientCompositionEnabled || textGradientMixedCompositionEnabled) ? styleTextureEnabled
                                                                            : styleEnabled;
  const bool textRevealEnabled =
    revealData && revealData->unit != Text::Internal::Reveal::Unit::DISABLED &&
    !isMarqueeEnabled && !mController->IsTextCutout();
  if(revealData && revealData->unit != Text::Internal::Reveal::Unit::DISABLED && !textRevealEnabled)
  {
    DALI_LOG_DEBUG_INFO("Text::Reveal foreground rendering is disabled while marquee or cutout is active\n");
  }
  RemoveTextRevealConstraints();

  TextVisualShaderFeature::FeatureBuilder featureBuilder;
  featureBuilder.EnableMultiColor(hasMultipleTextColors)
    .EnableEmoji(containsColorGlyph)
    .EnableStyle(featureStyleEnabled)
    .EnableOverlay(isOverlayStyle)
    .EnableEmboss(embossEnabled)
    .EnableTextGradient(textGradientCompositionEnabled)
    .EnableTextGradientMixed(textGradientMixedCompositionEnabled)
    .EnableTextGradientOverlay(textGradientOverlayCompositionEnabled)
    .EnableTextReveal(textRevealEnabled);

  Shader shader = GetTextShader(mFactoryCache, featureBuilder);
  mImpl->mRenderer.SetShader(shader);

  DALI_TRACE_SCOPE(gTraceFilter, "DALI_TEXT_VISUAL_UPDATE_RENDERER");

  if(auto* gradientData = GetTextVisualGradientData(mGradientData))
  {
    gradientData->mTextGradientMaskPixelData = PixelData();
  }

  // No tiling required. Use the default renderer.
  if(size.height < maxTextureSize)
  {
    TextureSet textureSet = GetTextTexture(size);

    mImpl->mRenderer.SetTextures(textureSet);
    mImpl->mRenderer.SetProperty(mHasMultipleTextColorsIndex, static_cast<float>(hasMultipleTextColors));
    mImpl->mRenderer.SetProperty(Renderer::Property::BLEND_MODE, BlendMode::ON);

    if(mTextShaderFeatureCache.IsEnabledAnyTextGradient() || mTextShaderFeatureCache.IsEnabledTextGradientOverlay())
    {
      const Vector4 contentBounds = CalculateGradientContentBounds(size);
      if(mTextShaderFeatureCache.IsEnabledAnyTextGradient())
      {
        const Vector4 textBounds = ResolveTextGradientBounds(size, contentBounds);
        ApplyTextGradientUniforms(mImpl->mRenderer, size, textBounds);
      }
      if(mTextShaderFeatureCache.IsEnabledTextGradientOverlay())
      {
        const Vector4 overlayBounds = ResolveTextGradientOverlayBounds(size, contentBounds);
        ApplyTextGradientOverlayUniforms(mImpl->mRenderer, size, overlayBounds);
      }
    }

    if(textRevealEnabled)
    {
      BindTextRevealConstraint(mImpl->mRenderer);
    }

    mRendererList.push_back(mImpl->mRenderer);
  }
  // If the pixel data exceeds the maximum size, tiling is required.
  else
  {
    // Filter mode needs to be set to linear to produce better quality while scaling.
    Sampler sampler = Sampler::New();
    sampler.SetFilterMode(FilterMode::LINEAR, FilterMode::LINEAR);

    // Create RGBA texture if the text contains emojis or multiple text colors, otherwise L8 texture
    Pixel::Format textPixelFormat = (containsColorGlyph || hasMultipleTextColors) ? Pixel::RGBA8888 : Pixel::L8;

    // Check the text direction
    Text::Direction textDirection = mController->GetTextDirection();
    Text::Internal::Reveal::Plan revealPlan;
    if(textRevealEnabled)
    {
      revealPlan = BuildTextRevealSourcePlan();
    }

    // Create a texture for the text without any styles
    PixelData data =
      mTypesetter->Render(size, textDirection, Text::Typesetter::RENDER_NO_STYLES, false, textPixelFormat);

    if(textRevealEnabled)
    {
      revealPlan = mTypesetter->CreateFinalRevealPlan(revealPlan, revealData->unit);
    }

    int verifiedWidth  = data.GetWidth();
    int verifiedHeight = data.GetHeight();

    // Set information for creating textures.
    TilingInfo info(verifiedWidth, maxTextureSize);

    // Get the pixel data of text.
    info.textPixelData = data;

    if(mTextShaderFeatureCache.IsEnabledStyle())
    {
      // Create RGBA texture for all the text styles (without the text itself)
      info.stylePixelData =
        mTypesetter->Render(size, textDirection, Text::Typesetter::RENDER_NO_TEXT, false, Pixel::RGBA8888);
    }

    if(mTextShaderFeatureCache.IsEnabledOverlay())
    {
      // Create RGBA texture for all the overlay styles
      info.overlayStylePixelData =
        mTypesetter->Render(size, textDirection, Text::Typesetter::RENDER_OVERLAY_STYLE, false, Pixel::RGBA8888);
    }

    if(mTextShaderFeatureCache.IsEnabledEmoji() && !mTextShaderFeatureCache.IsEnabledMultiColor())
    {
      // Create a L8 texture as a mask to avoid color glyphs (e.g. emojis) to be affected by text color animation
      info.maskPixelData = mTypesetter->Render(size, textDirection, Text::Typesetter::RENDER_MASK, false, Pixel::L8);
    }

    // Get the current offset for recalculate the offset when tiling.
    Property::Map retMap;
    if(mImpl->mTransform)
    {
      mImpl->mTransform->GetPropertyMap(retMap);
      Property::Value* offsetValue = retMap.Find(Dali::Ui::Visual::Transform::Property::OFFSET);
      if(offsetValue)
      {
        offsetValue->Get(info.transformOffset);
      }
    }

    // Create a textureset in the default renderer.
    if(textRevealEnabled)
    {
      info.revealPixelData = mTypesetter->RenderTextRevealMetadata(
        Vector2(static_cast<float>(info.width), static_cast<float>(info.height)), textDirection,
        revealPlan, revealData->fadeDuration, info.offsetHeight, size);
    }
    CreateTextureSet(info, mImpl->mRenderer, sampler);
    if(textRevealEnabled)
    {
      BindTextRevealConstraint(mImpl->mRenderer);
    }

    verifiedHeight -= maxTextureSize;

    Geometry geometry = mFactoryCache.GetGeometry(VisualFactoryCache::QUAD_GEOMETRY);

    // Create a renderer by cutting maxTextureSize.
    while(verifiedHeight > 0)
    {
      VisualRenderer tilingRenderer = VisualRenderer::New(geometry, shader);
      tilingRenderer.SetProperty(Dali::Renderer::Property::DEPTH_INDEX, Ui::Integration::DepthIndex::CONTENT);
      // New offset position of buffer for tiling.
      info.offsetHeight += maxTextureSize;
      // New height for tiling.
      info.height = (verifiedHeight - maxTextureSize) > 0 ? maxTextureSize : verifiedHeight;
      // New offset for tiling.
      info.transformOffset.y += maxTextureSize;
      if(textRevealEnabled)
      {
        info.revealPixelData = mTypesetter->RenderTextRevealMetadata(
          Vector2(static_cast<float>(info.width), static_cast<float>(info.height)), textDirection,
          revealPlan, revealData->fadeDuration, info.offsetHeight, size);
      }
      // Create a textureset int the new tiling renderer.
      CreateTextureSet(info, tilingRenderer, sampler);
      if(textRevealEnabled)
      {
        BindTextRevealConstraint(tilingRenderer);
      }

      verifiedHeight -= maxTextureSize;
    }
  }

  const Vector4& defaultColor = mController->GetRenderTextModel()->GetDefaultColor();

  for(RendererContainer::iterator iter = mRendererList.begin(); iter != mRendererList.end(); ++iter)
  {
    VisualRenderer renderer = (*iter);
    if(renderer)
    {
      // Register transform properties
      mImpl->SetTransformUniforms(renderer, static_cast<Ui::Integration::Direction::Type>(Text::Direction::LEFT_TO_RIGHT));

      // Note, AddRenderer will ignore renderer if it is already added.
      actor.AddRenderer(renderer);

      if(embossEnabled)
      {
        float          sizeX             = std::max(size.x, Math::MACHINE_EPSILON_100);
        float          sizeY             = std::max(std::min((float)maxTextureSize, size.y), Math::MACHINE_EPSILON_100);
        const Vector2& embossSize        = Vector2(1.0f / sizeX, 1.0f / sizeY);
        const Vector2& embossDirection   = mController->GetEmbossDirection();
        const float    embossStrength    = mController->GetEmbossStrength();
        const Vector4& embossLightColor  = mController->GetEmbossLightColor();
        const Vector4& embossShadowColor = mController->GetEmbossShadowColor();

        renderer.RegisterProperty("uEmbossSize", embossSize);
        renderer.RegisterProperty("uEmbossDirection", embossDirection);
        renderer.RegisterProperty("uEmbossStrength", embossStrength);
        renderer.RegisterProperty("uEmbossLightColor", embossLightColor);
        renderer.RegisterProperty("uEmbossShadowColor", embossShadowColor);
      }

      if(renderer != mImpl->mRenderer)
      {
        // Set constraint for text label's color for non-default renderers.
        if(mAnimatableTextColorPropertyIndex != Property::INVALID_INDEX)
        {
          // Register unique property, or get property for default renderer.
          Property::Index index = renderer.RegisterUniqueProperty("uTextColorAnimatable", defaultColor);

          // Create constraint for the animatable text's color Property with uTextColorAnimatable in the renderer.
          if(index != Property::INVALID_INDEX)
          {
            Constraint colorConstraint = Constraint::New<Vector4>(renderer, index, TextColorConstraint);
            colorConstraint.AddSource(Source(actor, mAnimatableTextColorPropertyIndex));
            colorConstraint.SetApplyRate(mIsConstraintAppliedAlways ? Dali::Constraint::APPLY_ALWAYS
                                                                    : Dali::Constraint::APPLY_ONCE);
            Dali::Integration::ConstraintSetInternalTag(colorConstraint, TEXT_VISUAL_COLOR_CONSTRAINT_TAG);
            colorConstraint.Apply();

            mColorConstraintList.push_back(colorConstraint);
          }

          // Make zero if the alpha value of text color is zero to skip rendering text
          // VisualRenderer::Property::OPACITY uses same animatable property internally.
          Constraint opacityConstraint =
            Constraint::New<float>(renderer, Dali::DevelRenderer::Property::OPACITY, OpacityConstraint);
          opacityConstraint.AddSource(Source(actor, mAnimatableTextColorPropertyIndex));
          opacityConstraint.AddSource(Source(mImpl->mRenderer, mTextRequireRenderPropertyIndex));
          opacityConstraint.SetApplyRate(mIsConstraintAppliedAlways ? Dali::Constraint::APPLY_ALWAYS
                                                                    : Dali::Constraint::APPLY_ONCE);
          Dali::Integration::ConstraintSetInternalTag(opacityConstraint, TEXT_VISUAL_OPACITY_CONSTRAINT_TAG);
          opacityConstraint.Apply();

          mOpacityConstraintList.push_back(opacityConstraint);
        }
      }
    }
  }
}

TextureSet TextVisual::GetTextTexture(const Vector2& size)
{
  const bool cutoutEnabled = mController->IsTextCutout();

  // Filter mode needs to be set to linear to produce better quality while scaling.
  Sampler sampler = Sampler::New();
  sampler.SetFilterMode(FilterMode::LINEAR, FilterMode::LINEAR);

  TextureSet textureSet = TextureSet::New();

  // Create RGBA texture if the text contains emojis or multiple text colors, otherwise L8 texture
  Pixel::Format textPixelFormat =
    (mTextShaderFeatureCache.IsEnabledEmoji() || mTextShaderFeatureCache.IsEnabledMultiColor() || cutoutEnabled)
      ? Pixel::RGBA8888
      : Pixel::L8;

  // Check the text direction
  Text::Direction textDirection   = mController->GetTextDirection();
  uint32_t        textureSetIndex = 0u;
  PixelBuffer     cutoutData;
  float           cutoutAlpha = mController->GetRenderTextModel()->GetDefaultColor().a;

  if(mTextShaderFeatureCache.IsEnabledTextGradientMixed())
  {
    auto&     gradientData = GetOrCreateTextVisualGradientData(mGradientData);
    PixelData preservedData =
      mTypesetter->RenderTextGradientPreserved(size, textDirection, false, Pixel::RGBA8888);
    AddTexture(textureSet, preservedData, sampler, textureSetIndex);
    ++textureSetIndex;

    gradientData.mTextGradientMaskPixelData =
      mTypesetter->RenderTextGradientMask(size, textDirection, false, Pixel::L8);
    AddTexture(textureSet, gradientData.mTextGradientMaskPixelData, sampler, textureSetIndex);
    ++textureSetIndex;

    Text::Internal::Gradient::AddLookupTexture(textureSet, textureSetIndex, gradientData.mTextGradientStyle);
    if(mTextShaderFeatureCache.IsEnabledTextGradientOverlay())
    {
      Text::Internal::Gradient::AddLookupTexture(textureSet, textureSetIndex, gradientData.mTextGradientOverlayStyle);
    }
  }
  else
  {
    // Create a texture for the text without any styles
    if(cutoutEnabled)
    {
      cutoutData = mTypesetter->RenderWithPixelBuffer(size, textDirection, Text::Typesetter::RENDER_NO_STYLES, false,
                                                      textPixelFormat);

      // Make transparent buffer.
      // If the cutout is enabled, a separate texture is not used for the text.
      PixelBuffer buffer = mTypesetter->CreateFullBackgroundBuffer(1, 1, Vector4(0.f, 0.f, 0.f, 0.f));
      PixelData   data   = PixelBuffer::Convert(buffer);
      AddTexture(textureSet, data, sampler, textureSetIndex);
      ++textureSetIndex;
    }
    else
    {
      PixelData data =
        mTypesetter->Render(size, textDirection, Text::Typesetter::RENDER_NO_STYLES, false, textPixelFormat);
      AddTexture(textureSet, data, sampler, textureSetIndex);
      ++textureSetIndex;
    }

    if(mTextShaderFeatureCache.IsEnabledTextGradient())
    {
      auto& gradientData = GetOrCreateTextVisualGradientData(mGradientData);
      Text::Internal::Gradient::AddLookupTexture(textureSet, textureSetIndex, gradientData.mTextGradientStyle);
    }
    if(mTextShaderFeatureCache.IsEnabledTextGradientOverlay())
    {
      auto& gradientData = GetOrCreateTextVisualGradientData(mGradientData);
      Text::Internal::Gradient::AddLookupTexture(textureSet, textureSetIndex, gradientData.mTextGradientOverlayStyle);
    }
  }

  if(mTextShaderFeatureCache.IsEnabledStyle())
  {
    // Create RGBA texture for all the text styles that render in the background (without the text itself)
    PixelData styleData;

    if(cutoutEnabled && cutoutData)
    {
      styleData = mTypesetter->RenderWithCutout(size, textDirection, cutoutData, Text::Typesetter::RENDER_NO_TEXT,
                                                false, Pixel::RGBA8888, cutoutAlpha);
    }
    else
    {
      styleData = mTypesetter->Render(size, textDirection, Text::Typesetter::RENDER_NO_TEXT, false, Pixel::RGBA8888);
    }

    AddTexture(textureSet, styleData, sampler, textureSetIndex);
    ++textureSetIndex;
  }

  if(mTextShaderFeatureCache.IsEnabledOverlay())
  {
    // Create RGBA texture for overlay styles such as underline and strikethrough (without the text itself)
    PixelData overlayStyleData =
      mTypesetter->Render(size, textDirection, Text::Typesetter::RENDER_OVERLAY_STYLE, false, Pixel::RGBA8888);
    AddTexture(textureSet, overlayStyleData, sampler, textureSetIndex);
    ++textureSetIndex;
  }

  if(!mTextShaderFeatureCache.IsEnabledAnyTextGradient() &&
     mTextShaderFeatureCache.IsEnabledEmoji() &&
     !mTextShaderFeatureCache.IsEnabledMultiColor())
  {
    // Create a L8 texture as a mask to avoid color glyphs (e.g. emojis) to be affected by text color animation
    PixelData maskData = mTypesetter->Render(size, textDirection, Text::Typesetter::RENDER_MASK, false, Pixel::L8);

    AddTexture(textureSet, maskData, sampler, textureSetIndex);
    ++textureSetIndex;
  }

  if(mTextShaderFeatureCache.IsEnabledTextReveal())
  {
    auto* revealData = GetTextVisualRevealData(mRevealData);
    DALI_ASSERT_ALWAYS(revealData);
    const auto sourcePlan = BuildTextRevealSourcePlan();
    const auto finalPlan  = mTypesetter->CreateFinalRevealPlan(sourcePlan, revealData->unit);
    PixelData  metadata   = mTypesetter->RenderTextRevealMetadata(size, textDirection, finalPlan, revealData->fadeDuration);

    DALI_ASSERT_ALWAYS(metadata && metadata.GetPixelFormat() == Pixel::RGBA8888 &&
                       metadata.GetWidth() == static_cast<uint32_t>(size.width) &&
                       metadata.GetHeight() == static_cast<uint32_t>(size.height));

    Sampler nearestSampler = Sampler::New();
    nearestSampler.SetFilterMode(FilterMode::NEAREST, FilterMode::NEAREST);
    AddTexture(textureSet, metadata, nearestSampler, textureSetIndex);
  }

  return textureSet;
}

Shader TextVisual::GetTextShader(VisualFactoryCache&                      factoryCache,
                                 TextVisualShaderFeature::FeatureBuilder& featureBuilder)
{
  // Cache feature builder informations.
  mTextShaderFeatureCache = featureBuilder;

  Shader shader = mTextVisualShaderFactory.GetShader(factoryCache, mTextShaderFeatureCache);
  return shader;
}

void TextVisual::SetRequireRender(bool requireRender)
{
  // Avoid function calls if there is no change.
  if(mTextRequireRender != requireRender)
  {
    mTextRequireRender = requireRender;
    if(mImpl->mRenderer)
    {
      mImpl->mRenderer.SetProperty(mTextRequireRenderPropertyIndex, mTextRequireRender);
    }

    // Notify once to opacity constraints
    if(!mIsConstraintAppliedAlways && mAnimatableTextColorPropertyIndex != Property::INVALID_INDEX &&
       mControl.GetHandle())
    {
      for(auto& constraint : mOpacityConstraintList)
      {
        if(constraint)
        {
          constraint.SetApplyRate(Dali::Constraint::APPLY_ONCE);
        }
      }
    }
  }
}

} // namespace Internal

} // namespace Ui

} // namespace Dali
