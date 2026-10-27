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

#include "chart-reference-cases.h"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <locale>
#include <sstream>
#include <utility>
#include "chart-additional-cases.h"

namespace ChartStandard
{
namespace
{
void AppendReferenceCases0To49(std::vector<ReferenceCase>& cases)
{
  {
    ReferenceCase spec;
    spec.id             = "vertical-bar-01";
    spec.category       = "Vertical Bar";
    spec.title          = "Preset_VBar_Basic";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 40.0f;
    spec.values         = {16.153807f, 29.720422f, 13.256173f, 8.2425411f, 15.049897f, 16.270833f, 20.053498f, 3.434928f, 25.674383f, 1.5866769f, 4.8755144f, 21.466821f, 22.842078f, 29.371656f, 13.648148f, 3.8197016f, 26.923354f, 12.218364f, 14.519547f, 7.530607f, 27.177469f, 4.9416152f, 11.193416f, 8.525463f};
    spec.secondary      = {14.13f, 13.71f, 17.13f, 16.62f, 4.81f, 14.96f, 2.11f, 18.19f, 8.83f, 13.18f, 15.24f, 14.27f, 6.67f, 3.81f, 3.42f, 16.34f, 4.85f, 2.53f, 18.34f, 10.88f, 19.48f, 11.0f, 3.21f, 13.72f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "vertical-bar-01: Explicit BAR / CHART; 24 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 304.0f;
    spec.chartHeight    = 219.0f;
    spec.interactive    = true;
    spec.frames         = {{190.5f, 128.5f, 304.0f, 219.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"0", 0.0f}, {"3", 3.0f}, {"6", 6.0f}, {"9", 9.0f}, {"12", 12.0f}, {"15", 15.0f}, {"18", 18.0f}, {"21", 21.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"10", 10.0f}, {"20", 20.0f}, {"30", 30.0f}, {"40", 40.0f}}, true};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "vertical-bar-02";
    spec.category       = "Vertical Bar";
    spec.title          = "Preset_VBar_Basic/Status";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {50.286351f, 59.705933f, 87.541152f, 83.174726f, 25.618999f, 1.2937243f, 48.470508f, 68.383916f, 3.0092593f, 79.50703f};
    spec.secondary      = {47.11f, 51.6f, 36.62f, 57.8f, 59.37f, 33.48f, 35.04f, 55.18f, 62.49f, 47.65f};
    spec.statusLevels   = 5;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "vertical-bar-02: Explicit BAR / CHART; 10 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 304.0f;
    spec.chartHeight    = 218.0f;
    spec.interactive    = true;
    spec.frames         = {{190.5f, 129.0f, 304.0f, 218.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.pointColors    = {16233728u, 16233728u, 15084845u, 15084845u, 1223424u, 2932476u, 16233728u, 16409600u, 2932476u, 16409600u};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}, {"9", 9.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"Good", 20.0f}, {"Moderate", 40.0f}, {"Slightly\nunhealthy", 60.0f}, {"Unhealthy", 80.0f}, {"Very\nunhealthy", 100.0f}}, true};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "vertical-bar-03";
    spec.category       = "Vertical Bar";
    spec.title          = "Preset_VBar_Basic/Status (3 Status)";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {21.334019f, 26.021091f, 9.6176269f, 17.802641f, 3.6625514f, 72.135631f, 91.123114f, 69.266975f, 0.39437586f, 74.628772f};
    spec.secondary      = {47.14f, 57.5f, 16.13f, 60.2f, 37.69f, 17.08f, 63.06f, 49.73f, 30.56f, 51.39f};
    spec.statusLevels   = 3;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "vertical-bar-03: Explicit BAR / CHART; 10 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 304.0f;
    spec.chartHeight    = 218.0f;
    spec.interactive    = true;
    spec.frames         = {{190.5f, 129.0f, 304.0f, 218.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.pointColors    = {2932476u, 2932476u, 2932476u, 2932476u, 2932476u, 16233728u, 16233728u, 16233728u, 2932476u, 16233728u};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}, {"9", 9.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"Low", 33.0f}, {"Moderate", 66.0f}, {"High", 100.0f}}, true};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "vertical-bar-04";
    spec.category       = "Vertical Bar";
    spec.title          = "Preset_VBar_Basic/Status (StatusBand)";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {56.167695f, 27.727195f, 1.6529493f, 97.698045f, 87.467421f, 5.5289781f, 27.191358f, 12.948388f, 19.590192f, 77.980967f};
    spec.secondary      = {47.16f, 63.39f, 60.63f, 62.61f, 16.01f, 0.68f, 26.08f, 44.28f, 63.62f, 55.12f};
    spec.statusLevels   = 5;
    spec.statusBand     = true;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "vertical-bar-04: Explicit BAR / CHART; 10 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 304.0f;
    spec.chartHeight    = 218.0f;
    spec.interactive    = true;
    spec.frames         = {{190.5f, 129.0f, 304.0f, 218.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.pointColors    = {16233728u, 1223424u, 2932476u, 16711680u, 16711680u, 2932476u, 1223424u, 2932476u, 2932476u, 16409600u};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}, {"9", 9.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"20", 20.0f}, {"40", 40.0f}, {"60", 60.0f}, {"80", 80.0f}, {"100", 100.0f}}, true};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "vertical-bar-05";
    spec.category       = "Vertical Bar";
    spec.title          = "Preset_VBar_Basic/Divergent";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = -40.0f;
    spec.maximum        = 40.0f;
    spec.values         = {-7.4979424f, 16.301955f, 20.040123f, 26.679527f, -23.039095f, 16.069444f, -19.69856f, 3.7309671f, 24.876543f, -6.6322016f, -3.3878601f, 6.4614198f, 25.878601f, -24.395576f, -19.175926f, -22.166152f, 20.707819f, 27.964506f, -10.766461f, -28.077675f, -12.117284f, 20.077675f, 29.247942f, 0.5787037f};
    spec.secondary      = {14.16f, 1.29f, 12.04f, 0.0f, 17.8f, 14.78f, 16.23f, 11.65f, 9.51f, 17.66f, 11.63f, 9.19f, 4.86f, 0.05f, 1.73f, 13.69f, 16.8f, 3.71f, 2.63f, 2.05f, 14.88f, 15.9f, 16.49f, 19.31f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "vertical-bar-05: Explicit BAR / CHART; 24 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 304.0f;
    spec.chartHeight    = 219.0f;
    spec.interactive    = true;
    spec.frames         = {{190.5f, 128.5f, 304.0f, 219.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"0", 0.0f}, {"3", 3.0f}, {"6", 6.0f}, {"9", 9.0f}, {"12", 12.0f}, {"15", 15.0f}, {"18", 18.0f}, {"21", 21.0f}}, true};
    spec.yAxis          = {{{"-40", -40.0f}, {"-20", -20.0f}, {"0", 0.0f}, {"20", 20.0f}, {"40", 40.0f}}, true};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "vertical-bar-06";
    spec.category       = "Vertical Bar";
    spec.title          = "Preset_VBar_Simple (grid)";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {6.0f, 66.0f};
    spec.secondary      = {};
    spec.simple         = false;
    spec.chartWidth     = 123.0f;
    spec.chartHeight    = 126.0f;
    spec.grid           = true;
    spec.pointThemes    = true;
    spec.interactive    = false;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "vertical-bar-06: Explicit BAR / CHART; 2 points";
    spec.secondaryChart = false;
    spec.frames         = {{281.0f, 175.0f, 123.0f, 126.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.833f;
    spec.cornerRadius   = 6.0f;
    spec.pointColors    = {12040123u, 3701503u};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"", 25.0f}, {"", 50.0f}, {"", 75.0f}, {"", 100.0f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "vertical-bar-07";
    spec.category       = "Vertical Bar";
    spec.title          = "Vertical Bar (scale x change animation)";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {0.61042524f, 4.4744513f, 93.420782f, 16.832133f, 53.72085f, 90.506687f, 65.461248f, 79.819102f, 75.555556f, 79.831104f, 16.102538f, 81.90072f};
    spec.secondary      = {47.24f, 16.08f, 64.15f, 4.83f, 15.97f, 16.48f, 45.13f, 27.93f, 32.82f, 1.32f, 32.74f, 54.65f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::X_RANGE;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "vertical-bar-07: Explicit BAR / CHART; 12 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"1", 1.0f}, {"3", 3.0f}, {"5", 5.0f}, {"7", 7.0f}, {"9", 9.0f}, {"11", 11.0f}}, true};
    spec.yAxis          = {{{"0", 0.0f}, {"20", 20.0f}, {"40", 40.0f}, {"60", 60.0f}, {"80", 80.0f}, {"100", 100.0f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "vertical-bar-08";
    spec.category       = "Vertical Bar";
    spec.title          = "Vertical Bar (scale y change animation)";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 80.0f;
    spec.values         = {74.620027f, 76.723594f, 34.300412f, 31.301097f, 42.046639f, 73.450617f, 40.57476f, 15.517833f, 49.63786f, 49.107682f, 23.803841f, 69.528807f};
    spec.secondary      = {37.81f, 17.58f, 34.93f, 5.79f, 47.43f, 0.07f, 6.52f, 17.99f, 0.71f, 4.05f, 49.78f, 27.33f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::Y_RANGE;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "vertical-bar-08: Y range 80/100; X unchanged";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"0", 0.0f}, {"2", 2.0f}, {"4", 4.0f}, {"6", 6.0f}, {"8", 8.0f}, {"10", 10.0f}}, false};
    spec.yAxis          = {{{"", 0.0f}, {"20", 20.0f}, {"40", 40.0f}, {"60", 60.0f}, {"80", 80.0f}}, true};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "vertical-bar-09";
    spec.category       = "Vertical Bar";
    spec.title          = "Vertical Bar (data update animation)";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {3.5665295f, 13.571331f, 79.790123f, 10.61797f, 11.486968f, 27.088477f, 16.928669f, 44.217421f, 18.090535f, 49.165295f, 58.429355f, 79.462963f};
    spec.secondary      = {37.83f, 22.3f, 18.53f, 7.71f, 30.09f, 38.95f, 28.93f, 13.63f, 27.17f, 7.03f, 21.38f, 10.94f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "vertical-bar-09: Explicit BAR / CHART; 12 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"0", 0.0f}, {"2", 2.0f}, {"4", 4.0f}, {"6", 6.0f}, {"8", 8.0f}, {"10", 10.0f}}, false};
    spec.yAxis          = {{{"", 0.0f}, {"25", 25.0f}, {"50", 50.0f}, {"75", 75.0f}, {"100", 100.0f}}, true};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "vertical-bar-10";
    spec.category       = "Vertical Bar";
    spec.title          = "Vertical Bar (data + scaleX update animation)";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {53.994513f, 0.78943759f, 77.872428f, 58.08299f, 57.964335f, 39.985597f, 8.0973937f, 16.620713f, 72.469136f, 10.70439f, 63.42524f, 41.989712f, 12.570645f, 65.044582f, 55.213992f, 40.362826f, 48.145405f, 58.808642f, 40.747599f, 39.394376f, 79.440329f, 40.391632f, 65.458162f, 63.77572f, 65.961591f, 73.003429f, 38.481481f, 64.124143f, 8.696845f, 30.22428f, 61.545953f, 59.205075f};
    spec.secondary      = {37.85f, 27.01f, 2.14f, 9.64f, 12.74f, 25.83f, 51.34f, 9.27f, 1.62f, 10.02f, 44.97f, 46.55f, 6.95f, 39.62f, 11.97f, 1.7f, 32.61f, 13.86f, 32.64f, 28.03f, 11.37f, 32.74f, 10.24f, 18.15f, 39.83f, 51.13f, 47.83f, 49.9f, 11.9f, 6.06f, 33.96f, 35.06f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA_X;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "vertical-bar-10: Explicit BAR / CHART; 32 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"1", 1.0f}, {"4", 4.0f}, {"7", 7.0f}, {"10", 10.0f}, {"13", 13.0f}, {"16", 16.0f}, {"19", 19.0f}, {"22", 22.0f}, {"25", 25.0f}, {"28", 28.0f}}, true};
    spec.yAxis          = {{{"0", 0.0f}, {"20", 20.0f}, {"40", 40.0f}, {"60", 60.0f}, {"80", 80.0f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "vertical-bar-11";
    spec.category       = "Vertical Bar";
    spec.title          = "Vertical Bar (forecast)";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {82.088477f, 6.1171125f};
    spec.secondary      = {47.34f, 39.66f};
    spec.forecast       = true;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "vertical-bar-11: Explicit BAR / CHART; 2 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.cornerRadius   = 8.0f;
    spec.groupWidth     = 0.5f;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}}, false};
    spec.yAxis          = {{{"0", 0.0f}, {"10", 10.0f}, {"20", 20.0f}, {"30", 30.0f}, {"40", 40.0f}, {"50", 50.0f}, {"60", 60.0f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "vertical-bar-12";
    spec.category       = "Vertical Bar";
    spec.title          = "Vertical Bar (solid border)";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 16.0f;
    spec.values         = {13.993484f, 14.359568f};
    spec.secondary      = {47.36f, 45.56f};
    spec.border         = true;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "vertical-bar-12: Explicit BAR / CHART; 2 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.cornerRadius   = 8.0f;
    spec.groupWidth     = 0.5f;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}}, false};
    spec.yAxis          = {{{"0", 0.0f}, {"10", 10.0f}, {"20", 20.0f}, {"30", 30.0f}, {"40", 40.0f}, {"50", 50.0f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "vertical-bar-13";
    spec.category       = "Vertical Bar";
    spec.title          = "Vertical Bar (RGBA color)";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 70.0f;
    spec.values         = {1.042524f, 64.165809f};
    spec.secondary      = {47.39f, 51.45f};
    spec.muted          = true;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "vertical-bar-13: Explicit BAR / CHART; 2 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.color          = 0u;
    spec.alpha          = 0.1f;
    spec.cornerRadius   = 8.0f;
    spec.groupWidth     = 0.5f;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}}, false};
    spec.yAxis          = {{{"0", 0.0f}, {"10", 10.0f}, {"20", 20.0f}, {"30", 30.0f}, {"40", 40.0f}, {"50", 50.0f}, {"60", 60.0f}, {"70", 70.0f}, {"80", 80.0f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "vertical-bar-14";
    spec.category       = "Vertical Bar";
    spec.title          = "Vertical Bar (sub)";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {67.926955f, 95.04201f};
    spec.secondary      = {47.42f, 57.35f};
    spec.muted          = true;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "vertical-bar-14: Explicit BAR / CHART; 2 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.cornerRadius   = 8.0f;
    spec.color          = 12966143u;
    spec.groupWidth     = 0.5f;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}}, false};
    spec.yAxis          = {{{"0", 0.0f}, {"10", 10.0f}, {"20", 20.0f}, {"30", 30.0f}, {"40", 40.0f}, {"50", 50.0f}, {"60", 60.0f}, {"70", 70.0f}, {"80", 80.0f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "vertical-bar-15";
    spec.category       = "Vertical Bar";
    spec.title          = "Vertical Bar (Highlight)";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {80.0f, 100.0f};
    spec.secondary      = {47.44f, 63.25f, 30.2f, 24.08f, 37.53f, 15.29f, 9.27f, 49.33f, 37.36f, 31.2f, 8.67f, 20.75f, 1.16f, 33.86f, 24.16f, 23.61f, 25.54f, 22.26f, 7.85f, 63.24f, 43.81f, 28.84f, 35.62f, 45.99f};
    spec.highlight      = true;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "vertical-bar-15: Explicit BAR / CHART; 24 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.cornerRadius   = 8.0f;
    spec.groupWidth     = 0.5f;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}}, false};
    spec.yAxis          = {{{"", 0.0f}, {"", 25.0f}, {"", 50.0f}, {"", 75.0f}, {"", 100.0f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "vertical-bar-16";
    spec.category       = "Vertical Bar";
    spec.title          = "Vertical Bar (Custom Gradient)";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 120.0f;
    spec.values         = {80.0f, 100.0f};
    spec.secondary      = {47.47f, 4.14f, 9.71f, 26.48f, 15.85f, 63.89f, 37.28f, 43.88f, 5.42f, 34.93f, 38.17f, 0.27f, 64.66f, 30.73f, 39.01f, 53.9f, 35.5f, 23.25f, 27.25f, 55.88f, 23.73f, 0.42f, 14.19f, 50.65f};
    spec.gradientFill   = true;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "vertical-bar-16: Explicit BAR / CHART; 24 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.cornerRadius   = 8.0f;
    spec.groupWidth     = 0.5f;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}}, false};
    spec.yAxis          = {{{"0", 0.0f}, {"20", 20.0f}, {"40", 40.0f}, {"60", 60.0f}, {"80", 80.0f}, {"100", 100.0f}, {"120", 120.0f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "vertical-bar-17";
    spec.category       = "Vertical Bar";
    spec.title          = "Vertical Bar selectable";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {57.856653f, 76.123971f, 21.448903f, 28.399348f, 72.283951f, 13.596536f, 9.1272291f, 89.740226f, 4.8182442f, 33.373628f, 61.826132f, 28.447359f, 34.471879f, 21.875f, 17.817215f, 45.755316f, 3.2201646f, 6.2611454f, 0.55727023f, 39.194959f, 27.112483f, 32.211077f, 63.132716f, 13.704561f};
    spec.secondary      = {47.49f, 10.04f, 54.21f, 28.89f, 59.17f, 47.5f, 0.3f, 38.43f, 38.49f, 38.67f, 2.66f, 44.78f, 63.15f, 27.6f, 53.85f, 19.2f, 45.45f, 24.24f, 46.66f, 48.52f, 3.65f, 37.01f, 57.75f, 55.31f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "vertical-bar-17: Explicit BAR / CHART; 24 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"0", 0.0f}, {"3", 3.0f}, {"6", 6.0f}, {"9", 9.0f}, {"12", 12.0f}, {"15", 15.0f}, {"18", 18.0f}, {"21", 21.0f}}, false};
    spec.yAxis          = {{{"0", 0.0f}, {"20", 20.0f}, {"40", 40.0f}, {"60", 60.0f}, {"80", 80.0f}, {"100", 100.0f}, {"120", 120.0f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "vertical-bar-18";
    spec.category       = "Vertical Bar";
    spec.title          = "Vertical Bar Data Point";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 10.0f;
    spec.values         = {2.0f, 8.0f, 5.0f, 7.0f, 4.0f, 5.0f};
    spec.secondary      = {47.52f, 15.93f, 33.72f, 31.29f, 37.49f, 31.1f, 28.32f, 32.98f, 6.56f, 42.4f, 32.15f, 24.29f, 61.65f, 24.47f, 3.69f, 49.5f, 55.41f, 25.23f, 1.07f, 41.16f, 48.57f, 8.59f, 36.32f, 59.98f};
    spec.rangeLows      = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 4.0f};
    spec.pointThemes    = true;
    spec.color          = 32768u;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "vertical-bar-18: Explicit BAR / CHART; 6 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.alpha          = 1.0f;
    spec.cornerRadius   = 0.0f;
    spec.groupWidth     = 0.5f;
    spec.pointColors    = {32768u, 32768u, 32768u, 16711680u, 255u, 32768u};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}}, false};
    spec.yAxis          = {{{"0", 0.0f}, {"2", 2.0f}, {"4", 4.0f}, {"6", 6.0f}, {"8", 8.0f}, {"10", 10.0f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "vertical-bar-19";
    spec.category       = "Vertical Bar";
    spec.title          = "Preset_VBar_Simple";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 120.0f;
    spec.values         = {74.050069f, 8.3667696f, 99.370713f, 59.407579f, 8.2304527f, 17.444273f, 21.616941f};
    spec.secondary      = {47.54f, 21.83f, 13.22f, 33.7f, 15.81f, 14.7f, 56.34f};
    spec.simple         = true;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "vertical-bar-19: Explicit BAR / CHART; 2 points";
    spec.secondaryChart = false;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.color          = 12966143u;
    spec.alpha          = 1.0f;
    spec.groupWidth     = 0.6f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}}, false};
    spec.yAxis          = {{{"0", 0.0f}, {"20", 20.0f}, {"40", 40.0f}, {"60", 60.0f}, {"80", 80.0f}, {"100", 100.0f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "vertical-bar-20";
    spec.category       = "Vertical Bar";
    spec.title          = "Preset_VBar_Simple (no interaction)";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {96.057099f, 1.2585734f, 94.011488f, 5.1800412f, 84.146948f, 9.9245542f, 68.932613f};
    spec.secondary      = {47.57f, 27.72f, 57.73f, 36.11f, 59.13f, 63.3f, 19.35f};
    spec.simple         = true;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = false;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "vertical-bar-20: Explicit BAR / CHART; 2 points";
    spec.secondaryChart = false;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.color          = 12966143u;
    spec.alpha          = 1.0f;
    spec.groupWidth     = 0.6f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}}, false};
    spec.yAxis          = {{{"0", 0.0f}, {"10", 10.0f}, {"20", 20.0f}, {"30", 30.0f}, {"40", 40.0f}, {"50", 50.0f}, {"60", 60.0f}, {"70", 70.0f}, {"80", 80.0f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "vertical-bar-21";
    spec.category       = "Vertical Bar";
    spec.title          = "Preset_VBar_Candle";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 3.5f;
    spec.values         = {63.49f, 61.27f, 38.43f, 75.77f, 17.79f, 1.63f, 88.08f, 36.14f, 97.57f, 69.63f, 68.04f, 40.93f, 80.4f, 30.54f, 95.49f, 12.67f, 30.34f, 95.28f, 62.72f, 14.2f, 65.34f, 69.26f, 6.98f, 3.62f};
    spec.secondary      = {47.59f, 33.62f, 37.24f, 38.51f, 37.45f, 46.9f, 47.37f, 16.63f, 40.75f, 53.6f, 55.63f, 27.83f, 57.14f, 15.07f, 48.21f, 10.38f, 20.27f, 28.19f, 59.29f, 19.09f, 53.34f, 53.34f, 37.01f, 8.96f};
    spec.rangeLows      = {22.22f, 21.44f, 13.45f, 26.52f, 6.23f, 0.57f, 30.83f, 12.65f, 34.15f, 24.37f, 23.81f, 14.33f, 28.14f, 10.69f, 33.42f, 4.43f, 10.62f, 33.35f, 21.95f, 4.97f, 22.87f, 24.24f, 2.44f, 1.27f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "vertical-bar-21: Explicit BAR / CHART; 24 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.3f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}}, false};
    spec.yAxis          = {{{"0", 0.0f}, {"0.5", 0.5f}, {"1.0", 1.0f}, {"1.5", 1.5f}, {"2.0", 2.0f}, {"2.5", 2.5f}, {"3.0", 3.0f}, {"3.5", 3.5f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "vertical-bar-22";
    spec.pageSizes      = {25, 25, 22};
    spec.category       = "Vertical Bar";
    spec.title          = "Preset_VBar scroll";
    spec.kind           = ChartKind::BAR;
    spec.pageSize       = 25;
    spec.minimum        = 0.0f;
    spec.maximum        = 120.0f;
    spec.values         = {49.721365f, 1.3447359f, 10.324074f, 40.239626f, 2.8197874f, 46.82999f, 30.294925f, 26.054098f, 10.65072f, 36.553927f, 37.714335f, 18.452932f, 25.683299f, 4.4671639f, 36.903292f, 14.349709f, 42.979252f, 32.668467f, 19.219822f, 19.917267f, 22.415123f, 6.9603052f, 1.9478738f, 22.809928f, 88.475652f, 11.47548f, 1.0390947f, 95.4381f, 95.907064f, 44.421296f, 68.141289f, 90.523834f, 9.0997942f, 39.918553f, 28.659122f, 28.407922f, 44.103224f, 43.646262f, 35.679012f, 14.028635f, 68.818587f, 64.246399f, 83.028121f, 37.509431f, 47.443416f, 84.435014f, 83.052126f, 18.603395f, 25.791324f, 19.390003f, 5.5298354f, 8.9021776f, 19.013203f, 29.072788f, 8.2167352f, 7.0623285f, 46.597222f, 40.401663f, 0.20404664f, 24.769805f, 22.123628f, 15.105024f, 30.257202f, 20.049297f, 18.431927f, 29.72608f, 0.84533608f, 26.851423f, 39.843107f, 31.178412f, 7.0301783f, 27.274949f};
    spec.secondary      = {23.81f, 19.76f, 8.37f, 20.46f, 7.89f, 15.25f, 5.19f, 5.59f, 4.41f, 28.67f, 10.06f, 3.67f, 27.82f, 5.97f, 31.53f, 20.34f, 15.11f, 14.59f, 6.85f, 5.86f, 16.63f, 12.46f, 7.79f, 6.81f, 5.68f, 9.26f, 3.63f, 5.35f, 30.78f, 17.04f, 22.89f, 17.41f, 12.2f, 8.11f, 3.23f, 21.68f, 20.15f, 8.74f, 24.82f, 14.43f, 27.72f, 10.14f, 5.45f, 5.82f, 24.61f, 4.0f, 23.83f, 9.02f, 21.01f, 3.04f, 8.18f, 16.59f, 31.38f, 12.37f, 5.2f, 31.51f, 6.28f, 23.42f, 10.47f, 1.19f, 13.22f, 12.54f, 6.41f, 30.68f, 1.03f, 32.21f, 17.69f, 24.47f, 17.32f, 3.35f, 1.49f, 26.96f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::SCROLL;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "vertical-bar-22: Explicit BAR / CHART; 72 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"0", 0.0f}, {"3", 3.0f}, {"6", 6.0f}, {"9", 9.0f}, {"12", 12.0f}, {"15", 15.0f}, {"18", 18.0f}, {"21", 21.0f}, {"24", 24.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"20", 20.0f}, {"40", 40.0f}, {"60", 60.0f}, {"80", 80.0f}}, true};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "vertical-bar-23";
    spec.pageSizes      = {25, 10, 26, 24, 35};
    spec.category       = "Vertical Bar";
    spec.title          = "Preset_VBar Swipe (update data + labels)";
    spec.kind           = ChartKind::BAR;
    spec.pageSize       = 24;
    spec.minimum        = 0.0f;
    spec.maximum        = 120.0f;
    spec.values         = {55.430384f, 51.269719f, 17.376543f, 14.244684f, 98.664266f, 1.4994856f, 72.133059f, 89.577332f, 40.252058f, 62.428841f, 57.34225f, 66.967593f, 18.465364f, 35.292353f, 14.979424f, 73.57596f, 56.760974f, 17.620885f, 61.093964f, 55.081447f, 8.2253086f, 14.352709f, 63.587106f, 20.126029f, 66.685528f, 15.611283f, 86.656379f, 51.425754f, 44.487311f, 41.149691f, 1.9067215f, 83.548525f, 16.9393f, 51.461763f, 66.128258f, 47.358539f, 33.424211f, 25.559842f, 65.740741f, 81.127401f, 95.176612f, 5.4192387f, 27.904664f, 8.3118999f, 99.727366f, 7.0893347f, 98.29904f, 81.998457f, 52.014746f, 98.471365f, 85.565844f, 96.014232f, 42.162208f, 43.76286f, 72.421125f, 62.704904f, 89.92284f, 14.568759f, 93.432785f, 57.379115f, 55.790466f, 67.679184f, 79.465021f, 29.419582f, 18.777435f, 89.513889f, 68.789438f, 80.060871f, 20.859054f, 7.2333676f, 84.862826f, 6.8338477f, 78.084705f, 66.516632f, 80.771605f, 14.676783f, 58.355624f, 76.005658f, 50.342936f, 93.713134f, 25.869342f, 18.416495f, 5.9224966f, 63.695988f, 52.230796f, 28.317044f, 22.81893f, 85.11917f, 94.23011f, 36.571502f, 50.414952f, 36.995027f, 38.287037f, 81.451475f, 89.94513f, 61.298868f, 11.562071f, 86.413752f, 38.940329f, 74.080075f, 59.734225f, 4.5447531f, 2.3388203f, 43.239883f, 91.445473f, 29.671639f, 70.26406f, 32.975823f, 89.411866f, 74.140089f, 62.469136f, 14.892833f, 88.201303f, 13.258745f, 39.447874f, 45.781036f, 18.677984f, 96.410322f, 80.21262f, 12.060185f, 19.113512f, 24.82939f, 26.738683f, 40.890775f, 12.964678f};
    spec.secondary      = {16.67f, 15.89f, 21.44f, 15.16f, 20.68f, 4.94f, 13.44f, 2.01f, 14.66f, 21.37f, 17.36f, 18.15f, 18.95f, 3.08f, 4.51f, 2.09f, 14.06f, 10.56f, 11.59f, 1.53f, 4.61f, 21.53f, 20.7f, 6.4f, 6.64f, 16.54f, 16.18f, 21.19f, 17.22f, 3.22f, 6.64f, 15.71f, 7.71f, 19.47f, 11.3f, 13.42f, 10.84f, 2.07f, 15.06f, 13.44f, 13.32f, 4.59f, 16.54f, 16.73f, 8.22f, 2.78f, 2.97f, 0.44f, 21.48f, 10.18f, 10.1f, 8.42f, 0.37f, 3.69f, 21.68f, 8.66f, 3.6f, 6.49f, 14.28f, 15.51f, 1.22f, 21.49f, 4.48f, 2.26f, 8.11f, 14.77f, 6.16f, 22.35f, 6.97f, 17.61f, 16.4f, 7.84f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::SWIPE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "vertical-bar-23: Explicit BAR / CHART; 72 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"0", 0.0f}, {"3", 3.0f}, {"6", 6.0f}, {"9", 9.0f}, {"12", 12.0f}, {"15", 15.0f}, {"18", 18.0f}, {"21", 21.0f}, {"24", 24.0f}}, true};
    spec.yAxis          = {{{"0", 0.0f}, {"20", 20.0f}, {"40", 40.0f}, {"60", 60.0f}, {"80", 80.0f}, {"100", 100.0f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "vertical-bar-24";
    spec.pageSizes      = {25, 25, 25, 25, 25, 19};
    spec.category       = "Vertical Bar";
    spec.title          = "Preset_VBar Swipe (update scaleX)";
    spec.kind           = ChartKind::BAR;
    spec.pageSize       = 25;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {96.046811f, 95.075446f, 77.951818f, 53.317901f, 15.000857f, 53.124143f, 31.885288f, 34.000343f, 71.814986f, 65.082305f, 85.407236f, 67.357682f, 86.242284f, 2.5548697f, 73.085562f, 28.69856f, 18.776577f, 22.331962f, 25.784465f, 67.405693f, 48.430213f, 10.833333f, 81.775549f, 84.713649f, 17.178498f, 95.219479f, 64.515604f, 78.153292f, 41.070816f, 21.16941f, 27.091049f, 52.662894f, 88.008402f, 97.325103f, 63.329047f, 98.365912f, 22.188786f, 6.4026063f, 85.575274f, 35.015432f, 15.216907f, 82.969822f, 69.138374f, 23.105281f, 23.882888f, 57.890946f, 63.401063f, 41.647805f, 34.606481f, 69.437586f, 69.597908f, 32.618313f, 74.548182f, 41.066529f, 85.259774f, 12.066187f, 89.387003f, 25.864198f, 15.324931f, 47.892661f, 87.764918f, 17.65775f, 49.916838f, 4.2952675f, 52.397977f, 28.792867f, 8.7885803f, 52.878944f, 17.854081f, 34.578189f, 52.433985f, 50.433813f, 14.997428f, 84.396433f, 59.865398f, 83.37963f, 82.099623f, 79.482167f, 73.058128f, 78.876886f, 42.617455f, 17.366255f, 8.0615569f, 82.604595f, 49.637346f, 2.9869684f, 32.77692f, 3.2047325f, 96.986454f, 26.467764f, 11.401749f, 23.393347f, 97.01046f, 7.5617284f, 15.540981f, 77.73834f, 25.018004f, 6.7626886f, 1.9847394f, 97.103909f, 30.391804f, 3.0829904f, 57.152778f, 19.76166f, 14.366427f, 38.497942f, 8.2055899f, 69.168381f, 74.472737f, 29.056927f, 0.82218793f, 98.410494f, 15.649005f, 42.66118f, 43.644547f, 1.3151578f, 28.01869f, 43.50823f, 19.388717f, 90.228052f, 31.334877f, 3.2030178f, 62.622599f, 40.457819f, 86.091392f, 78.535665f, 4.2103909f, 1.3871742f, 71.300583f, 55.925926f, 82.423697f, 74.250686f, 28.937757f, 62.534294f, 20.719307f, 56.579218f, 75.052298f, 44.039781f, 72.183642f, 53.311043f, 77.545439f, 9.0843621f, 30.643861f, 54.569616f};
    spec.secondary      = {47.67f, 51.31f, 40.75f, 45.73f, 37.41f, 62.7f, 1.42f, 0.28f, 9.95f, 64.81f, 14.1f, 31.37f, 52.63f, 5.68f, 27.74f, 36.27f, 50.14f, 31.15f, 52.52f, 62.01f, 58.1f, 33.09f, 37.71f, 22.94f, 26.61f, 10.96f, 20.17f, 45.41f, 36.85f, 49.33f, 57.17f, 54.98f, 19.64f, 30.05f, 58.11f, 33.33f, 21.66f, 59.35f, 36.43f, 47.91f, 20.66f, 5.93f, 18.64f, 18.99f, 62.73f, 7.88f, 34.28f, 49.5f, 15.74f, 52.11f, 41.36f, 14.96f, 4.32f, 61.36f, 48.46f, 51.46f, 8.02f, 55.23f, 60.65f, 21.25f, 45.55f, 32.7f, 12.77f, 16.56f, 44.26f, 19.99f, 64.85f, 13.76f, 5.18f, 28.91f, 25.7f, 55.88f, 50.41f, 11.88f, 1.89f, 18.06f, 20.5f, 46.44f, 30.65f, 41.57f, 61.3f, 22.74f, 46.53f, 8.44f, 15.28f, 9.46f, 52.67f, 19.77f, 64.34f, 19.15f, 59.17f, 22.32f, 16.73f, 55.84f, 16.82f, 58.94f, 42.32f, 43.58f, 56.22f, 34.93f, 21.63f, 31.02f, 58.05f, 53.42f, 20.24f, 43.41f, 1.99f, 49.3f, 61.42f, 9.6f, 22.64f, 2.37f, 51.22f, 12.73f, 27.22f, 11.64f, 11.36f, 24.89f, 46.24f, 49.03f, 11.49f, 56.94f, 34.09f, 6.91f, 53.85f, 46.11f, 63.44f, 20.18f, 49.38f, 55.21f, 52.86f, 19.81f, 40.68f, 8.53f, 3.93f, 26.09f, 63.47f, 25.72f, 50.01f, 41.04f, 29.45f, 26.33f, 25.16f, 40.62f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA_X;
    spec.navigation     = Navigation::SWIPE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "vertical-bar-24: Explicit BAR / CHART; 144 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"0", 0.0f}, {"3", 3.0f}, {"6", 6.0f}, {"9", 9.0f}, {"12", 12.0f}, {"15", 15.0f}, {"18", 18.0f}, {"21", 21.0f}, {"24", 24.0f}}, true};
    spec.yAxis          = {{{"0", 0.0f}, {"20", 20.0f}, {"40", 40.0f}, {"60", 60.0f}, {"80", 80.0f}, {"100", 100.0f}, {"120", 120.0f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "vertical-bar-25";
    spec.category       = "Vertical Bar";
    spec.title          = "Vertical Bar (Per-Point Themes)";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 80.0f;
    spec.values         = {50.0f, 70.0f, 40.0f, 30.0f};
    spec.secondary      = {47.69f, 57.2f, 20.26f, 48.14f, 15.73f, 46.3f, 29.44f, 59.83f, 43.02f, 3.54f, 43.6f, 10.88f, 51.12f, 2.55f, 42.58f, 1.57f, 60.09f, 32.14f, 6.93f, 54.65f, 38.02f, 4.68f, 16.27f, 27.6f};
    spec.pointThemes    = true;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "vertical-bar-25: Explicit BAR / CHART; 24 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.cornerRadius   = 8.0f;
    spec.groupWidth     = 0.5f;
    spec.pointColors    = {3701503u, 12573183u, 12040123u, 15856115u};
    spec.pointBorders   = {{1u, 0x387AFFu}, {3u, 0xB7B7BBu}};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}}, false};
    spec.yAxis          = {{{"0", 0.0f}, {"10", 10.0f}, {"20", 20.0f}, {"30", 30.0f}, {"40", 40.0f}, {"50", 50.0f}, {"60", 60.0f}, {"70", 70.0f}, {"80", 80.0f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "vertical-bar-26";
    spec.category       = "Vertical Bar";
    spec.title          = "Vertical Bar (Per-Point Themes, selectable)";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 80.0f;
    spec.values         = {50.0f, 70.0f, 40.0f, 30.0f};
    spec.secondary      = {47.72f, 63.1f, 64.77f, 50.54f, 59.05f, 29.91f, 57.46f, 54.38f, 11.09f, 7.27f, 8.09f, 55.39f, 49.62f, 64.41f, 57.42f, 31.87f, 5.05f, 33.13f, 26.33f, 47.29f, 17.94f, 41.26f, 59.84f, 32.26f};
    spec.pointThemes    = true;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "vertical-bar-26: Explicit BAR / CHART; 24 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.cornerRadius   = 8.0f;
    spec.groupWidth     = 0.5f;
    spec.pointColors    = {3701503u, 12573183u, 12040123u, 15856115u};
    spec.pointBorders   = {{1u, 0x387AFFu}, {3u, 0xB7B7BBu}};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}}, false};
    spec.yAxis          = {{{"0", 0.0f}, {"10", 10.0f}, {"20", 20.0f}, {"30", 30.0f}, {"40", 40.0f}, {"50", 50.0f}, {"60", 60.0f}, {"70", 70.0f}, {"80", 80.0f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "vertical-bar-27";
    spec.category       = "Vertical Bar";
    spec.title          = "Vertical Bar (Per-Point Themes, Status)";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 80.0f;
    spec.values         = {50.0f, 50.0f, 70.0f, 70.0f, 40.0f, 40.0f, 30.0f, 30.0f};
    spec.secondary      = {47.74f, 3.99f, 44.27f, 52.95f, 37.37f, 13.51f, 20.48f, 48.93f, 44.15f, 11.01f, 37.58f, 34.91f, 48.11f, 61.28f, 7.26f, 62.16f, 15.0f, 34.12f, 45.74f, 39.93f, 62.86f, 12.85f, 38.4f, 36.93f};
    spec.statusLevels   = 5;
    spec.pointThemes    = true;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "vertical-bar-27: Explicit BAR / CHART; 24 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.pointColors    = {3701503u, 3701503u, 12573183u, 12573183u, 12040123u, 12040123u, 15856115u, 15856115u};
    spec.pointBorders   = {{2u, 0x387AFFu}, {3u, 0x387AFFu}, {6u, 0xB7B7BBu}, {7u, 0xB7B7BBu}};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}}, false};
    spec.yAxis          = {{{"0", 0.0f}, {"10", 10.0f}, {"20", 20.0f}, {"30", 30.0f}, {"40", 40.0f}, {"50", 50.0f}, {"60", 60.0f}, {"70", 70.0f}, {"80", 80.0f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id        = "vertical-bar-28";
    spec.category  = "Vertical Bar";
    spec.title     = "Comp_Area_Solid";
    spec.kind      = ChartKind::BAR;
    spec.minimum   = 0.0f;
    spec.maximum   = 120.0f;
    spec.values    = {10.0f, std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), 50.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 20.0f, std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), 72.0f, 70.0f};
    spec.secondary = {47.77f, 9.89f, 23.78f, 55.36f, 15.69f, 62.11f, 48.49f, 43.48f, 12.22f, 14.74f, 2.07f, 14.42f, 46.61f, 58.15f, 22.1f, 27.46f, 24.96f, 35.11f, 0.15f, 32.57f, 42.78f, 49.43f, 16.97f, 41.59f};
    spec.fill      = true;
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::AREA;
      annotation.x        = 0.0f;
      annotation.y        = 20.0f;
      annotation.maximumY = 50.0f;
      annotation.point    = -1;
      annotation.text     = "";
      annotation.color    = 3701503u;
      spec.annotations.push_back(annotation);
    }
    spec.axis                        = AxisPreset::DEFAULT;
    spec.layout                      = LayoutPreset::CHART;
    spec.action                      = Action::DATA;
    spec.navigation                  = Navigation::NONE;
    spec.numberMode                  = NumberMode::NONE;
    spec.expected                    = "vertical-bar-28: Explicit BAR / CHART; 24 points";
    spec.secondaryChart              = false;
    spec.chartWidth                  = 312.0f;
    spec.chartHeight                 = 143.0f;
    spec.interactive                 = true;
    spec.frames                      = {{186.5f, 166.5f, 312.0f, 143.0f}};
    spec.referenceWidth              = 685.0f;
    spec.texts                       = {};
    spec.cornerRadius                = 3.2f;
    spec.groupWidth                  = 0.5f;
    spec.xAxis                       = {{{"0", 0.0f}, {"3", 3.0f}, {"6", 6.0f}, {"9", 9.0f}, {"12", 12.0f}, {"15", 15.0f}, {"18", 18.0f}, {"21", 21.0f}}, true};
    spec.yAxis                       = {{{"", 0.0f}, {"30", 30.0f}, {"60", 60.0f}, {"90", 90.0f}, {"120", 120.0f}}, true};
    spec.annotations[0].y            = 40.0f;
    spec.annotations[0].maximumY     = 60.0f;
    spec.annotations[0].minimumX     = 0.0f;
    spec.annotations[0].maximumX     = 26.5f;
    spec.annotations[0].hasXRange    = true;
    spec.annotations[0].cornerRadius = 4.0f;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "vertical-bar-29";
    spec.pageSizes      = {8, 4};
    spec.category       = "Vertical Bar";
    spec.title          = "Vertical Bar with Point Annotations";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {0.61471193f, 40.384088f, 8.4456447f, 80.108025f, 15.865055f, 72.506859f, 80.897634f, 90.420096f, 80.086591f, 36.316872f, 97.382545f, 64.518176f};
    spec.secondary      = {47.79f, 15.79f, 3.28f, 57.76f, 59.01f, 45.71f, 11.51f, 38.03f, 45.29f, 18.48f, 31.56f, 58.93f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::SCROLL;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "vertical-bar-29: Explicit BAR / CHART; 12 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 300.0f;
    spec.chartHeight    = 200.0f;
    spec.pageSize       = 8;
    spec.interactive    = true;
    spec.frames         = {{192.5f, 138.0f, 300.0f, 200.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"25", 25.0f}, {"50", 50.0f}, {"75", 75.0f}, {"100", 100.0f}}, true};
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::MARKER;
      annotation.x        = 0.0f;
      annotation.y        = 0.0f;
      annotation.maximumY = 0.0f;
      annotation.point    = 2;
      annotation.text     = "";
      annotation.color    = 0x387AFFu;
      annotation.width    = 0.0f;
      annotation.ellipsis = false;
      annotation.offsetX  = 0.0f;
      annotation.offsetY  = 0.0f;
      spec.annotations.push_back(annotation);
    }
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::MARKER;
      annotation.x        = 0.0f;
      annotation.y        = 0.0f;
      annotation.maximumY = 0.0f;
      annotation.point    = 5;
      annotation.text     = "";
      annotation.color    = 0x387AFFu;
      annotation.width    = 0.0f;
      annotation.ellipsis = false;
      annotation.offsetX  = 0.0f;
      annotation.offsetY  = 0.0f;
      spec.annotations.push_back(annotation);
    }
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::MARKER;
      annotation.x        = 0.0f;
      annotation.y        = 0.0f;
      annotation.maximumY = 0.0f;
      annotation.point    = 8;
      annotation.text     = "";
      annotation.color    = 0x387AFFu;
      annotation.width    = 0.0f;
      annotation.ellipsis = false;
      annotation.offsetX  = 0.0f;
      annotation.offsetY  = 0.0f;
      spec.annotations.push_back(annotation);
    }
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "vertical-bar-30";
    spec.pageSizes      = {9, 3};
    spec.category       = "Vertical Bar";
    spec.title          = "Vertical Bar with Point Annotations (Default Anim)";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {79.699074f, 70.085734f, 59.134945f, 44.377572f, 41.862997f, 97.270233f, 63.6857f, 46.047668f, 12.257373f, 70.95679f, 15.97308f, 37.429698f};
    spec.secondary      = {47.82f, 21.68f, 47.79f, 60.17f, 37.34f, 29.31f, 39.53f, 32.58f, 13.35f, 22.21f, 61.06f, 38.44f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::SCROLL;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "vertical-bar-30: Explicit BAR / CHART; 12 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 300.0f;
    spec.chartHeight    = 200.0f;
    spec.pageSize       = 9;
    spec.interactive    = true;
    spec.frames         = {{192.5f, 138.0f, 300.0f, 200.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"25", 25.0f}, {"50", 50.0f}, {"75", 75.0f}, {"100", 100.0f}}, true};
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::MARKER;
      annotation.x        = 0.0f;
      annotation.y        = 0.0f;
      annotation.maximumY = 0.0f;
      annotation.point    = 2;
      annotation.text     = "";
      annotation.color    = 0x387AFFu;
      annotation.width    = 0.0f;
      annotation.ellipsis = false;
      annotation.offsetX  = 0.0f;
      annotation.offsetY  = 0.0f;
      spec.annotations.push_back(annotation);
    }
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::MARKER;
      annotation.x        = 0.0f;
      annotation.y        = 0.0f;
      annotation.maximumY = 0.0f;
      annotation.point    = 5;
      annotation.text     = "";
      annotation.color    = 0x387AFFu;
      annotation.width    = 0.0f;
      annotation.ellipsis = false;
      annotation.offsetX  = 0.0f;
      annotation.offsetY  = 0.0f;
      spec.annotations.push_back(annotation);
    }
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "stacked-bar-01";
    spec.category       = "Stacked Bar";
    spec.title          = "Preset_VBar_Stack";
    spec.kind           = ChartKind::STACKED;
    spec.minimum        = 0.0f;
    spec.maximum        = 4.0f;
    spec.values         = {1.8f, 1.8f, 1.6f, 1.0f, 0.8f, 0.1f, 1.0f};
    spec.secondary      = {0.6f, 0.6f, 0.5f, 0.4f, 0.8f, 0.1f, 2.0f};
    spec.legend         = true;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "stacked-bar-01: Explicit STACKED / CHART; 7 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.3f;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}}, true};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "stacked-bar-02";
    spec.category       = "Stacked Bar";
    spec.title          = "Preset_VBar_Stack (3 segments)";
    spec.kind           = ChartKind::STACKED;
    spec.minimum        = 0.0f;
    spec.maximum        = 4.8f;
    spec.values         = {1.8f, 1.8f, 1.6f, 1.0f, 0.8f, 0.1f, 1.0f};
    spec.secondary      = {0.6f, 0.6f, 0.5f, 0.4f, 0.8f, 0.1f, 2.0f};
    spec.legend         = true;
    spec.segments       = 3;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "stacked-bar-02: Explicit STACKED / CHART; 7 points";
    spec.tertiary       = {0.5f, 0.3f, 0.5f, 0.6f, 0.4f, 0.2f, 1.0f};
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.3f;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"1.2", 1.2f}, {"2.4", 2.4f}, {"3.6", 3.6f}, {"4.8", 4.8f}}, true};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "stacked-bar-03";
    spec.category       = "Stacked Bar";
    spec.title          = "Preset_VBar_Stack (data points)";
    spec.kind           = ChartKind::STACKED;
    spec.minimum        = 0.0f;
    spec.maximum        = 4.0f;
    spec.values         = {1.8f, 1.8f, 1.6f, 1.0f, 0.8f, 0.1f, 1.0f};
    spec.secondary      = {0.6f, 0.6f, 0.5f, 0.4f, 0.8f, 0.1f, 2.0f};
    spec.legend         = true;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "stacked-bar-03: Explicit STACKED / CHART; 7 points";
    spec.segments       = 2;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.3f;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}}, true};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "stacked-bar-04";
    spec.category       = "Stacked Bar";
    spec.title          = "Preset_VBar_Stack (selectable)";
    spec.kind           = ChartKind::STACKED;
    spec.minimum        = 0.0f;
    spec.maximum        = 4.0f;
    spec.values         = {1.8f, 1.8f, 1.6f, 1.0f, 0.8f, 0.1f, 1.0f};
    spec.secondary      = {0.6f, 0.6f, 0.5f, 0.4f, 0.8f, 0.1f, 2.0f};
    spec.legend         = true;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "stacked-bar-04: Explicit STACKED / CHART; 7 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.3f;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}}, true};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "stacked-bar-05";
    spec.pageSizes      = {6, 6};
    spec.category       = "Stacked Bar";
    spec.title          = "Preset_VBar_Stack (swipe)";
    spec.kind           = ChartKind::STACKED;
    spec.pageSize       = 6;
    spec.minimum        = 0.0f;
    spec.maximum        = 4.0f;
    spec.values         = {1.8f, 1.6f, 1.6f, 1.0f, 0.8f, 0.1f, 1.0f, 1.4f, 1.2f, 0.5f, 0.6f, 1.3f};
    spec.secondary      = {0.6f, 0.6f, 0.5f, 0.4f, 0.8f, 0.1f, 2.0f, 0.9f, 0.8f, 0.4f, 0.6f, 1.5f};
    spec.legend         = true;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::SWIPE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "stacked-bar-05: Explicit STACKED / CHART; 12 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.3f;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}}, true};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "line-01";
    spec.category       = "Line";
    spec.title          = "Line Basic Main Theme - Default";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 8.0f;
    spec.values         = {5.0f, 6.0f, 7.0f, 4.0f, 2.0f, 3.0f, 6.0f, 5.0f, 7.0f};
    spec.secondary      = {9.59f, 11.41f, 10.97f, 1.92f, 7.45f, 12.18f, 2.53f, 12.98f, 3.35f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "line-01: Explicit LINE / CHART; 9 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, false};
    spec.yAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, false};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 4.0f, 0x387AFFu, 1.0f});
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "line-02";
    spec.category       = "Line";
    spec.title          = "Line Basic Sub Theme - Default";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 8.0f;
    spec.values         = {5.0f, 6.0f, 7.0f, 4.0f, 2.0f, 3.0f, 6.0f, 5.0f, 7.0f};
    spec.secondary      = {3.36f, 4.41f, 2.4f, 0.84f, 1.09f, 3.12f, 2.85f, 4.16f, 3.49f};
    spec.muted          = true;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "line-02: Explicit LINE / CHART; 9 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.color          = 12966143u;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, false};
    spec.yAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, false};
    spec.lines.push_back({0xE9E9ECu, 1.0f, 2.5f, 4.0f, 0xCCCCCEu, 1.0f});
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "line-03";
    spec.category       = "Line";
    spec.title          = "Line Basic Main Theme - Forecast";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 8.0f;
    spec.values         = {5.0f, 6.0f, 7.0f, 4.0f, 2.0f, 3.0f, 6.0f, 5.0f, 7.0f};
    spec.secondary      = {3.36f, 0.27f, 0.97f, 1.01f, 4.12f, 1.97f, 0.26f, 3.78f, 1.25f};
    spec.forecast       = true;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "line-03: Explicit LINE / CHART; 9 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, false};
    spec.yAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, false};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 4.0f, 0xBFD9FFu, 1.0f});
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "line-04";
    spec.category       = "Line";
    spec.title          = "Line Basic Sub Theme - Forecast";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 8.0f;
    spec.values         = {5.0f, 6.0f, 7.0f, 4.0f, 2.0f, 3.0f, 6.0f, 5.0f, 7.0f};
    spec.secondary      = {3.36f, 0.68f, 4.08f, 1.18f, 2.61f, 0.82f, 2.22f, 3.4f, 3.57f};
    spec.forecast       = true;
    spec.muted          = true;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "line-04: Explicit LINE / CHART; 9 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.color          = 12966143u;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, false};
    spec.yAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, false};
    spec.lines.push_back({0xCCCCCEu, 1.0f, 2.5f, 4.0f, 0xCCCCCEu, 1.0f});
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "line-05";
    spec.category       = "Line";
    spec.title          = "Line Basic Segment";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 8.0f;
    spec.values         = {5.0f, 6.0f, 7.0f, 4.0f, 2.0f, 3.0f, 6.0f, 5.0f, 7.0f};
    spec.secondary      = {3.36f, 1.09f, 2.65f, 1.35f, 1.09f, 4.22f, 4.18f, 3.02f, 1.33f};
    spec.fill           = true;
    spec.segmentStyle   = true;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "line-05: Explicit LINE / CHART; 9 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, false};
    spec.yAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, false};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 4.0f, 0x387AFFu, 1.0f});
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "line-06";
    spec.category       = "Line";
    spec.title          = "Line Basic Main Theme - Default Multiple Data Point";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 20.0f;
    spec.values         = {10.0f, 18.0f, 12.0f, 13.0f};
    spec.secondary      = {};
    spec.pointThemes    = true;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "line-06: Single chart with point overrides";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.pointColors    = {16711680u, 255u, 32768u, 3701503u};
    spec.labels         = {"0", "4", "7", "3"};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}}, false};
    spec.yAxis          = {{{"0", 0.0f}, {"5", 5.0f}, {"10", 10.0f}, {"15", 15.0f}, {"20", 20.0f}}, false};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 4.0f, 0xFF0000u, 1.0f});
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id              = "line-07";
    spec.category        = "Line";
    spec.title           = "Preset_Line_Simple";
    spec.kind            = ChartKind::LINE;
    spec.minimum         = 0.0f;
    spec.maximum         = 9.0f;
    spec.values          = {2.0f, 3.0f, 4.0f, 8.0f};
    spec.secondary       = {2.0f, 3.0f, 4.0f, 3.0f, 6.0f, 8.0f, 8.0f, 11.0f, 10.0f, 14.0f, 9.0f, 10.0f};
    spec.simple          = true;
    spec.axis            = AxisPreset::DEFAULT;
    spec.layout          = LayoutPreset::CHART;
    spec.action          = Action::DATA;
    spec.navigation      = Navigation::NONE;
    spec.numberMode      = NumberMode::NONE;
    spec.expected        = "line-07: Explicit LINE / CHART; 12 points";
    spec.lineSeriesCount = 2;
    spec.secondaryChart  = false;
    spec.chartWidth      = 643.0f;
    spec.chartHeight     = 150.0f;
    spec.interactive     = true;
    spec.frames          = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth  = 685.0f;
    spec.texts           = {};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "line-08";
    spec.category       = "Line";
    spec.title          = "Preset_Line (Solid Line)";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 8.0f;
    spec.values         = {5.0f, 6.0f, 7.0f, 4.0f, 2.0f, 3.0f, 6.0f, 5.0f, 7.0f};
    spec.secondary      = {9.63f, 6.67f, 8.27f, 5.29f, 3.1f, 2.23f, 2.75f, 5.35f, 10.64f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "line-08: Explicit LINE / CHART; 9 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, false};
    spec.yAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, false};
    spec.lines.push_back({0x008000u, 1.0f, 2.5f, 4.0f, 0x008000u, 1.0f});
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "line-09";
    spec.category       = "Line";
    spec.title          = "Preset_Line (Transparancy Line)";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 8.0f;
    spec.values         = {5.0f, 6.0f, 7.0f, 4.0f, 2.0f, 3.0f, 6.0f, 5.0f, 7.0f};
    spec.secondary      = {9.63f, 7.84f, 4.17f, 5.77f, 11.76f, 11.95f, 8.35f, 4.26f, 4.26f};
    spec.muted          = true;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "line-09: Explicit LINE / CHART; 9 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.color          = 12966143u;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, false};
    spec.yAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, false};
    spec.lines.push_back({0x000000u, 0.2f, 2.5f, 4.0f, 0x000000u, 0.2f});
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "line-10";
    spec.category       = "Line";
    spec.title          = "Preset_Line (gradient)";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 8.0f;
    spec.values         = {5.0f, 6.0f, 7.0f, 4.0f, 2.0f, 3.0f, 6.0f, 5.0f, 7.0f};
    spec.secondary      = {3.37f, 3.16f, 0.03f, 2.19f, 2.6f, 3.03f, 0.34f, 1.11f, 3.8f};
    spec.gradientStroke = true;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "line-10: Explicit LINE / CHART; 9 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, false};
    spec.yAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, false};
    spec.lines.push_back({0x387AFFu, 1.0f, 2.5f, 4.0f, 0x387AFFu, 1.0f});
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "line-11";
    spec.category       = "Line";
    spec.title          = "Preset_Line (Solid Background)";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 8.0f;
    spec.values         = {5.0f, 6.0f, 7.0f, 4.0f, 2.0f, 3.0f, 6.0f, 5.0f, 7.0f};
    spec.secondary      = {9.64f, 10.2f, 8.98f, 6.73f, 3.09f, 5.39f, 6.56f, 2.08f, 4.48f};
    spec.fill           = true;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "line-11: Explicit LINE / CHART; 9 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.fillColor      = 255u;
    spec.fillAlpha      = 1.0f;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, false};
    spec.yAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, false};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 4.0f, 0x387AFFu, 1.0f});
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "line-12";
    spec.category       = "Line";
    spec.title          = "Preset_Line (Gradient Background)";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 8.0f;
    spec.values         = {5.0f, 6.0f, 7.0f, 4.0f, 2.0f, 3.0f, 6.0f, 5.0f, 7.0f};
    spec.secondary      = {3.38f, 3.98f, 1.71f, 2.52f, 4.11f, 0.74f, 4.26f, 0.35f, 3.88f};
    spec.fill           = true;
    spec.gradientFill   = true;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "line-12: Explicit LINE / CHART; 9 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, false};
    spec.yAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, false};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 4.0f, 0x387AFFu, 1.0f});
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id        = "line-13";
    spec.category  = "Line";
    spec.title     = "Preset_Line_Area_Guidance";
    spec.kind      = ChartKind::LINE;
    spec.minimum   = 0.0f;
    spec.maximum   = 8.0f;
    spec.values    = {5.0f, 6.0f, 7.0f, 4.0f, 2.0f, 3.0f, 6.0f, 5.0f, 7.0f};
    spec.secondary = {9.65f, 12.56f, 0.78f, 7.7f, 7.42f, 11.83f, 4.77f, 12.9f, 4.71f};
    spec.fill      = true;
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::AREA;
      annotation.x        = 0.0f;
      annotation.y        = 2.0f;
      annotation.maximumY = 5.0f;
      annotation.point    = -1;
      annotation.text     = "";
      annotation.color    = 3701503u;
      annotation.pattern  = true;
      spec.annotations.push_back(annotation);
    }
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "line-13: Explicit LINE / CHART; 9 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, false};
    spec.yAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, false};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 4.0f, 0x387AFFu, 1.0f});
    spec.annotations[0].y            = 0.0f;
    spec.annotations[0].maximumY     = 3.5f;
    spec.annotations[0].cornerRadius = 8.0f;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "line-14";
    spec.pageSizes      = {6, 3};
    spec.category       = "Line";
    spec.title          = "Line with Point Annotations";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 10.0f;
    spec.values         = {5.0f, 6.0f, 7.0f, 4.0f, 2.0f, 3.0f, 6.0f, 5.0f, 7.0f};
    spec.secondary      = {3.38f, 0.26f, 3.39f, 2.86f, 1.08f, 2.99f, 3.63f, 4.13f, 3.96f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::SCROLL;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "line-14: Explicit LINE / CHART; 9 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 300.0f;
    spec.chartHeight    = 200.0f;
    spec.pageSize       = 6;
    spec.interactive    = true;
    spec.frames         = {{192.5f, 138.0f, 300.0f, 200.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"2.5", 2.5f}, {"5", 5.0f}, {"7.5", 7.5f}, {"10", 10.0f}}, true};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 4.0f, 0x387AFFu, 1.0f});
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::MARKER;
      annotation.x        = 0.0f;
      annotation.y        = 0.0f;
      annotation.maximumY = 0.0f;
      annotation.point    = 2;
      annotation.text     = "";
      annotation.color    = 0x387AFFu;
      annotation.width    = 0.0f;
      annotation.ellipsis = false;
      annotation.offsetX  = 0.0f;
      annotation.offsetY  = 0.0f;
      spec.annotations.push_back(annotation);
    }
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::MARKER;
      annotation.x        = 0.0f;
      annotation.y        = 0.0f;
      annotation.maximumY = 0.0f;
      annotation.point    = 5;
      annotation.text     = "";
      annotation.color    = 0x387AFFu;
      annotation.width    = 0.0f;
      annotation.ellipsis = false;
      annotation.offsetX  = 0.0f;
      annotation.offsetY  = 0.0f;
      spec.annotations.push_back(annotation);
    }
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::MARKER;
      annotation.x        = 0.0f;
      annotation.y        = 0.0f;
      annotation.maximumY = 0.0f;
      annotation.point    = 7;
      annotation.text     = "";
      annotation.color    = 0x387AFFu;
      annotation.width    = 0.0f;
      annotation.ellipsis = false;
      annotation.offsetX  = 0.0f;
      annotation.offsetY  = 0.0f;
      spec.annotations.push_back(annotation);
    }
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "combo-01";
    spec.category       = "Combo";
    spec.title          = "Preset_Combo_Basic 1";
    spec.kind           = ChartKind::COMBO;
    spec.minimum        = 0.0f;
    spec.maximum        = 240.0f;
    spec.values         = {10.0f, 15.0f, 10.0f, 30.0f, 40.0f, 60.0f, 75.0f, 70.0f, 90.0f, 80.0f, 80.0f, 75.0f, 90.0f, 100.0f, 130.0f, 140.0f, 135.0f, 140.0f, 170.0f, 190.0f};
    spec.secondary      = {30.0f, 20.0f, 25.0f, 40.0f, 45.0f, 50.0f, 60.0f, 60.0f, 50.0f, 55.0f, 70.0f, 65.0f, 60.0f, 65.0f, 70.0f, 80.0f, 80.0f, 75.0f, 70.0f, 65.0f, 70.0f, 75.0f, 70.0f, 60.0f, 65.0f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "combo-01: Explicit COMBO / CHART; 20 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.7f;
    spec.cornerRadius   = 10000.0f;
    spec.pointColors    = {255u, 255u, 255u, 255u, 255u, 32768u, 32768u, 32768u, 32768u, 32768u, 32768u, 32768u, 32768u, 16753920u, 16753920u, 16753920u, 16753920u, 16753920u, 16711680u, 16711680u};
    spec.pointThemes    = true;
    spec.xAxis          = {{{"0", 0.0f}, {"3", 3.0f}, {"6", 6.0f}, {"9", 9.0f}, {"12", 12.0f}, {"15", 15.0f}, {"18", 18.0f}, {"21", 21.0f}, {"24", 24.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"60", 60.0f}, {"120", 120.0f}, {"180", 180.0f}, {"240", 240.0f}}, true};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 0.0f, 0x387AFFu, 1.0f});
    cases.push_back(std::move(spec));
  }
}

