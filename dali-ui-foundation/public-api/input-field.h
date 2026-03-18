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
class InputFieldImpl;
}

#include "input-field.autogen.h"
/**
 * @brief InputField is a single-line editable text view.
 *
 * It supports user interaction for text input and editing,
 * and handles text layout and rendering.
 */
class DALI_UI_API InputField : public View
{
public:
  // Typedefs

public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized InputField handle.
   *
   * Only derived versions can be instantiated. Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   */
  InputField();

  /**
   * @brief Creates an initialized InputField.
   *
   * @return A handle to a newly allocated Dali resource
   */
  static InputField New();

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object.
   * @param[in] inputField Handle to copy
   */
  InputField(const InputField& inputField);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  InputField(InputField&& rhs) noexcept;

  /**
   * @brief Virtual destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~InputField();

public: // Operators
  /**
   * @brief Copy assignment operator.
   *
   * Changes this handle to point to another real object.
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  InputField& operator=(const InputField& handle);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  InputField& operator=(InputField&& rhs) noexcept;

public: // Static Methods
  /**
   * @brief Downcasts a handle to InputField handle.
   *
   * If handle points to a InputField, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a InputField or an uninitialized handle
   */
  static InputField DownCast(BaseHandle handle);

public: // Setters for chaining
  // @CHAIN_START(InputField, View)
  /**
   * @brief Sets the text.
   *
   * @param[in] text The text to display in UTF-8 format.
   */
  InputField& SetText(const Dali::String& text);

  /**
   * @brief Gets the text.
   *
   * @return The text currently set on the inputField in UTF-8 format.
   */
  Dali::String GetText() const;

  /**
   * @brief Sets the font family of the text.
   *
   * @param[in] fontFamily The requested font family to use.
   */
  InputField& SetFontFamily(const Dali::String& fontFamily);

  /**
   * @brief Gets the font family of the text.
   *
   * @return The font family currently set on the inputField.
   */
  Dali::String GetFontFamily() const;

  /**
   * @brief Sets the font size of the text.
   *
   * @param[in] fontSize The font size in pixels.
   */
  InputField& SetFontSize(float fontSize);

  /**
   * @brief Gets the font size of the text.
   *
   * @return The font size currently set on the inputField, in pixels.
   */
  float GetFontSize() const;

  /**
   * @brief Sets the color of the text.
   *
   * @param[in] color The required text color value.
   */
  InputField& SetTextColor(const Vector4& color);

  /**
   * @brief Gets the color of the text.
   *
   * @return The text color currently set on the inputField.
   */
  const Vector4& GetTextColor() const;

  /**
   * @brief Sets the horizontal alignment of the text within the inputField.
   *
   * @param[in] alignment The horizontal text alignment.
   */
  InputField& SetHorizontalTextAlignment(Text::Alignment alignment);

  /**
   * @brief Gets the horizontal text alignment.
   *
   * @return The horizontal text alignment.
   */
  Text::Alignment GetHorizontalTextAlignment() const;

  /**
   * @brief Sets the vertical alignment of the text within the inputField.
   *
   * @param[in] alignment The vertical text alignment.
   */
  InputField& SetVerticalTextAlignment(Text::Alignment alignment);

  /**
   * @brief Gets the vertical text alignment.
   *
   * @return The vertical text alignment.
   */
  Text::Alignment GetVerticalTextAlignment() const;

  /**
   * @brief Sets the placeholder text displayed when the input field is empty.
   *
   * @param[in] text The placeholder text in UTF-8 encoding.
   */
  InputField& SetPlaceholder(const Dali::String& text);

  /**
   * @brief Gets the placeholder text.
   *
   * @return The placeholder text in UTF-8 encoding.
   */
  Dali::String GetPlaceholder() const;

  /**
   * @brief Sets the color of the placeholder text.
   *
   * @param[in] color The placeholder text color as a Vector4.
   *
   * @return A reference to this InputField for method chaining.
   */
  InputField& SetPlaceholderColor(const Vector4& color);

  /**
   * @brief Gets the color of the placeholder text.
   *
   * @return The placeholder text color as a Vector4.
   */
  const Vector4& GetPlaceholderColor() const;

  /**
   * @brief Sets the width of the text cursor.
   *
   * @param[in] width The cursor width in pixels.
   *
   * @return A reference to this InputField for method chaining.
   */
  InputField& SetCursorWidth(int width);

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
   * @param[in] color The cursor color as a Vector4.
   *
   * @return A reference to this InputField for method chaining.
   */
  InputField& SetCursorColor(const Vector4& color);

  /**
   * @brief Gets the color of the text cursor.
   *
   * @return The cursor color as a Vector4.
   */
  const Vector4& GetCursorColor() const;

  /**
   * @brief Sets the highlight color of the selected text region.
   *
   * @param[in] color The selection highlight color as a Vector4).
   *
   * @return A reference to this InputField for method chaining.
   */
  InputField& SetSelectionColor(const Vector4& color);

  /**
   * @brief Gets the highlight color of the selected text region.
   *
   * @return The selection highlight color as a Vector4.
   */
  const Vector4& GetSelectionColor() const;

  // @CHAIN_END

public: // Signals
  /**
   * @brief This signal is emitted when the text content changes.
   *
   * @return The signal to connect to.
   */
  Signal<void(View)>& TextChangedSignal();

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Internal implementation.
   *
   * @param[in] implementation The InputField implementation
   */
  explicit DALI_UI_API InputField(Integration::InputFieldImpl& implementation);

  /**
   * @brief Allows the creation of this InputField from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit DALI_UI_API InputField(Dali::Internal::CustomActor* internal);
  /// @endcond

public:
  DALI_UI_CHAIN_VIEW_METHODS(InputField)
};

} // namespace Ui

} // namespace Dali
