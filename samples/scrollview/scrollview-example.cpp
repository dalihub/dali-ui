/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
 * scrollview-example.cpp
 *
 * Two diagnostic panels + ScrollView:
 *
 *   [0,   0] 600x160  ScrollStateObserver — live gesture/scroll/drag/fling states
 *   [0, 160] 600x120  ScrollView Signals  — last fired signal + scroll position + delta
 *   [0, 280] 600x1120 ScrollView
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/internal/scroll-state-observer.h>
#include <sstream>

using namespace Dali;
using namespace Dali::Ui;
using ScrollStateObserver = Dali::Ui::Internal::ScrollStateObserver;

// ─── layout ──────────────────────────────────────────────────────────────────
static constexpr float WINDOW_W       = 600.0f;
static constexpr float WINDOW_H       = 1400.0f;
static constexpr float OBSERVER_H     = 160.0f;
static constexpr float SIGNAL_LOG_H   = 120.0f;
static constexpr float SCROLL_Y       = OBSERVER_H + SIGNAL_LOG_H;
static constexpr float SCROLL_VIEW_H  = WINDOW_H - SCROLL_Y;
static constexpr float CONTENT_H      = 2400.0f;

// ─── colours ─────────────────────────────────────────────────────────────────
static const Vector4 COLOR_PANEL_BG (0.12f, 0.12f, 0.12f, 1.0f);
static const Vector4 COLOR_LOG_BG   (0.08f, 0.10f, 0.14f, 1.0f);
static const Vector4 COLOR_ACTIVE   (0.20f, 0.85f, 0.40f, 1.0f);
static const Vector4 COLOR_INACTIVE (0.35f, 0.35f, 0.35f, 1.0f);

// ─────────────────────────────────────────────────────────────────────────────

class ScrollViewController : public ConnectionTracker
{
public:
  explicit ScrollViewController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &ScrollViewController::Create);
  }

  ~ScrollViewController() = default;

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);
    window.KeyEventSignal().Connect(this, &ScrollViewController::OnKeyEvent);

    BuildObserverPanel(window);
    BuildSignalLogPanel(window);
    BuildScrollView(window);

    ConnectObserverSignals();
    ConnectScrollViewSignals();
  }

