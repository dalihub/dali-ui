#ifndef DALI_UI_TEXT_SELECTION_POPUP_CALLBACK_INTERFACE_H
#define DALI_UI_TEXT_SELECTION_POPUP_CALLBACK_INTERFACE_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES

namespace Dali
{
namespace Ui
{
namespace Text
{
/**
 * @brief Temp selection operations.
 */
enum class InputCommandType
{
  CUT = 0,
  COPY,
  PASTE,
  SELECT,
  SELECT_ALL,
  CLIPBOARD,
  NONE,
};
} //namespace Text

/**
 * @brief Interface used to receive the TextSelectionPopup's button callbacks.
 */
class TextSelectionPopupCallbackInterface
{
public:
  /**
   * @brief Virtual destructor.
   */
  virtual ~TextSelectionPopupCallbackInterface()
  {
  }

  /**
   * @brief Called when a button is touched.
   *
   * @param[in] button The button identifier.
   */
  virtual void TextPopupButtonTouched(Dali::Ui::Text::InputCommandType button) = 0;
};

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_TEXT_SELECTION_POPUP_CALLBACK_INTERFACE_H
