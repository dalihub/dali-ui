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
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/rendering/renderer.h>
#include <atomic>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/text/text-scroller-interface.h>
#include <dali-ui-foundation/internal/text/text-definitions.h>
#include <dali-ui-foundation/internal/text/text-enumerations.h>
#include <dali-ui-foundation/public-api/gradient/gradient-enumerations.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
class TextScroller;

typedef IntrusivePtr<TextScroller> TextScrollerPtr;

struct TextScrollerGradient
{
  bool            enabled{false};
  Gradient::Type  type{Gradient::Type::NONE};
  Vector2         startPosition{Vector2::ZERO};
  Vector2         endPosition{Vector2::ONE};
  Vector2         radialCenter{Vector2::ZERO};
  Vector2         radialScale{Vector2::ZERO};
  Vector2         conicCenter{Vector2::ZERO};
  Vector2         conicScale{Vector2::ONE};
  float           conicStartAngle{0.0f};
  float           startOffset{0.0f};
  Vector4         bounds{0.0f, 0.0f, 1.0f, 1.0f}; ///< Normalized viewport-local bounds for TextGradient evaluation.
  Property::Index startOffsetPropertyIndex{Property::INVALID_INDEX};
  bool            applyConstraintsAlways{false};
  bool            mixedTextGradient{false};          ///< True when scroller uses preserved color + gradient mask textures.
  bool            styleTextureEnabled{false};        ///< True when slot set includes a below-fill style texture.
  bool            overlayStyleTextureEnabled{false}; ///< True when slot set includes underline/strikethrough texture.

  bool                      overlayEnabled{false};
  Gradient::Type            overlayType{Gradient::Type::NONE};
  Vector2                   overlayStartPosition{Vector2::ZERO};
  Vector2                   overlayEndPosition{Vector2::ONE};
  Vector2                   overlayRadialCenter{Vector2::ZERO};
  Vector2                   overlayRadialScale{Vector2::ZERO};
  Vector2                   overlayConicCenter{Vector2::ZERO};
  Vector2                   overlayConicScale{Vector2::ONE};
  float                     overlayConicStartAngle{0.0f};
  float                     overlayStartOffset{0.0f};
  Vector4                   overlayBounds{0.0f, 0.0f, 1.0f, 1.0f}; ///< Normalized viewport-local bounds for TextGradientOverlay evaluation.
  Text::GradientOverlayMode overlayMode{Text::GradientOverlayMode::SRC_OVER};
  Property::Index           overlayStartOffsetPropertyIndex{Property::INVALID_INDEX};
  bool                      overlayApplyConstraintsAlways{false};
};

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
  static TextScrollerPtr New(Ui::Integration::Text::ScrollerInterface& scrollerInterface);

  /**
   * @brief Set parameters relating to source required for scrolling
   *
   * @param[in] scrollingTextActor actor containing the text to be scrolled
   * @param[in] renderer renderer to render the text
   * @param[in] textureSet texture of the text to be scrolled
   * @param[in] controlSize size of the control to scroll within
   * @param[in] textureSize size of the texture
   * @param[in] wrapGap The gap before scrolling wraps
   * @param[in] isTextContentOverflow Whether the text content, excluding the wrap gap, overflows the control
   * @param[in] direction text direction true for right to left text
   * @param[in] horizontalAlignment horizontal alignment of the text
   * @param[in] verticalAlignment vertical alignment of the text
   * @param[in] animationReStart Whether to start from the beginning when the animation is playing.
   * @param[in] textGradient Full marquee gradient renderer setup data, including optional source
   * property indices for initial animation binding.
   */
  void SetParameters(Actor scrollingTextActor, Dali::Renderer renderer, TextureSet textureSet, const Size& controlSize,
                     const Size& textureSize, const float wrapGap, bool isTextContentOverflow, CharacterDirection direction,
                     Alignment horizontalAlignment, Alignment verticalAlignment,
                     bool                        animationReStart = false,
                     const TextScrollerGradient& textGradient     = TextScrollerGradient());

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
   * @brief Sets the number of times the text scrolling should loop.
   * @param[in] loopCount The number of loops; 0 means infinite looping. Negative values are ignored.
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

  /**
   * @brief Sets the TextGradient start-offset source property index used by marquee constraints.
   *
   * This is the live update path after SetParameters() has already set up a
   * gradient-enabled scroller renderer.
   *
   * @param[in] startOffsetPropertyIndex Source property index for uTextGradientStartOffset.
   */
  void SetGradientAnimProperties(Property::Index startOffsetPropertyIndex);

  /**
   * @brief Sets whether TextGradient animation constraints should be applied every frame.
   *
   * @param[in] applyAlways True to use APPLY_ALWAYS, false to use APPLY_ONCE.
   * @param[in] notifyToConstraint True to update existing constraints even if the state did not change.
   */
  void SetGradientApplyAlways(bool applyAlways, bool notifyToConstraint = false);

  /**
   * @brief Sets the TextGradientOverlay start-offset source property index used by marquee constraints.
   *
   * This is the live update path after SetParameters() has already set up an
   * overlay-enabled scroller renderer.
   *
   * @param[in] startOffsetPropertyIndex Source property index for uTextGradientOverlayStartOffset.
   */
  void SetGradientOverlayAnimProperties(Property::Index startOffsetPropertyIndex);

  /**
   * @brief Sets whether TextGradientOverlay animation constraints should be applied every frame.
   *
   * @param[in] applyAlways True to use APPLY_ALWAYS, false to use APPLY_ONCE.
   * @param[in] notifyToConstraint True to update existing constraints even if the state did not change.
   */
  void SetGradientOverlayApplyAlways(bool applyAlways, bool notifyToConstraint = false);

  /**
   * @brief Returns whether the current scroller renderer has TextGradientOverlay enabled.
   */
  bool IsGradientOverlayEnabled() const;

