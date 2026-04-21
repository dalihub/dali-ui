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

// EXTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/view.h>
#include <functional>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/text/fit/text-fit-candidate.h>
#include <dali-ui-foundation/public-api/text/fit/text-fit-range.h>
#include <dali-ui-foundation/public-api/text/font-variation/font-variation-axis.h>
#include <dali-ui-foundation/public-api/text/label-properties.h>
#include <dali-ui-foundation/public-api/text/style/bevel.h>
#include <dali-ui-foundation/public-api/text/style/line-through.h>
#include <dali-ui-foundation/public-api/text/style/outline.h>
#include <dali-ui-foundation/public-api/text/style/shadow.h>
#include <dali-ui-foundation/public-api/text/style/underline.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>
#include <dali-ui-foundation/public-api/ui-color.h>

// None

namespace Dali
{

namespace Ui
{

// Forward declarations
class LabelAnimationBridge;
class LabelAnimationSpec;

// @ANIMATION_CONFIG(Label, View)

namespace Integration
{
class LabelImpl;
}

#include "label.autogen.h"
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
      MARKUP_ENABLED                 = Text::LabelPropertyIndex::MARKUP_ENABLED,
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
      FONT_SIZE_SCALE                = Text::LabelPropertyIndex::FONT_SIZE_SCALE,
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
      TEXT_COLOR_ALPHA               = Text::LabelPropertyIndex::TEXT_COLOR_ALPHA,
      PIXEL_SNAP_FACTOR              = Text::LabelPropertyIndex::PIXEL_SNAP_FACTOR
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

public: // Setters for chaining
  // @CHAIN_START(Label, View)
  /**
   * @brief Sets the text.
   *
   * @param[in] text The text to display in UTF-8 format.
   */
  Label& SetText(const Dali::String& text);

  /**
   * @brief Gets the text.
   *
   * @return The text currently set on the label in UTF-8 format.
   */
  Dali::String GetText() const;

  /**
   * @brief Sets the font family of the text.
   *
   * @param[in] fontFamily The requested font family to use.
   */
  Label& SetFontFamily(const Dali::String& fontFamily);

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
  Label& SetFontSize(float fontSize);

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
  Label& SetMultiLine(bool multiLine);

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
  Label& SetLineWrapMode(Text::LineWrapMode mode);

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
  Label& SetTextColor(const UiColor& color);

  /**
   * @brief Gets the color of the text.
   *
   * @return The text color currently set on the label.
   */
  UiColor GetTextColor();

  /**
   * @brief Sets the horizontal alignment of the text within the label.
   *
   * @param[in] alignment The horizontal text alignment.
   */
  Label& SetHorizontalTextAlignment(Text::Alignment alignment);

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
  Label& SetVerticalTextAlignment(Text::Alignment alignment);

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
  Label& SetOverflowMode(Text::OverflowMode mode);

  /**
   * @brief Gets the overflow mode.
   *
   * @return The current overflow mode.
   */
  Text::OverflowMode GetOverflowMode() const;

  /**
   * @brief Sets the line height of the text.
   *
   * The interpretation of this value depends on the current LineHeightMode.
   *
   * - If the mode is LineHeightMode::RELATIVE, the line height is calculated
   *   as a multiplier of the font pixel size:
   *   @code
   *   CalculatedLineHeight(px) = FontSize(px) * lineHeight
   *   @endcode
   *
   * - If the mode is LineHeightMode::ABSOLUTE, the value is treated as
   *   an absolute line height in pixels.
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
  Label& SetLineHeight(float lineHeight);

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
  Label& SetLineHeightMode(Text::LineHeightMode mode);

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
  Label& SetLayoutDirectionMode(Text::LayoutDirectionMode mode);

  /**
   * @brief Gets the current layout direction mode.
   *
   * @return The LayoutDirectionMode used to resolve the text layout direction.
   */
  Text::LayoutDirectionMode GetLayoutDirectionMode() const;

  /**
   * @brief Enables or disables mark-up processing for the text.
   *
   * @param[in] enabled True to enable mark-up parsing, false to render text as plain string.
   */
  Label& SetMarkupEnabled(bool enabled);

  /**
   * @brief Gets whether mark-up processing is enabled.
   *
   * @return True if mark-up parsing is enabled, otherwise false.
   */
  bool IsMarkupEnabled() const;

  /**
   * @brief Sets the color of anchors in the text.
   *
   * @param[in] color The color to apply to anchors.
   */
  Label& SetAnchorColor(const UiColor& color);

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
  Label& SetAnchorClickedColor(const UiColor& color);

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
  Label& SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy policy);

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
  Label& SetMarqueeSpeed(int speed);

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
  Label& SetMarqueeLoopCount(int loopCount);

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
  Label& SetMarqueeLoopDelay(float delay);

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
  Label& SetMarqueeGap(int gap);

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
  Label& SetMarqueeOrientation(Text::MarqueeOrientation orientation);

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
  Label& SetMarqueeStopMode(Text::MarqueeStopMode stopMode);

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
  Label& SetFontWeight(Text::FontWeight weight);

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
  Label& SetFontWidth(Text::FontWidth width);

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
  Label& SetFontSlant(Text::FontSlant slant);

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
  Label& SetTextBackgroundColor(const UiColor& color);

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
   * @param[in] underline The underline configuration.
   */
  Label& SetUnderline(const Text::Underline& underline);

