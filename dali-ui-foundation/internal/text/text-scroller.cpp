/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali-ui-foundation/internal/text/text-scroller.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/constraint-integ.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>
#include <dali/public-api/animation/constraint.h>
#include <cmath>
#include <string>
#include <string_view>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/text/text-scroller-interface.h>
#include <dali-ui-foundation/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-ui-foundation/internal/text/text-gradient-helper.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-foundation/public-api/types/ui-constraint-tag-ranges.h>

using Dali::Integration::ToDaliStringView;

namespace Dali
{
namespace Ui
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_SCROLLING");
#endif

const int MINIMUM_SCROLL_SPEED = 1; // Speed should be set by Property system.

static constexpr const char* TEXT_GRADIENT_SHADER_DEFINE         = "#define IS_REQUIRED_TEXT_GRADIENT\n";
static constexpr const char* TEXT_GRADIENT_MIXED_SHADER_DEFINE   = "#define IS_REQUIRED_TEXT_GRADIENT_MIXED\n";
static constexpr const char* TEXT_GRADIENT_OVERLAY_SHADER_DEFINE = "#define IS_REQUIRED_TEXT_GRADIENT_OVERLAY\n";
static constexpr const char* TEXT_STYLE_SHADER_DEFINE            = "#define IS_REQUIRED_TEXT_STYLE\n";
static constexpr const char* TEXT_OVERLAY_STYLE_SHADER_DEFINE    = "#define IS_REQUIRED_TEXT_OVERLAY_STYLE\n";
// Keep this tag separate from TextVisual gradient constraint tags because
// TextScroller reuses the visual renderer while marquee is active.
static constexpr uint32_t TEXT_SCROLLER_GRADIENT_START_OFFSET_CONSTRAINT_TAG(Dali::Ui::ConstraintTagRanges::UI_CONSTRAINT_TAG_START + 26u);
static constexpr uint32_t TEXT_SCROLLER_GRADIENT_OVERLAY_START_OFFSET_CONSTRAINT_TAG(Dali::Ui::ConstraintTagRanges::UI_CONSTRAINT_TAG_START + 27u);
constexpr const char*     UNIFORM_TEXT_GRADIENT_START_POSITION_NAME("uTextGradientStartPosition");
constexpr const char*     UNIFORM_TEXT_GRADIENT_END_POSITION_NAME("uTextGradientEndPosition");
constexpr const char*     UNIFORM_TEXT_GRADIENT_START_OFFSET_NAME("uTextGradientStartOffset");
constexpr const char*     UNIFORM_TEXT_GRADIENT_BOUNDS_NAME("uTextGradientBounds");
constexpr const char*     UNIFORM_TEXT_GRADIENT_TYPE_NAME("uTextGradientType");
constexpr const char*     UNIFORM_TEXT_GRADIENT_RADIAL_CENTER_NAME("uTextGradientRadialCenter");
constexpr const char*     UNIFORM_TEXT_GRADIENT_RADIAL_SCALE_NAME("uTextGradientRadialScale");
constexpr const char*     UNIFORM_TEXT_GRADIENT_CONIC_CENTER_NAME("uTextGradientConicCenter");
constexpr const char*     UNIFORM_TEXT_GRADIENT_CONIC_SCALE_NAME("uTextGradientConicScale");
constexpr const char*     UNIFORM_TEXT_GRADIENT_CONIC_START_ANGLE_NAME("uTextGradientConicStartAngle");
constexpr const char*     UNIFORM_TEXT_GRADIENT_OVERLAY_START_POSITION_NAME("uTextGradientOverlayStartPosition");
constexpr const char*     UNIFORM_TEXT_GRADIENT_OVERLAY_END_POSITION_NAME("uTextGradientOverlayEndPosition");
constexpr const char*     UNIFORM_TEXT_GRADIENT_OVERLAY_START_OFFSET_NAME("uTextGradientOverlayStartOffset");
constexpr const char*     UNIFORM_TEXT_GRADIENT_OVERLAY_BOUNDS_NAME("uTextGradientOverlayBounds");
constexpr const char*     UNIFORM_TEXT_GRADIENT_OVERLAY_TYPE_NAME("uTextGradientOverlayType");
constexpr const char*     UNIFORM_TEXT_GRADIENT_OVERLAY_RADIAL_CENTER_NAME("uTextGradientOverlayRadialCenter");
constexpr const char*     UNIFORM_TEXT_GRADIENT_OVERLAY_RADIAL_SCALE_NAME("uTextGradientOverlayRadialScale");
constexpr const char*     UNIFORM_TEXT_GRADIENT_OVERLAY_CONIC_CENTER_NAME("uTextGradientOverlayConicCenter");
constexpr const char*     UNIFORM_TEXT_GRADIENT_OVERLAY_CONIC_SCALE_NAME("uTextGradientOverlayConicScale");
constexpr const char*     UNIFORM_TEXT_GRADIENT_OVERLAY_CONIC_START_ANGLE_NAME("uTextGradientOverlayConicStartAngle");
constexpr const char*     UNIFORM_TEXT_GRADIENT_OVERLAY_MODE_NAME("uTextGradientOverlayMode");

/**
 * @brief How the text should be aligned vertically when scrolling the text.
 *
 * -0.5f aligns the text to the top, 0.0f aligns the text to the center, 0.5f aligns the text to the bottom.
 * The alignment depends on the alignment value of the text label (Use Text::VerticalAlignment enumerations).
 */
const float VERTICAL_ALIGNMENT_TABLE[static_cast<int>(Text::Alignment::END) + 1] = {
  -0.5f, // Alignment::START
  0.0f,  // Alignment::CENTER
  0.5f   // Alignment::END
};

void GradientOffsetConstraint(float& current, const PropertyInputContainer& inputs)
{
  current = inputs[0]->GetFloat();
}

std::string BuildTextScrollerShaderSource(std::string_view shaderSource, bool textGradientEnabled, bool textGradientMixedEnabled, bool textGradientOverlayEnabled, bool textStyleEnabled, bool textOverlayStyleEnabled)
{
  std::string shader;
  if(textGradientEnabled)
  {
    shader += TEXT_GRADIENT_SHADER_DEFINE;
  }
  if(textGradientMixedEnabled)
  {
    shader += TEXT_GRADIENT_MIXED_SHADER_DEFINE;
  }
  if(textGradientOverlayEnabled)
  {
    shader += TEXT_GRADIENT_OVERLAY_SHADER_DEFINE;
  }
  if(textStyleEnabled)
  {
    shader += TEXT_STYLE_SHADER_DEFINE;
  }
  if(textOverlayStyleEnabled)
  {
    shader += TEXT_OVERLAY_STYLE_SHADER_DEFINE;
  }
  shader.append(shaderSource.data(), shaderSource.size());
  return shader;
}

} // namespace

