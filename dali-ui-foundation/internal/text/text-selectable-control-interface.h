#ifndef DALI_UI_TEXT_SELECTABLE_CONTROL_INTERFACE_H
#define DALI_UI_TEXT_SELECTABLE_CONTROL_INTERFACE_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

namespace Dali
{
namespace Ui
{
using Uint32Pair = std::pair<uint32_t, uint32_t>;
using string     = std::string;

namespace Text
{
/**
 * @brief Interface used by Text::Controller to notify and control text selection.
 *
 * A selectable control implements this interface when it wants to expose
 * selection operations such as selecting a range, selecting all text, clearing
 * selection, querying selected text, and receiving selection change callbacks.
 */
class SelectableControlInterface
{
public:
  /**
   * @brief Virtual destructor.
   */
  virtual ~SelectableControlInterface() = default;

  /**
   * @brief Selects the text in the specified range.
   *
   * The range is specified by character indices in the current text.
   *
   * @param[in] start The start index of the selection.
   * @param[in] end The end index of the selection.
   */
  virtual void SelectText(uint32_t start, uint32_t end) = 0;

  /**
   * @brief Selects the whole text.
   */
  virtual void SelectWholeText() = 0;

  /**
   * @brief Clears the current text selection.
   */
  virtual void ClearSelection() = 0;

  /**
   * @brief Gets the currently selected text.
   *
   * @return The selected text.
   */
  virtual Dali::String GetSelectedText() const = 0;

  /**
   * @brief Sets the current text selection range.
   *
   * Either @p start or @p end may be nullptr to keep the corresponding
   * selection position unchanged.
   *
   * @param[in] start The start index of the selection, or nullptr to ignore.
   * @param[in] end The end index of the selection, or nullptr to ignore.
   */
  virtual void SetTextSelectionRange(const uint32_t* start, const uint32_t* end) = 0;

  /**
   * @brief Gets the current text selection range.
   *
   * @return A pair containing the start and end indices of the selection.
   */
  virtual Uint32Pair GetTextSelectionRange() const = 0;

  /**
   * @brief Called when the text selection range has changed.
   *
   * @param[in] oldStart The previous start index of the selection.
   * @param[in] oldEnd The previous end index of the selection.
   * @param[in] newStart The new start index of the selection.
   * @param[in] newEnd The new end index of the selection.
   */
  virtual void SelectionChanged(uint32_t oldStart, uint32_t oldEnd, uint32_t newStart, uint32_t newEnd) = 0;
};

} // namespace Text
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_TEXT_SELECTABLE_CONTROL_INTERFACE_H
