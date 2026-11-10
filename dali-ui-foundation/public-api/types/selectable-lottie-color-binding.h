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
 */

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-string-view.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/math/int-pair.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/image/lottie-animation-enumerations.h>

namespace DALI_NAMESPACE
{
namespace Ui
{

/**
 * @addtogroup dali_ui_view
 * @{
 */

/**
 * @brief Describes how a Lottie color property resolves selectable state colors.
 *
 * Each binding owns its key path and can be copied independently. Only fill and stroke
 * color properties are accepted.
 *
 * A moved-from binding is invalid and must only be destroyed or assigned a new value.
 * @SINCE_3_0.0
 */
class DALI_UI_API SelectableLottieColorBinding
{
public:
  /**
   * @brief Defines which selectable state color a dynamic property returns.
   * @SINCE_3_0.0
   */
  enum class ColorPolicy : uint8_t
  {
    BY_SELECTION_STATE      = 0, ///< Use the current logical selection state. @SINCE_3_0.0
    ALWAYS_DESELECTED       = 1, ///< Always use the deselected color. @SINCE_3_0.0
    ALWAYS_SELECTED         = 2, ///< Always use the selected color. @SINCE_3_0.0
    SELECTED_IN_FRAME_RANGE = 3  ///< Use selected color only inside the inclusive frame range. @SINCE_3_0.0
  };

  /**
   * @brief Deleted because an empty key path is not a valid binding.
   * @SINCE_3_0.0
   */
  SelectableLottieColorBinding() = delete;

  /**
   * @brief Creates a color binding.
   *
   * A reversed selected-color range is normalized into ascending order.
   * The key path is copied and owned by the binding. @p selectedColorRange is used only
   * when @p colorPolicy is ColorPolicy::SELECTED_IN_FRAME_RANGE.
   *
   * @SINCE_3_0.0
   * @param[in] keyPath The non-empty Lottie property key path
   * @param[in] property The fill-color or stroke-color property
   * @param[in] colorPolicy The rule selecting between state colors
   * @param[in] selectedColorRange The inclusive selected-color range for
   *                               SELECTED_IN_FRAME_RANGE
   * @pre @p keyPath must not be empty.
   * @pre @p property must be LottieAnimation::ContentProperty::FILL_COLOR or
   * LottieAnimation::ContentProperty::STROKE_COLOR.
   */
  SelectableLottieColorBinding(
    Dali::StringView                 keyPath,
    LottieAnimation::ContentProperty property,
    ColorPolicy                      colorPolicy,
    const Int32Pair&                 selectedColorRange = {});

  /**
   * @brief Copy constructor.
   * @SINCE_3_0.0
   * @param[in] rhs The binding to copy
   */
  SelectableLottieColorBinding(const SelectableLottieColorBinding& rhs);

  /**
   * @brief Move constructor.
   * @SINCE_3_0.0
   * @param[in] rhs The binding to move
   */
  SelectableLottieColorBinding(SelectableLottieColorBinding&& rhs) noexcept;

  /**
   * @brief Copy assignment operator.
   * @SINCE_3_0.0
   * @param[in] rhs The binding to copy
   * @return A reference to this binding
   */
  SelectableLottieColorBinding& operator=(const SelectableLottieColorBinding& rhs);

  /**
   * @brief Move assignment operator.
   * @SINCE_3_0.0
   * @param[in] rhs The binding to move
   * @return A reference to this binding
   */
  SelectableLottieColorBinding& operator=(SelectableLottieColorBinding&& rhs) noexcept;

  /**
   * @brief Destructor.
   * @SINCE_3_0.0
   */
  ~SelectableLottieColorBinding();

  /**
   * @brief Returns the owned Lottie property key path.
   * @SINCE_3_0.0
   * @return The key path
   */
  Dali::String GetKeyPath() const;

  /**
   * @brief Returns the bound Lottie property.
   * @SINCE_3_0.0
   * @return The fill-color or stroke-color property
   */
  LottieAnimation::ContentProperty GetProperty() const;

  /**
   * @brief Returns the color selection policy.
   * @SINCE_3_0.0
   * @return The color policy
   */
  ColorPolicy GetColorPolicy() const;

  /**
   * @brief Returns the normalized inclusive selected-color range.
   *
   * The range affects rendering only when GetColorPolicy() returns
   * ColorPolicy::SELECTED_IN_FRAME_RANGE.
   *
   * @SINCE_3_0.0
   * @return The frame range
   */
  Int32Pair GetSelectedColorRange() const;

private:
  struct Impl;
  Dali::UniquePtr<Impl> mImpl;
};

/**
 * @brief A list of selectable Lottie color bindings.
 * @SINCE_3_0.0
 */
using SelectableLottieColorBindings = Dali::Vector<SelectableLottieColorBinding>;

/** @} */

} // namespace Ui
} //namespace DALI_NAMESPACE
