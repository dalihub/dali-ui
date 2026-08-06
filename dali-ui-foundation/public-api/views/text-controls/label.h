#pragma once

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
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/gradient/gradient-base.h>
#include <dali-ui-foundation/public-api/text/fit/text-fit.h>
#include <dali-ui-foundation/public-api/text/font-variation/font-variation-axis.h>
#include <dali-ui-foundation/public-api/text/font-variation/font-variation.h>
#include <dali-ui-foundation/public-api/text/label-properties.h>
#include <dali-ui-foundation/public-api/text/style/bevel.h>
#include <dali-ui-foundation/public-api/text/style/line-through.h>
#include <dali-ui-foundation/public-api/text/style/outline.h>
#include <dali-ui-foundation/public-api/text/style/shadow.h>
#include <dali-ui-foundation/public-api/text/style/underline.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <functional>

namespace Dali
{

namespace Ui
{
class LabelAnimationBridge;
class LabelAnimationSpec;

namespace Integration
{
class LabelImpl;
}

// @ANIMATION_CONFIG(Label, View)
/**
 * @brief Label is a non-editable View that displays text.
 *
 * It performs text layout and rendering using the text rendering backend,
 * but does not support user interaction or text editing.
 */
class DALI_UI_API Label : public View
{
public:
  /**
   * @brief Property indices for Label.
   *
   * @note See Dali::Ui::Text::LabelPropertyIndex for the underlying property definitions.
   */
  struct Property
  {
    enum
    {
      TEXT                           = Text::LabelPropertyIndex::TEXT,
      FONT_FAMILY                    = Text::LabelPropertyIndex::FONT_FAMILY,
      FONT_SIZE                      = Text::LabelPropertyIndex::FONT_SIZE,
      MULTI_LINE                     = Text::LabelPropertyIndex::MULTI_LINE,
      LINE_WRAP_MODE                 = Text::LabelPropertyIndex::LINE_WRAP_MODE,
      HORIZONTAL_ALIGNMENT           = Text::LabelPropertyIndex::HORIZONTAL_ALIGNMENT,
      VERTICAL_ALIGNMENT             = Text::LabelPropertyIndex::VERTICAL_ALIGNMENT,
      OVERFLOW_MODE                  = Text::LabelPropertyIndex::OVERFLOW_MODE,
      LINE_HEIGHT                    = Text::LabelPropertyIndex::LINE_HEIGHT,
      LINE_HEIGHT_MODE               = Text::LabelPropertyIndex::LINE_HEIGHT_MODE,
      LAYOUT_DIRECTION_MODE          = Text::LabelPropertyIndex::LAYOUT_DIRECTION_MODE,
      ANCHOR_COLOR                   = Text::LabelPropertyIndex::ANCHOR_COLOR,
      ANCHOR_CLICKED_COLOR           = Text::LabelPropertyIndex::ANCHOR_CLICKED_COLOR,
      MARQUEE_TRIGGER_POLICY         = Text::LabelPropertyIndex::MARQUEE_TRIGGER_POLICY,
      MARQUEE_SPEED                  = Text::LabelPropertyIndex::MARQUEE_SPEED,
      MARQUEE_LOOP_COUNT             = Text::LabelPropertyIndex::MARQUEE_LOOP_COUNT,
      MARQUEE_LOOP_DELAY             = Text::LabelPropertyIndex::MARQUEE_LOOP_DELAY,
      MARQUEE_GAP                    = Text::LabelPropertyIndex::MARQUEE_GAP,
      MARQUEE_ORIENTATION            = Text::LabelPropertyIndex::MARQUEE_ORIENTATION,
      MARQUEE_STOP_MODE              = Text::LabelPropertyIndex::MARQUEE_STOP_MODE,
      FONT_WEIGHT                    = Text::LabelPropertyIndex::FONT_WEIGHT,
      FONT_WIDTH                     = Text::LabelPropertyIndex::FONT_WIDTH,
      FONT_SLANT                     = Text::LabelPropertyIndex::FONT_SLANT,
      TEXT_BACKGROUND_COLOR          = Text::LabelPropertyIndex::TEXT_BACKGROUND_COLOR,
      MINIMUM_FONT_SIZE_SCALE        = Text::LabelPropertyIndex::MINIMUM_FONT_SIZE_SCALE,
      MAXIMUM_FONT_SIZE_SCALE        = Text::LabelPropertyIndex::MAXIMUM_FONT_SIZE_SCALE,
      SYSTEM_FONT_SIZE_SCALE_ENABLED = Text::LabelPropertyIndex::SYSTEM_FONT_SIZE_SCALE_ENABLED,
      CUTOUT_ENABLED                 = Text::LabelPropertyIndex::CUTOUT_ENABLED,
      ASYNC_RENDERING                = Text::LabelPropertyIndex::ASYNC_RENDERING,
      RENDER_SCALE                   = Text::LabelPropertyIndex::RENDER_SCALE,
      TEXT_COLOR                     = Text::LabelPropertyIndex::TEXT_COLOR,
      TEXT_COLOR_RED                 = Text::LabelPropertyIndex::TEXT_COLOR_RED,
      TEXT_COLOR_GREEN               = Text::LabelPropertyIndex::TEXT_COLOR_GREEN,
      TEXT_COLOR_BLUE                = Text::LabelPropertyIndex::TEXT_COLOR_BLUE,
      TEXT_COLOR_ALPHA               = Text::LabelPropertyIndex::TEXT_COLOR_ALPHA
    };
  };

public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized Label handle.
   *
   * Only derived versions can be instantiated. Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   */
  Label();

