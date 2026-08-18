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

/**
 * @file  chart-gallery.cpp
 * @brief 20 chart samples in a 7-column scrollable grid — dali-ui port.
 *
 * Layout (7 cols × 3 rows):
 *   Line Smoothness | Basic Area     | Line Geometries | Animation      | Auto Updates  | Real Time      | Gaps / NaN
 *   Basic Bars      | Stacked Bars   | Bar + Labels    | Pie / Donut    | Gauge         | Basic Scatter  | ZIndex
 *   Zoom & Pan      | Dynamic Vis.   | Sections        | Mixed Line+Bar | Axis Labels   | Formatter      | (extra)
 */

#include <cmath>
#include <cstdlib>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include <dali-ui-components/dali-ui-components.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali/public-api/events/pan-gesture-detector.h>
#include <dali/public-api/events/pan-gesture.h>

using namespace Dali;
using namespace Dali::Ui;

// ---------------------------------------------------------------------------
namespace
{
// The chart API takes Dali::Vector, so convert the sample's std containers at the call boundary.
Dali::Vector<float> ToValues(const std::vector<float>& values)
{
  Dali::Vector<float> result;
  result.Reserve(values.size());
  for(float value : values)
  {
    result.PushBack(value);
  }
  return result;
}

Dali::Vector<Vector2> ToPoints(const std::vector<std::pair<float, float>>& points)
{
  Dali::Vector<Vector2> result;
  result.Reserve(points.size());
  for(const auto& point : points)
  {
    result.PushBack(Vector2(point.first, point.second));
  }
  return result;
}

// LiveCharts FluentDesign palette
const Vector4 LC_PURPLE(0.455f, 0.302f, 0.663f, 1.0f);
const Vector4 LC_RED(0.906f, 0.282f, 0.337f, 1.0f);
const Vector4 LC_ORANGE(1.000f, 0.549f, 0.000f, 1.0f);
const Vector4 LC_BLUE(0.000f, 0.600f, 0.737f, 1.0f);
const Vector4 LC_GRAY(0.298f, 0.290f, 0.282f, 1.0f);
const Vector4 LC_CYAN(0.000f, 0.718f, 0.765f, 1.0f);

// MaterialDesign500
const Vector4 MD_BLUE(0.129f, 0.588f, 0.953f, 1.0f);
const Vector4 MD_RED(0.957f, 0.263f, 0.212f, 1.0f);
const Vector4 MD_GREEN(0.545f, 0.765f, 0.290f, 1.0f);
const Vector4 MD_AMBER(1.000f, 0.757f, 0.027f, 1.0f);
const Vector4 MD_INDIGO(0.247f, 0.318f, 0.710f, 1.0f);
const Vector4 MD_BGRAY(0.376f, 0.490f, 0.545f, 1.0f);

// Light theme
const Vector4 LC_BG(1.0f, 1.0f, 1.0f, 1.0f);
const Vector4 LC_GRID(0.922f, 0.922f, 0.922f, 1.0f);

const float GAP      = 6.0f;
const float HEADER_H = 46.0f;
const float LABEL_H  = 20.0f;
const int   COLS     = 7;
} // namespace

// ---------------------------------------------------------------------------
class ChartGalleryController : public ConnectionTracker
{
public:
  explicit ChartGalleryController(Application& app)
  : mApplication(app)
  {
    mApplication.InitSignal().Connect(this, &ChartGalleryController::OnCreate);
  }

  // Update all chart/label AbsoluteLayoutParams Y when scroll offset changes.
  void UpdateScrollPositions()
  {
    for(int i = 0; i < static_cast<int>(mAllCharts.size()); ++i)
    {
      float x = GAP + (i % COLS) * (mCw + GAP);
      float y = GAP + (i / COLS) * mRowH + mScrollY;
      mAllCharts[i].SetLayoutParams(AbsoluteLayoutParams::New()
                                      .SetX(x)
                                      .SetY(y)
                                      .SetWidth(mCw)
                                      .SetHeight(mCh));
    }
    for(int i = 0; i < static_cast<int>(mEntryLabels.size()); ++i)
    {
      float x = GAP + (i % COLS) * (mCw + GAP);
      float y = GAP + (i / COLS) * mRowH + mCh + mScrollY;
      mEntryLabels[i].SetLayoutParams(AbsoluteLayoutParams::New()
                                        .SetX(x)
                                        .SetY(y)
                                        .SetWidth(mCw)
                                        .SetHeight(LABEL_H));
    }
  }

