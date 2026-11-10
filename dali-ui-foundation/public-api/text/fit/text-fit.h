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

namespace DALI_NAMESPACE
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
 * @SINCE_3_0.0
 */
class DALI_UI_API Fit
{
public:
  // Configuration type

  /**
   * @brief The text fit configuration type.
   * @SINCE_3_0.0
   */
  enum class Type : uint8_t
  {
    /**
     * @brief Text fit is disabled.
     * @SINCE_3_0.0
     */
    NONE = 0u,
    /**
     * @brief Text fit uses a font size range.
     * @SINCE_3_0.0
     */
    RANGE = 1u,
    /**
     * @brief Text fit uses a list of font size and line height candidates.
     * @SINCE_3_0.0
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
   * @SINCE_3_0.0
   */
  class DALI_UI_API Range
  {
  public:
    /**
     * @brief Creates a fit range with default values.
     * @SINCE_3_0.0
     */
    Range();

    /**
     * @brief Creates a fit range with given parameters.
     *
     * @SINCE_3_0.0
     * @param[in] minimumFontSize The minimum font size in pixels.
     * @param[in] maximumFontSize The maximum font size in pixels.
     * @param[in] fontSizeStep The font size step in pixels.
     */
    Range(float minimumFontSize, float maximumFontSize, float fontSizeStep);

    /**
     * @brief Creates a fit range by copying another fit range.
     *
     * @SINCE_3_0.0
     * @param[in] rhs The fit range to copy.
     */
    Range(const Range& rhs);

    /**
     * @brief Creates a fit range by moving another fit range.
     *
     * @SINCE_3_0.0
     * @param[in] rhs The fit range to move.
     */
    Range(Range&& rhs) noexcept;

    /**
     * @brief Copies another fit range to this object.
     *
     * @SINCE_3_0.0
     * @param[in] rhs The fit range to copy.
     */
    Range& operator=(const Range& rhs);

    /**
     * @brief Moves another fit range to this object.
     *
     * @SINCE_3_0.0
     * @param[in] rhs The fit range to move.
     */
    Range& operator=(Range&& rhs) noexcept;

    /**
     * @brief Destructor.
     * @SINCE_3_0.0
     */
    ~Range();

  public:
    /**
     * @brief Sets the minimum font size.
     *
     * @SINCE_3_0.0
     * @param[in] size The minimum font size in pixels.
     */
    void SetMinimumFontSize(float size);

    /**
     * @brief Returns the minimum font size.
     *
     * @SINCE_3_0.0
     * @return The minimum font size in pixels.
     */
    float GetMinimumFontSize() const;

    /**
     * @brief Sets the maximum font size.
     *
     * @SINCE_3_0.0
     * @param[in] size The maximum font size in pixels.
     */
    void SetMaximumFontSize(float size);

    /**
     * @brief Returns the maximum font size.
     *
     * @SINCE_3_0.0
     * @return The maximum font size in pixels.
     */
    float GetMaximumFontSize() const;

    /**
     * @brief Sets the font size step.
     *
     * The text fit algorithm decreases the font size by this step when
     * searching for the largest fitting size.
     *
     * @SINCE_3_0.0
     * @param[in] step The font size step in pixels.
     */
    void SetFontSizeStep(float step);

    /**
     * @brief Returns the font size step.
     *
     * @SINCE_3_0.0
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
   * @SINCE_3_0.0
   */
  class DALI_UI_API Candidate
  {
  public:
    /**
     * @brief Creates a fit candidate with default values.
     * @SINCE_3_0.0
     */
    Candidate();

    /**
     * @brief Creates a fit candidate with given parameters.
     *
     * @SINCE_3_0.0
     * @param[in] fontSize The font size in pixels.
     * @param[in] lineHeight The line height in pixels.
     */
    Candidate(float fontSize, float lineHeight);

