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

namespace Dali
{
namespace Ui
{

/**
 * @brief Represents a time duration for animations.
 *
 * Duration is a lightweight value type that wraps a float (seconds).
 * Use the user-defined literals for convenient construction:
 * @code
 *   Duration d1 = 300_ms;   // 0.3 seconds
 *   Duration d2 = 1.5_s;    // 1.5 seconds
 * @endcode
 */
class Duration
{
public:
  /**
   * @brief Constructs a Duration of zero seconds.
   */
  constexpr Duration()
  : mSeconds(0.0f)
  {
  }

  /**
   * @brief Constructs a Duration from seconds.
   * @param[in] seconds The duration in seconds
   */
  constexpr explicit Duration(float seconds)
  : mSeconds(seconds)
  {
  }

  /**
   * @brief Returns the duration in seconds.
   * @return The duration value in seconds
   */
  constexpr float InSeconds() const
  {
    return mSeconds;
  }

  /**
   * @brief Returns the duration in milliseconds.
   * @return The duration value in milliseconds
   */
  constexpr float InMilliseconds() const
  {
    return mSeconds * 1000.0f;
  }

private:
  float mSeconds;
};

/**
 * @brief User-defined literal for milliseconds (integer).
 * @code
 *   Duration d = 300_ms;  // 0.3 seconds
 * @endcode
 */
constexpr Duration operator""_ms(unsigned long long value)
{
  return Duration(static_cast<float>(value) / 1000.0f);
}

/**
 * @brief User-defined literal for milliseconds (floating-point).
 * @code
 *   Duration d = 300.0_ms;  // 0.3 seconds
 * @endcode
 */
constexpr Duration operator""_ms(long double value)
{
  return Duration(static_cast<float>(value) / 1000.0f);
}

/**
 * @brief User-defined literal for seconds (integer).
 * @code
 *   Duration d = 2_s;  // 2.0 seconds
 * @endcode
 */
constexpr Duration operator""_s(unsigned long long value)
{
  return Duration(static_cast<float>(value));
}

/**
 * @brief User-defined literal for seconds (floating-point).
 * @code
 *   Duration d = 1.5_s;  // 1.5 seconds
 * @endcode
 */
constexpr Duration operator""_s(long double value)
{
  return Duration(static_cast<float>(value));
}

} // namespace Ui
} // namespace Dali