  void OnCreate(Application application)
  {
    Window  window = application.GetWindow();
    auto    positionSize = window.GetPositionSize();
    Vector2 windowSize   = Vector2(static_cast<float>(positionSize.width), static_cast<float>(positionSize.height));
    window.SetBackgroundColor(Vector4(0.93f, 0.93f, 0.95f, 1.0f));
    window.KeyEventSignal().Connect(this, &ChartGalleryController::OnKeyEvent);

    mCw             = (windowSize.x - GAP * (COLS + 1)) / COLS;
    mCh             = mCw * 0.82f;
    mRowH           = mCh + LABEL_H + GAP;
    const int N     = 22;
    const int nRows = (N + COLS - 1) / COLS;
    mTotalH         = nRows * mRowH + GAP;
    mWinH           = windowSize.y - HEADER_H;

    // All UI lives in one MATCH_PARENT AbsoluteLayout so the dali-ui layout
    // system processes everything — essential for ChartView::OnArrange.
    // Scrolling works by updating AbsoluteLayoutParams Y for each chart/label.
    mRootLayout = AbsoluteLayout::New();
    mRootLayout.SetRequestedWidth(MATCH_PARENT);
    mRootLayout.SetRequestedHeight(MATCH_PARENT);

    // Header
    Label header = Label::New("LiveCharts Gallery");
    header.SetLayoutParams(AbsoluteLayoutParams::New()
                             .SetX(0.0f)
                             .SetY(0.0f)
                             .SetWidth(windowSize.x)
                             .SetHeight(HEADER_H));
    header.SetFontSize(11.0f);
    header.SetTextColor(UiColor(1.f, 1.f, 1.f, 1.f));
    header.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    header.SetVerticalTextAlignment(Text::Alignment::CENTER);
    header.SetBackgroundColor(UiColor(LC_PURPLE.r, LC_PURPLE.g, LC_PURPLE.b, LC_PURPLE.a));

    // Content area starts below the header. CLIPPING_MODE ensures charts
    // scrolled above the header boundary are clipped and do not overlap it.
    mContentArea = AbsoluteLayout::New();
    mContentArea.SetRequestedWidth(windowSize.x);
    mContentArea.SetRequestedHeight(mWinH);
    mContentArea.SetLayoutParams(AbsoluteLayoutParams::New()
                                   .SetX(0.0f)
                                   .SetY(HEADER_H)
                                   .SetWidth(windowSize.x)
                                   .SetHeight(mWinH));
    mContentArea.SetProperty(Actor::Property::CLIPPING_MODE,
                             static_cast<int>(ClippingMode::CLIP_TO_BOUNDING_BOX));

    mPanDetector = PanGestureDetector::New();
    mPanDetector.Attach(mContentArea);
    mPanDetector.DetectedSignal().Connect(this, &ChartGalleryController::OnPan);

    // Build chart list
    struct Entry
    {
      const char* label;
      ChartView   view;
    };
    std::vector<Entry> entries;

    // Row 0: Line family + updates
    entries.push_back({"Line Smoothness", MakeLineSmoothness(mCw, mCh)});
    entries.push_back({"Basic Area", MakeBasicArea(mCw, mCh)});
    entries.push_back({"Line Geometries", MakeLineGeometries(mCw, mCh)});
    entries.push_back({"Animation", MakeAnimation(mCw, mCh)});
    entries.push_back({"Auto Updates", MakeAutoUpdates(mCw, mCh)});
    entries.push_back({"Real Time", MakeRealTime(mCw, mCh)});
    entries.push_back({"Gaps / NaN", MakeGapNaN(mCw, mCh)});

    // Row 1: Bar + Pie + Gauge + Scatter + ZIndex
    entries.push_back({"Basic Bars", MakeBasicBars(mCw, mCh)});
    entries.push_back({"Stacked Bars", MakeStackedBars(mCw, mCh)});
    entries.push_back({"Bar + Labels", MakeBarLabels(mCw, mCh)});
    entries.push_back({"Pie / Donut", MakePieDonut(mCw, mCh)});
    mGaugeChart = MakeGauge(mCw, mCh);
    entries.push_back({"Gauge", mGaugeChart});
    entries.push_back({"Basic Scatter", MakeBasicScatter(mCw, mCh)});
    entries.push_back({"ZIndex", MakeZIndex(mCw, mCh)});

    // Row 2: Interaction + Advanced
    entries.push_back({"Zoom & Pan", MakeZoomPan(mCw, mCh)});
    entries.push_back({"Dynamic Visibility", MakeDynamicVisibility(mCw, mCh)});
    entries.push_back({"Sections", MakeSections(mCw, mCh)});
    entries.push_back({"Mixed Line + Bar", MakeMixedLineBar(mCw, mCh)});
    entries.push_back({"Axis Labels", MakeAxisLabels(mCw, mCh)});
    entries.push_back({"Formatter", MakeFormatter(mCw, mCh)});
    entries.push_back({"Sections (Full)", MakeSectionsFull(mCw, mCh)});
    entries.push_back({"Pie Chart", MakePieChart(mCw, mCh)});

    // Place in grid using AbsoluteLayoutParams — position must go through the
    // layout system so it is not overridden by ViewImpl::OnArrange.
    for(int i = 0; i < static_cast<int>(entries.size()); ++i)
    {
      float x = GAP + (i % COLS) * (mCw + GAP);
      float y = GAP + (i / COLS) * mRowH;

      auto& cv = entries[i].view;
      cv.SetLayoutParams(AbsoluteLayoutParams::New()
                           .SetX(x)
                           .SetY(y)
                           .SetWidth(mCw)
                           .SetHeight(mCh));
      mContentArea.Add(cv);
      mAllCharts.push_back(cv);

      Label lbl = Label::New(entries[i].label);
      lbl.SetLayoutParams(AbsoluteLayoutParams::New()
                            .SetX(x)
                            .SetY(y + mCh)
                            .SetWidth(mCw)
                            .SetHeight(LABEL_H));
      lbl.SetFontSize(6.0f);
      lbl.SetTextColor(UiColor(0.25f, 0.25f, 0.25f, 1.0f));
      lbl.SetHorizontalTextAlignment(Text::Alignment::CENTER);
      lbl.SetVerticalTextAlignment(Text::Alignment::CENTER);
      mContentArea.Add(lbl);
      mEntryLabels.push_back(lbl);
    }

    // mContentArea first (behind), header last (on top — covers scrolled content)
    mRootLayout.Add(mContentArea);
    mRootLayout.Add(header);
    window.Add(mRootLayout);

    // Timers
    mTimerAnim = Timer::New(2500);
    mTimerAnim.TickSignal().Connect(this, &ChartGalleryController::OnTimerAnim);
    mTimerAnim.Start();

    mTimerAppend = Timer::New(900);
    mTimerAppend.TickSignal().Connect(this, &ChartGalleryController::OnTimerAppend);
    mTimerAppend.Start();

    mTimerGauge = Timer::New(700);
    mTimerGauge.TickSignal().Connect(this, &ChartGalleryController::OnTimerGauge);
    mTimerGauge.Start();

    mTimerSine = Timer::New(100);
    mTimerSine.TickSignal().Connect(this, &ChartGalleryController::OnTimerSine);
    mTimerSine.Start();
  }