    /**
     * @brief Creates a fit candidate by copying another fit candidate.
     *
     * @SINCE_3_0.0
     * @param[in] rhs The fit candidate to copy.
     */
    Candidate(const Candidate& rhs);

    /**
     * @brief Creates a fit candidate by moving another fit candidate.
     *
     * @SINCE_3_0.0
     * @param[in] rhs The fit candidate to move.
     */
    Candidate(Candidate&& rhs) noexcept;

    /**
     * @brief Copies another fit candidate to this object.
     *
     * @SINCE_3_0.0
     * @param[in] rhs The fit candidate to copy.
     */
    Candidate& operator=(const Candidate& rhs);

    /**
     * @brief Moves another fit candidate to this object.
     *
     * @SINCE_3_0.0
     * @param[in] rhs The fit candidate to move.
     */
    Candidate& operator=(Candidate&& rhs) noexcept;

    /**
     * @brief Destructor.
     * @SINCE_3_0.0
     */
    ~Candidate();

  public:
    /**
     * @brief Sets the font size.
     *
     * @SINCE_3_0.0
     * @param[in] size The font size in pixels.
     */
    void SetFontSize(float size);

    /**
     * @brief Returns the font size.
     *
     * @SINCE_3_0.0
     * @return The font size in pixels.
     */
    float GetFontSize() const;

    /**
     * @brief Sets the line height.
     *
     * @SINCE_3_0.0
     * @param[in] height The line height in pixels.
     */
    void SetLineHeight(float height);

    /**
     * @brief Returns the line height.
     *
     * @SINCE_3_0.0
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
   * @SINCE_3_0.0
   */
  Fit();

  /**
   * @brief Creates a fit configuration by copying another fit configuration.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The fit configuration to copy.
   */
  Fit(const Fit& rhs);

  /**
   * @brief Creates a fit configuration by moving another fit configuration.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The fit configuration to move.
   */
  Fit(Fit&& rhs) noexcept;

  /**
   * @brief Copies another fit configuration to this object.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The fit configuration to copy.
   */
  Fit& operator=(const Fit& rhs);

  /**
   * @brief Moves another fit configuration to this object.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The fit configuration to move.
   */
  Fit& operator=(Fit&& rhs) noexcept;

  /**
   * @brief Destructor.
   * @SINCE_3_0.0
   */
  ~Fit();

  // Factory functions

  /**
   * @brief Returns a fit configuration representing disabled text fit.
   *
   * This value can be passed to SetTextFit() to clear text fit.
   *
   * @SINCE_3_0.0
   * @return A shared immutable none fit configuration.
   */
  static const Fit& None();

  /**
   * @brief Creates a range-based text fit configuration.
   *
   * @SINCE_3_0.0
   * @param[in] range The range-based text fit configuration.
   * @return A range-based text fit configuration.
   */
  static Fit FromRange(const Range& range);

  /**
   * @brief Creates a candidate-based text fit configuration.
   *
   * If @p candidates is empty, this returns Text::Fit::None().
   *
   * @SINCE_3_0.0
   * @param[in] candidates The candidate-based text fit configuration.
   * @return A candidate-based text fit configuration, or None() if @p candidates is empty.
   */
  static Fit FromCandidates(const Dali::Vector<Candidate>& candidates);

public:
  // Accessors

  /**
   * @brief Returns the text fit configuration type.
   *
   * @SINCE_3_0.0
   * @return The text fit configuration type.
   */
  Type GetType() const;

  /**
   * @brief Returns the range-based text fit configuration.
   *
   * This function must only be called when GetType() returns Type::RANGE.
   *
   * @SINCE_3_0.0
   * @return The range-based text fit configuration.
   */
  const Range& GetRange() const;

  /**
   * @brief Returns the candidate-based text fit configuration.
   *
   * This function must only be called when GetType() returns Type::CANDIDATES.
   *
   * @SINCE_3_0.0
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
} //namespace DALI_NAMESPACE
