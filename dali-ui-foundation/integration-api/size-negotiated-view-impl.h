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
#include <dali/devel-api/actors/actor-enumerations-devel.h>
#include <dali/devel-api/actors/size-negotiated-actor.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/views/view-impl.h>

namespace Dali::Ui
{

namespace Internal
{
class ViewDataImpl;
} // namespace Internal

namespace Integration
{

/**
 * @brief This is the internal implementation class for a View that requires
 * the deprecated Size Negotiation.
 */
class DALI_UI_API SizeNegotiatedViewImpl : public ViewImpl, public SizeNegotiatedActor
{
private:
  friend class Internal::ViewDataImpl;

protected:
  // ============================================================
  // protected: ABI-frozen virtual API
  // WARNING: Do NOT add, reorder, remove, or change signatures.
  // ============================================================

  /**
   * @brief Destructor. A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~SizeNegotiatedViewImpl();

  // ============================================================
  // protected: Framework overrides (SizeNegotiatedActor)
  // ============================================================

  /**
   * @copydoc Dali::SizeNegotiatedActor::OnRelayout()
   */
  void OnRelayout(const Vector2& size, RelayoutContainer& container) override;

  /**
   * @copydoc Dali::SizeNegotiatedActor::OnSetResizePolicy()
   */
  void OnSetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension) override;

  /**
   * @copydoc Dali::SizeNegotiatedActor::GetNaturalSize()
   */
  Vector3 GetNaturalSize() override;

  /**
   * @copydoc Dali::SizeNegotiatedActor::CalculateChildSize()
   */
  float CalculateChildSize(const Dali::Actor& child, Dimension::Type dimension) override;

  /**
   * @copydoc Dali::SizeNegotiatedActor::GetHeightForWidth()
   */
  float GetHeightForWidth(float width) override;

  /**
   * @copydoc Dali::SizeNegotiatedActor::GetWidthForHeight()
   */
  float GetWidthForHeight(float height) override;

  /**
   * @copydoc Dali::SizeNegotiatedActor::RelayoutDependentOnChildren()
   */
  bool RelayoutDependentOnChildren(Dimension::Type dimension = Dimension::ALL_DIMENSIONS) override;

  /**
   * @copydoc Dali::SizeNegotiatedActor::OnCalculateRelayoutSize()
   */
  void OnCalculateRelayoutSize(Dimension::Type dimension) override;

  /**
   * @copydoc Dali::SizeNegotiatedActor::OnLayoutNegotiated()
   */
  void OnLayoutNegotiated(float size, Dimension::Type dimension) override;

  // ============================================================
  // protected: Construction
  // ============================================================

  /**
   * @brief Default constructor.
   */
  SizeNegotiatedViewImpl();

private:
  SizeNegotiatedViewImpl(const SizeNegotiatedViewImpl&)            = delete;
  SizeNegotiatedViewImpl(SizeNegotiatedViewImpl&&)                 = delete;
  SizeNegotiatedViewImpl& operator=(const SizeNegotiatedViewImpl&) = delete;
  SizeNegotiatedViewImpl& operator=(SizeNegotiatedViewImpl&&)      = delete;
};

// // Helpers for public-api forwarding methods

// inline DALI_UI_API ViewImpl& GetImpl(Ui::View& view)
// {
//   DALI_ASSERT_ALWAYS(view);

//   Dali::RefObject& handle = view.GetImplementation();

//   return static_cast<ViewImpl&>(handle);
// }

// inline DALI_UI_API const ViewImpl& GetImpl(const Ui::View& view)
// {
//   DALI_ASSERT_ALWAYS(view);

//   const Dali::RefObject& handle = view.GetImplementation();

//   return static_cast<const ViewImpl&>(handle);
// }

} // namespace Integration

} // namespace Dali::Ui
