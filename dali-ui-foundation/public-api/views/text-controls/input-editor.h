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
#include <dali/public-api/adaptor-framework/input-method-context.h>
#include <functional>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/gradient/gradient-base.h>
#include <dali-ui-foundation/public-api/text/font-variation/font-variation.h>
#include <dali-ui-foundation/public-api/text/input-editor-properties.h>
#include <dali-ui-foundation/public-api/text/input-filter.h>
#include <dali-ui-foundation/public-api/text/style/line-through.h>
#include <dali-ui-foundation/public-api/text/style/outline.h>
#include <dali-ui-foundation/public-api/text/style/shadow.h>
#include <dali-ui-foundation/public-api/text/style/underline.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{

namespace Ui
{
class InputEditorAnimationBridge;
class InputEditorAnimationSpec;

namespace Integration
{
class InputEditorImpl;
}

// @ANIMATION_CONFIG(InputEditor, View)
/**
 * @brief InputEditor is a multi-line editable text view.
 *
 * It supports user interaction for text input and editing,
 * and handles multi-line text layout, scrolling, and rendering.
 */
class DALI_UI_API InputEditor : public View
{
public:
  /**
   * @brief Property indices for InputEditor.
   *
   * @note See Dali::Ui::Text::InputEditorPropertyIndex for the underlying property definitions.
   */
  struct Property
  {
    enum
    {
      TEXT                                 = Text::InputEditorPropertyIndex::TEXT,
      FONT_FAMILY                          = Text::InputEditorPropertyIndex::FONT_FAMILY,
      FONT_SIZE                            = Text::InputEditorPropertyIndex::FONT_SIZE,
      TEXT_COLOR                           = Text::InputEditorPropertyIndex::TEXT_COLOR,
      LINE_WRAP_MODE                       = Text::InputEditorPropertyIndex::LINE_WRAP_MODE,
      HORIZONTAL_ALIGNMENT                 = Text::InputEditorPropertyIndex::HORIZONTAL_ALIGNMENT,
      VERTICAL_ALIGNMENT                   = Text::InputEditorPropertyIndex::VERTICAL_ALIGNMENT,
      OVERFLOW_MODE                        = Text::InputEditorPropertyIndex::OVERFLOW_MODE,
      LINE_HEIGHT                          = Text::InputEditorPropertyIndex::LINE_HEIGHT,
      LINE_HEIGHT_MODE                     = Text::InputEditorPropertyIndex::LINE_HEIGHT_MODE,
      PLACEHOLDER                          = Text::InputEditorPropertyIndex::PLACEHOLDER,
      PLACEHOLDER_COLOR                    = Text::InputEditorPropertyIndex::PLACEHOLDER_COLOR,
      SHOW_PLACEHOLDER_ON_FOCUS            = Text::InputEditorPropertyIndex::SHOW_PLACEHOLDER_ON_FOCUS,
      CURSOR_WIDTH                         = Text::InputEditorPropertyIndex::CURSOR_WIDTH,
      CURSOR_COLOR                         = Text::InputEditorPropertyIndex::CURSOR_COLOR,
      CURSOR_BLINK_ENABLED                 = Text::InputEditorPropertyIndex::CURSOR_BLINK_ENABLED,
      CURSOR_BLINK_INTERVAL                = Text::InputEditorPropertyIndex::CURSOR_BLINK_INTERVAL,
      CURSOR_POSITION                      = Text::InputEditorPropertyIndex::CURSOR_POSITION,
      SELECTION_ENABLED                    = Text::InputEditorPropertyIndex::SELECTION_ENABLED,
      SELECTION_COLOR                      = Text::InputEditorPropertyIndex::SELECTION_COLOR,
      SELECTED_TEXT                        = Text::InputEditorPropertyIndex::SELECTED_TEXT,
      SELECTED_TEXT_START                  = Text::InputEditorPropertyIndex::SELECTED_TEXT_START,
      SELECTED_TEXT_END                    = Text::InputEditorPropertyIndex::SELECTED_TEXT_END,
      TEXT_HANDLE_ENABLED                  = Text::InputEditorPropertyIndex::TEXT_HANDLE_ENABLED,
      TEXT_HANDLE_COLOR                    = Text::InputEditorPropertyIndex::TEXT_HANDLE_COLOR,
      CURSOR_HANDLE_IMAGE                  = Text::InputEditorPropertyIndex::CURSOR_HANDLE_IMAGE,
      CURSOR_HANDLE_PRESSED_IMAGE          = Text::InputEditorPropertyIndex::CURSOR_HANDLE_PRESSED_IMAGE,
      SELECTION_HANDLE_IMAGE_LEFT          = Text::InputEditorPropertyIndex::SELECTION_HANDLE_IMAGE_LEFT,
      SELECTION_HANDLE_IMAGE_RIGHT         = Text::InputEditorPropertyIndex::SELECTION_HANDLE_IMAGE_RIGHT,
      SELECTION_HANDLE_PRESSED_IMAGE_LEFT  = Text::InputEditorPropertyIndex::SELECTION_HANDLE_PRESSED_IMAGE_LEFT,
      SELECTION_HANDLE_PRESSED_IMAGE_RIGHT = Text::InputEditorPropertyIndex::SELECTION_HANDLE_PRESSED_IMAGE_RIGHT,
      MAXIMUM_LENGTH                       = Text::InputEditorPropertyIndex::MAXIMUM_LENGTH,
      EDITABLE                             = Text::InputEditorPropertyIndex::EDITABLE,
      LAYOUT_DIRECTION_MODE                = Text::InputEditorPropertyIndex::LAYOUT_DIRECTION_MODE,
      FONT_WEIGHT                          = Text::InputEditorPropertyIndex::FONT_WEIGHT,
      FONT_WIDTH                           = Text::InputEditorPropertyIndex::FONT_WIDTH,
      FONT_SLANT                           = Text::InputEditorPropertyIndex::FONT_SLANT,
      TEXT_BACKGROUND_COLOR                = Text::InputEditorPropertyIndex::TEXT_BACKGROUND_COLOR,
      MINIMUM_FONT_SIZE_SCALE              = Text::InputEditorPropertyIndex::MINIMUM_FONT_SIZE_SCALE,
      MAXIMUM_FONT_SIZE_SCALE              = Text::InputEditorPropertyIndex::MAXIMUM_FONT_SIZE_SCALE,
      SYSTEM_FONT_SIZE_SCALE_ENABLED       = Text::InputEditorPropertyIndex::SYSTEM_FONT_SIZE_SCALE_ENABLED,
      AUTO_GROW_ENABLED                    = Text::InputEditorPropertyIndex::AUTO_GROW_ENABLED,
      TYPING_TEXT_COLOR                    = Text::InputEditorPropertyIndex::TYPING_TEXT_COLOR,
      TYPING_FONT_FAMILY                   = Text::InputEditorPropertyIndex::TYPING_FONT_FAMILY,
      TYPING_FONT_SIZE                     = Text::InputEditorPropertyIndex::TYPING_FONT_SIZE,
      TYPING_FONT_WEIGHT                   = Text::InputEditorPropertyIndex::TYPING_FONT_WEIGHT,
      TYPING_FONT_WIDTH                    = Text::InputEditorPropertyIndex::TYPING_FONT_WIDTH,
      TYPING_FONT_SLANT                    = Text::InputEditorPropertyIndex::TYPING_FONT_SLANT
    };
  };

public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized InputEditor handle.
   *
   * Only derived versions can be instantiated. Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   */
  InputEditor();