  // ── Scroll ────────────────────────────────────────────────────────────────
  void OnPan(Actor /*actor*/, PanGesture pan)
  {
    if(pan.GetState() == GestureState::STARTED ||
       pan.GetState() == GestureState::CONTINUING)
    {
      mScrollY += pan.GetDisplacement().y;
      // minY is the most negative scroll value: content bottom aligns with viewport bottom.
      // mTotalH = total content height, mWinH = viewport height (winH - HEADER_H).
      const float minY = -(mTotalH - mWinH);
      mScrollY         = std::max(minY, std::min(0.0f, mScrollY));
      UpdateScrollPositions();
    }
  }

  // ── Timers ────────────────────────────────────────────────────────────────
  bool OnTimerAnim()
  {
    if(!mAnimSeries) return false;
    mAnimToggle = !mAnimToggle;
    if(mAnimToggle)
      mAnimSeries.SetValues({60.0f, 40.0f, 280.0f, 100.0f, 310.0f, 180.0f});
    else
      mAnimSeries.SetValues({120.0f, 190.0f, 150.0f, 250.0f, 220.0f, 280.0f});
    return true;
  }

  bool OnTimerAppend()
  {
    if(!mAppendSeries) return false;
    mAppendY = 1.0f + std::fmod(mAppendY * 1.37f + 11.0f, 8.0f);
    mAppendSeries.AppendValue(mAppendY);
    return true;
  }

  bool OnTimerGauge()
  {
    if(!mGaugeChart) return false;
    mGaugeChart.SetGaugeValue(static_cast<float>(std::rand() % 101));
    return true;
  }

  bool OnTimerSine()
  {
    if(!mSineSeries) return false;
    float noise = (static_cast<float>(std::rand() % 100) / 500.0f) - 0.1f;
    mSineSeries.AppendValue(std::sin(mSineT) + noise);
    mSineT += 0.3f;
    return true;
  }

  // =========================================================================
  // Chart factories
  // =========================================================================

  // 1. Line Smoothness
  ChartView MakeLineSmoothness(float w, float h)
  {
    auto c = NewLine(w, h);
    c.SetProperty(ChartView::Property::SHOW_LEGEND, true);
    AddLine(c, "Smooth", LC_PURPLE, 0.8f, {5, 0, 8, 3, 7, 2, 9});
    AddLine(c, "Straight", LC_RED, 0.0f, {4, 6, 2, 8, 1, 7, 4});
    return c;
  }

  // 2. Basic Area
  ChartView MakeBasicArea(float w, float h)
  {
    auto c = ChartView::New(ChartView::Type::AREA, Vector2(w, h));
    Light(c);
    c.SetProperty(ChartView::Property::SHOW_LEGEND, true);
    c.SetProperty(ChartView::Property::SHOW_MARKERS, false);
    for(auto& p : std::vector<std::tuple<const char*, Vector4, std::vector<float>>>{
          {"S1", LC_BLUE, {4, 4, 7, 2, 8, 5, 9, 4, 6}},
          {"S2", LC_PURPLE, {2, 3, 5, 8, 1, 6, 3, 7, 2}}})
    {
      {
        LineSeries lineSeries1 = LineSeries::New();
        lineSeries1.SetColor(std::get<1>(p));
        lineSeries1.SetSmoothness(0.65f);
        lineSeries1.SetFillEnabled(true);
        lineSeries1.SetName(std::get<0>(p));
        lineSeries1.SetValues(ToValues(std::get<2>(p)));
        c.AddSeries(lineSeries1);
      }
    }
    return c;
  }

