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
#include <dali/public-api/common/dali-vector.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/text/font-variation/font-variation-axis.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
/**
 * @brief Utility namespace for font variation settings.
 *
 * This namespace provides helper functions to convert between string-based
 * font variation settings and axis-based representations.
 *
 * A font variation settings string consists of one or more pairs of
 * axis tags and numeric values separated by commas.
 *
 * Supported formats include:
 * - wght=700,wdth=90 (recommended)
 * - "wght" 700, "wdth" 90
 * - 'wght' 700, 'wdth' 90
 *
 * In quoted formats, the axis tag must be wrapped with single quotes
 * (U+0027) or double quotes (U+0022).
 *
 * Each axis tag must contain exactly four printable ASCII characters
 * in the range U+0020..U+007E.
 *
 * If duplicate axis tags are specified, the last value is used.
 *
 * Parsing is strict. If the input string does not conform to the expected
 * format, an empty axis list is returned. An empty input string also returns
 * an empty axis list.
 *
 * @note Unsupported axis tags may be ignored depending on the selected font.
 * @note FontVariation::None() is the typed clear value for SetFontVariation().
 *       Passing an empty string to SetFontVariation(const Dali::String&) is
 *       invalid and leaves the current font variation unchanged.
 */
namespace FontVariation
{

/**
 * @brief Returns an empty font variation axis list.
 *
 * This value can be passed to SetFontVariation() to clear font variation settings.
 * It is equivalent to passing an empty axis vector.
 *
 * @return An empty font variation axis list.
 */
DALI_UI_API const Dali::Vector<Axis>& None();

/**
 * @brief Parses a font variation settings string.
 *
 * The input string must follow one of the supported formats described
 * in this namespace documentation.
 *
 * @param[in] settings The font variation settings string.
 * @return A vector of parsed font variation axes. Returns an empty
 *         vector if @p settings is empty or parsing fails.
 */
DALI_UI_API Dali::Vector<Axis> FromString(const Dali::String& settings);

/**
 * @brief Converts font variation axes to string.
 *
 * The returned string uses the canonical format:
 * - wght=700,wdth=90
 *
 * @param[in] axes The font variation axes.
 * @return The font variation settings string. Returns an empty string if
 *         @p axes is empty.
 */
DALI_UI_API Dali::String ToString(const Dali::Vector<Axis>& axes);

} // namespace FontVariation
} // namespace Text
} // namespace Ui
} // namespace Dali