  /**
   * @brief Creates an initialized InputEditor.
   *
   * @return A handle to a newly allocated Dali resource
   */
  static InputEditor New();

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object.
   * @param[in] inputEditor Handle to copy
   */
  InputEditor(const InputEditor& inputEditor);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  InputEditor(InputEditor&& rhs) noexcept;

  /**
   * @brief Virtual destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~InputEditor();

public: // Operators
  /**
   * @brief Copy assignment operator.
   *
   * Changes this handle to point to another real object.
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  InputEditor& operator=(const InputEditor& handle);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  InputEditor& operator=(InputEditor&& rhs) noexcept;

  DALI_UI_VIEW_WITH(InputEditor)

public: // Static Methods
  /**
   * @brief Downcasts a handle to InputEditor handle.
   *
   * If handle points to an InputEditor, the downcast produces a valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to an InputEditor or an uninitialized handle
   */
  static InputEditor DownCast(BaseHandle handle);

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
   * @return The text currently set on the inputEditor in UTF-8 format.
   */
  Dali::String GetText() const;

  /**
   * @brief Replaces the current editable text with styled text.
   *
   * The plain text is taken from the StyledText snapshot, and supported visual
   * spans are applied as the initial editable style runs. Subsequent user
   * editing and typing style changes may update the internal style runs. This
   * API does not provide StyledText round-tripping; use GetText() to retrieve
   * plain text.
   *
   * Text edits update the ranges of surviving span attachments. SetText()
   * clears the styled text state.
   *
   * @param[in] styledText The styled text snapshot to apply.
   */
  void SetStyledText(const Text::StyledText& styledText);