namespace Text
{
TextScrollerPtr TextScroller::New(Ui::Integration::Text::ScrollerInterface& scrollerInterface)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextScroller::New\n");

  TextScrollerPtr textScroller(new TextScroller(scrollerInterface));
  return textScroller;
}

void TextScroller::SetGap(int gap)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextScroller::SetGap gap[%d]\n", gap);
  mWrapGap = static_cast<float>(gap);
}

int TextScroller::GetGap() const
{
  return static_cast<int>(mWrapGap);
}

void TextScroller::SetSpeed(int scrollSpeed)
{
  mScrollSpeed = std::max(MINIMUM_SCROLL_SPEED, scrollSpeed);
}

int TextScroller::GetSpeed() const
{
  return mScrollSpeed;
}

void TextScroller::SetLoopCount(int loopCount)
{
  if(loopCount >= 0)
  {
    mLoopCount = loopCount;
  }

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextScroller::SetLoopCount [%d] Mode[%s]\n", mLoopCount,
                (mLoopCount == MARQUEE_LOOP_COUNT_INFINITE) ? "infinite" : "finite");
}

int TextScroller::GetLoopCount() const
{
  return mLoopCount;
}

void TextScroller::SetLoopDelay(float delay)
{
  mLoopDelay = delay;
}

float TextScroller::GetLoopDelay() const
{
  return mLoopDelay;
}