  // 3. Line Geometries
  ChartView MakeLineGeometries(float w, float h)
  {
    auto c = NewLine(w, h);
    c.SetProperty(ChartView::Property::SHOW_MARKERS, true);
    c.SetProperty(ChartView::Property::SHOW_LEGEND, true);
    using MS = LineSeries::MarkerShape;
    struct G
    {
      const char*        nm;
      Vector4            col;
      MS                 shape;
      std::vector<float> v;
    };
    for(auto& g : std::vector<G>{
          {"Circle", LC_PURPLE, MS::CIRCLE, {3, 5, 2, 7, 4}},
          {"Square", LC_RED, MS::SQUARE, {5, 2, 8, 1, 6}},
          {"Triangle", LC_ORANGE, MS::TRIANGLE, {2, 7, 4, 6, 3}},
          {"Diamond", LC_BLUE, MS::DIAMOND, {7, 3, 6, 2, 8}}})
    {
      {
        LineSeries lineSeries2 = LineSeries::New();
        lineSeries2.SetColor(g.col);
        lineSeries2.SetSmoothness(0.65f);
        lineSeries2.SetMarkerShape(g.shape);
        lineSeries2.SetMarkerRadius(4.5f);
        lineSeries2.SetMarkersVisible(true);
        lineSeries2.SetName(g.nm);
        lineSeries2.SetValues(ToValues(g.v));
        c.AddSeries(lineSeries2);
      }
    }
    return c;
  }

  // 4. Animation (timer-driven data swap, 300ms EASE_OUT)
  ChartView MakeAnimation(float w, float h)
  {
    auto c = NewLine(w, h);
    c.SetProperty(ChartView::Property::SHOW_MARKERS, true);
    c.SetProperty(ChartView::Property::SHOW_LEGEND, true);
    c.SetAnimationDuration(300.0f);
    c.SetAnimationEasing(ChartView::EasingType::EASE_OUT);

    {
      ChartAxis chartAxis1 = ChartAxis::New();
      chartAxis1.SetLabels({"Jan", "Feb", "Mar", "Apr", "May", "Jun"});
      c.SetXAxis(chartAxis1);
    }

    mAnimSeries = LineSeries::New();
    mAnimSeries.SetName("Revenue");
    mAnimSeries.SetColor(LC_BLUE);
    mAnimSeries.SetSmoothness(0.65f);
    mAnimSeries.SetMarkersVisible(true);
    mAnimSeries.SetValues({120, 190, 150, 250, 220, 280});
    c.AddSeries(mAnimSeries);

    {
      LineSeries lineSeries3 = LineSeries::New();
      lineSeries3.SetColor(LC_RED);
      lineSeries3.SetSmoothness(0.65f);
      lineSeries3.SetMarkersVisible(true);
      lineSeries3.SetName("Cost");
      lineSeries3.SetValues({80, 110, 90, 150, 130, 160});
      c.AddSeries(lineSeries3);
    }
    return c;
  }

  // 5. Auto Updates (append)
  ChartView MakeAutoUpdates(float w, float h)
  {
    auto c = NewLine(w, h);
    c.SetUpdateThrottle(0.0f);
    mAppendSeries = LineSeries::New();
    mAppendSeries.SetName("Value");
    mAppendSeries.SetColor(LC_BLUE);
    mAppendSeries.SetSmoothness(0.65f);
    mAppendSeries.SetMaximumDataPoints(12);
    mAppendSeries.AppendValues({1, 3, 5, 2, 4, 6, 3, 5, 2});
    c.AddSeries(mAppendSeries);
    return c;
  }

  // 6. Real Time (sine wave)
  ChartView MakeRealTime(float w, float h)
  {
    auto c = NewLine(w, h);
    c.SetUpdateThrottle(0.0f);
    {
      ChartAxis chartAxis2 = ChartAxis::New();
      chartAxis2.SetMinimumLimit(-1.5f);
      chartAxis2.SetMaximumLimit(1.5f);
      c.SetYAxis(chartAxis2);
    }
    mSineSeries = LineSeries::New();
    mSineSeries.SetName("Signal");
    mSineSeries.SetColor(LC_BLUE);
    mSineSeries.SetSmoothness(0.3f);
    mSineSeries.SetMaximumDataPoints(40);
    c.AddSeries(mSineSeries);
    return c;
  }

  // 7. Gaps / NaN
  ChartView MakeGapNaN(float w, float h)
  {
    auto c = NewLine(w, h);
    c.SetProperty(ChartView::Property::SHOW_MARKERS, true);
    {
      ChartAxis chartAxis3 = ChartAxis::New();
      chartAxis3.SetLabels({"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"});
      c.SetXAxis(chartAxis3);
    }
    const float N = std::numeric_limits<float>::quiet_NaN();
    {
      LineSeries lineSeries4 = LineSeries::New();
      lineSeries4.SetColor(LC_PURPLE);
      lineSeries4.SetSmoothness(0.65f);
      lineSeries4.SetMarkersVisible(true);
      lineSeries4.SetName("Gaps");
      lineSeries4.SetValues({3, N, N, 5, 2, N, 8, 4, N, 6});
      c.AddSeries(lineSeries4);
    }
    return c;
  }

