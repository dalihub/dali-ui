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
 * Three diagnostic panels + ScrollView:
 *
 *   [0,   0] 600x160  ScrollStateObserver — live gesture/scroll/drag/fling states
 *   [0, 160] 600x100  ScrollView Signals  — last fired signal + scroll position + delta
 *   [0, 260] 600x140  Focus Scroll        — ScrollOnFocus toggle, mode selector, focused item
 *   [0, 400] 600x1000 ScrollView          — 15 focusable items
 *
 * Keyboard:
 *   ↑ / ↓     — move focus between items
 *   S         — toggle ScrollOnFocus on / off
 *   1         — mode: MakeVisible
 *   2         — mode: Start
 *   3         — mode: Center
 *   4         — mode: End
 *   + / -     — increase / decrease FocusScrollPeek by 10 px
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/internal/scroll-state-observer.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>
#include <sstream>

using namespace Dali;
using namespace Dali::Ui;
using ScrollStateObserver = Dali::Ui::Internal::ScrollStateObserver;

// ─── layout ──────────────────────────────────────────────────────────────────
static constexpr float WINDOW_W      = 600.0f;
static constexpr float WINDOW_H      = 1080.0f;
static constexpr float OBSERVER_H    = 130.0f;
static constexpr float SIGNAL_LOG_H  = 90.0f;
static constexpr float FOCUS_PANEL_H = 180.0f;
static constexpr float SCROLL_Y      = OBSERVER_H + SIGNAL_LOG_H + FOCUS_PANEL_H; // 400
static constexpr float SCROLL_VIEW_H = WINDOW_H - SCROLL_Y;                       // 680
static constexpr float FOCUS_SCROLL_PEEK = 40.0f;
static constexpr int   ITEM_COUNT    = 15;
static constexpr float ITEM_H        = 130.0f;
static constexpr float ITEM_SPACING  = 10.0f;
static constexpr float CONTENT_PAD   = 16.0f;
// total content height: 2*pad + N*item + (N-1)*spacing
static constexpr float CONTENT_H     = CONTENT_PAD * 2 + ITEM_COUNT * ITEM_H + (ITEM_COUNT - 1) * ITEM_SPACING;