  /**
   * @brief Creates an initialized Label.
   *
   * @return A handle to a newly allocated Dali resource
   */
  static Label New();

  /**
   * @brief Creates an initialized Label.
   *
   * @param[in] text The initial text to be displayed by the Label.
   * @return A handle to a newly allocated Dali resource
   */
  static Label New(const Dali::String& text);

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object.
   * @param[in] label Handle to copy
   */
  Label(const Label& label);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  Label(Label&& rhs) noexcept;

  /**
   * @brief Virtual destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Label();

public: // Operators
  /**
   * @brief Copy assignment operator.
   *
   * Changes this handle to point to another real object.
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  Label& operator=(const Label& handle);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  Label& operator=(Label&& rhs) noexcept;

  DALI_UI_VIEW_WITH(Label)

public: // Static Methods
  /**
   * @brief Downcasts a handle to Label handle.
   *
   * If handle points to a Label, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a Label or an uninitialized handle
   */
  static Label DownCast(BaseHandle handle);

public: // Text Size Measurement
  /**
   * @brief Calculates the height required to lay out the text at the given width.
   *
   * @param[in] width The total view width, including padding.
   * @return The required height, including padding.
   */
  float GetHeightForWidth(float width);

public: // Setters for chaining
  /**
   * @brief Sets the text.
   *
   * @param[in] text The text to display in UTF-8 format.
   */
  void SetText(const Dali::String& text);

  /**
   * @brief Gets the text.
   *
   * @return The text currently set on the label in UTF-8 format.
   */
  Dali::String GetText() const;

  /**
   * @brief Sets styled text.
   *
   * StyledText is applied as plain text plus span attachments and does not use
   * the raw markup parser.
   *
   * @param[in] styledText The styled text snapshot to display.
   */
  void SetStyledText(const Text::StyledText& styledText);

  /**
   * @brief Gets the styled text snapshot currently set on the label.
   *
   * @return The current styled text snapshot, or an empty handle if the current
   * source was set by SetText().
   */
  Text::StyledText GetStyledText() const;

  /**
   * @brief Sets the font family of the text.
   *
   * @param[in] fontFamily The requested font family to use.
   */
  void SetFontFamily(const Dali::String& fontFamily);

  /**
   * @brief Gets the font family of the text.
   *
   * @return The font family currently set on the label.
   */
  Dali::String GetFontFamily() const;

  /**
   * @brief Sets the font size of the text.
   *
   * @param[in] fontSize The font size in pixels.
   */
  void SetFontSize(float fontSize);

  /**
   * @brief Gets the font size of the text.
   *
   * @return The font size currently set on the label, in pixels.
   */
  float GetFontSize() const;

  /**
   * @brief Sets whether the text should be multi-line.
   *
   * @param[in] multiLine True for multi-line layout, false for single-line layout.
   */
  void SetMultiLine(bool multiLine);

  /**
   * @brief Gets whether the text should be multi-line.
   *
   * @return True if multi-line layout is enabled, otherwise false.
   */
  bool IsMultiLine() const;

  /**
   * @brief Sets the line wrap mode.
   *
   * @param[in] mode The line wrap mode to apply.
   */
  void SetLineWrapMode(Text::LineWrapMode mode);

  /**
   * @brief Gets the line wrap mode.
   *
   * @return The current line wrap mode.
   */
  Text::LineWrapMode GetLineWrapMode() const;

  // @ANIMATABLE(Label::Property::TEXT_COLOR, UiColor)
  /**
   * @brief Sets the color of the text.
   *
   * @param[in] color The required text color value.
   */
  void SetTextColor(const UiColor& color);

  /**
   * @brief Gets the color of the text.
   *
   * @return The text color currently set on the label.
   */
  UiColor GetTextColor();

  /**
   * @brief Sets the text gradient.
   *
   * A gradient value with Type::NONE, or a gradient with fewer than two stops,
   * removes the text gradient and restores normal single-color text rendering.
   *
   * @note For Label text rendering, Gradient::Units::OBJECT_BOUNDING_BOX uses
   * normalized coordinates inside the bounds selected by
   * SetTextGradientBoundsMode(), where the top-left is (-0.5, -0.5) and
   * bottom-right is (0.5, 0.5). Gradient::Units::USER_SPACE uses pixel
   * coordinates inside the same selected bounds. This text-specific bounds
   * selection only affects Label text gradient evaluation.
   * For Radial gradients, non-square OBJECT_BOUNDING_BOX bounds may therefore
   * follow the selected bounds aspect; use USER_SPACE for pixel-like center and
   * radius values.
   *
   * @note A stored authored gradient may still fall back to normal text rendering
   * when the active Label rendering path does not support its type, units, or
   * composition.
   *
   * @param[in] gradient The authored gradient value.
   */
  void SetTextGradient(const Gradient::Base& gradient);

