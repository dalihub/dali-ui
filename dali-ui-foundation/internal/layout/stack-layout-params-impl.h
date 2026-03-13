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
#include <dali-ui-foundation/integration-api/trait-id.h>
#include <dali-ui-foundation/internal/layout/layout-params-impl.h>
#include <dali-ui-foundation/public-api/stack-layout-params.h>
#include <dali-ui-foundation/public-api/trait.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

/**
 * @brief Trait implementation that stores StackLayout child parameters.
 */
class StackLayoutParamsImpl : public LayoutParamsImpl
{
public:
  /**
   * @brief Constructs with default weight (0, meaning no extra space allocation).
   */
  StackLayoutParamsImpl()
  : LayoutParamsImpl(),
    mWeight(0.0f)
  {
  }

  StackLayoutParamsImpl(const StackLayoutParamsImpl& other)
  : LayoutParamsImpl(),
    mWeight(other.mWeight)
  {
  }

  Integration::TraitId GetTraitId() const override
  {
    return Integration::ReservedTraitId::STACK_LAYOUT_PARAMS;
  }

  /**
   * @brief Sets the weight for proportional space distribution along the stack axis.
   * @param[in] weight The layout weight (0 means no extra space).
   */
  void SetWeight(float weight)
  {
    mWeight = weight;
  }

  /**
   * @brief Gets the layout weight.
   * @return The weight value.
   */
  float GetWeight() const
  {
    return mWeight;
  }

  /**
   * @brief Retrieves the StackLayoutParams trait attached to a view, if any.
   * @param[in] viewImpl The view implementation to query.
   * @return Pointer to the params, or nullptr if not attached.
   */
  static StackLayoutParamsImpl* Get(Integration::ViewImpl& viewImpl)
  {
    Trait trait = viewImpl.GetTrait(Integration::ReservedTraitId::STACK_LAYOUT_PARAMS);
    if(trait)
    {
      return static_cast<StackLayoutParamsImpl*>(&Ui::GetImpl(trait));
    }
    return nullptr;
  }

  /**
   * @brief Retrieves or creates the StackLayoutParams trait on a view.
   * @param[in] viewImpl The view implementation.
   * @return Reference to the params (newly created if not already attached).
   */
  static StackLayoutParamsImpl& GetOrCreate(Integration::ViewImpl& viewImpl)
  {
    StackLayoutParamsImpl* existing = Get(viewImpl);
    if(existing)
    {
      return *existing;
    }
    IntrusivePtr<StackLayoutParamsImpl> impl(new StackLayoutParamsImpl());
    Trait                               trait = Trait::DownCast(BaseHandle(impl.Get()));
    viewImpl.SetTrait(Integration::ReservedTraitId::STACK_LAYOUT_PARAMS, trait);
    return *impl;
  }

protected:
  ~StackLayoutParamsImpl() override = default;

private:
  float mWeight;
};

} // namespace Internal

inline Internal::StackLayoutParamsImpl& GetImpl(StackLayoutParams& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Internal::StackLayoutParamsImpl&>(handle);
}

inline const Internal::StackLayoutParamsImpl& GetImpl(const StackLayoutParams& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Internal::StackLayoutParamsImpl&>(handle);
}

} // namespace Ui
} // namespace Dali