  /**
   * @brief Sets the font family of the text.
   *
   * @param[in] fontFamily The requested font family to use.
   */
  void SetFontFamily(const Dali::String& fontFamily);

  /**
   * @brief Gets the font family of the text.
   *
   * @return The font family currently set on the inputEditor.
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
   * @return The font size currently set on the inputEditor, in pixels.
   */
  float GetFontSize() const;

  /**
   * @brief Sets the color of the text.
   *
   * @param[in] color The required text color value.
   */
  void SetTextColor(const UiColor& color);

  /**
   * @brief Gets the color of the text.
   *
   * @return The text color currently set on the inputEditor.
   */
  UiColor GetTextColor();

  /**
   * @brief Sets the gradient used to fill the default monochrome text glyphs.
   *
   * A renderable gradient replaces TextColor RGB for monochrome glyphs that
   * otherwise use the default text color. TextColor alpha remains an opacity
   * multiplier. Explicit foreground colors, color glyphs, embedded items, and
   * text decorations are not replaced.
   *
   * Changing TextColor while a gradient is active does not clear the gradient.
   * Setting Gradient::Base::None(), or a gradient with fewer than two stops,
   * clears the gradient and restores the latest TextColor.
   *
   * Linear, Radial, and Conic gradients are supported, including their units,
   * spread method, start offset, and stop alpha values.
   * Gradient::Units::OBJECT_BOUNDING_BOX uses normalized coordinates in the
   * selected bounds, from (-0.5, -0.5) to (0.5, 0.5), while
   * Gradient::Units::USER_SPACE uses pixel coordinates in the same bounds.
   *
   * @param[in] gradient The authored gradient value.
   */
  void SetTextGradient(const Gradient::Base& gradient);

  /**
   * @brief Gets the text gradient.
   *
   * @return The authored text gradient, or Type::NONE if no gradient is set.
   */
  Gradient::Base GetTextGradient() const;

  /**
   * @brief Sets the bounds used to evaluate text and placeholder gradients.
   *
   * The default is Text::GradientBoundsMode::CONTENT_BOUND. It uses the actual
   * laid-out extents of the currently displayed document or placeholder. During
   * vertical scrolling, the gradient moves with the document content.
   *
   * Text::GradientBoundsMode::VIEW_BOUND uses the full InputEditor view bounds,
   * including padding. Scrolling glyphs then move below a gradient fixed to the
   * control view; the clipping or stencil rectangle does not reduce the bounds.
   *
   * The selected mode is shared by the normal and placeholder gradients.
   *
   * @param[in] mode The text gradient bounds mode.
   */
  void SetTextGradientBoundsMode(Text::GradientBoundsMode mode);

  /**
   * @brief Gets the shared bounds mode used for text and placeholder gradients.
   *
   * @return The current text gradient bounds mode.
   */
  Text::GradientBoundsMode GetTextGradientBoundsMode() const;

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

  /**
   * @brief Sets the horizontal alignment of the text within the inputEditor.
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
   * @brief Sets the vertical alignment of the text within the inputEditor.
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
   * @brief Sets the placeholder text displayed when the input editor is empty.
   *
   * @param[in] text The placeholder text in UTF-8 encoding.
   */
  void SetPlaceholder(const Dali::String& text);