  /**
   * @brief Gets the text gradient.
   *
   * @return The authored text gradient, or Type::NONE if no text gradient is set.
   */
  Gradient::Base GetTextGradient() const;

  /**
   * @brief Sets the bounds used to evaluate text gradient coordinates.
   *
   * The default is Text::GradientBoundsMode::CONTENT_BOUND, which maps the
   * gradient to the laid-out text content. Text::GradientBoundsMode::VIEW_BOUND
   * maps the gradient to the full Label view bounds, including padding.
   *
   * @param[in] mode The text gradient bounds mode.
   */
  void SetTextGradientBoundsMode(Text::GradientBoundsMode mode);

  /**
   * @brief Gets the bounds mode used to evaluate text gradient coordinates.
   *
   * @return The current text gradient bounds mode.
   */
  Text::GradientBoundsMode GetTextGradientBoundsMode() const;

  /**
   * @brief Sets a gradient overlay applied to the resolved text glyph fill.
   *
   * The overlay is evaluated after the Label text color, TextGradient,
   * StyledText color spans, or color glyph result has been resolved. It affects
   * visible glyph fill pixels only and does not affect text decorations such as
   * shadow, underline, strikethrough, outline, or background.
   *
   * The overlay mode is evaluated inside the text shader and does not change
   * the framebuffer blending state.
   *
   * @param[in] gradient The overlay gradient. Use Gradient::Base::None()
   * to remove the overlay.
   * @return This Label.
   */
  Label& SetTextGradientOverlay(const Gradient::Base& gradient);

  /**
   * @brief Gets the text gradient overlay.
   *
   * @return The current text gradient overlay, or Type::NONE if no overlay is set.
   */
  Gradient::Base GetTextGradientOverlay() const;

  /**
   * @brief Sets the bounds used to evaluate text gradient overlay coordinates.
   *
   * This is independent from TextGradientBoundsMode used by the base
   * TextGradient.
   *
   * @param[in] mode The overlay gradient bounds mode.
   * @return This Label.
   */
  Label& SetTextGradientOverlayBoundsMode(Text::GradientBoundsMode mode);

  /**
   * @brief Gets the bounds mode used to evaluate text gradient overlay coordinates.
   *
   * @return The current text gradient overlay bounds mode.
   */
  Text::GradientBoundsMode GetTextGradientOverlayBoundsMode() const;

  /**
   * @brief Sets how the text gradient overlay is applied to the resolved text
   * glyph fill.
   *
   * GradientOverlayMode is a text-specific mode evaluated inside the text
   * shader. It does not change framebuffer blending state.
   *
   * @param[in] mode The overlay mode.
   * @return This Label.
   */
  Label& SetTextGradientOverlayMode(Text::GradientOverlayMode mode);

  /**
   * @brief Gets how the text gradient overlay is applied to the resolved text
   * glyph fill.
   *
   * @return The current text gradient overlay mode.
   */
  Text::GradientOverlayMode GetTextGradientOverlayMode() const;

  /**
   * @brief Sets the horizontal alignment of the text within the label.
   *
   * @param[in] alignment The horizontal text alignment.
   */
  void SetHorizontalTextAlignment(Text::Alignment alignment);

  /**
   * @brief Gets the horizontal text alignment.
   *
   * @return The horizontal text alignment.
   */
  Text::Alignment GetHorizontalTextAlignment() const;

  /**
   * @brief Sets the vertical alignment of the text within the label.
   *
   * @param[in] alignment The vertical text alignment.
   */
  void SetVerticalTextAlignment(Text::Alignment alignment);

  /**
   * @brief Gets the vertical text alignment.
   *
   * @return The vertical text alignment.
   */
  Text::Alignment GetVerticalTextAlignment() const;

  /**
   * @brief Sets the overflow mode.
   *
   * @param[in] mode The overflow mode to apply.
   */
  void SetTextOverflowMode(Text::OverflowMode mode);

  /**
   * @brief Gets the overflow mode.
   *
   * @return The current overflow mode.
   */
  Text::OverflowMode GetTextOverflowMode() const;

  /**
   * @brief Sets the line height of the text.
   *
   * The interpretation of this value depends on the current LineHeightMode.
   *
   * - If the mode is LineHeightMode::RELATIVE, the line height is calculated
   *   as a multiplier of the configured font pixel size after applying the
   *   effective text scale:
   *   @code
   *   CalculatedLineHeight(px) = fontSize(px) * lineHeight * effectiveTextScale
   *   @endcode
   *
   * - If the mode is LineHeightMode::ABSOLUTE, the value is treated as
   *   an absolute line height in pixels and then scaled by the effective text scale.
   *   @code
   *   CalculatedLineHeight(px) = lineHeight(px) * effectiveTextScale
   *   @endcode
   *
   * The effective text scale includes both UI scale and adjusted font size scale.
   * The minimum and maximum font size scale clamp only the adjusted font size scale;
   * they do not clamp UI scale or view scale.
   *
   * Setting lineHeight to LINE_HEIGHT_AUTO uses the natural line height
   * derived from the font metrics, regardless of the current LineHeightMode.
   * This behavior is similar to the "Auto" line height option in design tools
   * such as Figma.
   *
   * @note The final line height is clamped to be no smaller than
   *       the natural line height derived from the font metrics.
   *
   * @param[in] lineHeight The line height value.
   */
  void SetLineHeight(float lineHeight);

