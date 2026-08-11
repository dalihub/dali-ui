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
 * @file  chart-memory-probe.cpp
 * @brief ChartView memory & performance measurement sample app
 *
 * Runs 18 phases sequentially and measures, before and after each phase:
 *   - memory:      /proc/self/status (VmRSS, VmHWM) + mallinfo2 heap usage
 *   - build time:  main-thread CPU time to create + configure the chart (ms)
 *   - render time: wall-clock from build start until the GPU finishes the
 *                  first frame containing the chart, via the DevelWindow
 *                  frame-rendered callback
 * Results are shown in the right-side info panel and printed to stdout.
 *
 * Note: build time is measured on the event thread and render time on the
 * render thread. They are distinct numbers and must not be summed.
 *
 * Controls:
 *   [Next]  button / → / Enter — advance to next phase
 *   [Auto]  button / A         — toggle auto-advance (1.2 s delay between phases)
 *   [Reset] button / R         — reset baseline
 *   Escape                     — quit
 *
 * Save results:
 *   ./bin/chart-memory-probe 2>&1 | tee result.log
 */

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <functional>
#include <malloc.h>
#include <features.h>
#include <sstream>
#include <string>
#include <vector>

#include <dali-ui-components/dali-ui-components.h>
#include <dali-ui-foundation/dali-ui-foundation.h>

#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/public-api/signals/callback.h>

using namespace Dali;
using namespace Dali::Ui;

using SteadyClock = std::chrono::steady_clock;

// Elapsed milliseconds between two steady-clock time points.
static double MsBetween(SteadyClock::time_point a, SteadyClock::time_point b)
{
  return std::chrono::duration<double, std::milli>(b - a).count();
}

// =============================================================================
// MemSnapshot — /proc/self/status + mallinfo2
// =============================================================================

struct MemSnapshot
{
  long vmRss{0};     // physical RSS, kB
  long vmHwm{0};     // peak RSS, kB
  long vmSize{0};    // virtual address space, kB
  long heapUsed{0};  // mallinfo2 uordblks, kB
  long heapArena{0}; // mallinfo2 arena, kB
};

static MemSnapshot TakeSnapshot()
{
  MemSnapshot s;

  if(FILE* f = fopen("/proc/self/status", "r"))
  {
    char line[128];
    while(fgets(line, sizeof(line), f))
    {
      if     (strncmp(line, "VmRSS:",  6) == 0) sscanf(line + 6,  "%ld", &s.vmRss);
      else if(strncmp(line, "VmHWM:",  6) == 0) sscanf(line + 6,  "%ld", &s.vmHwm);
      else if(strncmp(line, "VmSize:", 7) == 0) sscanf(line + 7, "%ld", &s.vmSize);
    }
    fclose(f);
  }

#if __GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 33)
  struct mallinfo2 mi = mallinfo2();
  s.heapUsed  = static_cast<long>(mi.uordblks / 1024);
  s.heapArena = static_cast<long>(mi.arena     / 1024);
#else
  struct mallinfo mi = mallinfo();
  s.heapUsed  = static_cast<long>(mi.uordblks / 1024);
  s.heapArena = static_cast<long>(mi.arena     / 1024);
#endif

  return s;
}

static std::string FormatDelta(const char*        tag,
                                const MemSnapshot& before,
                                const MemSnapshot& after)
{
  char buf[320];
  std::snprintf(buf, sizeof(buf),
    "[%s]\n"
    "  RSS  : %ld kB (%+ld)\n"
    "  Heap : %ld kB (%+ld)\n"
    "  HWM  : %ld kB",
    tag,
    after.vmRss,    after.vmRss    - before.vmRss,
    after.heapUsed, after.heapUsed - before.heapUsed,
    after.vmHwm);
  return std::string(buf);
}

// Theoretical GPU texture size for 3 CanvasView layers (Background/Data/Overlay), kB
static int CalcGpuTexKb(int w, int h)
{
  return (3 * w * h * 4) / 1024;
}

// =============================================================================
// Constants
// =============================================================================

namespace
{
const int   MAX_LOG_LINES    = 32;
const float INFO_PANEL_W     = 420.0f;
const float HEADER_H         = 40.0f;
const float FOOTER_H         = 54.0f;
const float BTN_W            = 120.0f;
const float BTN_H            = 44.0f;
const int          SNAP_FRAME_COUNT = 3;     // rendered frames to wait for buffer settle
const unsigned int SNAP_FALLBACK_MS = 300u;  // safety net if frame callbacks never arrive
const unsigned int LEAK_DELAY_MS    = 32u;   // leak cycle interval
const unsigned int AUTO_NEXT_MS     = 1200u; // delay between phases in auto mode
} // namespace

// =============================================================================
// MemProbeApp
// =============================================================================

class MemProbeApp : public ConnectionTracker
{
public:
  explicit MemProbeApp(Application& app)
  : mApp(app)
  {
    app.InitSignal().Connect(this, &MemProbeApp::OnCreate);
  }

private:
  // ── app / dimensions ──────────────────────────────────────────────────────
  Application& mApp;
  float        mWinW{0};
  float        mWinH{0};
  float        mChartPanelW{0};
  float        mContentH{0};
  float        mChartW{0};
  float        mChartH{0};

