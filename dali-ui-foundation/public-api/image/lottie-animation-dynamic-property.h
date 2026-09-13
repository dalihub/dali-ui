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
#include <dali/public-api/object/property-value.h>
#include <dali/public-api/signals/callback.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/image/lottie-animation-enumerations.h>

namespace DALI_NAMESPACE
{

namespace Ui
{

namespace LottieAnimation
{

/**
 * @brief Supplies the value a dynamic property takes on each rendered frame.
 *
 * The arguments are the id given to DynamicProperty, the property being driven, and the
 * frame number being rendered. The returned Property::Value must suit that property's type.
 *
 * @note The callback is invoked on a worker thread. Do NOT call DALi APIs from it.
 */
using DynamicPropertyCallback = Callback<Dali::Property::Value(int32_t, ContentProperty, uint32_t)>;

/**
 * @brief A property of the Lottie content whose value a callback supplies each frame.
 *
 * It names what to drive — a key path selecting the content, and the ContentProperty within it —
 * and carries the callback that supplies the value. Ownership of that callback is transferred to
 * the visual once SetDynamicProperty() is called.
 *
 * @note The callback is invoked on a worker thread. Do NOT call DALi APIs from it.
 */
class DALI_UI_API DynamicProperty
{
public:
  /**
   * @brief Creates an empty dynamic property.
   *
   * The id is 0, the key path is empty, the content property is ContentProperty::FILL_COLOR and
   * there is no callback.
   */
  DynamicProperty();

  /**
   * @brief Creates a dynamic property with all of its values.
   *
   * @param[in] id Unique identifier passed to the callback
   * @param[in] keyPath Target layer or element path, e.g. "Layer 1.Ellipse 1.Fill 1", or "**" for all
   * @param[in] contentProperty The content property to drive
   * @param[in] callback Callback returning a Property::Value each frame
   */
  DynamicProperty(int32_t id, const Dali::String& keyPath, ContentProperty contentProperty, DynamicPropertyCallback callback);

  DynamicProperty(DynamicProperty&& rhs) noexcept;
  DynamicProperty& operator=(DynamicProperty&& rhs) noexcept;
  ~DynamicProperty();

  /// The callback it carries can only have one owner, so this type cannot be copied.
  DynamicProperty(const DynamicProperty& rhs)            = delete;
  DynamicProperty& operator=(const DynamicProperty& rhs) = delete;

public:
  /**
   * @brief Sets the identifier passed to the callback.
   *
   * It is how the callback tells apart the properties it serves.
   *
   * @param[in] id Unique identifier passed to the callback
   * @return This dynamic property
   */
  DynamicProperty& SetId(int32_t id);

  /**
   * @brief Gets the identifier passed to the callback.
   *
   * @return The identifier
   */
  int32_t GetId() const;

  /**
   * @brief Sets which layer or element the property belongs to.
   *
   * The path names the layer and the element within it, such as
   * "Layer 1.Ellipse 1.Fill 1". "**" matches every element.
   *
   * @param[in] keyPath Target layer or element path
   * @return This dynamic property
   */
  DynamicProperty& SetKeyPath(const Dali::String& keyPath);

  /**
   * @brief Gets which layer or element the property belongs to.
   *
   * @return The target layer or element path
   */
  const Dali::String& GetKeyPath() const;

  /**
   * @brief Sets which content property is driven.
   *
   * @param[in] contentProperty The content property to drive
   * @return This dynamic property
   */
  DynamicProperty& SetProperty(ContentProperty contentProperty);

  /**
   * @brief Gets which content property is driven.
   *
   * @return The content property
   */
  ContentProperty GetProperty() const;

  /**
   * @brief Sets the callback that supplies a value for each frame.
   *
   * @param[in] callback Callback returning a Property::Value each frame
   * @return This dynamic property
   */
  DynamicProperty& SetCallback(DynamicPropertyCallback callback);

  /**
   * @brief Gets the callback that supplies a value for each frame.
   *
   * @return The callback, empty when none is set
   */
  DynamicPropertyCallback& GetCallback();

  /**
   * @copydoc GetCallback()
   */
  const DynamicPropertyCallback& GetCallback() const;

private:
  class Impl;
  Impl* mImpl{nullptr};
};

} // namespace LottieAnimation

} // namespace Ui

} //namespace DALI_NAMESPACE