  /**
   * @brief Gets the current line height value.
   *
   * The returned value is interpreted according to the current
   * LineHeightMode.
   *
   * A value of -1.0f indicates that the natural line height
   * (based on font metrics) is used.
   *
   * @return The line height value.
   */
  float GetLineHeight() const;

  /**
   * @brief Sets how the line height value is interpreted.
   *
   * - LineHeightMode::RELATIVE:
   *   The line height is calculated as a multiplier of the font size.
   *
   * - LineHeightMode::ABSOLUTE:
   *   The line height is treated as an absolute pixel value.
   *
   * The default mode is LineHeightMode::RELATIVE.
   *
   * @param[in] mode The line height mode.
   */
  void SetLineHeightMode(Text::LineHeightMode mode);

  /**
   * @brief Gets the current line height mode.
   *
   * @return The current LineHeightMode.
   */
  Text::LineHeightMode GetLineHeightMode() const;

  /**
   * @brief Sets how the layout direction of the text is resolved.
   *
   * - LayoutDirectionMode::CONTENTS:
   *   The layout direction is determined from the text content itself.
   *
   * - LayoutDirectionMode::INHERIT:
   *   The layout direction is inherited from the parent view.
   *
   * - LayoutDirectionMode::LOCALE:
   *   The layout direction is determined based on the system locale.
   *
   * @param[in] mode The LayoutDirectionMode used to determine the text layout direction.
   */
  void SetLayoutDirectionMode(Text::LayoutDirectionMode mode);

  /**
   * @brief Gets the current layout direction mode.
   *
   * @return The LayoutDirectionMode used to resolve the text layout direction.
   */
  Text::LayoutDirectionMode GetLayoutDirectionMode() const;

  /**
   * @brief Sets the color of anchors in the text.
   *
   * @param[in] color The color to apply to anchors.
   */
  void SetAnchorColor(const UiColor& color);

  /**
   * @brief Gets the color of anchors in the text.
   *
   * @return The current anchor color.
   */
  UiColor GetAnchorColor();

  /**
   * @brief Sets the color of anchors when they are clicked.
   *
   * @param[in] color The color to apply to clicked anchors.
   */
  void SetAnchorClickedColor(const UiColor& color);

  /**
   * @brief Gets the color of anchors when they are clicked.
   *
   * @return The current clicked anchor color.
   */
  UiColor GetAnchorClickedColor();

  /**
   * @brief Sets how the marquee animation is triggered.
   *
   * - MarqueeTriggerPolicy::MANUAL:
   *   The marquee starts only when StartMarquee() is explicitly called.
   *
   * - MarqueeTriggerPolicy::ON_OVERFLOW:
   *   The marquee starts automatically during layout when the text
   *   exceeds the available space.
   *
   * The default policy is MarqueeTriggerPolicy::MANUAL.
   *
   * @param[in] policy The marquee trigger policy.
   */
  void SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy policy);

  /**
   * @brief Returns when the marquee animation is triggered.
   *
   * @return The current marquee trigger policy.
   */
  Text::MarqueeTriggerPolicy GetMarqueeTriggerPolicy() const;

  /**
   * @brief Sets the marquee speed.
   *
   * @param[in] speed The marquee speed in pixels per second.
   */
  void SetMarqueeSpeed(int speed);

  /**
   * @brief Returns the marquee speed.
   *
   * @return The marquee speed in pixels per second.
   */
  int GetMarqueeSpeed() const;

  /**
   * @brief Sets the number of complete loops for marquee.
   *
   * @param[in] loopCount The number of loops.
   */
  void SetMarqueeLoopCount(int loopCount);

  /**
   * @brief Returns the number of complete loops for marquee.
   *
   * @return The number of loops.
   */
  int GetMarqueeLoopCount() const;

  /**
   * @brief Sets the amount of time to delay the start of marquee and further loops.
   *
   * @param[in] delay The delay time in seconds.
   */
  void SetMarqueeLoopDelay(float delay);

  /**
   * @brief Returns the amount of time to delay the start of marquee and further loops.
   *
   * @return The delay time in seconds.
   */
  float GetMarqueeLoopDelay() const;

  /**
   * @brief Sets the gap before marquee wraps.
   *
   * @param[in] gap The gap in pixels.
   */
  void SetMarqueeGap(int gap);

  /**
   * @brief Returns the gap before marquee wraps.
   *
   * @return The gap in pixels.
   */
  int GetMarqueeGap() const;

  /**
   * @brief Sets the marquee orientation.
   *
   * Horizontal is applied only for single-line text, and vertical only for multi-line text.
   * The setting is ignored if the text layout does not match the required condition.
   *
   * @param[in] orientation The marquee orientation.
   */
  void SetMarqueeOrientation(Text::MarqueeOrientation orientation);

