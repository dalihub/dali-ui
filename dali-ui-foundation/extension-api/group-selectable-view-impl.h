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

#include <dali-ui-foundation/extension-api/selectable-view-impl.h>
#include <dali-ui-foundation/public-api/traits/group-selectable-trait.h>
#include <dali-ui-foundation/public-api/views/group-selectable-view.h>

namespace Dali
{

namespace Ui
{
namespace Extension
{

class GroupSelectableViewImpl;

using GroupSelectableViewImplPtr = IntrusivePtr<GroupSelectableViewImpl>;

/**
 * @brief Implementation class for GroupSelectableView.
 *
 * GroupSelectableViewImpl is a SelectableViewImpl subclass that guarantees a
 * GroupSelectableTrait is attached for the lifetime of the view. Component
 * implementations whose members form a single-selection group should subclass
 * this instead of SelectableViewImpl.
 *
 * @see Dali::Ui::GroupSelectableView
 */
class DALI_UI_API GroupSelectableViewImpl : public SelectableViewImpl
{
public:
  /**
   * @brief Creates a new GroupSelectableViewImpl.
   *
   * @return An intrusive pointer to a newly allocated GroupSelectableViewImpl
   */
  static GroupSelectableViewImplPtr New();

  /**
   * @copydoc ViewImpl::OnInitialize
   *
   * Attaches a GroupSelectableTrait to the view so that single-selection grouping
   * is always available without an explicit AsGroupSelectable() call.
   */
  void OnInitialize() override;

  /**
   * @copydoc GroupSelectableView::SetGroupName()
   */
  void SetGroupName(const Dali::String& name);

  /**
   * @copydoc GroupSelectableView::GetGroupName()
   */
  Dali::String GetGroupName() const;

  /**
   * @copydoc GroupSelectableView::GetGroup()
   */
  SelectionGroup GetGroup() const;

protected:
  /**
   * @brief Gets the guaranteed GroupSelectableTrait.
   *
   * @return The GroupSelectableTrait attached to this view
   */
  GroupSelectableTrait GetGroupSelectableTrait() const;

  /**
   * @brief Constructor.
   */
  GroupSelectableViewImpl();

  /**
   * @brief Destructor.
   */
  ~GroupSelectableViewImpl() override;
};

// Helpers for forwarding methods

inline GroupSelectableViewImpl& GetImpl(GroupSelectableView& view)
{
  DALI_ASSERT_ALWAYS(view);

  Dali::RefObject& handle = view.GetImplementation();

  return static_cast<GroupSelectableViewImpl&>(handle);
}

inline const GroupSelectableViewImpl& GetImpl(const GroupSelectableView& view)
{
  DALI_ASSERT_ALWAYS(view);

  const Dali::RefObject& handle = view.GetImplementation();

  return static_cast<const GroupSelectableViewImpl&>(handle);
}

} // namespace Extension
} // namespace Ui

} // namespace Dali