void TextScroller::SetStopMode(Text::MarqueeStopMode stopMode)
{
  mStopMode = stopMode;
}

Text::MarqueeStopMode TextScroller::GetStopMode() const
{
  return mStopMode;
}

Text::MarqueeOrientation TextScroller::GetOrientation() const
{
  return mOrientation;
}

void TextScroller::SetOrientation(Text::MarqueeOrientation orientation)
{
  mOrientation = orientation;
}

void TextScroller::StopScrolling()
{
  if(IsScrolling())
  {
    switch(mStopMode)
    {
      case Text::MarqueeStopMode::IMMEDIATE:
      {
        mIsStopRequested = false;
        mScrollAnimation.Stop();
        mScrollerInterface.ScrollingFinished();
        mIsStoppedImmediately.store(true);
        break;
      }
      case Text::MarqueeStopMode::FINISH_LOOP:
      {
        mIsStopRequested = true;
        mScrollAnimation.SetLoopCount(1); // As animation already playing this allows the current animation to finish
                                          // instead of trying to stop mid-way
        break;
      }
      default:
      {
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Undifined MarqueeStopMode\n");
      }
    }
  }
  else
  {
    mScrollerInterface.ScrollingFinished();
  }
}

bool TextScroller::IsStopRequested() const
{
  return mIsStopRequested;
}

bool TextScroller::IsScrolling() const
{
  return (mScrollAnimation && mScrollAnimation.GetState() == Animation::PLAYING);
}

void TextScroller::SetGradientApplyAlways(bool applyAlways, bool notifyToConstraint)
{
  if(mGradientApplyAlways != applyAlways || notifyToConstraint)
  {
    mGradientApplyAlways = applyAlways;
    for(auto& constraint : mGradientConstraints)
    {
      if(constraint)
      {
        constraint.SetApplyRate(mGradientApplyAlways ? Dali::Constraint::APPLY_ALWAYS
                                                     : Dali::Constraint::APPLY_ONCE);
      }
    }
  }
}

void TextScroller::SetGradientAnimProperties(Property::Index startOffsetPropertyIndex)
{
  const bool changed = mGradientAnimOffsetIndex != startOffsetPropertyIndex;

  mGradientAnimOffsetIndex = startOffsetPropertyIndex;

  if(changed)
  {
    RemoveGradientConstraints();
    if(mGradientEnabled && mRenderer)
    {
      BindGradientConstraint(mRenderer.GetPropertyIndex(UNIFORM_TEXT_GRADIENT_START_OFFSET_NAME));
    }
  }
}

void TextScroller::SetGradientOverlayAnimProperties(Property::Index startOffsetPropertyIndex)
{
  const bool changed = mGradientOverlayAnimOffsetIndex != startOffsetPropertyIndex;

  mGradientOverlayAnimOffsetIndex = startOffsetPropertyIndex;

  if(changed)
  {
    RemoveGradientOverlayConstraints();
    if(mGradientOverlayEnabled && mRenderer)
    {
      BindGradientOverlayConstraint(mRenderer.GetPropertyIndex(UNIFORM_TEXT_GRADIENT_OVERLAY_START_OFFSET_NAME));
    }
  }
}

void TextScroller::SetGradientOverlayApplyAlways(bool applyAlways, bool notifyToConstraint)
{
  if(mGradientOverlayApplyAlways != applyAlways || notifyToConstraint)
  {
    mGradientOverlayApplyAlways = applyAlways;
    for(auto& constraint : mGradientOverlayConstraints)
    {
      if(constraint)
      {
        constraint.SetApplyRate(mGradientOverlayApplyAlways ? Dali::Constraint::APPLY_ALWAYS
                                                            : Dali::Constraint::APPLY_ONCE);
      }
    }
  }
}

bool TextScroller::IsGradientOverlayEnabled() const
{
  return mGradientOverlayEnabled;
}

void TextScroller::RemoveGradientConstraints()
{
  for(auto& constraint : mGradientConstraints)
  {
    if(constraint)
    {
      constraint.Remove();
    }
  }
  mGradientConstraints.clear();
}