  // 8. Basic Bars (grouped)
  ChartView MakeBasicBars(float w, float h)
  {
    auto c = ChartView::New(ChartView::Type::BAR, Vector2(w, h));
    Light(c);
    c.SetProperty(ChartView::Property::SHOW_LEGEND, true);
    {
      ChartAxis chartAxis4 = ChartAxis::New();
      chartAxis4.SetLabels({"Jan", "Feb", "Mar", "Apr", "May", "Jun"});
      c.SetXAxis(chartAxis4);
    }
    for(auto& p : std::vector<std::tuple<const char*, Vector4, std::vector<float>>>{
          {"S1", LC_PURPLE, {7, 3, 6, 4, 8, 2}},
          {"S2", LC_RED, {4, 6, 2, 7, 3, 5}},
          {"S3", LC_ORANGE, {5, 4, 7, 2, 6, 8}}})
    {
      {
        BarSeries barSeries1 = BarSeries::New();
        barSeries1.SetColor(std::get<1>(p));
        barSeries1.SetStacked(false);
        barSeries1.SetName(std::get<0>(p));
        barSeries1.SetValues(ToValues(std::get<2>(p)));
        c.AddSeries(barSeries1);
      }
    }
    return c;
  }

  // 9. Stacked Bars
  ChartView MakeStackedBars(float w, float h)
  {
    auto c = ChartView::New(ChartView::Type::BAR, Vector2(w, h));
    Light(c);
    c.SetProperty(ChartView::Property::SHOW_LEGEND, true);
    {
      ChartAxis chartAxis5 = ChartAxis::New();
      chartAxis5.SetLabels({"Jan", "Feb", "Mar", "Apr", "May", "Jun"});
      c.SetXAxis(chartAxis5);
    }
    for(auto& p : std::vector<std::tuple<const char*, Vector4, std::vector<float>>>{
          {"S1", LC_BLUE, {6, 2, 3, 4, 2, 5}},
          {"S2", LC_PURPLE, {2, 4, 2, 2, 3, 3}},
          {"S3", LC_CYAN, {1, 1, 3, 2, 3, 2}}})
    {
      {
        BarSeries barSeries2 = BarSeries::New();
        barSeries2.SetColor(std::get<1>(p));
        barSeries2.SetStacked(true);
        barSeries2.SetName(std::get<0>(p));
        barSeries2.SetValues(ToValues(std::get<2>(p)));
        c.AddSeries(barSeries2);
      }
    }
    return c;
  }

  // 10. Bar + Data Labels
  ChartView MakeBarLabels(float w, float h)
  {
    auto c = ChartView::New(ChartView::Type::BAR, Vector2(w, h));
    Light(c);
    c.SetProperty(ChartView::Property::SHOW_LEGEND, true);
    {
      ChartAxis chartAxis6 = ChartAxis::New();
      chartAxis6.SetLabels({"Q1", "Q2", "Q3", "Q4"});
      c.SetXAxis(chartAxis6);
    }
    for(auto& p : std::vector<std::tuple<const char*, Vector4, std::vector<float>>>{
          {"Revenue", LC_BLUE, {210, 280, 250, 320}},
          {"Cost", LC_RED, {140, 160, 130, 190}}})
    {
      {
        BarSeries barSeries3 = BarSeries::New();
        barSeries3.SetColor(std::get<1>(p));
        barSeries3.SetStacked(false);
        barSeries3.SetDataLabelsVisible(true);
        barSeries3.SetName(std::get<0>(p));
        barSeries3.SetValues(ToValues(std::get<2>(p)));
        c.AddSeries(barSeries3);
      }
    }
    return c;
  }

  // 11. Pie / Donut (merged)
  ChartView MakePieDonut(float w, float h)
  {
    auto c = ChartView::New(ChartView::Type::PIE, Vector2(w, h));
    c.SetProperty(ChartView::Property::SHOW_LEGEND, true);
    PieSeries pie = PieSeries::New();
    pie.AddSlice("Apple", 248, MD_RED);
    pie.AddSlice("Samsung", 220, MD_BLUE);
    pie.AddSlice("Huawei", 95, MD_GREEN);
    pie.AddSlice("Xiaomi", 90, MD_AMBER);
    pie.AddSlice("Others", 190, MD_BGRAY);
    pie.SetInnerRadiusRatio(0.48f);
    pie.SetCenterLabel("843M");
    pie.SetSliceGap(2.0f);
    c.AddSeries(pie);
    return c;
  }

  // 12. Gauge (zones + auto-timer)
  ChartView MakeGauge(float w, float h)
  {
    auto c = ChartView::New(ChartView::Type::GAUGE, Vector2(w, h));
    c.SetGaugeMinimumValue(0);
    c.SetGaugeMaximumValue(100);
    c.SetGaugeValue(0);
    c.SetGaugeArcSpan(270);
    c.SetGaugeStartAngle(135);
    c.SetGaugeArcWidth(0.18f);
    c.AddGaugeRange(0, 60, MD_GREEN);
    c.AddGaugeRange(60, 80, MD_AMBER);
    c.AddGaugeRange(80, 100, MD_RED);
    c.SetGaugeProgressColor(LC_BLUE);
    c.SetGaugeTrackColor(Vector4(0.90f, 0.90f, 0.90f, 1));
    return c;
  }

