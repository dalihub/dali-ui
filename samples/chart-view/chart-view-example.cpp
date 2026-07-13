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
 * @file  chart-view-example.cpp
 * @brief ChartView verification sample — dali-ui port.
 *
 * Scene 1 — "Basic Chart Frame"       : empty chart frame (axes, grid, tick labels)
 * Scene 2 — "Multi-Series + Labels"   : Y auto-range, UpdateSeries, Label pool
 * Scene 3 — "Size Change Test"        : OnRelayout / RebuildBackground on resize
 * Scene 4 — "Marker Shapes & Labels"  : MarkerShape, DataLabel formatting
 * Scene 5 — "Area Chart"              : FillEnabled
 * Scene 6 — "Custom Styling"          : axis colors, grid dash, title position
 * Scene 7 — "Touch & Hover"           : DataPointSelectedSignal, SHOW_TOOLTIP toggle
 * Scene 8 — "Legend Signal & Toggle"  : LegendItemTappedSignal, SetLegendToggleEnabled
 * Scene 9 — "Input Enable/Disable"    : TOUCH_ENABLED / HOVER_ENABLED property cycle
 *
 * Navigation: left/right cursor keys OR Prev/Next push-buttons.
 */

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include <dali-ui-components/dali-ui-components.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using namespace Dali::Ui;

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------
namespace
{
const int   NUM_SCENES    = 25;
const float CHART_WIDTH   = 480.0f;
const float CHART_HEIGHT  = 360.0f;
const float CHART_SMALL_W = 300.0f;
const float CHART_SMALL_H = 200.0f;

// Toolkit-only color constants replaced with Vector4 literals
const Vector4 COLOR_CORNFLOWER_BLUE(0.39f, 0.58f, 0.93f, 1.0f);
const Vector4 COLOR_TOMATO(1.0f, 0.39f, 0.28f, 1.0f);
const Vector4 COLOR_DODGER_BLUE(0.12f, 0.56f, 1.0f, 1.0f);
const Vector4 COLOR_CORAL(1.0f, 0.5f, 0.31f, 1.0f);
const Vector4 COLOR_MEDIUM_PURPLE(0.58f, 0.44f, 0.86f, 1.0f);
const Vector4 COLOR_MEDIUM_SEA_GREEN(0.24f, 0.70f, 0.44f, 1.0f);
const Vector4 COLOR_DARK_RED(0.55f, 0.0f, 0.0f, 1.0f);
const Vector4 COLOR_DARK_SLATE_GRAY(0.18f, 0.31f, 0.31f, 1.0f);

const char* SCENE_TITLES[] = {
  "Scene 1: Basic Chart Frame",
  "Scene 2: Multi-Series + Labels",
  "Scene 3: Size Change Test",
  "Scene 4: Marker Shapes & Labels",
  "Scene 5: Area Chart",
  "Scene 6: Custom Styling",
  "Scene 7: Touch & Hover  [touch outside chart = toggle tooltip]",
  "Scene 8: Legend Signal & Toggle  [touch outside chart = toggle auto-toggle]",
  "Scene 9: Input Enable/Disable  [touch outside chart = cycle states]",
  "Scene 10: Formatter + AppendValue  [touch outside = add point]",
  "Scene 11: FindingStrategy SAME_X  [hover or touch]",
  "Scene 12: ZIndex + NaN Gap",
  "Scene 13: Animation  [touch outside = swap data]",
  "Scene 14: LineSmoothness  [touch outside = toggle 0/0.8]",
  "Scene 15: LabelsRotation + MinStep + DataPadding",
  "Scene 16: Zoom & Pan  [wheel=zoom, drag=pan, double-tap=reset]",
  "Scene 17: Sections  [X band, Y band, H-line, V-line, rect]",
  "Scene 18: ScatterSeries  [CIRCLE, SQUARE, TRIANGLE, DIAMOND + mixed]",
  "Scene 19: BarSeries  [grouped, negative, mixed line+bar]",
  "Scene 20: BarSeries + Zoom  [wheel=zoom, drag=pan, double-tap=reset]",
  "Scene 21: BarSeries Stacked  [grouped + stacked + mixed]",
  "Scene 22: Stacked + Zoom  [wheel=zoom, drag=pan, double-tap=reset]",
  "Scene 23: Pie Chart  [4 slices, legend]",
  "Scene 24: Donut Chart  [innerRadius=0.55, center label]",
  "Scene 25: Gauge Chart  [CPU usage, colored zones, touch=change value]"};

// Scene 2 — toggle data
const std::vector<float> DATA_A_INITIAL = {120.0f, 190.0f, 150.0f, 250.0f};
const std::vector<float> DATA_A_UPDATED = {200.0f, 350.0f, 280.0f, 420.0f};
const std::vector<float> DATA_B         = {80.0f, 230.0f, 170.0f, 310.0f};

} // namespace