  /**
   * @brief Gets the placeholder text.
   *
   * @return The placeholder text in UTF-8 encoding.
   */
  Dali::String GetPlaceholder() const;

  /**
   * @brief Sets the color of the placeholder text.
   *
   * @param[in] color The placeholder text color as a UiColor.
   */
  void SetPlaceholderColor(const UiColor& color);

  /**
   * @brief Gets the color of the placeholder text.
   *
   * @return The placeholder text color as a UiColor.
   */
  UiColor GetPlaceholderColor();

  /**
   * @brief Sets the gradient used to fill the default monochrome placeholder glyphs.
   *
   * While the placeholder is displayed, a renderable gradient replaces
   * PlaceholderColor RGB and preserves PlaceholderColor alpha as an opacity
   * multiplier. Changing PlaceholderColor does not clear the gradient.
   * Setting Gradient::Base::None(), or a gradient with fewer than two stops,
   * restores the latest PlaceholderColor.
   *
   * This authored value is independent of the normal text gradient.
   * Linear, Radial, and Conic gradients have the same support as TextGradient.
   *
   * @param[in] gradient The authored placeholder gradient value.
   */
  void SetPlaceholderTextGradient(const Gradient::Base& gradient);

  /**
   * @brief Gets the placeholder text gradient.
   *
   * @return The authored placeholder gradient, or Type::NONE if none is set.
   */
  Gradient::Base GetPlaceholderTextGradient() const;

  /**
   * @brief Sets whether the placeholder text is shown when the input editor has focus.
   *
   * @param[in] enabled True to show the placeholder text when focused, false otherwise.
   */
  void SetShowPlaceholderOnFocus(bool enabled);

  /**
   * @brief Returns whether the placeholder text is shown when the input editor has focus.
   *
   * @return True if the placeholder text is shown when focused, false otherwise.
   */
  bool IsPlaceholderShownOnFocus() const;

  /**
   * @brief Sets the width of the text cursor.
   *
   * @param[in] width The cursor width in pixels.
   */
  void SetCursorWidth(int width);

  /**
   * @brief Gets the width of the text cursor.
   *
   * @return The cursor width in pixels.
   */
  int GetCursorWidth() const;

  /**
   * @brief Sets the color of the text cursor
   *
   * This color is applied to both primary and secondary cursors
   * when a split cursor is shown in bidirectional text.
   *
   * @param[in] color The cursor color as a UiColor.
   */
  void SetCursorColor(const UiColor& color);

  /**
   * @brief Gets the color of the text cursor.
   *
   * @return The cursor color as a UiColor.
   */
  UiColor GetCursorColor();

  /**
   * @brief Sets whether the cursor should blink.
   *
   * @param[in] enabled True to enable cursor blinking, false otherwise.
   */
  void SetCursorBlinkEnabled(bool enabled);

  /**
   * @brief Returns whether the cursor is set to blink.
   *
   * @return True if cursor blinking is enabled, false otherwise.
   */
  bool IsCursorBlinkEnabled() const;

  /**
   * @brief Sets the time interval in seconds between cursor on and off states.
   *
   * @param[in] interval The cursor blink interval in seconds.
   */
  void SetCursorBlinkInterval(float interval);

  /**
   * @brief Returns the time interval in seconds between cursor on and off states.
   *
   * @return The cursor blink interval in seconds.
   */
  float GetCursorBlinkInterval() const;

  /**
   * @brief Sets the cursor position.
   *
   * The position is specified as a character index in the current text.
   *
   * @param[in] position The cursor position.
   */
  void SetCursorPosition(uint32_t position);

  /**
   * @brief Returns the current cursor position.
   *
   * The returned value is the character index in the current text.
   *
   * @return The current cursor position.
   */
  uint32_t GetCursorPosition() const;

  /**
   * @brief Sets whether text selection is enabled.
   *
   * @param[in] enabled True to enable text selection, false otherwise.
   */
  void SetSelectionEnabled(bool enabled);

  /**
   * @brief Returns whether text selection is enabled.
   *
   * @return True if text selection is enabled, false otherwise.
   */
  bool IsSelectionEnabled() const;

