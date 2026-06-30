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
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-ui-foundation/internal/text/text-scroller-interface.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>

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

/**
 * @brief How the text should be aligned horizontally when scrolling the text.
 *
 * -0.5f aligns the text to the left, 0.0f aligns the text to the center, 0.5f aligns the text to the right.
 * The final alignment depends on two factors:
 *   1) The alignment value of the text label (Use Text::HorizontalAlignment enumerations).
 *   2) The text direction, i.e. whether it's LTR or RTL (0 = LTR, 1 = RTL).
 */
const float HORIZONTAL_ALIGNMENT_TABLE[static_cast<int>(Text::Alignment::END) + 1][2] = {
  // Alignment::START
  {
    -0.5f, // LTR
    0.5f   // RTL
  },

  // Alignment::CENTER
  {
    0.0f, // LTR
    0.0f  // RTL
  },

  // Alignment::END
  {
    0.5f, // LTR
    -0.5f // RTL
  }};

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

} // namespace

namespace Text
{
TextScrollerPtr TextScroller::New(ScrollerInterface& scrollerInterface)
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

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextScroller::SetLoopCount [%d] Status[%s]\n", mLoopCount,
                (loopCount) ? "looping" : "stop");
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

TextScroller::TextScroller(ScrollerInterface& scrollerInterface)
: mScrollerInterface(scrollerInterface),
  mScrollDeltaIndex(Property::INVALID_INDEX),
  mScrollSpeed(MINIMUM_SCROLL_SPEED),
  mLoopCount(1),
  mLoopDelay(0.0f),
  mWrapGap(0.0f),
  mStopMode(Text::MarqueeStopMode::FINISH_LOOP),
  mOrientation(Text::MarqueeOrientation::HORIZONTAL),
  mIsStopRequested(false),
  mIsStoppedImmediately(false)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextScroller Default Constructor\n");
  if(UiConfig::HasCurrent())
  {
    const auto config = UiConfig::GetCurrent();
    SetSpeed(config.GetMarqueeSpeed());
    SetLoopCount(config.GetMarqueeLoopCount());
    SetLoopDelay(config.GetMarqueeLoopDelay());
    SetGap(config.GetMarqueeGap());
    SetStopMode(config.GetMarqueeStopMode());
    SetOrientation(config.GetMarqueeOrientation());
  }
}

TextScroller::~TextScroller()
{
}

void TextScroller::SetParameters(Actor scrollingTextActor, Renderer renderer, TextureSet textureSet,
                                 const Size& controlSize, const Size& textureSize, const float wrapGap,
                                 CharacterDirection direction, Alignment horizontalAlignment,
                                 Alignment verticalAlignment, bool animationReStart)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose,
                "TextScroller::SetParameters controlSize[%f,%f] textureSize[%f,%f] direction[%d]\n", controlSize.x,
                controlSize.y, textureSize.x, textureSize.y, direction);
  mRenderer = renderer;

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

    // Reset to the original shader and texture before scrolling
    mRenderer.SetShader(mShader);
    if(mTextureSet)
    {
      mRenderer.SetTextures(mTextureSet);
    }
  }

  mShader     = mRenderer.GetShader();
  mTextureSet = mRenderer.GetTextures();

  // Set the shader and texture for scrolling
  Shader shader =
    isHorizontal
      ? Shader::New(ToDaliStringView(SHADER_TEXT_SCROLLER_SHADER_VERT), ToDaliStringView(SHADER_TEXT_SCROLLER_SHADER_FRAG),
                    static_cast<Shader::Hint::Value>(Shader::Hint::FILE_CACHE_SUPPORT | Shader::Hint::INTERNAL),
                    "TEXT_SCROLLER")
      : Shader::New(ToDaliStringView(SHADER_TEXT_SCROLLER_VERTICAL_SHADER_VERT), ToDaliStringView(SHADER_TEXT_SCROLLER_VERTICAL_SHADER_FRAG),
                    static_cast<Shader::Hint::Value>(Shader::Hint::FILE_CACHE_SUPPORT | Shader::Hint::INTERNAL),
                    "TEXT_SCROLLER_VERTICAL");

  shader.RegisterUniqueProperty("viewEffectiveScale", 1.0f);
  shader.RegisterUniqueProperty("visualTransformUseEffectiveScale", 1.0f);
  mRenderer.SetShader(shader);
  mRenderer.SetTextures(textureSet);

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextScroller::SetParameters wrapGap[%f]\n", wrapGap);

  float horizontalAlign = 0.0f;
  if(isHorizontal)
  {
    if(textureSize.x > controlSize.x)
    {
      // if Text is elided, scroll should start at the begin of text.
      horizontalAlign = HORIZONTAL_ALIGNMENT_TABLE[static_cast<int>(Alignment::START)][direction];
    }
    else
    {
      horizontalAlign = HORIZONTAL_ALIGNMENT_TABLE[static_cast<int>(horizontalAlignment)][direction];
    }
  }

  const float verticalAlign =
    isHorizontal ? VERTICAL_ALIGNMENT_TABLE[static_cast<int>(verticalAlignment)] : VERTICAL_ALIGNMENT_TABLE[static_cast<int>(Alignment::START)];

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "TextScroller::SetParameters horizontalAlign[%f], verticalAlign[%f]\n",
                horizontalAlign, verticalAlign);

  shader.RegisterProperty("uTextureSize", textureSize);
  shader.RegisterProperty("uHorizontalAlign", horizontalAlign);
  shader.RegisterProperty("uVerticalAlign", verticalAlign);
  shader.RegisterProperty("uGap", wrapGap);
  mScrollDeltaIndex = shader.RegisterProperty("uDelta", 0.0f);

  float scrollAmount =
    isHorizontal ? std::max(textureSize.width, controlSize.width) : std::max(textureSize.height, controlSize.height);
  float scrollDuration = scrollAmount / mScrollSpeed;

  if(isHorizontal && direction)
  {
    scrollAmount = -scrollAmount; // reverse direction of scrolling
  }

  StartScrolling(scrollingTextActor, scrollAmount, scrollDuration, remainedLoop);
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
