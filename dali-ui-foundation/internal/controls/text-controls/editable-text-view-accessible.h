#ifndef DALI_UI_INTERNAL_TEXT_CONTROLS_EDITABLE_TEXT_VIEW_ACCESSIBLE_H
#define DALI_UI_INTERNAL_TEXT_CONTROLS_EDITABLE_TEXT_VIEW_ACCESSIBLE_H

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
#include <dali/devel-api/atspi-interfaces/editable-text.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/controls/text-controls/text-view-accessible.h>

namespace Dali::Ui::Internal
{
/**
 * @brief Common accessibility implementation for editable text Views.
 */
class EditableTextViewAccessible : public TextViewAccessible, public Dali::Accessibility::EditableText
{
public:
  using TextViewAccessible::TextViewAccessible;

  /**
   * @copydoc Dali::Ui::ViewAccessible::CalculateStates()
   */
  Dali::Integration::Accessibility::States CalculateStates() override; // LCOV_EXCL_LINE

  // Text

  /**
   * @copydoc Dali::Accessibility::Text::GetCursorOffset()
   */
  std::size_t GetCursorOffset() const override;

  /**
   * @copydoc Dali::Accessibility::Text::SetCursorOffset()
   */
  bool SetCursorOffset(std::size_t offset) override;

  // EditableText

  /**
   * @copydoc Dali::Accessibility::EditableText::CopyText()
   */
  bool CopyText(std::size_t startPosition, std::size_t endPosition) override;

  /**
   * @copydoc Dali::Accessibility::EditableText::CutText()
   */
  bool CutText(std::size_t startPosition, std::size_t endPosition) override;

  /**
   * @copydoc Dali::Accessibility::EditableText::DeleteText()
   */
  bool DeleteText(std::size_t startPosition, std::size_t endPosition) override;

  /**
   * @copydoc Dali::Accessibility::EditableText::InsertText()
   */
  bool InsertText(std::size_t startPosition, std::string text) override;

  /**
   * @copydoc Dali::Accessibility::EditableText::SetTextContents()
   */
  bool SetTextContents(std::string newContents) override;

protected:
  /**
   * @copydoc Dali::Accessibility::Accessible::InitDefaultFeatures()
   */
  void InitDefaultFeatures() override;

  /**
   * @copydoc Dali::Ui::Internal::TextViewAccessible::SupportsSelection()
   */
  bool SupportsSelection() const override;

  /**
   * @brief Requests a relayout after an accessibility text-edit operation.
   */
  virtual void RequestTextRelayout() = 0;
};

} // namespace Dali::Ui::Internal

#endif // DALI_UI_INTERNAL_TEXT_CONTROLS_EDITABLE_TEXT_VIEW_ACCESSIBLE_H