  /**
   * @brief Sets the highlight color of the selected text region.
   *
   * @param[in] color The selection highlight color as a UiColor.
   */
  void SetSelectionColor(const UiColor& color);

  /**
   * @brief Gets the highlight color of the selected text region.
   *
   * @return The selection highlight color as a UiColor.
   */
  UiColor GetSelectionColor();

  /**
   * @brief Sets whether text editing handles are enabled.
   *
   * Text editing handles include the cursor handle (shown below the insertion cursor)
   * and the left/right selection handles (shown at both ends of the selected text range).
   *
   * @param[in] enabled True to enable text editing handles, false otherwise.
   */
  void SetTextHandleEnabled(bool enabled);

  /**
   * @brief Returns whether text editing handles are enabled.
   *
   * @return True if text editing handles are enabled, false otherwise.
   */
  bool IsTextHandleEnabled() const;

  /**
   * @brief Sets the color of the text editing handles.
   *
   * This color is applied to both the cursor handle and the left/right selection handles.
   *
   * @param[in] color The text editing handle color as a UiColor.
   */
  void SetTextHandleColor(const UiColor& color);

  /**
   * @brief Gets the color of the text editing handles.
   *
   * @return The text editing handle color as a UiColor.
   */
  UiColor GetTextHandleColor() const;

  /**
   * @brief Sets the cursor handle image.
   *
   * The cursor handle is shown below the insertion cursor when there is no selection range.
   *
   * @param[in] image The cursor handle image URL.
   */
  void SetCursorHandleImage(const Dali::String& image);

  /**
   * @brief Gets the cursor handle image.
   *
   * @return The cursor handle image URL.
   */
  Dali::String GetCursorHandleImage() const;

  /**
   * @brief Sets the pressed cursor handle image.
   *
   * The cursor handle is shown below the insertion cursor when there is no selection range.
   *
   * @param[in] image The pressed cursor handle image URL.
   */
  void SetCursorHandlePressedImage(const Dali::String& image);

  /**
   * @brief Gets the pressed cursor handle image.
   *
   * @return The pressed cursor handle image URL.
   */
  Dali::String GetCursorHandlePressedImage() const;

  /**
   * @brief Sets the left selection handle image.
   *
   * The selection handle is shown at the left bound of the selected text range.
   *
   * @param[in] image The left selection handle image URL.
   */
  void SetSelectionHandleImageLeft(const Dali::String& image);

  /**
   * @brief Gets the left selection handle image.
   *
   * @return The left selection handle image URL.
   */
  Dali::String GetSelectionHandleImageLeft() const;

  /**
   * @brief Sets the right selection handle image.
   *
   * The selection handle is shown at the right bound of the selected text range.
   *
   * @param[in] image The right selection handle image URL.
   */
  void SetSelectionHandleImageRight(const Dali::String& image);

  /**
   * @brief Gets the right selection handle image.
   *
   * @return The right selection handle image URL.
   */
  Dali::String GetSelectionHandleImageRight() const;

  /**
   * @brief Sets the pressed left selection handle image.
   *
   * The selection handle is shown at the left bound of the selected text range.
   *
   * @param[in] image The pressed left selection handle image URL.
   */
  void SetSelectionHandlePressedImageLeft(const Dali::String& image);

  /**
   * @brief Gets the pressed left selection handle image.
   *
   * @return The pressed left selection handle image URL.
   */
  Dali::String GetSelectionHandlePressedImageLeft() const;

  /**
   * @brief Sets the pressed right selection handle image.
   *
   * The selection handle is shown at the right bound of the selected text range.
   *
   * @param[in] image The pressed right selection handle image URL.
   */
  void SetSelectionHandlePressedImageRight(const Dali::String& image);

  /**
   * @brief Gets the pressed right selection handle image.
   *
   * @return The pressed right selection handle image URL.
   */
  Dali::String GetSelectionHandlePressedImageRight() const;

  /**
   * @brief Sets the maximum number of characters that can be entered into the InputEditor.
   *
   * @param[in] length The maximum number of characters allowed.
   */
  void SetMaximumLength(int length);

