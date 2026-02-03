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
#include <dali-ui-foundation/public-api/dali-ui-foundation-common.h>

namespace Dali
{

namespace UI
{

/**
 * @brief Converts an integer spx value to a float pixel value.
 * @details The input value is multiplied by a scaling-factor.
 * @param v The value in spx (e.g. 35_spx)
 * @return The calculated physical pixels as a float.
 */
DALI_UI_FOUNDATION_API float operator"" _spx(unsigned long long int v);

/**
 * @brief Converts an floating-point spx value to a float pixel value.
 * @details The input value is multiplied by a scaling-factor.
 * @param v The value in spx (e.g. 35_spx)
 * @return The calculated physical pixels as a float.
 */
DALI_UI_FOUNDATION_API float operator"" _spx(long double v);

/**
 * @brief Converts an integer dp value to a float pixel value.
 * @details The input value is computed with dpi and baseline-dpi.
 * @param v The value in dp (e.g. 35_dp)
 * @return The calculated physical pixels as a float.
 */
DALI_UI_FOUNDATION_API float operator"" _dp(unsigned long long int v);

/**
 * @brief Converts an floating-point dp value to a float pixel value.
 * @details The input value is computed with dpi and baseline-dpi.
 * @param v The value in dp (e.g. 35_dp)
 * @return The calculated physical pixels as a float.
 */
DALI_UI_FOUNDATION_API float operator"" _dp(long double v);

/**
 * @brief Converts an integer sdp value to a float pixel value.
 * @details The input value is computed with dpi, baseline-dpi and scaling-factor.
 * @param v The value in dp (e.g. 35_dp)
 * @return The calculated physical pixels as a float.
 */
DALI_UI_FOUNDATION_API float operator"" _sdp(unsigned long long int v);

/**
 * @brief Converts an floating-point sdp value to a float pixel value.
 * @details The input value is computed with dpi, baseline-dpi and scaling-factor.
 * @param v The value in dp (e.g. 35_dp)
 * @return The calculated physical pixels as a float.
 */
DALI_UI_FOUNDATION_API float operator"" _sdp(long double v);

} // namespace UI

} // namespace Dali
