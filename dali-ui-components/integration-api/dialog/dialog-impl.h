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
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/dialog/dialog.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

/**
 * @brief Implementation class for Ui::Dialog.
 *
 * Arranges up to three sections (header / body / footer) vertically using a
 * StackLayoutManager. Base class for AlertDialogImpl, therefore the constructor
 * is protected and the class is non-final.
 */
class DALI_UI_COMPONENTS_API DialogImpl : public ViewImpl
{
public:
  /**
   * @brief Creates a new Dialog.
   */
  static Ui::Dialog New();

  // Section accessors
  void     SetHeaderView(Ui::View headerView);
  Ui::View GetHeaderView() const;
  void     SetBodyView(Ui::View bodyView);
  Ui::View GetBodyView() const;
  void     SetFooterView(Ui::View footerView);
  Ui::View GetFooterView() const;

  // Layout control
  void            SetSpacing(float spacing);
  float           GetSpacing() const;
  void            SetLayoutAlignment(LayoutAlignment alignment);
  LayoutAlignment GetLayoutAlignment() const;

protected:
  /**
   * @brief Constructor.
   */
  DialogImpl();

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~DialogImpl();

  /**
   * @brief Second-phase initialization; attaches the vertical stack layout.
   */
  void OnInitialize() override;

private:
  // Rebuilds the child order so sections always appear header -> body -> footer.
  void RebuildOrder();
  // Adds a single section (if valid) with the current alignment applied.
  void AddSection(Ui::View view);
  // Removes a view from Self() if it is currently parented here.
  void DetachIfParented(Ui::View view);
  // Applies the current cross-axis alignment to a section via StackLayoutParams.
  void ApplyAlignment(Ui::View view);

  // Not copyable or movable
  DialogImpl(const DialogImpl&)            = delete;
  DialogImpl(DialogImpl&&)                 = delete;
  DialogImpl& operator=(const DialogImpl&) = delete;
  DialogImpl& operator=(DialogImpl&&)      = delete;

private:
  Ui::View        mHeaderView;
  Ui::View        mBodyView;
  Ui::View        mFooterView;
  LayoutAlignment mAlignment{LayoutAlignment::FILL};
};

} // namespace Integration

// Helpers for public-api forwarding methods

inline Integration::DialogImpl& GetImpl(Ui::Dialog& dialog)
{
  DALI_ASSERT_ALWAYS(dialog);
  Dali::RefObject& handle = dialog.GetImplementation();
  return static_cast<Integration::DialogImpl&>(handle);
}

inline const Integration::DialogImpl& GetImpl(const Ui::Dialog& dialog)
{
  DALI_ASSERT_ALWAYS(dialog);
  const Dali::RefObject& handle = dialog.GetImplementation();
  return static_cast<const Integration::DialogImpl&>(handle);
}

} // namespace Ui
} // namespace Dali