  /**
   * @brief Returns the marquee orientation.
   *
   * @return The marquee orientation.
   */
  Text::MarqueeOrientation GetMarqueeOrientation() const;

  /**
   * @brief Sets how the marquee stops.
   *
   * @param[in] stopMode The marquee stop mode.
   */
  void SetMarqueeStopMode(Text::MarqueeStopMode stopMode);

  /**
   * @brief Returns how the marquee stops.
   *
   * @return The marquee stop mode.
   */
  Text::MarqueeStopMode GetMarqueeStopMode() const;

  /**
   * @brief Sets the font weight.
   *
   * @param[in] weight The font weight.
   */
  void SetFontWeight(Text::FontWeight weight);

  /**
   * @brief Returns the font weight.
   *
   * @return The font weight.
   */
  Text::FontWeight GetFontWeight() const;

  /**
   * @brief Sets the font width.
   *
   * @param[in] width The font width.
   */
  void SetFontWidth(Text::FontWidth width);

  /**
   * @brief Returns the font width.
   *
   * @return The font width.
   */
  Text::FontWidth GetFontWidth() const;

  /**
   * @brief Sets the font slant.
   *
   * @param[in] slant The font slant.
   */
  void SetFontSlant(Text::FontSlant slant);

  /**
   * @brief Returns the font slant.
   *
   * @return The font slant.
   */
  Text::FontSlant GetFontSlant() const;

  /**
   * @brief Sets the background color behind the text.
   *
   * The background is rendered behind the glyphs of the text.
   *
   * @param[in] color The text background color.
   */
  void SetTextBackgroundColor(const UiColor& color);

  /**
   * @brief Gets the background color behind the text.
   *
   * @return The current text background color.
   */
  UiColor GetTextBackgroundColor() const;

  /**
   * @brief Clears the text background color.
   *
   * Disables the text background and removes the previously set color.
   */
  void ClearTextBackgroundColor();

  /**
   * @brief Sets the underline style.
   *
   * Pass Text::Underline::None() to clear the underline style.
   *
   * @param[in] underline The underline configuration.
   */
  void SetTextUnderline(const Text::Underline& underline);

  /**
   * @brief Gets the underline style.
   *
   * @return The current underline style, or Text::Underline::None() if not set.
   */
  Text::Underline GetTextUnderline() const;

  /**
   * @brief Sets the shadow style.
   *
   * Pass Text::Shadow::None() to clear the shadow style.
   *
   * @param[in] shadow The shadow configuration.
   */
  void SetTextShadow(const Text::Shadow& shadow);

  /**
   * @brief Gets the shadow style.
   *
   * @return The current shadow style, or Text::Shadow::None() if not set.
   */
  Text::Shadow GetTextShadow() const;

  /**
   * @brief Sets the outline style.
   *
   * Pass Text::Outline::None() to clear the outline style.
   *
   * @param[in] outline The outline configuration.
   */
  void SetTextOutline(const Text::Outline& outline);

  /**
   * @brief Gets the outline style.
   *
   * @return The current outline style, or Text::Outline::None() if not set.
   */
  Text::Outline GetTextOutline() const;

  /**
   * @brief Sets the line-through style.
   *
   * Pass Text::LineThrough::None() to clear the line-through style.
   *
   * @param[in] lineThrough The line-through configuration.
   */
  void SetTextLineThrough(const Text::LineThrough& lineThrough);

  /**
   * @brief Gets the line-through style.
   *
   * @return The current line-through style, or Text::LineThrough::None() if not set.
   */
  Text::LineThrough GetTextLineThrough() const;

  /**
   * @brief Sets the bevel style.
   *
   * Pass Text::Bevel::None() to clear the bevel style.
   *
   * @param[in] bevel The bevel configuration.
   */
  void SetTextBevel(const Text::Bevel& bevel);

  /**
   * @brief Gets the bevel style.
   *
   * @return The current bevel style, or Text::Bevel::None() if not set.
   */
  Text::Bevel GetTextBevel() const;

  /**
   * @brief Sets the text fit configuration.
   *
   * Pass Text::Fit::None() to clear text fit. Text::Fit can also represent
   * range-based or candidate-based text fit.
   *
   * Font size scale is applied when measuring and laying out text fit.
   *
   * @note Text fit is designed for bounded layout sizes. It is recommended to
   * explicitly specify width and height when using this feature.
   *
   * @param[in] fit The text fit configuration.
   */
  void SetTextFit(const Text::Fit& fit);

  /**
   * @brief Sets range-based text fit.
   *
   * Text fit selects the largest font size within the configured range
   * that fits into the available layout space.
   *
   * Font size scale is applied when measuring and laying out text fit.
   *
   * @note Text fit is designed for bounded layout sizes. It is recommended to
   * explicitly specify width and height when using this feature.
   * When width or height is WRAP_CONTENT, measurement is performed using the
   * maximum font size in the configured range.
   *
   * @param[in] range The text fit range configuration.
   */
  void SetTextFit(const Text::Fit::Range& range);

