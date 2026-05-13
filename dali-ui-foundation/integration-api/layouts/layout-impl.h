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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/layouts/layout.h>
#include <dali-ui-foundation/public-api/view-impl.h>

namespace Dali
{
namespace Ui
{

class LayoutManager;

namespace Internal
{
class LayoutManagerObject;
} // namespace Internal

namespace Integration
{

class LayoutImpl;
using LayoutImplPtr = IntrusivePtr<LayoutImpl>;

/**
 * @brief This is the internal implementation class for Layout.
 *
 * Layout extends View with a LayoutManager that defines the layout algorithm.
 * The LayoutManager is stored in a trait slot (ReservedTraitId::LAYOUT_MANAGER),
 * so there is no per-instance member variable overhead.
 *
 * @see LayoutManager for the layout algorithm interface
 */
class DALI_UI_API LayoutImpl : public ViewImpl
{
public:
  /**
   * @brief Creates a new Layout implementation without a LayoutManager.
   *
   * @return An IntrusivePtr to the new LayoutImpl
   */
  static LayoutImplPtr New();

public: // LayoutManager API
  /**
   * @brief Gets the layout manager from the LayoutManager object.
   *
   * @return Pointer to the layout manager, or nullptr if not set
   */
  LayoutManager* GetLayoutManager() const;

  /**
   * @brief Checks if this layout has a layout manager.
   *
   * @return True if a layout manager is set
   */
  bool HasLayoutManager() const;

protected: // From ViewImpl
  /**
   * @copydoc ViewImpl::OnMeasure
   */
  MeasuredSize OnMeasure(float widthConstraint, float heightConstraint) override;

  /**
   * @copydoc ViewImpl::OnArrange
   */
  MeasuredSize OnArrange(const LayoutRect& bounds) override;

  /**
   * @copydoc ViewImpl::OnFocusRequested
   *
   * Iterates visible children calling RequestFocus() on each.
   * Returns the first child that accepts focus. If no child accepts,
   * falls back to ViewImpl::OnFocusRequested() (self).
   */
  View OnFocusRequested() override;

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~LayoutImpl();

  /**
   * @brief LayoutImpl constructor.
   */
  LayoutImpl();

  /**
   * @brief Stores a LayoutManager object on this Layout.
   *
   * Must be called in OnInitialize() of derived classes, not in constructors
   * (Self() is not available during construction).
   *
   * @param[in] layoutManager The layout manager (ownership transferred)
   */
  void SetLayoutManager(LayoutManager* layoutManager);

private:
  // Not copyable or movable
  LayoutImpl(const LayoutImpl&)            = delete;
  LayoutImpl(LayoutImpl&&)                 = delete;
  LayoutImpl& operator=(const LayoutImpl&) = delete;
  LayoutImpl& operator=(LayoutImpl&&)      = delete;
};

} // namespace Integration

// Helpers for public-api forwarding methods
inline DALI_UI_API Integration::LayoutImpl& GetImpl(Layout& layout)
{
  DALI_ASSERT_ALWAYS(layout);
  Dali::RefObject& handle = layout.GetImplementation();
  return static_cast<Integration::LayoutImpl&>(handle);
}

inline DALI_UI_API const Integration::LayoutImpl& GetImpl(const Layout& layout)
{
  DALI_ASSERT_ALWAYS(layout);
  const Dali::RefObject& handle = layout.GetImplementation();
  return static_cast<const Integration::LayoutImpl&>(handle);
}
} // namespace Ui
} // namespace Dali
