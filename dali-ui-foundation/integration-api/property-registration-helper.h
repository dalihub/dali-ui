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
#include <dali/devel-api/object/property-helper-devel.h>

/**
 * @brief Helper macros for property registration using an external property index type.
 *
 * These helpers allow implementation classes to register properties by reusing
 * an externally defined property index enum, such as a public property index,
 * instead of defining a local Property enum in the implementation class.
 *
 * The helper macros are intended to be used inside a type registration block
 * where a local variable named @c typeRegistration is available, for example:
 *
 * @code
 * DALI_TYPE_REGISTRATION_BEGIN(LabelImpl, ViewImpl, Create)
 * DALI_PROPERTY_REGISTRATION_EXTERNAL(Ui::Text, LabelPropertyIndex, Ui::Integration, LabelImpl, "text", STRING, TEXT)
 * DALI_TYPE_REGISTRATION_END()
 * @endcode
 */

/**
 * @brief Registers a property using an external property index definition.
 */
#define DALI_PROPERTY_REGISTRATION_EXTERNAL_INTERNAL(count, typeRegistrationObject, indexNamespace, indexType, objectNamespace, objectType, text, valueType, enumIndex)                                                             \
  Dali::PropertyRegistration DALI_TOKEN_PASTE(property, count)(typeRegistrationObject, text, indexNamespace::indexType::enumIndex, Dali::Property::valueType, &objectNamespace::objectType::SetProperty, &objectType::GetProperty); \
  static_assert((static_cast<int>(indexNamespace::indexType::enumIndex) - static_cast<int>(indexNamespace::indexType::PROPERTY_START_INDEX)) == count, "External property index must match registration order.");

/**
 * @brief Registers a read-only property using an external property index definition.
 */
#define DALI_PROPERTY_REGISTRATION_READ_ONLY_EXTERNAL_INTERNAL(count, typeRegistrationObject, indexNamespace, indexType, objectNamespace, objectType, text, valueType, enumIndex)                 \
  Dali::PropertyRegistration DALI_TOKEN_PASTE(property, count)(typeRegistrationObject, text, indexNamespace::indexType::enumIndex, Dali::Property::valueType, nullptr, &objectType::GetProperty); \
  static_assert((static_cast<int>(indexNamespace::indexType::enumIndex) - static_cast<int>(indexNamespace::indexType::PROPERTY_START_INDEX)) == count, "External property index must match registration order.");

/**
 * @brief Registers an animatable property with a default value using an external property index definition.
 */
#define DALI_ANIMATABLE_PROPERTY_REGISTRATION_WITH_DEFAULT_EXTERNAL_INTERNAL(count, typeRegistrationObject, indexNamespace, indexType, objectNamespace, objectType, text, value, enumIndex) \
  Dali::AnimatablePropertyRegistration DALI_TOKEN_PASTE(property, count)(typeRegistrationObject, text, indexNamespace::indexType::enumIndex, value, &objectNamespace::objectType::SetProperty, &objectType::GetProperty);

/**
 * @brief Registers an animatable property component using an external property index definition.
 */
#define DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION_EXTERNAL_INTERNAL(count, typeRegistrationObject, indexNamespace, indexType, objectNamespace, objectType, text, enumIndex, baseEnumIndex, componentIndex) \
  Dali::AnimatablePropertyComponentRegistration DALI_TOKEN_PASTE(property, count)(typeRegistrationObject, text, indexNamespace::indexType::enumIndex, indexNamespace::indexType::baseEnumIndex, componentIndex);

/**
 * @brief Registers an animatable property using an external property index definition.
 */
#define DALI_ANIMATABLE_PROPERTY_REGISTRATION_EXTERNAL_INTERNAL(count, typeRegistrationObject, indexNamespace, indexType, objectNamespace, objectType, text, valueType, enumIndex) \
  Dali::AnimatablePropertyRegistration DALI_TOKEN_PASTE(property, count)(typeRegistrationObject, text, indexNamespace::indexType::enumIndex, Dali::Property::valueType, &objectNamespace::objectType::SetProperty, &objectType::GetProperty);

/**
 * @brief Registers a property using an external property index definition.
 *
 * This macro expects a local variable named @c typeRegistration to be available
 * in the current scope.
 */
#define DALI_PROPERTY_REGISTRATION_EXTERNAL(indexNamespace, indexType, objectNamespace, objectType, text, valueType, enumIndex) \
  DALI_PROPERTY_REGISTRATION_EXTERNAL_INTERNAL(__COUNTER__, typeRegistration, indexNamespace, indexType, objectNamespace, objectType, text, valueType, enumIndex)

/**
 * @brief Registers a read-only property using an external property index definition.
 *
 * This macro expects a local variable named @c typeRegistration to be available
 * in the current scope.
 */
#define DALI_PROPERTY_REGISTRATION_READ_ONLY_EXTERNAL(indexNamespace, indexType, objectNamespace, objectType, text, valueType, enumIndex) \
  DALI_PROPERTY_REGISTRATION_READ_ONLY_EXTERNAL_INTERNAL(__COUNTER__, typeRegistration, indexNamespace, indexType, objectNamespace, objectType, text, valueType, enumIndex)

/**
 * @brief Registers an animatable property with a default value using an external property index definition.
 *
 * This macro expects a local variable named @c typeRegistration to be available
 * in the current scope.
 */
#define DALI_ANIMATABLE_PROPERTY_REGISTRATION_WITH_DEFAULT_EXTERNAL(indexNamespace, indexType, objectNamespace, objectType, text, value, enumIndex) \
  DALI_ANIMATABLE_PROPERTY_REGISTRATION_WITH_DEFAULT_EXTERNAL_INTERNAL(__COUNTER__, typeRegistration, indexNamespace, indexType, objectNamespace, objectType, text, value, enumIndex)

/**
 * @brief Registers an animatable property component using an external property index definition.
 *
 * This macro expects a local variable named @c typeRegistration to be available
 * in the current scope.
 */
#define DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION_EXTERNAL(indexNamespace, indexType, objectNamespace, objectType, text, enumIndex, baseEnumIndex, componentIndex) \
  DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION_EXTERNAL_INTERNAL(__COUNTER__, typeRegistration, indexNamespace, indexType, objectNamespace, objectType, text, enumIndex, baseEnumIndex, componentIndex)

/**
 * @brief Registers an animatable property using an external property index definition.
 *
 * This macro expects a local variable named @c typeRegistration to be available
 * in the current scope.
 */
#define DALI_ANIMATABLE_PROPERTY_REGISTRATION_EXTERNAL(indexNamespace, indexType, objectNamespace, objectType, text, valueType, enumIndex) \
  DALI_ANIMATABLE_PROPERTY_REGISTRATION_EXTERNAL_INTERNAL(__COUNTER__, typeRegistration, indexNamespace, indexType, objectNamespace, objectType, text, valueType, enumIndex)