  /**
   * @brief Sets candidate-based text fit.
   *
   * Text fit selects the largest candidate that fits into the available
   * layout space. Each candidate defines a font size and line height.
   *
   * Font size scale is applied to both font size and line height when measuring
   * and laying out text fit candidates.
   *
   * Passing an empty candidate vector clears text fit. This is equivalent to
   * calling SetTextFit(Text::Fit::None()).
   *
   * @note Text fit is designed for bounded layout sizes. It is recommended to
   * explicitly specify width and height when using this feature.
   * When width or height is WRAP_CONTENT, measurement uses the maximum fit
   * candidate to determine the size. The maximum candidate is selected by the
   * largest font size, and if equal, by the larger line height.
   *
   * @param[in] candidates The vector of text fit candidates.
   */
  void SetTextFit(const Dali::Vector<Text::Fit::Candidate>& candidates);

  /**
   * @brief Gets the text fit configuration.
   *
   * The returned fit type indicates whether text fit is disabled,
   * range-based, or candidate-based.
   *
   * @return The current text fit configuration.
   */
  Text::Fit GetTextFit() const;

  /**
   * @brief Sets the minimum font size scale.
   *
   * If this value is greater than the maximum font size scale,
   * the adjusted font size scale follows this minimum value.
   *
   * @param[in] scale The minimum font size scale.
   */
  void SetMinimumFontSizeScale(float scale);

  /**
   * @brief Gets the minimum font size scale.
   *
   * @return The minimum font size scale.
   */
  float GetMinimumFontSizeScale() const;

  /**
   * @brief Sets the maximum font size scale.
   *
   * If this value is less than the minimum font size scale,
   * the adjusted font size scale follows the minimum font size scale.
   *
   * @param[in] scale The maximum font size scale.
   */
  void SetMaximumFontSizeScale(float scale);

  /**
   * @brief Gets the maximum font size scale.
   *
   * @return The maximum font size scale.
   */
  float GetMaximumFontSizeScale() const;

  /**
   * @brief Sets whether the system font size scale is applied.
   *
   * When enabled, the system font size scale is combined with the current
   * font size scale before applying the minimum and maximum constraints.
   *
   * @param[in] enabled True to apply the system font size scale, false otherwise.
   */
  void SetSystemFontSizeScaleEnabled(bool enabled);

  /**
   * @brief Gets whether the system font size scale is applied.
   *
   * @return True if the system font size scale is applied, otherwise false.
   */
  bool IsSystemFontSizeScaleEnabled() const;

  /**
   * @brief Sets the font variation axes.
   *
   * This replaces all previously set font variation axes. Passing
   * Text::FontVariation::None() or an empty axis vector clears the font variation.
   *
   * If duplicate axis tags are provided, the last value is used.
   *
   * Unsupported axis tags may be ignored depending on the selected font.
   *
   * @param[in] axes The font variation axes.
   */
  void SetFontVariation(const Dali::Vector<Text::FontVariation::Axis>& axes);

  /**
   * @brief Sets the font variation from a settings string.
   *
   * The settings string consists of one or more pairs of axis tags and
   * numeric values separated by commas.
   *
   * Supported formats include:
   * - wght=700,wdth=90 (recommended)
   * - "wght" 700, "wdth" 90
   * - 'wght' 700, 'wdth' 90
   *
   * In quoted formats, the axis tag must be wrapped with single quotes
   * (U+0027) or double quotes (U+0022).
   *
   * Each axis tag must contain exactly four printable ASCII characters
   * in the range U+0020..U+007E. Space is allowed only as trailing
   * characters in the axis tag.
   *
   * If duplicate axis tags are specified, the last value is used.
   *
   * If the input string is empty or invalid, the font variation is not changed.
   * Use SetFontVariation(Text::FontVariation::None()) to clear the font variation.
   *
   * Unsupported axis tags may be ignored depending on the selected font.
   *
   * @param[in] settings The font variation settings string.
   */
  void SetFontVariation(const Dali::String& settings);

  /**
   * @brief Returns the font variation axes.
   *
   * @return The font variation axes.
   */
  Dali::Vector<Text::FontVariation::Axis> GetFontVariation() const;

  /**
   * @brief Sets whether the text is rendered as a cutout.
   *
   * When enabled, the glyph shapes are cut out from the rendered content
   * instead of being filled with the text color.
   *
   * @param[in] enabled True to render the text as a cutout, false to render it normally.
   */
  void SetTextCutoutEnabled(bool enabled);

  /**
   * @brief Gets whether the text is rendered as a cutout.
   *
   * @return True if the text is rendered as a cutout, otherwise false.
   */
  bool IsTextCutoutEnabled() const;

  /**
   * @brief Applies a mask effect using the given view.
   *
   * This helper creates a MaskEffect using the rendered output
   * of the view as the mask source and applies it to the label.
   *
   * The given view is added as a child of the label and retained internally
   * until ClearMaskEffect() is called.
   *
   * @note Any existing RenderEffect on the label will be replaced.
   *
   * @param[in] view The view used as the mask source.
   *
   * @see Dali::Ui::MaskEffect
   * @see Dali::Ui::View::SetRenderEffect()
   */
  void SetMaskEffect(View view);

