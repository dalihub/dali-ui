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
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/ui-style.h>

namespace Dali
{
namespace Ui
{
namespace Provider
{

namespace Internal
{
class UiStylePrivate;
}

/**
 * @brief Base implementation for immutable UI style objects.
 *
 * Component style implementations should derive from UiStyleImpl and expose
 * lightweight public handle classes derived from UiStyle.
 */
class DALI_UI_API UiStyleImpl : public BaseObject
{
protected:
  /**
   * @brief Constructor.
   */
  UiStyleImpl();

  /**
   * @brief Destructor.
   */
  ~UiStyleImpl() override;

private:
  UiStyleImpl(const UiStyleImpl&)            = delete;
  UiStyleImpl(UiStyleImpl&&)                 = delete;
  UiStyleImpl& operator=(const UiStyleImpl&) = delete;
  UiStyleImpl& operator=(UiStyleImpl&&)      = delete;

private:
  Internal::UiStylePrivate* mPrivate;
};

inline DALI_UI_API UiStyleImpl& GetImpl(UiStyle& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<UiStyleImpl&>(handle);
}

inline DALI_UI_API const UiStyleImpl& GetImpl(const UiStyle& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const UiStyleImpl&>(handle);
}

} // namespace Provider
} // namespace Ui
} // namespace Dali