  /**
   * @brief Gets the maximum number of characters allowed in the InputEditor.
   *
   * @return The maximum character count.
   */
  int GetMaximumLength() const;

  /**
   * @brief Sets the input filter.
   *
   * The input filter defines allow and deny patterns used to filter text
   * before it is inserted.
   *
   * If an allow pattern is set, input that does not match the pattern is rejected.
   * If a deny pattern is set, input that matches the pattern is rejected.
   *
   * If both patterns are set, input must match the allow pattern and must not
   * match the deny pattern.
   *
   * @note This filter is applied to inserted input, such as user input, input
   * method commits, and paste operations. It is not applied when text is set
   * directly with SetText().
   * Pass Text::InputFilter::None() to clear the input filter.
   *
   * @param[in] inputFilter The input filter to apply.
   */
  void SetInputFilter(const Text::InputFilter& inputFilter);

  /**
   * @brief Gets the input filter.
   *
   * @return The current input filter, or Text::InputFilter::None() if not set.
   */
  Text::InputFilter GetInputFilter() const;

  /**
   * @brief Sets whether the InputEditor can be edited by user interaction.
   *
   * @param[in] editable True to allow editing, false otherwise.
   */
  void SetEditable(bool editable);

  /**
   * @brief Returns whether the InputEditor can be edited by user interaction.
   *
   * @return True if the InputEditor is editable, false otherwise.
   */
  bool IsEditable() const;

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
   * @note The default layout direction mode of InputEditor is LayoutDirectionMode::INHERIT.
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
   *
   * @return This input editor.
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
   * When enabled, the system font size scale is used instead of
   * the user-defined font size scale before applying the minimum
   * and maximum constraints.
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
   * @brief Enables or disables auto grow behavior.
   *
   * When enabled, InputEditor may invalidate its measured size when text changes.
   * Only dimensions using WRAP_CONTENT are affected.
   * Fixed-size and MATCH_PARENT dimensions keep their normal layout behavior.
   *
   * @param[in] enabled True to enable auto grow, false otherwise.
   */
  void SetAutoGrowEnabled(bool enabled);

  /**
   * @brief Gets whether auto grow behavior is enabled.
   *
   * @return True if auto grow is enabled, otherwise false.
   */
  bool IsAutoGrowEnabled() const;

  /**
   * @brief Sets the text color used for typing.
   *
   * If a text range is selected, the color is applied to the selected text.
   * If the selection is collapsed, the color is applied to text inserted after
   * the current cursor position.
   *
   * @param[in] color The typing text color.
   * @return This input editor.
   */
  void SetTypingTextColor(const UiColor& color);

  /**
   * @brief Gets the text color used for typing.
   *
   * @return The typing text color.
   */
  UiColor GetTypingTextColor() const;

  /**
   * @brief Sets the font family used for typing.
   *
   * If a text range is selected, the font family is applied to the selected text.
   * If the selection is collapsed, the font family is applied to text inserted after
   * the current cursor position.
   *
   * @param[in] fontFamily The typing font family.
   * @return This input editor.
   */
  void SetTypingFontFamily(const Dali::String& fontFamily);

  /**
   * @brief Gets the font family used for typing.
   *
   * @return The typing font family.
   */
  Dali::String GetTypingFontFamily() const;

  /**
   * @brief Sets the font size in pixels used for typing.
   *
   * If a text range is selected, the font size is applied to the selected text.
   * If the selection is collapsed, the font size is applied to text inserted after
   * the current cursor position.
   *
   * @param[in] fontSize The typing font size in pixels.
   * @return This input editor.
   */
  void SetTypingFontSize(float fontSize);

  /**
   * @brief Gets the font size in pixels used for typing.
   *
   * @return The typing font size in pixels.
   */
  float GetTypingFontSize() const;

  /**
   * @brief Sets the font weight used for typing.
   *
   * If a text range is selected, the font weight is applied to the selected text.
   * If the selection is collapsed, the font weight is applied to text inserted after
   * the current cursor position.
   *
   * @param[in] weight The typing font weight.
   * @return This input editor.
   */
  void SetTypingFontWeight(Text::FontWeight weight);

  /**
   * @brief Gets the font weight used for typing.
   *
   * @return The typing font weight.
   */
  Text::FontWeight GetTypingFontWeight() const;