// ─── colours ─────────────────────────────────────────────────────────────────
static const Vector4 COLOR_PANEL_BG    (0.12f, 0.12f, 0.12f, 1.0f);
static const Vector4 COLOR_LOG_BG      (0.08f, 0.10f, 0.14f, 1.0f);
static const Vector4 COLOR_FOCUS_BG    (0.10f, 0.12f, 0.18f, 1.0f);
static const Vector4 COLOR_ACTIVE      (0.20f, 0.85f, 0.40f, 1.0f);
static const Vector4 COLOR_INACTIVE    (0.35f, 0.35f, 0.35f, 1.0f);
static const Vector4 COLOR_MODE_ON     (0.20f, 0.55f, 0.90f, 1.0f);
static const Vector4 COLOR_ITEM_EVEN   (0.82f, 0.87f, 0.95f, 1.0f);
static const Vector4 COLOR_ITEM_ODD    (0.88f, 0.93f, 0.88f, 1.0f);
static const Vector4 COLOR_ITEM_FOCUS  (0.20f, 0.50f, 0.88f, 1.0f);

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
    window.SetSize(Dali::Window::WindowSize(static_cast<uint32_t>(WINDOW_W), static_cast<uint32_t>(WINDOW_H)));
    window.SetBackgroundColor(Color::WHITE);
    window.KeyEventSignal().Connect(this, &ScrollViewController::OnKeyEvent);

    BuildObserverPanel(window);
    BuildSignalLogPanel(window);
    BuildFocusPanel(window);
    BuildScrollView(window);

    ConnectObserverSignals();
    ConnectScrollViewSignals();

    // Highlight focus changes: update item visuals and the focus panel
    FocusManager::Get().FocusChangedSignal().Connect(this, &ScrollViewController::OnFocusChanged);

    // Sync panel labels with the initial ScrollView state
    RefreshFocusPanel();
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
      .SetRequestedHeight(26.0f)
      .SetRequestedPositionX(0.0f)
      .SetRequestedPositionY(2.0f)
      .SetTextColor(Color::WHITE));

    mLastSignalLabel = Label::New("Last:  —")
      .SetRequestedWidth(WINDOW_W * 0.5f)
      .SetRequestedHeight(26.0f)
      .SetRequestedPositionX(8.0f)
      .SetRequestedPositionY(28.0f)
      .SetTextColor(Color::WHITE);
    panel.Add(mLastSignalLabel);

    mScrollPosLabel = Label::New("ScrollPos: (0, 0)")
      .SetRequestedWidth(WINDOW_W * 0.5f)
      .SetRequestedHeight(26.0f)
      .SetRequestedPositionX(WINDOW_W * 0.5f)
      .SetRequestedPositionY(28.0f)
      .SetTextColor(Color::WHITE);
    panel.Add(mScrollPosLabel);

    mDragDeltaLabel = Label::New("Delta: (0.0, 0.0)")
      .SetRequestedWidth(WINDOW_W * 0.5f)
      .SetRequestedHeight(26.0f)
      .SetRequestedPositionX(8.0f)
      .SetRequestedPositionY(56.0f)
      .SetTextColor(Color::WHITE);
    panel.Add(mDragDeltaLabel);

    mFlingVelocityLabel = Label::New("FlingStart: (0, 0)")
      .SetRequestedWidth(WINDOW_W * 0.5f)
      .SetRequestedHeight(26.0f)
      .SetRequestedPositionX(WINDOW_W * 0.5f)
      .SetRequestedPositionY(56.0f)
      .SetTextColor(Color::WHITE);
    panel.Add(mFlingVelocityLabel);

    window.Add(panel);
  }

  // ── Section 3: Focus Scroll ─────────────────────────────────────────────────

  void BuildFocusPanel(Window& window)
  {
    static constexpr float PANEL_Y = OBSERVER_H + SIGNAL_LOG_H;

    View panel = View::New()
      .SetBackgroundColor(COLOR_FOCUS_BG)
      .SetRequestedWidth(WINDOW_W)
      .SetRequestedHeight(FOCUS_PANEL_H)
      .SetRequestedPositionX(0.0f)
      .SetRequestedPositionY(PANEL_Y);

    panel.Add(Label::New("Focus Scroll")
      .SetRequestedWidth(WINDOW_W)
      .SetRequestedHeight(26.0f)
      .SetRequestedPositionX(0.0f)
      .SetRequestedPositionY(4.0f)
      .SetTextColor(Color::WHITE));

    // Row 1: ScrollOnFocus toggle chip + Focused label
    static constexpr float CHIP_H   = 34.0f;
    static constexpr float CHIP_W   = 160.0f;
    static constexpr float ROW1_Y   = 34.0f;

    mScrollOnFocusChip = View::New()
      .SetBackgroundColor(COLOR_ACTIVE)
      .SetRequestedWidth(CHIP_W)
      .SetRequestedHeight(CHIP_H)
      .SetRequestedPositionX(8.0f)
      .SetRequestedPositionY(ROW1_Y);
    mScrollOnFocusChip.SetProperty(View::Property::CORNER_RADIUS, Vector4(6, 6, 6, 6));
    mScrollOnFocusChip.Add(Label::New("ScrollOnFocus: ON")
      .SetRequestedWidth(CHIP_W)
      .SetRequestedHeight(CHIP_H)
      .SetTextColor(Color::WHITE));
    panel.Add(mScrollOnFocusChip);

    mFocusedLabel = Label::New("Focused: —")
      .SetRequestedWidth(WINDOW_W - CHIP_W - 24.0f)
      .SetRequestedHeight(CHIP_H)
      .SetRequestedPositionX(CHIP_W + 16.0f)
      .SetRequestedPositionY(ROW1_Y)
      .SetTextColor(Color::WHITE);
    panel.Add(mFocusedLabel);

    // Row 2: Mode chips  [MakeVisible] [Start] [Center] [End]
    static constexpr float MODE_CHIP_W = 128.0f;
    static constexpr float MODE_GAP    = 8.0f;
    static constexpr float ROW2_Y      = ROW1_Y + CHIP_H + 8.0f;
    static constexpr float MODE_ROW_X0 = (WINDOW_W - (MODE_CHIP_W * 4 + MODE_GAP * 3)) * 0.5f;
    static const char*     MODE_NAMES[4] = {"MakeVisible", "Start", "Center", "End"};

    for(int i = 0; i < 4; ++i)
    {
      float x = MODE_ROW_X0 + i * (MODE_CHIP_W + MODE_GAP);
      mModeChips[i] = View::New()
        .SetBackgroundColor(i == 0 ? COLOR_MODE_ON : COLOR_INACTIVE)
        .SetRequestedWidth(MODE_CHIP_W)
        .SetRequestedHeight(CHIP_H)
        .SetRequestedPositionX(x)
        .SetRequestedPositionY(ROW2_Y);
      mModeChips[i].SetProperty(View::Property::CORNER_RADIUS, Vector4(6, 6, 6, 6));
      mModeChips[i].Add(Label::New(MODE_NAMES[i])
        .SetRequestedWidth(MODE_CHIP_W)
        .SetRequestedHeight(CHIP_H)
        .SetTextColor(Color::WHITE));
      mModeChips[i].TouchedSignal().Connect(this, &ScrollViewController::OnModeChipTouched);
      panel.Add(mModeChips[i]);
    }

    // Row 3: Peek label + -/+ buttons
    static constexpr float ROW3_Y   = ROW2_Y + CHIP_H + 8.0f;

    panel.Add(Label::New("Peek:")
      .SetRequestedWidth(52.0f)
      .SetRequestedHeight(CHIP_H)
      .SetRequestedPositionX(8.0f)
      .SetRequestedPositionY(ROW3_Y)
      .SetTextColor(Color::WHITE));

    mPeekLabel = Label::New("0 px")
      .SetRequestedWidth(80.0f)
      .SetRequestedHeight(CHIP_H)
      .SetRequestedPositionX(64.0f)
      .SetRequestedPositionY(ROW3_Y)
      .SetTextColor(Color::WHITE);
    panel.Add(mPeekLabel);

    panel.Add(Label::New("( +/- keys to adjust )")
      .SetRequestedWidth(WINDOW_W - 64.0f - 80.0f - 8.0f)
      .SetRequestedHeight(CHIP_H)
      .SetRequestedPositionX(64.0f + 80.0f + 8.0f)
      .SetRequestedPositionY(ROW3_Y)
      .SetTextColor(Color::WHITE));

    // Key hint row
    panel.Add(Label::New("Keys:  ↑↓ focus   S toggle   1~4 mode   +/- peek")
      .SetRequestedWidth(WINDOW_W - 8.0f)
      .SetRequestedHeight(22.0f)
      .SetRequestedPositionX(8.0f)
      .SetRequestedPositionY(ROW3_Y + CHIP_H + 2.0f)
      .SetTextColor(Color::WHITE));

    window.Add(panel);
  }

  void RefreshFocusPanel()
  {
    // ScrollOnFocus chip
    bool on = mScrollView.GetScrollOnFocus();
    mScrollOnFocusChip.SetBackgroundColor(on ? COLOR_ACTIVE : COLOR_INACTIVE);
    std::ostringstream sv;
    sv << "ScrollOnFocus: " << (on ? "ON" : "OFF");
    Label::DownCast(mScrollOnFocusChip.GetChildAt(0)).SetText(sv.str().c_str());

    // Mode chips
    static const ScrollToPosition MODES[4] = {
      ScrollToPosition::MakeVisible,
      ScrollToPosition::Start,
      ScrollToPosition::Center,
      ScrollToPosition::End,
    };
    ScrollToPosition current = mScrollView.GetFocusScrollToPosition();
    for(int i = 0; i < 4; ++i)
    {
      mModeChips[i].SetBackgroundColor(MODES[i] == current ? COLOR_MODE_ON : COLOR_INACTIVE);
    }

    // Peek label
    std::ostringstream pk;
    pk << static_cast<int>(mScrollView.GetFocusScrollPeek()) << " px";
    mPeekLabel.SetText(pk.str().c_str());
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

  // ── Section 4: ScrollView ──────────────────────────────────────────────────

  void BuildScrollView(Window& window)
  {
    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(WINDOW_W);
    content.SetRequestedHeight(CONTENT_H);
    content.SetSpacing(ITEM_SPACING);
    content.SetPadding(Extents(16, 16, 16, 16));

    for(int i = 0; i < ITEM_COUNT; ++i)
    {
      View item = View::New()
        .SetBackgroundColor(i % 2 == 0 ? COLOR_ITEM_EVEN : COLOR_ITEM_ODD)
        .SetRequestedWidth(MATCH_PARENT)
        .SetRequestedHeight(ITEM_H);
      item.SetProperty(View::Property::CORNER_RADIUS, Vector4(10, 10, 10, 10));
      item.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);

      // Item number label (large, centered vertically)
      std::ostringstream nameOss;
      nameOss << "Item " << (i + 1);
      Label nameLabel = Label::New(nameOss.str().c_str())
        .SetRequestedWidth(WINDOW_W - 2 * CONTENT_PAD)
        .SetRequestedHeight(ITEM_H * 0.6f)
        .SetRequestedPositionX(0.0f)
        .SetRequestedPositionY(ITEM_H * 0.1f);
      item.Add(nameLabel);

      // Hint label (small, bottom)
      std::ostringstream hintOss;
      hintOss << "index " << i << "  (y ≈ " << static_cast<int>(CONTENT_PAD + i * (ITEM_H + ITEM_SPACING)) << "px)";
      Label hintLabel = Label::New(hintOss.str().c_str())
        .SetRequestedWidth(WINDOW_W - 2 * CONTENT_PAD)
        .SetRequestedHeight(ITEM_H * 0.3f)
        .SetRequestedPositionX(0.0f)
        .SetRequestedPositionY(ITEM_H * 0.65f);
      item.Add(hintLabel);

      mFocusItems[i]      = item;
      mFocusItemNames[i]  = nameLabel;
      content.Add(item);
    }

    mScrollView = ScrollView::New()
      .SetScrollDirection(ScrollDirection::Vertical)
      .SetMaxFlingDistance(6000.0f)
      .SetMinimumFlingDuration(1000)
      .SetMaximumFlingDuration(2000)
      .SetFlingSensitivity(1.0f)
      .SetDecelerationRate(0.998f)
      .SetOverScrollMode(OverScrollMode::ContentScrolls)
      .SetScrollOnFocus(true)
      .SetFocusScrollToPosition(ScrollToPosition::MakeVisible)
      .SetFocusScrollPeek(FOCUS_SCROLL_PEEK)
      .SetRequestedWidth(WINDOW_W)
      .SetRequestedHeight(SCROLL_VIEW_H)
      .SetRequestedPositionX(0.0f)
      .SetRequestedPositionY(SCROLL_Y)
      .SetBackgroundColor(Vector4(0.9f, 0.9f, 0.9f, 1.0f))
      .SetContent(content);

    window.Add(mScrollView);
  }

  // ── Mode chip touch ────────────────────────────────────────────────────────

  bool OnModeChipTouched(Actor actor, TouchEvent touch)
  {
    if(touch.GetState(0) != PointState::UP)
    {
      return false;
    }
    static const ScrollToPosition MODES[4] = {
      ScrollToPosition::MakeVisible,
      ScrollToPosition::Start,
      ScrollToPosition::Center,
      ScrollToPosition::End,
    };
    for(int i = 0; i < 4; ++i)
    {
      if(actor == mModeChips[i])
      {
        mScrollView.SetFocusScrollToPosition(MODES[i]);
        RefreshFocusPanel();
        return true;
      }
    }
    return false;
  }

  // ── Focus change ───────────────────────────────────────────────────────────

  void OnFocusChanged(View from, View to)
  {
    // Restore previous item colour
    for(int i = 0; i < ITEM_COUNT; ++i)
    {
      if(mFocusItems[i] == from)
      {
        mFocusItems[i].SetBackgroundColor(i % 2 == 0 ? COLOR_ITEM_EVEN : COLOR_ITEM_ODD);
        mFocusItemNames[i].SetTextColor(Color::BLACK);
      }
      if(mFocusItems[i] == to)
      {
        mCurrentFocusIndex = i;
        mFocusItems[i].SetBackgroundColor(COLOR_ITEM_FOCUS);
        mFocusItemNames[i].SetTextColor(Color::WHITE);
      }
    }

    // Update focused label
    if(mCurrentFocusIndex >= 0)
    {
      std::ostringstream oss;
      oss << "Focused: Item " << (mCurrentFocusIndex + 1);
      mFocusedLabel.SetText(oss.str().c_str());
    }
    else
    {
      mFocusedLabel.SetText("Focused: —");
    }
  }

  void MoveFocusBy(int delta)
  {
    int next = mCurrentFocusIndex + delta;
    if(next < 0 || next >= ITEM_COUNT)
    {
      return;
    }
    FocusManager::Get().SetCurrentFocusView(mFocusItems[next]);
  }

  // ── Key ────────────────────────────────────────────────────────────────────

  void OnKeyEvent(Window /*window*/, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::DOWN)
    {
      return;
    }

    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
      return;
    }

    const Dali::String& key = event.GetKeyName();

    if(key == "Up")
    {
      // Move focus to previous item; ScrollView auto-scrolls via OnFocusChanged
      MoveFocusBy(-1);
    }
    else if(key == "Down")
    {
      // Move focus to next item
      MoveFocusBy(+1);
    }
    else if(key == "s" || key == "S")
    {
      // Toggle ScrollOnFocus
      mScrollView.SetScrollOnFocus(!mScrollView.GetScrollOnFocus());
      RefreshFocusPanel();
    }
    else if(key == "1")
    {
      mScrollView.SetFocusScrollToPosition(ScrollToPosition::MakeVisible);
      RefreshFocusPanel();
    }
    else if(key == "2")
    {
      mScrollView.SetFocusScrollToPosition(ScrollToPosition::Start);
      RefreshFocusPanel();
    }
    else if(key == "3")
    {
      mScrollView.SetFocusScrollToPosition(ScrollToPosition::Center);
      RefreshFocusPanel();
    }
    else if(key == "4")
    {
      mScrollView.SetFocusScrollToPosition(ScrollToPosition::End);
      RefreshFocusPanel();
    }
    else if(key == "plus" || key == "equal")
    {
      float peek = mScrollView.GetFocusScrollPeek() + 10.0f;
      mScrollView.SetFocusScrollPeek(peek);
      RefreshFocusPanel();
    }
    else if(key == "minus")
    {
      float peek = mScrollView.GetFocusScrollPeek() - 10.0f;
      if(peek < 0.0f) peek = 0.0f;
      mScrollView.SetFocusScrollPeek(peek);
      RefreshFocusPanel();
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

  // Focus scroll panel
  View  mScrollOnFocusChip;
  View  mModeChips[4];
  Label mFocusedLabel;
  Label mPeekLabel;

  // Focusable items
  View  mFocusItems[ITEM_COUNT];
  Label mFocusItemNames[ITEM_COUNT];
  int   mCurrentFocusIndex{-1};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  ScrollViewController test(application);
  application.MainLoop();
  return 0;
}
