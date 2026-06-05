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
#include <dali-ui-components/integration-api/chart/pie-series-impl.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

PieSeries::PieSeries() = default;

void PieSeries::AddSlice(const Dali::String& label, float value, const Vector4& color)
{
  if(value <= 0.0f) return;
  mSlices.push_back({label, value, color});
  EmitDataChangedSignal();
}

void PieSeries::ClearSlices()
{
  mSlices.clear();
  EmitDataChangedSignal();
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
