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
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/common/unique-ptr.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/image/lottie-animation-enumerations.h>
#include <dali-ui-foundation/public-api/types/selectable-lottie-image.h>

namespace Dali
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
 */
class DALI_UI_API SelectableLottieColorBinding
{
public:
  /**
   * @brief Defines which selectable state color a dynamic property returns.
   */
  enum class ColorPolicy : uint8_t
  {
    BY_SELECTION_STATE      = 0, ///< Use the current logical selection state.
    ALWAYS_DESELECTED       = 1, ///< Always use the deselected color.
    ALWAYS_SELECTED         = 2, ///< Always use the selected color.
    SELECTED_IN_FRAME_RANGE = 3  ///< Use selected color only inside the inclusive frame range.
  };

  /**
   * @brief Deleted because an empty key path is not a valid binding.
   */
  SelectableLottieColorBinding() = delete;

  /**
   * @brief Creates a color binding.
   *
   * A reversed selected-color range is normalized into ascending order.
   * The key path is copied and owned by the binding. @p selectedColorRange is used only
   * when @p colorPolicy is ColorPolicy::SELECTED_IN_FRAME_RANGE.
   *
   * @param[in] keyPath The non-empty Lottie property key path
   * @param[in] property The fill-color or stroke-color property
   * @param[in] colorPolicy The rule selecting between state colors
   * @param[in] selectedColorRange The inclusive selected-color range for
   *                               SELECTED_IN_FRAME_RANGE
   * @pre @p keyPath must not be empty.
   * @pre @p property must be LottieAnimation::VectorProperty::FILL_COLOR or
   * LottieAnimation::VectorProperty::STROKE_COLOR.
   */
  SelectableLottieColorBinding(
    Dali::StringView                         keyPath,
    LottieAnimation::VectorProperty          property,
    ColorPolicy                              colorPolicy,
    const SelectableLottieImage::FrameRange& selectedColorRange = {});

  /**
   * @brief Copy constructor.
   * @param[in] rhs The binding to copy
   */
  SelectableLottieColorBinding(const SelectableLottieColorBinding& rhs);

  /**
   * @brief Move constructor.
   * @param[in] rhs The binding to move
   */
  SelectableLottieColorBinding(SelectableLottieColorBinding&& rhs) noexcept;

  /**
   * @brief Copy assignment operator.
   * @param[in] rhs The binding to copy
   * @return A reference to this binding
   */
  SelectableLottieColorBinding& operator=(const SelectableLottieColorBinding& rhs);

  /**
   * @brief Move assignment operator.
   * @param[in] rhs The binding to move
   * @return A reference to this binding
   */
  SelectableLottieColorBinding& operator=(SelectableLottieColorBinding&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~SelectableLottieColorBinding();

  /**
   * @brief Returns the owned Lottie property key path.
   * @return The key path
   */
  Dali::String GetKeyPath() const;

  /**
   * @brief Returns the bound Lottie property.
   * @return The fill-color or stroke-color property
   */
  LottieAnimation::VectorProperty GetProperty() const;

  /**
   * @brief Returns the color selection policy.
   * @return The color policy
   */
  ColorPolicy GetColorPolicy() const;

  /**
   * @brief Returns the normalized inclusive selected-color range.
   *
   * The range affects rendering only when GetColorPolicy() returns
   * ColorPolicy::SELECTED_IN_FRAME_RANGE.
   *
   * @return The frame range
   */
  SelectableLottieImage::FrameRange GetSelectedColorRange() const;

private:
  struct Impl;
  Dali::UniquePtr<Impl> mImpl;
};

/**
 * @brief A list of selectable Lottie color bindings.
 */
using SelectableLottieColorBindings = Dali::Vector<SelectableLottieColorBinding>;

/** @} */

} // namespace Ui
} // namespace Dali