private: // Implementation
  /**
   * Constructor
   */
  TextScroller(Ui::Integration::Text::ScrollerInterface& scrollerInterface);

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
  void MarqueeAnimationFinished(Dali::Animation animation);

  /**
   * @brief variables required to set up scrolling animation
   * @param[in] scrollingTextActor actor that shows scrolling text
   * @param[in] scrollAmount distance to animate text for the given duration
   * @param[in] scrollDuration duration of aninmation
   * @param[in] loopCount number of times to loop the scrolling text
   */
  void StartScrolling(Actor scrollingTextActor, float scrollAmount, float scrollDuration, int loopCount);

  /**
   * @brief Removes TextGradient animation constraints from the current renderer.
   */
  void RemoveGradientConstraints();

  /**
   * @brief Removes TextGradientOverlay animation constraints from the current renderer.
   */
  void RemoveGradientOverlayConstraints();

  /**
   * @brief Binds TextGradient animation constraints to the current renderer.
   */
  void BindGradientConstraint(Property::Index rendererStartOffsetIndex);

  /**
   * @brief Binds TextGradientOverlay animation constraints to the current renderer.
   */
  void BindGradientOverlayConstraint(Property::Index rendererStartOffsetIndex);

private:
  Ui::Integration::Text::ScrollerInterface& mScrollerInterface;              // Interface implemented by control that requires scrolling
  Property::Index                           mScrollDeltaIndex;               // Property used by shader to represent distance to scroll
  Animation                                 mScrollAnimation;                // Animation used to update the mScrollDeltaIndex
  Dali::Renderer                            mRenderer;                       // Renderer used to render the text
  Actor                                     mScrollingTextActor;             // Actor used as source for TextGradient animation properties
  std::vector<Constraint>                   mGradientConstraints;            // Constraints for animated TextGradient uniforms.
  std::vector<Constraint>                   mGradientOverlayConstraints;     // Constraints for animated TextGradientOverlay uniforms.
  Property::Index                           mGradientAnimOffsetIndex;        // Source property for uTextGradientStartOffset.
  Property::Index                           mGradientOverlayAnimOffsetIndex; // Source property for uTextGradientOverlayStartOffset.

  int                      mScrollSpeed;                    ///< Speed which text should automatically scroll at
  int                      mLoopCount;                      ///< Number of time the text should scroll
  float                    mLoopDelay;                      ///< Time delay of loop start
  float                    mWrapGap;                        ///< Gap before text wraps around when scrolling
  Text::MarqueeStopMode    mStopMode;                       ///< Stop mode of scrolling text, when loop count is 0.
  Text::MarqueeOrientation mOrientation;                    ///< Orientation of the marquee. (HORIZONTAL, VERTICAL)
  bool                     mIsStopRequested : 1;            ///< Whether the stop scrolling has been triggered or not.
  bool                     mGradientEnabled : 1;            ///< Whether the current scroller renderer has TextGradient uniforms.
  bool                     mGradientApplyAlways : 1;        ///< Whether TextGradient constraints need to be applied always.
  bool                     mGradientOverlayApplyAlways : 1; ///< Whether TextGradientOverlay constraints need to be applied always.
  bool                     mGradientOverlayEnabled : 1;     ///< Whether the current scroller renderer has TextGradientOverlay uniforms.
  std::atomic<bool>        mIsStoppedImmediately;           ///< Whether the stop is triggered by immediate stop.

}; // TextScroller class

} // namespace Text

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_TEXT_SCROLLER_H
