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

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/reserved-trait-id.h>
#include <dali-ui-foundation/integration-api/view-integ.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/traits/trait-id.h>
#include <dali-ui-foundation/public-api/traits/trait-object.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

/**
 * @brief Trait implementation that stores StackLayout child parameters.
 */
class StackLayoutParamsImpl : public TraitObject
{
public:
  explicit StackLayoutParamsImpl(const StackLayoutParams& params)
  : mWeight(params.GetWeight()),
    mAlignment(params.GetAlignment())
  {
  }

  /**
   * @brief Gets the layout weight.
   * @return The weight value.
   */
  float GetWeight() const
  {
    return mWeight;
  }

  LayoutAlignment GetAlignment() const
  {
    return mAlignment;
  }

  /**
   * @brief Retrieves the StackLayoutParams trait attached to a view, if any.
   * @param[in] viewImpl The view implementation to query.
   * @return Pointer to the params, or nullptr if not attached.
   */
  static StackLayoutParamsImpl* Get(ViewImpl& viewImpl)
  {
    IntrusivePtr<TraitObject> object = Integration::View::GetTrait(viewImpl, Integration::ReservedTraitId::STACK_LAYOUT_PARAMS);
    DALI_ASSERT_DEBUG(!object || (dynamic_cast<StackLayoutParamsImpl*>(object.Get()) && "STACK_LAYOUT_PARAMS trait must be a StackLayoutParamsImpl"));
    return object ? static_cast<StackLayoutParamsImpl*>(object.Get()) : nullptr;
  }

protected:
  ~StackLayoutParamsImpl() override = default;

private:
  float           mWeight;
  LayoutAlignment mAlignment;
};

} // namespace Internal

} // namespace Ui
} // namespace Dali
