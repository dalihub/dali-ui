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
#include <dali-ui-foundations/public-api/unit.h>

namespace Dali
{

namespace UI
{

// TODO Read values from the device and configuration
static constexpr float SCALING_FACTOR = 4.0f;
static constexpr int DPI = 160.0f;
static constexpr int BASELINE_DPI = 160;
static constexpr float DPI_FACTOR = DPI / BASELINE_DPI;
static constexpr float SCALED_DPI_FACTOR = DPI_FACTOR * SCALING_FACTOR;

float operator "" _spx(unsigned long long int v)
{
    return static_cast<float>(v) * SCALING_FACTOR;
}

float operator "" _spx(long double v)
{
    return static_cast<float>(v) * SCALING_FACTOR;
}

float operator "" _dp(unsigned long long int v)
{
    return static_cast<float>(v) * DPI_FACTOR;
}

float operator "" _dp(long double v)
{
    return static_cast<float>(v) * DPI_FACTOR;
}

float operator "" _sdp(unsigned long long int v)
{
    return static_cast<float>(v) * SCALED_DPI_FACTOR;
}

float operator "" _sdp(long double v)
{
    return static_cast<float>(v) * SCALED_DPI_FACTOR;
}

} // namespace UI

} // namespace Dali