void TextScroller::BindGradientConstraint(Property::Index rendererStartOffsetIndex)
{
  if(!mGradientEnabled ||
     !mScrollingTextActor ||
     rendererStartOffsetIndex == Property::INVALID_INDEX ||
     mGradientAnimOffsetIndex == Property::INVALID_INDEX)
  {
    return;
  }

  Constraint constraint = Constraint::New<float>(mRenderer, rendererStartOffsetIndex, GradientOffsetConstraint);
  constraint.AddSource(Source(mScrollingTextActor, mGradientAnimOffsetIndex));
  constraint.SetApplyRate(mGradientApplyAlways ? Dali::Constraint::APPLY_ALWAYS
                                               : Dali::Constraint::APPLY_ONCE);
  Dali::Integration::ConstraintSetInternalTag(constraint, TEXT_SCROLLER_GRADIENT_START_OFFSET_CONSTRAINT_TAG);
  constraint.Apply();
  mGradientConstraints.push_back(constraint);
}

void TextScroller::RemoveGradientOverlayConstraints()
{
  for(auto& constraint : mGradientOverlayConstraints)
  {
    if(constraint)
    {
      constraint.Remove();
    }
  }
  mGradientOverlayConstraints.clear();
}

void TextScroller::BindGradientOverlayConstraint(Property::Index rendererStartOffsetIndex)
{
  if(!mGradientOverlayEnabled ||
     !mScrollingTextActor ||
     rendererStartOffsetIndex == Property::INVALID_INDEX ||
     mGradientOverlayAnimOffsetIndex == Property::INVALID_INDEX)
  {
    return;
  }

  Constraint constraint = Constraint::New<float>(mRenderer, rendererStartOffsetIndex, GradientOffsetConstraint);
  constraint.AddSource(Source(mScrollingTextActor, mGradientOverlayAnimOffsetIndex));
  constraint.SetApplyRate(mGradientOverlayApplyAlways ? Dali::Constraint::APPLY_ALWAYS
                                                      : Dali::Constraint::APPLY_ONCE);
  Dali::Integration::ConstraintSetInternalTag(constraint, TEXT_SCROLLER_GRADIENT_OVERLAY_START_OFFSET_CONSTRAINT_TAG);
  constraint.Apply();
  mGradientOverlayConstraints.push_back(constraint);
}

TextScroller::TextScroller(Ui::Integration::Text::ScrollerInterface& scrollerInterface)
: mScrollerInterface(scrollerInterface),
  mScrollDeltaIndex(Property::INVALID_INDEX),
  mGradientAnimOffsetIndex(Property::INVALID_INDEX),
  mGradientOverlayAnimOffsetIndex(Property::INVALID_INDEX),
  mScrollSpeed(MINIMUM_SCROLL_SPEED),
  mLoopCount(1),
  mLoopDelay(0.0f),
  mWrapGap(0.0f),
  mStopMode(Text::MarqueeStopMode::FINISH_LOOP),
  mOrientation(Text::MarqueeOrientation::HORIZONTAL),
  mIsStopRequested(false),
  mGradientEnabled(false),
  mGradientApplyAlways(false),
  mGradientOverlayApplyAlways(false),
  mGradientOverlayEnabled(false),
  mIsStoppedImmediately(false)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextScroller Default Constructor\n");
  if(UiConfig::HasCurrent())
  {
    const auto config = UiConfig::GetCurrent();
    SetSpeed(config.GetMarqueeSpeed());
    SetLoopCount(config.GetMarqueeLoopCount());
    SetLoopDelay(config.GetMarqueeLoopDelay());
    SetGap(static_cast<int>(config.GetMarqueeGap()));
    SetStopMode(config.GetMarqueeStopMode());
    SetOrientation(config.GetMarqueeOrientation());
  }
}

TextScroller::~TextScroller()
{
}