  /**
   * @brief Clears the mask effect applied to the label.
   *
   * Removes the internally retained mask view from the label and clears
   * the applied RenderEffect.
   *
   * @see Dali::Ui::View::ClearRenderEffect()
   */
  void ClearMaskEffect();

  /**
   * @brief Sets whether the text is rendered asynchronously.
   *
   * When enabled, the label automatically requests asynchronous text rendering
   * during relayout as needed.
   *
   * By default, text is rendered synchronously.
   *
   * @param[in] asyncRendering True to enable asynchronous text rendering,
   * false to render text synchronously.
   *
   * @note The render result is delivered through Label::AsyncRenderFinishedSignal().
   */
  void SetAsyncRendering(bool asyncRendering);

  /**
   * @brief Gets whether asynchronous text rendering is enabled.
   *
   * @return True if asynchronous text rendering is enabled, otherwise false.
   */
  bool IsAsyncRendering() const;

  /**
   * @brief Sets the render scale of the text.
   * @details Renders text by rasterizing glyphs at a larger scale and downscaling the result.
   * This improves rendering quality when the view is visually scaled, by reducing
   * quality loss caused by texture upscaling.
   * The layout size of the view is not affected.
   * Valid only when async rendering is enabled, and the value must be 1.0f or greater.
   *
   * @param[in] scale The render scale.
   */
  void SetRenderScale(float scale);

  /**
   * @brief Gets the render scale of the text.
   *
   * @return The current render scale.
   */
  float GetRenderScale() const;

  /**
   * @brief Sets the translatable text resource ID using the default domain.
   *
   * When set, the Label registers a localization binding with UiLocalizationManager
   * and displays the localized string for the given resourceId.
   *
   * The displayed text is automatically updated when:
   * - UiLocalizationManager::RefreshBindings() is called
   * - The default domain changes
   * - The localization override or bypass mode changes
   *
   * @note SetText() does not clear the translatable text binding.
   *       Use ClearTranslatableText() to remove the binding.
   *
   * @param[in] resourceId The resource ID for the localized string (e.g., "IDS_TITLE").
   */
  void SetTranslatableText(StringView resourceId);

  /**
   * @brief Sets the translatable text resource ID with an explicit domain.
   *
   * Passing an empty domain makes the binding use the current default domain,
   * equivalent to SetTranslatableText(resourceId).
   *
   * @param[in] resourceId The resource ID for the localized string (e.g., "IDS_TITLE").
   * @param[in] domain The translation domain, or empty to use the default domain.
   */
  void SetTranslatableText(StringView resourceId, StringView domain);

  /**
   * @brief Gets the translatable text resource ID.
   *
   * @return The resource ID currently set, or an empty string if not set.
   */
  Dali::String GetTranslatableText() const;

  /**
   * @brief Clears the translatable text binding.
   *
   * Removes the localization binding from this Label.
   * The current display text is not changed.
   * Subsequent RefreshBindings() calls will no longer update this Label's text.
   */
  void ClearTranslatableText();
  /**
   * @brief Gets the number of lines of text within the current layout width.
   *
   * @note The line count is calculated based on the current width of the label,
   * clamped between its minimum and maximum width.
   * If the width is not yet resolved (e.g., when using wrap content or match parent constraints),
   * it may be zero before layout is completed, which can result in an incorrect line count.
   * @return The number of lines.
   */
  int GetLineCount();

  /**
   * @brief Gets the number of lines of text within the given width.
   *
   * @param[in] width The width used to calculate the line count.
   * @return The number of lines.
   */
  int GetLineCount(float width);

  /**
   * @brief Gets the line count from the most recent asynchronous result.
   *
   * @note This value is updated when an asynchronous render or asynchronous size
   * computation completes.
   *
   * @return The number of lines from the most recent asynchronous result.
   */
  int GetAsyncLineCount() const;

  /**
   * @brief Returns whether the marquee animation is currently running.
   *
   * @return True if the marquee animation is running, false otherwise.
   */
  bool IsMarqueeRunning() const;

  /**
   * @brief Gets the adjusted font size scale used for rendering.
   *
   * The adjusted font size scale is resolved after applying the current
   * minimum and maximum font size scale constraints and, if enabled,
   * the system font size scale.
   *
   * If the minimum font size scale is greater than the maximum font size scale,
   * the minimum font size scale takes precedence and is used as the adjusted scale.
   *
   * @return The adjusted font size scale used for rendering.
   */
  float GetAdjustedFontSizeScale() const;
  /**
   * @brief Starts the marquee animation using the current marquee settings.
   *
   * The marquee starts only when the orientation matches the current text layout:
   * horizontal for single-line text, and vertical for multi-line text.
   * If the condition is not met, this call has no effect.
   *
   * @note If the trigger policy is MarqueeTriggerPolicy::ON_OVERFLOW,
   *       the marquee starts only when the text exceeds the available space.
   */
  void StartMarquee();

