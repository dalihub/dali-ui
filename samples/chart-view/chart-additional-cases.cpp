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
 */

#include "chart-additional-cases.h"
#include <utility>

namespace ChartStandard
{
void AppendAdditionalCases(std::vector<ReferenceCase>& cases)
{
  ReferenceCase donut;
  donut.id           = "donut-01";
  donut.category     = "Donut";
  donut.title        = "Donut (Device Distribution)";
  donut.kind         = ChartKind::PIE;
  donut.webReference = false;
  donut.values       = {248, 220, 95, 90, 190};
  donut.labels       = {"Apple", "Samsung", "Huawei", "Xiaomi", "Others"};
  donut.pointColors  = {0x387AFFu, 0x12AB88u, 0xFF9A3Du, 0x9B81F6u, 0xA3A9B6u};
  donut.innerRadius  = 0.62f;
  donut.unit         = "M";
  donut.subtitle     = "Device distribution";
  donut.description  = "Example data / million devices";
  donut.maximum      = 300;
  donut.frames       = {{28, 116, 618, 300}};
  donut.expected     = "Five slices; total 843M; select a segment or its legend";
  cases.push_back(std::move(donut));

  ReferenceCase pie;
  pie.id           = "pie-01";
  pie.category     = "Pie";
  pie.title        = "Pie (Browser Distribution)";
  pie.kind         = ChartKind::PIE;
  pie.webReference = false;
  pie.values       = {63, 19, 11, 4, 3};
  pie.labels       = {"Chrome", "Safari", "Edge", "Firefox", "Other"};
  pie.pointColors  = {0x387AFFu, 0x12AB88u, 0xFF9A3Du, 0x9B81F6u, 0xA3A9B6u};
  pie.unit         = "%";
  pie.subtitle     = "Browser distribution";
  pie.description  = "Example data / relative share";
  pie.frames       = {{28, 116, 618, 300}};
  pie.expected     = "Five solid sectors; total 100; legend shows value and share";
  cases.push_back(std::move(pie));

  ReferenceCase gauge;
  gauge.id           = "gauge-01";
  gauge.category     = "Gauge";
  gauge.title        = "Gauge (Activity Goal)";
  gauge.kind         = ChartKind::GAUGE;
  gauge.webReference = false;
  gauge.values       = {72};
  gauge.unit         = "%";
  gauge.subtitle     = "Weekly activity";
  gauge.description  = "Progress toward a 100% goal";
  gauge.interactive  = false;
  gauge.frames       = {{28, 116, 618, 300}};
  gauge.expected     = "270 degree gauge; initial progress 72%; Update and Reset";
  cases.push_back(std::move(gauge));

  ReferenceCase scatter;
  scatter.id               = "scatter-01";
  scatter.category         = "Scatter";
  scatter.title            = "Scatter (Two Groups)";
  scatter.kind             = ChartKind::SCATTER;
  scatter.webReference     = false;
  scatter.values           = {5.3f, 2.8f, 7.1f, 4.6f, 6.9f, 3.2f, 8.4f, 5.7f, 2.3f, 7.8f};
  scatter.xValues          = {1.2f, 2.5f, 3.1f, 4.7f, 2.3f, 5.8f, 1.9f, 3.6f, 4.2f, 6.1f};
  scatter.secondary        = {1.4f, 3.7f, 2.1f, 4.8f, 1.9f, 3.2f, 2.6f, 4.5f, 1.7f, 3.9f};
  scatter.secondaryXValues = {7.1f, 8.4f, 6.3f, 9.2f, 7.8f, 8.9f, 6.6f, 9.5f, 7.4f, 8.1f};
  scatter.maximum          = 10;
  scatter.unit             = "";
  scatter.subtitle         = "Two groups / independent X and Y values";
  scatter.description      = "Circle and diamond markers identify each group";
  scatter.frames           = {{28, 116, 618, 300}};
  scatter.xAxis            = {{{"0", 0}, {"2", 2}, {"4", 4}, {"6", 6}, {"8", 8}, {"10", 10}}, true};
  scatter.yAxis            = scatter.xAxis;
  scatter.expected         = "Two groups of ten XY points; toggle each legend independently";
  cases.push_back(std::move(scatter));
}
} // namespace ChartStandard
