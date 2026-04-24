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
#include <memory>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/trait-impl.h>
#include <dali-ui-foundation/public-api/trait.h>

namespace Dali
{

namespace Ui
{

class LayoutManager;

namespace Internal
{

class LayoutManagerTraitImpl;

// ============================================================================
// LayoutManagerTrait — internal handle
// ============================================================================

/**
 * @brief Internal handle for LayoutManagerTrait.
 *
 * Wraps LayoutManagerTraitImpl as a Trait so it can be stored in
 * ViewImpl's trait slot. Not exposed to applications.
 */
class LayoutManagerTrait : public Trait
{
public:
  LayoutManagerTrait() = default;

  static LayoutManagerTrait New(LayoutManager* layoutManager);

  static LayoutManagerTrait DownCast(BaseHandle handle);

  LayoutManagerTraitImpl& GetImpl();

private:
  explicit LayoutManagerTrait(LayoutManagerTraitImpl* impl);
};

// ============================================================================
// LayoutManagerTraitImpl — implementation
// ============================================================================

/**
 * @brief Trait implementation that holds a LayoutManager.
 *
 * This trait is attached to Layout views to store their layout algorithm.
 * It is an integration-api concept — applications do not interact with
 * this trait directly.
 */
class LayoutManagerTraitImpl : public Integration::TraitImpl
{
public:
  explicit LayoutManagerTraitImpl(LayoutManager* layoutManager);

  LayoutManager* GetLayoutManager() const;

protected:
  ~LayoutManagerTraitImpl() override;

  void OnBeforeAttached(TraitId id, View& view) override;
  void OnAttached(TraitId id, View& view) override;
  void OnDetached(TraitId id, View& view) override;
  void OnViewDestroying(ViewImpl* viewImpl) override;

private:
  std::unique_ptr<LayoutManager> mLayoutManager;
};

} // namespace Internal
} // namespace Ui
} // namespace Dali
