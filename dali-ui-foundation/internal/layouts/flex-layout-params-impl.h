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
#include <dali-ui-foundation/public-api/layouts/flex-layout-params.h>
#include <dali-ui-foundation/public-api/traits/trait-id.h>
#include <dali-ui-foundation/public-api/traits/trait-object.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

/**
 * @brief Trait implementation that stores FlexLayout child parameters.
 */
class FlexLayoutParamsImpl : public TraitObject
{
public:
  explicit FlexLayoutParamsImpl(const FlexLayoutParams& params)
  : mFlexGrow(params.GetFlexGrow()),
    mFlexShrink(params.GetFlexShrink()),
    mFlexBasis(params.GetFlexBasis()),
    mAlignSelf(params.GetAlignSelf())
  {
  }

  /**
   * @brief Gets the flex grow factor.
   * @return The grow factor.
   */
  float GetFlexGrow() const
  {
    return mFlexGrow;
  }

  /**
   * @brief Gets the flex shrink factor.
   * @return The shrink factor.
   */
  float GetFlexShrink() const
  {
    return mFlexShrink;
  }

  /**
   * @brief Gets the flex basis value.
   * @return The flex basis.
   */
  float GetFlexBasis() const
  {
    return mFlexBasis;
  }

  /**
   * @brief Gets the cross-axis alignment override for this child.
   * @return The align-self value.
   */
  FlexAlign GetAlignSelf() const
  {
    return mAlignSelf;
  }

  /**
   * @brief Retrieves the FlexLayoutParams trait attached to a view, if any.
   * @param[in] viewImpl The view implementation to query.
   * @return Pointer to the params, or nullptr if not attached.
   */
  static FlexLayoutParamsImpl* Get(ViewImpl& viewImpl)
  {
    IntrusivePtr<TraitObject> object = Integration::View::GetTrait(viewImpl, Integration::ReservedTraitId::FLEX_LAYOUT_PARAMS);
    DALI_ASSERT_DEBUG(!object || (dynamic_cast<FlexLayoutParamsImpl*>(object.Get()) && "FLEX_LAYOUT_PARAMS trait must be a FlexLayoutParamsImpl"));
    return object ? static_cast<FlexLayoutParamsImpl*>(object.Get()) : nullptr;
  }

protected:
  ~FlexLayoutParamsImpl() override = default;

private:
  float     mFlexGrow;
  float     mFlexShrink;
  float     mFlexBasis;
  FlexAlign mAlignSelf;
};

} // namespace Internal

} // namespace Ui
} // namespace Dali
