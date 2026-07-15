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
#include <dali-ui-foundation/public-api/layouts/absolute-layout-params.h>
#include <dali-ui-foundation/public-api/traits/trait-id.h>
#include <dali-ui-foundation/public-api/traits/trait-object.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

/**
 * @brief Trait implementation that stores AbsoluteLayout child parameters.
 */
class AbsoluteLayoutParamsImpl : public TraitObject
{
public:
  explicit AbsoluteLayoutParamsImpl(const AbsoluteLayoutParams& params)
  : mBounds(params.GetBounds()),
    mFlags(params.GetFlags())
  {
  }

  /**
   * @brief Gets the absolute position and size of the child within the layout.
   * @return The layout bounds.
   */
  const LayoutRect& GetBounds() const
  {
    return mBounds;
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
    IntrusivePtr<TraitObject> object = Integration::View::GetTrait(viewImpl, Integration::ReservedTraitId::ABSOLUTE_LAYOUT_PARAMS);
    DALI_ASSERT_DEBUG(!object || (dynamic_cast<AbsoluteLayoutParamsImpl*>(object.Get()) && "ABSOLUTE_LAYOUT_PARAMS trait must be an AbsoluteLayoutParamsImpl"));
    return object ? static_cast<AbsoluteLayoutParamsImpl*>(object.Get()) : nullptr;
  }

protected:
  ~AbsoluteLayoutParamsImpl() override = default;

private:
  LayoutRect          mBounds;
  AbsoluteLayoutFlags mFlags;
};

} // namespace Internal

} // namespace Ui
} // namespace Dali