  // ── UI widgets ────────────────────────────────────────────────────────────
  AbsoluteLayout mRootLayout;
  AbsoluteLayout mChartPanel;
  AbsoluteLayout mInfoPanel;
  Label          mPhaseLabel;
  Label          mInfoLabel;
  Label          mChartDescLabel;
  AbsoluteLayout mNextButton;
  AbsoluteLayout mAutoButton;
  AbsoluteLayout mResetButton;

  // ── chart ─────────────────────────────────────────────────────────────────
  ChartView mChart;
  ChartView mLeakChart;

  // ── phase management ──────────────────────────────────────────────────────
  using PhaseFunc = std::function<void()>;
  std::vector<PhaseFunc>   mPhases;
  std::vector<std::string> mPhaseNames;
  int                      mCurrentPhase{-1};

  // ── snapshot timer ────────────────────────────────────────────────────────
  Timer                            mSnapTimer;
  std::function<void(MemSnapshot)> mSnapCallback;

  // ── auto-advance timer ────────────────────────────────────────────────────
  Timer mAutoTimer;
  bool  mAutoRunning{false};

  // ── leak detection ────────────────────────────────────────────────────────
  Timer       mLeakTimer;
  int         mLeakCycleCount{0};
  MemSnapshot mLeakStartSnap;
  bool        mLeakRunning{false};

  // ── performance timing ────────────────────────────────────────────────────
  SteadyClock::time_point mBuildStart;          // captured before chart construction
  double                  mBuildMs{0.0};        // main-thread build time
  double                  mRenderMs{-1.0};      // -1 until frame-rendered callback fires
  int32_t                 mFrameIdSeq{0};       // monotonic frame-callback id
  int32_t                 mPendingFrameId{-1};  // id we are currently waiting on
  int                     mFramesRemaining{0};  // rendered frames left before snapshot
  bool                    mSnapPending{false};  // a snapshot is in flight

  // ── baseline / log ────────────────────────────────────────────────────────
  MemSnapshot              mBaseline;
  MemSnapshot              mPrevSnap;
  std::vector<std::string> mLogLines;

  // =========================================================================
  // OnCreate
  // =========================================================================

  // InitSignal signature: Signal<void(Application)> — must receive by value
  void OnCreate(Application /*app*/)
  {
    Window  win = mApp.GetWindow();
    PositionSize positionSize = win.GetPositionSize();
    Vector2 wsz(positionSize.width, positionSize.height);
    mWinW        = wsz.x;
    mWinH        = wsz.y;
    mChartPanelW = mWinW - INFO_PANEL_W;
    mContentH    = mWinH - HEADER_H - FOOTER_H;
    mChartW      = mChartPanelW - 20.0f;
    mChartH      = std::min(mContentH - 70.0f, mChartW * 0.70f);

    win.SetBackgroundColor(Vector4(0.10f, 0.10f, 0.12f, 1.0f));

    // KeyEventSignal signature: Signal<void(Window, KeyEvent)>
    win.KeyEventSignal().Connect(this,
      [this](Window /*w*/, KeyEvent e) { OnKeyEvent(e); });

    BuildUI();
    RegisterPhases();

    mBaseline = TakeSnapshot();
    mPrevSnap = mBaseline;

    AddLog("=== ChartView Memory Probe ===");
    {
      char buf[128];
      std::snprintf(buf, sizeof(buf),
        "Win: %.0fx%.0f  Chart: %.0fx%.0f",
        mWinW, mWinH, mChartW, mChartH);
      AddLog(buf);
    }
    {
      char buf[128];
      std::snprintf(buf, sizeof(buf),
        "Baseline RSS: %ld kB  Heap: %ld kB",
        mBaseline.vmRss, mBaseline.heapUsed);
      AddLog(buf);
    }
    AddLog("-> / [Next] to advance phase, A / [Auto] for auto-run");
    FlushLog();
  }

  // =========================================================================
  // BuildUI
  // =========================================================================

  void BuildUI()
  {
    // header
    mPhaseLabel = Label::New("Ready");
    mPhaseLabel.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetX(0.0f).SetY(0.0f).SetWidth(mWinW).SetHeight(HEADER_H));
    mPhaseLabel.SetFontSize(8.5f);
    mPhaseLabel.SetTextColor(UiColor(0xFFFFFF));
    mPhaseLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mPhaseLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mPhaseLabel.SetBackgroundColor(UiColor(0x1a2744, 1.0f));

