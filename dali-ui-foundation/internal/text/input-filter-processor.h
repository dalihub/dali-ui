#ifndef DALI_INPUT_FILTER_PROCESSOR_H
#define DALI_INPUT_FILTER_PROCESSOR_H

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
#include <string>

namespace Dali
{
namespace Ui
{
namespace Text
{

/**
 * @brief Processes input text filtering rules.
 *
 * This class stores allow and deny regular expression patterns and applies
 * them to input text.
 */
class InputFilterProcessor
{
public:
  /**
   * @brief Constructor.
   */
  InputFilterProcessor();

  /**
   * @brief Sets the allow pattern.
   *
   * If set, input that does not match this pattern is not allowed.
   *
   * @param[in] pattern A regular expression that defines allowed input.
   */
  void SetAllowPattern(const std::string& pattern);

  /**
   * @brief Returns the allow pattern.
   *
   * @return The allow pattern.
   */
  const std::string& GetAllowPattern() const;

  /**
   * @brief Sets the deny pattern.
   *
   * If set, input that matches this pattern is denied.
   *
   * @param[in] pattern A regular expression that defines denied input.
   */
  void SetDenyPattern(const std::string& pattern);

  /**
   * @brief Returns the deny pattern.
   *
   * @return The deny pattern.
   */
  const std::string& GetDenyPattern() const;

  /**
   * @brief Checks whether the source is allowed by the allow pattern.
   *
   * If the allow pattern is empty, this returns true.
   *
   * @param[in] source The input text to check.
   * @return true if the source is allowed, otherwise false.
   */
  bool IsAllowed(const std::string& source) const;

  /**
   * @brief Checks whether the source is denied by the deny pattern.
   *
   * If the deny pattern is empty, this returns false.
   *
   * @param[in] source The input text to check.
   * @return true if the source is denied, otherwise false.
   */
  bool IsDenied(const std::string& source) const;

  /**
   * @brief Applies the allow pattern to the text.
   *
   * Only text matching the allow pattern is kept.
   *
   * @param[in,out] text The text to filter and write back.
   * @return true if the text was changed, otherwise false.
   */
  bool ApplyAllowPattern(std::string& text) const;

  /**
   * @brief Applies the deny pattern to the text.
   *
   * Text matching the deny pattern is removed.
   *
   * @param[in,out] text The text to filter and write back.
   * @return true if the text was changed, otherwise false.
   */
  bool ApplyDenyPattern(std::string& text) const;

private:
  std::string mAllowPattern;
  std::string mDenyPattern;
};

} // namespace Text
} // namespace Ui
} // namespace Dali

#endif // DALI_INPUT_FILTER_PROCESSOR_H
