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

#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/traits/trait-id.h>

namespace Dali
{
namespace Ui
{
class View;
class ViewImpl;

/**
 * @brief Base object for data and behavior attached to a View trait slot.
 *
 * TraitObject provides lifecycle hooks for View-owned on-demand extensions.
 * It is a BaseObject so public trait handles can keep the usual DALi
 * handle/body shape while View stores only objects that explicitly opt into
 * trait semantics.
 */
class DALI_UI_API TraitObject : public BaseObject
{
public:
  /**
   * @brief Called after this trait is attached to a View.
   */
  virtual void OnAttached(TraitId id, View& view);

  /**
   * @brief Called before this trait is detached from a View.
   */
  virtual void OnDetaching(TraitId id, View& view);

  /**
   * @brief Called when the attached View is being destroyed.
   */
  virtual void OnViewDestroying(ViewImpl* viewImpl);

protected:
  TraitObject();
  ~TraitObject() override;
};

} // namespace Ui
} // namespace Dali