private:
  // ── Section 1: ScrollStateObserver ─────────────────────────────────────────

  void BuildObserverPanel(Window& window)
  {
    View panel = View::New()
      .SetBackgroundColor(COLOR_PANEL_BG)
      .SetRequestedWidth(WINDOW_W)
      .SetRequestedHeight(OBSERVER_H)
      .SetRequestedPositionX(0.0f)
      .SetRequestedPositionY(0.0f);

    panel.Add(Label::New("ScrollStateObserver")
      .SetRequestedWidth(WINDOW_W)
      .SetRequestedHeight(34.0f)
      .SetRequestedPositionX(0.0f)
      .SetRequestedPositionY(4.0f)
      .SetTextColor(Color::WHITE));

    static constexpr float CHIP_W     = 140.0f;
    static constexpr float CHIP_H     = 48.0f;
    static constexpr float CHIP_GAP   = 8.0f;
    static constexpr float ROW_Y      = 44.0f;
    static constexpr float ROW_X0     = (WINDOW_W - (CHIP_W * 4 + CHIP_GAP * 3)) * 0.5f;

    const char* names[4] = {"Disambiguating", "Dragging", "Scrolling", "Flinging"};
    for(int i = 0; i < 4; ++i)
    {
      float x = ROW_X0 + i * (CHIP_W + CHIP_GAP);

      mObserverChips[i] = View::New()
        .SetBackgroundColor(COLOR_INACTIVE)
        .SetRequestedWidth(CHIP_W)
        .SetRequestedHeight(CHIP_H)
        .SetRequestedPositionX(x)
        .SetRequestedPositionY(ROW_Y);
      mObserverChips[i].SetProperty(View::Property::CORNER_RADIUS, Vector4(8.0f, 8.0f, 8.0f, 8.0f));
      panel.Add(mObserverChips[i]);

      mObserverChips[i].Add(Label::New(names[i])
        .SetRequestedWidth(CHIP_W)
        .SetRequestedHeight(CHIP_H / 2.0f)
        .SetRequestedPositionX(0.0f)
        .SetRequestedPositionY(4.0f)
        .SetTextColor(Color::WHITE));

      mObserverValues[i] = Label::New("OFF")
        .SetRequestedWidth(CHIP_W)
        .SetRequestedHeight(CHIP_H / 2.0f)
        .SetRequestedPositionX(0.0f)
        .SetRequestedPositionY(CHIP_H / 2.0f)
        .SetTextColor(Color::WHITE);
      mObserverChips[i].Add(mObserverValues[i]);
    }

    window.Add(panel);
  }

  void RefreshObserverPanel()
  {
    auto& obs = ScrollStateObserver::Get();
    const bool states[4] = {
      obs.IsGestureDisambiguating(),
      obs.IsDragging(),
      obs.IsScrolling(),
      obs.IsFlinging(),
    };
    for(int i = 0; i < 4; ++i)
    {
      mObserverChips[i].SetBackgroundColor(states[i] ? COLOR_ACTIVE : COLOR_INACTIVE);
      mObserverValues[i].SetText(states[i] ? "ON" : "OFF");
    }
  }

  void ConnectObserverSignals()
  {
    auto& obs = ScrollStateObserver::Get();
    obs.DisambiguationBeganSignal().Connect(this, &ScrollViewController::OnObserverDisambiguationBegan);
    obs.DisambiguationEndedSignal().Connect(this, &ScrollViewController::OnObserverDisambiguationEnded);
    obs.DragStartedSignal().Connect(this,  &ScrollViewController::OnObserverDragStarted);
    obs.DragFinishedSignal().Connect(this, &ScrollViewController::OnObserverDragFinished);
    obs.ScrollStartedSignal().Connect(this,  &ScrollViewController::OnObserverScrollStarted);
    obs.ScrollFinishedSignal().Connect(this, &ScrollViewController::OnObserverScrollFinished);
  }

  void OnObserverDisambiguationBegan()  { RefreshObserverPanel(); }
  void OnObserverDisambiguationEnded()  { RefreshObserverPanel(); }
  void OnObserverDragStarted()          { RefreshObserverPanel(); }
  void OnObserverDragFinished()         { RefreshObserverPanel(); }
  void OnObserverScrollStarted()        { RefreshObserverPanel(); }
  void OnObserverScrollFinished()       { RefreshObserverPanel(); }

  // ── Section 2: ScrollView Signals ──────────────────────────────────────────

  void BuildSignalLogPanel(Window& window)
  {
    View panel = View::New()
      .SetBackgroundColor(COLOR_LOG_BG)
      .SetRequestedWidth(WINDOW_W)
      .SetRequestedHeight(SIGNAL_LOG_H)
      .SetRequestedPositionX(0.0f)
      .SetRequestedPositionY(OBSERVER_H);

    panel.Add(Label::New("ScrollView Signals")
      .SetRequestedWidth(WINDOW_W)
      .SetRequestedHeight(28.0f)
      .SetRequestedPositionX(0.0f)
      .SetRequestedPositionY(2.0f)
      .SetTextColor(Color::WHITE));

    // Row: "Last:  <signal name>"
    mLastSignalLabel = Label::New("Last:  —")
      .SetRequestedWidth(WINDOW_W * 0.5f)
      .SetRequestedHeight(28.0f)
      .SetRequestedPositionX(8.0f)
      .SetRequestedPositionY(30.0f)
      .SetTextColor(Color::WHITE);
    panel.Add(mLastSignalLabel);

    // Row: scroll position (right half)
    mScrollPosLabel = Label::New("ScrollPos: (0, 0)")
      .SetRequestedWidth(WINDOW_W * 0.5f)
      .SetRequestedHeight(28.0f)
      .SetRequestedPositionX(WINDOW_W * 0.5f)
      .SetRequestedPositionY(30.0f)
      .SetTextColor(Color::WHITE);
    panel.Add(mScrollPosLabel);

    // Row: drag delta
    mDragDeltaLabel = Label::New("Delta: (0.0, 0.0)")
      .SetRequestedWidth(WINDOW_W * 0.5f)
      .SetRequestedHeight(28.0f)
      .SetRequestedPositionX(8.0f)
      .SetRequestedPositionY(60.0f)
      .SetTextColor(Color::WHITE);
    panel.Add(mDragDeltaLabel);

    // Row: fling velocity
    mFlingVelocityLabel = Label::New("FlingDelta: (0.0, 0.0)")
      .SetRequestedWidth(WINDOW_W * 0.5f)
      .SetRequestedHeight(28.0f)
      .SetRequestedPositionX(WINDOW_W * 0.5f)
      .SetRequestedPositionY(60.0f)
      .SetTextColor(Color::WHITE);
    panel.Add(mFlingVelocityLabel);

    window.Add(panel);
  }

  void SetLastSignal(const char* name)
  {
    std::ostringstream oss;
    oss << "Last:  " << name;
    mLastSignalLabel.SetText(oss.str().c_str());
  }

  void UpdateScrollPos(ScrollView sv)
  {
    Vector2 pos = sv.GetScrollPosition();
    std::ostringstream oss;
    oss << "ScrollPos: (" << static_cast<int>(pos.x) << ", " << static_cast<int>(pos.y) << ")";
    mScrollPosLabel.SetText(oss.str().c_str());
  }

  void ConnectScrollViewSignals()
  {
    mScrollView.ScrollStartedSignal().Connect(this,  &ScrollViewController::OnSVScrollStarted);
    mScrollView.ScrollingSignal().Connect(this,      &ScrollViewController::OnSVScrolling);
    mScrollView.ScrollFinishedSignal().Connect(this, &ScrollViewController::OnSVScrollFinished);
    mScrollView.DragStartedSignal().Connect(this,    &ScrollViewController::OnSVDragStarted);
    mScrollView.DraggingSignal().Connect(this,       &ScrollViewController::OnSVDragging);
    mScrollView.DragFinishedSignal().Connect(this,   &ScrollViewController::OnSVDragFinished);
  }

  void OnSVScrollStarted(ScrollView sv)
  {
    SetLastSignal("ScrollStarted");
    UpdateScrollPos(sv);
  }

  void OnSVScrolling(ScrollView sv)
  {
    SetLastSignal("Scrolling");
    UpdateScrollPos(sv);
  }

  void OnSVScrollFinished(ScrollView sv)
  {
    SetLastSignal("ScrollFinished");
    UpdateScrollPos(sv);
  }

  void OnSVDragStarted(ScrollView sv)
  {
    SetLastSignal("DragStarted");
    UpdateScrollPos(sv);
    mDragDeltaLabel.SetText("Delta: (0.0, 0.0)");
  }

  void OnSVDragging(ScrollView sv, float deltaX, float deltaY)
  {
    SetLastSignal("Dragging");
    UpdateScrollPos(sv);
    std::ostringstream oss;
    oss << "Delta: (" << deltaX << ", " << deltaY << ")";
    mDragDeltaLabel.SetText(oss.str().c_str());
  }

  void OnSVDragFinished(ScrollView sv)
  {
    SetLastSignal("DragFinished");
    UpdateScrollPos(sv);

    // Show fling displacement once drag ends
    Vector2 pos = sv.GetScrollPosition();
    std::ostringstream oss;
    oss << "FlingStart: (" << static_cast<int>(pos.x) << ", " << static_cast<int>(pos.y) << ")";
    mFlingVelocityLabel.SetText(oss.str().c_str());
  }

  // ── Section 3: ScrollView ──────────────────────────────────────────────────

  void BuildScrollView(Window& window)
  {
    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(WINDOW_W);
    content.SetRequestedHeight(CONTENT_H);
    content.SetSpacing(10.0f);
    content.SetPadding(Extents(20, 20, 20, 20));

    static const Vector4 blockColors[3] = {Color::RED, Color::GREEN, Color::BLUE};
    for(int i = 0; i < 3; ++i)
    {
      View block = View::New()
        .SetBackgroundColor(blockColors[i])
        .SetRequestedWidth(MATCH_PARENT)
        .SetRequestedHeight(WRAP_CONTENT)
        .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
      content.Add(block);
    }

    mScrollView = ScrollView::New()
      .SetScrollDirection(ScrollDirection::Vertical)
      .SetMaxFlingDistance(6000.0f)
      .SetMinimumFlingDuration(1000)
      .SetMaximumFlingDuration(2000)
      .SetFlingSensitivity(1.0f)
      .SetDecelerationRate(0.998f)
      .SetOverScrollMode(OverScrollMode::ContentScrolls)
      .SetRequestedWidth(WINDOW_W)
      .SetRequestedHeight(SCROLL_VIEW_H)
      .SetRequestedPositionX(0.0f)
      .SetRequestedPositionY(SCROLL_Y)
      .SetBackgroundColor(Vector4(0.9f, 0.9f, 0.9f, 1.0f))
      .SetContent(content);

    window.Add(mScrollView);
  }

  // ── Key ────────────────────────────────────────────────────────────────────

  void OnKeyEvent(Window window, KeyEvent event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

  // ── Members ────────────────────────────────────────────────────────────────

  Application& mApplication;
  ScrollView   mScrollView;

  // Observer panel
  View  mObserverChips[4];
  Label mObserverValues[4];

  // Signal log panel
  Label mLastSignalLabel;
  Label mScrollPosLabel;
  Label mDragDeltaLabel;
  Label mFlingVelocityLabel;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  ScrollViewController test(application);
  application.MainLoop();
  return 0;
}
