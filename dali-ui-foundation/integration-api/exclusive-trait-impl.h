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
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/trait-impl.h>

namespace Dali
{

namespace UI
{

namespace Integration
{
/**
 * @brief Internal implementation of Trait.
 */
class DALI_UI_API ExclusiveTraitImpl : public TraitImpl
{
protected:
  /**
   * @brief Creates an uninitialized ExclusiveTrait.
   */
  ExclusiveTraitImpl();

  /**
   * @brief Destructor.
   */
  virtual ~ExclusiveTraitImpl() override;

  /**
   * @copydoc Dali::UI::Integration::TraitImpl::OnBeforeAttached
   */
  void OnBeforeAttached(TraitId id, View& view) override;

private:

  WeakHandle<View> mOwner;
};

} // namespace Integration

} // namespace UI

} // namespace Dali