void AppendReferenceCases50To99(std::vector<ReferenceCase>& cases)
{
  {
    ReferenceCase spec;
    spec.id             = "combo-02";
    spec.category       = "Combo";
    spec.title          = "Preset_Combo_Basic (Line position))";
    spec.kind           = ChartKind::COMBO;
    spec.minimum        = 0.0f;
    spec.maximum        = 200.0f;
    spec.values         = {10.0f, 15.0f, 10.0f, 30.0f, 40.0f, 60.0f, 75.0f, 70.0f, 90.0f, 80.0f, 80.0f, 75.0f, 90.0f, 100.0f, 130.0f, 140.0f, 135.0f, 140.0f, 170.0f, 190.0f};
    spec.secondary      = {30.0f, 20.0f, 25.0f, 40.0f, 45.0f, 50.0f, 60.0f, 60.0f, 50.0f, 55.0f, 70.0f, 65.0f, 60.0f, 65.0f, 70.0f, 80.0f, 80.0f, 75.0f, 70.0f, 65.0f, 70.0f, 75.0f, 70.0f, 60.0f, 65.0f};
    spec.fill           = true;
    spec.lineBehind     = true;
    spec.segmentStyle   = true;
    spec.statusLevels   = 5;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "combo-02: Explicit COMBO / CHART; 20 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.fillColor      = 15658734u;
    spec.fillAlpha      = 1.0f;
    spec.groupWidth     = 0.7f;
    spec.cornerRadius   = 10000.0f;
    spec.pointColors    = {255u, 255u, 255u, 255u, 255u, 32768u, 32768u, 32768u, 32768u, 32768u, 32768u, 32768u, 32768u, 16753920u, 16753920u, 16753920u, 16753920u, 16753920u, 16711680u, 16711680u};
    spec.xAxis          = {{{"0", 0.0f}, {"3", 3.0f}, {"6", 6.0f}, {"9", 9.0f}, {"12", 12.0f}, {"15", 15.0f}, {"18", 18.0f}, {"21", 21.0f}, {"24", 24.0f}}, false};
    spec.yAxis          = {{{"0", 0.0f}, {"50", 50.0f}, {"100", 100.0f}, {"150", 150.0f}, {"200", 200.0f}}, false};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 0.0f, 0x387AFFu, 1.0f});
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "axis-01";
    spec.category       = "Axis";
    spec.title          = "DataBasicUnit";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 25.0f;
    spec.values         = {20.182613f, 11.033179f, 22.223251f, 17.456533f, 22.658951f, 19.311986f, 7.7860082f};
    spec.secondary      = {14.51f, 6.42f, 15.58f, 14.43f, 4.61f, 17.56f, 1.77f};
    spec.simple         = true;
    spec.chartWidth     = 304.0f;
    spec.chartHeight    = 219.0f;
    spec.axis           = AxisPreset::WEEK_UNIT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "axis-01: Explicit BAR / CHART; 7 points";
    spec.secondaryChart = false;
    spec.interactive    = true;
    spec.frames         = {{190.5f, 128.5f, 304.0f, 219.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"S", 0.0f}, {"M", 1.0f}, {"T", 2.0f}, {"W", 3.0f}, {"T", 4.0f}, {"F", 5.0f}, {"S", 6.0f}}, true};
    spec.yAxis          = {{{"0", 0.0f}, {"5", 5.0f}, {"10", 10.0f}, {"15", 15.0f}, {"20", 20.0f}, {"25", 25.0f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "axis-02";
    spec.category       = "Axis";
    spec.title          = "DataBasicPeriod";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 25.0f;
    spec.values         = {0.67361111f, 6.1229424f, 1.1710391f, 15.07716f, 22.656121f, 7.6116255f, 15.869599f};
    spec.secondary      = {14.52f, 8.19f, 9.43f, 15.15f, 17.61f, 12.64f, 10.18f};
    spec.simple         = true;
    spec.chartWidth     = 304.0f;
    spec.chartHeight    = 219.0f;
    spec.axis           = AxisPreset::WEEK_PERIOD;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "axis-02: Explicit BAR / CHART; 7 points";
    spec.secondaryChart = false;
    spec.interactive    = true;
    spec.frames         = {{190.5f, 128.5f, 304.0f, 219.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"S", 0.0f}, {"M", 1.0f}, {"T", 2.0f}, {"W", 3.0f}, {"T", 4.0f}, {"F", 5.0f}, {"S", 6.0f}}, true};
    spec.yAxis          = {{{"0", 0.0f}, {"5", 5.0f}, {"10", 10.0f}, {"15", 15.0f}, {"20", 20.0f}, {"25", 25.0f}, {"30", 30.0f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "axis-03";
    spec.category       = "Axis";
    spec.title          = "DataSimple";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 25.0f;
    spec.values         = {18.911523f, 7.1077675f, 13.050926f, 4.8891461f, 9.659465f, 16.621142f, 20.588992f};
    spec.secondary      = {14.53f, 9.95f, 3.28f, 15.87f, 11.11f, 7.72f, 18.58f};
    spec.simple         = true;
    spec.chartWidth     = 304.0f;
    spec.chartHeight    = 219.0f;
    spec.axis           = AxisPreset::WEEK_SIMPLE;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "axis-03: Explicit BAR / CHART; 7 points";
    spec.secondaryChart = false;
    spec.interactive    = true;
    spec.frames         = {{190.5f, 128.5f, 304.0f, 219.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"S", 0.0f}, {"M", 1.0f}, {"T", 2.0f}, {"W", 3.0f}, {"T", 4.0f}, {"F", 5.0f}, {"S", 6.0f}}, true};
    spec.yAxis          = {{{"0", 0.0f}, {"5", 5.0f}, {"10", 10.0f}, {"15", 15.0f}, {"20", 20.0f}, {"25", 25.0f}, {"30", 30.0f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "axis-04";
    spec.category       = "Axis";
    spec.title          = "CategoryBasicUnit";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 30.0f;
    spec.values         = {25.266718f, 6.5802469f, 16.01106f, 3.9295267f, 22.928241f, 1.1553498f, 15.647891f, 15.665123f, 16.021862f, 10.421811f, 24.790895f, 10.610597f, 28.251286f, 0.30555556f, 24.921554f, 19.136317f, 12.209105f, 8.9547325f, 23.076903f, 10.501543f, 2.7101337f, 20.073045f, 5.1828704f, 16.187757f};
    spec.secondary      = {14.53f, 11.72f, 16.63f, 16.6f, 4.6f, 2.8f, 7.49f, 7.9f, 18.0f, 15.17f, 5.51f, 13.88f, 1.8f, 11.58f, 9.84f, 0.14f, 10.12f, 18.53f, 1.25f, 8.66f, 6.19f, 18.66f, 6.97f, 11.23f};
    spec.simple         = true;
    spec.chartWidth     = 304.0f;
    spec.chartHeight    = 219.0f;
    spec.axis           = AxisPreset::TIME_UNIT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "axis-04: Explicit BAR / CHART; 24 points";
    spec.secondaryChart = false;
    spec.interactive    = true;
    spec.frames         = {{190.5f, 128.5f, 304.0f, 219.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"6PM", 0.0f}, {"12AM\n2026", 6.0f}, {"1AM", 12.0f}, {"2AM", 18.0f}}, true};
    spec.yAxis          = {{{"0", 0.0f}, {"5", 5.0f}, {"10", 10.0f}, {"15", 15.0f}, {"20", 20.0f}, {"25", 25.0f}, {"30", 30.0f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "axis-05";
    spec.category       = "Axis";
    spec.title          = "CategoryBasicPeriod";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 35.0f;
    spec.values         = {20.124743f, 6.2530864f, 29.387603f, 3.2319959f, 23.712191f, 22.309671f, 9.3948045f, 7.5601852f, 4.9539609f, 18.613169f, 27.797068f, 17.320473f, 10.887088f, 4.4228395f, 9.4092078f, 26.216564f, 7.4375f, 21.220165f, 14.601595f, 16.841049f, 2.7533436f, 16.042181f, 12.633488f, 24.008745f};
    spec.secondary      = {14.54f, 13.49f, 10.48f, 17.32f, 17.6f, 17.38f, 15.89f, 6.26f, 8.42f, 16.29f, 14.36f, 7.73f, 1.35f, 10.64f, 14.3f, 9.23f, 13.11f, 18.83f, 7.07f, 6.45f, 0.17f, 10.13f, 0.54f, 12.63f};
    spec.simple         = true;
    spec.chartWidth     = 304.0f;
    spec.chartHeight    = 219.0f;
    spec.axis           = AxisPreset::TIME_PERIOD;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "axis-05: Explicit BAR / CHART; 24 points";
    spec.secondaryChart = false;
    spec.interactive    = true;
    spec.frames         = {{190.5f, 128.5f, 304.0f, 219.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"12AM", 0.0f}, {"1/12", 6.0f}, {"12PM", 12.0f}, {"6PM", 18.0f}, {"12AM\n1/1", 23.0f}}, true};
    spec.yAxis          = {{{"0", 0.0f}, {"5", 5.0f}, {"10", 10.0f}, {"15", 15.0f}, {"20", 20.0f}, {"25", 25.0f}, {"30", 30.0f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "axis-06";
    spec.category       = "Axis";
    spec.title          = "CategorySimpleCenter";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 20.0f;
    spec.values         = {10.538323f, 4.8148148f, 4.9863683f, 8.0900206f, 3.3850309f, 15.686214f, 18.697274f};
    spec.secondary      = {14.55f, 15.26f, 4.34f, 18.04f, 11.09f, 12.46f, 4.8f};
    spec.simple         = true;
    spec.chartWidth     = 304.0f;
    spec.chartHeight    = 219.0f;
    spec.axis           = AxisPreset::CENTER;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "axis-06: Explicit BAR / CHART; 7 points";
    spec.secondaryChart = false;
    spec.interactive    = true;
    spec.frames         = {{190.5f, 128.5f, 304.0f, 219.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"This Week", 3.0f}}, true};
    spec.yAxis          = {{{"0", 0.0f}, {"5", 5.0f}, {"10", 10.0f}, {"15", 15.0f}, {"20", 20.0f}, {"25", 25.0f}, {"30", 30.0f}, {"35", 35.0f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "axis-07";
    spec.category       = "Axis";
    spec.title          = "CategorySimpleEdge";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 35.0f;
    spec.values         = {18.344136f, 6.1082819f, 22.360082f, 29.69213f, 16.252572f, 29.078446f, 17.429012f, 26.119084f, 18.852366f, 21.554784f, 15.707819f, 1.6818416f, 22.069444f, 25.018776f, 17.566872f, 28.972994f, 4.0519547f, 16.507459f, 22.265432f, 22.807356f, 8.5036008f, 11.946759f, 1.2849794f, 3.5552984f};
    spec.secondary      = {14.56f, 17.03f, 17.69f, 18.76f, 4.59f, 7.54f, 13.2f, 2.99f, 8.76f, 18.53f, 12.55f, 14.94f, 0.45f, 8.76f, 3.7f, 7.9f, 19.08f, 19.42f, 18.72f, 2.04f, 7.62f, 12.58f, 7.18f, 15.43f};
    spec.simple         = true;
    spec.chartWidth     = 304.0f;
    spec.chartHeight    = 219.0f;
    spec.axis           = AxisPreset::EDGE;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "axis-07: Explicit BAR / CHART; 24 points";
    spec.secondaryChart = false;
    spec.interactive    = true;
    spec.frames         = {{190.5f, 128.5f, 304.0f, 219.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"Start", 0.0f}, {"End", 23.0f}}, true};
    spec.yAxis          = {{{"0", 0.0f}, {"5", 5.0f}, {"10", 10.0f}, {"15", 15.0f}, {"20", 20.0f}, {"25", 25.0f}, {"30", 30.0f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "unit-01";
    spec.category       = "Unit";
    spec.title          = "Unit Label with Bar Chart";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 120.0f;
    spec.values         = {95.792181f, 42.043038f, 5.4749657f, 94.729938f, 3.6351166f, 22.313957f, 14.963992f};
    spec.secondary      = {48.55f, 62.66f, 38.47f, 64.94f, 58.62f, 8.75f, 7.03f};
    spec.unit           = "kWh";
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "unit-01: Explicit BAR / CHART; 7 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.color          = 12966143u;
    spec.alpha          = 1.0f;
    spec.groupWidth     = 0.6f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}}, false};
    spec.yAxis          = {{{"20", 20.0f}, {"40", 40.0f}, {"60", 60.0f}, {"80", 80.0f}, {"100", 100.0f}}, true};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "unit-02";
    spec.legendItems    = {{"Green", 29.0f, 168.0f, 43.421875f, 5025616u, false}, {"Blue", 80.421875f, 168.0f, 34.5f, 2201331u, false}, {"Yellow", 122.921875f, 168.0f, 44.921875f, 16761095u, false}};
    spec.category       = "Unit";
    spec.title          = "Unit Label with Legend";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 120.0f;
    spec.values         = {64.301269f, 82.671468f, 89.827675f, 57.374829f, 19.88083f, 52.654321f, 16.189129f};
    spec.secondary      = {48.57f, 3.55f, 17.97f, 2.35f, 36.94f, 57.35f, 35.05f};
    spec.legend         = true;
    spec.unit           = "kWh";
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "unit-02: Explicit BAR / CHART; 7 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 170.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.color          = 12966143u;
    spec.alpha          = 1.0f;
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}}, false};
    spec.yAxis          = {{{"", 0.0f}, {"30", 30.0f}, {"60", 60.0f}, {"90", 90.0f}, {"120", 120.0f}}, true};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "unit-03";
    spec.category       = "Unit";
    spec.title          = "Unit Label with Line Chart";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 10.0f;
    spec.values         = {5.0f, 6.0f, 7.0f, 4.0f, 2.0f, 3.0f, 6.0f, 5.0f, 7.0f};
    spec.secondary      = {3.4f, 0.66f, 4.37f, 0.33f, 1.07f, 2.87f, 4.41f, 4.1f, 4.44f};
    spec.unit           = "kWh";
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "unit-03: Explicit BAR / CHART; 9 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, false};
    spec.yAxis          = {{{"", 0.0f}, {"2.5", 2.5f}, {"5", 5.0f}, {"7.5", 7.5f}, {"10", 10.0f}}, true};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 4.0f, 0x387AFFu, 1.0f});
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id       = "scale-01";
    spec.category = "Scale";
    spec.title    = "Log Scale";
    spec.kind     = ChartKind::BAR;
    // A positive decade keeps the 0.02 fixture visible and permits viewport restoration.
    spec.minimum                 = 0.01f;
    spec.maximum                 = 10000.0f;
    spec.values                  = {266.73525f, 3316.5981f, 1617.4897f, 1786.6941f, 4811.8656f, 4273.251f, 5899.2455f, 4455.2812f, 5966.0494f, 7271.0562f, 4913.5117f, 1362.5514f, 568.79287f, 853.22359f, 1129.4239f, 4459.1221f, 2941.0837f, 7933.3333f, 5608.7106f, 5843.7586f, 440.9465f, 6684.225f, 4227.915f, 5318.93f, 2352.332f, 760.21948f, 1233.9506f, 3279.6982f, 6107.3388f, 6852.6749f, 0.0f, 0.02f, 9000.0f};
    spec.secondary               = {3.89f, 1.23f, 3.36f, 0.57f, 4.69f, 1.96f, 2.09f, 4.25f, 2.52f, 0.94f, 2.38f, 2.63f, 4.84f, 1.33f, 0.54f, 1.4f, 3.07f, 0.3f, 0.8f, 3.39f, 0.81f, 4.66f, 0.25f, 0.41f, 4.53f, 4.98f, 0.44f, 4.41f, 1.77f, 1.13f};
    spec.subtitle                = "Subtitle";
    spec.description             = "2월 6일 오후 5시 - 6시";
    spec.hint                    = "Hint text · here";
    spec.unit                    = "kWh";
    spec.logarithmic             = true;
    spec.axis                    = AxisPreset::DEFAULT;
    spec.layout                  = LayoutPreset::REPORT;
    spec.action                  = Action::DATA;
    spec.navigation              = Navigation::NONE;
    spec.numberMode              = NumberMode::NONE;
    spec.expected                = "scale-01: Explicit BAR / REPORT; 30 points";
    spec.secondaryChart          = false;
    spec.chartWidth              = 643.0f;
    spec.chartHeight             = 200.0f;
    spec.frames                  = {{21.0f, 173.5f, 643.0f, 200.0f}};
    spec.referenceWidth          = 685.0f;
    spec.texts                   = {{"Subtitle", 21.0f, 102.5f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"100 Unit", 21.0f, 121.5f, 82.28125f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"2026년 8월 20일 - 2025년 12월 20일", 21.0f, 157.5f, 643.0f, 16.0f, 11.0f, 8684679u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Hint text • here", 107.28125f, 125.0f, 91.9375f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}};
    spec.interactive             = true;
    spec.groupWidth              = 0.5f;
    spec.cornerRadius            = 10000.0f;
    spec.xAxis                   = {{{"0", 0.0f}, {"4", 4.0f}, {"8", 8.0f}, {"12", 12.0f}, {"16", 16.0f}, {"20", 20.0f}, {"24", 24.0f}, {"28", 28.0f}, {"32", 32.0f}}, true};
    spec.yAxis                   = {{{"", 0.0f}, {"0.03", 0.03f}, {"3", 3.0f}, {"300", 300.0f}, {"10000", 10000.0f}}, true};
    spec.texts[3].color          = 0xB88400u;
    spec.texts[3].background     = 0xFFEAB0u;
    spec.texts[3].darkColor      = 0xCC9300u;
    spec.texts[3].darkBackground = 0x594000u;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id                      = "scale-02";
    spec.category                = "Scale";
    spec.title                   = "Linear Scale";
    spec.kind                    = ChartKind::BAR;
    spec.minimum                 = 0.0f;
    spec.maximum                 = 120.0f;
    spec.values                  = {51.66238f, 86.699246f, 10.522119f, 94.73594f, 73.908608f, 23.348765f, 3.5502401f, 71.303155f, 57.471708f, 11.438615f, 12.216221f, 46.22428f, 51.734396f, 29.981139f, 22.939815f, 57.770919f, 57.931241f, 20.951646f, 62.881516f, 29.399863f, 73.593107f, 0.39951989f, 77.720336f, 14.197531f, 3.6582647f, 36.225995f, 76.098251f, 5.9910837f, 38.250171f, 92.628601f};
    spec.secondary               = {48.65f, 21.24f, 21.49f, 9.57f, 36.91f, 8.15f, 54.1f, 47.73f, 64.55f, 15.44f, 59.3f, 12.37f, 58.97f, 13.53f, 21.54f, 47.82f, 48.38f, 4.69f, 29.43f, 35.0f, 55.0f, 29.86f, 46.75f, 9.73f, 64.22f, 25.96f, 44.47f, 39.95f, 9.82f, 54.21f};
    spec.subtitle                = "Subtitle";
    spec.description             = "2월 6일 오후 5시 - 6시";
    spec.hint                    = "Hint text · here";
    spec.unit                    = "kWh";
    spec.axis                    = AxisPreset::DEFAULT;
    spec.layout                  = LayoutPreset::REPORT;
    spec.action                  = Action::DATA;
    spec.navigation              = Navigation::NONE;
    spec.numberMode              = NumberMode::NONE;
    spec.expected                = "scale-02: Explicit BAR / REPORT; 30 points";
    spec.secondaryChart          = false;
    spec.chartWidth              = 643.0f;
    spec.chartHeight             = 200.0f;
    spec.frames                  = {{21.0f, 173.5f, 643.0f, 200.0f}};
    spec.referenceWidth          = 685.0f;
    spec.texts                   = {{"Subtitle", 21.0f, 102.5f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"100 Unit", 21.0f, 121.5f, 82.28125f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"2026년 8월 20일 - 2025년 12월 20일", 21.0f, 157.5f, 643.0f, 16.0f, 11.0f, 8684679u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Hint text • here", 107.28125f, 125.0f, 91.9375f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}};
    spec.interactive             = true;
    spec.groupWidth              = 0.5f;
    spec.cornerRadius            = 10000.0f;
    spec.xAxis                   = {{{"0", 0.0f}, {"3", 3.0f}, {"6", 6.0f}, {"9", 9.0f}, {"12", 12.0f}, {"15", 15.0f}, {"18", 18.0f}, {"21", 21.0f}, {"24", 24.0f}, {"27", 27.0f}}, true};
    spec.yAxis                   = {{{"", 0.0f}, {"30", 30.0f}, {"60", 60.0f}, {"90", 90.0f}, {"120", 120.0f}}, true};
    spec.texts[3].color          = 0xB88400u;
    spec.texts[3].background     = 0xFFEAB0u;
    spec.texts[3].darkColor      = 0xCC9300u;
    spec.texts[3].darkBackground = 0x594000u;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id                      = "scale-03";
    spec.category                = "Scale";
    spec.title                   = "Diverging Scale";
    spec.kind                    = ChartKind::BAR;
    spec.minimum                 = -120.0f;
    spec.maximum                 = 120.0f;
    spec.values                  = {-18.53738f, -65.747599f, 94.243827f, -17.575446f, -87.625171f, -54.176955f, -18.465364f, -22.465706f, -93.338477f, 45.459534f, -3.6025377f, 43.425926f, 40.865912f, 35.631001f, 22.782922f, 34.420439f, -38.098422f, -88.600823f, 92.789781f, 41.875857f, -24.058642f, 82.640604f, -57.779492f, 83.076132f, 70.639575f, -70.397805f, 99.470165f, -76.546639f, 70.687586f, 91.790123f};
    spec.secondary               = {48.67f, 27.14f, 1.0f, 11.97f, 15.23f, 56.75f, 17.12f, 42.28f, 32.61f, 19.17f, 23.79f, 56.88f, 57.47f, 10.4f, 36.39f, 13.12f, 58.34f, 5.67f, 48.84f, 27.64f, 34.92f, 1.45f, 25.31f, 14.39f, 6.85f, 54.67f, 18.42f, 24.81f, 62.46f, 29.34f};
    spec.subtitle                = "Subtitle";
    spec.description             = "2월 6일 오후 5시 - 6시";
    spec.hint                    = "Hint text · here";
    spec.unit                    = "kWh";
    spec.axis                    = AxisPreset::DEFAULT;
    spec.layout                  = LayoutPreset::REPORT;
    spec.action                  = Action::DATA;
    spec.navigation              = Navigation::NONE;
    spec.numberMode              = NumberMode::NONE;
    spec.expected                = "scale-03: Explicit BAR / REPORT; 30 points";
    spec.secondaryChart          = false;
    spec.chartWidth              = 643.0f;
    spec.chartHeight             = 200.0f;
    spec.frames                  = {{21.0f, 173.5f, 643.0f, 200.0f}};
    spec.referenceWidth          = 685.0f;
    spec.texts                   = {{"Subtitle", 21.0f, 102.5f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"100 Unit", 21.0f, 121.5f, 82.28125f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"2026년 8월 20일 - 2025년 12월 20일", 21.0f, 157.5f, 643.0f, 16.0f, 11.0f, 8684679u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Hint text • here", 107.28125f, 125.0f, 91.9375f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}};
    spec.interactive             = true;
    spec.groupWidth              = 0.5f;
    spec.cornerRadius            = 10000.0f;
    spec.xAxis                   = {{{"0", 0.0f}, {"3", 3.0f}, {"6", 6.0f}, {"9", 9.0f}, {"12", 12.0f}, {"15", 15.0f}, {"18", 18.0f}, {"21", 21.0f}, {"24", 24.0f}, {"27", 27.0f}}, true};
    spec.yAxis                   = {{{"-120", -120.0f}, {"-60", -60.0f}, {"0", 0.0f}, {"60", 60.0f}, {"120", 120.0f}}, true};
    spec.texts[3].color          = 0xB88400u;
    spec.texts[3].background     = 0xFFEAB0u;
    spec.texts[3].darkColor      = 0xCC9300u;
    spec.texts[3].darkBackground = 0x594000u;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "annotation-01";
    spec.category       = "Annotation";
    spec.title          = "Marker Annotation - Basic (data-index)";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 10.0f;
    spec.values         = {5.0f, 6.0f, 7.0f, 4.0f, 2.0f, 3.0f, 6.0f, 5.0f, 7.0f};
    spec.secondary      = {3.41f, 2.31f, 3.19f, 1.01f, 4.1f, 2.82f, 3.16f, 2.58f, 0.05f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "annotation-01: Explicit LINE / CHART; 9 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"2.5", 2.5f}, {"5", 5.0f}, {"7.5", 7.5f}, {"10", 10.0f}}, true};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 4.0f, 0x387AFFu, 1.0f});
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::MARKER;
      annotation.x        = 0.0f;
      annotation.y        = 0.0f;
      annotation.maximumY = 0.0f;
      annotation.point    = 2;
      annotation.text     = "";
      annotation.color    = 0x387AFFu;
      annotation.width    = 0.0f;
      annotation.ellipsis = false;
      annotation.offsetX  = 0.0f;
      annotation.offsetY  = 0.0f;
      spec.annotations.push_back(annotation);
    }
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::MARKER;
      annotation.x        = 0.0f;
      annotation.y        = 0.0f;
      annotation.maximumY = 0.0f;
      annotation.point    = 5;
      annotation.text     = "";
      annotation.color    = 0x387AFFu;
      annotation.width    = 0.0f;
      annotation.ellipsis = false;
      annotation.offsetX  = 0.0f;
      annotation.offsetY  = 0.0f;
      spec.annotations.push_back(annotation);
    }
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::MARKER;
      annotation.x        = 0.0f;
      annotation.y        = 0.0f;
      annotation.maximumY = 0.0f;
      annotation.point    = 7;
      annotation.text     = "";
      annotation.color    = 0x387AFFu;
      annotation.width    = 0.0f;
      annotation.ellipsis = false;
      annotation.offsetX  = 0.0f;
      annotation.offsetY  = 0.0f;
      spec.annotations.push_back(annotation);
    }
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "annotation-02";
    spec.category       = "Annotation";
    spec.title          = "Marker Annotation - Custom Styling";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 10.0f;
    spec.values         = {5.0f, 6.0f, 7.0f, 4.0f, 2.0f, 3.0f, 6.0f, 5.0f, 7.0f};
    spec.secondary      = {3.41f, 2.73f, 1.75f, 1.17f, 2.58f, 1.68f, 0.57f, 2.2f, 2.36f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "annotation-02: Explicit LINE / CHART; 9 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"2.5", 2.5f}, {"5", 5.0f}, {"7.5", 7.5f}, {"10", 10.0f}}, true};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 4.0f, 0x387AFFu, 1.0f});
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::MARKER;
      annotation.x        = 0.0f;
      annotation.y        = 0.0f;
      annotation.maximumY = 0.0f;
      annotation.point    = 2;
      annotation.text     = "";
      annotation.color    = 0xFF0000u;
      annotation.width    = 0.0f;
      annotation.ellipsis = false;
      annotation.offsetX  = 0.0f;
      annotation.offsetY  = 0.0f;
      spec.annotations.push_back(annotation);
    }
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::MARKER;
      annotation.x        = 0.0f;
      annotation.y        = 0.0f;
      annotation.maximumY = 0.0f;
      annotation.point    = 5;
      annotation.text     = "";
      annotation.color    = 0x00FF00u;
      annotation.width    = 0.0f;
      annotation.ellipsis = false;
      annotation.offsetX  = 0.0f;
      annotation.offsetY  = 0.0f;
      spec.annotations.push_back(annotation);
    }
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::MARKER;
      annotation.x        = 0.0f;
      annotation.y        = 0.0f;
      annotation.maximumY = 0.0f;
      annotation.point    = 7;
      annotation.text     = "";
      annotation.color    = 0x0000FFu;
      annotation.width    = 0.0f;
      annotation.ellipsis = false;
      annotation.offsetX  = 0.0f;
      annotation.offsetY  = 0.0f;
      spec.annotations.push_back(annotation);
    }
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "annotation-03";
    spec.category       = "Annotation";
    spec.title          = "Marker Annotation - xValue/yValue Position";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 8.0f;
    spec.values         = {5.0f, 6.0f, 7.0f, 4.0f, 2.0f, 3.0f, 6.0f, 5.0f, 7.0f};
    spec.secondary      = {3.41f, 3.14f, 0.32f, 1.34f, 1.06f, 0.53f, 2.53f, 1.82f, 0.13f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "annotation-03: Explicit LINE / CHART; 9 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::MARKER;
      annotation.x        = 2.0f;
      annotation.y        = 7.0f;
      annotation.maximumY = 7.0f;
      annotation.point    = -1;
      annotation.text     = "";
      annotation.color    = 0xFF5733u;
      annotation.width    = 0.0f;
      annotation.ellipsis = false;
      annotation.offsetX  = 0.0f;
      annotation.offsetY  = 0.0f;
      spec.annotations.push_back(annotation);
    }
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::MARKER;
      annotation.x        = 5.0f;
      annotation.y        = 3.0f;
      annotation.maximumY = 3.0f;
      annotation.point    = -1;
      annotation.text     = "";
      annotation.color    = 0x33FF57u;
      annotation.width    = 0.0f;
      annotation.ellipsis = false;
      annotation.offsetX  = 0.0f;
      annotation.offsetY  = 0.0f;
      spec.annotations.push_back(annotation);
    }
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::MARKER;
      annotation.x        = 7.0f;
      annotation.y        = 5.0f;
      annotation.maximumY = 5.0f;
      annotation.point    = -1;
      annotation.text     = "";
      annotation.color    = 0x3357FFu;
      annotation.width    = 0.0f;
      annotation.ellipsis = false;
      annotation.offsetX  = 0.0f;
      annotation.offsetY  = 0.0f;
      spec.annotations.push_back(annotation);
    }
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "annotation-04";
    spec.category       = "Annotation";
    spec.title          = "Marker Annotation - With Adjustments";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 10.0f;
    spec.values         = {5.0f, 6.0f, 7.0f, 4.0f, 2.0f, 3.0f, 6.0f, 5.0f, 7.0f};
    spec.secondary      = {3.41f, 3.55f, 3.43f, 1.51f, 4.1f, 3.93f, 4.49f, 1.43f, 2.44f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "annotation-04: Explicit LINE / CHART; 9 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"2.5", 2.5f}, {"5", 5.0f}, {"7.5", 7.5f}, {"10", 10.0f}}, true};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 4.0f, 0x387AFFu, 1.0f});
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::MARKER;
      annotation.x        = 0.0f;
      annotation.y        = 0.0f;
      annotation.maximumY = 0.0f;
      annotation.point    = 3;
      annotation.text     = "";
      annotation.color    = 0xFF9900u;
      annotation.width    = 0.0f;
      annotation.ellipsis = false;
      annotation.offsetX  = 0.0f;
      annotation.offsetY  = -10.0f;
      spec.annotations.push_back(annotation);
    }
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::MARKER;
      annotation.x        = 0.0f;
      annotation.y        = 0.0f;
      annotation.maximumY = 0.0f;
      annotation.point    = 4;
      annotation.text     = "";
      annotation.color    = 0x9900FFu;
      annotation.width    = 0.0f;
      annotation.ellipsis = false;
      annotation.offsetX  = 15.0f;
      annotation.offsetY  = 0.0f;
      spec.annotations.push_back(annotation);
    }
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "annotation-05";
    spec.category       = "Annotation";
    spec.title          = "Line Annotation - Average Preset (default rendering)";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 10.0f;
    spec.values         = {5.0f, 6.0f, 7.0f, 4.0f, 2.0f, 3.0f, 6.0f, 5.0f, 7.0f};
    spec.secondary      = {3.42f, 3.96f, 2.0f, 1.68f, 2.58f, 2.78f, 1.9f, 1.05f, 0.21f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "annotation-05: Explicit LINE / CHART; 9 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, true};
    spec.yAxis          = {{{"0", 0.0f}, {"10", 10.0f}}, true};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 4.0f, 0x387AFFu, 1.0f});
    {
      AnnotationSpec annotation;
      annotation.kind      = AnnotationKind::LINE;
      annotation.x         = 8.0f;
      annotation.y         = 5.0f;
      annotation.maximumY  = 5.0f;
      annotation.point     = -1;
      annotation.text      = "Avg.";
      annotation.color     = 0x387AFFu;
      annotation.width     = 0.0f;
      annotation.ellipsis  = false;
      annotation.offsetX   = 0.0f;
      annotation.offsetY   = 0.0f;
      annotation.lineWidth = 1.5f;
      spec.annotations.push_back(annotation);
    }
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "annotation-06";
    spec.category       = "Annotation";
    spec.title          = "Line Annotation - Reference Preset";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 10.0f;
    spec.values         = {5.0f, 6.0f, 7.0f, 4.0f, 2.0f, 3.0f, 6.0f, 5.0f, 7.0f};
    spec.secondary      = {3.42f, 4.38f, 0.56f, 1.85f, 1.06f, 1.64f, 3.87f, 0.67f, 2.52f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "annotation-06: Explicit LINE / CHART; 9 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, true};
    spec.yAxis          = {{{"0", 0.0f}, {"10", 10.0f}}, true};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 4.0f, 0x387AFFu, 1.0f});
    {
      AnnotationSpec annotation;
      annotation.kind      = AnnotationKind::LINE;
      annotation.x         = 8.0f;
      annotation.y         = 3.0f;
      annotation.maximumY  = 3.0f;
      annotation.point     = -1;
      annotation.text      = "Target";
      annotation.color     = 0x387AFFu;
      annotation.width     = 0.0f;
      annotation.ellipsis  = false;
      annotation.offsetX   = 0.0f;
      annotation.offsetY   = 0.0f;
      annotation.lineWidth = 1.5f;
      spec.annotations.push_back(annotation);
    }
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "annotation-07";
    spec.category       = "Annotation";
    spec.title          = "Line Annotation - Custom Overrides";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 10.0f;
    spec.values         = {5.0f, 6.0f, 7.0f, 4.0f, 2.0f, 3.0f, 6.0f, 5.0f, 7.0f};
    spec.secondary      = {3.42f, 0.24f, 3.68f, 2.02f, 4.09f, 0.49f, 1.28f, 0.29f, 0.29f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "annotation-07: Explicit LINE / CHART; 9 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, true};
    spec.yAxis          = {{{"0", 0.0f}, {"10", 10.0f}}, true};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 4.0f, 0x387AFFu, 1.0f});
    {
      AnnotationSpec annotation;
      annotation.kind      = AnnotationKind::LINE;
      annotation.x         = 8.0f;
      annotation.y         = 5.0f;
      annotation.maximumY  = 5.0f;
      annotation.point     = -1;
      annotation.text      = "Avg.";
      annotation.color     = 0xFF0000u;
      annotation.width     = 0.0f;
      annotation.ellipsis  = false;
      annotation.offsetX   = 0.0f;
      annotation.offsetY   = 0.0f;
      annotation.lineWidth = 2.0f;
      spec.annotations.push_back(annotation);
    }
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "annotation-08";
    spec.category       = "Annotation";
    spec.title          = "Line Annotation - Custom Label with Ellipsis Overflow";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 10.0f;
    spec.values         = {5.0f, 6.0f, 7.0f, 4.0f, 2.0f, 3.0f, 6.0f, 5.0f, 7.0f};
    spec.secondary      = {3.42f, 0.65f, 2.24f, 2.19f, 2.58f, 3.89f, 3.24f, 4.46f, 2.6f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "annotation-08: Explicit LINE / CHART; 9 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, true};
    spec.yAxis          = {{{"0", 0.0f}, {"10", 10.0f}}, true};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 4.0f, 0x387AFFu, 1.0f});
    {
      AnnotationSpec annotation;
      annotation.kind      = AnnotationKind::LINE;
      annotation.x         = 8.0f;
      annotation.y         = 3.5f;
      annotation.maximumY  = 3.5f;
      annotation.point     = -1;
      annotation.text      = "This is a very long label that will be truncated";
      annotation.color     = 0x387AFFu;
      annotation.width     = 110.0f;
      annotation.ellipsis  = true;
      annotation.offsetX   = 0.0f;
      annotation.offsetY   = 0.0f;
      annotation.lineWidth = 1.5f;
      spec.annotations.push_back(annotation);
    }
    {
      AnnotationSpec annotation;
      annotation.kind      = AnnotationKind::LINE;
      annotation.x         = 8.0f;
      annotation.y         = 6.5f;
      annotation.maximumY  = 6.5f;
      annotation.point     = -1;
      annotation.text      = "Long custom text for average";
      annotation.color     = 0x2196F3u;
      annotation.width     = 110.0f;
      annotation.ellipsis  = true;
      annotation.offsetX   = 0.0f;
      annotation.offsetY   = 0.0f;
      annotation.lineWidth = 2.0f;
      spec.annotations.push_back(annotation);
    }
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "annotation-09";
    spec.category       = "Annotation";
    spec.title          = "Line Annotation - Edge Case (near Y-axis max/min)";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {26.723251f, 31.616084f, 38.875171f, 68.253601f, 91.35631f, 42.7512f, 97.746914f, 16.837277f, 56.812414f, 48.536523f};
    spec.secondary      = {48.9f, 15.2f, 11.55f, 33.63f, 15.11f, 39.16f, 9.28f, 58.23f, 5.21f, 52.78f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "annotation-09: Explicit LINE / CHART; 10 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}, {"9", 9.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"", 100.0f}}, true};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 4.0f, 0x387AFFu, 1.0f});
    {
      AnnotationSpec annotation;
      annotation.kind      = AnnotationKind::LINE;
      annotation.x         = 8.0f;
      annotation.y         = 97.0f;
      annotation.maximumY  = 97.0f;
      annotation.point     = -1;
      annotation.text      = "Avg.";
      annotation.color     = 0x387AFFu;
      annotation.width     = 0.0f;
      annotation.ellipsis  = false;
      annotation.offsetX   = 0.0f;
      annotation.offsetY   = 0.0f;
      annotation.lineWidth = 1.5f;
      spec.annotations.push_back(annotation);
    }
    {
      AnnotationSpec annotation;
      annotation.kind      = AnnotationKind::LINE;
      annotation.x         = 8.0f;
      annotation.y         = 3.0f;
      annotation.maximumY  = 3.0f;
      annotation.point     = -1;
      annotation.text      = "Target";
      annotation.color     = 0x387AFFu;
      annotation.width     = 0.0f;
      annotation.ellipsis  = false;
      annotation.offsetX   = 0.0f;
      annotation.offsetY   = 0.0f;
      annotation.lineWidth = 1.5f;
      spec.annotations.push_back(annotation);
    }
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "annotation-10";
    spec.category       = "Annotation";
    spec.title          = "Line Annotation - Y-axis Min/Max Exact Match";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {41.392318f, 14.392147f, 53.955761f, 98.354767f, 48.823731f, 47.337963f, 21.057956f, 93.440501f, 62.016461f, 42.835219f};
    spec.secondary      = {48.93f, 21.09f, 56.06f, 36.03f, 58.43f, 22.76f, 37.29f, 52.78f, 38.28f, 56.51f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "annotation-10: Explicit LINE / CHART; 10 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}, {"9", 9.0f}}, true};
    spec.yAxis          = {{}, true};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 4.0f, 0x387AFFu, 1.0f});
    {
      AnnotationSpec annotation;
      annotation.kind      = AnnotationKind::LINE;
      annotation.x         = 8.0f;
      annotation.y         = 100.0f;
      annotation.maximumY  = 100.0f;
      annotation.point     = -1;
      annotation.text      = "Avg.";
      annotation.color     = 0x387AFFu;
      annotation.width     = 0.0f;
      annotation.ellipsis  = false;
      annotation.offsetX   = 0.0f;
      annotation.offsetY   = 0.0f;
      annotation.lineWidth = 1.5f;
      spec.annotations.push_back(annotation);
    }
    {
      AnnotationSpec annotation;
      annotation.kind      = AnnotationKind::LINE;
      annotation.x         = 8.0f;
      annotation.y         = 0.0f;
      annotation.maximumY  = 0.0f;
      annotation.point     = -1;
      annotation.text      = "Avg.";
      annotation.color     = 0x387AFFu;
      annotation.width     = 0.0f;
      annotation.ellipsis  = false;
      annotation.offsetX   = 0.0f;
      annotation.offsetY   = 0.0f;
      annotation.lineWidth = 1.5f;
      spec.annotations.push_back(annotation);
    }
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "annotation-11";
    spec.category       = "Annotation";
    spec.title          = "Point Annotation - Text Preset (basic)";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 10.0f;
    spec.values         = {5.0f, 6.0f, 7.0f, 4.0f, 2.0f, 3.0f, 6.0f, 5.0f, 7.0f};
    spec.secondary      = {3.43f, 1.89f, 2.49f, 2.69f, 2.57f, 0.45f, 0.02f, 3.31f, 0.44f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "annotation-11: Explicit LINE / CHART; 9 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"2.5", 2.5f}, {"5", 5.0f}, {"7.5", 7.5f}, {"10", 10.0f}}, true};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 4.0f, 0x387AFFu, 1.0f});
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::TEXT;
      annotation.x        = 2.0f;
      annotation.y        = 7.0f;
      annotation.maximumY = 7.0f;
      annotation.point    = -1;
      annotation.text     = "Peak";
      annotation.color    = 0x387AFFu;
      annotation.width    = 0.0f;
      annotation.ellipsis = false;
      annotation.offsetX  = 0.0f;
      annotation.offsetY  = 0.0f;
      spec.annotations.push_back(annotation);
    }
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "annotation-12";
    spec.category       = "Annotation";
    spec.title          = "Point Annotation - Data Index";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 10.0f;
    spec.values         = {5.0f, 6.0f, 7.0f, 4.0f, 2.0f, 3.0f, 6.0f, 5.0f, 7.0f};
    spec.secondary      = {3.43f, 2.3f, 1.05f, 2.86f, 1.05f, 3.85f, 1.98f, 2.93f, 2.76f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "annotation-12: Explicit LINE / CHART; 9 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"2.5", 2.5f}, {"5", 5.0f}, {"7.5", 7.5f}, {"10", 10.0f}}, true};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 4.0f, 0x387AFFu, 1.0f});
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::TEXT;
      annotation.x        = 0.0f;
      annotation.y        = 0.0f;
      annotation.maximumY = 0.0f;
      annotation.point    = 2;
      annotation.text     = "Index";
      annotation.color    = 0x387AFFu;
      annotation.width    = 0.0f;
      annotation.ellipsis = false;
      annotation.offsetX  = 0.0f;
      annotation.offsetY  = 0.0f;
      spec.annotations.push_back(annotation);
    }
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "annotation-13";
    spec.category       = "Annotation";
    spec.title          = "Point Annotation - Custom Overrides";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 10.0f;
    spec.values         = {5.0f, 6.0f, 7.0f, 4.0f, 2.0f, 3.0f, 6.0f, 5.0f, 7.0f};
    spec.secondary      = {3.43f, 2.71f, 4.17f, 3.03f, 4.09f, 2.7f, 3.94f, 2.55f, 0.52f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "annotation-13: Explicit LINE / CHART; 9 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"2.5", 2.5f}, {"5", 5.0f}, {"7.5", 7.5f}, {"10", 10.0f}}, true};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 4.0f, 0x387AFFu, 1.0f});
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::TEXT;
      annotation.x        = 5.0f;
      annotation.y        = 3.0f;
      annotation.maximumY = 3.0f;
      annotation.point    = -1;
      annotation.text     = "Low Point";
      annotation.color    = 0x387AFFu;
      annotation.width    = 0.0f;
      annotation.ellipsis = false;
      annotation.offsetX  = 0.0f;
      annotation.offsetY  = 0.0f;
      spec.annotations.push_back(annotation);
    }
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "annotation-14";
    spec.category       = "Annotation";
    spec.title          = "Point Annotation - Overflow Ellipsis";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 10.0f;
    spec.values         = {5.0f, 6.0f, 7.0f, 4.0f, 2.0f, 3.0f, 6.0f, 5.0f, 7.0f};
    spec.secondary      = {3.43f, 3.13f, 2.74f, 3.2f, 2.57f, 1.55f, 1.36f, 2.17f, 2.84f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "annotation-14: Explicit LINE / CHART; 9 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"2.5", 2.5f}, {"5", 5.0f}, {"7.5", 7.5f}, {"10", 10.0f}}, true};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 4.0f, 0x387AFFu, 1.0f});
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::TEXT;
      annotation.x        = 3.0f;
      annotation.y        = 5.0f;
      annotation.maximumY = 5.0f;
      annotation.point    = -1;
      annotation.text     = "This is a very long label that will be truncated";
      annotation.color    = 0x387AFFu;
      annotation.width    = 110.0f;
      annotation.ellipsis = true;
      annotation.offsetX  = 0.0f;
      annotation.offsetY  = 0.0f;
      spec.annotations.push_back(annotation);
    }
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "annotation-15";
    spec.category       = "Annotation";
    spec.title          = "Point Annotation - Image";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 10.0f;
    spec.values         = {5.0f, 6.0f, 7.0f, 4.0f, 2.0f, 3.0f, 6.0f, 5.0f, 7.0f};
    spec.secondary      = {3.43f, 3.54f, 1.3f, 3.36f, 1.05f, 0.4f, 3.32f, 1.79f, 0.6f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "annotation-15: Explicit LINE / CHART; 9 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"2.5", 2.5f}, {"5", 5.0f}, {"7.5", 7.5f}, {"10", 10.0f}}, true};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 4.0f, 0x387AFFu, 1.0f});
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::IMAGE;
      annotation.x        = 6.0f;
      annotation.y        = 6.0f;
      annotation.maximumY = 6.0f;
      annotation.point    = -1;
      annotation.text     = "";
      annotation.color    = 0x387AFFu;
      annotation.width    = 0.0f;
      annotation.ellipsis = false;
      annotation.offsetX  = 0.0f;
      annotation.offsetY  = 0.0f;
      spec.annotations.push_back(annotation);
    }
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id        = "annotation-16";
    spec.category  = "Annotation";
    spec.title     = "Area Annotation - Solid";
    spec.kind      = ChartKind::LINE;
    spec.minimum   = 0.0f;
    spec.maximum   = 10.0f;
    spec.values    = {5.0f, 6.0f, 7.0f, 4.0f, 2.0f, 3.0f, 6.0f, 5.0f, 7.0f};
    spec.secondary = {3.44f, 3.95f, 4.42f, 3.53f, 4.08f, 3.81f, 0.73f, 1.41f, 2.92f};
    spec.fill      = true;
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::AREA;
      annotation.x        = 0.0f;
      annotation.y        = 2.0f;
      annotation.maximumY = 5.0f;
      annotation.point    = -1;
      annotation.text     = "";
      annotation.color    = 3701503u;
      annotation.pattern  = false;
      spec.annotations.push_back(annotation);
    }
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "annotation-16: Explicit BAR / CHART; 9 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"2.5", 2.5f}, {"5", 5.0f}, {"7.5", 7.5f}, {"10", 10.0f}}, true};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 4.0f, 0x387AFFu, 1.0f});
    spec.annotations[0].y            = 0.0f;
    spec.annotations[0].maximumY     = 3.0f;
    spec.annotations[0].cornerRadius = 4.0f;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id        = "annotation-17";
    spec.category  = "Annotation";
    spec.title     = "Area Annotation - Pattern Preset";
    spec.kind      = ChartKind::LINE;
    spec.minimum   = 0.0f;
    spec.maximum   = 10.0f;
    spec.values    = {5.0f, 6.0f, 7.0f, 4.0f, 2.0f, 3.0f, 6.0f, 5.0f, 7.0f};
    spec.secondary = {3.44f, 4.37f, 2.98f, 3.7f, 2.57f, 2.66f, 2.69f, 1.02f, 0.68f};
    spec.fill      = true;
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::AREA;
      annotation.x        = 0.0f;
      annotation.y        = 2.0f;
      annotation.maximumY = 5.0f;
      annotation.point    = -1;
      annotation.text     = "";
      annotation.color    = 3701503u;
      annotation.pattern  = true;
      spec.annotations.push_back(annotation);
    }
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "annotation-17: Explicit BAR / CHART; 9 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}, {"8", 8.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"2.5", 2.5f}, {"5", 5.0f}, {"7.5", 7.5f}, {"10", 10.0f}}, true};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 4.0f, 0x387AFFu, 1.0f});
    spec.annotations[0].y            = 0.0f;
    spec.annotations[0].maximumY     = 3.5f;
    spec.annotations[0].cornerRadius = 8.0f;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "legend-01";
    spec.legendItems    = {{"Green", 29.0f, 168.0f, 43.421875f, 5025616u, false}, {"Blue", 80.421875f, 168.0f, 34.5f, 2201331u, false}, {"Yellow", 122.921875f, 168.0f, 44.921875f, 16761095u, false}};
    spec.category       = "Legend";
    spec.title          = "Legend in Header slot";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 120.0f;
    spec.values         = {99.524177f, 84.972565f, 6.1205418f, 82.721193f, 86.379458f, 51.663237f, 53.881173f};
    spec.secondary      = {49.13f, 3.26f, 22.1f, 55.28f, 14.99f, 21.57f, 1.43f};
    spec.legend         = true;
    spec.legendHeader   = true;
    spec.legendImages   = false;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "legend-01: Explicit BAR / CHART; 7 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 170.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.color          = 12966143u;
    spec.alpha          = 1.0f;
    spec.groupWidth     = 0.6f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}}, false};
    spec.yAxis          = {{{"0", 0.0f}, {"20", 20.0f}, {"40", 40.0f}, {"60", 60.0f}, {"80", 80.0f}, {"100", 100.0f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "legend-02";
    spec.legendItems    = {{"Recommended range", 29.0f, 168.0f, 124.8125f, 7829367u, true}};
    spec.category       = "Legend";
    spec.title          = "Legend with Images";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {53.527092f, 7.391118f, 46.337449f, 19.7488f, 6.6375171f, 93.423354f, 18.377915f};
    spec.secondary      = {49.15f, 9.16f, 1.61f, 57.69f, 58.31f, 5.17f, 29.45f};
    spec.legend         = true;
    spec.legendHeader   = false;
    spec.legendImages   = true;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "legend-02: Explicit BAR / CHART; 7 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 170.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.color          = 12966143u;
    spec.alpha          = 1.0f;
    spec.groupWidth     = 0.6f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}}, false};
    spec.yAxis          = {{{"0", 0.0f}, {"20", 20.0f}, {"40", 40.0f}, {"60", 60.0f}, {"80", 80.0f}, {"100", 100.0f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "legend-03";
    spec.legendItems    = {{"Green", 29.0f, 318.0f, 43.421875f, 5025616u, false}, {"Blue", 80.421875f, 318.0f, 34.5f, 2201331u, false}, {"Yellow", 122.921875f, 318.0f, 44.921875f, 16761095u, false}};
    spec.category       = "Legend";
    spec.title          = "Legend in Footer Slot";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 120.0f;
    spec.values         = {82.735768f, 28.472222f, 82.747771f, 69.019204f, 84.817387f, 46.191701f, 98.821159f};
    spec.secondary      = {49.18f, 15.05f, 46.12f, 60.09f, 36.63f, 53.78f, 57.47f};
    spec.legend         = true;
    spec.legendHeader   = false;
    spec.legendImages   = false;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "legend-03: Explicit BAR / CHART; 7 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 156.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.color          = 12966143u;
    spec.alpha          = 1.0f;
    spec.groupWidth     = 0.6f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}}, false};
    spec.yAxis          = {{{"0", 0.0f}, {"20", 20.0f}, {"40", 40.0f}, {"60", 60.0f}, {"80", 80.0f}, {"100", 100.0f}, {"120", 120.0f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id          = "sleep-report-01";
    spec.legendItems = {{"권장 5lux 이하", 291.53125f, 394.0f, 83.421875f, 7829367u, true}};
    spec.category    = "Sleep Report";
    spec.title       = "Sleep Report Template - 1";
    spec.kind        = ChartKind::BAR;
    spec.minimum     = 0.0f;
    spec.maximum     = 100.0f;
    spec.values      = {41.160837f, 72.706447f, 58.284979f, 57.64952f, 62.405007f, 67.119342f, 47.101166f, 42.844307f, 71.138889f, 62.849108f, 67.357682f, 43.676955f, 78.22668f, 69.278464f, 78.066872f, 66.567215f, 61.939986f, 67.641975f, 41.204047f, 58.675583f, 65.735597f, 55.470508f, 72.818587f, 45.68107f};
    spec.secondary   = {49.08f, 43.87f, 44.73f, 51.54f, 42.76f, 46.9f, 43.78f, 50.68f, 42.22f, 46.02f, 50.73f, 43.07f, 44.78f, 41.78f, 44.26f, 51.88f, 41.37f, 44.88f, 40.26f, 40.57f, 40.6f, 50.1f, 45.57f, 48.73f};
    spec.subtitle    = "Subtitle";
    spec.description = "2월 6일 오후 5시 - 6시";
    spec.hint        = "권장 5lux 이하";
    spec.logarithmic = false;
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::AREA;
      annotation.x        = 0.0f;
      annotation.y        = 1.0f;
      annotation.maximumY = 5.0f;
      annotation.point    = -1;
      annotation.text     = "";
      annotation.color    = 3701503u;
      annotation.pattern  = false;
      spec.annotations.push_back(annotation);
    }
    spec.axis                        = AxisPreset::DEFAULT;
    spec.layout                      = LayoutPreset::REPORT;
    spec.action                      = Action::DATA;
    spec.navigation                  = Navigation::NONE;
    spec.numberMode                  = NumberMode::NONE;
    spec.expected                    = "sleep-report-01: Explicit BAR / REPORT; 24 points";
    spec.secondaryChart              = false;
    spec.chartWidth                  = 300.0f;
    spec.chartHeight                 = 233.0f;
    spec.frames                      = {{192.5f, 149.0f, 300.0f, 233.0f}};
    spec.referenceWidth              = 685.0f;
    spec.texts                       = {{"Subtitle", 192.5f, 78.0f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"120.5 Unit", 192.5f, 97.0f, 105.28125f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"2월 6일 오후 5시 - 6시", 192.5f, 133.0f, 300.0f, 16.0f, 11.0f, 8684679u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}};
    spec.interactive                 = true;
    spec.groupWidth                  = 0.5f;
    spec.cornerRadius                = 10000.0f;
    spec.pointColors                 = {13421774u, 13421774u, 13421774u, 13421774u, 13421774u, 13421774u, 13421774u, 13421774u, 13421774u, 13421774u, 13421774u, 16743462u, 3701503u, 3701503u, 3701503u, 3701503u, 3701503u, 3701503u, 16743462u, 3701503u, 13421774u, 13421774u, 13421774u, 13421774u};
    spec.pointThemes                 = true;
    spec.xAxis                       = {{{"6PM", 0.0f}, {"12AM", 6.0f}, {"1AM", 12.0f}, {"2AM", 18.0f}}, true};
    spec.yAxis                       = {{{"", 0.0f}, {"5", 20.0f}, {"22", 40.0f}, {"100", 60.0f}, {"447", 80.0f}, {"2000", 100.0f}}, true};
    spec.annotations[0].y            = 35.0f;
    spec.annotations[0].maximumY     = 55.0f;
    spec.annotations[0].minimumX     = 11.5f;
    spec.annotations[0].maximumX     = 19.5f;
    spec.annotations[0].hasXRange    = true;
    spec.annotations[0].cornerRadius = 4.0f;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id           = "sleep-report-02";
    spec.category     = "Sleep Report";
    spec.title        = "Sleep Report Template - 2";
    spec.kind         = ChartKind::LINE;
    spec.minimum      = 0.0f;
    spec.maximum      = 100.0f;
    spec.values       = {5.3998628f, 75.404664f, 41.623457f, 32.451303f, 13.320302f, 8.9218107f, 58.762003f, 66.050754f, 19.923868f, 30.998628f, 20.262689f, 21.296296f, 55.827846f, 62.622771f, 39.705761f, 79.916324f, 59.797668f, 21.81893f, 49.930727f, 38.454047f, 74.302469f, 72.537723f, 25.258573f, 63.823045f};
    spec.secondary    = {39.38f, 21.47f, 4.1f, 51.93f, 46.62f, 16.78f, 38.8f, 41.9f, 36.06f, 29.06f, 18.09f, 48.93f, 19.5f, 5.2f, 30.31f, 23.71f, 13.88f, 21.93f, 16.65f, 48.59f, 38.54f, 21.04f, 7.0f, 41.54f};
    spec.subtitle     = "Subtitle";
    spec.description  = "2월 6일 오후 5시 - 6시";
    spec.hint         = "Hint text · here";
    spec.logarithmic  = false;
    spec.segmentStyle = true;
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::AREA;
      annotation.x        = 0.0f;
      annotation.y        = 1.0f;
      annotation.maximumY = 5.0f;
      annotation.point    = -1;
      annotation.text     = "";
      annotation.color    = 3701503u;
      annotation.pattern  = false;
      spec.annotations.push_back(annotation);
    }
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::REPORT;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "sleep-report-02: Explicit LINE / REPORT; 24 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 304.0f;
    spec.chartHeight    = 233.0f;
    spec.frames         = {{190.5f, 157.0f, 304.0f, 233.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {{"Subtitle", 190.5f, 86.0f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"120.5 Unit", 190.5f, 105.0f, 105.28125f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"2월 6일 오후 5시 - 6시", 190.5f, 141.0f, 304.0f, 16.0f, 11.0f, 8684679u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}};
    spec.interactive    = true;
    spec.pointColors    = {13421774u, 13421774u, 13421774u, 13421774u, 13421774u, 13421774u, 13421774u, 13421774u, 13421774u, 13421774u, 13421774u, 13421774u, 16743462u, 16743462u, 16743462u, 16743462u, 16743462u, 3701503u, 16743462u, 16743462u, 13421774u, 13421774u, 13421774u, 13421774u};
    spec.pointThemes    = true;
    spec.xAxis          = {{{"6PM", 0.0f}, {"12AM", 6.0f}, {"1AM", 12.0f}, {"2AM", 18.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"5", 20.0f}, {"22", 40.0f}, {"100", 60.0f}, {"447", 80.0f}, {"2000", 100.0f}}, true};
    spec.lines.push_back({0xE9E9ECu, 1.0f, 2.5f, 2.5f, 0xCCCCCEu, 1.0f});
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id           = "sleep-report-03";
    spec.category     = "Sleep Report";
    spec.title        = "Sleep Report Template - 3";
    spec.kind         = ChartKind::LINE;
    spec.minimum      = 0.0f;
    spec.maximum      = 100.0f;
    spec.values       = {18.0f, 25.0f, 30.0f, 50.0f, 71.0f, 76.0f, 82.0f, 90.0f};
    spec.secondary    = {44.33f, 29.47f, 44.67f, 2.08f, 32.93f, 4.12f, 10.37f, 42.24f};
    spec.subtitle     = "Subtitle";
    spec.description  = "2월 6일 오후 5시 - 6시";
    spec.hint         = "Hint text · here";
    spec.metric       = "100";
    spec.unit         = "Unit";
    spec.slots        = 2;
    spec.chartWidth   = 132.0f;
    spec.chartHeight  = 96.0f;
    spec.segmentStyle = true;
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::AREA;
      annotation.x        = 0.0f;
      annotation.y        = 1.0f;
      annotation.maximumY = 5.0f;
      annotation.point    = -1;
      annotation.text     = "";
      annotation.color    = 3701503u;
      annotation.pattern  = false;
      spec.annotations.push_back(annotation);
    }
    spec.axis                    = AxisPreset::DEFAULT;
    spec.layout                  = LayoutPreset::SPARKLINE;
    spec.action                  = Action::DATA;
    spec.navigation              = Navigation::NONE;
    spec.numberMode              = NumberMode::NONE;
    spec.expected                = "sleep-report-03: Explicit LINE / SPARKLINE; 8 points";
    spec.secondaryChart          = false;
    spec.interactive             = false;
    spec.frames                  = {{385.5f, 190.0f, 132.0f, 96.0f}};
    spec.referenceWidth          = 685.0f;
    spec.texts                   = {{"Subtitle", 167.5f, 201.5f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"100 Unit", 167.5f, 220.5f, 82.28125f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"20 kWh", 255.78125f, 220.5f, 70.890625f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Hint text • here", 167.5f, 252.5f, 91.9375f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}};
    spec.pointColors             = {13421774u, 13421774u, 13421774u, 13421774u, 16743462u, 16743462u, 16743462u, 16743462u};
    spec.pointThemes             = true;
    spec.xAxis                   = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}}, true};
    spec.yAxis                   = {{{"", 0.0f}, {"", 25.0f}, {"", 50.0f}, {"", 75.0f}, {"", 100.0f}}, false};
    spec.texts[3].color          = 0xB88400u;
    spec.texts[3].background     = 0xFFEAB0u;
    spec.texts[3].darkColor      = 0xCC9300u;
    spec.texts[3].darkBackground = 0x594000u;
    spec.lines.push_back({0xFFA265u, 1.0f, 2.5f, 4.0f, 0xCCCCCEu, 1.0f});
    spec.annotations[0].y            = 5.0f;
    spec.annotations[0].maximumY     = 70.0f;
    spec.annotations[0].cornerRadius = 4.0f;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id           = "sleep-report-04";
    spec.category     = "Sleep Report";
    spec.title        = "Sleep Report Template - 4";
    spec.kind         = ChartKind::LINE;
    spec.minimum      = 0.0f;
    spec.maximum      = 120.0f;
    spec.values       = {67.201989f, 83.438957f, 68.523663f, 91.09808f, 84.989369f, 80.320988f, 61.290466f, 90.613855f};
    spec.secondary    = {63.79f, 62.97f, 62.24f, 60.36f, 61.15f, 64.09f, 63.04f, 63.19f};
    spec.subtitle     = "Subtitle";
    spec.description  = "2월 6일 오후 5시 - 6시";
    spec.hint         = "Hint text · here";
    spec.metric       = "100";
    spec.unit         = "Unit";
    spec.slots        = 2;
    spec.chartWidth   = 132.0f;
    spec.chartHeight  = 96.0f;
    spec.segmentStyle = true;
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::AREA;
      annotation.x        = 0.0f;
      annotation.y        = 1.0f;
      annotation.maximumY = 5.0f;
      annotation.point    = -1;
      annotation.text     = "";
      annotation.color    = 3701503u;
      annotation.pattern  = false;
      spec.annotations.push_back(annotation);
    }
    spec.axis                    = AxisPreset::DEFAULT;
    spec.layout                  = LayoutPreset::SPARKLINE;
    spec.action                  = Action::DATA;
    spec.navigation              = Navigation::NONE;
    spec.numberMode              = NumberMode::NONE;
    spec.expected                = "sleep-report-04: Explicit LINE / SPARKLINE; 8 points";
    spec.secondaryChart          = false;
    spec.interactive             = false;
    spec.frames                  = {{385.5f, 190.0f, 132.0f, 96.0f}};
    spec.referenceWidth          = 685.0f;
    spec.texts                   = {{"Subtitle", 167.5f, 201.5f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"100 Unit", 167.5f, 220.5f, 82.28125f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"20 kWh", 255.78125f, 220.5f, 70.890625f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Hint text • here", 167.5f, 252.5f, 91.9375f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}};
    spec.xAxis                   = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}}, true};
    spec.yAxis                   = {{{"", 0.0f}, {"", 30.0f}, {"", 60.0f}, {"", 90.0f}, {"", 120.0f}}, false};
    spec.texts[3].color          = 0xB88400u;
    spec.texts[3].background     = 0xFFEAB0u;
    spec.texts[3].darkColor      = 0xCC9300u;
    spec.texts[3].darkBackground = 0x594000u;
    spec.lines.push_back({0xFFA265u, 1.0f, 2.5f, 4.0f, 0xFF7C26u, 1.0f});
    spec.annotations[0].y            = 15.0f;
    spec.annotations[0].maximumY     = 50.0f;
    spec.annotations[0].cornerRadius = 4.0f;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id           = "sleep-report-05";
    spec.category     = "Sleep Report";
    spec.title        = "Sleep Report Template - 5";
    spec.kind         = ChartKind::LINE;
    spec.minimum      = 0.0f;
    spec.maximum      = 100.0f;
    spec.values       = {40.39802f, 46.945302f, 48.528592f, 41.752829f, 61.185914f, 57.136488f, 40.09838f, 41.861711f};
    spec.secondary    = {41.71f, 41.54f, 40.3f, 40.25f, 42.02f, 41.27f, 40.09f, 41.25f};
    spec.subtitle     = "Subtitle";
    spec.description  = "2월 6일 오후 5시 - 6시";
    spec.hint         = "Hint text · here";
    spec.metric       = "100";
    spec.unit         = "Unit";
    spec.slots        = 2;
    spec.chartWidth   = 132.0f;
    spec.chartHeight  = 96.0f;
    spec.segmentStyle = true;
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::AREA;
      annotation.x        = 0.0f;
      annotation.y        = 1.0f;
      annotation.maximumY = 5.0f;
      annotation.point    = -1;
      annotation.text     = "";
      annotation.color    = 3701503u;
      annotation.pattern  = false;
      spec.annotations.push_back(annotation);
    }
    spec.axis                    = AxisPreset::DEFAULT;
    spec.layout                  = LayoutPreset::SPARKLINE;
    spec.action                  = Action::DATA;
    spec.navigation              = Navigation::NONE;
    spec.numberMode              = NumberMode::NONE;
    spec.expected                = "sleep-report-05: Explicit LINE / SPARKLINE; 8 points";
    spec.secondaryChart          = false;
    spec.interactive             = false;
    spec.frames                  = {{385.5f, 190.0f, 132.0f, 96.0f}};
    spec.referenceWidth          = 685.0f;
    spec.texts                   = {{"Subtitle", 167.5f, 201.5f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"100 Unit", 167.5f, 220.5f, 82.28125f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"20 kWh", 255.78125f, 220.5f, 70.890625f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Hint text • here", 167.5f, 252.5f, 91.9375f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}};
    spec.xAxis                   = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}}, true};
    spec.yAxis                   = {{{"", 0.0f}, {"", 25.0f}, {"", 50.0f}, {"", 75.0f}, {"", 100.0f}}, false};
    spec.texts[3].color          = 0xB88400u;
    spec.texts[3].background     = 0xFFEAB0u;
    spec.texts[3].darkColor      = 0xCC9300u;
    spec.texts[3].darkBackground = 0x594000u;
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 4.0f, 0x387AFFu, 1.0f});
    spec.annotations[0].y            = 30.0f;
    spec.annotations[0].maximumY     = 75.0f;
    spec.annotations[0].cornerRadius = 4.0f;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id           = "sleep-report-06";
    spec.category     = "Sleep Report";
    spec.title        = "Sleep Report Template - 6";
    spec.kind         = ChartKind::BAR;
    spec.minimum      = 0.0f;
    spec.maximum      = 100.0f;
    spec.values       = {13.162723f, 15.072016f, 29.224108f, 1.298011f, 84.380144f, 83.408779f, 66.285151f, 41.651235f};
    spec.secondary    = {49.33f, 50.43f, 53.15f, 9.53f, 36.55f, 20.38f, 30.57f, 30.58f};
    spec.subtitle     = "Subtitle";
    spec.description  = "2월 6일 오후 5시 - 6시";
    spec.hint         = "Hint text · here";
    spec.metric       = "100";
    spec.unit         = "Unit";
    spec.slots        = 2;
    spec.chartWidth   = 132.0f;
    spec.chartHeight  = 96.0f;
    spec.segmentStyle = true;
    {
      AnnotationSpec annotation;
      annotation.kind     = AnnotationKind::AREA;
      annotation.x        = 0.0f;
      annotation.y        = 1.0f;
      annotation.maximumY = 5.0f;
      annotation.point    = -1;
      annotation.text     = "";
      annotation.color    = 3701503u;
      annotation.pattern  = false;
      spec.annotations.push_back(annotation);
    }
    spec.axis                        = AxisPreset::DEFAULT;
    spec.layout                      = LayoutPreset::SPARKLINE;
    spec.action                      = Action::DATA;
    spec.navigation                  = Navigation::NONE;
    spec.numberMode                  = NumberMode::NONE;
    spec.expected                    = "sleep-report-06: Explicit LINE / SPARKLINE; 8 points";
    spec.secondaryChart              = false;
    spec.interactive                 = false;
    spec.frames                      = {{385.5f, 190.0f, 132.0f, 96.0f}};
    spec.referenceWidth              = 685.0f;
    spec.texts                       = {{"Subtitle", 167.5f, 201.5f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"100 Unit", 167.5f, 220.5f, 82.28125f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"20 kWh", 255.78125f, 220.5f, 70.890625f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Hint text • here", 167.5f, 252.5f, 91.9375f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}};
    spec.groupWidth                  = 0.5f;
    spec.cornerRadius                = 10000.0f;
    spec.pointColors                 = {16743462u, 16743462u, 13421774u, 16743462u, 16743462u, 16743462u, 16743462u, 13421774u};
    spec.pointThemes                 = true;
    spec.xAxis                       = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}, {"7", 7.0f}}, true};
    spec.yAxis                       = {{{"0", 0.0f}, {"50", 50.0f}, {"100", 100.0f}}, false};
    spec.texts[3].color              = 0xB88400u;
    spec.texts[3].background         = 0xFFEAB0u;
    spec.texts[3].darkColor          = 0xCC9300u;
    spec.texts[3].darkBackground     = 0x594000u;
    spec.annotations[0].y            = 20.0f;
    spec.annotations[0].maximumY     = 50.0f;
    spec.annotations[0].cornerRadius = 4.0f;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "pattern-01";
    spec.pageSizes      = {31, 31, 31};
    spec.category       = "Pattern";
    spec.title          = "X Label Chart - Scroll";
    spec.kind           = ChartKind::BAR;
    spec.pageSize       = 31;
    spec.minimum        = 0.0f;
    spec.maximum        = 120.0f;
    spec.values         = {48.811728f, 6.7909808f, 18.98834f, 16.268004f, 48.012689f, 93.234739f, 38.353909f, 21.641804f, 44.33299f, 48.402778f, 61.01166f, 5.6164266f, 79.747942f, 99.45559f, 10.418381f, 65.722737f, 70.306927f, 92.072188f, 39.660494f, 6.8990055f, 83.91118f, 34.894547f, 42.565158f, 19.26869f, 84.75823f, 10.638717f, 31.478052f, 22.584877f, 44.453018f, 53.872599f, 81.707819f, 77.341392f, 19.785665f, 95.460391f, 42.637174f, 62.550583f, 97.175926f, 73.673697f, 15.500686f, 20.187757f, 3.7842936f, 11.969307f, 97.829218f, 66.302298f, 85.289781f, 63.433642f, 94.561043f, 68.795439f, 50.334362f, 21.893861f, 95.819616f, 91.864712f, 81.634088f, 99.695645f, 21.358025f, 7.1150549f, 13.756859f, 72.147634f, 31.670096f, 71.336591f, 77.566872f, 88.632545f, 5.7681756f, 70.949074f, 11.335734f, 50.384945f, 85.627572f, 33.112997f, 38.520233f, 54.9357f, 87.297668f, 3.5073731f, 12.20679f, 7.2230796f, 78.679698f, 90.774177f, 26.222565f, 97.370542f, 23.971193f, 77.629458f, 92.913237f, 45.131173f, 94.777092f, 98.641118f, 87.587449f, 10.9988f, 47.887517f, 84.673354f, 59.627915f, 73.985768f, 69.722222f, 73.997771f, 10.269204f};
    spec.secondary      = {49.35f, 56.32f, 32.66f, 11.94f, 14.87f, 3.98f, 58.59f, 25.13f, 15.41f, 54.99f, 40.08f, 23.73f, 16.86f, 55.84f, 47.1f, 51.12f, 2.12f, 32.35f, 52.85f, 23.94f, 12.78f, 14.21f, 31.57f, 10.24f, 17.88f, 50.05f, 30.24f, 6.02f, 53.76f, 7.72f, 16.18f, 15.54f, 54.86f, 5.89f, 33.67f, 21.9f, 47.04f, 64.17f, 49.17f, 36.0f, 25.7f, 45.26f, 50.63f, 37.85f, 27.85f, 4.0f, 8.17f, 30.68f, 12.95f, 34.16f, 33.91f, 54.5f, 28.99f, 20.39f, 55.85f, 22.0f, 51.34f, 43.79f, 26.13f, 35.88f};
    spec.subtitle       = "Subtitle";
    spec.description    = "2월 6일 오후 5시 - 6시";
    spec.hint           = "Hint text · here";
    spec.unit           = "kWh";
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::REPORT;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::SCROLL;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "pattern-01: Explicit BAR / REPORT; 60 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 200.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 138.0f, 643.0f, 200.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"1\nA", 0.0f}, {"7", 6.0f}, {"13", 12.0f}, {"19", 18.0f}, {"25", 24.0f}, {"31", 30.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"30", 30.0f}, {"60", 60.0f}, {"90", 90.0f}, {"120", 120.0f}}, true};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "pattern-02";
    spec.pageSizes      = {28, 31, 30};
    spec.category       = "Pattern";
    spec.title          = "X Label Chart - Swipe";
    spec.kind           = ChartKind::BAR;
    spec.pageSize       = 24;
    spec.minimum        = 0.0f;
    spec.maximum        = 120.0f;
    spec.values         = {76.067387f, 87.441701f, 90.071159f, 37.042181f, 33.293038f, 46.724966f, 85.979938f, 44.885117f, 13.563957f, 56.213992f, 55.551269f, 23.921468f, 81.077675f, 98.624829f, 11.13083f, 93.904321f, 7.4391289f, 8.5253772f, 28.027263f, 15.327503f, 49.438443f, 16.779835f, 55.623285f, 67.203361f, 93.49537f, 61.659808f, 95.153464f, 91.507202f, 66.770405f, 66.622085f, 44.148663f, 4.2884088f, 14.942558f, 84.753086f, 7.5471536f, 73.448217f, 46.653807f, 9.8799726f, 75.472394f, 63.184156f, 44.620199f, 54.348422f, 67.677469f, 45.101166f, 43.409636f, 93.467078f, 44.656207f, 75.989369f, 73.886317f, 76.618656f, 85.420953f, 42.268519f, 74.321845f, 5.0377229f, 31.947016f, 71.099108f, 68.173011f, 76.255144f, 0.28377915f, 8.1601509f, 8.5262346f, 95.209191f, 58.332476f, 62.093621f, 89.208676f, 52.02332f, 70.290638f, 15.615569f, 22.566015f, 66.450617f, 7.763203f, 3.2938957f, 83.906893f, 98.984911f, 27.540295f, 55.992798f, 22.614026f, 28.638546f, 16.041667f, 11.983882f, 39.921982f, 97.386831f, 0.42781207f, 94.723937f, 33.361626f, 21.27915f, 26.377743f, 57.299383f, 7.8712277f};
    spec.secondary      = {49.38f, 62.22f, 12.16f, 14.34f, 58.19f, 52.59f, 21.61f, 19.68f, 48.48f, 58.73f, 4.57f, 3.24f, 15.35f, 52.71f, 61.94f, 16.41f, 12.08f, 33.34f, 7.26f, 16.58f, 57.7f, 50.8f, 10.14f, 14.9f, 25.51f, 13.77f, 4.2f, 55.88f, 41.4f, 47.85f, 54.37f, 25.62f, 52.48f, 45.31f, 59.5f, 16.88f, 37.72f, 52.6f, 42.56f, 45.52f, 8.32f, 38.09f, 22.01f, 9.02f, 2.11f, 3.94f, 33.97f, 13.9f, 32.31f, 57.18f, 46.41f, 45.39f, 32.27f, 6.2f, 42.38f, 48.73f, 49.08f, 15.48f, 45.99f, 12.81f};
    spec.subtitle       = "Subtitle";
    spec.description    = "2월 6일 오후 5시 - 6시";
    spec.hint           = "Hint text · here";
    spec.unit           = "kWh";
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::REPORT;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::SWIPE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "pattern-02: Explicit BAR / REPORT; 60 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 200.0f;
    spec.interactive    = true;
    spec.frames         = {{21.0f, 138.0f, 643.0f, 200.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"1\nFeb", 0.0f}, {"8", 7.0f}, {"15", 14.0f}, {"21", 20.0f}, {"28", 27.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"30", 30.0f}, {"60", 60.0f}, {"90", 90.0f}, {"120", 120.0f}}, true};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id                      = "pattern-03";
    spec.category                = "Pattern";
    spec.title                   = "Temp Insight - Single Chart";
    spec.kind                    = ChartKind::BAR;
    spec.minimum                 = 0.0f;
    spec.maximum                 = 15.0f;
    spec.values                  = {8.0930041f, 4.1520062f, 9.6122428f, 7.2144547f, 4.1438272f, 4.6966564f, 4.9470165f, 9.4134259f, 9.7255144f, 9.2906893f, 9.9608025f, 8.6988169f, 4.2454732f, 7.7859568f, 9.6165638f, 9.8113683f, 4.8888889f, 8.4787551f, 9.9883745f, 9.2695988f, 7.285391f, 6.7764918f, 4.9280864f, 8.0364712f, 8.1757202f, 5.8643519f, 6.7319959f, 4.1860597f, 8.0783951f, 5.371965f, 8.8075103f};
    spec.secondary               = {5.9f, 4.12f, 6.18f, 4.64f, 5.4f, 5.39f, 5.91f, 4.55f, 4.64f, 6.4f, 5.31f, 5.84f, 4.53f, 5.91f, 4.45f, 5.8f, 4.85f, 5.32f, 5.03f, 4.35f, 5.45f, 4.86f, 6.07f, 4.75f, 5.27f, 5.63f, 5.66f, 5.57f, 5.12f, 4.88f, 5.06f};
    spec.subtitle                = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.";
    spec.hint                    = "Hint text";
    spec.chartWidth              = 350.0f;
    spec.chartHeight             = 84.0f;
    spec.simple                  = true;
    spec.axis                    = AxisPreset::DEFAULT;
    spec.layout                  = LayoutPreset::INSIGHT;
    spec.action                  = Action::DATA;
    spec.navigation              = Navigation::NONE;
    spec.numberMode              = NumberMode::NONE;
    spec.expected                = "pattern-03: Explicit BAR / INSIGHT; 31 points";
    spec.secondaryChart          = false;
    spec.interactive             = true;
    spec.frames                  = {{167.5f, 254.0f, 350.0f, 84.0f}};
    spec.referenceWidth          = 685.0f;
    spec.texts                   = {{"Lorem ipsum dolor sit amet, consectetur adipiscing elit.", 175.5f, 138.0f, 334.0f, 50.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Lorem ipsum dolor sit amet, consectetur adipiscing elit.", 175.5f, 138.0f, 334.0f, 50.0f, 19.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Hint text", 312.45312f, 192.0f, 60.078125f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}};
    spec.groupWidth              = 0.5f;
    spec.cornerRadius            = 10000.0f;
    spec.xAxis                   = {{{"Start", 0.0f}, {"End", 30.0f}}, true};
    spec.yAxis                   = {{{"0", 0.0f}, {"5", 5.0f}, {"10", 10.0f}, {"15", 15.0f}}, false};
    spec.texts[2].color          = 0xB88400u;
    spec.texts[2].background     = 0xFFEAB0u;
    spec.texts[2].darkColor      = 0xCC9300u;
    spec.texts[2].darkBackground = 0x594000u;
    spec.texts[0].fontSize       = 16.0f;
    spec.texts[1].fontSize       = 19.0f;
    spec.texts[2].fontSize       = 12.0f;
    spec.texts.erase(spec.texts.begin() + 0);
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "pattern-04";
    spec.category       = "Pattern";
    spec.title          = "Temp Insight - Multi Chart";
    spec.secondaryChart = true;
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 15.0f;
    spec.values         = {7.570216f, 5.9563786f, 6.040072f, 6.3398148f, 4.4852366f, 9.8837449f, 6.3871914f};
    spec.secondary      = {7.570216f, 5.9563786f, 6.040072f, 6.3398148f, 4.4852366f, 9.8837449f, 6.3871914f};
    spec.subtitle       = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.";
    spec.hint           = "Hint text";
    spec.chartWidth     = 156.0f;
    spec.chartHeight    = 84.0f;
    spec.simple         = true;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::INSIGHT;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "pattern-04: Explicit BAR / INSIGHT; 7 points";
    spec.secondaryKind  = ChartKind::BAR;
    spec.interactive    = true;
    spec.frames         = {{178.5f, 254.0f, 156.0f, 84.0f}, {350.5f, 254.0f, 156.0f, 84.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {{"Lorem ipsum dolor sit amet, consectetur adipiscing elit.", 175.5f, 138.0f, 334.0f, 50.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Lorem ipsum dolor sit amet, consectetur adipiscing elit.", 175.5f, 138.0f, 334.0f, 50.0f, 19.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Hint text", 312.45312f, 192.0f, 60.078125f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}};
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.companions.push_back({{}, {}, {7.57021605f, 5.9563786f, 6.04007202f, 6.33981481f, 4.48523663f, 9.88374486f, 6.38719136f}, {}, ChartKind::BAR, 0.0f, 15.0f});
    spec.xAxis                   = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}}, true};
    spec.yAxis                   = {{{"0", 0.0f}, {"5", 5.0f}, {"10", 10.0f}}, false};
    spec.companions[0].xAxis     = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}}, true};
    spec.companions[0].yAxis     = {{{"0", 0.0f}, {"5", 5.0f}, {"10", 10.0f}}, false};
    spec.texts[2].color          = 0xB88400u;
    spec.texts[2].background     = 0xFFEAB0u;
    spec.texts[2].darkColor      = 0xCC9300u;
    spec.texts[2].darkBackground = 0x594000u;
    spec.texts[0].fontSize       = 16.0f;
    spec.texts[1].fontSize       = 19.0f;
    spec.texts[2].fontSize       = 12.0f;
    spec.texts.erase(spec.texts.begin() + 0);
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "pattern-05";
    spec.category       = "Pattern";
    spec.title          = "Overflow Text";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {40.0f, 70.0f, 50.0f, 30.0f, 55.0f, 78.0f, 70.0f, 55.0f, 48.0f, 37.0f, 43.0f, 55.0f, 37.0f, 23.0f, 30.0f, 55.0f, 70.0f, 83.0f, 70.0f, 55.0f, 45.0f, 55.0f, 70.0f};
    spec.secondary      = {41.05f, 12.37f, 13.02f, 17.9f, 48.27f, 2.81f, 33.75f, 2.76f, 14.67f, 4.09f, 23.28f, 5.62f, 9.0f, 35.95f, 34.41f, 35.11f, 34.81f, 30.13f, 0.4f, 49.39f, 51.84f, 25.36f, 8.99f};
    spec.subtitle       = "Very Long Long Long Long Long Long Long Long Long Long Long Long Long Long Text";
    spec.description    = "Very Long Long Long Long Long Long Long Long Long Description";
    spec.hint           = "Hint text · here";
    spec.unit           = "kWh";
    spec.slots          = 2;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::RESPONSIVE;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "pattern-05: Explicit BAR / RESPONSIVE; 23 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 304.0f;
    spec.chartHeight    = 156.0f;
    spec.frames         = {{190.5f, 195.5f, 304.0f, 156.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {{"Subtitle", 190.5f, 124.5f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"12345345457456456 Unit", 190.5f, 143.5f, 106.28125f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"2월 6일 오후 5시 - 6시", 190.5f, 179.5f, 304.0f, 16.0f, 11.0f, 8684679u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}};
    spec.interactive    = true;
    spec.xAxis          = {{{"6PM", 0.0f}, {"12AM\n2026", 6.0f}, {"1AM", 12.0f}, {"2AM", 18.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"5", 33.0f}, {"22", 66.0f}, {"100", 100.0f}}, true};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 2.5f, 0x387AFFu, 1.0f});
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id                      = "pattern-06";
    spec.pageSizes               = {30, 30};
    spec.legendItems             = {{"Green", 183.25f, 389.0f, 43.421875f, 5025616u, false}, {"Blue", 234.671875f, 389.0f, 34.5f, 2201331u, false}, {"Yellow", 277.171875f, 389.0f, 44.921875f, 16761095u, false}};
    spec.category                = "Pattern";
    spec.title                   = "Updated Number Transition";
    spec.kind                    = ChartKind::BAR;
    spec.minimum                 = 0.0f;
    spec.maximum                 = 120.0f;
    spec.values                  = {82.642318f, 5.6421468f, 95.205761f, 89.604767f, 90.073731f, 38.587963f, 62.307956f, 84.690501f, 3.2664609f, 34.085219f, 22.825789f, 22.574588f, 38.26989f, 37.812929f, 29.845679f, 8.1953018f, 62.985254f, 58.413066f, 77.194787f, 31.676097f, 41.610082f, 78.60168f, 77.218793f, 12.770062f, 45.749314f, 32.946674f, 5.2263374f, 11.971022f, 32.193073f, 52.312243f, 10.600137f, 8.2913237f, 87.361111f, 74.969993f, 94.57476f, 43.706276f, 38.413923f, 24.376715f, 54.68107f, 34.265261f, 31.030521f, 53.618827f, 95.857339f, 47.869513f, 73.852881f, 56.523491f, 8.2270233f, 48.716564f, 49.597051f, 45.436385f, 11.54321f, 8.4113512f, 92.830933f, 95.666152f, 66.299726f, 83.743999f, 34.418724f, 56.595508f, 51.508916f, 61.134259f};
    spec.secondary               = {49.48f, 20.8f, 60.19f, 23.97f, 36.48f, 51.99f, 3.68f, 62.88f, 50.74f, 8.66f, 57.54f, 51.29f, 9.34f, 40.18f, 56.3f, 7.6f, 51.9f, 37.29f, 19.89f, 52.14f, 42.38f, 2.13f, 54.4f, 33.54f, 56.04f, 63.64f, 30.02f, 60.32f, 56.96f, 13.35f, 12.15f, 0.95f, 42.94f, 7.97f, 32.82f, 61.79f, 0.43f, 6.32f, 16.16f, 18.61f, 3.77f, 9.39f, 37.5f, 23.73f, 29.13f, 3.71f, 7.19f, 11.81f, 44.76f, 19.24f, 31.41f, 8.95f, 45.38f, 14.42f, 53.49f, 25.63f, 40.03f, 32.26f, 60.42f, 50.55f};
    spec.subtitle                = "Subtitle";
    spec.description             = "2월 6일 오후 5시 - 6시";
    spec.hint                    = "Hint text · here";
    spec.unit                    = "kWh";
    spec.axis                    = AxisPreset::DEFAULT;
    spec.layout                  = LayoutPreset::REPORT;
    spec.action                  = Action::DATA;
    spec.navigation              = Navigation::SWIPE;
    spec.numberMode              = NumberMode::NONE;
    spec.expected                = "pattern-06: Explicit BAR / REPORT; 60 points";
    spec.secondaryChart          = false;
    spec.chartWidth              = 300.0f;
    spec.chartHeight             = 200.0f;
    spec.pageSize                = 30;
    spec.frames                  = {{192.5f, 177.0f, 300.0f, 200.0f}};
    spec.referenceWidth          = 685.0f;
    spec.texts                   = {{"Subtitle", 192.5f, 85.0f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"100.56 Unit", 192.5f, 104.0f, 120.28125f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"SubValue SubUnit", 192.5f, 140.0f, 300.0f, 17.0f, 23.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"2026년 8월 20일 - 2025년 12월 20일", 192.5f, 161.0f, 300.0f, 16.0f, 11.0f, 8684679u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Hint text • here", 316.78125f, 107.5f, 91.9375f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}};
    spec.interactive             = true;
    spec.groupWidth              = 0.5f;
    spec.cornerRadius            = 10000.0f;
    spec.xAxis                   = {{{"0", 0.0f}, {"3", 3.0f}, {"6", 6.0f}, {"9", 9.0f}, {"12", 12.0f}, {"15", 15.0f}, {"18", 18.0f}, {"21", 21.0f}, {"24", 24.0f}, {"27", 27.0f}}, true};
    spec.yAxis                   = {{{"", 0.0f}, {"30", 30.0f}, {"60", 60.0f}, {"90", 90.0f}, {"120", 120.0f}}, true};
    spec.texts[4].color          = 0xB88400u;
    spec.texts[4].background     = 0xFFEAB0u;
    spec.texts[4].darkColor      = 0xCC9300u;
    spec.texts[4].darkBackground = 0x594000u;
    spec.texts[0].fontSize       = 12.0f;
    spec.texts[1].fontSize       = 23.0f;
    spec.texts[2].fontSize       = 13.0f;
    spec.texts[3].fontSize       = 11.0f;
    spec.texts[4].fontSize       = 12.0f;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id                      = "pattern-07";
    spec.pageSizes               = {31, 29};
    spec.legendItems             = {{"Green", 183.25f, 395.0f, 43.421875f, 5025616u, false}, {"Blue", 234.671875f, 395.0f, 34.5f, 2201331u, false}, {"Yellow", 277.171875f, 395.0f, 44.921875f, 16761095u, false}};
    spec.category                = "Pattern";
    spec.title                   = "compDataInfo - Default";
    spec.kind                    = ChartKind::BAR;
    spec.minimum                 = 0.0f;
    spec.maximum                 = 120.0f;
    spec.values                  = {12.63203f, 29.459019f, 9.1460905f, 67.742627f, 50.927641f, 11.787551f, 55.260631f, 49.248114f, 2.3919753f, 8.5193759f, 57.753772f, 14.292695f, 60.852195f, 9.7779493f, 80.823045f, 45.592421f, 38.653978f, 35.316358f, 96.073388f, 77.715192f, 11.105967f, 45.628429f, 60.294925f, 41.525206f, 27.590878f, 19.726509f, 59.907407f, 75.294067f, 89.343278f, 99.585905f, 22.071331f, 2.4785665f, 93.894033f, 1.2560014f, 92.465706f, 76.165123f, 46.181413f, 92.638032f, 79.73251f, 90.180898f, 36.328875f, 37.929527f, 66.587791f, 56.871571f, 84.089506f, 8.7354252f, 87.599451f, 51.545782f, 49.957133f, 61.84585f, 73.631687f, 23.586248f, 12.944102f, 83.680556f, 62.956104f, 74.227538f, 15.02572f, 1.4000343f, 79.029492f, 1.0005144f};
    spec.secondary               = {49.51f, 26.7f, 39.69f, 26.37f, 14.8f, 35.59f, 31.69f, 57.43f, 18.81f, 12.4f, 22.03f, 30.8f, 7.83f, 37.05f, 6.15f, 37.9f, 61.85f, 38.28f, 39.3f, 44.78f, 22.3f, 38.72f, 32.96f, 38.2f, 63.67f, 27.36f, 3.98f, 45.18f, 44.6f, 53.47f, 50.34f, 11.04f, 40.56f, 47.38f, 58.65f, 56.77f, 56.1f, 59.75f, 9.56f, 28.14f, 51.38f, 2.22f, 8.87f, 59.91f, 3.39f, 3.65f, 32.99f, 60.04f, 64.12f, 42.25f, 43.91f, 64.83f, 48.66f, 0.23f, 40.02f, 52.35f, 37.76f, 3.96f, 15.28f, 27.49f};
    spec.subtitle                = "Subtitle";
    spec.description             = "2월 6일 오후 5시 - 6시";
    spec.hint                    = "Hint text · here";
    spec.unit                    = "kWh";
    spec.axis                    = AxisPreset::DEFAULT;
    spec.layout                  = LayoutPreset::DATA_INFO;
    spec.action                  = Action::DATA;
    spec.navigation              = Navigation::SWIPE;
    spec.numberMode              = NumberMode::NONE;
    spec.expected                = "pattern-07: Explicit BAR / DATA_INFO; 60 points";
    spec.secondaryChart          = false;
    spec.chartWidth              = 300.0f;
    spec.chartHeight             = 200.0f;
    spec.pageSize                = 31;
    spec.interactive             = true;
    spec.frames                  = {{192.5f, 183.0f, 300.0f, 200.0f}};
    spec.referenceWidth          = 685.0f;
    spec.texts                   = {{"Subtitle", 204.5f, 79.0f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"100 Unit", 192.5f, 98.0f, 82.28125f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"200 kWh", 280.78125f, 98.0f, 85.890625f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"SubValue SubUnit", 192.5f, 134.0f, 300.0f, 17.0f, 23.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Proin tincidunt facilisis condimentum. Long Loing Long LongLongLongLongLongLong Long Long", 192.5f, 155.0f, 300.0f, 28.0f, 11.0f, 8684679u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Hint text • here", 370.67188f, 101.5f, 91.9375f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}};
    spec.groupWidth              = 0.5f;
    spec.cornerRadius            = 10000.0f;
    spec.xAxis                   = {{{"0", 0.0f}, {"4", 4.0f}, {"8", 8.0f}, {"12", 12.0f}, {"16", 16.0f}, {"20", 20.0f}, {"24", 24.0f}, {"28", 28.0f}}, true};
    spec.yAxis                   = {{{"", 0.0f}, {"30", 30.0f}, {"60", 60.0f}, {"90", 90.0f}, {"120", 120.0f}}, true};
    spec.texts[5].color          = 0xB88400u;
    spec.texts[5].background     = 0xFFEAB0u;
    spec.texts[5].darkColor      = 0xCC9300u;
    spec.texts[5].darkBackground = 0x594000u;
    spec.texts[0].fontSize       = 12.0f;
    spec.texts[1].fontSize       = 23.0f;
    spec.texts[2].fontSize       = 23.0f;
    spec.texts[3].fontSize       = 13.0f;
    spec.texts[4].fontSize       = 11.0f;
    spec.texts[5].fontSize       = 12.0f;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id                      = "pattern-08";
    spec.legendItems             = {{"Green", 183.25f, 378.5f, 43.421875f, 5025616u, false}, {"Blue", 234.671875f, 378.5f, 34.5f, 2201331u, false}, {"Yellow", 277.171875f, 378.5f, 44.921875f, 16761095u, false}};
    spec.category                = "Pattern";
    spec.title                   = "compDataInfo - 1 slot";
    spec.kind                    = ChartKind::BAR;
    spec.minimum                 = 0.0f;
    spec.maximum                 = 120.0f;
    spec.values                  = {72.251372f, 60.683299f, 74.938272f, 8.8434499f, 52.522291f, 70.172325f, 44.509602f, 87.879801f, 20.036008f, 12.583162f, 0.08916324f, 57.862654f, 46.397462f, 22.483711f, 16.985597f, 79.285837f, 88.396776f, 30.738169f, 44.581619f, 31.161694f, 32.453704f, 75.618141f, 84.111797f, 55.465535f, 5.728738f, 80.580418f, 33.106996f, 68.246742f, 53.900892f, 98.71142f};
    spec.secondary               = {49.53f, 32.59f, 19.2f, 28.78f, 58.12f, 19.19f, 59.71f, 51.98f, 51.87f, 16.13f, 51.52f, 10.31f, 6.33f, 33.92f, 20.99f, 3.19f, 6.81f, 39.27f, 58.71f, 37.42f, 2.23f, 10.3f, 11.53f, 42.86f, 6.3f, 56.08f, 42.94f, 30.04f, 32.24f, 28.6f};
    spec.subtitle                = "Subtitle";
    spec.description             = "2월 6일 오후 5시 - 6시";
    spec.hint                    = "Hint text · here";
    spec.unit                    = "kWh";
    spec.slots                   = 1;
    spec.axis                    = AxisPreset::DEFAULT;
    spec.layout                  = LayoutPreset::DATA_INFO;
    spec.action                  = Action::DATA;
    spec.navigation              = Navigation::NONE;
    spec.numberMode              = NumberMode::NONE;
    spec.expected                = "pattern-08: Explicit BAR / DATA_INFO; 30 points";
    spec.secondaryChart          = false;
    spec.chartWidth              = 300.0f;
    spec.chartHeight             = 200.0f;
    spec.frames                  = {{192.5f, 166.5f, 300.0f, 200.0f}};
    spec.referenceWidth          = 685.0f;
    spec.texts                   = {{"Subtitle", 192.5f, 95.5f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"100 Unit", 192.5f, 114.5f, 82.28125f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"200 kWh", 280.78125f, 114.5f, 85.890625f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"2026년 8월 20일 - 2025년 12월 20일", 192.5f, 150.5f, 300.0f, 16.0f, 11.0f, 8684679u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Hint text • here", 370.67188f, 118.0f, 91.9375f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}};
    spec.interactive             = true;
    spec.groupWidth              = 0.5f;
    spec.cornerRadius            = 10000.0f;
    spec.xAxis                   = {{{"0", 0.0f}, {"3", 3.0f}, {"6", 6.0f}, {"9", 9.0f}, {"12", 12.0f}, {"15", 15.0f}, {"18", 18.0f}, {"21", 21.0f}, {"24", 24.0f}, {"27", 27.0f}}, true};
    spec.yAxis                   = {{{"", 0.0f}, {"30", 30.0f}, {"60", 60.0f}, {"90", 90.0f}, {"120", 120.0f}}, true};
    spec.texts[4].color          = 0xB88400u;
    spec.texts[4].background     = 0xFFEAB0u;
    spec.texts[4].darkColor      = 0xCC9300u;
    spec.texts[4].darkBackground = 0x594000u;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id                      = "pattern-09";
    spec.legendItems             = {{"Green", 183.25f, 389.5f, 43.421875f, 5025616u, false}, {"Blue", 234.671875f, 389.5f, 34.5f, 2201331u, false}, {"Yellow", 277.171875f, 389.5f, 44.921875f, 16761095u, false}};
    spec.category                = "Pattern";
    spec.title                   = "compDataInfo - 2 slot";
    spec.kind                    = ChartKind::BAR;
    spec.minimum                 = 0.0f;
    spec.maximum                 = 120.0f;
    spec.values                  = {96.505487f, 37.40655f, 85.61214f, 23.838306f, 64.430727f, 27.14249f, 83.578532f, 68.306756f, 56.635802f, 9.0594993f, 82.36797f, 7.4254115f, 33.61454f, 39.947702f, 12.84465f, 90.576989f, 74.379287f, 6.2268519f, 13.280178f, 18.996056f, 20.90535f, 35.057442f, 7.1313443f, 90.213477f, 89.242112f, 72.118484f, 47.484568f, 9.167524f, 47.290809f, 26.051955f};
    spec.secondary               = {49.56f, 38.49f, 63.71f, 31.19f, 36.44f, 2.79f, 22.73f, 46.53f, 19.94f, 19.87f, 16.02f, 54.83f, 4.83f, 30.79f, 35.83f, 33.49f, 16.76f, 40.26f, 13.12f, 30.06f, 47.15f, 46.88f, 55.09f, 47.53f, 13.93f, 19.79f, 16.89f, 14.9f, 19.88f, 3.72f};
    spec.subtitle                = "Subtitle";
    spec.description             = "2월 6일 오후 5시 - 6시";
    spec.hint                    = "Hint text · here";
    spec.unit                    = "kWh";
    spec.slots                   = 2;
    spec.axis                    = AxisPreset::DEFAULT;
    spec.layout                  = LayoutPreset::DATA_INFO;
    spec.action                  = Action::DATA;
    spec.navigation              = Navigation::NONE;
    spec.numberMode              = NumberMode::NONE;
    spec.expected                = "pattern-09: Explicit BAR / DATA_INFO; 30 points";
    spec.secondaryChart          = false;
    spec.chartWidth              = 300.0f;
    spec.chartHeight             = 200.0f;
    spec.frames                  = {{192.5f, 177.5f, 300.0f, 200.0f}};
    spec.referenceWidth          = 685.0f;
    spec.texts                   = {{"Subtitle", 204.5f, 84.5f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"10 Unit", 192.5f, 103.5f, 67.28125f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"20 kWh", 265.78125f, 103.5f, 70.890625f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Subtitle", 354.5f, 84.5f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"10 Unit", 342.5f, 103.5f, 67.28125f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"20 kWh", 415.78125f, 103.5f, 70.890625f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"2026년 8월 20일 - 2025년 12월 20일", 192.5f, 161.5f, 300.0f, 16.0f, 11.0f, 8684679u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Hint text • here", 192.5f, 135.5f, 91.9375f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}, {"Hint text • here", 342.5f, 135.5f, 91.9375f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}};
    spec.interactive             = true;
    spec.groupWidth              = 0.5f;
    spec.cornerRadius            = 10000.0f;
    spec.xAxis                   = {{{"0", 0.0f}, {"3", 3.0f}, {"6", 6.0f}, {"9", 9.0f}, {"12", 12.0f}, {"15", 15.0f}, {"18", 18.0f}, {"21", 21.0f}, {"24", 24.0f}, {"27", 27.0f}}, true};
    spec.yAxis                   = {{{"", 0.0f}, {"30", 30.0f}, {"60", 60.0f}, {"90", 90.0f}, {"120", 120.0f}}, true};
    spec.texts[7].color          = 0xB88400u;
    spec.texts[7].background     = 0xFFEAB0u;
    spec.texts[7].darkColor      = 0xCC9300u;
    spec.texts[7].darkBackground = 0x594000u;
    spec.texts[8].color          = 0xB88400u;
    spec.texts[8].background     = 0xFFEAB0u;
    spec.texts[8].darkColor      = 0xCC9300u;
    spec.texts[8].darkBackground = 0x594000u;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id                      = "pattern-10";
    spec.legendItems             = {{"Green", 183.25f, 386.0f, 43.421875f, 5025616u, false}, {"Blue", 234.671875f, 386.0f, 34.5f, 2201331u, false}, {"Yellow", 277.171875f, 386.0f, 44.921875f, 16761095u, false}};
    spec.category                = "Pattern";
    spec.title                   = "compDataInfo - 3 slot";
    spec.kind                    = ChartKind::BAR;
    spec.minimum                 = 0.0f;
    spec.maximum                 = 120.0f;
    spec.values                  = {28.16701f, 65.981653f, 59.248971f, 79.573903f, 61.524348f, 80.408951f, 96.721536f, 67.252229f, 22.865226f, 12.943244f, 16.498628f, 19.951132f, 61.572359f, 42.596879f, 5.0f, 75.942215f, 78.880315f, 11.345165f, 89.386145f, 58.68227f, 72.319959f, 35.237483f, 15.336077f, 21.257716f, 46.829561f, 82.175069f, 91.49177f, 57.495713f, 92.532579f, 16.355453f};
    spec.secondary               = {49.58f, 44.38f, 43.21f, 33.59f, 14.76f, 51.4f, 50.75f, 41.08f, 53.01f, 23.6f, 45.51f, 34.34f, 3.32f, 27.66f, 50.67f, 63.78f, 26.72f, 41.24f, 32.52f, 22.7f, 27.07f, 18.47f, 33.66f, 52.19f, 21.56f, 48.51f, 55.85f, 64.76f, 7.52f, 43.85f};
    spec.subtitle                = "Subtitle";
    spec.description             = "2월 6일 오후 5시 - 6시";
    spec.hint                    = "Hint text · here";
    spec.unit                    = "kWh";
    spec.slots                   = 3;
    spec.axis                    = AxisPreset::DEFAULT;
    spec.layout                  = LayoutPreset::DATA_INFO;
    spec.action                  = Action::DATA;
    spec.navigation              = Navigation::NONE;
    spec.numberMode              = NumberMode::NONE;
    spec.expected                = "pattern-10: Explicit BAR / DATA_INFO; 30 points";
    spec.secondaryChart          = false;
    spec.chartWidth              = 300.0f;
    spec.chartHeight             = 200.0f;
    spec.frames                  = {{192.5f, 174.0f, 300.0f, 200.0f}};
    spec.referenceWidth          = 685.0f;
    spec.texts                   = {{"Subtitle", 204.5f, 88.0f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"10 Unit", 192.5f, 104.0f, 58.65625f, 28.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Subtitle", 304.5f, 88.0f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"20 Unit", 292.5f, 104.0f, 58.65625f, 28.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Subtitle", 404.5f, 88.0f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"30 Unit", 392.5f, 104.0f, 58.65625f, 28.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"2026년 8월 20일 - 2025년 12월 20일", 192.5f, 158.0f, 300.0f, 16.0f, 11.0f, 8684679u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Hint text", 192.5f, 132.0f, 60.078125f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}, {"Hint text", 292.5f, 132.0f, 60.078125f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}, {"Hint text", 392.5f, 132.0f, 60.078125f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}};
    spec.interactive             = true;
    spec.groupWidth              = 0.5f;
    spec.cornerRadius            = 10000.0f;
    spec.xAxis                   = {{{"0", 0.0f}, {"3", 3.0f}, {"6", 6.0f}, {"9", 9.0f}, {"12", 12.0f}, {"15", 15.0f}, {"18", 18.0f}, {"21", 21.0f}, {"24", 24.0f}, {"27", 27.0f}}, true};
    spec.yAxis                   = {{{"", 0.0f}, {"30", 30.0f}, {"60", 60.0f}, {"90", 90.0f}, {"120", 120.0f}}, true};
    spec.texts[7].color          = 0xB88400u;
    spec.texts[7].background     = 0xFFEAB0u;
    spec.texts[7].darkColor      = 0xCC9300u;
    spec.texts[7].darkBackground = 0x594000u;
    spec.texts[8].color          = 0xB88400u;
    spec.texts[8].background     = 0xFFEAB0u;
    spec.texts[8].darkColor      = 0xCC9300u;
    spec.texts[8].darkBackground = 0x594000u;
    spec.texts[9].color          = 0xB88400u;
    spec.texts[9].background     = 0xFFEAB0u;
    spec.texts[9].darkColor      = 0xCC9300u;
    spec.texts[9].darkBackground = 0x594000u;
    cases.push_back(std::move(spec));
  }
}

void AppendReferenceCases100To136(std::vector<ReferenceCase>& cases)
{
  {
    ReferenceCase spec;
    spec.id                      = "pattern-11";
    spec.category                = "Pattern";
    spec.title                   = "compDataInfo - Insight";
    spec.kind                    = ChartKind::BAR;
    spec.minimum                 = 0.0f;
    spec.maximum                 = 120.0f;
    spec.values                  = {0.56927298f, 79.741941f, 29.182099f, 9.3835734f, 77.136488f, 63.305041f, 17.271948f, 18.049554f, 52.057613f, 57.56773f, 35.814472f, 28.773148f, 63.604252f, 63.764575f, 26.784979f, 68.714849f, 35.233196f, 79.42644f, 6.2328532f, 83.553669f, 20.030864f, 9.4915981f, 42.059328f, 81.931584f, 11.824417f, 44.083505f, 98.461934f, 46.564643f, 22.959534f, 2.9552469f};
    spec.secondary               = {49.61f, 50.28f, 22.72f, 36.0f, 58.08f, 35.0f, 13.76f, 35.63f, 21.07f, 27.33f, 10.0f, 13.85f, 1.82f, 24.52f, 0.51f, 29.08f, 36.67f, 42.23f, 51.93f, 15.35f, 6.99f, 55.05f, 12.22f, 56.85f, 29.19f, 12.23f, 29.81f, 49.62f, 60.16f, 18.97f};
    spec.subtitle                = "Subtitle";
    spec.description             = "2월 6일 오후 5시 - 6시";
    spec.hint                    = "Insight · Recommended range";
    spec.unit                    = "kWh";
    spec.axis                    = AxisPreset::DEFAULT;
    spec.layout                  = LayoutPreset::DATA_INFO;
    spec.action                  = Action::DATA;
    spec.navigation              = Navigation::NONE;
    spec.numberMode              = NumberMode::NONE;
    spec.expected                = "pattern-11: Explicit BAR / DATA_INFO; 30 points";
    spec.secondaryChart          = false;
    spec.chartWidth              = 300.0f;
    spec.chartHeight             = 200.0f;
    spec.frames                  = {{192.5f, 176.0f, 300.0f, 200.0f}};
    spec.referenceWidth          = 685.0f;
    spec.texts                   = {{"Lorem ipsum dolor sit amet, consectetur adipiscing elit.", 200.5f, 100.0f, 284.0f, 50.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Lorem ipsum dolor sit amet, consectetur adipiscing elit.", 200.5f, 100.0f, 284.0f, 50.0f, 19.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Hint text", 312.45312f, 154.0f, 60.078125f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}};
    spec.interactive             = true;
    spec.groupWidth              = 0.5f;
    spec.cornerRadius            = 10000.0f;
    spec.xAxis                   = {{{"0", 0.0f}, {"3", 3.0f}, {"6", 6.0f}, {"9", 9.0f}, {"12", 12.0f}, {"15", 15.0f}, {"18", 18.0f}, {"21", 21.0f}, {"24", 24.0f}, {"27", 27.0f}}, true};
    spec.yAxis                   = {{{"", 0.0f}, {"30", 30.0f}, {"60", 60.0f}, {"90", 90.0f}, {"120", 120.0f}}, true};
    spec.texts[2].color          = 0xB88400u;
    spec.texts[2].background     = 0xFFEAB0u;
    spec.texts[2].darkColor      = 0xCC9300u;
    spec.texts[2].darkBackground = 0x594000u;
    spec.texts[0].fontSize       = 16.0f;
    spec.texts[1].fontSize       = 19.0f;
    spec.texts[2].fontSize       = 12.0f;
    spec.texts.erase(spec.texts.begin() + 0);
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id                      = "pattern-12";
    spec.pageSizes               = {31, 29};
    spec.legendItems             = {{"Green", 183.25f, 378.5f, 43.421875f, 5025616u, false}, {"Blue", 234.671875f, 378.5f, 34.5f, 2201331u, false}, {"Yellow", 277.171875f, 378.5f, 44.921875f, 16761095u, false}};
    spec.category                = "Pattern";
    spec.title                   = "ptrnVBarBasic - hierarchy";
    spec.kind                    = ChartKind::BAR;
    spec.minimum                 = 0.0f;
    spec.maximum                 = 120.0f;
    spec.values                  = {47.04561f, 12.020748f, 28.744856f, 46.600652f, 44.60048f, 9.1640946f, 78.5631f, 54.032064f, 77.546296f, 76.266289f, 73.648834f, 67.224794f, 73.043553f, 36.784122f, 11.532922f, 2.2282236f, 76.771262f, 43.804012f, 97.153635f, 26.943587f, 97.371399f, 91.153121f, 20.634431f, 5.5684156f, 17.560014f, 91.177126f, 1.7283951f, 9.7076475f, 71.905007f, 19.184671f, 0.92935528f, 96.151406f, 91.270576f, 24.558471f, 97.249657f, 51.319444f, 13.928326f, 8.5330933f, 32.664609f, 2.3722565f, 63.335048f, 68.639403f, 23.223594f, 94.988855f, 92.57716f, 9.8156721f, 36.827846f, 37.811214f, 95.481824f, 22.185357f, 37.674897f, 13.555384f, 84.394719f, 25.501543f, 97.369685f, 56.789266f, 34.624486f, 80.258059f, 72.702332f, 98.377058f};
    spec.secondary               = {49.63f, 56.18f, 2.22f, 38.4f, 36.4f, 18.6f, 41.78f, 30.18f, 54.14f, 31.07f, 39.49f, 58.37f, 0.31f, 21.39f, 15.35f, 59.38f, 46.63f, 43.22f, 6.34f, 7.99f, 51.91f, 26.64f, 55.79f, 61.51f, 36.82f, 40.95f, 3.76f, 34.48f, 47.8f, 59.1f, 46.32f, 61.45f, 28.63f, 49.46f, 57.8f, 31.66f, 9.49f, 1.9f, 41.55f, 10.76f, 29.44f, 31.34f, 60.74f, 45.79f, 4.67f, 3.36f, 32.01f, 41.17f, 30.93f, 27.33f, 41.42f, 19.28f, 0.05f, 59.26f, 37.66f, 55.97f, 26.44f, 57.43f, 49.57f, 42.16f};
    spec.subtitle                = "Subtitle";
    spec.description             = "2월 6일 오후 5시 - 6시";
    spec.hint                    = "Hint text · here";
    spec.unit                    = "kWh";
    spec.slots                   = 2;
    spec.axis                    = AxisPreset::DEFAULT;
    spec.layout                  = LayoutPreset::REPORT;
    spec.action                  = Action::DATA;
    spec.navigation              = Navigation::SWIPE;
    spec.numberMode              = NumberMode::NONE;
    spec.expected                = "pattern-12: Explicit BAR / REPORT; 60 points";
    spec.secondaryChart          = false;
    spec.chartWidth              = 300.0f;
    spec.chartHeight             = 200.0f;
    spec.pageSize                = 31;
    spec.frames                  = {{192.5f, 166.5f, 300.0f, 200.0f}};
    spec.referenceWidth          = 685.0f;
    spec.texts                   = {{"Subtitle", 192.5f, 95.5f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"120.5 kWh", 192.5f, 114.5f, 108.89062f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"2월 6일 오후 5시 - 6시", 192.5f, 150.5f, 300.0f, 16.0f, 11.0f, 8684679u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Hint text • here", 305.39062f, 118.0f, 105.9375f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}};
    spec.interactive             = true;
    spec.groupWidth              = 0.5f;
    spec.cornerRadius            = 10000.0f;
    spec.xAxis                   = {{{"0", 0.0f}, {"4", 4.0f}, {"8", 8.0f}, {"12", 12.0f}, {"16", 16.0f}, {"20", 20.0f}, {"24", 24.0f}, {"28", 28.0f}}, true};
    spec.yAxis                   = {{{"", 0.0f}, {"30", 30.0f}, {"60", 60.0f}, {"90", 90.0f}, {"120", 120.0f}}, true};
    spec.texts[3].color          = 0x4D4D52u;
    spec.texts[3].background     = 0xE1E2E8u;
    spec.texts[3].darkColor      = 0xCCCCCEu;
    spec.texts[3].darkBackground = 0x545457u;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id                      = "pattern-13";
    spec.legendItems             = {{"권장 영역", 303.640625f, 377.5f, 59.21875f, 7829367u, true}};
    spec.category                = "Pattern";
    spec.title                   = "tempBasic";
    spec.kind                    = ChartKind::BAR;
    spec.minimum                 = 0.0f;
    spec.maximum                 = 100.0f;
    spec.values                  = {95.553841f, 65.46725f, 50.092593f, 76.590364f, 68.417353f, 23.104424f, 56.70096f, 14.885974f, 50.745885f, 69.218964f, 38.206447f, 66.350309f, 47.477709f, 71.712106f, 3.2510288f, 24.810528f, 48.736283f, 94.781379f, 34.550754f, 2.6123114f, 74.274691f, 10.031722f, 66.673525f, 75.0643f};
    spec.secondary               = {49.66f, 62.07f, 46.73f, 40.81f, 14.72f, 2.2f, 4.8f, 24.73f, 22.21f, 34.8f, 3.98f, 37.88f, 63.81f, 18.26f, 30.19f, 24.67f, 56.58f, 44.21f, 25.75f, 0.63f, 31.83f, 63.22f, 34.35f, 1.17f};
    spec.subtitle                = "Subtitle";
    spec.description             = "2월 6일 오후 5시 - 6시";
    spec.hint                    = "Hint text · here";
    spec.unit                    = "kWh";
    spec.axis                    = AxisPreset::DEFAULT;
    spec.layout                  = LayoutPreset::DATA_INFO;
    spec.action                  = Action::DATA;
    spec.navigation              = Navigation::NONE;
    spec.numberMode              = NumberMode::NONE;
    spec.expected                = "pattern-13: Explicit BAR / DATA_INFO; 24 points";
    spec.secondaryChart          = false;
    spec.chartWidth              = 300.0f;
    spec.chartHeight             = 200.0f;
    spec.frames                  = {{192.5f, 165.5f, 300.0f, 200.0f}};
    spec.referenceWidth          = 685.0f;
    spec.texts                   = {{"Subtitle", 192.5f, 94.5f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"120.5 Unit", 192.5f, 113.5f, 105.28125f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"2월 6일 오후 5시 - 6시", 192.5f, 149.5f, 300.0f, 16.0f, 11.0f, 8684679u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Hint text • here", 301.78125f, 117.0f, 91.9375f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}};
    spec.interactive             = true;
    spec.groupWidth              = 0.5f;
    spec.cornerRadius            = 10000.0f;
    spec.xAxis                   = {{{"6PM", 0.0f}, {"12AM\n2026", 6.0f}, {"1AM", 12.0f}, {"2AM", 18.0f}}, true};
    spec.yAxis                   = {{{"", 0.0f}, {"5", 33.0f}, {"22", 66.0f}, {"100", 100.0f}}, true};
    spec.texts[3].color          = 0x4D4D52u;
    spec.texts[3].background     = 0xE1E2E8u;
    spec.texts[3].darkColor      = 0xCCCCCEu;
    spec.texts[3].darkBackground = 0x545457u;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "pattern-14";
    spec.category       = "Pattern";
    spec.title          = "tempMultiple";
    spec.secondaryChart = true;
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 40.0f;
    spec.values         = {25.376029f, 22.275977f, 9.1450617f, 27.464763f, 17.605453f, 22.159722f, 19.27572f, 15.990484f, 11.563272f, 10.808899f, 27.43107f, 17.35571f, 12.0643f, 1.9272119f, 19.537037f, 3.0419239f, 9.4789095f, 28.107253f, 23.74177f, 0.08616255f, 23.066358f, 24.163837f, 13.748971f, 14.414352f};
    spec.secondary      = {14.308128f, 0.46733539f, 12.151235f, 4.1746399f, 0.24125514f, 26.277006f, 3.7633745f, 23.07073f, 6.7916667f, 23.074331f, 18.955761f, 23.695216f, 12.10751f, 27.896348f, 26.987654f, 10.862912f, 29.89249f, 26.668981f, 29.340535f, 4.9441872f, 2.7391975f, 17.540381f, 23.05144f, 25.198302f};
    spec.subtitle       = "Subtitle";
    spec.description    = "2월 6일 오후 5시 - 6시";
    spec.hint           = "Hint text · here";
    spec.unit           = "kWh";
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::DATA_INFO;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "pattern-14: Explicit BAR / DATA_INFO; 24 points";
    spec.secondaryKind  = ChartKind::BAR;
    spec.chartWidth     = 304.0f;
    spec.chartHeight    = 73.0f;
    spec.frames         = {{190.5f, 134.0f, 304.0f, 73.0f}, {190.5f, 294.0f, 304.0f, 73.0f}, {190.5f, 454.0f, 304.0f, 108.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {{"Subtitle", 190.5f, 63.0f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"120.5 kWh", 190.5f, 82.0f, 108.89062f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"2월 6일 오후 5시 - 6시", 190.5f, 118.0f, 304.0f, 16.0f, 11.0f, 8684679u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Subtitle", 190.5f, 223.0f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"120.5 kWh", 190.5f, 242.0f, 108.89062f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"2월 6일 오후 5시 - 6시", 190.5f, 278.0f, 304.0f, 16.0f, 11.0f, 8684679u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Subtitle", 190.5f, 383.0f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"120.5 kWh", 190.5f, 402.0f, 108.89062f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"2월 6일 오후 5시 - 6시", 190.5f, 438.0f, 304.0f, 16.0f, 11.0f, 8684679u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Hint text • here", 303.39062f, 85.5f, 91.9375f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}, {"Hint text • here", 303.39062f, 245.5f, 91.9375f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}, {"Hint text • here", 303.39062f, 405.5f, 91.9375f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}};
    spec.interactive    = true;
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.companions.push_back({{}, {}, {14.3081276f, 0.46733539f, 12.1512346f, 4.17463992f, 0.24125514f, 26.2770062f, 3.76337449f, 23.0707304f, 6.79166667f, 23.0743313f, 18.9557613f, 23.695216f, 12.1075103f, 27.8963477f, 26.9876543f, 10.8629115f, 29.8924897f, 26.6689815f, 29.340535f, 4.94418724f, 2.73919753f, 17.5403807f, 23.0514403f, 25.1983025f}, {}, ChartKind::BAR, 0.0f, 40.0f});
    spec.companions.push_back({{}, {}, {15.4624486f, 4.21424897f, 14.0462963f, 3.10673868f, 18.4326132f, 9.28317901f, 20.473251f, 15.7065329f, 20.9089506f, 17.5619856f, 6.03600823f, 28.9236111f, 4.37294239f, 29.4210391f, 13.3271605f, 20.9061214f, 5.86162551f, 14.1195988f, 17.1615226f, 5.35776749f, 11.3009259f, 3.13914609f, 7.90946502f, 14.871142f}, {}, ChartKind::BAR, 0.0f, 40.0f});
    spec.xAxis                    = {{{"0", 0.0f}, {"3", 3.0f}, {"6", 6.0f}, {"9", 9.0f}, {"12", 12.0f}, {"15", 15.0f}, {"18", 18.0f}, {"21", 21.0f}}, false};
    spec.yAxis                    = {{{"", 0.0f}, {"20", 20.0f}, {"40", 40.0f}}, true};
    spec.companions[0].xAxis      = {{{"0", 0.0f}, {"3", 3.0f}, {"6", 6.0f}, {"9", 9.0f}, {"12", 12.0f}, {"15", 15.0f}, {"18", 18.0f}, {"21", 21.0f}}, false};
    spec.companions[0].yAxis      = {{{"", 0.0f}, {"20", 20.0f}, {"40", 40.0f}}, true};
    spec.companions[1].xAxis      = {{{"6PM", 0.0f}, {"12AM\n2026", 6.0f}, {"1AM", 12.0f}, {"2AM", 18.0f}}, true};
    spec.companions[1].yAxis      = {{{"", 0.0f}, {"16", 16.0f}, {"32", 32.0f}}, true};
    spec.texts[9].color           = 0x4D4D52u;
    spec.texts[9].background      = 0xE1E2E8u;
    spec.texts[9].darkColor       = 0xCCCCCEu;
    spec.texts[9].darkBackground  = 0x545457u;
    spec.texts[10].color          = 0x4D4D52u;
    spec.texts[10].background     = 0xE1E2E8u;
    spec.texts[10].darkColor      = 0xCCCCCEu;
    spec.texts[10].darkBackground = 0x545457u;
    spec.texts[11].color          = 0x4D4D52u;
    spec.texts[11].background     = 0xE1E2E8u;
    spec.texts[11].darkColor      = 0xCCCCCEu;
    spec.texts[11].darkBackground = 0x545457u;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id                      = "pattern-15";
    spec.pageSizes               = {31, 29};
    spec.category                = "Pattern";
    spec.title                   = "ptrnVBarBasic - default";
    spec.kind                    = ChartKind::BAR;
    spec.minimum                 = 0.0f;
    spec.maximum                 = 120.0f;
    spec.values                  = {62.796639f, 78.38906f, 16.100823f, 47.536866f, 7.2650892f, 70.594136f, 98.017833f, 46.326303f, 46.383745f, 47.572874f, 28.906036f, 76.802984f, 29.535322f, 88.33762f, 95.185185f, 77.238512f, 57.95439f, 34.863683f, 24.015775f, 71.089678f, 29.171811f, 3.2004458f, 61.076818f, 11.442901f, 48.125857f, 61.249143f, 15.010288f, 92.125343f, 4.9399863f, 73.207305f, 68.532236f, 25.482682f, 19.367284f, 10.67987f, 56.210562f, 86.82356f, 51.901578f, 30.456962f, 8.909465f, 25.530693f, 81.555213f, 18.958333f, 64.900549f, 42.838649f, 50.303498f, 3.3444787f, 47.640604f, 36.278292f, 74.195816f, 29.29441f, 10.216049f, 10.787894f, 21.133402f, 5.4501029f, 46.454047f, 56.490912f, 55.313786f, 14.527606f, 68.700274f, 93.140432f};
    spec.secondary               = {49.71f, 8.86f, 5.74f, 45.62f, 36.36f, 34.4f, 60.83f, 13.83f, 23.34f, 42.27f, 62.97f, 61.9f, 60.8f, 12.0f, 59.88f, 20.26f, 11.49f, 46.18f, 64.56f, 50.91f, 56.67f, 6.39f, 56.48f, 10.49f, 59.71f, 62.1f, 55.63f, 54.06f, 10.72f, 49.48f, 30.9f, 26.7f, 21.48f, 37.71f, 5.28f, 16.6f, 46.52f, 32.19f, 21.75f, 39.33f, 42.28f, 9.82f, 39.86f, 24.32f, 57.43f, 3.19f, 44.43f, 55.85f, 24.01f, 31.38f, 13.92f, 56.94f, 9.89f, 16.68f, 62.25f, 6.15f, 19.65f, 37.51f, 44.14f, 37.97f};
    spec.subtitle                = "Subtitle";
    spec.description             = "2월 6일 오후 5시 - 6시";
    spec.hint                    = "Hint text · here";
    spec.unit                    = "kWh";
    spec.axis                    = AxisPreset::DEFAULT;
    spec.layout                  = LayoutPreset::REPORT;
    spec.action                  = Action::DATA;
    spec.navigation              = Navigation::SWIPE;
    spec.numberMode              = NumberMode::NONE;
    spec.expected                = "pattern-15: Explicit BAR / REPORT; 60 points";
    spec.secondaryChart          = false;
    spec.chartWidth              = 300.0f;
    spec.chartHeight             = 200.0f;
    spec.pageSize                = 31;
    spec.frames                  = {{192.5f, 173.5f, 300.0f, 200.0f}};
    spec.referenceWidth          = 685.0f;
    spec.texts                   = {{"Subtitle", 192.5f, 102.5f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"120.5 kWh", 192.5f, 121.5f, 108.89062f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"2월 6일 오후 5시 - 6시", 192.5f, 157.5f, 300.0f, 16.0f, 11.0f, 8684679u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Hint text • here", 305.39062f, 125.0f, 91.9375f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}};
    spec.interactive             = true;
    spec.groupWidth              = 0.5f;
    spec.cornerRadius            = 10000.0f;
    spec.xAxis                   = {{{"0", 0.0f}, {"4", 4.0f}, {"8", 8.0f}, {"12", 12.0f}, {"16", 16.0f}, {"20", 20.0f}, {"24", 24.0f}, {"28", 28.0f}}, true};
    spec.yAxis                   = {{{"", 0.0f}, {"30", 30.0f}, {"60", 60.0f}, {"90", 90.0f}, {"120", 120.0f}}, true};
    spec.texts[3].color          = 0x4D4D52u;
    spec.texts[3].background     = 0xE1E2E8u;
    spec.texts[3].darkColor      = 0xCCCCCEu;
    spec.texts[3].darkBackground = 0x545457u;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id                      = "pattern-16";
    spec.pageSizes               = {31, 29};
    spec.category                = "Pattern";
    spec.title                   = "ptrnVBarBasic - medium";
    spec.kind                    = ChartKind::BAR;
    spec.minimum                 = 0.0f;
    spec.maximum                 = 120.0f;
    spec.values                  = {48.341907f, 91.094822f, 52.263374f, 81.230281f, 57.007888f, 66.015946f, 46.526063f, 99.772805f, 67.731481f, 77.562586f, 52.722908f, 90.743313f, 7.6731824f, 49.191529f, 68.384774f, 70.191187f, 22.512003f, 33.989198f, 98.449931f, 6.0176612f, 20.889918f, 25.78275f, 33.041838f, 62.420267f, 85.522977f, 36.917867f, 91.91358f, 11.003944f, 50.979081f, 42.703189f, 35.558985f, 8.5588134f, 48.122428f, 92.521433f, 42.990398f, 41.50463f, 15.224623f, 87.607167f, 56.183128f, 37.001886f, 75.742455f, 25.491255f, 91.186557f, 40.729595f, 82.762346f, 11.111968f, 15.90192f, 61.329733f, 30.111454f, 34.592764f, 94.526749f, 81.518347f, 30.13546f, 15.686728f, 98.665981f, 35.86334f, 58.143004f, 14.887689f, 85.109739f, 55.228909f};
    spec.secondary               = {49.73f, 14.76f, 50.25f, 48.03f, 14.68f, 18.0f, 23.85f, 8.38f, 56.41f, 46.01f, 27.46f, 41.42f, 59.3f, 8.87f, 9.72f, 50.56f, 21.45f, 47.17f, 18.97f, 43.55f, 36.59f, 42.97f, 35.05f, 15.15f, 2.34f, 25.82f, 29.59f, 38.92f, 63.36f, 24.6f, 4.09f, 36.78f, 19.09f, 12.12f, 31.11f, 11.58f, 37.2f, 20.62f, 15.15f, 48.85f, 24.89f, 2.65f, 11.23f, 60.5f, 31.69f, 3.13f, 5.23f, 39.07f, 43.37f, 54.4f, 26.42f, 47.83f, 13.17f, 2.49f, 48.77f, 32.87f, 17.39f, 9.21f, 64.0f, 14.9f};
    spec.subtitle                = "Subtitle";
    spec.description             = "2월 6일 오후 5시 - 6시";
    spec.hint                    = "Hint text · here";
    spec.unit                    = "kWh";
    spec.chartWidth              = 300.0f;
    spec.chartHeight             = 130.0f;
    spec.axis                    = AxisPreset::DEFAULT;
    spec.layout                  = LayoutPreset::REPORT;
    spec.action                  = Action::DATA;
    spec.navigation              = Navigation::SWIPE;
    spec.numberMode              = NumberMode::NONE;
    spec.expected                = "pattern-16: Explicit BAR / REPORT; 60 points";
    spec.secondaryChart          = false;
    spec.pageSize                = 31;
    spec.frames                  = {{192.5f, 208.5f, 300.0f, 130.0f}};
    spec.referenceWidth          = 685.0f;
    spec.texts                   = {{"Subtitle", 192.5f, 137.5f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"120.5 kWh", 192.5f, 156.5f, 108.89062f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"2월 6일 오후 5시 - 6시", 192.5f, 192.5f, 300.0f, 16.0f, 11.0f, 8684679u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Hint text • here", 305.39062f, 160.0f, 91.9375f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}};
    spec.interactive             = true;
    spec.groupWidth              = 0.5f;
    spec.cornerRadius            = 10000.0f;
    spec.xAxis                   = {{{"0", 0.0f}, {"4", 4.0f}, {"8", 8.0f}, {"12", 12.0f}, {"16", 16.0f}, {"20", 20.0f}, {"24", 24.0f}, {"28", 28.0f}}, true};
    spec.yAxis                   = {{{"", 0.0f}, {"60", 60.0f}, {"120", 120.0f}}, true};
    spec.texts[3].color          = 0x4D4D52u;
    spec.texts[3].background     = 0xE1E2E8u;
    spec.texts[3].darkColor      = 0xCCCCCEu;
    spec.texts[3].darkBackground = 0x545457u;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id                      = "pattern-17";
    spec.pageSizes               = {31, 29};
    spec.category                = "Pattern";
    spec.title                   = "ptrnVBarBasic - small";
    spec.kind                    = ChartKind::BAR;
    spec.minimum                 = 0.0f;
    spec.maximum                 = 120.0f;
    spec.values                  = {63.516804f, 11.20799f, 40.277778f, 77.88666f, 47.491427f, 46.622942f, 91.33059f, 27.293381f, 7.5977366f, 37.181927f, 83.947188f, 56.535494f, 48.774005f, 50.78618f, 26.769547f, 59.440158f, 61.14369f, 51.63323f, 2.5137174f, 48.353052f, 64.459877f, 11.328018f, 45.747599f, 98.582819f, 19.216392f, 86.660665f, 87.335391f, 59.512174f, 4.425583f, 64.050926f, 65.548697f, 32.375686f, 62.062757f, 70.659294f, 3.8443073f, 14.704218f, 8.1772977f, 52.164781f, 55.308642f, 11.436043f, 10.670439f, 17.209362f, 13.768861f, 12.694616f, 33.739712f, 48.509088f, 91.570645f, 38.233025f, 48.990055f, 80.631859f, 64.022634f, 48.545096f, 13.211591f, 44.441872f, 80.507545f, 22.643176f, 12.824074f, 78.210734f, 42.259945f, 2.502572f};
    spec.secondary               = {49.76f, 20.65f, 29.76f, 50.43f, 58.0f, 1.6f, 51.87f, 2.93f, 24.47f, 49.74f, 56.95f, 20.93f, 57.79f, 5.73f, 24.56f, 15.86f, 31.4f, 48.16f, 38.38f, 36.19f, 16.51f, 14.56f, 13.61f, 19.81f, 9.98f, 54.54f, 3.54f, 23.78f, 51.0f, 64.73f, 42.29f, 46.87f, 16.71f, 51.54f, 56.94f, 6.56f, 27.88f, 9.05f, 8.55f, 58.37f, 7.5f, 60.47f, 47.6f, 31.68f, 5.94f, 3.07f, 31.04f, 22.3f, 62.73f, 12.41f, 38.92f, 38.72f, 16.45f, 53.29f, 35.3f, 59.59f, 15.12f, 45.9f, 18.86f, 56.84f};
    spec.subtitle                = "Subtitle";
    spec.description             = "2월 6일 오후 5시 - 6시";
    spec.hint                    = "Hint text · here";
    spec.unit                    = "kWh";
    spec.chartWidth              = 300.0f;
    spec.chartHeight             = 100.0f;
    spec.simple                  = true;
    spec.axis                    = AxisPreset::DEFAULT;
    spec.layout                  = LayoutPreset::REPORT;
    spec.action                  = Action::DATA;
    spec.navigation              = Navigation::SWIPE;
    spec.numberMode              = NumberMode::NONE;
    spec.expected                = "pattern-17: Explicit BAR / REPORT; 60 points";
    spec.secondaryChart          = false;
    spec.pageSize                = 31;
    spec.frames                  = {{192.5f, 223.5f, 300.0f, 100.0f}};
    spec.referenceWidth          = 685.0f;
    spec.texts                   = {{"Subtitle", 192.5f, 152.5f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"120.5 kWh", 192.5f, 171.5f, 108.89062f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"2월 6일 오후 5시 - 6시", 192.5f, 207.5f, 300.0f, 16.0f, 11.0f, 8684679u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Hint text • here", 305.39062f, 175.0f, 91.9375f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}};
    spec.interactive             = true;
    spec.groupWidth              = 0.5f;
    spec.cornerRadius            = 10000.0f;
    spec.xAxis                   = {{{"0", 0.0f}, {"4", 4.0f}, {"8", 8.0f}, {"12", 12.0f}, {"16", 16.0f}, {"20", 20.0f}, {"24", 24.0f}, {"28", 28.0f}}, true};
    spec.yAxis                   = {{{"", 0.0f}, {"120", 120.0f}}, true};
    spec.texts[3].color          = 0x4D4D52u;
    spec.texts[3].background     = 0xE1E2E8u;
    spec.texts[3].darkColor      = 0xCCCCCEu;
    spec.texts[3].darkBackground = 0x545457u;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "pattern-18";
    spec.pageSizes      = {25, 25, 25, 25, 25, 25, 25, 5};
    spec.category       = "Pattern";
    spec.title          = "ptrnVBarBasic - Diverging";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = -120.0f;
    spec.maximum        = 120.0f;
    spec.values         = {49.975995f, -89.209534f, -6.3786008f, -91.654664f, -9.2352538f, 58.16358f, 98.196159f, 91.109396f, -34.701646f, 86.19513f, 78.491084f, -18.307613f, -60.991084f, 19.576475f, -25.987654f, -76.695816f, -18.967764f, 8.9248971f, -94.252401f, 29.525034f, -46.903292f, -46.99417f, 31.721536f, 73.194444f, -68.254458f, 54.288409f, 35.884774f, -91.366598f, -36.107682f, -92.165638f, -49.663923f, 27.199931f, -44.290123f, -76.479767f, -89.122085f, 46.177984f, 94.852538f, 81.592936f, 45.90535f, -69.000343f, 6.0116598f, 21.558642f, 98.628258f, -49.199246f, 39.804527f, 64.405007f, -17.373114f, -32.690329f, 94.996571f, -31.843278f, 70.740741f, 57.069616f, -25.943073f, 16.764403f, 17.290809f, 66.99417f, 72.047325f, 42.326818f, -86.364883f, -96.743827f, 34.2488f, 22.420182f, 20.087449f, 4.7814643f, -7.4483882f, 9.0769033f, 17.443759f, 62.590449f, -17.58179f, -14.430984f, 15.870027f, -16.555298f, 82.490569f, 25.72368f, 55.489712f, 91.541752f, 5.4847394f, -18.113426f, 56.055898f, -1.5134602f, 65.968621f, 19.366341f, 48.062414f, 91.069187f, 24.806413f, 67.545696f, -29.478395f, -14.290552f, -29.730281f, 7.6592078f, 75.408779f, 59.567816f, -14.184671f, 77.23774f, -11.22668f, 78.323302f, 34.529664f, 61.219564f, 68.516461f, -9.3821159f, 60.239883f, -0.27186214f, -11.164266f, 29.16761f, 45.291667f, 72.516547f, 11.336077f, -11.459619f, 24.993656f, 50.078618f, 2.8076132f, 19.600394f, 58.728567f, 1.4266975f, 99.670096f, 80.619256f, 27.730967f, 48.536094f, 29.084019f, -4.9462449f, -25.119685f, 15.722051f, 14.938272f, -82.779664f, -57.925583f, -7.2896091f, -1.7359396f, -70.647291f, -93.036008f, -15.321845f, 24.223594f, -55.634259f, -76.870713f, -6.646262f, 11.582305f, 0.28412209f, 23.409808f, 15.28035f, -17.190672f, 21.058471f, 2.1265432f, -82.628429f, 32.966392f, 18.787551f, -9.3624828f, -34.19976f, -28.069959f, -30.726166f, 6.2266804f, -91.779321f, 39.947188f, -79.08762f, 14.326132f, -30.675754f, 36.524005f, -83.086934f, -55.928326f, -20.271776f, -57.351852f, 10.856139f, -62.808299f, -1.8019547f, -63.655693f, -44.418896f, -9.7705761f, -92.797154f, -58.4369f, -34.591049f, -29.901577f, -58.195645f, -29.596708f, 31.697702f, 2.9715364f, -88.120885f, -1.3326475f, 31.73131f, -23.496914f, -82.32596f, -65.249657f, -69.058128f};
    spec.secondary      = {14.93f, 7.96f, 2.78f, 15.85f, 10.9f, 15.06f, 4.47f, 18.74f, 17.26f, 16.04f, 6.43f, 0.13f, 16.89f, 0.78f, 11.82f, 13.85f, 12.41f, 14.74f, 17.34f, 8.65f, 18.43f, 15.34f, 17.15f, 7.34f};
    spec.subtitle       = "Subtitle";
    spec.description    = "2월 6일 오후 5시 - 6시";
    spec.hint           = "Hint text · here";
    spec.unit           = "kWh";
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::REPORT;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::SCROLL;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "pattern-18: Explicit BAR / REPORT; 24 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 300.0f;
    spec.chartHeight    = 200.0f;
    spec.pageSize       = 25;
    spec.frames         = {{192.5f, 138.0f, 300.0f, 200.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.interactive    = true;
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"0", 0.0f}, {"3", 3.0f}, {"6", 6.0f}, {"9", 9.0f}, {"12", 12.0f}, {"15", 15.0f}, {"18", 18.0f}, {"21", 21.0f}, {"24", 24.0f}}, true};
    spec.yAxis          = {{{"-120", -120.0f}, {"-60", -60.0f}, {"0", 0.0f}, {"60", 60.0f}, {"120", 120.0f}}, true};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "pattern-19";
    spec.category       = "Pattern";
    spec.title          = "ptrnVBarBasic - Stack";
    spec.kind           = ChartKind::STACKED;
    spec.minimum        = 0.0f;
    spec.maximum        = 3.5f;
    spec.values         = {1.8f, 1.8f, 1.6f, 1.0f, 0.8f, 0.1f, 1.0f};
    spec.secondary      = {0.6f, 0.6f, 0.5f, 0.4f, 0.8f, 0.1f, 2.0f};
    spec.subtitle       = "Subtitle";
    spec.description    = "2월 6일 오후 5시 - 6시";
    spec.hint           = "Hint text · here";
    spec.unit           = "kWh";
    spec.legend         = true;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::REPORT;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "pattern-19: Explicit STACKED / REPORT; 24 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 300.0f;
    spec.chartHeight    = 150.0f;
    spec.frames         = {{192.5f, 163.0f, 300.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.interactive    = true;
    spec.groupWidth     = 0.3f;
    spec.xAxis          = {{{"S", 0.0f}, {"M", 1.0f}, {"T", 2.0f}, {"W", 3.0f}, {"T", 4.0f}, {"F", 5.0f}, {"S", 6.0f}}, true};
    spec.yAxis          = {{{"0", 0.0f}, {"0.5", 0.5f}, {"1.0", 1.0f}, {"1.5", 1.5f}, {"2.0", 2.0f}, {"2.5", 2.5f}, {"3.0", 3.0f}, {"3.5", 3.5f}}, false};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "generic-plugin-01";
    spec.category       = "Generic Plugin";
    spec.title          = "Generic Plugin Template - 1";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 40.0f;
    spec.maximum        = 72.0f;
    spec.values         = {59.9393f, 50.613769f, 52.635974f, 54.647891f, 50.47668f, 50.245799f, 58.152778f, 56.913666f, 58.874143f, 53.787294f, 53.258059f, 51.854338f, 54.884774f, 52.843193f, 52.519719f, 54.778549f, 59.002401f, 54.203618f, 56.801955f, 55.069016f, 50.239369f, 54.288323f, 54.376372f, 53.960305f, 50.570988f, 50.257802f, 58.69976f, 58.983282f, 56.046639f, 57.791067f, 52.858539f};
    spec.secondary      = {41.57f, 43.51f, 44.37f, 40.24f, 40.19f, 47.05f, 49.0f, 41.27f, 40.07f, 39.69f, 47.39f, 45.86f, 40.98f, 39.62f, 49.31f, 42.94f, 39.63f, 41.35f, 44.65f, 47.61f, 46.4f, 39.96f, 47.58f, 44.28f, 44.44f, 48.19f, 40.62f, 42.66f, 47.64f, 40.68f, 45.45f};
    spec.subtitle       = "Subtitle";
    spec.description    = "2월 6일 오후 5시 - 6시";
    spec.hint           = "Hint text · here";
    spec.unit           = "kWh";
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::DATA_INFO;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "generic-plugin-01: Explicit BAR / DATA_INFO; 31 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 171.0f;
    spec.frames         = {{21.0f, 178.0f, 643.0f, 171.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {{"Subtitle", 21.0f, 127.0f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"120.5 Unit", 21.0f, 146.0f, 105.28125f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}};
    spec.interactive    = true;
    spec.xAxis          = {{{"6PM", 0.0f}, {"12AM\n2026", 8.0f}, {"1AM", 16.0f}, {"2AM", 24.0f}}, true};
    spec.yAxis          = {{{"40", 40.0f}, {"48", 48.0f}, {"56", 56.0f}, {"64", 64.0f}, {"72", 72.0f}}, true};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 4.0f, 0x387AFFu, 1.0f});
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id                      = "generic-plugin-02";
    spec.category                = "Generic Plugin";
    spec.title                   = "Generic Plugin Template - 2";
    spec.kind                    = ChartKind::BAR;
    spec.minimum                 = 0.0f;
    spec.maximum                 = 100.0f;
    spec.values                  = {50.762174f, 45.675583f, 55.300926f, 6.7986968f, 23.625686f, 3.3127572f, 61.909294f, 45.094307f, 5.9542181f, 49.427298f, 43.414781f, 96.558642f, 2.6860425f, 51.920439f, 8.4593621f, 55.018861f, 3.9446159f, 74.989712f, 39.759088f, 32.820645f, 29.483025f, 90.240055f, 71.881859f, 5.2726337f};
    spec.secondary               = {49.86f, 44.24f, 12.78f, 60.06f, 36.28f, 1.01f, 33.94f, 46.13f, 26.74f, 64.68f, 44.92f, 3.98f, 51.78f, 58.21f, 18.92f, 7.04f, 6.22f, 52.11f, 51.01f, 6.75f, 1.19f, 30.89f, 57.87f, 38.46f};
    spec.subtitle                = "Subtitle";
    spec.description             = "2월 6일 오후 5시 - 6시";
    spec.hint                    = "Hint text · here";
    spec.unit                    = "kWh";
    spec.axis                    = AxisPreset::DEFAULT;
    spec.layout                  = LayoutPreset::DATA_INFO;
    spec.action                  = Action::DATA;
    spec.navigation              = Navigation::NONE;
    spec.numberMode              = NumberMode::NONE;
    spec.expected                = "generic-plugin-02: Explicit BAR / DATA_INFO; 24 points";
    spec.secondaryChart          = false;
    spec.chartWidth              = 304.0f;
    spec.chartHeight             = 171.0f;
    spec.frames                  = {{190.5f, 178.0f, 304.0f, 171.0f}};
    spec.referenceWidth          = 685.0f;
    spec.texts                   = {{"Subtitle", 190.5f, 127.0f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"120.5", 190.5f, 147.5f, 69.0f, 26.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Hint text • here", 263.5f, 149.5f, 91.9375f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}};
    spec.interactive             = true;
    spec.groupWidth              = 0.5f;
    spec.cornerRadius            = 10000.0f;
    spec.pointColors             = {16233728u, 16233728u, 16233728u, 2932476u, 1223424u, 2932476u, 16409600u, 16233728u, 2932476u, 16233728u, 16233728u, 15084845u, 2932476u, 16233728u, 2932476u, 16233728u, 2932476u, 16409600u, 1223424u, 1223424u, 1223424u, 15084845u, 16409600u, 2932476u};
    spec.pointThemes             = true;
    spec.xAxis                   = {{{"6PM", 0.0f}, {"12AM\n2026", 6.0f}, {"1AM", 12.0f}, {"2AM", 18.0f}}, true};
    spec.yAxis                   = {{{"", 0.0f}, {"1", 20.0f}, {"22", 40.0f}, {"100", 60.0f}, {"447", 80.0f}, {"2000", 100.0f}}, true};
    spec.texts[2].color          = 0xB88400u;
    spec.texts[2].background     = 0xFFEAB0u;
    spec.texts[2].darkColor      = 0xCC9300u;
    spec.texts[2].darkBackground = 0x594000u;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "generic-plugin-03";
    spec.category       = "Generic Plugin";
    spec.title          = "Generic Plugin Template - 3";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {std::numeric_limits<float>::quiet_NaN(), 0.0f, std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), 20.0f, 50.0f, 80.0f, std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), 50.0f, 65.0f, 50.0f, 75.0f, 40.0f, 20.0f, 50.0f, 80.0f, 60.0f, 50.0f, 30.0f, 50.0f, 30.0f, 65.0f};
    spec.secondary      = {49.88f, 50.13f, 57.29f, 62.46f, 14.6f, 49.61f, 61.96f, 40.68f, 59.8f, 3.41f, 9.41f, 48.49f, 50.27f, 55.07f, 33.76f, 37.34f, 16.18f, 53.1f, 5.42f, 64.39f, 46.11f, 2.47f, 36.44f, 43.12f};
    spec.subtitle       = "Subtitle";
    spec.description    = "2월 6일 오후 5시 - 6시";
    spec.hint           = "Hint text · here";
    spec.unit           = "kWh";
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::DATA_INFO;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "generic-plugin-03: Explicit BAR / DATA_INFO; 24 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 304.0f;
    spec.chartHeight    = 171.0f;
    spec.frames         = {{190.5f, 178.0f, 304.0f, 171.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {{"Subtitle", 190.5f, 127.0f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"120.5 Unit", 190.5f, 146.0f, 105.28125f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}};
    spec.interactive    = true;
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"6PM", 0.0f}, {"12AM\n2026", 6.0f}, {"1AM", 12.0f}, {"2AM", 18.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"0000", 50.0f}, {"0000", 100.0f}}, true};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "generic-plugin-04";
    spec.category       = "Generic Plugin";
    spec.title          = "Generic Plugin Template - 4";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {40.0f, 40.0f, 70.0f, 50.0f, 30.0f, 55.0f, 78.0f, 70.0f, 55.0f, 48.0f, 37.0f, 43.0f, 55.0f, 37.0f, 23.0f, 30.0f, 55.0f, 70.0f, 83.0f, 70.0f, 55.0f, 45.0f, 55.0f, 70.0f};
    spec.secondary      = {11.938529f, 16.338477f, 10.707562f, 6.5272634f, 4.1679527f, 16.222222f, 20.83822f, 25.052984f, 28.125772f, 4.8713992f, 28.99357f, 26.41821f, 28.6268f, 25.989712f, 21.099537f, 12.104424f, 26.041409f, 22.169753f, 25.30427f, 9.1486625f, 9.628858f, 18.226337f, 15.311471f, 23.476852f};
    spec.subtitle       = "Subtitle";
    spec.description    = "2월 6일 오후 5시 - 6시";
    spec.hint           = "Hint text · here";
    spec.unit           = "kWh";
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::DATA_INFO;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "generic-plugin-04: Explicit BAR / DATA_INFO; 24 points";
    spec.secondaryChart = true;
    spec.secondaryKind  = ChartKind::BAR;
    spec.chartWidth     = 304.0f;
    spec.chartHeight    = 109.0f;
    spec.frames         = {{190.5f, 114.0f, 304.0f, 109.0f}, {190.5f, 290.0f, 304.0f, 123.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {{"Subtitle", 190.5f, 63.0f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"120.5 Unit", 190.5f, 82.0f, 105.28125f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Subtitle", 190.5f, 239.0f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"120.5 Unit", 190.5f, 258.0f, 105.28125f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}};
    spec.interactive    = true;
    spec.companions.push_back({{}, {}, {11.9385288f, 16.3384774f, 10.7075617f, 6.52726337f, 4.16795267f, 16.2222222f, 20.8382202f, 25.0529835f, 28.1257716f, 4.87139918f, 28.99357f, 26.4182099f, 28.6268004f, 25.9897119f, 21.099537f, 12.1044239f, 26.0414095f, 22.1697531f, 25.3042696f, 9.14866255f, 9.62885802f, 18.2263374f, 15.3114712f, 23.4768518f}, {}, ChartKind::BAR, 0.0f, 40.0f});
    spec.xAxis               = {{{"0", 0.0f}, {"3", 3.0f}, {"6", 6.0f}, {"9", 9.0f}, {"12", 12.0f}, {"15", 15.0f}, {"18", 18.0f}, {"21", 21.0f}}, false};
    spec.yAxis               = {{{"0", 0.0f}, {"50", 50.0f}, {"100", 100.0f}}, true};
    spec.companions[0].xAxis = {{{"6PM", 0.0f}, {"12AM\n2026", 6.0f}, {"1AM", 12.0f}, {"2AM", 18.0f}}, true};
    spec.companions[0].yAxis = {{{"0", 0.0f}, {"20", 20.0f}, {"40", 40.0f}}, true};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 2.5f, 0x387AFFu, 1.0f});
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "generic-plugin-05";
    spec.category       = "Generic Plugin";
    spec.title          = "Temperature and Humidity (Combined)";
    spec.secondaryChart = true;
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {40.0f, 40.0f, std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), 55.0f, 78.0f, 70.0f, 55.0f, 48.0f, 37.0f, 43.0f, 55.0f, 37.0f, 23.0f, 30.0f, 55.0f, 70.0f, 83.0f, 70.0f, 55.0f, 45.0f, 55.0f, 70.0f};
    spec.secondary      = {40.0f, 40.0f, std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), 55.0f, 78.0f, 70.0f, 55.0f, 48.0f, 37.0f, 43.0f, 55.0f, 37.0f, 23.0f, 30.0f, 55.0f, 70.0f, 83.0f, 70.0f, 55.0f, 45.0f, 55.0f, 70.0f};
    spec.subtitle       = "Subtitle";
    spec.description    = "2월 6일 오후 5시 - 6시";
    spec.hint           = "Hint text · here";
    spec.unit           = "kWh";
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::DATA_INFO;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "generic-plugin-05: Explicit BAR / DATA_INFO; 24 points";
    spec.secondaryKind  = ChartKind::LINE;
    spec.chartWidth     = 304.0f;
    spec.chartHeight    = 109.0f;
    spec.frames         = {{190.5f, 114.0f, 304.0f, 109.0f}, {190.5f, 290.0f, 304.0f, 123.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {{"Temperature", 190.5f, 63.0f, 69.796875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"40 ℃", 190.5f, 82.0f, 53.765625f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Humidity", 190.5f, 239.0f, 48.890625f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"40 %", 190.5f, 258.0f, 50.75f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}};
    spec.interactive    = true;
    spec.companions.push_back({{}, {}, {40.0f, 40.0f, std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), 55.0f, 78.0f, 70.0f, 55.0f, 48.0f, 37.0f, 43.0f, 55.0f, 37.0f, 23.0f, 30.0f, 55.0f, 70.0f, 83.0f, 70.0f, 55.0f, 45.0f, 55.0f, 70.0f}, {}, ChartKind::LINE, 0.0f, 100.0f});
    spec.xAxis               = {{{"0", 0.0f}, {"3", 3.0f}, {"6", 6.0f}, {"9", 9.0f}, {"12", 12.0f}, {"15", 15.0f}, {"18", 18.0f}, {"21", 21.0f}}, false};
    spec.yAxis               = {{{"0", 0.0f}, {"50", 50.0f}, {"100", 100.0f}}, true};
    spec.companions[0].xAxis = {{{"11:35", 0.0f}, {"11:50", 8.0f}, {"12:05\nMM", 16.0f}, {"12:20", 23.0f}}, true};
    spec.companions[0].yAxis = {{{"0", 0.0f}, {"50", 50.0f}, {"100", 100.0f}}, true};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 2.5f, 0x387AFFu, 1.0f});
    spec.companions[0].line = {0xBFD9FFu, 1.0f, 2.5f, 2.5f, 0x387AFFu, 1.0f};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id                      = "custom-01";
    spec.legendItems             = {{"Green", 29.0f, 428.0f, 43.421875f, 5025616u, false}, {"Blue", 80.421875f, 428.0f, 34.5f, 2201331u, false}, {"Yellow", 122.921875f, 428.0f, 44.921875f, 16761095u, false}};
    spec.category                = "Custom";
    spec.title                   = "Fake Dom simulation";
    spec.kind                    = ChartKind::BAR;
    spec.minimum                 = 0.0f;
    spec.maximum                 = 110.0f;
    spec.values                  = {0.38743141f, 6.6071674f, 1.3351337f, 9.5096022f, 9.0318073f, 8.5437243f, 1.8725137f, 9.1416324f, 4.5486111f, 0.80949931f, 0.26997599f, 2.6831276f, 9.6538923f, 5.7501715f, 6.280607f, 1.7390261f, 8.9155521f, 8.6743827f, 0.39823388f, 3.0994513f, 3.1977881f, 8.9648491f, 1.6352023f, 3.1841564f, 0.77220508f, 7.8561385f, 1.966821f, 9.1536351f, 5.0955933f, 2.8791152f, 74.424726f, 66.868999f, 92.543724f, 89.720508f, 59.633916f, 44.259259f, 70.75703f, 62.584019f, 17.271091f, 50.867627f, 9.0526406f, 44.912551f, 63.385631f, 32.373114f, 60.516975f, 41.644376f, 65.878772f, 97.417695f, 18.977195f, 42.902949f, 88.948045f, 28.717421f, 96.778978f, 68.441358f, 4.1983882f, 60.840192f, 69.230967f, 78.753429f, 68.419925f, 24.650206f};
    spec.secondary               = {49.96f, 2.82f, 60.8f, 4.68f, 14.56f, 0.41f, 16.01f, 24.33f, 29.0f, 14.61f, 32.89f, 52.03f, 45.76f, 45.68f, 13.29f, 63.23f, 46.04f, 56.06f, 63.64f, 42.32f, 50.88f, 47.23f, 37.13f, 57.1f};
    spec.subtitle                = "Subtitle";
    spec.description             = "2월 6일 오후 5시 - 6시";
    spec.hint                    = "Hint text · here";
    spec.unit                    = "kWh";
    spec.axis                    = AxisPreset::DEFAULT;
    spec.layout                  = LayoutPreset::REPORT;
    spec.action                  = Action::RECREATE;
    spec.navigation              = Navigation::NONE;
    spec.numberMode              = NumberMode::NONE;
    spec.expected                = "custom-01: Explicit BAR / REPORT; 24 points";
    spec.referenceWidth          = 685.0f;
    spec.texts                   = {{"Very Long Long Long Long Long Long Long Long Long Long Long Long Long Long Long Long Long Long Long Long Long Text", 21.0f, 111.0f, 643.0f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Unit", 21.0f, 130.0f, 72.21875f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"kWh", 99.21875f, 130.0f, 77.90625f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"SubValue SubUnit", 21.0f, 166.0f, 643.0f, 17.0f, 23.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Very Long Long Long Long Long Long Long Long Long Long Long Long Long Description", 21.0f, 187.0f, 643.0f, 14.0f, 11.0f, 8684679u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Hint text • here", 181.125f, 133.5f, 91.9375f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}};
    spec.interactive             = true;
    spec.frames                  = {{190.5f, 145.0f, 304.0f, 219.0f}};
    spec.texts[5].color          = 0xB88400u;
    spec.texts[5].background     = 0xFFEAB0u;
    spec.texts[5].darkColor      = 0xCC9300u;
    spec.texts[5].darkBackground = 0x594000u;
    spec.texts[0].fontSize       = 12.0f;
    spec.texts[1].fontSize       = 23.0f;
    spec.texts[2].fontSize       = 23.0f;
    spec.texts[3].fontSize       = 13.0f;
    spec.texts[4].fontSize       = 11.0f;
    spec.texts[5].fontSize       = 12.0f;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "custom-02";
    spec.category       = "Custom";
    spec.title          = "Custom Button to hide show container";
    spec.kind           = ChartKind::BAR;
    spec.minimum        = 0.0f;
    spec.maximum        = 110.0f;
    spec.values         = {67.17f, 22.96f, 43.16f, 27.43f, 49.22f, 4.88f, 82.94f, 39.6f, 30.36f, 15.39f, 78.44f, 46.64f, 60.59f, 72.81f, 64.59f, 40.58f, 85.31f, 39.68f, 99.27f, 38.62f, 30.66f, 16.13f, 74.19f, 84.86f};
    spec.secondary      = {49.98f, 8.72f, 40.31f, 7.09f, 57.88f, 49.01f, 44.03f, 18.88f, 62.07f, 18.35f, 62.38f, 31.54f, 44.26f, 42.55f, 28.13f, 28.52f, 56.0f, 57.05f, 18.05f, 34.96f, 30.8f, 18.81f, 15.7f, 61.76f};
    spec.subtitle       = "Subtitle";
    spec.description    = "2월 6일 오후 5시 - 6시";
    spec.hint           = "Hint text · here";
    spec.unit           = "kWh";
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::REPORT;
    spec.action         = Action::SHOW_HIDE;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "custom-02: Explicit BAR / REPORT; 24 points";
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.interactive    = true;
    spec.frames         = {{190.5f, 145.0f, 304.0f, 219.0f}};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id                      = "custom-03";
    spec.category                = "Custom";
    spec.title                   = "Data Update";
    spec.kind                    = ChartKind::BAR;
    spec.minimum                 = 0.0f;
    spec.maximum                 = 120.0f;
    spec.values                  = {71.081104f, 57.352538f, 73.15072f, 34.525034f, 87.154492f, 84.125514f, 30.376372f, 93.808299f, 83.063272f, 91.96845f, 10.647291f, 3.2973251f, 52.634602f, 71.004801f, 78.161008f, 45.708162f, 8.2141632f, 40.987654f, 4.5224623f, 55.608711f, 25.110597f, 62.410837f, 46.521776f, 63.863169f, 52.706619f, 14.286694f, 90.578704f, 8.7431413f, 92.236797f, 38.590535f};
    spec.secondary               = {50.01f, 14.61f, 19.82f, 9.49f, 36.2f, 32.62f, 7.04f, 13.43f, 30.14f, 22.08f, 26.87f, 11.06f, 42.75f, 39.41f, 42.97f, 58.82f, 0.95f, 58.04f, 37.46f, 27.6f, 10.72f, 55.39f, 59.26f, 1.43f};
    spec.subtitle                = "Subtitle";
    spec.description             = "2월 6일 오후 5시 - 6시";
    spec.hint                    = "Hint text · here";
    spec.unit                    = "kWh";
    spec.axis                    = AxisPreset::DEFAULT;
    spec.layout                  = LayoutPreset::REPORT;
    spec.action                  = Action::DATA;
    spec.navigation              = Navigation::NONE;
    spec.numberMode              = NumberMode::NONE;
    spec.expected                = "custom-03: Explicit BAR / REPORT; 24 points";
    spec.secondaryChart          = false;
    spec.chartWidth              = 643.0f;
    spec.chartHeight             = 300.0f;
    spec.frames                  = {{21.0f, 134.0f, 643.0f, 300.0f}};
    spec.referenceWidth          = 685.0f;
    spec.texts                   = {{"Subtitle", 21.0f, 63.0f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"100 Unit", 21.0f, 82.0f, 82.28125f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"200 kWh", 109.28125f, 82.0f, 85.890625f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"2026년 8월 20일 - 2025년 12월 20일", 21.0f, 118.0f, 643.0f, 16.0f, 11.0f, 8684679u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Hint text • here", 199.17188f, 85.5f, 91.9375f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}};
    spec.interactive             = true;
    spec.groupWidth              = 0.5f;
    spec.cornerRadius            = 10000.0f;
    spec.xAxis                   = {{{"0", 0.0f}, {"3", 3.0f}, {"6", 6.0f}, {"9", 9.0f}, {"12", 12.0f}, {"15", 15.0f}, {"18", 18.0f}, {"21", 21.0f}, {"24", 24.0f}, {"27", 27.0f}}, true};
    spec.yAxis                   = {{{"", 0.0f}, {"30", 30.0f}, {"60", 60.0f}, {"90", 90.0f}, {"120", 120.0f}}, true};
    spec.texts[4].color          = 0xB88400u;
    spec.texts[4].background     = 0xFFEAB0u;
    spec.texts[4].darkColor      = 0xCC9300u;
    spec.texts[4].darkBackground = 0x594000u;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "custom-04";
    spec.category       = "Custom";
    spec.title          = "Data Update 2";
    spec.kind           = ChartKind::LINE;
    spec.minimum        = 0.0f;
    spec.maximum        = 120.0f;
    spec.values         = {4.954561f, 15.300069f, 99.61677f, 40.620713f, 50.657579f, 49.480453f, 8.694273f, 62.866941f, 87.307099f, 42.508573f, 85.261488f, 46.430041f, 75.396948f, 51.174554f, 60.182613f, 40.69273f, 93.939472f, 61.898148f, 71.729252f, 46.889575f, 84.909979f, 1.8398491f, 43.358196f, 62.55144f, 64.357853f, 16.678669f, 28.155864f, 92.616598f, 0.18432785f, 15.056584f};
    spec.secondary      = {4.954561f, 15.300069f, 99.61677f, 40.620713f, 50.657579f, 49.480453f, 8.694273f, 62.866941f, 87.307099f, 42.508573f, 85.261488f, 46.430041f, 75.396948f, 51.174554f, 60.182613f, 40.69273f, 93.939472f, 61.898148f, 71.729252f, 46.889575f, 84.909979f, 1.8398491f, 43.358196f, 62.55144f, 64.357853f, 16.678669f, 28.155864f, 92.616598f, 0.18432785f, 15.056584f};
    spec.subtitle       = "Subtitle";
    spec.description    = "2월 6일 오후 5시 - 6시";
    spec.hint           = "Hint text · here";
    spec.unit           = "kWh";
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::REPORT;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "custom-04: Explicit BAR / REPORT; 30 points";
    spec.secondaryChart = true;
    spec.secondaryKind  = ChartKind::LINE;
    spec.chartWidth     = 304.0f;
    spec.chartHeight    = 123.0f;
    spec.frames         = {{190.5f, 132.0f, 304.0f, 123.0f}, {190.5f, 340.0f, 304.0f, 123.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {{"40", 190.5f, 63.0f, 14.421875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"40 kWh", 190.5f, 82.0f, 70.890625f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"12:30 - 12:34", 190.5f, 118.0f, 304.0f, 14.0f, 11.0f, 8684679u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Humidity", 190.5f, 271.0f, 48.890625f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"40 %", 190.5f, 290.0f, 50.75f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"12:30 - 12:34", 190.5f, 326.0f, 304.0f, 14.0f, 11.0f, 8684679u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}};
    spec.interactive    = true;
    spec.companions.push_back({{}, {}, {4.95456104f, 15.3000686f, 99.6167696f, 40.6207133f, 50.6575789f, 49.4804527f, 8.69427298f, 62.866941f, 87.3070988f, 42.5085734f, 85.2614883f, 46.4300412f, 75.3969479f, 51.1745542f, 60.1826132f, 40.6927298f, 93.9394719f, 61.8981481f, 71.7292524f, 46.8895748f, 84.9099794f, 1.83984911f, 43.3581962f, 62.5514403f, 64.3578532f, 16.6786694f, 28.1558642f, 92.6165981f, 0.18432785f, 15.0565844f}, {}, ChartKind::LINE, 0.0f, 120.0f});
    spec.xAxis               = {{{"11:35", 0.0f}, {"11:50", 10.0f}, {"12:05\nMM", 20.0f}, {"12:20", 29.0f}}, true};
    spec.yAxis               = {{{"", 0.0f}, {"60", 60.0f}, {"120", 120.0f}}, true};
    spec.companions[0].xAxis = {{{"11:35", 0.0f}, {"11:50", 10.0f}, {"12:05\nMM", 20.0f}, {"12:20", 29.0f}}, true};
    spec.companions[0].yAxis = {{{"", 0.0f}, {"60", 60.0f}, {"120", 120.0f}}, true};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 2.5f, 0x387AFFu, 1.0f});
    spec.companions[0].line = {0xBFD9FFu, 1.0f, 2.5f, 2.5f, 0x387AFFu, 1.0f};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id                      = "custom-05";
    spec.pageSizes               = {31, 31, 31, 27};
    spec.category                = "Custom";
    spec.title                   = "Scale Update Test";
    spec.kind                    = ChartKind::BAR;
    spec.minimum                 = 0.0f;
    spec.maximum                 = 120.0f;
    spec.values                  = {5.3746571f, 34.444444f, 72.053326f, 41.658093f, 40.789609f, 85.497257f, 21.460048f, 1.7644033f, 31.348594f, 78.113855f, 50.70216f, 42.940672f, 44.952846f, 20.936214f, 53.606824f, 55.310357f, 45.799897f, 96.680384f, 42.519719f, 58.626543f, 5.4946845f, 39.914266f, 92.749486f, 13.383059f, 80.827332f, 81.502058f, 53.678841f, 98.59225f, 58.217593f, 59.715364f, 26.542353f, 56.229424f, 64.82596f, 98.010974f, 8.8708848f, 2.3439643f, 46.331447f, 49.475309f, 5.6027092f, 4.8371056f, 11.376029f, 7.9355281f, 6.8612826f, 27.906379f, 42.675754f, 85.737311f, 32.399691f, 43.156722f, 74.798525f, 58.1893f, 42.711763f, 7.3782579f, 38.608539f, 74.674211f, 16.809842f, 6.9907407f, 72.377401f, 36.426612f, 96.669239f, 69.154664f, 199.5619f, 140.97737f, 198.33933f, 139.54904f, 173.24846f, 193.26475f, 189.72136f, 126.81584f, 187.26423f, 183.41221f, 135.01286f, 113.67112f, 153.9549f, 131.17284f, 105.81876f, 134.68278f, 148.62912f, 197.04047f, 158.92918f, 120.71502f, 120.66958f, 160.02743f, 180.76389f, 110.03944f, 171.31087f, 162.10905f, 198.48337f, 126.11283f, 198.08385f, 119.33471f, 157.76663f, 122.0216f, 105.92678f, 199.60562f, 167.25566f, 191.59294f, 184.96313f, 167.11934f, 109.6665f, 147.1725f, 154.94599f, 193.4808f, 119.56704f, 164.06893f, 176.36917f, 135.48011f, 127.8215f, 191.66495f, 128.24503f, 179.53704f, 172.70147f, 131.19513f, 152.54887f, 152.81207f, 177.66375f, 180.19033f, 165.33008f, 100.98422f, 195.79475f, 143.58882f};
    spec.secondary               = {50.06f, 26.4f, 43.83f, 14.31f, 57.84f, 64.82f, 63.08f, 2.53f, 31.27f, 29.55f, 20.86f, 35.08f, 39.74f, 33.15f, 7.65f, 54.41f, 20.86f, 60.02f, 11.27f, 12.88f, 35.56f, 63.56f, 16.4f, 10.75f};
    spec.subtitle                = "Subtitle";
    spec.description             = "2월 6일 오후 5시 - 6시";
    spec.hint                    = "Hint text · here";
    spec.unit                    = "kWh";
    spec.axis                    = AxisPreset::DEFAULT;
    spec.layout                  = LayoutPreset::REPORT;
    spec.action                  = Action::Y_RANGE;
    spec.navigation              = Navigation::SWIPE;
    spec.numberMode              = NumberMode::NONE;
    spec.expected                = "custom-05: Y range 80/100; X unchanged";
    spec.secondaryChart          = false;
    spec.chartWidth              = 300.0f;
    spec.chartHeight             = 200.0f;
    spec.pageSize                = 31;
    spec.frames                  = {{192.5f, 173.5f, 300.0f, 200.0f}};
    spec.referenceWidth          = 685.0f;
    spec.texts                   = {{"Subtitle", 192.5f, 102.5f, 41.96875f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"120.5 kWh", 192.5f, 121.5f, 108.89062f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"2 월 6 일 오후 5 시 - 6 시", 192.5f, 157.5f, 300.0f, 16.0f, 11.0f, 8684679u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Hint text • here", 305.39062f, 125.0f, 91.9375f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}};
    spec.interactive             = true;
    spec.groupWidth              = 0.5f;
    spec.cornerRadius            = 10000.0f;
    spec.xAxis                   = {{{"0", 0.0f}, {"4", 4.0f}, {"8", 8.0f}, {"12", 12.0f}, {"16", 16.0f}, {"20", 20.0f}, {"24", 24.0f}, {"28", 28.0f}}, true};
    spec.yAxis                   = {{{"", 0.0f}, {"30", 30.0f}, {"60", 60.0f}, {"90", 90.0f}, {"120", 120.0f}}, true};
    spec.texts[3].color          = 0x4D4D52u;
    spec.texts[3].background     = 0xE1E2E8u;
    spec.texts[3].darkColor      = 0xCCCCCEu;
    spec.texts[3].darkBackground = 0x545457u;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id                      = "custom-06";
    spec.tabValues               = {{34.4898834f, 32.69547325f, 20.92163923f, 11.51406036f, 24.22582305f, 30.66186557f, 65.39008916f, 3.7191358f, 6.14283265f, 29.45130316f, 4.50874486f, 80.6978738f, 37.03103567f, 59.92798354f, 87.66032236f, 21.46262003f, 3.31018519f, 60.36351166f, 16.07938957f, 67.98868313f, 32.14077503f, 54.21467764f, 87.2968107f, 36.32544582f}, {std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN()}, {615.21090535f, 767.40740741f, 237.50514403f, 766.24485597f, 338.8117284f, 651.50205761f, 578.3899177f, 237.99382716f, 659.94341564f, 251.64609053f, 664.9537037f, 462.82921811f, 586.01337449f, 619.69135802f, 260.15946502f, 581.49176955f, 302.20679012f, 251.93415638f, 438.08127572f, 512.5f, 638.15329218f, 355.781893f, 250.57098765f, 418.81687243f, 534.5936214f, 316.41975309f, 393.92489712f, 574.51646091f, 310.0462963f, 763.47736626f, 675.55041152f}};
    spec.category                = "Custom";
    spec.title                   = "Multi Tab Chart Demo";
    spec.kind                    = ChartKind::BAR;
    spec.minimum                 = 0.0f;
    spec.maximum                 = 120.0f;
    spec.values                  = {34.489883f, 32.695473f, 20.921639f, 11.51406f, 24.225823f, 30.661866f, 65.390089f, 3.7191358f, 6.1428326f, 29.451303f, 4.5087449f, 80.697874f, 37.031036f, 59.927984f, 87.660322f, 21.46262f, 3.3101852f, 60.363512f, 16.07939f, 67.988683f, 32.140775f, 54.214678f, 87.296811f, 36.325446f};
    spec.secondary               = {std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN()};
    spec.subtitle                = "Subtitle";
    spec.description             = "2월 6일 오후 5시 - 6시";
    spec.hint                    = "Hint text · here";
    spec.unit                    = "kWh";
    spec.axis                    = AxisPreset::DEFAULT;
    spec.layout                  = LayoutPreset::REPORT;
    spec.action                  = Action::TABS;
    spec.navigation              = Navigation::NONE;
    spec.numberMode              = NumberMode::NONE;
    spec.expected                = "custom-06: Explicit BAR / REPORT; 24 points";
    spec.secondaryChart          = false;
    spec.secondaryKind           = ChartKind::BAR;
    spec.chartWidth              = 643.0f;
    spec.chartHeight             = 150.0f;
    spec.frames                  = {{21.0f, 194.0f, 643.0f, 150.0f}, {37.075012f, 194.0f, 610.84998f, 150.0f}, {37.075012f, 194.0f, 610.84998f, 150.0f}};
    spec.referenceWidth          = 685.0f;
    spec.texts                   = {{"Bar Chart", 21.0f, 123.0f, 52.125f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"79.86659582238644 Unit", 21.0f, 142.0f, 135.28125f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"2026 년 8 월 20 일 - 2025 년 12 월 20 일", 21.0f, 178.0f, 643.0f, 16.0f, 11.0f, 8684679u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Hint text • here", 160.28125f, 145.5f, 91.9375f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}};
    spec.interactive             = true;
    spec.groupWidth              = 0.5f;
    spec.cornerRadius            = 10000.0f;
    spec.xAxis                   = {{{"0", 0.0f}, {"3", 3.0f}, {"6", 6.0f}, {"9", 9.0f}, {"12", 12.0f}, {"15", 15.0f}, {"18", 18.0f}, {"21", 21.0f}}, true};
    spec.yAxis                   = {{{"", 0.0f}, {"30", 30.0f}, {"60", 60.0f}, {"90", 90.0f}, {"120", 120.0f}}, true};
    spec.texts[3].color          = 0xB88400u;
    spec.texts[3].background     = 0xFFEAB0u;
    spec.texts[3].darkColor      = 0xCC9300u;
    spec.texts[3].darkBackground = 0x594000u;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id                      = "responsive-01";
    spec.pageSizes               = {30, 30};
    spec.legendItems             = {{"Green", 29.0f, 395.5f, 43.421875f, 5025616u, false}, {"Blue", 80.421875f, 395.5f, 34.5f, 2201331u, false}, {"Yellow", 122.921875f, 395.5f, 44.921875f, 16761095u, false}};
    spec.category                = "Responsive";
    spec.title                   = "Responsive Example (Portrait or Landscape)";
    spec.kind                    = ChartKind::BAR;
    spec.pageSize                = 30;
    spec.minimum                 = 0.0f;
    spec.maximum                 = 12.0f;
    spec.values                  = {3.8575103f, 5.4579047f, 3.9615912f, 1.3438786f, 4.7652606f, 7.6825274f, 7.6265432f, 0.64669067f, 2.4219822f, 6.0388374f, 6.4355281f, 1.5132888f, 9.9140946f, 5.4651063f, 8.2897805f, 2.5856481f, 1.0687586f, 6.0847908f, 7.3868313f, 6.5798182f, 8.231653f, 7.6509774f, 5.3316187f, 8.0637003f, 6.7114198f, 0.65749314f, 8.9142661f, 7.9014918f, 5.890775f, 4.1166838f, 45.545267f, 43.647977f, 70.042867f, 0.03858025f, 94.128944f, 9.1040809f, 75.828189f, 43.683985f, 91.683813f, 6.247428f, 25.646433f, 51.115398f, 24.62963f, 73.349623f, 20.732167f, 64.308128f, 20.126886f, 33.867455f, 58.616255f, 99.311557f, 23.854595f, 40.887346f, 44.236968f, 24.02692f, 44.454733f, 88.236454f, 67.717764f, 2.651749f, 64.643347f, 88.26046f};
    spec.secondary               = {50.11f, 38.19f, 2.84f, 19.12f, 14.48f, 32.02f, 54.11f, 56.63f, 32.4f, 37.02f, 14.84f, 59.11f, 36.74f, 26.89f, 37.34f, 50.0f, 40.77f, 61.99f, 50.09f, 63.16f, 60.4f, 6.73f, 38.53f, 20.07f, 51.81f, 1.58f, 28.93f, 6.82f, 7.97f, 41.48f, 57.01f, 58.03f, 48.32f, 18.36f, 28.55f, 1.26f, 27.36f, 42.07f, 46.14f, 61.71f, 24.08f, 25.04f, 36.82f, 18.15f, 35.52f, 2.26f, 2.3f, 47.47f, 8.79f, 9.63f, 18.93f, 41.16f, 62.35f, 49.58f, 41.7f, 43.74f, 48.43f, 39.63f, 36.87f, 58.92f};
    spec.subtitle                = "Very Long Long Long Long Long Long Long Long Long Long Long Long Long Long Text";
    spec.description             = "Very Long Long Long Long Long Long Long Long Long Description";
    spec.hint                    = "Hint text · here";
    spec.unit                    = "kWh";
    spec.slots                   = 2;
    spec.axis                    = AxisPreset::DEFAULT;
    spec.layout                  = LayoutPreset::RESPONSIVE;
    spec.action                  = Action::DATA;
    spec.navigation              = Navigation::SWIPE;
    spec.numberMode              = NumberMode::NONE;
    spec.expected                = "responsive-01: Explicit BAR / RESPONSIVE; 60 points";
    spec.secondaryChart          = false;
    spec.chartWidth              = 643.0f;
    spec.chartHeight             = 215.0f;
    spec.frames                  = {{21.0f, 168.5f, 643.0f, 215.0f}};
    spec.referenceWidth          = 685.0f;
    spec.texts                   = {{"Very Long Long Long Long Long Long Long Long Long Long Long Long Long Long Long Long Long Long Long Long Long Text", 21.0f, 78.5f, 643.0f, 16.0f, 12.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"100 Unit", 21.0f, 97.5f, 82.28125f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"100 kWh", 109.28125f, 97.5f, 85.890625f, 29.0f, 23.0f, 0u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"SubValue SubUnit", 21.0f, 133.5f, 643.0f, 17.0f, 23.0f, 2434344u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Very Long Long Long Long Long Long Long Long Long Long Long Long Long Description", 21.0f, 154.5f, 643.0f, 14.0f, 11.0f, 8684679u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, false}, {"Hint text • here", 199.17188f, 101.0f, 91.9375f, 22.0f, 12.0f, 11237120u, 0xE1E2E8u, 0x414147u, 0xCCCCD1u, true}};
    spec.interactive             = true;
    spec.groupWidth              = 0.5f;
    spec.cornerRadius            = 10000.0f;
    spec.xAxis                   = {{{"0", 0.0f}, {"3", 3.0f}, {"6", 6.0f}, {"9", 9.0f}, {"12", 12.0f}, {"15", 15.0f}, {"18", 18.0f}, {"21", 21.0f}, {"24", 24.0f}, {"27", 27.0f}}, true};
    spec.yAxis                   = {{{"", 0.0f}, {"3", 3.0f}, {"6", 6.0f}, {"9", 9.0f}, {"12", 12.0f}}, true};
    spec.texts[5].color          = 0xB88400u;
    spec.texts[5].background     = 0xFFEAB0u;
    spec.texts[5].darkColor      = 0xCC9300u;
    spec.texts[5].darkBackground = 0x594000u;
    spec.texts[0].fontSize       = 12.0f;
    spec.texts[1].fontSize       = 23.0f;
    spec.texts[2].fontSize       = 23.0f;
    spec.texts[3].fontSize       = 13.0f;
    spec.texts[4].fontSize       = 11.0f;
    spec.texts[5].fontSize       = 12.0f;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "interaction-01";
    spec.category       = "Interaction";
    spec.title          = "Number Revolving (100-220)";
    spec.kind           = ChartKind::NONE;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {};
    spec.secondary      = {};
    spec.from           = 100.0f;
    spec.to             = 220.0f;
    spec.fractionDigits = 0;
    spec.metric         = "100";
    spec.unit           = "";
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::NUMBER;
    spec.action         = Action::NUMBER;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::REVOLVE;
    spec.expected       = "interaction-01: Target 220";
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.interactive    = true;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "interaction-02";
    spec.category       = "Interaction";
    spec.title          = "Number Revolving (32-25)";
    spec.kind           = ChartKind::NONE;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {};
    spec.secondary      = {};
    spec.from           = 32.0f;
    spec.to             = 25.0f;
    spec.fractionDigits = 0;
    spec.metric         = "32";
    spec.unit           = "";
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::NUMBER;
    spec.action         = Action::NUMBER;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::REVOLVE;
    spec.expected       = "interaction-02: Target 25";
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.interactive    = true;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "interaction-03";
    spec.category       = "Interaction";
    spec.title          = "Number Revolving (1200-1800)";
    spec.kind           = ChartKind::NONE;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {};
    spec.secondary      = {};
    spec.from           = 1200.0f;
    spec.to             = 1800.0f;
    spec.fractionDigits = 0;
    spec.metric         = "1200";
    spec.unit           = "";
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::NUMBER;
    spec.action         = Action::NUMBER;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::REVOLVE;
    spec.expected       = "interaction-03: Target 1800";
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.interactive    = true;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "interaction-04";
    spec.category       = "Interaction";
    spec.title          = "Number Revolving (100-3500)";
    spec.kind           = ChartKind::NONE;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {};
    spec.secondary      = {};
    spec.from           = 100.0f;
    spec.to             = 3500.0f;
    spec.fractionDigits = 0;
    spec.metric         = "100";
    spec.unit           = "";
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::NUMBER;
    spec.action         = Action::NUMBER;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::REVOLVE;
    spec.expected       = "interaction-04: Target 3500";
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.interactive    = true;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "interaction-05";
    spec.category       = "Interaction";
    spec.title          = "Number Revolving (1000-0)";
    spec.kind           = ChartKind::NONE;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {};
    spec.secondary      = {};
    spec.from           = 1000.0f;
    spec.to             = 0.0f;
    spec.fractionDigits = 0;
    spec.metric         = "1000";
    spec.unit           = "";
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::NUMBER;
    spec.action         = Action::NUMBER;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::REVOLVE;
    spec.expected       = "interaction-05: Target 0";
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.interactive    = true;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "interaction-06";
    spec.category       = "Interaction";
    spec.title          = "Number Counting (Steps)";
    spec.kind           = ChartKind::NONE;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {};
    spec.secondary      = {};
    spec.from           = 0.0f;
    spec.to             = 10000.0f;
    spec.fractionDigits = 0;
    spec.metric         = "0";
    spec.unit           = "";
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::NUMBER;
    spec.action         = Action::NUMBER;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::COUNT;
    spec.expected       = "interaction-06: Target 10000";
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.interactive    = true;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "interaction-07";
    spec.category       = "Interaction";
    spec.title          = "Number Counting (Weight)";
    spec.kind           = ChartKind::NONE;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {};
    spec.secondary      = {};
    spec.from           = 18.2f;
    spec.to             = 26.5f;
    spec.fractionDigits = 1;
    spec.metric         = "18.2";
    spec.unit           = "";
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::NUMBER;
    spec.action         = Action::NUMBER;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::COUNT;
    spec.expected       = "interaction-07: Target 26.5";
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.interactive    = true;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "interaction-08";
    spec.category       = "Interaction";
    spec.title          = "Number Counting";
    spec.kind           = ChartKind::NONE;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {};
    spec.secondary      = {};
    spec.from           = 900.0f;
    spec.to             = 1024.5f;
    spec.fractionDigits = 1;
    spec.metric         = "900";
    spec.unit           = "";
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::NUMBER;
    spec.action         = Action::NUMBER;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::COUNT;
    spec.expected       = "interaction-08: Target 1024.5";
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.interactive    = true;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "interaction-09";
    spec.category       = "Interaction";
    spec.title          = "Cross Fading Number";
    spec.kind           = ChartKind::NONE;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {};
    spec.secondary      = {};
    spec.from           = 1024.0f;
    spec.to             = 24500.0f;
    spec.fractionDigits = 0;
    spec.metric         = "1024";
    spec.unit           = "";
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::NUMBER;
    spec.action         = Action::NUMBER;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::FADE;
    spec.expected       = "interaction-09: Target 24500";
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.interactive    = true;
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "a11y-01";
    spec.category       = "Accessibility";
    spec.title          = "A11y: Bar Chart";
    spec.kind           = ChartKind::BAR;
    spec.pageSize       = 7;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {10.0f, 15.0f, 10.0f, 30.0f, 40.0f, 60.0f, 75.0f};
    spec.secondary      = {50.36f, 32.15f, 57.9f, 43.18f, 57.69f, 63.03f, 9.29f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "a11y-01: Explicit BAR / CHART; 7 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.interactive    = true;
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"25", 25.0f}, {"50", 50.0f}, {"75", 75.0f}, {"100", 100.0f}}, true};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "a11y-02";
    spec.category       = "Accessibility";
    spec.title          = "A11y: Stacked Bar Chart";
    spec.kind           = ChartKind::STACKED;
    spec.pageSize       = 7;
    spec.minimum        = 0.0f;
    spec.maximum        = 4.0f;
    spec.values         = {1.8f, 1.8f, 1.6f, 1.0f, 0.8f, 0.1f, 1.0f};
    spec.secondary      = {0.6f, 0.6f, 0.5f, 0.4f, 0.8f, 0.1f, 2.0f};
    spec.legend         = true;
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "a11y-02: Explicit STACKED / CHART; 7 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.interactive    = true;
    spec.groupWidth     = 0.3f;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}}, true};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "a11y-03";
    spec.category       = "Accessibility";
    spec.title          = "A11y: Line Chart";
    spec.kind           = ChartKind::LINE;
    spec.pageSize       = 7;
    spec.minimum        = 15.0f;
    spec.maximum        = 75.0f;
    spec.values         = {30.0f, 20.0f, 25.0f, 40.0f, 45.0f, 50.0f, 60.0f};
    spec.secondary      = {50.41f, 43.94f, 16.91f, 47.99f, 14.33f, 30.24f, 0.32f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "a11y-03: Explicit BAR / CHART; 7 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.interactive    = true;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}}, true};
    spec.yAxis          = {{{"15", 15.0f}, {"30", 30.0f}, {"45", 45.0f}, {"60", 60.0f}, {"75", 75.0f}}, true};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 4.0f, 0x387AFFu, 1.0f});
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "a11y-04";
    spec.category       = "Accessibility";
    spec.title          = "A11y: Combo Chart (Bar + Line)";
    spec.kind           = ChartKind::COMBO;
    spec.pageSize       = 7;
    spec.minimum        = 0.0f;
    spec.maximum        = 100.0f;
    spec.values         = {10.0f, 15.0f, 10.0f, 30.0f, 40.0f, 60.0f, 75.0f};
    spec.secondary      = {30.0f, 20.0f, 25.0f, 40.0f, 45.0f, 50.0f, 60.0f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::NONE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "a11y-04: Explicit COMBO / CHART; 7 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.interactive    = true;
    spec.groupWidth     = 0.4f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}, {"6", 6.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"25", 25.0f}, {"50", 50.0f}, {"75", 75.0f}, {"100", 100.0f}}, true};
    spec.lines.push_back({0xBFD9FFu, 1.0f, 2.5f, 0.0f, 0x387AFFu, 1.0f});
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "a11y-05";
    spec.pageSizes      = {6, 6};
    spec.category       = "Accessibility";
    spec.title          = "A11y: Swipeable Chart (Page Navigation)";
    spec.kind           = ChartKind::BAR;
    spec.pageSize       = 6;
    spec.minimum        = 0.0f;
    spec.maximum        = 80.0f;
    spec.values         = {10.0f, 15.0f, 10.0f, 30.0f, 40.0f, 60.0f, 75.0f, 70.0f, 90.0f, 80.0f, 80.0f, 75.0f};
    spec.secondary      = {50.46f, 55.74f, 40.92f, 52.8f, 35.97f, 62.44f, 56.36f, 45.33f, 40.33f, 24.3f, 37.73f, 32.29f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::SWIPE;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "a11y-05: Explicit BAR / CHART; 12 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.interactive    = true;
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"20", 20.0f}, {"40", 40.0f}, {"60", 60.0f}, {"80", 80.0f}}, true};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id             = "a11y-06";
    spec.pageSizes      = {6, 6, 6};
    spec.category       = "Accessibility";
    spec.title          = "A11y: Scrollable Chart (Drag Navigation)";
    spec.kind           = ChartKind::BAR;
    spec.pageSize       = 6;
    spec.minimum        = 0.0f;
    spec.maximum        = 80.0f;
    spec.values         = {10.0f, 15.0f, 10.0f, 30.0f, 40.0f, 60.0f, 75.0f, 70.0f, 90.0f, 80.0f, 80.0f, 75.0f, 90.0f, 100.0f, 130.0f, 140.0f, 135.0f, 140.0f};
    spec.secondary      = {50.49f, 61.63f, 20.43f, 55.21f, 14.29f, 46.04f, 19.37f, 39.88f, 8.4f, 28.03f, 2.23f, 11.8f};
    spec.axis           = AxisPreset::DEFAULT;
    spec.layout         = LayoutPreset::CHART;
    spec.action         = Action::DATA;
    spec.navigation     = Navigation::SCROLL;
    spec.numberMode     = NumberMode::NONE;
    spec.expected       = "a11y-06: Explicit BAR / CHART; 12 points";
    spec.secondaryChart = false;
    spec.chartWidth     = 643.0f;
    spec.chartHeight    = 150.0f;
    spec.frames         = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth = 685.0f;
    spec.texts          = {};
    spec.interactive    = true;
    spec.groupWidth     = 0.5f;
    spec.cornerRadius   = 10000.0f;
    spec.xAxis          = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}}, true};
    spec.yAxis          = {{{"", 0.0f}, {"20", 20.0f}, {"40", 40.0f}, {"60", 60.0f}, {"80", 80.0f}}, true};
    cases.push_back(std::move(spec));
  }
  {
    ReferenceCase spec;
    spec.id                 = "a11y-07";
    spec.pageSizes          = {6, 6};
    spec.category           = "Accessibility";
    spec.title              = "A11y: Custom Slide Announcement (onSlideAnnounce)";
    spec.customAnnouncement = true;
    spec.kind               = ChartKind::BAR;
    spec.pageSize           = 6;
    spec.minimum            = 0.0f;
    spec.maximum            = 80.0f;
    spec.values             = {10.0f, 15.0f, 10.0f, 30.0f, 40.0f, 60.0f, 75.0f, 70.0f, 90.0f, 80.0f, 80.0f, 75.0f};
    spec.secondary          = {50.51f, 2.53f, 64.94f, 57.62f, 57.61f, 29.64f, 47.39f, 34.43f, 41.47f, 31.76f, 31.72f, 56.31f};
    spec.axis               = AxisPreset::DEFAULT;
    spec.layout             = LayoutPreset::CHART;
    spec.action             = Action::DATA;
    spec.navigation         = Navigation::SWIPE;
    spec.numberMode         = NumberMode::NONE;
    spec.expected           = "a11y-07: Explicit BAR / CHART; 12 points";
    spec.secondaryChart     = false;
    spec.chartWidth         = 643.0f;
    spec.chartHeight        = 150.0f;
    spec.frames             = {{21.0f, 163.0f, 643.0f, 150.0f}};
    spec.referenceWidth     = 685.0f;
    spec.texts              = {};
    spec.interactive        = true;
    spec.groupWidth         = 0.5f;
    spec.cornerRadius       = 10000.0f;
    spec.xAxis              = {{{"0", 0.0f}, {"1", 1.0f}, {"2", 2.0f}, {"3", 3.0f}, {"4", 4.0f}, {"5", 5.0f}}, true};
    spec.yAxis              = {{{"", 0.0f}, {"20", 20.0f}, {"40", 40.0f}, {"60", 60.0f}, {"80", 80.0f}}, true};
    cases.push_back(std::move(spec));
  }
}

} // unnamed namespace

std::vector<ReferenceCase> BuildReferenceCases()
{
  std::vector<ReferenceCase> cases;
  cases.reserve(141u);
  AppendReferenceCases0To49(cases);
  AppendReferenceCases50To99(cases);
  AppendReferenceCases100To136(cases);
  AppendAdditionalCases(cases);
  for(auto& spec : cases)
  {
    if(!spec.logarithmic)
    {
      continue;
    }
    // Web log axes can report zero as a bound. Native viewport restoration
    // requires positive bounds; retain every positive fixture and guide band.
    if(spec.minimum <= 0.0f)
    {
      float lowest  = std::numeric_limits<float>::infinity();
      auto  include = [&lowest](float value)
      {
        if(std::isfinite(value) && value > 0.0f)
        {
          lowest = std::min(lowest, value);
        }
      };
      for(float value : spec.values)
      {
        include(value);
      }
      for(float value : spec.rangeLows)
      {
        include(value);
      }
      for(const auto& annotation : spec.annotations)
      {
        include(annotation.y);
      }
      spec.minimum = std::isfinite(lowest) ? std::pow(10.0f, std::floor(std::log10(lowest))) : 1.0f;
      spec.maximum = std::max(spec.maximum, spec.minimum * 10.0f);
    }
    auto& ticks = spec.yAxis.ticks;
    ticks.erase(std::remove_if(ticks.begin(), ticks.end(), [&spec](const auto& tick)
    { return tick.value < spec.minimum; }),
                ticks.end());
  }
  return cases;
}

float CardHeight(const ReferenceCase& spec)
{
  if(!spec.webReference)
  {
    return 660.0f;
  }
  float bottom = 0.0f;
  for(const auto& frame : spec.frames)
  {
    bottom = std::max(bottom, frame.y + frame.height);
  }
  for(const auto& text : spec.texts)
  {
    bottom = std::max(bottom, text.y + text.height + 6.0f);
  }
  return std::max(516.0f, bottom + 110.0f);
}

std::vector<float> BuildValues(const ReferenceCase& spec, const CaseState& state, uint32_t seriesIndex)
{
  auto result = seriesIndex == 0u ? spec.values : seriesIndex == 1u ? spec.secondary
                                                                    : spec.tertiary;
  if(spec.action == Action::TABS && !spec.tabValues.empty())
  {
    return spec.tabValues[static_cast<size_t>(state.period) % spec.tabValues.size()];
  }
  if(spec.action == Action::TABS && state.period % 3 == 1)
  {
    return std::vector<float>(20u, std::numeric_limits<float>::quiet_NaN());
  }
  if(spec.action == Action::TABS && state.period % 3 == 2)
  {
    result.resize(31u, 260.0f);
    for(size_t i = 0; i < result.size(); ++i)
    {
      result[i] = 200.0f + static_cast<float>((i * 71u + 23u) % 600u);
    }
    return result;
  }
  if(state.revision != 0 && spec.action != Action::X_RANGE && spec.action != Action::Y_RANGE && spec.action != Action::SHOW_HIDE)
  {
    for(size_t i = 0; i < result.size(); ++i)
    {
      if(std::isfinite(result[i]))
      {
        const float fraction = static_cast<float>((i * 17u + state.revision * 31u) % 101u) / 100.0f;
        result[i]            = spec.minimum + (spec.maximum - spec.minimum) * fraction;
        if(spec.logarithmic)
        {
          result[i] = std::max(1.0f, result[i]);
        }
      }
    }
  }
  if(!spec.webReference && spec.kind == ChartKind::PIE)
  {
    float total = 0.0f;
    for(auto& value : result)
    {
      // Keep the authored slice indices stable even when an update reaches zero.
      value = std::max(1.0f, value);
      total += value;
    }
    if(spec.unit == "%" && total > 0.0f)
    {
      for(auto& value : result)
      {
        value = value / total * 100.0f;
      }
    }
  }
  return result;
}

uint32_t StatusColor(float value, int levels)
{
  if(levels == 3)
  {
    return value < 100.0f / 3.0f ? 0x2CBEFCu : value < 200.0f / 3.0f ? 0x12AB00u
                                                                     : 0xF7B500u;
  }
  return value <= 20.0f ? 0x2CBEFCu : value <= 40.0f ? 0x12AB00u
                                    : value <= 60.0f ? 0xF7B500u
                                    : value <= 80.0f ? 0xFA6400u
                                                     : 0xE62D2Du;
}

std::string FormatNumber(double value, int fractionDigits)
{
  std::ostringstream stream;
  stream.imbue(std::locale::classic());
  stream << std::fixed << std::setprecision(fractionDigits) << value;
  return stream.str();
}

bool ParseNumber(const std::string& text, double& value)
{
  if(text.empty())
  {
    return false;
  }
  try
  {
    size_t       consumed = 0u;
    const double parsed   = std::stod(text, &consumed);
    if(consumed != text.size() || !std::isfinite(parsed) || std::abs(parsed) > 1.0e12)
    {
      return false;
    }
    value = parsed;
    return true;
  }
  catch(const std::exception&)
  {
    return false;
  }
}

int PageCount(const ReferenceCase& spec)
{
  if(!spec.pageSizes.empty())
  {
    return static_cast<int>(spec.pageSizes.size());
  }
  return std::max(1, static_cast<int>((spec.values.size() + spec.pageSize - 1) / spec.pageSize));
}

int PageStart(const ReferenceCase& spec, int page)
{
  int first = 0;
  for(int i = 0; i < page; ++i)
  {
    first += PageSize(spec, i);
  }
  return first;
}

int PageSize(const ReferenceCase& spec, int page)
{
  return spec.pageSizes.empty() ? spec.pageSize : spec.pageSizes[static_cast<size_t>(page)];
}

std::string PageAnnouncement(const ReferenceCase& spec, int page)
{
  const int first = PageStart(spec, page);
  const int last  = std::min(static_cast<int>(spec.values.size()), first + PageSize(spec, page));
  return spec.customAnnouncement ? "Now viewing items " + std::to_string(first + 1) + " through " + std::to_string(last) + " on page " + std::to_string(page + 1) + " of " + std::to_string(PageCount(spec)) + "." : "Page " + std::to_string(page + 1) + " of " + std::to_string(PageCount(spec));
}
} //namespace ChartStandard
