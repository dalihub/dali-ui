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

namespace UI
{

/**
 * @brief Converts an integer spx value to a float pixel value.
 * @details The input value is multiplied by a scaling-factor configured via UIConfig.
 *
 * @pre UIConfigManager::Init() must have been called before using this operator.
 *      An assertion failure is triggered if UIConfig has not been initialized.
 * @warning Do NOT use this operator to initialize static or global variables.
 *          Static initialization order is undefined across translation units,
 *          so UIConfigManager::Init() may not have been called yet at that point.
 *          Use lazy initialization or compute values at first access instead.
 *
 * @param v The value in spx (e.g. 35_spx)
 * @return The calculated physical pixels as a float.
 */
DALI_UI_API float operator"" _spx(unsigned long long int v);

/**
 * @brief Converts a floating-point spx value to a float pixel value.
 * @details The input value is multiplied by a scaling-factor configured via UIConfig.
 *
 * @pre UIConfigManager::Init() must have been called before using this operator.
 *      An assertion failure is triggered if UIConfig has not been initialized.
 * @warning Do NOT use this operator to initialize static or global variables.
 *          Static initialization order is undefined across translation units,
 *          so UIConfigManager::Init() may not have been called yet at that point.
 *          Use lazy initialization or compute values at first access instead.
 *
 * @param v The value in spx (e.g. 35.0_spx)
 * @return The calculated physical pixels as a float.
 */
DALI_UI_API float operator"" _spx(long double v);

/**
 * @brief Converts an integer dp value to a float pixel value.
 * @details The input value is computed with dpi and baseline-dpi configured via UIConfig.
 *
 * @pre UIConfigManager::Init() must have been called before using this operator.
 *      An assertion failure is triggered if UIConfig has not been initialized.
 * @warning Do NOT use this operator to initialize static or global variables.
 *          Static initialization order is undefined across translation units,
 *          so UIConfigManager::Init() may not have been called yet at that point.
 *          Use lazy initialization or compute values at first access instead.
 *
 * @param v The value in dp (e.g. 35_dp)
 * @return The calculated physical pixels as a float.
 */
DALI_UI_API float operator"" _dp(unsigned long long int v);

/**
 * @brief Converts a floating-point dp value to a float pixel value.
 * @details The input value is computed with dpi and baseline-dpi configured via UIConfig.
 *
 * @pre UIConfigManager::Init() must have been called before using this operator.
 *      An assertion failure is triggered if UIConfig has not been initialized.
 * @warning Do NOT use this operator to initialize static or global variables.
 *          Static initialization order is undefined across translation units,
 *          so UIConfigManager::Init() may not have been called yet at that point.
 *          Use lazy initialization or compute values at first access instead.
 *
 * @param v The value in dp (e.g. 35.0_dp)
 * @return The calculated physical pixels as a float.
 */
DALI_UI_API float operator"" _dp(long double v);

/**
 * @brief Converts an integer sdp value to a float pixel value.
 * @details The input value is computed with dpi, baseline-dpi and scaling-factor
 *          configured via UIConfig.
 *
 * @pre UIConfigManager::Init() must have been called before using this operator.
 *      An assertion failure is triggered if UIConfig has not been initialized.
 * @warning Do NOT use this operator to initialize static or global variables.
 *          Static initialization order is undefined across translation units,
 *          so UIConfigManager::Init() may not have been called yet at that point.
 *          Use lazy initialization or compute values at first access instead.
 *
 * @param v The value in sdp (e.g. 35_sdp)
 * @return The calculated physical pixels as a float.
 */
DALI_UI_API float operator"" _sdp(unsigned long long int v);

/**
 * @brief Converts a floating-point sdp value to a float pixel value.
 * @details The input value is computed with dpi, baseline-dpi and scaling-factor
 *          configured via UIConfig.
 *
 * @pre UIConfigManager::Init() must have been called before using this operator.
 *      An assertion failure is triggered if UIConfig has not been initialized.
 * @warning Do NOT use this operator to initialize static or global variables.
 *          Static initialization order is undefined across translation units,
 *          so UIConfigManager::Init() may not have been called yet at that point.
 *          Use lazy initialization or compute values at first access instead.
 *
 * @param v The value in sdp (e.g. 35.0_sdp)
 * @return The calculated physical pixels as a float.
 */
DALI_UI_API float operator"" _sdp(long double v);

} // namespace UI

} // namespace Dali
