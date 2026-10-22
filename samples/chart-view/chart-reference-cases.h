#ifndef DALI_CHART_SAMPLE_CHART_REFERENCE_CASES_H
#define DALI_CHART_SAMPLE_CHART_REFERENCE_CASES_H
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

#include <cstdint>
#include <string>
#include <vector>

namespace ChartStandard
{
enum class ChartKind
{
  BAR,
  STACKED,
  LINE,
  COMBO,
  NONE,
  PIE,
  GAUGE,
  SCATTER
};
enum class AxisPreset
{
  DEFAULT,
  WEEK_UNIT,
  WEEK_PERIOD,
  WEEK_SIMPLE,
  TIME_UNIT,
  TIME_PERIOD,
  CENTER,
  EDGE
};
enum class LayoutPreset
{
  CHART,
  REPORT,
  SPARKLINE,
  INSIGHT,
  DATA_INFO,
  SPLIT_INFO,
  RESPONSIVE,
  NUMBER
};
enum class Action
{
  NONE,
  DATA,
  X_RANGE,
  Y_RANGE,
  DATA_X,
  SHOW_HIDE,
  RECREATE,
  TABS,
  NUMBER
};
enum class Navigation
{
  NONE,
  SCROLL,
  SWIPE
};
enum class NumberMode
{
  NONE,
  REVOLVE,
  COUNT,
  FADE
};
enum class AnnotationKind
{
  MARKER,
  TEXT,
  IMAGE,
  LINE,
  AREA
};

/**
 * @brief Authored native text placement captured from the corresponding reference card.
 */
struct TextSpec
{
  std::string text;
  float       x;
  float       y;
  float       width;
  float       height;
  float       fontSize;
  uint32_t    color;
  uint32_t    background{0xE1E2E8u};
  uint32_t    darkBackground{0x414147u};
  uint32_t    darkColor{0xCCCCD1u};
  bool        tag;
};

/**
 * @brief Chart slot geometry relative to the reference card.
 */
struct ChartFrame
{
  float x;
  float y;
  float width;
  float height;
};

/**
 * @brief Informational dot or image legend in its authored slot.
 */
struct LegendSpec
{
  std::string label;
  float       x;
  float       y;
  float       width;
  uint32_t    color;
  bool        image;
};

/**
 * @brief Authored tick text from the reference chart engine.
 */
struct TickSpec
{
  std::string text;
  float       value;
};

/**
 * @brief Visibility and explicit ticks for a chart axis.
 */
struct AxisSpec
{
  std::vector<TickSpec> ticks;
  bool                  visible{true};
};

/**
 * @brief Resolved stroke and marker appearance from one reference line series.
 */
struct LineAppearance
{
  uint32_t color{0x387AFFu};
  float    alpha{1.0f};
  float    width{2.5f};
  float    radius{3.0f};
  uint32_t markerColor{0x387AFFu};
  float    markerAlpha{1.0f};
};

/**
 * @brief Independent chart data in a multi-chart reference card.
 */
struct CompanionSpec
{
  AxisSpec           xAxis;
  AxisSpec           yAxis;
  std::vector<float> values;
  LineAppearance     line;
  ChartKind          kind{ChartKind::BAR};
  float              minimum{0.0f};
  float              maximum{100.0f};
};

/**
 * @brief Sample-local annotation configuration; no exported ABI.
 */
struct AnnotationSpec
{
  std::string    text;
  AnnotationKind kind{AnnotationKind::MARKER};
  float          x{0.0f};
  float          y{0.0f};
  float          maximumY{0.0f};
  int            point{-1};
  uint32_t       color{0x387AFFu};
  float          width{0.0f};
  float          offsetX{0.0f};
  float          offsetY{-12.0f};
  float          minimumX{0.0f};
  float          maximumX{0.0f};
  float          cornerRadius{6.0f};
  float          lineWidth{1.5f};
  bool           ellipsis{false};
  bool           pattern{false};
  bool           hasXRange{false};
};

/**
 * @brief Sample-local border override authored for one reference bar.
 */
struct PointBorderSpec
{
  uint32_t index;
  uint32_t color;
  float    width{1.0f};
  float    dash{2.0f};
  float    gap{2.0f};
};

/**
 * @brief Explicit, immutable input for a reference example. Titles never select behavior.
 */
struct ReferenceCase
{
  std::string                     id;
  std::string                     category;
  std::string                     title;
  AxisSpec                        xAxis;
  AxisSpec                        yAxis;
  std::string                     subtitle;
  std::string                     metric{"120.5"};
  std::string                     unit{"Unit"};
  std::string                     description;
  std::string                     hint;
  std::string                     expected;
  std::vector<float>              values;
  std::vector<float>              secondary;
  std::vector<float>              tertiary;
  std::vector<float>              xValues;
  std::vector<float>              secondaryXValues;
  std::vector<std::vector<float>> tabValues;
  std::vector<TextSpec>           texts;
  std::vector<ChartFrame>         frames;
  std::vector<CompanionSpec>      companions;
  std::vector<LineAppearance>     lines;
  std::vector<LegendSpec>         legendItems;
  std::vector<float>              rangeLows;
  std::vector<uint32_t>           pointColors;
  std::vector<PointBorderSpec>    pointBorders;
  std::vector<AnnotationSpec>     annotations;
  std::vector<std::string>        labels;
  std::vector<int>                pageSizes;
  ChartKind                       kind{ChartKind::BAR};
  AxisPreset                      axis{AxisPreset::DEFAULT};
  LayoutPreset                    layout{LayoutPreset::CHART};
  Action                          action{Action::DATA};
  Navigation                      navigation{Navigation::NONE};
  NumberMode                      numberMode{NumberMode::NONE};
  float                           referenceWidth{685.0f};
  ChartKind                       secondaryKind{ChartKind::BAR};
  int                             lineSeriesCount{1};
  float                           minimum{0.0f};
  float                           maximum{100.0f};
  float                           chartWidth{304.0f};
  float                           chartHeight{219.0f};
  float                           from{0.0f};
  float                           to{0.0f};
  float                           duration{0.5f};
  int                             fractionDigits{1};
  int                             pageSize{24};
  int                             statusLevels{0};
  int                             segments{2};
  int                             slots{1};
  uint32_t                        color{0x387AFFu};
  uint32_t                        fillColor{0x387AFFu};
  float                           alpha{1.0f};
  float                           fillAlpha{0.15f};
  float                           cornerRadius{6.0f};
  float                           groupWidth{0.55f};
  float                           innerRadius{0.0f};
  bool                            webReference{true};
  bool                            logarithmic{false};
  bool                            simple{false};
  bool                            grid{true};
  bool                            markers{true};
  bool                            fill{false};
  bool                            gradientFill{false};
  bool                            gradientStroke{false};
  bool                            forecast{false};
  bool                            border{false};
  bool                            muted{false};
  bool                            highlight{false};
  bool                            pointThemes{false};
  bool                            segmentStyle{false};
  bool                            lineBehind{false};
  bool                            secondaryChart{false};
  bool                            legend{false};
  bool                            legendHeader{false};
  bool                            legendImages{false};
  bool                            interactive{true};
  bool                            statusBand{false};
  bool                            customAnnouncement{false};
};

/**
 * @brief State survives card virtualization and theme/layout reconstruction.
 */
struct CaseState
{
  std::string numberInput;
  float       viewport[4]{0.0f, 0.0f, 0.0f, 0.0f};
  double      numberValue{0.0};
  double      numberTarget{0.0};
  int         revision{0};
  int         page{0};
  int         period{0};
  uint32_t    series{0u};
  uint32_t    selected{0u};
  bool        hasSelection{false};
  bool        primaryVisible{true};
  bool        secondaryVisible{true};
  bool        hidden{false};
  bool        hasViewport{false};
  bool        numberRunning{false};
  bool        numberInitialized{false};
};

/**
 * @brief Returns the 137 web reference cases followed by additional native gallery cases.
 *
 * @return The complete ordered catalog
 */
std::vector<ReferenceCase> BuildReferenceCases();

/**
 * @brief Returns a card height that keeps authored content above its controls.
 *
 * @param[in] spec The authored case
 * @return The card height in pixels
 */
float CardHeight(const ReferenceCase& spec);

/**
 * @brief Returns revision/tab data while preserving fixture cardinality and gaps.
 *
 * @param[in] spec The authored case
 * @param[in] state The current revision, period and visibility state
 * @param[in] seriesIndex The zero-based primary or companion series index
 * @return The generated values with authored gaps preserved
 */
std::vector<float> BuildValues(const ReferenceCase& spec, const CaseState& state, uint32_t seriesIndex = 0u);

/**
 * @brief Returns the RGB status color shared by bars, ticks and tooltips.
 *
 * @param[in] value The sample data value
 * @param[in] levels The three-level or five-level status scale
 * @return The packed RGB status color
 */
uint32_t StatusColor(float value, int levels);

/**
 * @brief Formats numeric data independently from its animation.
 *
 * @param[in] value The number to format
 * @param[in] fractionDigits The requested number of fractional digits
 * @return The formatted text
 */
std::string FormatNumber(double value, int fractionDigits);

/**
 * @brief Returns a finite parsed value; leaves output unchanged for invalid input.
 *
 * @param[in] text The complete numeric input
 * @param[out] value The parsed result, unchanged when input is invalid
 * @return True for a valid finite number
 */
bool ParseNumber(const std::string& text, double& value);

/**
 * @brief Returns the page count from data and authored page size.
 *
 * @param[in] spec The authored case
 * @return The number of data pages
 */
int PageCount(const ReferenceCase& spec);

/**
 * @brief Returns the first data index of a page, including variable-size pages.
 *
 * @param[in] spec The authored case
 * @param[in] page The zero-based page index
 * @return The first data index on the page
 */
int PageStart(const ReferenceCase& spec, int page);

/**
 * @brief Returns the authored number of entries in a page.
 *
 * @param[in] spec The authored case
 * @param[in] page The zero-based page index
 * @return The number of values in the page
 */
int PageSize(const ReferenceCase& spec, int page);

/**
 * @brief Returns the announcement for the visible range.
 *
 * @param[in] spec The authored case
 * @param[in] page The zero-based page index
 * @return The visible-range description
 */
std::string PageAnnouncement(const ReferenceCase& spec, int page);
} //namespace ChartStandard

#endif // DALI_CHART_SAMPLE_CHART_REFERENCE_CASES_H