  // 13. Basic Scatter
  ChartView MakeBasicScatter(float w, float h)
  {
    auto c = ChartView::New(ChartView::Type::LINE, Vector2(w, h));
    Light(c);
    c.SetProperty(ChartView::Property::SHOW_LEGEND, true);
    using XY = std::pair<float, float>;
    struct S
    {
      const char*     nm;
      Vector4         col;
      std::vector<XY> pts;
    };
    for(auto& s : std::vector<S>{
          {"S1", LC_PURPLE, {{1.2f, 5.3f}, {2.5f, 2.8f}, {3.1f, 7.1f}, {4.7f, 4.6f}, {2.3f, 6.9f}, {5.8f, 3.2f}, {1.9f, 8.4f}, {3.6f, 5.7f}, {4.2f, 2.3f}, {6.1f, 7.8f}}},
          {"S2", LC_RED, {{7.1f, 1.4f}, {8.4f, 3.7f}, {6.3f, 2.1f}, {9.2f, 4.8f}, {7.8f, 1.9f}, {8.9f, 3.2f}, {6.6f, 2.6f}, {9.5f, 4.5f}, {7.4f, 1.7f}, {8.1f, 3.9f}}}})
    {
      {
        ScatterSeries scatterSeries1 = ScatterSeries::New();
        scatterSeries1.SetColor(s.col);
        scatterSeries1.SetMarkerRadius(4.0f);
        scatterSeries1.SetMarkerShape(ScatterSeries::MarkerShape::CIRCLE);
        scatterSeries1.SetName(s.nm);
        scatterSeries1.SetValues(ToPoints(s.pts));
        c.AddSeries(scatterSeries1);
      }
    }
    return c;
  }

  // 14. ZIndex — thick base series drawn below thin top series
  ChartView MakeZIndex(float w, float h)
  {
    auto c = NewLine(w, h);
    c.SetProperty(ChartView::Property::SHOW_MARKERS, true);
    c.SetProperty(ChartView::Property::SHOW_LEGEND, true);
    {
      ChartAxis chartAxis7 = ChartAxis::New();
      chartAxis7.SetLabels({"A", "B", "C", "D", "E", "F", "G"});
      c.SetXAxis(chartAxis7);
    }

    {
      LineSeries lineSeries5 = LineSeries::New();
      lineSeries5.SetColor(LC_BLUE);
      lineSeries5.SetSmoothness(0.5f);
      lineSeries5.SetMarkersVisible(true);
      lineSeries5.SetZIndex(10);
      lineSeries5.SetName("Top (ZIndex=10)");
      lineSeries5.SetValues({100, 150, 120, 170, 200, 180, 220});
      c.AddSeries(lineSeries5);
    }

    {
      LineSeries lineSeries6 = LineSeries::New();
      lineSeries6.SetColor(LC_RED);
      lineSeries6.SetSmoothness(0.5f);
      lineSeries6.SetLineWidth(6.0f);
      lineSeries6.SetMarkersVisible(true);
      lineSeries6.SetZIndex(0);
      lineSeries6.SetName("Base (ZIndex=0)");
      lineSeries6.SetValues({80, 130, 160, 140, 170, 160, 190});
      c.AddSeries(lineSeries6);
    }
    return c;
  }

  // 15. Zoom & Pan
  ChartView MakeZoomPan(float w, float h)
  {
    auto c = NewLine(w, h);
    c.SetProperty(ChartView::Property::SHOW_TOOLTIP, true);
    c.SetProperty(ChartView::Property::SHOW_LEGEND, true);
    c.SetZoomMode(static_cast<int>(ChartView::ZoomMode::PAN_X) |
                  static_cast<int>(ChartView::ZoomMode::ZOOM_X));
    c.SetZoomClampEnabled(true);
    c.SetAutoFitYOnPanEnabled(true);
    AddLine(c, "Revenue", LC_BLUE, 0.65f,
            {120, 150, 130, 190, 210, 250, 230, 280, 260, 300, 290, 320,
             340, 310, 360, 400, 380, 420, 410, 460, 440, 490, 470, 510});
    AddLine(c, "Cost", LC_RED, 0.65f,
            {80, 95, 90, 110, 130, 150, 140, 170, 160, 190, 180, 200,
             210, 200, 220, 250, 230, 260, 250, 280, 270, 300, 290, 310});
    return c;
  }

  // 16. Dynamic Visibility
  ChartView MakeDynamicVisibility(float w, float h)
  {
    auto c = NewLine(w, h);
    c.SetProperty(ChartView::Property::SHOW_LEGEND, true);
    c.SetLegendToggleEnabled(true);
    AddLine(c, "S1", LC_PURPLE, 0.65f, {3, 5, 7, 2, 8, 4, 6});
    AddLine(c, "S2", LC_RED, 0.65f, {6, 2, 8, 4, 3, 7, 5});
    AddLine(c, "S3", LC_ORANGE, 0.65f, {4, 7, 3, 9, 2, 6, 8});
    return c;
  }

  // 17. Sections
  ChartView MakeSections(float w, float h)
  {
    auto c = NewLine(w, h);
    {
      ChartAxis chartAxis8 = ChartAxis::New();
      chartAxis8.SetMinimumLimit(0);
      chartAxis8.SetMaximumLimit(10);
      c.SetYAxis(chartAxis8);
    }
    AddLine(c, "Value", LC_PURPLE, 0.5f, {2, 4, 6, 3, 7, 5, 8, 4, 6, 3});
    auto band = [&](float a, float b, const Vector4& col)
    {
      {
        ChartSection chartSection1 = ChartSection::New();
        chartSection1.SetMinimumY(a);
        chartSection1.SetMaximumY(b);
        chartSection1.SetFillColor(Vector4(col.r, col.g, col.b, 0.15f));
        c.AddSection(chartSection1);
      }
    };
    band(7, 10, MD_RED);
    band(4, 7, MD_AMBER);
    band(0, 4, MD_GREEN);
    return c;
  }

