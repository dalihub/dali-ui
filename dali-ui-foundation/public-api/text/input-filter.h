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

#include <cstdint>

#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali/public-api/common/dali-string.h>

namespace Dali
{
namespace Ui
{
namespace Text
{

/**
 * @brief Describes input filtering rules for text input.
 *
 * This is a value-type object used to configure which input characters are
 * allowed or denied.
 *
 * The allow pattern defines input that can be inserted.
 * If it is set, input that does not match the allow pattern is rejected.
 *
 * The deny pattern defines input that cannot be inserted.
 * If it is set, input that matches the deny pattern is rejected.
 *
 * If both patterns are set, input must match the allow pattern and must not
 * match the deny pattern. Input is rejected if it does not match the allow
 * pattern, or if it matches the deny pattern.
 *
 * The patterns follow std::regex ECMAScript grammar.
 */
class DALI_UI_API InputFilter
{
public:
  /**
   * @brief The reason why input was rejected by the input filter.
   */
  enum class RejectReason : uint8_t
  {
    /**
     * @brief The input was rejected because it did not match the allow pattern.
     */
    NOT_ALLOWED,

    /**
     * @brief The input was rejected because it matched the deny pattern.
     */
    DENIED
  };

public:
  /**
   * @brief Creates an input filter with default values.
   */
  InputFilter();

  /**
   * @brief Creates an input filter by copying another input filter.
   *
   * @param[in] rhs The input filter to copy.
   */
  InputFilter(const InputFilter& rhs);

  /**
   * @brief Creates an input filter by moving another input filter.
   *
   * @param[in] rhs The input filter to move.
   */
  InputFilter(InputFilter&& rhs) noexcept;

  /**
   * @brief Copies another input filter to this object.
   *
   * @param[in] rhs The input filter to copy.
   */
  InputFilter& operator=(const InputFilter& rhs);

  /**
   * @brief Moves another input filter to this object.
   *
   * @param[in] rhs The input filter to move.
   */
  InputFilter& operator=(InputFilter&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~InputFilter();

public:
  /**
   * @brief Sets the allow pattern.
   *
   * If set, input that does not match this pattern is rejected.
   *
   * @param[in] pattern A regular expression that defines allowed input.
   */
  void SetAllowPattern(const Dali::String& pattern);

  /**
   * @brief Returns the allow pattern.
   *
   * @return The allow pattern.
   */
  const Dali::String& GetAllowPattern() const;

  /**
   * @brief Sets the deny pattern.
   *
   * If set, input that matches this pattern is rejected.
   *
   * @param[in] pattern A regular expression that defines denied input.
   */
  void SetDenyPattern(const Dali::String& pattern);

  /**
   * @brief Returns the deny pattern.
   *
   * @return The deny pattern.
   */
  const Dali::String& GetDenyPattern() const;

private:
  class Impl;
  Impl* mImpl{nullptr};
};

} // namespace Text
} // namespace Ui
} // namespace Dali
