#ifndef DALI_UI_INTERNAL_INPUT_EDITOR_ACCESSIBLE_H
#define DALI_UI_INTERNAL_INPUT_EDITOR_ACCESSIBLE_H

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
 * @brief Connects InputEditorImpl with the common editable-text accessibility implementation.
 */
class InputEditorAccessible final : public Internal::EditableTextViewAccessible
{
public:
  using EditableTextViewAccessible::EditableTextViewAccessible;

  /**
   * @copydoc Dali::Ui::ViewAccessible::GetNameRaw()
   */
  std::pair<std::string, bool> GetNameRaw() const override;

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
   * @copydoc Dali::Ui::Internal::EditableTextViewAccessible::RequestTextRelayout()
   */
  void RequestTextRelayout() override;
};

} // namespace Dali::Ui::Integration

#endif // DALI_UI_INTERNAL_INPUT_EDITOR_ACCESSIBLE_H