  // 18. Mixed Line + Bar
  ChartView MakeMixedLineBar(float w, float h)
  {
    auto c = ChartView::New(ChartView::Type::BAR, Vector2(w, h));
    Light(c);
    c.SetProperty(ChartView::Property::SHOW_LEGEND, true);
    {
      ChartAxis chartAxis9 = ChartAxis::New();
      chartAxis9.SetLabels({"Jan", "Feb", "Mar", "Apr", "May", "Jun"});
      c.SetXAxis(chartAxis9);
    }

    {
      BarSeries barSeries4 = BarSeries::New();
      barSeries4.SetColor(LC_BLUE);
      barSeries4.SetStacked(false);
      barSeries4.SetName("Revenue");
      barSeries4.SetValues({210, 280, 250, 320, 300, 360});
      c.AddSeries(barSeries4);
    }

    {
      LineSeries lineSeries7 = LineSeries::New();
      lineSeries7.SetColor(LC_RED);
      lineSeries7.SetSmoothness(0.5f);
      lineSeries7.SetMarkersVisible(true);
      lineSeries7.SetName("Target");
      lineSeries7.SetValues({220, 260, 270, 300, 310, 340});
      c.AddSeries(lineSeries7);
    }
    return c;
  }

  // 19. Axis Labels
  ChartView MakeAxisLabels(float w, float h)
  {
    auto c = NewLine(w, h);
    {
      ChartAxis chartAxis10 = ChartAxis::New();
      chartAxis10.SetTitle("Month");
      chartAxis10.SetLabels({"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul"});
      c.SetXAxis(chartAxis10);
    }
    {
      ChartAxis chartAxis11 = ChartAxis::New();
      chartAxis11.SetTitle("Revenue (M)");
      chartAxis11.SetMinimumLimit(0);
      chartAxis11.SetMaximumLimit(4);
      c.SetYAxis(chartAxis11);
    }
    AddLine(c, "Revenue", LC_BLUE, 0.5f, {1.2f, 1.8f, 2.1f, 1.5f, 2.4f, 3.1f, 2.8f});
    return c;
  }

  // 20. Formatter (custom tooltip)
  ChartView MakeFormatter(float w, float h)
  {
    auto c = NewLine(w, h);
    c.SetProperty(ChartView::Property::SHOW_TOOLTIP, true);
    c.SetProperty(ChartView::Property::SHOW_LEGEND, true);
    c.SetTooltipFormatter([](const Dali::String& name, const Dali::String& xLabel, float y)
    {
      char buf[48];
      std::snprintf(buf, sizeof(buf), "%s @ %s: %.0f%%", name.CStr(), xLabel.CStr(), y);
      return Dali::String(buf);
    });
    {
      ChartAxis chartAxis12 = ChartAxis::New();
      chartAxis12.SetLabels({"Mon", "Tue", "Wed", "Thu", "Fri"});
      c.SetXAxis(chartAxis12);
    }
    AddLine(c, "CPU", LC_BLUE, 0.65f, {42, 58, 71, 49, 83});
    AddLine(c, "Memory", LC_PURPLE, 0.65f, {65, 72, 68, 74, 79});
    return c;
  }