  /**
   * @brief Sets the font width used for typing.
   *
   * If a text range is selected, the font width is applied to the selected text.
   * If the selection is collapsed, the font width is applied to text inserted after
   * the current cursor position.
   *
   * @param[in] width The typing font width.
   * @return This input editor.
   */
  void SetTypingFontWidth(Text::FontWidth width);

  /**
   * @brief Gets the font width used for typing.
   *
   * @return The typing font width.
   */
  Text::FontWidth GetTypingFontWidth() const;

  /**
   * @brief Sets the font slant used for typing.
   *
   * If a text range is selected, the font slant is applied to the selected text.
   * If the selection is collapsed, the font slant is applied to text inserted after
   * the current cursor position.
   *
   * @param[in] slant The typing font slant.
   * @return This input editor.
   */
  void SetTypingFontSlant(Text::FontSlant slant);

  /**
   * @brief Gets the font slant used for typing.
   *
   * @return The typing font slant.
   */
  Text::FontSlant GetTypingFontSlant() const;

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
   * @brief Sets the translatable placeholder resource ID.
   *
   * Registers a localization binding that resolves the given resource ID
   * and displays the localized string for the given resourceId as placeholder text.
   *
   * The displayed placeholder is automatically updated when:
   * - UiLocalizationManager::RefreshBindings() is called
   * - The default domain changes
   * - The localization override or bypass mode changes
   *
   * @note SetPlaceholder() does not clear the translatable placeholder binding.
   *       Use ClearTranslatablePlaceholder() to remove the binding.
   *
   * @param[in] resourceId The resource ID for the localized string (e.g., "IDS_PLACEHOLDER").
   */
  void SetTranslatablePlaceholder(StringView resourceId);

  /**
   * @brief Sets the translatable placeholder resource ID with an explicit domain.
   *
   * Passing an empty domain makes the binding use the current default domain,
   * equivalent to SetTranslatablePlaceholder(resourceId).
   *
   * @param[in] resourceId The resource ID for the localized string (e.g., "IDS_PLACEHOLDER").
   * @param[in] domain The translation domain, or empty to use the default domain.
   */
  void SetTranslatablePlaceholder(StringView resourceId, StringView domain);

  /**
   * @brief Gets the translatable placeholder resource ID.
   *
   * @return The resource ID currently set, or an empty string if not set.
   */
  Dali::String GetTranslatablePlaceholder() const;

  /**
   * @brief Clears the translatable placeholder binding.
   *
   * Removes the localization binding from this InputEditor.
   * The current display placeholder is not changed.
   * Subsequent RefreshBindings() calls will no longer update this InputEditor's placeholder.
   */
  void ClearTranslatablePlaceholder();
  /**
   * @brief Gets the number of lines of text within the current layout width.
   *
   * @note The line count is calculated based on the current width of the input editor,
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
   * @brief Gets the currently selected text.
   *
   * @return The selected text in UTF-8 format.
   */
  Dali::String GetSelectedText() const;

  /**
   * @brief Gets the start index of the currently selected text.
   *
   * @return The start index of the selected text.
   */
  uint32_t GetSelectedTextStart() const;

  /**
   * @brief Gets the end index of the currently selected text.
   *
   * @return The end index of the selected text.
   */
  uint32_t GetSelectedTextEnd() const;
  /**
   * @brief Selects the text within the specified index range.
   *
   * @param[in] startIndex The start index of the selection.
   * @param[in] endIndex The end index of the selection.
   * @return This input editor.
   */
  void SelectText(uint32_t startIndex, uint32_t endIndex);

  /**
   * @brief Selects the whole text.
   *
   * @return This input editor.
   */
  void SelectWholeText();

  /**
   * @brief Clears the current selection.
   *
   * @return This input editor.
   */
  void ClearSelection();

  /**
   * @brief Gets the input method context used by this input editor.
   *
   * The returned context can be used to control input panel behavior and
   * configure application-level input method options.
   *
   * @return The input method context.
   */
  InputMethodContext GetInputMethodContext();

public: // Signals
  /**
   * @brief This signal is emitted when the text content changes.
   *
   * @code
   *   void OnTextChanged(View view);
   * @endcode
   * @return The signal to connect to.
   */
  Signal<void(View)>& TextChangedSignal();