    // left: chart panel
    mChartPanel = AbsoluteLayout::New();
    mChartPanel.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetX(0.0f).SetY(HEADER_H)
      .SetWidth(mChartPanelW).SetHeight(mContentH));
    mChartPanel.SetBackgroundColor(UiColor(0x1a1a20, 1.0f));

    // description label below chart (chart panel coordinate space)
    mChartDescLabel = Label::New("");
    mChartDescLabel.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetX(4.0f).SetY(mChartH + 16.0f)
      .SetWidth(mChartPanelW - 8.0f)
      .SetHeight(mContentH - mChartH - 22.0f));
    mChartDescLabel.SetFontSize(7.5f);
    mChartDescLabel.SetTextColor(UiColor(0xaaaaaa));
    mChartDescLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mChartDescLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mChartPanel.Add(mChartDescLabel);

    // right: info panel
    mInfoPanel = AbsoluteLayout::New();
    mInfoPanel.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetX(mChartPanelW).SetY(HEADER_H)
      .SetWidth(INFO_PANEL_W).SetHeight(mContentH));
    mInfoPanel.SetBackgroundColor(UiColor(0x0d1117, 1.0f));

    // log label
    mInfoLabel = Label::New("");
    mInfoLabel.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetX(8.0f).SetY(6.0f)
      .SetWidth(INFO_PANEL_W - 16.0f)
      .SetHeight(mContentH - 12.0f));
    mInfoLabel.SetFontSize(6.5f);
    mInfoLabel.SetTextColor(UiColor(0x7ec8a0));
    mInfoLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mInfoLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mInfoPanel.Add(mInfoLabel);

    // bottom buttons
    float btnY = mWinH - FOOTER_H + 5.0f;
    float midX = mWinW * 0.5f - BTN_W * 0.5f;

    mResetButton = MakeButton("Reset",  10.0f,                 btnY, [this]{ OnReset(); });
    mNextButton  = MakeButton("Next",   midX,                  btnY, [this]{ OnNext(); });
    mAutoButton  = MakeButton("Auto",   mWinW - BTN_W - 10.0f, btnY, [this]{ OnToggleAuto(); });

    mRootLayout = AbsoluteLayout::New();
    mRootLayout.SetRequestedWidth(MATCH_PARENT);
    mRootLayout.SetRequestedHeight(MATCH_PARENT);
    mRootLayout.Add(mPhaseLabel);
    mRootLayout.Add(mChartPanel);
    mRootLayout.Add(mInfoPanel);
    mRootLayout.Add(mResetButton);
    mRootLayout.Add(mNextButton);
    mRootLayout.Add(mAutoButton);

    mApp.GetWindow().Add(mRootLayout);
  }

  // ClickedSignal signature: Signal<void(View, InputEvent)>
  // Pass 'this' as ConnectionTrackerInterface* so signal disconnection is handled automatically
  AbsoluteLayout MakeButton(const char* text, float x, float y,
                              std::function<void()> cb)
  {
    auto btn = AbsoluteLayout::New();
    btn.SetRequestedWidth(BTN_W);
    btn.SetRequestedHeight(BTN_H);
    btn.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetX(x).SetY(y).SetWidth(BTN_W).SetHeight(BTN_H));
    btn.SetBackgroundColor(UiColor(0x2e4a7a, 1.0f));
    btn.AsInteractive().ClickedSignal().Connect(this, [cb](View, InputEvent) -> bool {
      cb();
      return true;
    });

    auto lbl = Label::New(text);
    lbl.SetRequestedWidth(MATCH_PARENT);
    lbl.SetRequestedHeight(MATCH_PARENT);
    lbl.SetFontSize(9.0f);
    lbl.SetTextColor(UiColor(0xFFFFFF));
    lbl.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    lbl.SetVerticalTextAlignment(Text::Alignment::CENTER);
    btn.Add(lbl);

    return btn;
  }

  // =========================================================================
  // Phase registration
  // =========================================================================

  void RegisterPhases()
  {
    // P0: empty chart
    Add("P0:  Empty LINE chart",             [this]{ PhaseCreateEmpty(); });
    // P1~P4: point count scaling
    Add("P1:  1 series x    10 points",      [this]{ PhaseDataScale(1,     10); });
    Add("P2:  1 series x   100 points",      [this]{ PhaseDataScale(1,    100); });
    Add("P3:  1 series x  1000 points",      [this]{ PhaseDataScale(1,   1000); });
    Add("P4:  1 series x 10000 points",      [this]{ PhaseDataScale(1,  10000); });
    // P5~P7: series count scaling
    Add("P5:   3 series x   100 points",     [this]{ PhaseDataScale( 3,   100); });
    Add("P6:  10 series x   100 points",     [this]{ PhaseDataScale(10,   100); });
    Add("P7:  20 series x   100 points",     [this]{ PhaseDataScale(20,   100); });
    // P8~P10: chart type
    Add("P8:  LINE chart  (5 series x 20)",  [this]{ PhaseChartType(false, 5, 20, "LINE"); });
    Add("P9:  BAR  chart  (5 series x 20)",  [this]{ PhaseChartType(true,  5, 20, "BAR");  });
    Add("P10: PIE  chart  (4 slices)",       [this]{ PhaseChartTypePie(); });
    // P11~P13: chart size
    Add("P11: Size  200 x 150",              [this]{ PhaseSize( 200,  150); });
    Add("P12: Size  480 x 360",              [this]{ PhaseSize( 480,  360); });
    Add("P13: Size 1280 x 720",              [this]{ PhaseSize(1280,  720); });
    // P14: leak detection
    Add("P14: Create/Destroy x100 leak test",[this]{ PhaseLeakTest(); });
    // P15~P17: tick label count
    Add("P15: X-tick  5 labels",             [this]{ PhaseTickLabels( 5); });
    Add("P16: X-tick 20 labels",             [this]{ PhaseTickLabels(20); });
    Add("P17: X-tick 50 labels",             [this]{ PhaseTickLabels(50); });
    // done
    Add("Done",                              [this]{ PhaseDone(); });
  }

  void Add(const char* name, PhaseFunc fn)
  {
    mPhaseNames.push_back(name);
    mPhases.push_back(std::move(fn));
  }

  // =========================================================================
  // Helper: swap active chart
  // =========================================================================

  void SetActiveChart(ChartView chart, float w, float h)
  {
    if(mChart)
    {
      mChartPanel.Remove(mChart);
      mChart.Reset();
    }
    mChart = chart;
    mChart.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetX((mChartPanelW - w) * 0.5f)
      .SetY(8.0f)
      .SetWidth(w)
      .SetHeight(h));
    mChartPanel.Add(mChart);
  }

  // =========================================================================
  // Helper: snapshot after render completes
  // Measuring immediately after AddSeries() is inaccurate — CanvasView
  // re-render buffers are allocated in subsequent frames. We drive the
  // snapshot off DevelWindow frame-rendered callbacks: the first callback
  // gives the real time-to-rendered, and we wait SNAP_FRAME_COUNT frames so
  // the rasterization buffers settle before reading memory. A fallback Timer
  // guarantees completion if the graphics backend never delivers the
  // callbacks (e.g. some desktop GL drivers).
  // =========================================================================

  void SnapAfterFrame(std::function<void(MemSnapshot)> cb)
  {
    // Build just completed (this runs synchronously right after construction),
    // so now() marks the end of the main-thread build work.
    mBuildMs  = MsBetween(mBuildStart, SteadyClock::now());
    mRenderMs = -1.0;

    mSnapCallback    = std::move(cb);
    mSnapPending     = true;
    mFramesRemaining = SNAP_FRAME_COUNT;

    RegisterFrameRenderedCallback();

    // Safety net: snapshot anyway if frame-rendered callbacks never arrive.
    if(mSnapTimer) { mSnapTimer.Stop(); mSnapTimer.Reset(); }
    mSnapTimer = Timer::New(SNAP_FALLBACK_MS);
    mSnapTimer.TickSignal().Connect(this, &MemProbeApp::OnSnapTimerTick);
    mSnapTimer.Start();
  }

  void RegisterFrameRenderedCallback()
  {
    mPendingFrameId = ++mFrameIdSeq;
    mApp.GetWindow().AddFrameRenderedCallback(MakeCallback(this, &MemProbeApp::OnFrameRendered), mPendingFrameId);
  }

  // Frame-rendered callback (DevelWindow): fires when the graphics driver
  // finishes rendering. The first one records the real time-to-rendered; we
  // then wait a few more frames for buffer settle before snapshotting memory.
  // Signature must be void(int32_t frameId).
  void OnFrameRendered(int32_t frameId)
  {
    if(frameId != mPendingFrameId || !mSnapPending) return; // stale / already done

    if(mRenderMs < 0.0)
      mRenderMs = MsBetween(mBuildStart, SteadyClock::now());

    if(--mFramesRemaining > 0)
    {
      RegisterFrameRenderedCallback(); // wait for buffers to settle
      return;
    }

    FinishSnapshot();
  }

  bool OnSnapTimerTick()
  {
    FinishSnapshot(); // fallback path — frame callbacks did not complete in time
    return false;     // fires once then stops
  }

  // Takes the memory snapshot and delivers it to the pending phase callback.
  // Reached via either the frame-rendered path or the fallback Timer; guarded
  // so it runs exactly once per phase.
  void FinishSnapshot()
  {
    if(!mSnapPending) return;
    mSnapPending    = false;
    mPendingFrameId = -1; // ignore any further/stale frame callbacks

    if(mSnapTimer) { mSnapTimer.Stop(); mSnapTimer.Reset(); }

    MemSnapshot snap = TakeSnapshot();
    auto cb = std::move(mSnapCallback);
    mSnapCallback = nullptr;
    if(cb) cb(snap);

    if(mAutoRunning && !mLeakRunning)
      ScheduleAutoNext();
  }

  // Formats the build/render performance lines appended to each phase log.
  std::string PerfLine() const
  {
    char buf[96];
    if(mRenderMs >= 0.0)
      std::snprintf(buf, sizeof(buf),
        "\n  Build: %.2f ms\n  Rendered: %.2f ms", mBuildMs, mRenderMs);
    else
      std::snprintf(buf, sizeof(buf),
        "\n  Build: %.2f ms\n  Rendered: n/a", mBuildMs);
    return std::string(buf);
  }

  // =========================================================================
  // Phase implementations
  // =========================================================================

  // P0: create empty LINE chart ─────────────────────────────────────────────

  void PhaseCreateEmpty()
  {
    MemSnapshot before = TakeSnapshot();
    mBuildStart        = SteadyClock::now();

    ChartView chart = ChartView::New(ChartView::Type::LINE,
                                      Vector2(mChartW, mChartH));
    chart.SetTitle("Memory Probe");
    auto xAxis0 = ChartAxis::New();
    xAxis0.SetTitle("X");
    xAxis0.SetMinimumLimit(0.0f);
    xAxis0.SetMaximumLimit(10.0f);
    chart.SetXAxis(xAxis0);
    auto yAxis0 = ChartAxis::New();
    yAxis0.SetTitle("Y");
    yAxis0.SetMinimumLimit(0.0f);
    yAxis0.SetMaximumLimit(100.0f);
    chart.SetYAxis(yAxis0);

    SetActiveChart(chart, mChartW, mChartH);

    SnapAfterFrame([this, before](MemSnapshot after) {
      std::string log = FormatDelta("P0: Empty LINE chart", before, after);
      char gpu[96];
      std::snprintf(gpu, sizeof(gpu),
        "\n  GPU theory: %d kB (3x%.0fx%.0fx4B)",
        CalcGpuTexKb(static_cast<int>(mChartW), static_cast<int>(mChartH)),
        mChartW, mChartH);
      log += gpu;
      log += PerfLine();
      PrintAndLog(log);
      mPrevSnap = after;
    });

    SetDesc("Empty LINE chart (axes/grid only, no series)");
  }

  // P1~P7: data scaling ──────────────────────────────────────────────────────

  void PhaseDataScale(int numSeries, int numPoints)
  {
    MemSnapshot before = TakeSnapshot();
    mBuildStart        = SteadyClock::now();

    ChartView chart = ChartView::New(ChartView::Type::LINE,
                                      Vector2(mChartW, mChartH));
    auto xAxisD = ChartAxis::New();
    xAxisD.SetMinimumLimit(0.0f);
    xAxisD.SetMaximumLimit(static_cast<float>(std::max(numPoints - 1, 1)));
    chart.SetXAxis(xAxisD);
    auto yAxisD = ChartAxis::New();
    yAxisD.SetMinimumLimit(-1.5f);
    yAxisD.SetMaximumLimit(1.5f);
    chart.SetYAxis(yAxisD);

    static const Vector4 COLORS[10] = {
      Vector4(0.20f, 0.60f, 1.00f, 1.0f), Vector4(1.00f, 0.40f, 0.20f, 1.0f),
      Vector4(0.20f, 0.80f, 0.40f, 1.0f), Vector4(0.90f, 0.70f, 0.10f, 1.0f),
      Vector4(0.70f, 0.30f, 0.90f, 1.0f), Vector4(0.10f, 0.80f, 0.80f, 1.0f),
      Vector4(1.00f, 0.50f, 0.70f, 1.0f), Vector4(0.50f, 0.90f, 0.30f, 1.0f),
      Vector4(0.90f, 0.30f, 0.30f, 1.0f), Vector4(0.30f, 0.50f, 0.90f, 1.0f),
    };

    for(int s = 0; s < numSeries; ++s)
    {
      LineSeries series = LineSeries::New();
      series.SetColor(COLORS[s % 10]);
      char name[16]; std::snprintf(name, sizeof(name), "S%d", s + 1);
      series.SetName(name);

      Dali::Vector<float> vals;
      vals.Resize(numPoints);
      for(int i = 0; i < numPoints; ++i)
        vals[i] = std::sin((i + s * 30) * 0.1f) * 0.8f;
      series.SetValues(vals);
      chart.AddSeries(series);
    }

    SetActiveChart(chart, mChartW, mChartH);

    SnapAfterFrame([this, before, numSeries, numPoints](MemSnapshot after) {
      char tag[64];
      std::snprintf(tag, sizeof(tag), "%d series x %d points", numSeries, numPoints);

      std::string log = FormatDelta(tag, before, after);

      long   theoryB   = static_cast<long>(numSeries) * numPoints * 8L;
      int    total     = numSeries * numPoints;
      double heapDelta = static_cast<double>(after.heapUsed - before.heapUsed) * 1024.0;
      double perPoint  = total > 0 ? heapDelta / total : 0.0;

      char extra[160];
      std::snprintf(extra, sizeof(extra),
        "\n  Data theory: %ld B\n  Heap/point: %.1f B", theoryB, perPoint);
      log += extra;
      log += PerfLine();

      PrintAndLog(log);
      mPrevSnap = after;
    });

    char desc[96];
    std::snprintf(desc, sizeof(desc), "%d series x %d points (sine wave)", numSeries, numPoints);
    SetDesc(desc);
  }

  // P8~P9: chart type (LINE vs BAR) ─────────────────────────────────────────

  void PhaseChartType(bool useBar, int numSeries, int numPoints,
                       const char* typeName)
  {
    MemSnapshot before = TakeSnapshot();
    mBuildStart        = SteadyClock::now();

    ChartView chart = ChartView::New(ChartView::Type::LINE,
                                      Vector2(mChartW, mChartH));
    auto xAxisC = ChartAxis::New();
    xAxisC.SetMinimumLimit(useBar ? -0.5f : 0.0f);
    xAxisC.SetMaximumLimit(static_cast<float>(numPoints) + (useBar ? -0.5f : -1.0f));
    chart.SetXAxis(xAxisC);
    auto yAxisC = ChartAxis::New();
    yAxisC.SetMinimumLimit(0.0f);
    yAxisC.SetMaximumLimit(100.0f);
    chart.SetYAxis(yAxisC);
    chart.SetProperty(ChartView::Property::SHOW_LEGEND, true);

    static const Vector4 COLORS[5] = {
      Vector4(0.20f, 0.60f, 1.00f, 1.0f), Vector4(1.00f, 0.40f, 0.20f, 1.0f),
      Vector4(0.20f, 0.80f, 0.40f, 1.0f), Vector4(0.90f, 0.70f, 0.10f, 1.0f),
      Vector4(0.70f, 0.30f, 0.90f, 1.0f),
    };

    for(int s = 0; s < numSeries; ++s)
    {
      char name[16]; std::snprintf(name, sizeof(name), "S%d", s + 1);
      Dali::Vector<float> vals;
      vals.Resize(numPoints);
      for(int i = 0; i < numPoints; ++i)
        vals[i] = 20.0f + std::fabs(std::sin(i * 0.3f + s) * 60.0f);

      if(useBar)
      {
        BarSeries bar = BarSeries::New();
        bar.SetName(name);
        bar.SetColor(COLORS[s % 5]);
        bar.SetValues(vals);
        chart.AddSeries(bar);
      }
      else
      {
        LineSeries line = LineSeries::New();
        line.SetName(name);
        line.SetColor(COLORS[s % 5]);
        line.SetValues(vals);
        chart.AddSeries(line);
      }
    }

    SetActiveChart(chart, mChartW, mChartH);

    SnapAfterFrame([this, before, typeName, numSeries, numPoints](MemSnapshot after) {
      char tag[64];
      std::snprintf(tag, sizeof(tag), "%s (%d series x %d points)", typeName, numSeries, numPoints);
      std::string log = FormatDelta(tag, before, after);
      log += PerfLine();
      PrintAndLog(log);
      mPrevSnap = after;
    });

    char desc[80];
    std::snprintf(desc, sizeof(desc), "%s chart - %d series x %d points", typeName, numSeries, numPoints);
    SetDesc(desc);
  }

  // P10: PIE chart ───────────────────────────────────────────────────────────

  void PhaseChartTypePie()
  {
    MemSnapshot before = TakeSnapshot();
    mBuildStart        = SteadyClock::now();

    ChartView chart = ChartView::New(ChartView::Type::PIE,
                                      Vector2(mChartW, mChartH));
    chart.SetTitle("Pie Chart");
    chart.SetProperty(ChartView::Property::SHOW_LEGEND, true);

    PieSeries pie = PieSeries::New();
    pie.AddSlice("Alpha",  35.0f, Vector4(0.20f, 0.60f, 1.00f, 1.0f));
    pie.AddSlice("Beta",   25.0f, Vector4(1.00f, 0.40f, 0.20f, 1.0f));
    pie.AddSlice("Gamma",  20.0f, Vector4(0.20f, 0.80f, 0.40f, 1.0f));
    pie.AddSlice("Delta",  20.0f, Vector4(0.90f, 0.70f, 0.10f, 1.0f));
    chart.AddSeries(pie);

    SetActiveChart(chart, mChartW, mChartH);

    SnapAfterFrame([this, before](MemSnapshot after) {
      std::string log = FormatDelta("PIE chart (4 slices)", before, after);
      log += PerfLine();
      PrintAndLog(log);
      mPrevSnap = after;
    });

    SetDesc("PIE chart - 4 slices");
  }

  // P11~P13: chart size variants ────────────────────────────────────────────

  void PhaseSize(int w, int h)
  {
    MemSnapshot before = TakeSnapshot();
    mBuildStart        = SteadyClock::now();

    ChartView chart = ChartView::New(ChartView::Type::LINE,
                                      Vector2(static_cast<float>(w),
                                              static_cast<float>(h)));
    auto xAxisS = ChartAxis::New();
    xAxisS.SetMinimumLimit(0.0f);
    xAxisS.SetMaximumLimit(49.0f);
    chart.SetXAxis(xAxisS);
    auto yAxisS = ChartAxis::New();
    yAxisS.SetMinimumLimit(-1.5f);
    yAxisS.SetMaximumLimit(1.5f);
    chart.SetYAxis(yAxisS);

    for(int s = 0; s < 3; ++s)
    {
      LineSeries series = LineSeries::New();
      char name[16]; std::snprintf(name, sizeof(name), "S%d", s + 1);
      series.SetName(name);
      Dali::Vector<float> vals;
      vals.Resize(50);
      for(int i = 0; i < 50; ++i)
        vals[i] = std::sin(i * 0.12f + s * 1.0f);
      series.SetValues(vals);
      chart.AddSeries(series);
    }

    // clip display size if chart exceeds panel bounds
    float displayW = std::min(static_cast<float>(w), mChartPanelW - 10.0f);
    float displayH = std::min(static_cast<float>(h), mContentH - 60.0f);
    SetActiveChart(chart, displayW, displayH);

    SnapAfterFrame([this, before, w, h](MemSnapshot after) {
      char tag[64];
      std::snprintf(tag, sizeof(tag), "Size %dx%d (3 series x 50pt)", w, h);

      std::string log = FormatDelta(tag, before, after);
      char gpu[80];
      std::snprintf(gpu, sizeof(gpu), "\n  GPU theory: %d kB", CalcGpuTexKb(w, h));
      log += gpu;
      log += PerfLine();

      PrintAndLog(log);
      mPrevSnap = after;
    });

    char desc[96];
    std::snprintf(desc, sizeof(desc),
      "Size %dx%d  GPU theory: %d kB", w, h, CalcGpuTexKb(w, h));
    SetDesc(desc);
  }

  // P14: leak detection (create/destroy x 100) ──────────────────────────────

  void PhaseLeakTest()
  {
    mLeakCycleCount = 0;
    mLeakStartSnap  = TakeSnapshot();
    mLeakRunning    = true;

    if(mChart) { mChartPanel.Remove(mChart); mChart.Reset(); }

    AddLog("=== Leak detection start (x100) ===");
    FlushLog();
    SetDesc("Create/Destroy 0 / 100");

    RunLeakCycle();
  }

  void RunLeakCycle()
  {
    ChartView chart = ChartView::New(ChartView::Type::LINE,
                                      Vector2(mChartW, mChartH));
    auto xAxisL = ChartAxis::New();
    xAxisL.SetMinimumLimit(0.0f);
    xAxisL.SetMaximumLimit(49.0f);
    chart.SetXAxis(xAxisL);
    auto yAxisL = ChartAxis::New();
    yAxisL.SetMinimumLimit(-1.5f);
    yAxisL.SetMaximumLimit(1.5f);
    chart.SetYAxis(yAxisL);

    LineSeries series = LineSeries::New();
    series.SetName("leak-probe");
    Dali::Vector<float> vals;
    vals.Resize(50);
    for(int i = 0; i < 50; ++i) vals[i] = std::sin(i * 0.1f);
    series.SetValues(vals);
    chart.AddSeries(series);

    chart.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetX((mChartPanelW - mChartW) * 0.5f)
      .SetY(8.0f)
      .SetWidth(mChartW)
      .SetHeight(mChartH));
    mChartPanel.Add(chart);
    mLeakChart = chart;

    if(mLeakTimer) { mLeakTimer.Stop(); mLeakTimer.Reset(); }
    mLeakTimer = Timer::New(LEAK_DELAY_MS);
    mLeakTimer.TickSignal().Connect(this, &MemProbeApp::OnLeakTimerTick);
    mLeakTimer.Start();
  }

  bool OnLeakTimerTick()
  {
    mChartPanel.Remove(mLeakChart);
    mLeakChart.Reset();

    ++mLeakCycleCount;

    if(mLeakCycleCount % 10 == 0)
    {
      MemSnapshot cur = TakeSnapshot();
      char buf[96];
      std::snprintf(buf, sizeof(buf),
        "  %3d/100: RSS=%ld kB (%+ld)",
        mLeakCycleCount, cur.vmRss, cur.vmRss - mLeakStartSnap.vmRss);
      AddLog(buf);
      FlushLog();

      char desc[48];
      std::snprintf(desc, sizeof(desc), "Create/Destroy %d / 100", mLeakCycleCount);
      SetDesc(desc);
    }

    if(mLeakCycleCount < 100)
    {
      RunLeakCycle();
    }
    else
    {
      MemSnapshot end  = TakeSnapshot();
      long        leak = end.vmRss - mLeakStartSnap.vmRss;
      bool        pass = std::abs(leak) <= 64;

      std::string log = FormatDelta("After 100 cycles", mLeakStartSnap, end);
      char verdict[96];
      std::snprintf(verdict, sizeof(verdict),
        "\n  Verdict: %+ld kB  %s", leak, pass ? "[PASS]" : "[WARNING: leak suspected]");
      log += verdict;

      printf("\n[Leak verdict] %s (%+ld kB/100cycle)\n",
             pass ? "PASS" : "WARNING", leak);
      PrintAndLog(log);
      SetDesc(pass ? "Leak detection PASS" : "Leak detection WARNING");

      mPrevSnap    = end;
      mLeakRunning = false;

      if(mAutoRunning)
        ScheduleAutoNext();
    }

    return false;
  }

  // P15~P17: tick label count ───────────────────────────────────────────────

  void PhaseTickLabels(int numTicks)
  {
    MemSnapshot before = TakeSnapshot();
    mBuildStart        = SteadyClock::now();

    // SetLabels requires Dali::Vector<Dali::String>
    Dali::Vector<Dali::String> labels;
    labels.Resize(numTicks);
    for(int i = 0; i < numTicks; ++i)
    {
      char buf[16]; std::snprintf(buf, sizeof(buf), "L%d", i);
      labels[i] = buf;
    }

    ChartView chart = ChartView::New(ChartView::Type::LINE,
                                      Vector2(mChartW, mChartH));
    auto xAxisT = ChartAxis::New();
    xAxisT.SetLabels(labels);
    xAxisT.SetMinimumLimit(0.0f);
    xAxisT.SetMaximumLimit(static_cast<float>(std::max(numTicks - 1, 1)));
    chart.SetXAxis(xAxisT);
    auto yAxisT = ChartAxis::New();
    yAxisT.SetMinimumLimit(0.0f);
    yAxisT.SetMaximumLimit(100.0f);
    chart.SetYAxis(yAxisT);

    LineSeries series = LineSeries::New();
    series.SetName("tick-data");
    Dali::Vector<float> vals;
    vals.Resize(numTicks);
    for(int i = 0; i < numTicks; ++i)
      vals[i] = 50.0f + std::sin(i * 0.3f) * 40.0f;
    series.SetValues(vals);
    chart.AddSeries(series);

    SetActiveChart(chart, mChartW, mChartH);

    SnapAfterFrame([this, before, numTicks](MemSnapshot after) {
      char tag[48];
      std::snprintf(tag, sizeof(tag), "X-tick %d labels", numTicks);

      std::string log = FormatDelta(tag, before, after);
      if(numTicks > 0)
      {
        double heapDelta = static_cast<double>(after.heapUsed - before.heapUsed) * 1024.0;
        char per[80];
        std::snprintf(per, sizeof(per), "\n  Heap/label: %.0f B", heapDelta / numTicks);
        log += per;
      }
      log += PerfLine();

      PrintAndLog(log);
      mPrevSnap = after;
    });

    char desc[48];
    std::snprintf(desc, sizeof(desc), "X-axis tick labels: %d", numTicks);
    SetDesc(desc);
  }

  // done ────────────────────────────────────────────────────────────────────

  void PhaseDone()
  {
    MemSnapshot fin = TakeSnapshot();

    AddLog("=== All phases complete ===");
    AddLog(FormatDelta("Final vs baseline", mBaseline, fin));
    {
      char hwm[80];
      std::snprintf(hwm, sizeof(hwm), "  HWM (peak RSS): %ld kB", fin.vmHwm);
      AddLog(hwm);
    }
    FlushLog();
    SetDesc("All phases done - R/[Reset] to re-run");

    if(mAutoRunning)
    {
      mAutoRunning = false;
      if(mAutoTimer) { mAutoTimer.Stop(); mAutoTimer.Reset(); }
      mAutoButton.SetBackgroundColor(UiColor(0x2e4a7a, 1.0f));
    }
  }

  // =========================================================================
  // Auto-advance timer
  // =========================================================================

  void ScheduleAutoNext()
  {
    if(mAutoTimer) { mAutoTimer.Stop(); mAutoTimer.Reset(); }
    mAutoTimer = Timer::New(AUTO_NEXT_MS);
    mAutoTimer.TickSignal().Connect(this, &MemProbeApp::OnAutoTimerTick);
    mAutoTimer.Start();
  }

  bool OnAutoTimerTick()
  {
    if(mAutoRunning) AdvancePhase();
    return false;
  }

  // =========================================================================
  // Log helpers
  // =========================================================================

  void AddLog(const std::string& text)
  {
    std::istringstream iss(text);
    std::string        line;
    while(std::getline(iss, line))
      mLogLines.push_back(line);

    while(static_cast<int>(mLogLines.size()) > MAX_LOG_LINES)
      mLogLines.erase(mLogLines.begin());
  }

  void FlushLog()
  {
    std::string combined;
    combined.reserve(2048);
    for(const auto& l : mLogLines)
    {
      combined += l;
      combined += '\n';
    }
    mInfoLabel.SetProperty(Label::Property::TEXT, combined.c_str());
  }

  void PrintAndLog(const std::string& text)
  {
    printf("%s\n", text.c_str());
    fflush(stdout);
    AddLog(text);
    FlushLog();
  }

  void SetDesc(const char* text)
  {
    mChartDescLabel.SetProperty(Label::Property::TEXT, text);
  }

  // =========================================================================
  // Button / key handlers
  // =========================================================================

  void OnNext()
  {
    if(mLeakRunning) return; // ignore Next while leak detection is running
    AdvancePhase();
  }

  void OnReset()
  {
    mLeakRunning = false;
    mAutoRunning = false;
    if(mAutoTimer) { mAutoTimer.Stop(); mAutoTimer.Reset(); }
    if(mSnapTimer) { mSnapTimer.Stop(); mSnapTimer.Reset(); }
    if(mLeakTimer) { mLeakTimer.Stop(); mLeakTimer.Reset(); }
    mSnapCallback = nullptr;
    mSnapPending  = false;

    if(mLeakChart) { mChartPanel.Remove(mLeakChart); mLeakChart.Reset(); }
    if(mChart)     { mChartPanel.Remove(mChart);     mChart.Reset(); }

    mCurrentPhase    = -1;
    mLeakCycleCount  = 0;
    mBuildMs         = 0.0;
    mRenderMs        = -1.0;
    mFramesRemaining = 0;
    mPendingFrameId  = -1;   // ignore any in-flight frame-rendered callback
    mLogLines.clear();
    mBaseline = TakeSnapshot();
    mPrevSnap = mBaseline;

    mAutoButton.SetBackgroundColor(UiColor(0x2e4a7a, 1.0f));
    mPhaseLabel.SetProperty(Label::Property::TEXT, "Ready");

    AddLog("=== Reset ===");
    {
      char buf[96];
      std::snprintf(buf, sizeof(buf),
        "Baseline RSS: %ld kB  Heap: %ld kB",
        mBaseline.vmRss, mBaseline.heapUsed);
      AddLog(buf);
    }
    FlushLog();
    SetDesc("");
  }

  void OnToggleAuto()
  {
    if(mAutoRunning)
    {
      mAutoRunning = false;
      if(mAutoTimer) { mAutoTimer.Stop(); mAutoTimer.Reset(); }
      mAutoButton.SetBackgroundColor(UiColor(0x2e4a7a, 1.0f));
    }
    else
    {
      mAutoRunning = true;
      mAutoButton.SetBackgroundColor(UiColor(0x7a2e2e, 1.0f));
      if(!mLeakRunning)
        AdvancePhase();
    }
  }

  void AdvancePhase()
  {
    int next = mCurrentPhase + 1;
    if(next >= static_cast<int>(mPhases.size())) return;

    mCurrentPhase = next;
    mPhaseLabel.SetProperty(Label::Property::TEXT,
                            mPhaseNames[mCurrentPhase].c_str());

    printf("\n[PHASE %d] %s\n",
           mCurrentPhase, mPhaseNames[mCurrentPhase].c_str());
    fflush(stdout);

    mPhases[mCurrentPhase]();
  }

  // KeyEventSignal: Signal<void(Window, KeyEvent)>
  void OnKeyEvent(KeyEvent event)
  {
    if(event.GetState() != KeyEvent::DOWN) return;

    Dali::String key = event.GetKeyName();
    if     (key == "Right" || key == "Return") OnNext();
    else if(key == "r"     || key == "R")      OnReset();
    else if(key == "a"     || key == "A")      OnToggleAuto();
    else if(key == "Escape")                   mApp.Quit();
  }
};

// =============================================================================
// Entry point
// =============================================================================

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application         app = Application::New(&argc, &argv);
  Components::UiConfig config = Components::UiConfig::New();
  config.SetDefaultStateEffectForInteractive(OverlayEffect::Plain());
  config.Apply();
  MemProbeApp         probe(app);
  app.MainLoop();
  return 0;
}
