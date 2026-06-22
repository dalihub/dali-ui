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
#include <dali/public-api/object/weak-handle.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/views/view/interactive-trait-impl.h>
#include <dali-ui-foundation/internal/views/view/selectable-trait-impl.h>
#include <dali-ui-foundation/public-api/trait-object.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

/**
 * @brief Internal trait object stored in the core interaction trait slot.
 *
 * Public trait handles such as InteractiveTrait and SelectableTrait can point to
 * this object. Individual trait implementations are created on demand and share
 * the same View attachment lifecycle.
 */
class CoreInteractionObject : public TraitObject
{
public:
  CoreInteractionObject();

  InteractiveTraitImpl& EnsureInteractiveTraitImpl();
  SelectableTraitImpl&  EnsureSelectableTraitImpl();

  InteractiveTraitImpl* GetInteractiveTraitImpl() const;
  SelectableTraitImpl*  GetSelectableTraitImpl() const;

protected:
  ~CoreInteractionObject() override;

  void OnAttached(TraitId id, View& view) override;
  void OnDetaching(TraitId id, View& view) override;
  void OnViewDestroying(ViewImpl* viewImpl) override;

private:
  WeakHandle<View>                      mOwner;
  std::unique_ptr<InteractiveTraitImpl> mInteractiveTraitImpl;
  std::unique_ptr<SelectableTraitImpl>  mSelectableTraitImpl;
};

} // namespace Internal
} // namespace Ui
} // namespace Dali
