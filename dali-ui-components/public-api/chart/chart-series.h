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
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/object/base-handle.h>
#include <initializer_list>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{

namespace Integration
{
class ChartSeries;
}

/**
 * @brief Base class for all chart series (Line, Bar, Pie, Scatter).
 *
 * A series holds one set of data points and the style used to draw them.
 * Add a series to a ChartView with ChartView::AddSeries(). This base class is
 * not instantiated directly; create one of the concrete types instead.
 *
 * Data points are always stored as XY pairs. The SetValues() overload that
 * takes Y values only assigns X automatically from the index of each value.
 *
 * A Y value of NaN marks a gap in the data. Gaps are skipped when drawing and
 * are excluded from the automatic axis range.
 *
 * @code
 *   LineSeries series = LineSeries::New();
 *   series.SetName("CPU");
 *   series.SetValues({12.0f, 48.0f, 31.0f, 66.0f});
 *   chart.AddSeries(series);
 * @endcode
 */
class DALI_UI_COMPONENTS_API ChartSeries : public Dali::BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized ChartSeries handle.
   */
  ChartSeries();

  /**
   * @brief Destructor.
   */
  ~ChartSeries();

  /**
   * @brief Copy constructor.
   *
   * @param[in] rhs The handle to copy
   */
  ChartSeries(const ChartSeries& rhs);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs The handle to move
   */
  ChartSeries(ChartSeries&& rhs);

  /**
   * @brief Copy assignment operator.
   *
   * @param[in] rhs The handle to copy
   * @return A reference to this handle
   */
  ChartSeries& operator=(const ChartSeries& rhs);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs The handle to move
   * @return A reference to this handle
   */
  ChartSeries& operator=(ChartSeries&& rhs);

  /**
   * @brief Downcasts a handle to a ChartSeries handle.
   *
   * @param[in] handle The handle to downcast
   * @return A ChartSeries handle, or an uninitialized handle if the cast fails
   */
  static ChartSeries DownCast(BaseHandle handle);

  /**
   * @brief Sets the name of the series.
   *
   * The name is shown in the legend and in the default tooltip text, and is
   * the key used by ChartView::RemoveSeries().
   *
   * @param[in] name The series name
   */
  void SetName(const Dali::String& name);

  /**
   * @brief Retrieves the name of the series.
   *
   * @return The series name
   */
  Dali::String GetName() const;

  /**
   * @brief Sets the base color of the series.
   *
   * Each concrete type applies this differently: LineSeries uses it for the
   * line, BarSeries for the bars, and ScatterSeries for the markers.
   * PieSeries does not use this; each slice's color is set explicitly with
   * PieSeries::AddSlice().
   *
   * Derived types can override parts of this color. For example
   * LineSeries::SetMarkerColor() replaces the marker color only.
   *
   * The default is (0.3, 0.6, 1.0, 1.0).
   *
   * @param[in] color The base color of the series
   */
  void SetColor(const Vector4& color);

  /**
   * @brief Retrieves the base color of the series.
   *
   * @return The base color of the series
   */
  Vector4 GetColor() const;

  /**
   * @brief Sets the Y values of the series. X is the index of each value.
   *
   * Replaces any existing data. A NaN value marks a gap in the data.
   *
   * @param[in] values The Y values, in series order
   */
  void SetValues(const Dali::Vector<float>& values);

  /**
   * @brief Sets the XY values of the series.
   *
   * Replaces any existing data. A NaN y-component marks a gap in the data.
   *
   * @param[in] xyValues The values, where x is the X and y is the Y coordinate
   */
  void SetValues(const Dali::Vector<Vector2>& xyValues);

  /**
   * @brief Sets the Y values of the series. X is the index of each value.
   *
   * @param[in] values The Y values, in series order
   */
  void SetValues(std::initializer_list<float> values)
  {
    Dali::Vector<float> list;
    list.Reserve(values.size());
    for(auto&& value : values)
    {
      list.PushBack(value);
    }
    SetValues(list);
  }

  /**
   * @brief Sets the XY values of the series.
   *
   * @param[in] xyValues The values, where x is the X and y is the Y coordinate
   */
  void SetValues(std::initializer_list<Vector2> xyValues)
  {
    Dali::Vector<Vector2> list;
    list.Reserve(xyValues.size());
    for(auto&& xyValue : xyValues)
    {
      list.PushBack(xyValue);
    }
    SetValues(list);
  }

  /**
   * @brief Sets whether the series is drawn and included in the axis range.
   *
   * The default is true.
   *
   * @param[in] visible True to draw the series
   */
  void SetVisible(bool visible);

  /**
   * @brief Retrieves whether the series is drawn and included in the axis range.
   *
   * @return True if the series is drawn
   */
  bool IsVisible() const;

  /**
   * @brief Appends one Y value to the end of the series.
   *
   * The X value is assigned automatically as the previous last X plus one,
   * or 0 if the series is empty. If SetMaxDataPoints() is set, the oldest
   * point is dropped once the limit is exceeded.
   *
   * @param[in] y The Y value to append
   */
  void AppendValue(float y);

  /**
   * @brief Appends one XY value to the end of the series.
   *
   * If SetMaxDataPoints() is set, the oldest point is dropped once the limit
   * is exceeded.
   *
   * @param[in] x The X value to append
   * @param[in] y The Y value to append
   */
  void AppendValue(float x, float y);

  /**
   * @brief Appends Y values to the end of the series.
   *
   * X values are assigned automatically, continuing from the previous last X.
   * If SetMaxDataPoints() is set, the oldest points are dropped once the
   * limit is exceeded.
   *
   * @param[in] values The Y values to append
   */
  void AppendValues(const Dali::Vector<float>& values);

  /**
   * @brief Appends Y values to the end of the series.
   *
   * @param[in] values The Y values to append
   */
  void AppendValues(std::initializer_list<float> values)
  {
    Dali::Vector<float> list;
    list.Reserve(values.size());
    for(auto&& value : values)
    {
      list.PushBack(value);
    }
    AppendValues(list);
  }

  /**
   * @brief Sets the maximum number of data points retained by the series.
   *
   * Once the series holds more points than this, AppendValue() and
   * AppendValues() drop the oldest points to stay at the limit. SetValues()
   * is not trimmed by this setting.
   *
   * The default is 0, meaning unlimited. Negative values are treated as 0.
   *
   * @param[in] maximumCount The maximum number of data points, or 0 for unlimited
   */
  void SetMaximumDataPoints(int maximumCount);

  /**
   * @brief Retrieves the maximum number of data points retained by the series.
   *
   * @return The maximum number of data points, or 0 if unlimited
   */
  int GetMaximumDataPoints() const;

  /**
   * @brief Sets the draw order of the series relative to other series in the same chart.
   *
   * Series with a higher z-index are drawn on top of series with a lower one.
   * The default is 0.
   *
   * @param[in] zIndex The draw order index
   */
  void SetZIndex(int zIndex);

  /**
   * @brief Retrieves the draw order of the series relative to other series in the same chart.
   *
   * @return The draw order index
   */
  int GetZIndex() const;

public:
  explicit ChartSeries(Integration::ChartSeries* impl);
};

} // namespace Ui
} // namespace Dali
