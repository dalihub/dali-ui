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
#include <dali/public-api/common/intrusive-ptr.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/trait-id.h>
#include <dali-ui-foundation/integration-api/trait-impl.h>
#include <dali-ui-foundation/integration-api/view-impl.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

/**
 * @brief Base implementation class for all layout parameter traits.
 *
 * LayoutParamsImpl is a pure data container with no View dependency.
 * Concrete subclasses must implement GetTraitId().
 *
 * Use View::SetLayoutParams() to attach params to a View
 * (which also invalidates the View's measure cache).
 */
class LayoutParamsImpl : public Integration::TraitImpl
{
public:
  /**
   * @brief Returns the ReservedTraitId for this layout params type.
   *
   * @return The TraitId for this layout params type
   */
  virtual Integration::TraitId GetTraitId() const = 0;

protected:
  LayoutParamsImpl()
  : Integration::TraitImpl()
  {
  }

  ~LayoutParamsImpl() override = default;

  void OnBeforeAttached(Integration::TraitId, View&) override
  {
  }

  void OnAttached(Integration::TraitId, View&) override
  {
  }

  void OnDetached(Integration::TraitId, View&) override
  {
  }

  void OnViewDestroying(Integration::ViewImpl*) override
  {
  }
};

} // namespace Internal
} // namespace Ui
} // namespace Dali
