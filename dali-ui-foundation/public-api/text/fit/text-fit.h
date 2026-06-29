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
#include <dali/public-api/common/dali-vector.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{
namespace Text
{

/**
 * @brief Describes the complete text fit configuration.
 *
 * Text fit can be disabled, range-based, or candidate-based. This value
 * represents exactly one of those states and is returned by Label::GetTextFit().
 */
class DALI_UI_API Fit
{
public:
  // Configuration type

  /**
   * @brief The text fit configuration type.
   */
  enum class Type : uint8_t
  {
    /**
     * @brief Text fit is disabled.
     */
    NONE = 0u,
    /**
     * @brief Text fit uses a font size range.
     */
    RANGE = 1u,
    /**
     * @brief Text fit uses a list of font size and line height candidates.
     */
    CANDIDATES = 2u
  };

  // Range-based configuration

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
   * This object is used to configure range-based text fit behavior.
   */
  class DALI_UI_API Range
  {
  public:
    /**
     * @brief Creates a fit range with default values.
     */
    Range();

    /**
     * @brief Creates a fit range with given parameters.
     *
     * @param[in] minimumFontSize The minimum font size in pixels.
     * @param[in] maximumFontSize The maximum font size in pixels.
     * @param[in] fontSizeStep The font size step in pixels.
     */
    Range(float minimumFontSize, float maximumFontSize, float fontSizeStep);

    /**
     * @brief Creates a fit range by copying another fit range.
     *
     * @param[in] rhs The fit range to copy.
     */
    Range(const Range& rhs);

    /**
     * @brief Creates a fit range by moving another fit range.
     *
     * @param[in] rhs The fit range to move.
     */
    Range(Range&& rhs) noexcept;

    /**
     * @brief Copies another fit range to this object.
     *
     * @param[in] rhs The fit range to copy.
     */
    Range& operator=(const Range& rhs);

    /**
     * @brief Moves another fit range to this object.
     *
     * @param[in] rhs The fit range to move.
     */
    Range& operator=(Range&& rhs) noexcept;

    /**
     * @brief Destructor.
     */
    ~Range();

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

  // Candidate-based configuration

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
  class DALI_UI_API Candidate
  {
  public:
    /**
     * @brief Creates a fit candidate with default values.
     */
    Candidate();

    /**
     * @brief Creates a fit candidate with given parameters.
     *
     * @param[in] fontSize The font size in pixels.
     * @param[in] lineHeight The line height in pixels.
     */
    Candidate(float fontSize, float lineHeight);

    /**
     * @brief Creates a fit candidate by copying another fit candidate.
     *
     * @param[in] rhs The fit candidate to copy.
     */
    Candidate(const Candidate& rhs);

    /**
     * @brief Creates a fit candidate by moving another fit candidate.
     *
     * @param[in] rhs The fit candidate to move.
     */
    Candidate(Candidate&& rhs) noexcept;

    /**
     * @brief Copies another fit candidate to this object.
     *
     * @param[in] rhs The fit candidate to copy.
     */
    Candidate& operator=(const Candidate& rhs);

    /**
     * @brief Moves another fit candidate to this object.
     *
     * @param[in] rhs The fit candidate to move.
     */
    Candidate& operator=(Candidate&& rhs) noexcept;

    /**
     * @brief Destructor.
     */
    ~Candidate();

  public:
    /**
     * @brief Sets the font size.
     *
     * @param[in] size The font size in pixels.
     */
    void SetFontSize(float size);

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
     */
    void SetLineHeight(float height);

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

public:
  // Lifecycle

  /**
   * @brief Creates a fit configuration representing disabled text fit.
   */
  Fit();

  /**
   * @brief Creates a fit configuration by copying another fit configuration.
   *
   * @param[in] rhs The fit configuration to copy.
   */
  Fit(const Fit& rhs);

  /**
   * @brief Creates a fit configuration by moving another fit configuration.
   *
   * @param[in] rhs The fit configuration to move.
   */
  Fit(Fit&& rhs) noexcept;

  /**
   * @brief Copies another fit configuration to this object.
   *
   * @param[in] rhs The fit configuration to copy.
   */
  Fit& operator=(const Fit& rhs);

  /**
   * @brief Moves another fit configuration to this object.
   *
   * @param[in] rhs The fit configuration to move.
   */
  Fit& operator=(Fit&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~Fit();

  // Factory functions

  /**
   * @brief Returns a fit configuration representing disabled text fit.
   *
   * This value can be passed to SetTextFit() to clear text fit.
   *
   * @return A shared immutable none fit configuration.
   */
  static const Fit& None();

  /**
   * @brief Creates a range-based text fit configuration.
   *
   * @param[in] range The range-based text fit configuration.
   * @return A range-based text fit configuration.
   */
  static Fit FromRange(const Range& range);

  /**
   * @brief Creates a candidate-based text fit configuration.
   *
   * If @p candidates is empty, this returns Text::Fit::None().
   *
   * @param[in] candidates The candidate-based text fit configuration.
   * @return A candidate-based text fit configuration, or None() if @p candidates is empty.
   */
  static Fit FromCandidates(const Dali::Vector<Candidate>& candidates);

public:
  // Accessors

  /**
   * @brief Returns the text fit configuration type.
   *
   * @return The text fit configuration type.
   */
  Type GetType() const;

  /**
   * @brief Returns the range-based text fit configuration.
   *
   * This function must only be called when GetType() returns Type::RANGE.
   *
   * @return The range-based text fit configuration.
   */
  const Range& GetRange() const;

  /**
   * @brief Returns the candidate-based text fit configuration.
   *
   * This function must only be called when GetType() returns Type::CANDIDATES.
   *
   * @return The candidate-based text fit configuration.
   */
  const Dali::Vector<Candidate>& GetCandidates() const;

private:
  explicit Fit(const Range& range);
  explicit Fit(const Dali::Vector<Candidate>& candidates);

private:
  class Impl;
  Impl* mImpl{nullptr};
};

} // namespace Text
} // namespace Ui
} // namespace Dali