// ---------------------------------------------------------------------------
// ChartViewController
// ---------------------------------------------------------------------------
class ChartViewController : public ConnectionTracker
{
public:
  explicit ChartViewController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &ChartViewController::Create);
  }

  ~ChartViewController() = default;

  // -------------------------------------------------------------------------
  // Lifecycle
  // -------------------------------------------------------------------------
  void Create(Application application)
  {
    Window  window     = application.GetWindow();
    auto    posSize    = window.GetPositionSize();
    Vector2 windowSize = Vector2(posSize.width, posSize.height);
    window.SetBackgroundColor(Color::LIGHT_GRAY);
    window.KeyEventSignal().Connect(this, &ChartViewController::OnKeyEvent);

    mWinW = windowSize.x;
    mWinH = windowSize.y;

    // --- Scene title label (top) ---
    mSceneLabel = Label::New();
    mSceneLabel.SetLayoutParams(AbsoluteLayoutParams::New()
                                  .SetX(10.0f)
                                  .SetY(4.0f)
                                  .SetWidth(mWinW - 20.0f)
                                  .SetHeight(36.0f));
    mSceneLabel.SetFontSize(9.0f);
    mSceneLabel.SetTextColor(UiColor(0x000000));
    mSceneLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mSceneLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);

    // --- Debug label (above navigation buttons) ---
    mDebugLabel = Label::New();
    mDebugLabel.SetLayoutParams(AbsoluteLayoutParams::New()
                                  .SetX(10.0f)
                                  .SetY(mWinH - 92.0f)
                                  .SetWidth(mWinW - 20.0f)
                                  .SetHeight(36.0f));
    mDebugLabel.SetFontSize(7.0f);
    mDebugLabel.SetTextColor(UiColor(COLOR_DARK_SLATE_GRAY));
    mDebugLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mDebugLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);

    // --- Prev button (bottom-left) ---
    mPrevButton = AbsoluteLayout::New();
    mPrevButton.SetRequestedWidth(100.0f);
    mPrevButton.SetRequestedHeight(50.0f);
    mPrevButton.SetLayoutParams(AbsoluteLayoutParams::New()
                                  .SetX(0.0f)
                                  .SetY(mWinH - 50.0f)
                                  .SetWidth(100.0f)
                                  .SetHeight(50.0f));
    mPrevButton.SetBackgroundColor(UiColor(0x424242, 0.85f));
    mPrevButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    { OnPrevScene(); });

    Label prevLabel = Label::New("Prev");
    prevLabel.SetRequestedWidth(MATCH_PARENT);
    prevLabel.SetRequestedHeight(MATCH_PARENT);
    prevLabel.SetFontSize(11.0f);
    prevLabel.SetTextColor(UiColor(0xFFFFFF));
    prevLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    prevLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mPrevButton.Add(prevLabel);

    // --- Next button (bottom-right) ---
    mNextButton = AbsoluteLayout::New();
    mNextButton.SetRequestedWidth(100.0f);
    mNextButton.SetRequestedHeight(50.0f);
    mNextButton.SetLayoutParams(AbsoluteLayoutParams::New()
                                  .SetX(mWinW - 100.0f)
                                  .SetY(mWinH - 50.0f)
                                  .SetWidth(100.0f)
                                  .SetHeight(50.0f));
    mNextButton.SetBackgroundColor(UiColor(0x424242, 0.85f));
    mNextButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    { OnNextScene(); });

    Label nextLabel = Label::New("Next");
    nextLabel.SetRequestedWidth(MATCH_PARENT);
    nextLabel.SetRequestedHeight(MATCH_PARENT);
    nextLabel.SetFontSize(11.0f);
    nextLabel.SetTextColor(UiColor(0xFFFFFF));
    nextLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    nextLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mNextButton.Add(nextLabel);

    // Single root layout — ALL UI in one hierarchy so the dali-ui layout
    // system processes everything, including ChartView::OnArrange.
    mRootLayout = AbsoluteLayout::New();
    mRootLayout.SetRequestedWidth(MATCH_PARENT);
    mRootLayout.SetRequestedHeight(MATCH_PARENT);
    mRootLayout.Add(mSceneLabel);
    mRootLayout.Add(mDebugLabel);
    mRootLayout.Add(mPrevButton);
    mRootLayout.Add(mNextButton);
    window.Add(mRootLayout);

    // Show first scene
    CreateScene(mCurrentScene);
  }

  // -------------------------------------------------------------------------
  // Scene management
  // -------------------------------------------------------------------------
  void CreateScene(int sceneIndex)
  {
    Window window = mApplication.GetWindow();

    // Update scene title label
    mSceneLabel.SetProperty(Label::Property::TEXT, SCENE_TITLES[sceneIndex]);
    mDebugLabel.SetProperty(Label::Property::TEXT, "");

    switch(sceneIndex)
    {
      case 0:
        CreateBasicFrameScene(window);
        break;
      case 1:
        CreateMultiSeriesScene(window);
        break;
      case 2:
        CreateSizeChangeScene(window);
        break;
      case 3:
        CreateMarkerShapesScene(window);
        break;
      case 4:
        CreateAreaChartScene(window);
        break;
      case 5:
        CreateCustomStylingScene(window);
        break;
      case 6:
        CreateTouchHoverScene(window);
        break;
      case 7:
        CreateLegendSignalScene(window);
        break;
      case 8:
        CreateInputControlScene(window);
        break;
      case 9:
        CreateFormatterAppendScene(window);
        break;
      case 10:
        CreateSameXStrategyScene(window);
        break;
      case 11:
        CreateZIndexNanGapScene(window);
        break;
      case 12:
        CreateAnimationScene(window);
        break;
      case 13:
        CreateSmoothnessScene(window);
        break;
      case 14:
        CreateAxisOptionsScene(window);
        break;
      case 15:
        CreateZoomPanScene(window);
        break;
      case 16:
        CreateSectionsScene(window);
        break;
      case 17:
        CreateScatterScene(window);
        break;
      case 18:
        CreateBarScene(window);
        break;
      case 19:
        CreateBarZoomScene(window);
        break;
      case 20:
        CreateStackedBarScene(window);
        break;
      case 21:
        CreateStackedBarZoomScene(window);
        break;
      case 22:
        CreatePieScene(window);
        break;
      case 23:
        CreateDonutScene(window);
        break;
      case 24:
        CreateGaugeScene(window);
        break;
    }
  }

  void ResetScene()
  {
    // Stop size timer if running
    if(mSizeTimer)
    {
      mSizeTimer.Stop();
      mSizeTimer.Reset();
    }

    // Remove and reset the chart
    if(mChart)
    {
      mRootLayout.Remove(mChart);
      mChart.Reset();
    }

    // Disconnect root-layer touch signals used by individual scenes
    Layer rootLayer = mApplication.GetWindow().GetRootLayer();
    rootLayer.TouchEventSignal().Disconnect(this, &ChartViewController::OnScene2Touch);
    rootLayer.TouchEventSignal().Disconnect(this, &ChartViewController::OnScene7Touch);
    rootLayer.TouchEventSignal().Disconnect(this, &ChartViewController::OnScene8Touch);
    rootLayer.TouchEventSignal().Disconnect(this, &ChartViewController::OnScene9Touch);
    rootLayer.TouchEventSignal().Disconnect(this, &ChartViewController::OnScene10Touch);
    rootLayer.TouchEventSignal().Disconnect(this, &ChartViewController::OnScene13Touch);
    rootLayer.TouchEventSignal().Disconnect(this, &ChartViewController::OnScene14Touch);
    rootLayer.TouchEventSignal().Disconnect(this, &ChartViewController::OnScene25Touch);

    mScene2TouchToggle  = false;
    mScene7ShowTooltip  = true;
    mScene8LegendToggle = true;
    mScene9InputState   = 0;
  }

  // -------------------------------------------------------------------------
  // Scene 1: Basic Chart Frame
  // — Tests rendering of empty chart frame (axes, grid, tick labels, titles)
  // -------------------------------------------------------------------------
  void CreateBasicFrameScene(Window window)
  {
    auto t0 = std::chrono::high_resolution_clock::now();

    mChart = ChartView::New(ChartView::Type::LINE,
                            Vector2(CHART_WIDTH, CHART_HEIGHT));

    // Axes and titles
    mChart.SetTitle("Monthly Sales");

    {
      ChartAxis chartAxis1 = ChartAxis::New();
      chartAxis1.SetTitle("Month");
      chartAxis1.SetLabels({"Jan", "Feb", "Mar", "Apr", "May", "Jun"});
      mChart.SetXAxis(chartAxis1);
    }

    {
      ChartAxis chartAxis2 = ChartAxis::New();
      chartAxis2.SetTitle("Amount");
      chartAxis2.SetMinLimit(0.0f);
      chartAxis2.SetMaxLimit(500.0f);
      mChart.SetYAxis(chartAxis2);
    }

    // Style properties
    mChart.SetProperty(ChartView::Property::SHOW_GRID, true);
    mChart.SetProperty(ChartView::Property::BACKGROUND_COLOR, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    mChart.SetProperty(ChartView::Property::AXIS_LABEL_SIZE, 8.0f);
    mChart.SetProperty(ChartView::Property::TITLE_SIZE, 11.0f);

    // Center the chart on screen
    AddChartToRoot();

    auto t1      = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

    std::ostringstream oss;
    oss << "RebuildBackground: " << elapsed << " ms  |  "
        << "Chart: " << CHART_WIDTH << "x" << CHART_HEIGHT;
    mDebugLabel.SetProperty(Label::Property::TEXT, oss.str().c_str());

    DALI_LOG_DEBUG_INFO("Scene1 CreateBasicFrameScene elapsed: %lldms\n", (long long)elapsed);
  }

  // -------------------------------------------------------------------------
  // Scene 2: Multi-Series + Labels
  // — Tests Y auto-range, UpdateSeries, Label pool reuse
  // -------------------------------------------------------------------------
  void CreateMultiSeriesScene(Window window)
  {
    mChart = ChartView::New(ChartView::Type::LINE,
                            Vector2(CHART_WIDTH, CHART_HEIGHT));

    mChart.SetTitle("Sales Comparison");

    {
      ChartAxis chartAxis3 = ChartAxis::New();
      chartAxis3.SetTitle("Quarter");
      chartAxis3.SetLabels({"Q1", "Q2", "Q3", "Q4"});
      mChart.SetXAxis(chartAxis3);
    }

    {
      ChartAxis chartAxis4 = ChartAxis::New();
      chartAxis4.SetTitle("Revenue (M)");
      mChart.SetYAxis(chartAxis4);
    }

    mChart.SetProperty(ChartView::Property::SHOW_GRID, true);
    mChart.SetProperty(ChartView::Property::TOUCH_ENABLED, false);

    // Measure AddSeries rebuild time
    auto t0  = std::chrono::high_resolution_clock::now();
    mSeriesA = LineSeries::New();
    mSeriesA.SetName("Product A");
    mSeriesA.SetValues(DATA_A_INITIAL);
    mSeriesA.SetColor(Vector4(0.2f, 0.6f, 1.0f, 1.0f));
    mChart.AddSeries(mSeriesA);

    {
      LineSeries lineSeries1 = LineSeries::New();
      lineSeries1.SetColor(Vector4(1.0f, 0.4f, 0.2f, 1.0f));
      lineSeries1.SetName("Product B");
      lineSeries1.SetValues(DATA_B);
      mChart.AddSeries(lineSeries1);
    }
    auto t1      = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

    AddChartToRoot();

    UpdateDebugLabel(elapsed, "AddSeries");

    DALI_LOG_DEBUG_INFO("Scene2 AddSeries elapsed: %lldms\n", (long long)elapsed);

    // Connect touch for UpdateSeries test
    window.GetRootLayer().TouchEventSignal().Connect(this, &ChartViewController::OnScene2Touch);
  }

  // -------------------------------------------------------------------------
  // Scene 3: Size Change Test
  // — Timer alternates chart size every 2s to verify OnRelayout / RebuildBackground
  // -------------------------------------------------------------------------
  void CreateSizeChangeScene(Window window)
  {
    mChart = ChartView::New(ChartView::Type::LINE,
                            Vector2(CHART_SMALL_W, CHART_SMALL_H));

    mChart.SetTitle("Resize Test");

    {
      ChartAxis chartAxis5 = ChartAxis::New();
      chartAxis5.SetTitle("X");
      chartAxis5.SetLabels({"A", "B", "C", "D"});
      mChart.SetXAxis(chartAxis5);
    }

    {
      ChartAxis chartAxis6 = ChartAxis::New();
      chartAxis6.SetTitle("Y");
      mChart.SetYAxis(chartAxis6);
    }

    {
      LineSeries lineSeries2 = LineSeries::New();
      lineSeries2.SetName("Data");
      lineSeries2.SetValues({20.0f, 60.0f, 40.0f, 80.0f});
      mChart.AddSeries(lineSeries2);
    }

    mChart.SetProperty(ChartView::Property::SHOW_GRID, true);
    AddChartToRoot(CHART_SMALL_W, CHART_SMALL_H);

    mScene3SizeToggle = false;
    UpdateScene3DebugLabel();

    // Start timer — toggles size every 2 seconds
    mSizeTimer = Timer::New(2000);
    mSizeTimer.TickSignal().Connect(this, &ChartViewController::OnSizeTimer);
    mSizeTimer.Start();
  }

  // -------------------------------------------------------------------------
  // Timers and touch handlers
  // -------------------------------------------------------------------------
  bool OnSizeTimer()
  {
    mScene3SizeToggle = !mScene3SizeToggle;

    Vector2 newSize = mScene3SizeToggle
                        ? Vector2(CHART_WIDTH, CHART_HEIGHT)
                        : Vector2(CHART_SMALL_W, CHART_SMALL_H);

    auto t0 = std::chrono::high_resolution_clock::now();
    mChart.SetLayoutParams(AbsoluteLayoutParams::New()
                             .SetX((mWinW - newSize.x) * 0.5f)
                             .SetY((mWinH - newSize.y) * 0.5f - 20.0f)
                             .SetWidth(newSize.x)
                             .SetHeight(newSize.y));
    mRootLayout.Remove(mChart);
    mRootLayout.Add(mChart);
    auto t1      = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

    DALI_LOG_DEBUG_INFO("Scene3 SIZE_SET to (%.0f x %.0f) elapsed: %lldms\n",
                        newSize.x, newSize.y, (long long)elapsed);
    UpdateScene3DebugLabel();
    return true; // keep timer running
  }

  bool OnScene2Touch(Actor actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::DOWN && mCurrentScene == 1)
    {
      mScene2TouchToggle = !mScene2TouchToggle;

      const std::vector<float>& newData = mScene2TouchToggle ? DATA_A_UPDATED : DATA_A_INITIAL;

      auto t0 = std::chrono::high_resolution_clock::now();
      mSeriesA.SetValues(newData);
      auto t1      = std::chrono::high_resolution_clock::now();
      auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

      UpdateDebugLabel(elapsed, "UpdateSeries");
      DALI_LOG_DEBUG_INFO("Scene2 UpdateSeries elapsed: %lldms\n", (long long)elapsed);
    }
    return false; // don't consume
  }

  // -------------------------------------------------------------------------
  // Navigation
  // -------------------------------------------------------------------------
  void OnPrevScene()
  {
    ResetScene();
    mCurrentScene = (mCurrentScene - 1 + NUM_SCENES) % NUM_SCENES;
    CreateScene(mCurrentScene);
  }

  void OnNextScene()
  {
    ResetScene();
    mCurrentScene = (mCurrentScene + 1) % NUM_SCENES;
    CreateScene(mCurrentScene);
  }

  void OnKeyEvent(Window /*window*/, KeyEvent event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
      else if(IsKey(event, DALI_KEY_CURSOR_LEFT))
      {
        ResetScene();
        mCurrentScene = (mCurrentScene - 1 + NUM_SCENES) % NUM_SCENES;
        CreateScene(mCurrentScene);
      }
      else if(IsKey(event, DALI_KEY_CURSOR_RIGHT))
      {
        ResetScene();
        mCurrentScene = (mCurrentScene + 1) % NUM_SCENES;
        CreateScene(mCurrentScene);
      }
    }
  }

  // -------------------------------------------------------------------------
  // Debug helpers
  // -------------------------------------------------------------------------
  void UpdateDebugLabel(long long elapsedMs, const char* operation)
  {
    std::ostringstream oss;
    oss << operation << ": " << elapsedMs << " ms  |  Touch to toggle UpdateSeries";
    mDebugLabel.SetProperty(Label::Property::TEXT, oss.str().c_str());
  }

  void UpdateScene3DebugLabel()
  {
    float              w = mScene3SizeToggle ? CHART_WIDTH : CHART_SMALL_W;
    float              h = mScene3SizeToggle ? CHART_HEIGHT : CHART_SMALL_H;
    std::ostringstream oss;
    oss << "Chart size: " << w << " x " << h << "  (changes every 2s)";
    mDebugLabel.SetProperty(Label::Property::TEXT, oss.str().c_str());
  }

  void CreateMarkerShapesScene(Window window)
  {
    mChart = ChartView::New(ChartView::Type::LINE, Vector2(CHART_WIDTH, CHART_HEIGHT));
    mChart.SetTitle("Marker Shapes & Data Labels");

    {
      ChartAxis chartAxis7 = ChartAxis::New();
      chartAxis7.SetLabels({"Circle", "Square", "Triangle", "Diamond"});
      mChart.SetXAxis(chartAxis7);
    }

    mChart.SetYAxis(ChartAxis::New());

    // Circle (Default)
    {
      LineSeries lineSeries3 = LineSeries::New();
      lineSeries3.SetColor(Color::BLUE);
      lineSeries3.SetMarkerShape(LineSeries::MarkerShape::CIRCLE);
      lineSeries3.SetDataLabelsVisible(true);
      lineSeries3.SetValues({10.0f, 20.0f, 15.0f, 25.0f});
      mChart.AddSeries(lineSeries3);
    }

    // Square
    {
      LineSeries lineSeries4 = LineSeries::New();
      lineSeries4.SetColor(Color::RED);
      lineSeries4.SetMarkerShape(LineSeries::MarkerShape::SQUARE);
      lineSeries4.SetMarkerRadius(6.0f);
      lineSeries4.SetDataLabelsVisible(true);
      lineSeries4.SetValues({20.0f, 30.0f, 25.0f, 35.0f});
      mChart.AddSeries(lineSeries4);
    }

    // Triangle
    {
      LineSeries lineSeries5 = LineSeries::New();
      lineSeries5.SetColor(Color::GREEN);
      lineSeries5.SetMarkerShape(LineSeries::MarkerShape::TRIANGLE);
      lineSeries5.SetMarkerColor(Color::YELLOW);
      lineSeries5.SetMarkerBorderColor(Color::BLACK);
      lineSeries5.SetMarkerBorderWidth(1.0f);
      lineSeries5.SetDataLabelsVisible(true);
      lineSeries5.SetValues({30.0f, 40.0f, 35.0f, 45.0f});
      mChart.AddSeries(lineSeries5);
    }

    // Diamond
    {
      LineSeries lineSeries6 = LineSeries::New();
      lineSeries6.SetColor(Color::MAGENTA);
      lineSeries6.SetMarkerShape(LineSeries::MarkerShape::DIAMOND);
      lineSeries6.SetDataLabelsVisible(true);
      lineSeries6.SetDataLabelFormat("Value: %.0f");
      lineSeries6.SetValues({40.0f, 50.0f, 45.0f, 55.0f});
      mChart.AddSeries(lineSeries6);
    }

    AddChartToRoot();
  }

  void CreateAreaChartScene(Window window)
  {
    mChart = ChartView::New(ChartView::Type::LINE, Vector2(CHART_WIDTH, CHART_HEIGHT));
    mChart.SetTitle("Area Chart (Fill Enabled)");

    {
      ChartAxis chartAxis8 = ChartAxis::New();
      chartAxis8.SetLabels({"Jan", "Feb", "Mar", "Apr", "May"});
      mChart.SetXAxis(chartAxis8);
    }

    {
      LineSeries lineSeries7 = LineSeries::New();
      lineSeries7.SetColor(Vector4(0.2f, 0.4f, 0.8f, 1.0f));
      lineSeries7.SetFillEnabled(true);
      lineSeries7.SetFillColor(Vector4(0.2f, 0.4f, 0.8f, 0.3f));
      lineSeries7.SetValues({50, 80, 60, 100, 70});
      mChart.AddSeries(lineSeries7);
    }

    AddChartToRoot();
  }

  // -------------------------------------------------------------------------
  // Scene 7: Touch & Hover Interaction
  // — DataPointSelectedSignal updates debug label
  // — SHOW_TOOLTIP toggled by touching outside the chart
  // -------------------------------------------------------------------------
  void CreateTouchHoverScene(Window window)
  {
    mChart = ChartView::New(ChartView::Type::LINE,
                            Vector2(CHART_WIDTH, CHART_HEIGHT));
    mChart.SetTitle("Touch or Hover a Data Point");
    mChart.SetProperty(ChartView::Property::SHOW_LEGEND, true);
    mChart.SetProperty(ChartView::Property::SHOW_GRID, true);

    {
      ChartAxis chartAxis9 = ChartAxis::New();
      chartAxis9.SetLabels({"Jan", "Feb", "Mar", "Apr", "May"});
      mChart.SetXAxis(chartAxis9);
    }

    mChart.SetYAxis(ChartAxis::New());

    {
      LineSeries lineSeries8 = LineSeries::New();
      lineSeries8.SetColor(Vector4(0.2f, 0.6f, 1.0f, 1.0f));
      lineSeries8.SetMarkersVisible(true);
      lineSeries8.SetName("Revenue");
      lineSeries8.SetValues({120.0f, 190.0f, 150.0f, 250.0f, 210.0f});
      mChart.AddSeries(lineSeries8);
    }

    {
      LineSeries lineSeries9 = LineSeries::New();
      lineSeries9.SetColor(Vector4(1.0f, 0.4f, 0.2f, 1.0f));
      lineSeries9.SetMarkersVisible(true);
      lineSeries9.SetName("Cost");
      lineSeries9.SetValues({80.0f, 110.0f, 130.0f, 160.0f, 140.0f});
      mChart.AddSeries(lineSeries9);
    }

    // DataPointSelectedSignal: update debug label with hit info
    mChart.DataPointSelectedSignal().Connect(this, &ChartViewController::OnScene7DataPointSelected);

    AddChartToRoot();

    mScene7ShowTooltip = true;
    mDebugLabel.SetProperty(Label::Property::TEXT,
                            "Touch/hover a point.  Outside chart = toggle tooltip");

    // Root-layer touch to toggle SHOW_TOOLTIP (Chart's own touch stays active)
    window.GetRootLayer().TouchEventSignal().Connect(this, &ChartViewController::OnScene7Touch);
  }

  void OnScene7DataPointSelected(const ChartPointEventArgs& e)
  {
    std::ostringstream oss;
    oss << "Hit  series=" << e.seriesIndex << "  point=" << e.pointIndex
        << "  [" << e.xLabel.CStr() << "] = " << e.dataY
        << "  |  outside chart = toggle tooltip";
    mDebugLabel.SetProperty(Label::Property::TEXT, oss.str().c_str());
  }

  bool OnScene7Touch(Actor /*actor*/, TouchEvent touch)
  {
    // Only react to touches that land outside the chart actor
    if(touch.GetState(0) != PointState::DOWN || mCurrentScene != 6) return false;

    // Check if the touch hit the chart (GetHitActor returns the deepest actor hit)
    // If hit actor is a descendant of mChart, don't handle here
    Actor hitActor = touch.GetHitActor(0);
    while(hitActor)
    {
      if(hitActor == mChart) return false; // inside chart — let chart handle it
      hitActor = hitActor.GetParent();
    }

    mScene7ShowTooltip = !mScene7ShowTooltip;
    mChart.SetProperty(ChartView::Property::SHOW_TOOLTIP, mScene7ShowTooltip);

    std::ostringstream oss;
    oss << "Tooltip: " << (mScene7ShowTooltip ? "ON" : "OFF")
        << "  |  Touch/hover a data point";
    mDebugLabel.SetProperty(Label::Property::TEXT, oss.str().c_str());
    return false;
  }

  // -------------------------------------------------------------------------
  // Scene 8: Legend Signal & Auto-Toggle Control
  // — LegendItemTappedSignal shows which series was tapped
  // — Touch outside chart toggles SetLegendToggleEnabled(false/true)
  // -------------------------------------------------------------------------
  void CreateLegendSignalScene(Window window)
  {
    mChart = ChartView::New(ChartView::Type::LINE,
                            Vector2(CHART_WIDTH, CHART_HEIGHT));
    mChart.SetTitle("Legend Interaction");
    mChart.SetProperty(ChartView::Property::SHOW_LEGEND, true);
    mChart.SetProperty(ChartView::Property::SHOW_GRID, true);

    {
      ChartAxis chartAxis10 = ChartAxis::New();
      chartAxis10.SetLabels({"Q1", "Q2", "Q3", "Q4"});
      mChart.SetXAxis(chartAxis10);
    }

    mChart.SetYAxis(ChartAxis::New());

    {
      LineSeries lineSeries10 = LineSeries::New();
      lineSeries10.SetColor(Vector4(0.2f, 0.6f, 1.0f, 1.0f));
      lineSeries10.SetMarkersVisible(true);
      lineSeries10.SetName("Alpha");
      lineSeries10.SetValues({100.0f, 140.0f, 120.0f, 180.0f});
      mChart.AddSeries(lineSeries10);
    }

    {
      LineSeries lineSeries11 = LineSeries::New();
      lineSeries11.SetColor(Vector4(0.9f, 0.3f, 0.2f, 1.0f));
      lineSeries11.SetMarkersVisible(true);
      lineSeries11.SetName("Beta");
      lineSeries11.SetValues({60.0f, 90.0f, 80.0f, 130.0f});
      mChart.AddSeries(lineSeries11);
    }

    {
      LineSeries lineSeries12 = LineSeries::New();
      lineSeries12.SetColor(Vector4(0.2f, 0.75f, 0.35f, 1.0f));
      lineSeries12.SetMarkersVisible(true);
      lineSeries12.SetName("Gamma");
      lineSeries12.SetValues({40.0f, 70.0f, 55.0f, 95.0f});
      mChart.AddSeries(lineSeries12);
    }

    // LegendItemTappedSignal: show tap info in debug label
    mChart.LegendItemTappedSignal().Connect(this, &ChartViewController::OnScene8LegendTapped);

    AddChartToRoot();

    mScene8LegendToggle = true; // auto-toggle ON by default
    mDebugLabel.SetProperty(Label::Property::TEXT,
                            "Tap legend items.  Auto-toggle: ON  |  Outside = switch mode");

    window.GetRootLayer().TouchEventSignal().Connect(this, &ChartViewController::OnScene8Touch);
  }

  void OnScene8LegendTapped(int seriesIndex, bool isNowVisible)
  {
    std::ostringstream oss;
    oss << "Legend tapped: series " << seriesIndex
        << "  ->  " << (isNowVisible ? "VISIBLE" : "HIDDEN")
        << "  |  Auto-toggle: " << (mScene8LegendToggle ? "ON" : "OFF (signal-only)");
    mDebugLabel.SetProperty(Label::Property::TEXT, oss.str().c_str());
  }

  bool OnScene8Touch(Actor /*actor*/, TouchEvent touch)
  {
    if(touch.GetState(0) != PointState::DOWN || mCurrentScene != 7) return false;

    Actor hitActor = touch.GetHitActor(0);
    while(hitActor)
    {
      if(hitActor == mChart) return false;
      hitActor = hitActor.GetParent();
    }

    mScene8LegendToggle = !mScene8LegendToggle;
    mChart.SetLegendToggleEnabled(mScene8LegendToggle);

    std::ostringstream oss;
    oss << "Auto-toggle: " << (mScene8LegendToggle ? "ON  (tap legend = hide/show)" : "OFF  (tap legend = signal only)")
        << "  |  Tap a legend item";
    mDebugLabel.SetProperty(Label::Property::TEXT, oss.str().c_str());
    return false;
  }

  // -------------------------------------------------------------------------
  // Scene 9: Input Enable/Disable (TOUCH_ENABLED / HOVER_ENABLED)
  // — Touch outside chart cycles: Both ON -> Touch OFF -> Hover OFF -> Both OFF -> Both ON
  // -------------------------------------------------------------------------
  void CreateInputControlScene(Window window)
  {
    mChart = ChartView::New(ChartView::Type::LINE,
                            Vector2(CHART_WIDTH, CHART_HEIGHT));
    mChart.SetTitle("Input Enable / Disable");
    mChart.SetProperty(ChartView::Property::SHOW_LEGEND, true);
    mChart.SetProperty(ChartView::Property::SHOW_GRID, true);

    {
      ChartAxis chartAxis11 = ChartAxis::New();
      chartAxis11.SetLabels({"Mon", "Tue", "Wed", "Thu", "Fri"});
      mChart.SetXAxis(chartAxis11);
    }

    mChart.SetYAxis(ChartAxis::New());

    {
      LineSeries lineSeries13 = LineSeries::New();
      lineSeries13.SetColor(Vector4(0.2f, 0.5f, 0.9f, 1.0f));
      lineSeries13.SetMarkersVisible(true);
      lineSeries13.SetName("Series A");
      lineSeries13.SetValues({30.0f, 70.0f, 50.0f, 90.0f, 60.0f});
      mChart.AddSeries(lineSeries13);
    }

    {
      LineSeries lineSeries14 = LineSeries::New();
      lineSeries14.SetColor(Vector4(0.9f, 0.5f, 0.1f, 1.0f));
      lineSeries14.SetMarkersVisible(true);
      lineSeries14.SetName("Series B");
      lineSeries14.SetValues({20.0f, 45.0f, 65.0f, 55.0f, 80.0f});
      mChart.AddSeries(lineSeries14);
    }

    AddChartToRoot();

    mScene9InputState = 0; // Both ON
    UpdateScene9DebugLabel();

    window.GetRootLayer().TouchEventSignal().Connect(this, &ChartViewController::OnScene9Touch);
  }

  bool OnScene9Touch(Actor /*actor*/, TouchEvent touch)
  {
    if(touch.GetState(0) != PointState::DOWN || mCurrentScene != 8) return false;

    Actor hitActor = touch.GetHitActor(0);
    while(hitActor)
    {
      if(hitActor == mChart) return false;
      hitActor = hitActor.GetParent();
    }

    mScene9InputState = (mScene9InputState + 1) % 4;

    // State cycle: 0=Both ON, 1=Touch OFF, 2=Hover OFF, 3=Both OFF
    const bool touchOn = (mScene9InputState == 0 || mScene9InputState == 2);
    const bool hoverOn = (mScene9InputState == 0 || mScene9InputState == 1);

    mChart.SetProperty(ChartView::Property::TOUCH_ENABLED, touchOn);
    mChart.SetProperty(ChartView::Property::HOVER_ENABLED, hoverOn);

    UpdateScene9DebugLabel();
    return false;
  }

  void UpdateScene9DebugLabel()
  {
    // State cycle: 0=Both ON, 1=Touch OFF, 2=Hover OFF, 3=Both OFF
    const bool touchOn = (mScene9InputState == 0 || mScene9InputState == 2);
    const bool hoverOn = (mScene9InputState == 0 || mScene9InputState == 1);

    std::ostringstream oss;
    oss << "Touch: " << (touchOn ? "ON " : "OFF")
        << "  |  Hover: " << (hoverOn ? "ON " : "OFF")
        << "  |  Touch outside = cycle states";
    mDebugLabel.SetProperty(Label::Property::TEXT, oss.str().c_str());
  }

  void CreateCustomStylingScene(Window window)
  {
    mChart = ChartView::New(ChartView::Type::LINE, Vector2(CHART_WIDTH, CHART_HEIGHT));
    mChart.SetProperty(ChartView::Property::SHOW_GRID, true);

    // Custom Title position and color
    mChart.SetTitle("Custom Chart Style");
    mChart.SetTitlePosition(ChartView::TitlePosition::TOP_LEFT);
    mChart.SetTitleColor(COLOR_DARK_RED);

    {
      ChartAxis chartAxis12 = ChartAxis::New();
      chartAxis12.SetAxisLineColor(Color::BLUE);
      chartAxis12.SetAxisLineWidth(3.0f);
      mChart.SetXAxis(chartAxis12);
    }

    {
      ChartAxis chartAxis13 = ChartAxis::New();
      chartAxis13.SetAxisLineColor(Color::GREEN);
      chartAxis13.SetAxisLineWidth(3.0f);
      chartAxis13.SetGridDash(2.0f, 2.0f);
      chartAxis13.SetGridColor(Vector4(0.0f, 0.5f, 1.0f, 0.9f));
      mChart.SetYAxis(chartAxis13);
    } // Light blue grid

    {
      LineSeries lineSeries15 = LineSeries::New();
      lineSeries15.SetColor(Color::BLUE);
      lineSeries15.SetLineWidth(4.0f);
      lineSeries15.SetValues({10, 30, 20, 50});
      mChart.AddSeries(lineSeries15);
    }

    AddChartToRoot();
  }

  // -------------------------------------------------------------------------
  // Scene 10: Formatter + AppendValue real-time
  // -------------------------------------------------------------------------
  void CreateFormatterAppendScene(Window window)
  {
    mChart = ChartView::New(ChartView::Type::LINE, Vector2(CHART_WIDTH, CHART_HEIGHT));
    mChart.SetProperty(ChartView::Property::SHOW_GRID, true);
    mChart.SetProperty(ChartView::Property::SHOW_MARKERS, true);
    mChart.SetProperty(ChartView::Property::SHOW_TOOLTIP, true);
    mChart.SetTitle("Real-time AppendValue (max 10 pts)");

    mChart.SetXAxis(ChartAxis::New());

    // Custom tooltip formatter
    mChart.SetTooltipFormatter([](const Dali::String& name, const Dali::String& xLabel, float y)
    {
      char buf[64];
      std::snprintf(buf, sizeof(buf), "[%s] x=%s  →  %.1f", name.CStr(), xLabel.CStr(), y);
      return Dali::String(buf);
    });

    mScene10Series = LineSeries::New();
    mScene10Series.SetName("Live");
    mScene10Series.SetColor(COLOR_MEDIUM_PURPLE);
    mScene10Series.SetMarkersVisible(true);
    mScene10Series.SetMaxDataPoints(10);

    // Custom data label formatter
    mScene10Series.SetDataLabelFormatter([](float value, int /*pi*/)
    {
      char buf[16];
      std::snprintf(buf, sizeof(buf), "%.0f", value);
      return Dali::String(buf);
    });

    // Seed with initial points
    mScene10Series.AppendValues({30.0f, 55.0f, 40.0f, 80.0f, 60.0f});

    mChart.AddSeries(mScene10Series);

    // SetUpdateThrottle controls the minimum interval between data-triggered redraws.
    // Default is 16ms (~60fps). For high-frequency sensors (e.g. every 5ms) use a
    // larger value to reduce rasterization load:
    //   mChart.SetUpdateThrottle(100.0f);  // redraw at most once per 100ms
    // Use 0 to disable throttling (immediate redraw on every AppendValue):
    //   mChart.SetUpdateThrottle(0.0f);

    AddChartToRoot();

    mScene10NextY = 70.0f;
    mDebugLabel.SetProperty(Label::Property::TEXT,
                            "Touch outside chart = append random point (max 10)");

    window.GetRootLayer().TouchEventSignal().Connect(this, &ChartViewController::OnScene10Touch);
  }

  bool OnScene10Touch(Actor /*actor*/, TouchEvent event)
  {
    if(event.GetState(0) != PointState::STARTED) return false;
    // Only react to touches outside the chart
    Vector2 local    = event.GetLocalPosition(0u);
    Vector3 chartPos = mChart.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION);
    Vector3 chartSz  = mChart.GetCurrentProperty<Vector3>(Actor::Property::SIZE);
    float   l        = chartPos.x - chartSz.x * 0.5f;
    float   r        = chartPos.x + chartSz.x * 0.5f;
    float   t        = chartPos.y - chartSz.y * 0.5f;
    float   b        = chartPos.y + chartSz.y * 0.5f;
    if(local.x >= l && local.x <= r && local.y >= t && local.y <= b) return false;

    Scene10AppendPoint();
    return true;
  }

  void Scene10AppendPoint()
  {
    mScene10NextY = 20.0f + std::fmod(mScene10NextY * 1.37f + 13.0f, 80.0f);
    mScene10Series.AppendValue(mScene10NextY);
  }

  // -------------------------------------------------------------------------
  // Scene 11: FindingStrategy SAME_X — multi-series tooltip
  // -------------------------------------------------------------------------
  void CreateSameXStrategyScene(Window window)
  {
    mChart = ChartView::New(ChartView::Type::LINE, Vector2(CHART_WIDTH, CHART_HEIGHT));
    mChart.SetProperty(ChartView::Property::SHOW_GRID, true);
    mChart.SetProperty(ChartView::Property::SHOW_MARKERS, true);
    mChart.SetProperty(ChartView::Property::SHOW_TOOLTIP, true);
    mChart.SetProperty(ChartView::Property::SHOW_LEGEND, true);
    mChart.SetTitle("SAME_X Strategy — hover to compare");

    // Use SAME_X so all series at the same X index are shown in tooltip
    mChart.SetFindingStrategy(ChartView::FindingStrategy::SAME_X);

    // Connect MultiPointSelectedSignal to show in debug label
    mChart.MultiPointSelectedSignal().Connect(this, &ChartViewController::OnMultiPointSelected);

    {
      ChartAxis chartAxis14 = ChartAxis::New();
      chartAxis14.SetLabels({"Jan", "Feb", "Mar", "Apr", "May", "Jun"});
      mChart.SetXAxis(chartAxis14);
    }

    {
      LineSeries lineSeries16 = LineSeries::New();
      lineSeries16.SetColor(COLOR_CORNFLOWER_BLUE);
      lineSeries16.SetMarkersVisible(true);
      lineSeries16.SetName("Revenue");
      lineSeries16.SetValues({120.0f, 190.0f, 150.0f, 250.0f, 220.0f, 280.0f});
      mChart.AddSeries(lineSeries16);
    }

    {
      LineSeries lineSeries17 = LineSeries::New();
      lineSeries17.SetColor(COLOR_CORAL);
      lineSeries17.SetMarkersVisible(true);
      lineSeries17.SetName("Cost");
      lineSeries17.SetValues({80.0f, 100.0f, 90.0f, 130.0f, 110.0f, 150.0f});
      mChart.AddSeries(lineSeries17);
    }

    {
      LineSeries lineSeries18 = LineSeries::New();
      lineSeries18.SetColor(COLOR_MEDIUM_SEA_GREEN);
      lineSeries18.SetMarkersVisible(true);
      lineSeries18.SetName("Profit");
      lineSeries18.SetValues({40.0f, 90.0f, 60.0f, 120.0f, 110.0f, 130.0f});
      mChart.AddSeries(lineSeries18);
    }

    AddChartToRoot();

    mDebugLabel.SetProperty(Label::Property::TEXT,
                            "Hover/touch to see all series at same X");
  }

  void OnMultiPointSelected(const ChartPointEventArgs& e)
  {
    std::ostringstream oss;
    oss << "MultiPoint: series=" << e.seriesIndex << " [" << e.xLabel.CStr() << "] = " << e.dataY;
    mDebugLabel.SetProperty(Label::Property::TEXT, oss.str().c_str());
  }

  // -------------------------------------------------------------------------
  // Scene 12: ZIndex + NaN Gap
  // -------------------------------------------------------------------------
  void CreateZIndexNanGapScene(Window window)
  {
    mChart = ChartView::New(ChartView::Type::LINE, Vector2(CHART_WIDTH, CHART_HEIGHT));
    mChart.SetProperty(ChartView::Property::SHOW_GRID, true);
    mChart.SetProperty(ChartView::Property::SHOW_MARKERS, true);
    mChart.SetTitle("ZIndex + NaN Gap");

    {
      ChartAxis chartAxis15 = ChartAxis::New();
      chartAxis15.SetLabels({"A", "B", "C", "D", "E", "F", "G"});
      mChart.SetXAxis(chartAxis15);
    }

    // s1: has a NaN gap at index 3 → line breaks between C and E
    {
      LineSeries lineSeries19 = LineSeries::New();
      lineSeries19.SetColor(COLOR_DODGER_BLUE);
      lineSeries19.SetMarkersVisible(true);
      lineSeries19.SetZIndex(10);
      lineSeries19.SetName("Gap Series (ZIndex=10)");
      lineSeries19.SetValues({100.0f, 150.0f, 120.0f,
                              std::numeric_limits<float>::quiet_NaN(),
                              200.0f, 180.0f, 220.0f});
      mChart.AddSeries(lineSeries19);
    }

    // s2: solid line, rendered below s1 (ZIndex=0 default)
    {
      LineSeries lineSeries20 = LineSeries::New();
      lineSeries20.SetColor(COLOR_TOMATO);
      lineSeries20.SetLineWidth(6.0f);
      lineSeries20.SetMarkersVisible(true);
      lineSeries20.SetZIndex(0);
      lineSeries20.SetName("Base Series (ZIndex=0)");
      lineSeries20.SetValues({80.0f, 130.0f, 160.0f, 140.0f, 170.0f, 160.0f, 190.0f});
      mChart.AddSeries(lineSeries20);
    }

    AddChartToRoot();

    mDebugLabel.SetProperty(Label::Property::TEXT,
                            "Blue line breaks at D (NaN). Blue drawn on top (ZIndex=10).");
  }

  // -------------------------------------------------------------------------
  // Scene 13: Animation — touch outside to swap data sets
  // -------------------------------------------------------------------------
  void CreateAnimationScene(Window window)
  {
    mChart = ChartView::New(ChartView::Type::LINE, Vector2(CHART_WIDTH, CHART_HEIGHT));
    mChart.SetProperty(ChartView::Property::SHOW_GRID, true);
    mChart.SetProperty(ChartView::Property::SHOW_MARKERS, true);
    mChart.SetTitle("Animation (300ms EASE_OUT)");

    // Enable 300ms animation with ease-out easing
    mChart.SetAnimationDuration(300.0f);
    mChart.SetAnimationEasing(ChartView::EasingType::EASE_OUT);

    ChartAxis xAxis = ChartAxis::New();
    xAxis.SetLabels({"Jan", "Feb", "Mar", "Apr", "May", "Jun"});
    mChart.SetXAxis(xAxis);

    mScene13Series = LineSeries::New();
    mScene13Series.SetName("Revenue");
    mScene13Series.SetValues({120.0f, 190.0f, 150.0f, 250.0f, 220.0f, 280.0f});
    mScene13Series.SetColor(COLOR_CORNFLOWER_BLUE);
    mScene13Series.SetMarkersVisible(true);
    mChart.AddSeries(mScene13Series);

    AddChartToRoot();

    mScene13Toggle = false;
    mDebugLabel.SetProperty(Label::Property::TEXT,
                            "Touch outside chart = swap data (animates)");
    window.GetRootLayer().TouchEventSignal().Connect(this, &ChartViewController::OnScene13Touch);
  }

  bool OnScene13Touch(Actor /*actor*/, TouchEvent event)
  {
    if(event.GetState(0) != PointState::STARTED) return false;
    // Only react outside chart
    Vector3 cs = mChart.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION);
    Vector3 sz = mChart.GetCurrentProperty<Vector3>(Actor::Property::SIZE);
    Vector2 lp = event.GetLocalPosition(0u);
    if(lp.x >= cs.x - sz.x * 0.5f && lp.x <= cs.x + sz.x * 0.5f &&
       lp.y >= cs.y - sz.y * 0.5f && lp.y <= cs.y + sz.y * 0.5f) return false;

    mScene13Toggle = !mScene13Toggle;
    if(mScene13Toggle)
      mScene13Series.SetValues({60.0f, 40.0f, 280.0f, 100.0f, 310.0f, 180.0f});
    else
      mScene13Series.SetValues({120.0f, 190.0f, 150.0f, 250.0f, 220.0f, 280.0f});
    return true;
  }

  // -------------------------------------------------------------------------
  // Scene 14: LineSmoothness — toggle between 0.0 and 0.8
  // -------------------------------------------------------------------------
  void CreateSmoothnessScene(Window window)
  {
    mChart = ChartView::New(ChartView::Type::LINE, Vector2(CHART_WIDTH, CHART_HEIGHT));
    mChart.SetProperty(ChartView::Property::SHOW_GRID, true);
    mChart.SetProperty(ChartView::Property::SHOW_MARKERS, true);
    mChart.SetTitle("LineSmoothness  [touch outside = toggle]");

    ChartAxis xAxis = ChartAxis::New();
    xAxis.SetLabels({"A", "B", "C", "D", "E", "F", "G"});
    mChart.SetXAxis(xAxis);

    mScene14Series = LineSeries::New();
    mScene14Series.SetName("Smooth");
    mScene14Series.SetValues({50.0f, 180.0f, 90.0f, 240.0f, 130.0f, 210.0f, 160.0f});
    mScene14Series.SetColor(COLOR_MEDIUM_PURPLE);
    mScene14Series.SetMarkersVisible(true);
    mScene14Series.SetSmoothness(0.0f); // start straight
    mChart.AddSeries(mScene14Series);

    AddChartToRoot();

    mScene14Smooth = false;
    mDebugLabel.SetProperty(Label::Property::TEXT,
                            "Smoothness=0.0 (straight). Touch outside to toggle.");
    window.GetRootLayer().TouchEventSignal().Connect(this, &ChartViewController::OnScene14Touch);
  }

  bool OnScene14Touch(Actor /*actor*/, TouchEvent event)
  {
    if(event.GetState(0) != PointState::STARTED) return false;
    Vector3 cs = mChart.GetCurrentProperty<Vector3>(Actor::Property::WORLD_POSITION);
    Vector3 sz = mChart.GetCurrentProperty<Vector3>(Actor::Property::SIZE);
    Vector2 lp = event.GetLocalPosition(0u);
    if(lp.x >= cs.x - sz.x * 0.5f && lp.x <= cs.x + sz.x * 0.5f &&
       lp.y >= cs.y - sz.y * 0.5f && lp.y <= cs.y + sz.y * 0.5f) return false;

    mScene14Smooth = !mScene14Smooth;
    float s        = mScene14Smooth ? 0.8f : 0.0f;
    mScene14Series.SetSmoothness(s);
    std::ostringstream oss;
    oss << "Smoothness=" << s << ". Touch outside to toggle.";
    mDebugLabel.SetProperty(Label::Property::TEXT, oss.str().c_str());
    return true;
  }

  // -------------------------------------------------------------------------
  // Scene 15: LabelsRotation + MinStep + DataPadding
  // -------------------------------------------------------------------------
  void CreateAxisOptionsScene(Window window)
  {
    mChart = ChartView::New(ChartView::Type::LINE, Vector2(CHART_WIDTH, CHART_HEIGHT));
    mChart.SetProperty(ChartView::Property::SHOW_GRID, true);
    mChart.SetProperty(ChartView::Property::SHOW_MARKERS, true);
    mChart.SetTitle("LabelsRotation 45° + MinStep 50 + DataPadding 10%");

    // X axis: rotated labels (B-3) + 10% padding (B-5)
    {
      ChartAxis chartAxis16 = ChartAxis::New();
      chartAxis16.SetLabelsRotation(45.0f);
      chartAxis16.SetDataPadding(0.0f);
      chartAxis16.SetLabels({"January", "February", "March", "April", "May", "June"});
      mChart.SetXAxis(chartAxis16);
    }

    // Y axis: min step 50 (B-4) + 10% padding (B-5)
    {
      ChartAxis chartAxis17 = ChartAxis::New();
      chartAxis17.SetMinStep(50.0f);
      chartAxis17.SetDataPadding(0.1f);
      mChart.SetYAxis(chartAxis17);
    } // B-5: 10% headroom

    {
      LineSeries lineSeries21 = LineSeries::New();
      lineSeries21.SetColor(COLOR_DODGER_BLUE);
      lineSeries21.SetMarkersVisible(true);
      lineSeries21.SetName("Sales");
      lineSeries21.SetValues({120.0f, 190.0f, 150.0f, 250.0f, 220.0f, 280.0f});
      mChart.AddSeries(lineSeries21);
    }

    AddChartToRoot();

    mDebugLabel.SetProperty(Label::Property::TEXT,
                            "X labels rotated 45°  |  Y ticks at 50-unit steps  |  10% Y padding");
  }

  // -------------------------------------------------------------------------
  // Scene 16: Zoom & Pan
  // -------------------------------------------------------------------------
  void CreateZoomPanScene(Window window)
  {
    mChart = ChartView::New(ChartView::Type::LINE, Vector2(CHART_WIDTH, CHART_HEIGHT));
    mChart.SetProperty(ChartView::Property::SHOW_GRID, true);
    mChart.SetProperty(ChartView::Property::SHOW_MARKERS, true);
    mChart.SetProperty(ChartView::Property::SHOW_TOOLTIP, true);
    mChart.SetTitle("Zoom & Pan");

    // Monthly data for 2 years (24 points)
    std::vector<Dali::String> months;
    for(const char* yr : {"2024", "2025"})
      for(const char* mo : {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"})
      {
        std::string s = std::string(mo) + " " + yr;
        months.push_back(Dali::String(s.c_str()));
      }
    {
      ChartAxis chartAxis18 = ChartAxis::New();
      chartAxis18.SetLabelsRotation(45.0f);
      chartAxis18.SetLabels(months);
      mChart.SetXAxis(chartAxis18);
    }

    {
      ChartAxis chartAxis19 = ChartAxis::New();
      chartAxis19.SetMinStep(50.0f);
      mChart.SetYAxis(chartAxis19);
    }

    {
      LineSeries lineSeries22 = LineSeries::New();
      lineSeries22.SetColor(COLOR_CORNFLOWER_BLUE);
      lineSeries22.SetMarkersVisible(true);
      lineSeries22.SetName("Revenue");
      lineSeries22.SetValues({120, 150, 130, 190, 210, 250, 230, 280, 260, 300, 290, 320,
                              340, 310, 360, 400, 380, 420, 410, 460, 440, 490, 470, 510});
      mChart.AddSeries(lineSeries22);
    }

    {
      LineSeries lineSeries23 = LineSeries::New();
      lineSeries23.SetColor(COLOR_TOMATO);
      lineSeries23.SetMarkersVisible(true);
      lineSeries23.SetName("Cost");
      lineSeries23.SetValues({80, 95, 90, 110, 130, 150, 140, 170, 160, 190, 180, 200,
                              210, 200, 220, 250, 230, 260, 250, 280, 270, 300, 290, 310});
      mChart.AddSeries(lineSeries23);
    }

    // X축만 Pan/Zoom, Y축은 AutoFitY로 자동 맞춤 (주식 차트 동작)
    mChart.SetZoomMode(
      static_cast<int>(ChartView::ZoomMode::PAN_X) |
      static_cast<int>(ChartView::ZoomMode::ZOOM_X));
    mChart.SetZoomClampEnabled(true);
    mChart.SetAutoFitYOnPan(true);

    // ZoomedSignal: show current viewport in debug label
    mChart.ZoomedSignal().Connect(this, &ChartViewController::OnZoomed);

    mChart.SetProperty(ChartView::Property::SHOW_LEGEND, true);
    AddChartToRoot();

    mDebugLabel.SetProperty(Label::Property::TEXT,
                            "Wheel = zoom  |  Drag = pan  |  Double-tap = reset");
  }

  void OnZoomed(const ChartViewportArgs& vp)
  {
    std::ostringstream oss;
    oss << "X[" << std::fixed << std::setprecision(1) << vp.xMin << " ~ " << vp.xMax << "]"
        << "  Y[" << vp.yMin << " ~ " << vp.yMax << "]";
    mDebugLabel.SetProperty(Label::Property::TEXT, oss.str().c_str());
  }

  // -------------------------------------------------------------------------
  // Scene 19: BarSeries
  // -------------------------------------------------------------------------
  void CreateBarScene(Window window)
  {
    mChart = ChartView::New(ChartView::Type::LINE, Vector2(CHART_WIDTH, CHART_HEIGHT));
    mChart.SetProperty(ChartView::Property::SHOW_GRID, true);
    mChart.SetProperty(ChartView::Property::SHOW_LEGEND, true);
    mChart.SetTitle("BarSeries");

    // Category X axis
    {
      ChartAxis chartAxis20 = ChartAxis::New();
      chartAxis20.SetLabels({"Q1", "Q2", "Q3", "Q4"});
      mChart.SetXAxis(chartAxis20);
    }

    {
      ChartAxis chartAxis21 = ChartAxis::New();
      chartAxis21.SetMinStep(50.0f);
      mChart.SetYAxis(chartAxis21);
    }

    // ── Series 1: Revenue (grouped) ─────────────────────────────────────────
    {
      BarSeries barSeries1 = BarSeries::New();
      barSeries1.SetColor(COLOR_CORNFLOWER_BLUE);
      barSeries1.SetDataLabelsVisible(true);
      barSeries1.SetName("Revenue");
      barSeries1.SetValues({320, 410, 380, 450});
      mChart.AddSeries(barSeries1);
    }

    // ── Series 2: Cost (grouped) ────────────────────────────────────────────
    {
      BarSeries barSeries2 = BarSeries::New();
      barSeries2.SetColor(COLOR_TOMATO);
      barSeries2.SetDataLabelsVisible(true);
      barSeries2.SetName("Cost");
      barSeries2.SetValues({210, 250, 230, 280});
      mChart.AddSeries(barSeries2);
    }

    // ── Series 3: Profit (grouped) ──────────────────────────────────────────
    {
      BarSeries barSeries3 = BarSeries::New();
      barSeries3.SetColor(Vector4(0.2f, 0.75f, 0.3f, 1.0f));
      barSeries3.SetDataLabelsVisible(true);
      barSeries3.SetName("Profit");
      barSeries3.SetValues({110, 160, 150, 170});
      mChart.AddSeries(barSeries3);
    }

    // ── Series 4: Mixed LineSeries ────────────────────────────────────────
    {
      LineSeries lineSeries24 = LineSeries::New();
      lineSeries24.SetColor(Vector4(0.5f, 0.0f, 0.5f, 0.8f));
      lineSeries24.SetMarkersVisible(true);
      lineSeries24.SetName("Trend");
      lineSeries24.SetValues({320, 410, 380, 450});
      mChart.AddSeries(lineSeries24);
    }

    AddChartToRoot();

    mDebugLabel.SetProperty(Label::Property::TEXT,
                            "3 grouped bars (blue/red/green) + purple trend line");
  }

  // -------------------------------------------------------------------------
  // Scene 21: BarSeries Stacked
  // -------------------------------------------------------------------------
  void CreateStackedBarScene(Window window)
  {
    mChart = ChartView::New(ChartView::Type::LINE, Vector2(CHART_WIDTH, CHART_HEIGHT));
    mChart.SetProperty(ChartView::Property::SHOW_GRID, true);
    mChart.SetProperty(ChartView::Property::SHOW_LEGEND, true);
    mChart.SetTitle("BarSeries Stacked");

    {
      ChartAxis chartAxis22 = ChartAxis::New();
      chartAxis22.SetLabels({"Q1", "Q2", "Q3", "Q4"});
      mChart.SetXAxis(chartAxis22);
    }

    {
      ChartAxis chartAxis23 = ChartAxis::New();
      chartAxis23.SetMinStep(100.0f);
      mChart.SetYAxis(chartAxis23);
    }

    // ── Stacked group A + B + C ──────────────────────────────────────────────
    {
      BarSeries barSeries4 = BarSeries::New();
      barSeries4.SetColor(COLOR_CORNFLOWER_BLUE);
      barSeries4.SetStacked(true);
      barSeries4.SetDataLabelsVisible(true);
      barSeries4.SetName("Revenue");
      barSeries4.SetValues({200, 240, 220, 270});
      mChart.AddSeries(barSeries4);
    }

    {
      BarSeries barSeries5 = BarSeries::New();
      barSeries5.SetColor(Vector4(0.2f, 0.75f, 0.3f, 1.0f));
      barSeries5.SetStacked(true);
      barSeries5.SetDataLabelsVisible(true);
      barSeries5.SetName("Export");
      barSeries5.SetValues({80, 110, 90, 130});
      mChart.AddSeries(barSeries5);
    }

    {
      BarSeries barSeries6 = BarSeries::New();
      barSeries6.SetColor(COLOR_TOMATO);
      barSeries6.SetStacked(true);
      barSeries6.SetDataLabelsVisible(true);
      barSeries6.SetName("Other");
      barSeries6.SetValues({40, 60, 50, 70});
      mChart.AddSeries(barSeries6);
    }

    // ── Grouped bar (non-stacked) alongside the stacked group ────────────────
    {
      BarSeries barSeries7 = BarSeries::New();
      barSeries7.SetColor(Vector4(0.6f, 0.2f, 0.8f, 1.0f));
      barSeries7.SetStacked(false);
      barSeries7.SetName("Cost");
      barSeries7.SetValues({150, 180, 160, 200});
      mChart.AddSeries(barSeries7);
    }

    // ── Trend line on top ────────────────────────────────────────────────────
    {
      LineSeries lineSeries25 = LineSeries::New();
      lineSeries25.SetColor(Vector4(1.0f, 0.6f, 0.0f, 0.9f));
      lineSeries25.SetMarkersVisible(true);
      lineSeries25.SetName("Target");
      lineSeries25.SetValues({350, 430, 400, 500});
      mChart.AddSeries(lineSeries25);
    }

    AddChartToRoot();

    mDebugLabel.SetProperty(Label::Property::TEXT,
                            "Blue/Green/Red stacked | Purple grouped | Orange trend line");
  }

  // -------------------------------------------------------------------------
  // Scene 22: Stacked BarSeries + Zoom
  // -------------------------------------------------------------------------
  void CreateStackedBarZoomScene(Window window)
  {
    mChart = ChartView::New(ChartView::Type::LINE, Vector2(CHART_WIDTH, CHART_HEIGHT));
    mChart.SetProperty(ChartView::Property::SHOW_GRID, true);
    mChart.SetProperty(ChartView::Property::SHOW_LEGEND, true);
    mChart.SetTitle("Stacked + Zoom");

    // Monthly data for 2 years — enough points to make panning/zooming meaningful
    {
      ChartAxis chartAxis24 = ChartAxis::New();
      chartAxis24.SetLabels({"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                             "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
                             "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                             "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"});
      mChart.SetXAxis(chartAxis24);
    }

    {
      ChartAxis chartAxis25 = ChartAxis::New();
      chartAxis25.SetMinStep(100.0f);
      mChart.SetYAxis(chartAxis25);
    }

    // ── Stacked: Revenue ─────────────────────────────────────────────────────
    {
      BarSeries barSeries8 = BarSeries::New();
      barSeries8.SetColor(COLOR_CORNFLOWER_BLUE);
      barSeries8.SetStacked(true);
      barSeries8.SetName("Revenue");
      barSeries8.SetValues({120, 140, 130, 160, 170, 200, 190, 210, 200, 230, 220, 250,
                            260, 250, 270, 300, 290, 320, 310, 340, 330, 360, 350, 380});
      mChart.AddSeries(barSeries8);
    }

    // ── Stacked: Export ──────────────────────────────────────────────────────
    {
      BarSeries barSeries9 = BarSeries::New();
      barSeries9.SetColor(Vector4(0.2f, 0.75f, 0.3f, 1.0f));
      barSeries9.SetStacked(true);
      barSeries9.SetName("Export");
      barSeries9.SetValues({50, 60, 55, 70, 75, 90, 80, 95, 85, 100, 95, 110,
                            115, 110, 120, 135, 130, 145, 140, 155, 150, 165, 160, 175});
      mChart.AddSeries(barSeries9);
    }

    // ── Stacked: Other ───────────────────────────────────────────────────────
    {
      BarSeries barSeries10 = BarSeries::New();
      barSeries10.SetColor(COLOR_TOMATO);
      barSeries10.SetStacked(true);
      barSeries10.SetName("Other");
      barSeries10.SetValues({20, 25, 22, 30, 28, 35, 32, 38, 35, 42, 40, 45,
                             48, 46, 50, 55, 52, 58, 56, 62, 60, 66, 64, 70});
      mChart.AddSeries(barSeries10);
    }

    // ── Grouped: Cost (separate bar slot) ────────────────────────────────────
    {
      BarSeries barSeries11 = BarSeries::New();
      barSeries11.SetColor(Vector4(0.6f, 0.2f, 0.8f, 1.0f));
      barSeries11.SetStacked(false);
      barSeries11.SetName("Cost");
      barSeries11.SetValues({90, 100, 95, 110, 115, 130, 120, 140, 130, 150, 145, 160,
                             165, 160, 170, 185, 180, 195, 190, 205, 200, 215, 210, 225});
      mChart.AddSeries(barSeries11);
    }

    // ── Zoom: X pan + zoom, Y auto-fits to visible stacked tops ──────────────
    mChart.SetZoomMode(
      static_cast<int>(ChartView::ZoomMode::PAN_X) |
      static_cast<int>(ChartView::ZoomMode::ZOOM_X));
    mChart.SetZoomClampEnabled(true);
    mChart.SetAutoFitYOnPan(true);

    mChart.ZoomedSignal().Connect(this, &ChartViewController::OnZoomed);

    AddChartToRoot();

    mDebugLabel.SetProperty(Label::Property::TEXT,
                            "Wheel = zoom X  |  Drag = pan  |  Double-tap = reset");
  }

  // -------------------------------------------------------------------------
  // Scene 20: BarSeries + Zoom
  // -------------------------------------------------------------------------
  void CreateBarZoomScene(Window window)
  {
    mChart = ChartView::New(ChartView::Type::LINE, Vector2(CHART_WIDTH, CHART_HEIGHT));
    mChart.SetProperty(ChartView::Property::SHOW_GRID, true);
    mChart.SetProperty(ChartView::Property::SHOW_LEGEND, true);
    mChart.SetTitle("BarSeries + Zoom");

    // Monthly data for 2 years — enough points to make panning/zooming meaningful
    {
      ChartAxis chartAxis26 = ChartAxis::New();
      chartAxis26.SetLabels({"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                             "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
                             "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                             "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"});
      mChart.SetXAxis(chartAxis26);
    }

    {
      ChartAxis chartAxis27 = ChartAxis::New();
      chartAxis27.SetMinStep(50.0f);
      mChart.SetYAxis(chartAxis27);
    }

    // ── Revenue ──────────────────────────────────────────────────────────────
    {
      BarSeries barSeries12 = BarSeries::New();
      barSeries12.SetColor(COLOR_CORNFLOWER_BLUE);
      barSeries12.SetName("Revenue");
      barSeries12.SetValues({120, 150, 130, 190, 210, 250, 230, 280, 260, 300, 290, 320,
                             340, 310, 360, 400, 380, 420, 410, 460, 440, 490, 470, 510});
      mChart.AddSeries(barSeries12);
    }

    // ── Cost ─────────────────────────────────────────────────────────────────
    {
      BarSeries barSeries13 = BarSeries::New();
      barSeries13.SetColor(COLOR_TOMATO);
      barSeries13.SetName("Cost");
      barSeries13.SetValues({80, 95, 90, 110, 130, 150, 140, 170, 160, 190, 180, 200,
                             210, 200, 220, 250, 230, 260, 250, 280, 270, 300, 290, 310});
      mChart.AddSeries(barSeries13);
    }

    // ── Zoom: X pan + zoom, Y auto-fits to visible bars ──────────────────────
    mChart.SetZoomMode(
      static_cast<int>(ChartView::ZoomMode::PAN_X) |
      static_cast<int>(ChartView::ZoomMode::ZOOM_X));
    mChart.SetZoomClampEnabled(true);
    mChart.SetAutoFitYOnPan(true);

    mChart.ZoomedSignal().Connect(this, &ChartViewController::OnZoomed);

    AddChartToRoot();

    mDebugLabel.SetProperty(Label::Property::TEXT,
                            "Wheel = zoom X  |  Drag = pan  |  Double-tap = reset");
  }

  // Scene 18: ScatterSeries
  // -------------------------------------------------------------------------
  void CreateScatterScene(Window window)
  {
    mChart = ChartView::New(ChartView::Type::LINE, Vector2(CHART_WIDTH, CHART_HEIGHT));
    mChart.SetProperty(ChartView::Property::SHOW_GRID, true);
    mChart.SetProperty(ChartView::Property::SHOW_LEGEND, true);
    mChart.SetTitle("ScatterSeries");

    // ── Series 1: CIRCLE (기본) ──────────────────────────────────────────────
    {
      ScatterSeries scatterSeries1 = ScatterSeries::New();
      scatterSeries1.SetColor(COLOR_CORNFLOWER_BLUE);
      scatterSeries1.SetMarkerRadius(7.0f);
      scatterSeries1.SetMarkerShape(ScatterSeries::MarkerShape::CIRCLE);
      scatterSeries1.SetName("Circle");
      scatterSeries1.SetValues({{0.5f, 30.f}, {1.8f, 55.f}, {3.2f, 45.f}, {4.1f, 80.f}, {5.7f, 60.f}});
      mChart.AddSeries(scatterSeries1);
    }

    // ── Series 2: SQUARE ────────────────────────────────────────────────────
    {
      ScatterSeries scatterSeries2 = ScatterSeries::New();
      scatterSeries2.SetColor(COLOR_TOMATO);
      scatterSeries2.SetMarkerRadius(7.0f);
      scatterSeries2.SetMarkerShape(ScatterSeries::MarkerShape::SQUARE);
      scatterSeries2.SetName("Square");
      scatterSeries2.SetValues({{1.0f, 70.f}, {2.5f, 40.f}, {3.8f, 90.f}, {5.0f, 25.f}, {6.2f, 65.f}});
      mChart.AddSeries(scatterSeries2);
    }

    // ── Series 3: TRIANGLE ──────────────────────────────────────────────────
    {
      ScatterSeries scatterSeries3 = ScatterSeries::New();
      scatterSeries3.SetColor(Color::GREEN);
      scatterSeries3.SetMarkerRadius(7.0f);
      scatterSeries3.SetMarkerShape(ScatterSeries::MarkerShape::TRIANGLE);
      scatterSeries3.SetName("Triangle");
      scatterSeries3.SetValues({{0.2f, 85.f}, {2.0f, 20.f}, {4.5f, 70.f}, {5.5f, 50.f}, {6.8f, 40.f}});
      mChart.AddSeries(scatterSeries3);
    }

    // ── Series 4: DIAMOND (큰 마커) ─────────────────────────────────────────
    {
      ScatterSeries scatterSeries4 = ScatterSeries::New();
      scatterSeries4.SetColor(Color::YELLOW);
      scatterSeries4.SetMarkerRadius(10.0f);
      scatterSeries4.SetMarkerShape(ScatterSeries::MarkerShape::DIAMOND);
      scatterSeries4.SetName("Diamond");
      scatterSeries4.SetValues({{1.5f, 95.f}, {3.0f, 10.f}, {4.8f, 55.f}, {6.0f, 85.f}});
      mChart.AddSeries(scatterSeries4);
    }

    // ── Series 5: LineSeries 혼합 ────────────────────────────────────────────
    {
      LineSeries lineSeries26 = LineSeries::New();
      lineSeries26.SetColor(Vector4(0.5f, 0.5f, 0.5f, 0.6f));
      lineSeries26.SetMarkersVisible(false);
      lineSeries26.SetName("Trend");
      lineSeries26.SetValues({{0.0f, 20.f}, {7.0f, 90.f}});
      mChart.AddSeries(lineSeries26);
    }

    AddChartToRoot();

    mDebugLabel.SetProperty(Label::Property::TEXT,
                            "4 scatter shapes + trend line | legend color check");
  }

  // Scene 17: Sections
  // -------------------------------------------------------------------------
  void CreateSectionsScene(Window window)
  {
    mChart = ChartView::New(ChartView::Type::LINE, Vector2(CHART_WIDTH, CHART_HEIGHT));
    mChart.SetProperty(ChartView::Property::SHOW_GRID, true);
    mChart.SetProperty(ChartView::Property::SHOW_MARKERS, true);
    mChart.SetTitle("Sections");

    // X 카테고리 축 (월별)
    {
      ChartAxis chartAxis28 = ChartAxis::New();
      chartAxis28.SetLabels({"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                             "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"});
      mChart.SetXAxis(chartAxis28);
    }

    {
      ChartAxis chartAxis29 = ChartAxis::New();
      chartAxis29.SetMinStep(50.0f);
      mChart.SetYAxis(chartAxis29);
    }

    // 데이터
    {
      LineSeries lineSeries27 = LineSeries::New();
      lineSeries27.SetColor(COLOR_CORNFLOWER_BLUE);
      lineSeries27.SetMarkersVisible(true);
      lineSeries27.SetName("Revenue");
      lineSeries27.SetValues({120, 150, 130, 190, 210, 250, 230, 280, 260, 300, 290, 320});
      mChart.AddSeries(lineSeries27);
    }

    {
      LineSeries lineSeries28 = LineSeries::New();
      lineSeries28.SetColor(COLOR_TOMATO);
      lineSeries28.SetMarkersVisible(true);
      lineSeries28.SetName("Cost");
      lineSeries28.SetValues({80, 95, 90, 110, 130, 150, 140, 170, 160, 190, 180, 200});
      mChart.AddSeries(lineSeries28);
    }

    // ── Section 1: X 구간 강조 (Q2 = Apr~Jun, index 3~5) ─────────────────
    {
      ChartSection chartSection1 = ChartSection::New();
      chartSection1.SetXMin(2.5f);
      chartSection1.SetXMax(5.5f);
      chartSection1.SetFillColor(Vector4(0.2f, 0.7f, 0.3f, 0.12f));
      chartSection1.SetStrokeColor(Vector4(0.2f, 0.7f, 0.3f, 0.5f));
      chartSection1.SetStrokeWidth(1.5f);
      mChart.AddSection(chartSection1);
    }

    // ── Section 2: Y 구간 강조 (목표 범위 200~280) ────────────────────────
    {
      ChartSection chartSection2 = ChartSection::New();
      chartSection2.SetYMin(200.0f);
      chartSection2.SetYMax(280.0f);
      chartSection2.SetFillColor(Vector4(1.0f, 0.85f, 0.2f, 0.10f));
      mChart.AddSection(chartSection2);
    }

    // ── Section 3: 수평 목표선 (Y = 250) ─────────────────────────────────
    {
      ChartSection chartSection3 = ChartSection::New();
      chartSection3.SetYMin(250.0f);
      chartSection3.SetYMax(250.0f);
      chartSection3.SetStrokeColor(Vector4(1.0f, 0.3f, 0.3f, 0.9f));
      chartSection3.SetStrokeWidth(2.0f);
      mChart.AddSection(chartSection3);
    }

    // ── Section 4: 수직 기준선 (X = 6, July) ────────────────────────────
    {
      ChartSection chartSection4 = ChartSection::New();
      chartSection4.SetXMin(6.0f);
      chartSection4.SetXMax(6.0f);
      chartSection4.SetStrokeColor(Vector4(0.4f, 0.4f, 1.0f, 0.8f));
      chartSection4.SetStrokeWidth(2.0f);
      mChart.AddSection(chartSection4);
    }

    // ── Section 5: 직사각형 강조 (Q4 high zone: Oct~Dec, Y 270~320) ──────
    {
      ChartSection chartSection5 = ChartSection::New();
      chartSection5.SetXMin(8.5f);
      chartSection5.SetXMax(11.5f);
      chartSection5.SetYMin(270.0f);
      chartSection5.SetYMax(320.0f);
      chartSection5.SetFillColor(Vector4(0.6f, 0.2f, 1.0f, 0.12f));
      chartSection5.SetStrokeColor(Vector4(0.6f, 0.2f, 1.0f, 0.6f));
      chartSection5.SetStrokeWidth(1.5f);
      mChart.AddSection(chartSection5);
    }

    mChart.SetProperty(ChartView::Property::SHOW_LEGEND, true);
    AddChartToRoot();

    mDebugLabel.SetProperty(Label::Property::TEXT,
                            "Green=Q2 band | Yellow=target range | Red=250 line | Blue=July | Purple=Q4 rect");
  }

  void AddChartToRoot(float w = CHART_WIDTH, float h = CHART_HEIGHT)
  {
    mChart.SetLayoutParams(AbsoluteLayoutParams::New()
                             .SetX((mWinW - w) * 0.5f)
                             .SetY((mWinH - h) * 0.5f - 20.0f)
                             .SetWidth(w)
                             .SetHeight(h));
    mRootLayout.Add(mChart);
  }

private:
  Application&   mApplication;
  ChartView      mChart;
  AbsoluteLayout mRootLayout;
  float          mWinW{0.0f};
  float          mWinH{0.0f};
  Label          mSceneLabel;
  Label          mDebugLabel;
  AbsoluteLayout mPrevButton;
  AbsoluteLayout mNextButton;
  Timer          mSizeTimer;
  int            mCurrentScene{0};
  bool           mScene2TouchToggle{false};
  bool           mScene3SizeToggle{false};
  LineSeries     mSeriesA;

  // Scene 7
  bool mScene7ShowTooltip{true};
  // Scene 8
  bool mScene8LegendToggle{true};
  // Scene 9 — state: 0=Both ON, 1=Touch OFF, 2=Hover OFF, 3=Both OFF
  int mScene9InputState{0};
  // Scene 10
  LineSeries mScene10Series;
  float      mScene10NextY{70.0f};
  // Scene 13
  LineSeries mScene13Series;
  bool       mScene13Toggle{false};
  // Scene 14
  LineSeries mScene14Series;
  bool       mScene14Smooth{false};

  // -------------------------------------------------------------------------
  // Scene 23: Pie Chart
  // -------------------------------------------------------------------------
  void CreatePieScene(Window window)
  {
    mChart = ChartView::New(ChartView::Type::PIE, Vector2(CHART_WIDTH, CHART_HEIGHT));
    mChart.SetProperty(ChartView::Property::SHOW_LEGEND, true);
    mChart.SetTitle("Browser Market Share");

    PieSeries pie = PieSeries::New();
    pie.AddSlice("Chrome", 63.0f, COLOR_CORNFLOWER_BLUE);
    pie.AddSlice("Safari", 19.0f, COLOR_TOMATO);
    pie.AddSlice("Edge", 11.0f, Vector4(0.2f, 0.75f, 0.3f, 1.0f));
    pie.AddSlice("Firefox", 4.0f, Vector4(0.6f, 0.2f, 0.8f, 1.0f));
    pie.AddSlice("Other", 3.0f, Color::GRAY);
    pie.SetDataLabelsVisible(true);
    mChart.AddSeries(pie);

    AddChartToRoot();

    mChart.DataPointSelectedSignal().Connect(this, &ChartViewController::OnPieSliceTapped);

    mDebugLabel.SetProperty(Label::Property::TEXT,
                            "Tap a slice — label updates | Tap gap/hole/outside → no change");
  }

  // -------------------------------------------------------------------------
  // Scene 24: Donut Chart
  // -------------------------------------------------------------------------
  void CreateDonutScene(Window window)
  {
    mChart = ChartView::New(ChartView::Type::PIE, Vector2(CHART_WIDTH, CHART_HEIGHT));
    mChart.SetProperty(ChartView::Property::SHOW_LEGEND, true);
    mChart.SetTitle("Q1 Revenue by Region");

    PieSeries pie = PieSeries::New();
    pie.AddSlice("Asia", 42.0f, COLOR_CORNFLOWER_BLUE);
    pie.AddSlice("America", 28.0f, COLOR_TOMATO);
    pie.AddSlice("Europe", 20.0f, Vector4(0.2f, 0.75f, 0.3f, 1.0f));
    pie.AddSlice("Others", 10.0f, Color::GRAY);
    pie.SetInnerRadiusRatio(0.55f);
    pie.SetCenterLabel("$4.2B");
    pie.SetSliceGap(2.0f);
    pie.SetDataLabelsVisible(true);
    mChart.AddSeries(pie);

    AddChartToRoot();

    mChart.DataPointSelectedSignal().Connect(this, &ChartViewController::OnPieSliceTapped);

    mDebugLabel.SetProperty(Label::Property::TEXT,
                            "Tap a slice — label updates | Tap gap/hole/outside → no change");
  }

  void OnPieSliceTapped(const ChartPointEventArgs& args)
  {
    char buf[80];
    std::snprintf(buf, sizeof(buf), "Tapped [%d] %s  (%.0f)",
                  args.pointIndex, args.xLabel.CStr(), args.dataY);
    mDebugLabel.SetProperty(Label::Property::TEXT, buf);
  }

  // ── Scene 25: Gauge ───────────────────────────────────────────────────────

  void CreateGaugeScene(Window window)
  {
    mChart = ChartView::New(ChartView::Type::GAUGE, Vector2(CHART_WIDTH, CHART_HEIGHT));
    mChart.SetTitle("CPU Usage");

    mChart.SetGaugeMinValue(0.0f);
    mChart.SetGaugeMaxValue(100.0f);
    mChart.SetGaugeValue(72.0f);

    mChart.AddGaugeRange(0.0f, 60.0f, Vector4(0.2f, 0.75f, 0.3f, 1.0f)); // green
    mChart.AddGaugeRange(60.0f, 80.0f, Vector4(1.0f, 0.7f, 0.0f, 1.0f)); // yellow
    mChart.AddGaugeRange(80.0f, 100.0f, COLOR_TOMATO);                   // red

    AddChartToRoot();

    mScene25GaugeValue = 72.0f;
    mDebugLabel.SetProperty(Label::Property::TEXT,
                            "Touch outside chart = cycle value (0 / 50 / 72 / 90 / 100)");
    window.GetRootLayer().TouchEventSignal().Connect(this, &ChartViewController::OnScene25Touch);
  }

  bool OnScene25Touch(Actor /*actor*/, TouchEvent touch)
  {
    if(touch.GetState(0) != PointState::DOWN) return false;
    const float   steps[] = {0.0f, 50.0f, 72.0f, 90.0f, 100.0f};
    constexpr int N       = sizeof(steps) / sizeof(steps[0]);
    int           next    = 0;
    for(int i = 0; i < N; ++i)
      if(std::abs(mScene25GaugeValue - steps[i]) < 0.1f)
      {
        next = (i + 1) % N;
        break;
      }
    mScene25GaugeValue = steps[next];
    mChart.SetGaugeValue(mScene25GaugeValue);
    char buf[48];
    std::snprintf(buf, sizeof(buf), "Value = %.0f", mScene25GaugeValue);
    mDebugLabel.SetProperty(Label::Property::TEXT, buf);
    return true;
  }

  float mScene25GaugeValue{72.0f};
};

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------
int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  Components::UiConfig config = Components::UiConfig::New();
  config.SetDefaultStateEffectForInteractive(OverlayEffect::Plain());
  config.Apply();
  ChartViewController controller(application);
  application.MainLoop();
  return 0;
}
