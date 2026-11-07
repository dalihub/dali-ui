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
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/type-info-id.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace DALI_NAMESPACE
{

namespace Ui
{

/**
 * @brief Registers a custom View type with the DALi type registry.
 *
 * An application's ViewImpl subclass works as a plain View without any
 * registration: its type resolves to Ui::View, so every View property, signal
 * and action keeps working. Register the subclass when it needs an identity of
 * its own -- its own type name, creation by type name, or per-type lookup by
 * styling and scripting.
 *
 * Define one ViewRegistration object at namespace scope in the implementation
 * file of the custom view:
 *
 * @code
 * namespace
 * {
 * Dali::BaseHandle CreateMyView()
 * {
 *   Dali::IntrusivePtr<MyViewImpl> impl = new MyViewImpl();
 *   Dali::Ui::View view(*impl);
 *   impl->Initialize();
 *   return view;
 * }
 *
 * Dali::Ui::ViewRegistration myViewRegistration(DALI_TYPE_ID(MyViewImpl), DALI_TYPE_ID(Dali::Ui::ViewImpl), CreateMyView);
 * } // namespace
 * @endcode
 *
 * The registered name is the identifier's name with namespace qualifiers
 * stripped ("MyViewImpl" above) and must be unique within the type registry.
 * Each DALI_TYPE_ID must spell the actual class name -- instances are
 * recognised by matching the registered name against the implementation
 * class's RTTI, so a typedef or alias spelling would not be recognised.
 * Registration is permanent for the lifetime of the process; destroying the
 * ViewRegistration object does not unregister the type.
 *
 * @see Dali::Ui::ViewImpl
 */
class DALI_UI_API ViewRegistration
{
public:
  /**
   * @brief Function registered for creation by type name.
   *
   * Returns a handle to a newly created, initialized instance of the
   * registered view type.
   */
  using CreateFunction = Dali::BaseHandle (*)();

  /**
   * @brief Registers a custom view type under its implementation class name.
   *
   * @param[in] viewImplType Identifier of the view implementation class (a ViewImpl
   *            subclass), e.g. DALI_TYPE_ID(MyViewImpl)
   * @param[in] baseViewImplType Identifier of its nearest registered base implementation
   *            class, typically DALI_TYPE_ID(Dali::Ui::ViewImpl)
   * @param[in] createFunction Function creating an instance, or nullptr if the type
   *            does not need creation by type name
   */
  ViewRegistration(Dali::TypeInfoId viewImplType,
                   Dali::TypeInfoId baseViewImplType,
                   CreateFunction   createFunction = nullptr);

  /**
   * @brief Registers a custom view type under the name of its handle class.
   *
   * Use this form when the handle class and the implementation class have
   * different names: the type is registered under the class name of
   * @p viewType, while instances keep being recognised through the RTTI of
   * @p viewImplType.
   *
   * @param[in] viewType Identifier of the handle class whose name the type is registered under
   * @param[in] viewImplType Identifier of the view implementation class (a ViewImpl subclass)
   * @param[in] baseViewImplType Identifier of its nearest registered base implementation
   *            class, typically DALI_TYPE_ID(Dali::Ui::ViewImpl)
   * @param[in] createFunction Function creating an instance, or nullptr if the type
   *            does not need creation by type name
   */
  ViewRegistration(Dali::TypeInfoId viewType,
                   Dali::TypeInfoId viewImplType,
                   Dali::TypeInfoId baseViewImplType,
                   CreateFunction   createFunction = nullptr);

  /**
   * @brief The name the type is registered under.
   *
   * @return The registered name
   */
  const Dali::String& RegisteredName() const;

private:
  Dali::String mName; ///< Name the type was registered under
};

} // namespace Ui

} //namespace DALI_NAMESPACE
