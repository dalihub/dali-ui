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
 * @brief Describes a text fit candidate for candidate-based text fit.
 *
 * Each fit candidate defines a font size and line height configuration
 * that can be selected during text fit.
 *
 * Text fit selects the largest candidate that fits into the available
 * layout space.
 *
 * The line height is specified as an absolute value.
 *
 * This object is used to configure candidate-based text fit behavior.
 */
class DALI_UI_API FitCandidate
{
public:
  /**
   * @brief Creates a fit candidate with default values.
   */
  FitCandidate();

  /**
   * @brief Creates a fit candidate with given parameters.
   *
   * @param[in] fontSize The font size in pixels.
   * @param[in] lineHeight The line height in pixels.
   */
  FitCandidate(float fontSize, float lineHeight);

  /**
   * @brief Creates a fit candidate by copying another fit candidate.
   *
   * @param[in] rhs The fit candidate to copy.
   */
  FitCandidate(const FitCandidate& rhs);

  /**
   * @brief Creates a fit candidate by moving another fit candidate.
   *
   * @param[in] rhs The fit candidate to move.
   */
  FitCandidate(FitCandidate&& rhs) noexcept;

  /**
   * @brief Copies another fit candidate to this object.
   *
   * @param[in] rhs The fit candidate to copy.
   * @return A reference to this fit candidate.
   */
  FitCandidate& operator=(const FitCandidate& rhs);

  /**
   * @brief Moves another fit candidate to this object.
   *
   * @param[in] rhs The fit candidate to move.
   * @return A reference to this fit candidate.
   */
  FitCandidate& operator=(FitCandidate&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~FitCandidate();

public:
  /**
   * @brief Sets the font size.
   *
   * @param[in] size The font size in pixels.
   * @return A reference to this fit candidate.
   */
  FitCandidate& SetFontSize(float size);

  /**
   * @brief Returns the font size.
   *
   * @return The font size in pixels.
   */
  float GetFontSize() const;

  /**
   * @brief Sets the line height.
   *
   * @param[in] height The line height in pixels.
   * @return A reference to this fit candidate.
   */
  FitCandidate& SetLineHeight(float height);

  /**
   * @brief Returns the line height.
   *
   * @return The line height in pixels.
   */
  float GetLineHeight() const;

private:
  class Impl;
  Impl* mImpl{nullptr};
};

} // namespace Text
} // namespace Ui
} // namespace Dali
