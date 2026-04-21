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
#include <dali-ui-foundation/integration-api/trait-impl.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/trait.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

class LayoutCallbacksTraitImpl;

// ============================================================================
// LayoutCallbacksTrait — internal handle
// ============================================================================

/**
 * @brief Internal handle for LayoutCallbacksTrait.
 *
 * Wraps LayoutCallbacksTraitImpl as a Trait so it can be stored in
 * ViewImpl's trait slot. Not exposed to applications.
 */
class LayoutCallbacksTrait : public Trait
{
public:
  LayoutCallbacksTrait() = default;

  static LayoutCallbacksTrait New(LayoutCallbacksTraitImpl* impl);

private:
  explicit LayoutCallbacksTrait(LayoutCallbacksTraitImpl* impl);
};

// ============================================================================
// LayoutCallbacksTraitImpl — implementation
// ============================================================================

/**
 * @brief Internal implementation of LayoutCallbacks trait.
 *
 * Stores measure/arrange callbacks as typed Callback objects.
 */
class LayoutCallbacksTraitImpl : public Integration::TraitImpl
{
public:
  LayoutCallbacksTraitImpl();

  void SetMeasureCallback(MeasureCallback callback);
  void SetArrangeCallback(ArrangeCallback callback);

  MeasureCallback* GetMeasureCallback();
  ArrangeCallback* GetArrangeCallback();

protected:
  ~LayoutCallbacksTraitImpl() override;

  void OnBeforeAttached(TraitId id, View& view) override;
  void OnAttached(TraitId id, View& view) override;
  void OnDetached(TraitId id, View& view) override;
  void OnViewDestroying(ViewImpl* viewImpl) override;

private:
  MeasureCallback mOnMeasure;
  ArrangeCallback mOnArrange;
};

} // namespace Internal
} // namespace Ui
} // namespace Dali
