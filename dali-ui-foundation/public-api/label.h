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
#include <dali-ui-foundation/public-api/text/text-enumerations.h>

// None

namespace Dali
{

namespace Ui
{

// Forward declarations
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
  // Typedefs

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
   * @brief Sets the color of the text.
   *
   * @param[in] color The required text color value.
   */
  Label& SetTextColor(const Vector4& color);

  /**
   * @brief Gets the color of the text.
   *
   * @return The text color currently set on the label.
   */
  const Vector4& GetTextColor() const;

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
   * Setting lineHeight to -1.0f ensures enough vertical space to display
   * the full font metrics (NaturalSize). This behavior is similar to the
   * "Auto" line height option in design tools such as Figma.
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

  // @CHAIN_END

public: // Signals
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

public:
  DALI_UI_CHAIN_VIEW_METHODS(Label)
};

} // namespace Ui

} // namespace Dali
