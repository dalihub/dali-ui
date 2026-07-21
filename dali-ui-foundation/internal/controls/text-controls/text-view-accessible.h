#ifndef DALI_UI_INTERNAL_TEXT_CONTROLS_TEXT_VIEW_ACCESSIBLE_H
#define DALI_UI_INTERNAL_TEXT_CONTROLS_TEXT_VIEW_ACCESSIBLE_H

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
 */

// EXTERNAL INCLUDES
#include <dali/devel-api/atspi-interfaces/hypertext.h>
#include <dali/devel-api/atspi-interfaces/text.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-accessible.h>
#include <dali-ui-foundation/internal/controls/text-controls/text-anchor.h>
#include <dali-ui-foundation/internal/text/controller/text-controller.h>

namespace Dali::Ui::Internal
{
/**
 * @brief Common accessibility implementation for Views that expose text.
 *
 * Text range offsets are Unicode code point indices, matching the text
 * controller and the Dali::Accessibility::Text interface. WORD and LINE
 * boundary lookup retains dali-toolkit's ordinal offset behavior.
 */
class TextViewAccessible : public ViewAccessible,
                           public Dali::Accessibility::Text,
                           public Dali::Accessibility::Hypertext
{
public:
  using ViewAccessible::ViewAccessible;

  // Text

  /**
   * @copydoc Dali::Accessibility::Text::GetCharacterCount()
   */
  std::size_t GetCharacterCount() const override;

  /**
   * @copydoc Dali::Accessibility::Text::GetCursorOffset()
   */
  std::size_t GetCursorOffset() const override;

  /**
   * @copydoc Dali::Accessibility::Text::GetRangeExtents()
   */
  Bounds GetRangeExtents(std::size_t startOffset, std::size_t endOffset,
                         Dali::Devel::Accessibility::CoordinateType type) override; // LCOV_EXCL_LINE

  /**
   * @copydoc Dali::Accessibility::Text::GetRangeOfSelection()
   */
  Dali::Devel::Accessibility::Range GetRangeOfSelection(std::size_t selectionIndex) const override; // LCOV_EXCL_LINE

  /**
   * @copydoc Dali::Accessibility::Text::GetText()
   */
  std::string GetText(std::size_t startOffset, std::size_t endOffset) const override;

  /**
   * @copydoc Dali::Accessibility::Text::GetTextAtOffset()
   *
   * @note WORD and LINE boundaries retain dali-toolkit compatibility and treat
   *       @p offset as a boundary ordinal. Returned range offsets are Unicode
   *       code point indices.
   */
  Dali::Devel::Accessibility::Range GetTextAtOffset(std::size_t                              offset,
                                                    Dali::Devel::Accessibility::TextBoundary boundary) const override; // LCOV_EXCL_LINE

  /**
   * @copydoc Dali::Accessibility::Text::SetCursorOffset()
   */
  bool SetCursorOffset(std::size_t offset) override;

  /**
   * @copydoc Dali::Accessibility::Text::RemoveSelection()
   */
  bool RemoveSelection(std::size_t selectionIndex) override;

  /**
   * @copydoc Dali::Accessibility::Text::SetRangeOfSelection()
   */
  bool SetRangeOfSelection(std::size_t selectionIndex, std::size_t startOffset, std::size_t endOffset) override;

  // Hypertext

  /**
   * @copydoc Dali::Accessibility::Hypertext::GetLink()
   */
  Dali::Accessibility::Accessible* GetLink(std::int32_t linkIndex) const override;

  /**
   * @copydoc Dali::Accessibility::Hypertext::GetLinkCount()
   */
  std::int32_t GetLinkCount() const override;

  /**
   * @copydoc Dali::Accessibility::Hypertext::GetLinkIndex()
   */
  std::int32_t GetLinkIndex(std::int32_t characterOffset) const override;

protected:
  /**
   * @copydoc Dali::Accessibility::Accessible::InitDefaultFeatures()
   */
  void InitDefaultFeatures() override;

  /**
   * @brief Converts a non-empty Unicode code point range to a UTF-8 byte range.
   *
   * @param[in] string The UTF-8 string to index
   * @param[in] begin The first Unicode code point index in the range
   * @param[in] end The Unicode code point index immediately after the range
   * @param[out] utf8Begin The first UTF-8 byte offset in the range
   * @param[out] utf8End The UTF-8 byte offset immediately after the range
   * @return True if the code point range is valid, false otherwise
   */
  static bool ConvertToUtf8Range(const std::string& string, std::size_t begin, std::size_t end,
                                 std::size_t& utf8Begin, std::size_t& utf8End);

  /**
   * @brief Converts a Unicode code point index to a UTF-8 byte offset.
   *
   * @param[in] string The UTF-8 string to index
   * @param[in] offset The Unicode code point index
   * @param[out] utf8Offset The corresponding UTF-8 byte offset
   * @return True if the code point index is valid, false otherwise
   */
  static bool ConvertToUtf8Offset(const std::string& string, std::size_t offset, std::size_t& utf8Offset);

  /**
   * @brief Gets the complete text exposed by the View.
   *
   * Hidden input is replaced with the configured substitute character.
   *
   * @return The exposed text
   */
  std::string GetWholeText() const;

  /**
   * @brief Gets the currently displayed placeholder text.
   *
   * @return The placeholder text, or an empty string if no placeholder is displayed
   */
  std::string GetCurrentPlaceholderText() const;

  /**
   * @brief Gets the TextAnchor actors associated with the View.
   *
   * @return The text anchors
   */
  virtual const std::vector<Ui::TextAnchor>& GetTextAnchors() const = 0;

  /**
   * @brief Gets the controller that owns the View's text model.
   *
   * @return The text controller
   */
  virtual Ui::Text::ControllerPtr GetTextController() const = 0;

  /**
   * @brief Gets the substitute character used for hidden text.
   *
   * @return The substitute character as a Unicode code point
   */
  virtual std::uint32_t GetSubstituteCharacter() const;

  /**
   * @brief Checks whether the exposed text must be replaced with substitute characters.
   *
   * @return True if the text is hidden, false otherwise
   */
  virtual bool IsHiddenInput() const;

  /**
   * @brief Checks whether the View supports text selection operations.
   *
   * @return True if selection operations are supported, false otherwise
   */
  virtual bool SupportsSelection() const;
};

} // namespace Dali::Ui::Internal

#endif // DALI_UI_INTERNAL_TEXT_CONTROLS_TEXT_VIEW_ACCESSIBLE_H
