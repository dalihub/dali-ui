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

#include <dali-ui-foundation/public-api/interactive-view-impl.h>
#include <dali-ui-foundation/public-api/selectable-trait.h>
#include <dali-ui-foundation/public-api/selectable-view.h>

namespace Dali
{

namespace Ui
{

using SelectableViewImplPtr = IntrusivePtr<SelectableViewImpl>;

/**
 * @brief Implementation class for SelectableView.
 *
 * SelectableViewImpl is an InteractiveViewImpl subclass that guarantees a
 * SelectableTrait is attached for the lifetime of the view. Component
 * implementations such as SwitchImpl and CheckboxImpl should subclass this
 * instead of InteractiveViewImpl when selection state is part of the component.
 *
 * @see Dali::Ui::SelectableView
 */
class DALI_UI_API SelectableViewImpl : public InteractiveViewImpl
{
public:
  /**
   * @brief Creates a new SelectableViewImpl.
   *
   * @return An intrusive pointer to a newly allocated SelectableViewImpl
   */
  static SelectableViewImplPtr New();

  /**
   * @copydoc ViewImpl::OnInitialize
   *
   * Attaches a SelectableTrait to the view so that selectable behavior is
   * always available without an explicit AsSelectable() call.
   */
  void OnInitialize() override;

  /**
   * @copydoc SelectableView::SelectionChangedSignal()
   */
  Signal<void(View, bool, InputEvent)>& SelectionChangedSignal();

  /**
   * @copydoc SelectableView::IsSelected()
   */
  bool IsSelected() const;

  /**
   * @copydoc SelectableView::SetSelected()
   */
  void SetSelected(bool selected);

  /**
   * @copydoc SelectableView::IsToggleByClickEnabled()
   */
  bool IsToggleByClickEnabled() const;

  /**
   * @copydoc SelectableView::EnableToggleByClick()
   */
  void EnableToggleByClick(bool enabled);

protected:
  /**
   * @brief Gets the guaranteed SelectableTrait.
   *
   * @return The SelectableTrait attached to this view
   */
  SelectableTrait GetSelectableTrait() const;

  /**
   * @brief Constructor.
   */
  SelectableViewImpl();

  /**
   * @brief Destructor.
   */
  ~SelectableViewImpl() override;
};

// Helpers for forwarding methods

inline DALI_UI_API SelectableViewImpl& GetImpl(SelectableView& view)
{
  DALI_ASSERT_ALWAYS(view);

  Dali::RefObject& handle = view.GetImplementation();

  return static_cast<SelectableViewImpl&>(handle);
}

inline DALI_UI_API const SelectableViewImpl& GetImpl(const SelectableView& view)
{
  DALI_ASSERT_ALWAYS(view);

  const Dali::RefObject& handle = view.GetImplementation();

  return static_cast<const SelectableViewImpl&>(handle);
}

} // namespace Ui

} // namespace Dali