void TextScroller::SetParameters(Actor scrollingTextActor, Renderer renderer, TextureSet textureSet,
                                 const Size& controlSize, const Size& textureSize, const float wrapGap,
                                 bool isTextContentOverflow, CharacterDirection direction, Alignment horizontalAlignment,
                                 Alignment verticalAlignment, bool animationReStart,
                                 const TextScrollerGradient& textGradient,
                                 const MarqueeInitialDelta&  marqueeInitialDelta)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose,
                "TextScroller::SetParameters controlSize[%f,%f] textureSize[%f,%f] direction[%d]\n", controlSize.x,
                controlSize.y, textureSize.x, textureSize.y, direction);
  mRenderer           = renderer;
  mScrollingTextActor = scrollingTextActor;
  RemoveGradientConstraints();
  RemoveGradientOverlayConstraints();
  mGradientEnabled        = false;
  mGradientOverlayEnabled = false;

  bool  isHorizontal      = mOrientation == Text::MarqueeOrientation::HORIZONTAL;
  float animationProgress = 0.0f;
  int   remainedLoop      = mLoopCount;
  if(mScrollAnimation)
  {
    if(mScrollAnimation.GetState() == Animation::PLAYING)
    {
      animationProgress = animationReStart ? 0.0f : mScrollAnimation.GetCurrentProgress();

      if(mLoopCount > 0) // If not a ininity loop, then calculate remained loop
      {
        remainedLoop = mLoopCount - (mScrollAnimation.GetCurrentLoop());
        remainedLoop = mIsStopRequested ? 1 : (remainedLoop <= 0 ? 1 : remainedLoop);
      }
    }
    mScrollAnimation.Clear();
  }

  // Set the shader and texture for scrolling
  const std::string_view vertexShaderSource       = isHorizontal ? SHADER_TEXT_SCROLLER_SHADER_VERT : SHADER_TEXT_SCROLLER_VERTICAL_SHADER_VERT;
  const std::string_view fragmentShaderSource     = isHorizontal ? SHADER_TEXT_SCROLLER_SHADER_FRAG : SHADER_TEXT_SCROLLER_VERTICAL_SHADER_FRAG;
  const bool             textGradientMixedEnabled = textGradient.enabled && textGradient.mixedTextGradient;
  const bool             textStyleEnabled         = textGradient.styleTextureEnabled;
  const bool             textOverlayStyleEnabled  = textGradient.overlayStyleTextureEnabled;
  const std::string      vertexShader             = BuildTextScrollerShaderSource(vertexShaderSource, textGradient.enabled, textGradientMixedEnabled, textGradient.overlayEnabled, textStyleEnabled, textOverlayStyleEnabled);
  const std::string      fragmentShader           = BuildTextScrollerShaderSource(fragmentShaderSource, textGradient.enabled, textGradientMixedEnabled, textGradient.overlayEnabled, textStyleEnabled, textOverlayStyleEnabled);
  std::string            shaderName               = isHorizontal ? "TEXT_SCROLLER" : "TEXT_SCROLLER_VERTICAL";
  if(textGradient.enabled)
  {
    shaderName += "_TEXT_GRADIENT";
  }
  if(textGradientMixedEnabled)
  {
    shaderName += "_TEXT_GRADIENT_MIXED";
  }
  if(textGradient.overlayEnabled)
  {
    shaderName += "_TEXT_GRADIENT_OVERLAY";
  }
  if(textStyleEnabled)
  {
    shaderName += "_TEXT_STYLE";
  }
  if(textOverlayStyleEnabled)
  {
    shaderName += "_TEXT_OVERLAY_STYLE";
  }

  const Dali::StringView vertexShaderView   = ToDaliStringView(vertexShader);
  const Dali::StringView fragmentShaderView = ToDaliStringView(fragmentShader);
  Shader                 shader             = Shader::New(vertexShaderView,
                                                          fragmentShaderView,
                                                          static_cast<Shader::Hint::Value>(Shader::Hint::FILE_CACHE_SUPPORT | Shader::Hint::INTERNAL),
                                                          ToDaliStringView(shaderName));

  shader.RegisterUniqueProperty("viewEffectiveScale", 1.0f);
  shader.RegisterUniqueProperty("visualTransformUseEffectiveScale", 1.0f);
  shader.RegisterProperty("pixelSnapFactor", 0.0f);
  mRenderer.SetShader(shader);
  mRenderer.SetTextures(textureSet);

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextScroller::SetParameters wrapGap[%f]\n", wrapGap);

  float horizontalAlign = 0.0f;
  if(isHorizontal)
  {
    horizontalAlign = ResolveHorizontalMarqueeAlignment(isTextContentOverflow,
                                                        direction,
                                                        horizontalAlignment);
  }

  const float verticalAlign =
    isHorizontal ? VERTICAL_ALIGNMENT_TABLE[static_cast<int>(verticalAlignment)] : VERTICAL_ALIGNMENT_TABLE[static_cast<int>(Alignment::START)];

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextScroller::SetParameters horizontalAlign[%f], verticalAlign[%f]\n",
                horizontalAlign, verticalAlign);

  shader.RegisterProperty("uTextureSize", textureSize);
  shader.RegisterProperty("uHorizontalAlign", horizontalAlign);
  shader.RegisterProperty("uVerticalAlign", verticalAlign);
  shader.RegisterProperty("uGap", wrapGap);
  if(textGradient.enabled)
  {
    mGradientEnabled = true;

    // The text visual may have registered TextGradient uniforms on this renderer.
    // Update the renderer values so stale non-marquee bounds cannot override the scroller shader.
    Text::Internal::Gradient::SetRendererProperty(mRenderer, UNIFORM_TEXT_GRADIENT_START_POSITION_NAME, textGradient.startPosition);
    Text::Internal::Gradient::SetRendererProperty(mRenderer, UNIFORM_TEXT_GRADIENT_END_POSITION_NAME, textGradient.endPosition);
    Text::Internal::Gradient::SetRendererProperty(mRenderer, UNIFORM_TEXT_GRADIENT_TYPE_NAME, static_cast<float>(textGradient.type));
    Text::Internal::Gradient::SetRendererProperty(mRenderer, UNIFORM_TEXT_GRADIENT_RADIAL_CENTER_NAME, textGradient.radialCenter);
    Text::Internal::Gradient::SetRendererProperty(mRenderer, UNIFORM_TEXT_GRADIENT_RADIAL_SCALE_NAME, textGradient.radialScale);
    Text::Internal::Gradient::SetRendererProperty(mRenderer, UNIFORM_TEXT_GRADIENT_CONIC_CENTER_NAME, textGradient.conicCenter);
    Text::Internal::Gradient::SetRendererProperty(mRenderer, UNIFORM_TEXT_GRADIENT_CONIC_SCALE_NAME, textGradient.conicScale);
    Text::Internal::Gradient::SetRendererProperty(mRenderer, UNIFORM_TEXT_GRADIENT_CONIC_START_ANGLE_NAME, textGradient.conicStartAngle);
    const Property::Index startOffsetIndex =
      Text::Internal::Gradient::SetRendererProperty(mRenderer, UNIFORM_TEXT_GRADIENT_START_OFFSET_NAME, textGradient.startOffset);
    Text::Internal::Gradient::SetRendererProperty(mRenderer, UNIFORM_TEXT_GRADIENT_BOUNDS_NAME, textGradient.bounds);

    mGradientApplyAlways     = textGradient.applyConstraintsAlways;
    mGradientAnimOffsetIndex = textGradient.startOffsetPropertyIndex;
    BindGradientConstraint(startOffsetIndex);
  }
  if(textGradient.overlayEnabled)
  {
    Text::Internal::Gradient::SetRendererProperty(mRenderer, UNIFORM_TEXT_GRADIENT_OVERLAY_START_POSITION_NAME, textGradient.overlayStartPosition);
    Text::Internal::Gradient::SetRendererProperty(mRenderer, UNIFORM_TEXT_GRADIENT_OVERLAY_END_POSITION_NAME, textGradient.overlayEndPosition);
    Text::Internal::Gradient::SetRendererProperty(mRenderer, UNIFORM_TEXT_GRADIENT_OVERLAY_TYPE_NAME, static_cast<float>(textGradient.overlayType));
    Text::Internal::Gradient::SetRendererProperty(mRenderer, UNIFORM_TEXT_GRADIENT_OVERLAY_RADIAL_CENTER_NAME, textGradient.overlayRadialCenter);
    Text::Internal::Gradient::SetRendererProperty(mRenderer, UNIFORM_TEXT_GRADIENT_OVERLAY_RADIAL_SCALE_NAME, textGradient.overlayRadialScale);
    Text::Internal::Gradient::SetRendererProperty(mRenderer, UNIFORM_TEXT_GRADIENT_OVERLAY_CONIC_CENTER_NAME, textGradient.overlayConicCenter);
    Text::Internal::Gradient::SetRendererProperty(mRenderer, UNIFORM_TEXT_GRADIENT_OVERLAY_CONIC_SCALE_NAME, textGradient.overlayConicScale);
    Text::Internal::Gradient::SetRendererProperty(mRenderer, UNIFORM_TEXT_GRADIENT_OVERLAY_CONIC_START_ANGLE_NAME, textGradient.overlayConicStartAngle);
    const Property::Index overlayStartOffsetIndex =
      Text::Internal::Gradient::SetRendererProperty(mRenderer, UNIFORM_TEXT_GRADIENT_OVERLAY_START_OFFSET_NAME, textGradient.overlayStartOffset);
    Text::Internal::Gradient::SetRendererProperty(mRenderer, UNIFORM_TEXT_GRADIENT_OVERLAY_BOUNDS_NAME, textGradient.overlayBounds);
    Text::Internal::Gradient::SetRendererProperty(mRenderer, UNIFORM_TEXT_GRADIENT_OVERLAY_MODE_NAME, static_cast<float>(textGradient.overlayMode));

    mGradientOverlayEnabled         = true;
    mGradientOverlayApplyAlways     = textGradient.overlayApplyConstraintsAlways;
    mGradientOverlayAnimOffsetIndex = textGradient.overlayStartOffsetPropertyIndex;
    BindGradientOverlayConstraint(overlayStartOffsetIndex);
  }
  float initialScrollDelta = 0.0f;
  if(isHorizontal && isTextContentOverflow && marqueeInitialDelta.valid)
  {
    initialScrollDelta = marqueeInitialDelta.value;
    if(!std::isfinite(initialScrollDelta))
    {
      initialScrollDelta = 0.0f;
    }
  }
  mScrollDeltaIndex = shader.RegisterProperty("uDelta", initialScrollDelta);

  float scrollAmount =
    isHorizontal ? std::max(textureSize.width, controlSize.width) : std::max(textureSize.height, controlSize.height);
  float scrollDuration = scrollAmount / mScrollSpeed;

  if(isHorizontal && direction)
  {
    scrollAmount = -scrollAmount; // reverse direction of scrolling
  }

  StartScrolling(scrollingTextActor, initialScrollDelta + scrollAmount, scrollDuration, remainedLoop);
  mScrollAnimation.SetCurrentProgress(animationProgress);
}

