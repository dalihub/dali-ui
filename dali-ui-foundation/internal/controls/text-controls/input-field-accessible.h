#ifndef DALI_UI_INTERNAL_INPUT_FIELD_ACCESSIBLE_H
#define DALI_UI_INTERNAL_INPUT_FIELD_ACCESSIBLE_H

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

#include <dali-ui-foundation/internal/controls/text-controls/editable-text-view-accessible.h>

namespace Dali::Ui::Integration
{
/**
 * @brief Connects InputFieldImpl with the common editable-text accessibility implementation.
 */
class InputFieldAccessible final : public Internal::EditableTextViewAccessible
{
public:
  using EditableTextViewAccessible::EditableTextViewAccessible;

  /**
   * @copydoc Dali::Ui::ViewAccessible::GetNameRaw()
   */
  std::pair<std::string, bool> GetNameRaw() const override;

  /**
   * @brief Emits a text-inserted event without exposing hidden input.
   *
   * @param[in] position The insertion position in Unicode code points
   * @param[in] length The number of inserted Unicode code points
   * @param[in] content The inserted UTF-8 text
   */
  void EmitTextInserted(unsigned int position, unsigned int length, const std::string& content);

  /**
   * @brief Emits a text-deleted event without exposing hidden input.
   *
   * @param[in] position The deletion position in Unicode code points
   * @param[in] length The number of deleted Unicode code points
   * @param[in] content The deleted UTF-8 text
   */
  void EmitTextDeleted(unsigned int position, unsigned int length, const std::string& content);

protected:
  /**
   * @copydoc Dali::Ui::Internal::TextViewAccessible::GetTextAnchors()
   */
  const std::vector<Ui::TextAnchor>& GetTextAnchors() const override;

  /**
   * @copydoc Dali::Ui::Internal::TextViewAccessible::GetTextController()
   */
  Ui::Text::ControllerPtr GetTextController() const override;

  /**
   * @copydoc Dali::Ui::Internal::TextViewAccessible::GetSubstituteCharacter()
   */
  std::uint32_t GetSubstituteCharacter() const override;

  /**
   * @copydoc Dali::Ui::Internal::TextViewAccessible::IsHiddenInput()
   */
  bool IsHiddenInput() const override;

  /**
   * @copydoc Dali::Ui::Internal::EditableTextViewAccessible::RequestTextRelayout()
   */
  void RequestTextRelayout() override;

private:
  /**
   * @brief Replaces hidden event content with the configured substitute character.
   *
   * @param[in] length The number of affected Unicode code points
   * @param[in] content The original UTF-8 event content
   * @return The content safe to expose through accessibility
   */
  std::string GetTextEventContent(unsigned int length, const std::string& content) const;
};

} // namespace Dali::Ui::Integration

#endif // DALI_UI_INTERNAL_INPUT_FIELD_ACCESSIBLE_H
