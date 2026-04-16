#ifndef DALI_UI_TEXT_SCROLLER_H
#define DALI_UI_TEXT_SCROLLER_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/rendering/renderer.h>
#include <atomic>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/text-definitions.h>
#include <dali-ui-foundation/internal/text/text-enumerations.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
class TextScroller;
class ScrollerInterface;

typedef IntrusivePtr<TextScroller> TextScrollerPtr;

/**
 * @brief A helper class for scrolling text
 */
class TextScroller : public RefObject, public ConnectionTracker
{
public:
  /**
   * @brief Text Scrolling helper, used to automatically scroll text, SetParameters should be called before scrolling is
   * needed. CleanUp removes the Scrolling actors from stage whilst keeping the Scroller object alive and preserving
   * Speed, Gap and Loop count.
   *
   * @param[in] scrollerInterface scroller interface
   */
  static TextScrollerPtr New(ScrollerInterface& scrollerInterface);

  /**
   * @brief Set parameters relating to source required for scrolling
   *
   * @param[in] scrollingTextActor actor containing the text to be scrolled
   * @param[in] renderer renderer to render the text
   * @param[in] textureSet texture of the text to be scrolled
   * @param[in] controlSize size of the control to scroll within
   * @param[in] textureSize size of the texture
   * @param[in] wrapGap The gap before scrolling wraps
   * @param[in] direction text direction true for right to left text
   * @param[in] horizontalAlignment horizontal alignment of the text
   * @param[in] verticalAlignment vertical alignment of the text
   * @param[in] animationReStart Whether to start from the beginning when the animation is playing.
   */
  void SetParameters(Actor scrollingTextActor, Dali::Renderer renderer, TextureSet textureSet, const Size& controlSize,
                     const Size& textureSize, const float wrapGap, CharacterDirection direction,
                     Alignment horizontalAlignment, Alignment verticalAlignment,
                     bool animationReStart = false);

  /**
   * @brief Set the gap distance to elapse before the text wraps around
   * @param[in] gap distance to elapse
   */
  void SetGap(int gap);

  /**
   * @brief Get the distance before scrolling wraps
   * @return gap distance to elapse
   */
  int GetGap() const;

  /**
   * @brief Set speed the text should scroll
   * @param[in] scrollSpeed pixels per second
   */
  void SetSpeed(int scrollSpeed);

  /**
   * @brief Get the speed of text scrolling
   * @return speed in pixels per second
   */
  int GetSpeed() const;

  /**
   * @brief Set the number of times the text scrolling should loop, can stop current scrolling by passing in 0;
   * @param[in] loopCount number of times the scrolled text should loop, 0 to stop scrolling
   */
  void SetLoopCount(int loopCount);

  /**
   * @brief Get the number of loops
   * @return int number of loops
   */
  int GetLoopCount() const;

  /**
   * @brief Set the delay time of scroll animation loop
   * @param[in] float delay time seconds of loops
   */
  void SetLoopDelay(float delay);

  /**
   * @brief Get the delay time of scroll
   * @return float delay time seconds of loops
   */
  float GetLoopDelay() const;

  /**
   * @brief Set the mode of scrolling stop
   * @param[in] stopMode type when text scrolling is stopped.
   */
  void SetStopMode(Text::MarqueeStopMode stopMode);

  /**
   * @brief Get the mode of scrolling stop
   * @return MarqueeStopMode when text scrolling is stopped.
   */
  Text::MarqueeStopMode GetStopMode() const;

  /**
   * @brief Set orientation of the marquee.
   * @param[in] orientation Orientation of the marquee.
   */
  void SetOrientation(Text::MarqueeOrientation orientation);

  /**
   * @brief Get orientation of the marquee.
   * @return MarqueeOrientation, HORIZONTAL or VERTICAL.
   */
  Text::MarqueeOrientation GetOrientation() const;

  /**
   * @brief Stop the marquee scrolling.
   */
  void StopScrolling();

  /**
   * @brief Whether the stop scrolling has been triggered or not.
   */
  bool IsStopRequested() const;

  /**
   * @brief Whether the scroll animation is playing or not.
   */
  bool IsScrolling() const;

private: // Implementation
  /**
   * Constructor
   */
  TextScroller(ScrollerInterface& scrollerInterface);

  /**
   * Destructor
   */
  ~TextScroller();

  // Undefined
  TextScroller(const TextScroller& handle);

  // Undefined
  TextScroller& operator=(const TextScroller& handle);

  /**
   * @brief Callback for end of animation
   * @param[in] animation Animation handle
   */
  void AutoScrollAnimationFinished(Dali::Animation& animation);

  /**
   * @brief variables required to set up scrolling animation
   * @param[in] scrollingTextActor actor that shows scrolling text
   * @param[in] scrollAmount distance to animate text for the given duration
   * @param[in] scrollDuration duration of aninmation
   * @param[in] loopCount number of times to loop the scrolling text
   */
  void StartScrolling(Actor scrollingTextActor, float scrollAmount, float scrollDuration, int loopCount);

private:
  ScrollerInterface& mScrollerInterface; // Interface implemented by control that requires scrolling
  Property::Index    mScrollDeltaIndex;  // Property used by shader to represent distance to scroll
  Animation          mScrollAnimation;   // Animation used to update the mScrollDeltaIndex
  Dali::Renderer     mRenderer;          // Renderer used to render the text
  Shader             mShader;            // Shader originally used by the renderer while not scrolling
  TextureSet         mTextureSet;        // Texture originally used by the renderer while not scrolling

  int                      mScrollSpeed;          ///< Speed which text should automatically scroll at
  int                      mLoopCount;            ///< Number of time the text should scroll
  float                    mLoopDelay;            ///< Time delay of loop start
  float                    mWrapGap;              ///< Gap before text wraps around when scrolling
  Text::MarqueeStopMode    mStopMode;             ///< Stop mode of scrolling text, when loop count is 0.
  Text::MarqueeOrientation mOrientation;          ///< Orientation of the marquee. (HORIZONTAL, VERTICAL)
  bool                     mIsStopRequested : 1;  ///< Whether the stop scrolling has been triggered or not.
  std::atomic<bool>        mIsStoppedImmediately; ///< Whether the stop is triggered by immediate stop.

}; // TextScroller class

} // namespace Text

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_TEXT_SCROLLER_H