  /**
   * @brief This signal is emitted when the text input reaches the maximum allowed length.
   *
   * The signal is triggered when an attempt is made to insert additional
   * characters beyond the configured maximum length.
   *
   * @code
   *   void OnMaximumLengthReached(View view);
   * @endcode
   *
   * @return The signal to connect to.
   */
  Signal<void(View)>& MaximumLengthReachedSignal();

  /**
   * @brief This signal is emitted when input is rejected by the input filter.
   *
   * The signal is triggered when an attempt is made to insert text that does
   * not match the allow pattern, or matches the deny pattern.
   *
   * @code
   *   void OnInputRejected(View view, Text::InputFilter::RejectReason reason);
   * @endcode
   *
   * The reason indicates whether the input did not match the allow pattern,
   * or matched the deny pattern.
   *
   * @return The signal to connect to.
   */
  Signal<void(View, Text::InputFilter::RejectReason)>& InputRejectedSignal();

  /**
   * @brief This signal is emitted when the cursor position changes.
   *
   * @code
   *   void OnCursorPositionChanged(View view, uint32_t position);
   * @endcode
   *
   * @param[in] view The view whose cursor position changed.
   * @param[in] position The current cursor position.
   *
   * @return The signal to connect to.
   */
  Signal<void(View, uint32_t)>& CursorPositionChangedSignal();

  /**
   * @brief This signal is emitted when text selection starts.
   *
   * @code
   *   void OnSelectionStarted(View view);
   * @endcode
   *
   * @return The signal to connect to.
   */
  Signal<void(View)>& SelectionStartedSignal();

  /**
   * @brief This signal is emitted when the text selection changes.
   *
   * @code
   *   void OnSelectionChanged(View view, uint32_t start, uint32_t end);
   * @endcode
   *
   * @param[in] view The view whose selection changed.
   * @param[in] start The current selection start index.
   * @param[in] end The current selection end index.
   *
   * @return The signal to connect to.
   */
  Signal<void(View, uint32_t, uint32_t)>& SelectionChangedSignal();

  /**
   * @brief This signal is emitted when the text selection is cleared.
   *
   * @code
   *   void OnSelectionCleared(View view);
   * @endcode
   *
   * @return The signal to connect to.
   */
  Signal<void(View)>& SelectionClearedSignal();

  /**
   * @brief This signal is emitted when the current typing style changes.
   *
   * The signal is emitted when the typing style attributes resolved at the
   * current cursor position or selected text range change, for example after
   * cursor movement or selection changes.
   *
   * The mask indicates which typing style attributes changed.
   *
   * @note This signal is intended to notify changes detected from the current
   * cursor position or selection, not to mirror every SetTyping*() API call.
   *
   * @code
   *   void OnTypingStyleChanged(View view, Text::TypingStyle::Mask mask);
   * @endcode
   *
   * @return The signal to connect to.
   */
  Signal<void(View, Text::TypingStyle::Mask)>& TypingStyleChangedSignal();

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Internal implementation.
   *
   * @param[in] implementation The InputEditor implementation
   */
  explicit InputEditor(Integration::InputEditorImpl& implementation);

  /**
   * @brief Allows the creation of this InputEditor from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit InputEditor(Dali::Internal::CustomActor* internal);
  /// @endcond

  // @ANIMATABLE_MANUAL(TextGradientStartOffset, float)
  // @ANIMATABLE_MANUAL(PlaceholderTextGradientStartOffset, float)
public: // Animation
  /**
   * @brief Creates an InputEditorAnimationBridge for this InputEditor.
   *
   * @param[in] animation The Animation to apply to
   * @return An InputEditorAnimationBridge
   */
  InputEditorAnimationBridge Animate(Animation animation);

  /**
   * @brief Creates a new InputEditorAnimationSpec.
   *
   * @return A new InputEditorAnimationSpec
   */
  static InputEditorAnimationSpec NewAnimationSpec();

public:
};

} // namespace Ui

} // namespace Dali
