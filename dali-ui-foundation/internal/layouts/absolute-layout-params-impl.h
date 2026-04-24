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
#include <dali/public-api/object/base-handle.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/reserved-trait-id.h>
#include <dali-ui-foundation/integration-api/view-integ.h>
#include <dali-ui-foundation/internal/layouts/layout-params-impl.h>
#include <dali-ui-foundation/public-api/layouts/absolute-layout-params.h>
#include <dali-ui-foundation/public-api/trait-id.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

/**
 * @brief Trait implementation that stores AbsoluteLayout child parameters.
 */
class AbsoluteLayoutParamsImpl : public LayoutParamsImpl
{
public:
  /**
   * @brief Constructs with default bounds (0,0,-1,-1) and no flags.
   */
  AbsoluteLayoutParamsImpl()
  : LayoutParamsImpl(),
    mBounds(0.0f, 0.0f, -1.0f, -1.0f),
    mFlags(AbsoluteLayoutFlags::NONE)
  {
  }

  AbsoluteLayoutParamsImpl(const AbsoluteLayoutParamsImpl& other)
  : LayoutParamsImpl(),
    mBounds(other.mBounds),
    mFlags(other.mFlags)
  {
  }

  TraitId GetTraitId() const override
  {
    return Integration::ReservedTraitId::ABSOLUTE_LAYOUT_PARAMS;
  }

  /**
   * @brief Sets the absolute position and size of the child within the layout.
   * @param[in] bounds The layout bounds (x, y, width, height).
   */
  void SetBounds(const LayoutRect& bounds)
  {
    mBounds = bounds;
  }

  /**
   * @brief Gets the absolute position and size of the child within the layout.
   * @return The layout bounds.
   */
  const LayoutRect& GetBounds() const
  {
    return mBounds;
  }

  void SetX(float x)
  {
    mBounds.SetX(x);
  }

  void SetY(float y)
  {
    mBounds.SetY(y);
  }

  void SetWidth(float width)
  {
    mBounds.SetWidth(width);
  }

  void SetHeight(float height)
  {
    mBounds.SetHeight(height);
  }

  /**
   * @brief Sets the layout flags that control proportional sizing behavior.
   * @param[in] flags The absolute layout flags.
   */
  void SetFlags(AbsoluteLayoutFlags flags)
  {
    mFlags = flags;
  }

  /**
   * @brief Gets the layout flags that control proportional sizing behavior.
   * @return The absolute layout flags.
   */
  AbsoluteLayoutFlags GetFlags() const
  {
    return mFlags;
  }

  /**
   * @brief Retrieves the AbsoluteLayoutParams trait attached to a view, if any.
   * @param[in] viewImpl The view implementation to query.
   * @return Pointer to the params, or nullptr if not attached.
   */
  static AbsoluteLayoutParamsImpl* Get(ViewImpl& viewImpl)
  {
    AbsoluteLayoutParams params = IntegrationView::GetTrait<AbsoluteLayoutParams>(viewImpl, Integration::ReservedTraitId::ABSOLUTE_LAYOUT_PARAMS);
    return params ? static_cast<AbsoluteLayoutParamsImpl*>(&params.GetBaseObject()) : nullptr;
  }

  /**
   * @brief Retrieves or creates the AbsoluteLayoutParams trait on a view.
   * @param[in] viewImpl The view implementation.
   * @return Reference to the params (newly created if not already attached).
   */
  static AbsoluteLayoutParamsImpl& GetOrCreate(ViewImpl& viewImpl)
  {
    AbsoluteLayoutParamsImpl* existing = Get(viewImpl);
    if(existing)
    {
      return *existing;
    }
    AbsoluteLayoutParams params = AbsoluteLayoutParams::New();
    IntegrationView::SetTrait(viewImpl, Integration::ReservedTraitId::ABSOLUTE_LAYOUT_PARAMS, params);
    return static_cast<AbsoluteLayoutParamsImpl&>(params.GetBaseObject());
  }

protected:
  ~AbsoluteLayoutParamsImpl() override = default;

private:
  LayoutRect          mBounds;
  AbsoluteLayoutFlags mFlags;
};

} // namespace Internal

inline Internal::AbsoluteLayoutParamsImpl& GetImpl(AbsoluteLayoutParams& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Internal::AbsoluteLayoutParamsImpl&>(handle);
}

inline const Internal::AbsoluteLayoutParamsImpl& GetImpl(const AbsoluteLayoutParams& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Internal::AbsoluteLayoutParamsImpl&>(handle);
}

} // namespace Ui
} // namespace Dali