  // 21. Sections (Full) — X band, Y band, H-line, V-line, rect (Scene 17)
  ChartView MakeSectionsFull(float w, float h)
  {
    auto c = NewLine(w, h);
    c.SetProperty(ChartView::Property::SHOW_MARKERS, true);
    c.SetProperty(ChartView::Property::SHOW_LEGEND, true);
    {
      ChartAxis chartAxis13 = ChartAxis::New();
      chartAxis13.SetLabels({"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                             "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"});
      c.SetXAxis(chartAxis13);
    }

    {
      LineSeries lineSeries8 = LineSeries::New();
      lineSeries8.SetColor(MD_BLUE);
      lineSeries8.SetMarkersVisible(true);
      lineSeries8.SetName("Revenue");
      lineSeries8.SetValues({120, 150, 130, 190, 210, 250, 230, 280, 260, 300, 290, 320});
      c.AddSeries(lineSeries8);
    }

    {
      LineSeries lineSeries9 = LineSeries::New();
      lineSeries9.SetColor(MD_RED);
      lineSeries9.SetMarkersVisible(true);
      lineSeries9.SetName("Cost");
      lineSeries9.SetValues({80, 95, 90, 110, 130, 150, 140, 170, 160, 190, 180, 200});
      c.AddSeries(lineSeries9);
    }

    // X band: Q2 (green)
    {
      ChartSection chartSection2 = ChartSection::New();
      chartSection2.SetMinimumX(2.5f);
      chartSection2.SetMaximumX(5.5f);
      chartSection2.SetFillColor(Vector4(0.2f, 0.7f, 0.3f, 0.12f));
      chartSection2.SetStrokeColor(Vector4(0.2f, 0.7f, 0.3f, 0.5f));
      chartSection2.SetStrokeWidth(1.5f);
      c.AddSection(chartSection2);
    }

    // Y band: target range 200~280 (yellow)
    {
      ChartSection chartSection3 = ChartSection::New();
      chartSection3.SetMinimumY(200.0f);
      chartSection3.SetMaximumY(280.0f);
      chartSection3.SetFillColor(Vector4(1.0f, 0.85f, 0.2f, 0.10f));
      c.AddSection(chartSection3);
    }

    // H-line: Y=250 (red)
    {
      ChartSection chartSection4 = ChartSection::New();
      chartSection4.SetMinimumY(250.0f);
      chartSection4.SetMaximumY(250.0f);
      chartSection4.SetStrokeColor(Vector4(1.0f, 0.3f, 0.3f, 0.9f));
      chartSection4.SetStrokeWidth(2.0f);
      c.AddSection(chartSection4);
    }

    // V-line: July (blue)
    {
      ChartSection chartSection5 = ChartSection::New();
      chartSection5.SetMinimumX(6.0f);
      chartSection5.SetMaximumX(6.0f);
      chartSection5.SetStrokeColor(Vector4(0.4f, 0.4f, 1.0f, 0.8f));
      chartSection5.SetStrokeWidth(2.0f);
      c.AddSection(chartSection5);
    }

    // Rect: Q4 high zone Oct~Dec, Y 270~320 (purple)
    {
      ChartSection chartSection6 = ChartSection::New();
      chartSection6.SetMinimumX(8.5f);
      chartSection6.SetMaximumX(11.5f);
      chartSection6.SetMinimumY(270.0f);
      chartSection6.SetMaximumY(320.0f);
      chartSection6.SetFillColor(Vector4(0.6f, 0.2f, 1.0f, 0.12f));
      chartSection6.SetStrokeColor(Vector4(0.6f, 0.2f, 1.0f, 0.6f));
      chartSection6.SetStrokeWidth(1.5f);
      c.AddSection(chartSection6);
    }
    return c;
  }

  // 22. Pie Chart — Browser Market Share (Scene 23)
  ChartView MakePieChart(float w, float h)
  {
    auto c = ChartView::New(ChartView::Type::PIE, Vector2(w, h));
    c.SetProperty(ChartView::Property::SHOW_LEGEND, true);
    PieSeries pie = PieSeries::New();
    pie.AddSlice("Chrome", 63.0f, MD_BLUE);
    pie.AddSlice("Safari", 19.0f, MD_RED);
    pie.AddSlice("Edge", 11.0f, MD_GREEN);
    pie.AddSlice("Firefox", 4.0f, LC_PURPLE);
    pie.AddSlice("Other", 3.0f, MD_BGRAY);
    pie.SetDataLabelsVisible(true);
    c.AddSeries(pie);
    return c;
  }

  // ── Helpers ───────────────────────────────────────────────────────────────
  ChartView NewLine(float w, float h)
  {
    auto c = ChartView::New(ChartView::Type::LINE, Vector2(w, h));
    Light(c);
    return c;
  }

  void Light(ChartView& c)
  {
    c.SetProperty(ChartView::Property::BACKGROUND_COLOR, LC_BG);
    c.SetProperty(ChartView::Property::GRID_COLOR, LC_GRID);
    c.SetProperty(ChartView::Property::SHOW_GRID, true);
  }

  void AddLine(ChartView& c, const char* name, const Vector4& col,
               float smooth, std::vector<float> vals)
  {
    {
      LineSeries lineSeries10 = LineSeries::New();
      lineSeries10.SetColor(col);
      lineSeries10.SetSmoothness(smooth);
      lineSeries10.SetName(name);
      lineSeries10.SetValues(ToValues(vals));
      c.AddSeries(lineSeries10);
    }
  }

  void OnKeyEvent(Window /*window*/, KeyEvent event)
  {
    if(event.GetState() == KeyEvent::DOWN &&
       (IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK)))
      mApplication.Quit();
  }

private:
  Application&           mApplication;
  AbsoluteLayout         mRootLayout;
  AbsoluteLayout         mContentArea;
  PanGestureDetector     mPanDetector;
  float                  mTotalH{0.0f};
  float                  mWinH{0.0f};
  float                  mCw{0.0f};
  float                  mCh{0.0f};
  float                  mRowH{0.0f};
  float                  mScrollY{0.0f};
  std::vector<ChartView> mAllCharts;
  std::vector<Label>     mEntryLabels;

  // Animation
  LineSeries mAnimSeries;
  Timer      mTimerAnim;
  bool       mAnimToggle{false};

  // Auto Updates
  LineSeries mAppendSeries;
  Timer      mTimerAppend;
  float      mAppendY{3.0f};

  // Gauge
  ChartView mGaugeChart;
  Timer     mTimerGauge;

  // Real Time
  LineSeries mSineSeries;
  Timer      mTimerSine;
  float      mSineT{0.0f};
};

// ---------------------------------------------------------------------------
int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  Components::UiConfig::New().Apply();
  ChartGalleryController controller(application);
  application.MainLoop();
  return 0;
}
