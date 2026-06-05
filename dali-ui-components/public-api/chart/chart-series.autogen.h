// Manually maintained (mirrors autogen pattern from dali-ui-foundation).
#pragma once

/**
 * @brief Injects ChartSeries base-class setters with ChildClass& return type
 *        for fluent method chaining — excludes SetColor because LineSeries,
 *        BarSeries, and ScatterSeries each declare their own SetColor override
 *        that already returns DerivedClass&.
 *
 * Usage (classes that already override SetColor):
 *   DALI_UI_CHAIN_CHARTSERIES_METHODS_BASE(LineSeries)
 *
 * Usage (classes that do NOT override SetColor):
 *   DALI_UI_CHAIN_CHARTSERIES_METHODS(PieSeries)
 */
#define DALI_UI_CHAIN_CHARTSERIES_METHODS_BASE(ChildClass)                                                              \
  ChildClass& SetName(const Dali::String& name) { ChartSeries::SetName(name); return *this; }                          \
  ChildClass& SetValues(const std::vector<float>& values) { ChartSeries::SetValues(values); return *this; }            \
  ChildClass& SetValues(const std::vector<std::pair<float, float>>& xy) { ChartSeries::SetValues(xy); return *this; }  \
  ChildClass& SetVisible(bool visible) { ChartSeries::SetVisible(visible); return *this; }                             \
  ChildClass& AppendValue(float y) { ChartSeries::AppendValue(y); return *this; }                                      \
  ChildClass& AppendValue(float x, float y) { ChartSeries::AppendValue(x, y); return *this; }                         \
  ChildClass& AppendValues(const std::vector<float>& values) { ChartSeries::AppendValues(values); return *this; }     \
  ChildClass& SetMaxDataPoints(int maxCount) { ChartSeries::SetMaxDataPoints(maxCount); return *this; }               \
  ChildClass& SetZIndex(int zIndex) { ChartSeries::SetZIndex(zIndex); return *this; }

#define DALI_UI_CHAIN_CHARTSERIES_METHODS(ChildClass)                                                 \
  DALI_UI_CHAIN_CHARTSERIES_METHODS_BASE(ChildClass)                                                  \
  ChildClass& SetColor(const Vector4& color) { ChartSeries::SetColor(color); return *this; }