  /**
   * @brief Clears the underline style.
   */
  void ClearUnderline();

  /**
   * @brief Sets the shadow style.
   *
   * @param[in] shadow The shadow configuration.
   */
  Label& SetShadow(const Text::Shadow& shadow);

  /**
   * @brief Clears the shadow style.
   */
  void ClearShadow();

  /**
   * @brief Sets the outline style.
   *
   * @param[in] outline The outline configuration.
   */
  Label& SetOutline(const Text::Outline& outline);

  /**
   * @brief Clears the outline style.
   */
  void ClearOutline();

  /**
   * @brief Sets the line-through style.
   *
   * @param[in] lineThrough The line-through configuration.
   */
  Label& SetLineThrough(const Text::LineThrough& lineThrough);

  /**
   * @brief Clears the line-through style.
   */
  void ClearLineThrough();

  /**
   * @brief Sets the bevel style.
   *
   * @param[in] bevel The bevel configuration.
   */
  Label& SetBevel(const Text::Bevel& bevel);

  /**
   * @brief Clears the bevel style.
   */
  void ClearBevel();

  /**
   * @brief Sets the text fit range.
   *
   * Text fit selects the largest font size within the configured range
   * that fits into the available layout space.
   *
   * @note Text fit is designed for bounded layout sizes. It is recommended to
   * explicitly specify width and height when using this feature.
   * When width or height is WRAP_CONTENT, measurement is performed using the
   * maximum font size in the configured range.
   *
   * @param[in] range The text fit range configuration.
   */
  Label& SetTextFit(const Text::FitRange& range);

  /**
   * @brief Sets the text fit candidates.
   *
   * Text fit selects the largest candidate that fits into the available
   * layout space. Each candidate defines a font size and line height.
   *
   * @note Text fit is designed for bounded layout sizes. It is recommended to
   * explicitly specify width and height when using this feature.
   * When width or height is WRAP_CONTENT, measurement uses the maximum fit
   * candidate to determine the size. The maximum candidate is selected by the
   * largest font size, and if equal, by the larger line height.
   *
   * @param[in] candidates The vector of text fit candidates.
   */
  Label& SetTextFit(const Dali::Vector<Text::FitCandidate>& candidates);

  /**
   * @brief Clears the text fit configuration.
   */
  void ClearTextFit();

  /**
   * @brief Sets the font size scale.
   *
   * The scaled font size is calculated from the current font size
   * multiplied by this scale value.
   *
   * @param[in] scale The font size scale.
   */
  Label& SetFontSizeScale(float scale);

  /**
   * @brief Gets the font size scale.
   *
   * @return The font size scale.
   */
  float GetFontSizeScale() const;

  /**
   * @brief Sets the minimum font size scale.
   *
   * If this value is greater than the maximum font size scale,
   * the adjusted font size scale follows this minimum value.
   *
   * @param[in] scale The minimum font size scale.
   */
  Label& SetMinimumFontSizeScale(float scale);

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
  Label& SetMaximumFontSizeScale(float scale);

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
  Label& SetSystemFontSizeScaleEnabled(bool enabled);

  /**
   * @brief Gets whether the system font size scale is applied.
   *
   * @return True if the system font size scale is applied, otherwise false.
   */
  bool IsSystemFontSizeScaleEnabled() const;

  /**
   * @brief Sets the font variation axes.
   *
   * This replaces all previously set font variation axes.
   *
   * If duplicate axis tags are provided, the last value is used.
   *
   * Unsupported axis tags may be ignored depending on the selected font.
   *
   * @param[in] axes The font variation axes.
   */
  Label& SetFontVariation(const Dali::Vector<Text::FontVariationAxis>& axes);

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
   * If the input string is invalid, the font variation is not changed.
   *
   * Unsupported axis tags may be ignored depending on the selected font.
   *
   * @param[in] settings The font variation settings string.
   */
  Label& SetFontVariation(const Dali::String& settings);

  /**
   * @brief Returns the font variation axes.
   *
   * @return The font variation axes.
   */
  Dali::Vector<Text::FontVariationAxis> GetFontVariation() const;

  /**
   * @brief Clears the font variation.
   *
   * This removes all previously set font variation axes.
   */
  void ClearFontVariation();

  /**
   * @brief Sets whether the text is rendered as a cutout.
   *
   * When enabled, the glyph shapes are cut out from the rendered content
   * instead of being filled with the text color.
   *
   * @param[in] enabled True to render the text as a cutout, false to render it normally.
   */
  Label& SetCutoutEnabled(bool enabled);

  /**
   * @brief Gets whether the text is rendered as a cutout.
   *
   * @return True if the text is rendered as a cutout, otherwise false.
   */
  bool IsCutoutEnabled() const;

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
  Label& SetMaskEffect(View view);

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
  Label& SetAsyncRendering(bool asyncRendering);

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
  Label& SetRenderScale(float scale);

  /**
   * @brief Gets the render scale of the text.
   *
   * @return The current render scale.
   */
  float GetRenderScale() const;

  // @CHAIN_END

  // Read Only
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

  // Method
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
  explicit DALI_UI_API Label(Integration::LabelImpl& implementation);

  /**
   * @brief Allows the creation of this Label from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit DALI_UI_API Label(Dali::Internal::CustomActor* internal);
  /// @endcond

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
  DALI_UI_CHAIN_VIEW_METHODS(Label)
};

} // namespace Ui

} // namespace Dali
