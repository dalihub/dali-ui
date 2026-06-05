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

// CLASS HEADER
#include <dali-ui-components/integration-api/chart/chart-color-palette.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

const Vector4 ChartColorPalette::sColors[DEFAULT_SIZE] = {
  Vector4(0.259f, 0.522f, 0.957f, 1.0f), // Blue
  Vector4(0.918f, 0.263f, 0.208f, 1.0f), // Red
  Vector4(0.188f, 0.729f, 0.384f, 1.0f), // Green
  Vector4(1.000f, 0.596f, 0.000f, 1.0f), // Orange
  Vector4(0.612f, 0.153f, 0.690f, 1.0f), // Purple
  Vector4(0.000f, 0.737f, 0.831f, 1.0f), // Cyan
  Vector4(0.949f, 0.769f, 0.067f, 1.0f), // Yellow
  Vector4(0.475f, 0.333f, 0.282f, 1.0f), // Brown
};

Vector4 ChartColorPalette::Get(int seriesIndex)
{
  return sColors[seriesIndex % DEFAULT_SIZE];
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
