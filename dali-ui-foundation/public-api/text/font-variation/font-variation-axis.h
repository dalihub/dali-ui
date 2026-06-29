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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace FontVariation
{
/**
 * @brief Describes a single variable font axis.
 *
 * A font variation axis consists of a four-character OpenType axis tag
 * and a numeric value. Examples of axis tags include "wght", "wdth",
 * "slnt", and "opsz".
 *
 * This object is used to configure font variation settings explicitly.
 */
class DALI_UI_API Axis
{
public:
  /**
   * @brief Creates a font variation axis with default values.
   */
  Axis();

  /**
   * @brief Creates a font variation axis with the given parameters.
   *
   * @param[in] tag The OpenType axis tag.
   * @param[in] value The axis value.
   */
  Axis(const Dali::String& tag, float value);

  /**
   * @brief Creates a font variation axis by copying another axis.
   *
   * @param[in] rhs The font variation axis to copy.
   */
  Axis(const Axis& rhs);

  /**
   * @brief Creates a font variation axis by moving another axis.
   *
   * @param[in] rhs The font variation axis to move.
   */
  Axis(Axis&& rhs) noexcept;

  /**
   * @brief Copies another font variation axis to this object.
   *
   * @param[in] rhs The font variation axis to copy.
   */
  Axis& operator=(const Axis& rhs);

  /**
   * @brief Moves another font variation axis to this object.
   *
   * @param[in] rhs The font variation axis to move.
   */
  Axis& operator=(Axis&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~Axis();

public:
  /**
   * @brief Sets the axis tag.
   *
   * @param[in] tag The OpenType axis tag.
   */
  void SetTag(const Dali::String& tag);

  /**
   * @brief Returns the axis tag.
   *
   * @return The OpenType axis tag.
   */
  const Dali::String& GetTag() const;

  /**
   * @brief Sets the axis value.
   *
   * @param[in] value The axis value.
   */
  void SetValue(float value);

  /**
   * @brief Returns the axis value.
   *
   * @return The axis value.
   */
  float GetValue() const;

private:
  class Impl;
  Impl* mImpl{nullptr};
};

} // namespace FontVariation
} // namespace Text
} // namespace Ui
} // namespace Dali