void TextScroller::MarqueeAnimationFinished(Dali::Animation animation)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextScroller::MarqueeAnimationFinished\n");
  mIsStopRequested = false;
  if(!mIsStoppedImmediately.load())
  {
    mScrollerInterface.ScrollingFinished();
  }
}

void TextScroller::StartScrolling(Actor scrollingTextActor, float scrollAmount, float scrollDuration, int loopCount)
{
  mIsStoppedImmediately.store(false);

  DALI_LOG_INFO(gLogFilter, Debug::Verbose,
                "TextScroller::StartScrolling scrollAmount[%f] scrollDuration[%f], loop[%d] speed[%d]\n", scrollAmount,
                scrollDuration, loopCount, mScrollSpeed);
  Shader shader    = mRenderer.GetShader();
  mScrollAnimation = Animation::New(scrollDuration);
  mScrollAnimation.AnimateTo(Property(shader, mScrollDeltaIndex), scrollAmount, TimePeriod(mLoopDelay, scrollDuration));
  mScrollAnimation.SetEndAction(Animation::DISCARD);
  mScrollAnimation.SetLoopCount(loopCount);
  mScrollAnimation.FinishedSignal().Connect(this, &TextScroller::MarqueeAnimationFinished);
  mScrollAnimation.Play();

  mIsStopRequested = false;
}

} // namespace Text

} // namespace Ui

} // namespace Dali
