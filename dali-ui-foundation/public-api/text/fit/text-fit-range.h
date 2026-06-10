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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
/**
 * @brief Describes a font size range for range-based text fit.
 *
 * Text fit selects the largest font size within the configured range
 * that fits into the available layout space.
 *
 * Text fit searches within the configured range using the specified
 * font size step to find the largest fitting size.
 *
 * The line height is not specified by this object. It follows the
 * current text style configuration, including the configured line
 * height mode and value.
 *
 * This object is used to configure text fit behavior.
 */

class DALI_UI_API FitRange
{
public:
  /**
   * @brief Creates a fit range with default values.
   */
  FitRange();

  /**
   * @brief Creates a fit range with given parameters.
   *
   * @param[in] minimumFontSize The minimum font size in pixels.
   * @param[in] maximumFontSize The maximum font size in pixels.
   * @param[in] fontSizeStep The font size step in pixels.
   */
  FitRange(float minimumFontSize, float maximumFontSize, float fontSizeStep);

  /**
   * @brief Creates a fit range by copying another fit range.
   *
   * @param[in] rhs The fit range to copy.
   */
  FitRange(const FitRange& rhs);

  /**
   * @brief Creates a fit range by moving another fit range.
   *
   * @param[in] rhs The fit range to move.
   */
  FitRange(FitRange&& rhs) noexcept;

  /**
   * @brief Copies another fit range to this object.
   *
   * @param[in] rhs The fit range to copy.
   */
  FitRange& operator=(const FitRange& rhs);

  /**
   * @brief Moves another fit range to this object.
   *
   * @param[in] rhs The fit range to move.
   */
  FitRange& operator=(FitRange&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~FitRange();

public:
  /**
   * @brief Sets the minimum font size.
   *
   * @param[in] size The minimum font size in pixels.
   */
  void SetMinimumFontSize(float size);

  /**
   * @brief Returns the minimum font size.
   *
   * @return The minimum font size in pixels.
   */
  float GetMinimumFontSize() const;

  /**
   * @brief Sets the maximum font size.
   *
   * @param[in] size The maximum font size in pixels.
   */
  void SetMaximumFontSize(float size);

  /**
   * @brief Returns the maximum font size.
   *
   * @return The maximum font size in pixels.
   */
  float GetMaximumFontSize() const;

  /**
   * @brief Sets the font size step.
   *
   * The text fit algorithm decreases the font size by this step when
   * searching for the largest fitting size.
   *
   * @param[in] step The font size step in pixels.
   */
  void SetFontSizeStep(float step);

  /**
   * @brief Returns the font size step.
   *
   * @return The font size step in pixels.
   */
  float GetFontSizeStep() const;

private:
  class Impl;
  Impl* mImpl{nullptr};
};

} // namespace Text
} // namespace Ui
} // namespace Dali
