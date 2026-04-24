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

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/trait-interface.h>
#include <dali-ui-foundation/public-api/trait-id.h>
#include <dali-ui-foundation/public-api/trait.h>

namespace Dali
{

namespace Ui
{

namespace Integration
{

/**
 * @brief Base implementation class for traits that need lifecycle callbacks.
 *
 * Inherits from BaseObject (for reference counting / BaseHandle storage)
 * and TraitInterface (for lifecycle callbacks). All lifecycle methods have
 * default empty implementations; subclasses override only what they need.
 */
class DALI_UI_API TraitImpl : public BaseObject, public TraitInterface
{
protected:
  /**
   * @copydoc Dali::Ui::Trait::Trait
   */
  TraitImpl();

  /**
   * @copydoc Dali::Ui::Trait::~Trait
   */
  virtual ~TraitImpl() override;

  void OnBeforeAttached(TraitId id, View& view) override
  {
  }
  void OnAttached(TraitId id, View& view) override
  {
  }
  void OnDetached(TraitId id, View& view) override
  {
  }
  void OnViewDestroying(ViewImpl* viewImpl) override
  {
  }

  TraitImpl(const TraitImpl&)            = delete;
  TraitImpl(TraitImpl&&)                 = delete;
  TraitImpl& operator=(TraitImpl&&)      = delete; // no move()
  TraitImpl& operator=(const TraitImpl&) = delete; // no copy()
};

} // namespace Integration

inline DALI_UI_API Integration::TraitImpl& GetImpl(Trait& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Integration::TraitImpl&>(handle);
}

inline DALI_UI_API const Integration::TraitImpl& GetImpl(const Trait& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Integration::TraitImpl&>(handle);
}

} // namespace Ui

} // namespace Dali