  /**
   * @brief Stops the marquee animation.
   *
   * The stopping behavior follows the current MarqueeStopMode:
   * - MarqueeStopMode::IMMEDIATE:
   *   Stops the marquee immediately.
   *
   * - MarqueeStopMode::FINISH_LOOP:
   *   Continues the animation until the current loop finishes, then stops.
   *
   * This method works regardless of the current MarqueeTriggerPolicy.
   * It stops the marquee even if the policy is MANUAL or ON_OVERFLOW.
   */
  void StopMarquee();

  /**
   * @brief Sets the pixel snap factor used by text rendering.
   *
   * This value controls the degree of pixel snapping applied to the visual
   * position. 0.0f disables snapping and preserves the original position,
   * while 1.0f applies full pixel alignment.
   *
   * The backing animatable property is registered on demand when this method
   * is first called or when PixelSnapFactor animation is first used.
   *
   * @param[in] factor The pixel snap factor.
   */
  void SetPixelSnapFactor(float factor);

  /**
   * @brief Gets the pixel snap factor value set on this Label.
   *
   * @return The pixel snap factor, or 0.0f if it has not been set.
   */
  float GetPixelSnapFactor() const;

  /**
   * @brief Requests asynchronous natural size computation.
   *
   * This method can be used regardless of whether asynchronous rendering is enabled.
   *
   * The computed natural size includes the label padding.
   *
   * @note The computed result is delivered through Label::AsyncNaturalSizeComputedSignal().
   */
  void RequestAsyncNaturalSize();

  /**
   * @brief Requests asynchronous height-for-width computation.
   *
   * This method can be used regardless of whether asynchronous rendering is enabled.
   *
   * The given width must be the total label width including padding.
   *
   * @param[in] width The total width used for the computation, including padding.
   *
   * @note The computed result is delivered through Label::AsyncHeightForWidthComputedSignal().
   */
  void RequestAsyncHeightForWidth(float width);

public: // Signals
  /**
   * @brief This signal is emitted when an anchor in the text is clicked.
   *
   * @code
   *   void OnAnchorClicked(View view, const Dali::String& href);
   * @endcode
   *
   * @param[in] view The view that received the click event.
   * @param[in] href The href of the clicked anchor.
   *
   * @return The signal to connect to.
   */
  Signal<void(View, const Dali::String&)>& AnchorClickedSignal();

  /**
   * @brief This signal is emitted when asynchronous text rendering is finished.
   *
   * @code
   *   void OnAsyncRenderFinished(View view, float width, float height);
   * @endcode
   *
   * @param[in] view The view whose async text rendering has completed.
   * @param[in] width The rendered text width, including padding.
   * @param[in] height The rendered text height, including padding.
   *
   * @return The signal to connect to.
   */
  Signal<void(View, float, float)>& AsyncRenderFinishedSignal();

  /**
   * @brief This signal is emitted when asynchronous natural size computation is finished.
   *
   * @code
   *   void OnAsyncNaturalSizeComputed(View view, float width, float height);
   * @endcode
   *
   * @param[in] view The view whose async natural size computation has completed.
   * @param[in] width The computed natural width, including padding.
   * @param[in] height The computed natural height, including padding.
   *
   * @return The signal to connect to.
   */
  Signal<void(View, float, float)>& AsyncNaturalSizeComputedSignal();

  /**
   * @brief This signal is emitted when asynchronous height-for-width computation is finished.
   *
   * @code
   *   void OnAsyncHeightForWidthComputed(View view, float width, float height);
   * @endcode
   *
   * @param[in] view The view whose async height-for-width computation has completed.
   * @param[in] width The total width used for the computation, including padding.
   * @param[in] height The computed height for the given width, including padding.
   *
   * @return The signal to connect to.
   */
  Signal<void(View, float, float)>& AsyncHeightForWidthComputedSignal();

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Internal implementation.
   *
   * @param[in] implementation The Label implementation
   */
  explicit Label(Integration::LabelImpl& implementation);

  /**
   * @brief Allows the creation of this Label from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit Label(Dali::Internal::CustomActor* internal);
  /// @endcond

  // @ANIMATABLE_MANUAL(TextGradientStartOffset, float)
  // @ANIMATABLE_MANUAL(TextGradientOverlayStartOffset, float)
  // @ANIMATABLE_MANUAL(PixelSnapFactor, float)
public: // Animation
  /**
   * @brief Creates a LabelAnimationBridge for this Label.
   *
   * @code
   *   auto anim = Animation::New();
   *   label.Animate(anim)
   *     .TextColor(UiColor::PRIMARY, 500_ms)
   *     .Opacity(0.0f, 300_ms);
   *   anim.Play();
   * @endcode
   *
   * @param[in] animation The Animation to apply to
   * @return A LabelAnimationBridge
   */
  LabelAnimationBridge Animate(Animation animation);

  /**
   * @brief Creates a new LabelAnimationSpec.
   *
   * @code
   *   auto spec = Label::NewAnimationSpec()
   *     .TextColor(UiColor::PRIMARY, 500_ms)
   *     .Opacity(1.0f, 300_ms);
   *   spec.ApplyTo(anim, label);
   * @endcode
   *
   * @return A new LabelAnimationSpec
   */
  static LabelAnimationSpec NewAnimationSpec();

public:
};

} // namespace Ui

} // namespace Dali
