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
 *   [0,   0] 600x130  Scroll State         — live scroll/drag/fling states, derived
 *                                            from ScrollView's public signals
 *   [0, 130] 600x90   ScrollView Signals  — last fired signal + scroll position + delta
 *   [0, 220] 600x214  Focus Scroll        — ScrollOnFocus, mode, peek, key-scroll controls
 *   [0, 434] 600x646  ScrollView          — 15 focusable items in 3 groups of 5,
 *                                           separated by 320px non-focusable gaps.
 *                                           Within a group: adjacent items are 140px
 *                                           apart (< KEY_SCROLL_STEP=150) → focus moves.
 *                                           Across a gap: distance is 320px (> 150)
 *                                           → key-scroll fires until close enough.
 *
 * Keyboard:
 *   ↑ / ↓     — move focus between items (FocusManager navigation)
 *   F         — programmatically focus last item (tests scroll-on-focus via API)
 *   S         — toggle ScrollOnFocus on / off
 *   1         — mode: MakeVisible
 *   2         — mode: Start
 *   3         — mode: Center
 *   4         — mode: End
 *   + / -     — increase / decrease FocusScrollPeek by 10 px
 *   F         — programmatically focus Item 15 (tests scroll-on-focus via API)
 *   K         — toggle KeyScrollEnabled on / off
 *   [ / ]     — decrease / increase KeyScrollStep by 20 px
 *   E         — toggle BounceEdgeEffect on / off (drag past top/bottom to test)
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/views/scroll/bounce-edge-effect.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>
#include <sstream>

using namespace Dali;
using namespace Dali::Ui;

// ─── layout ──────────────────────────────────────────────────────────────────
static constexpr float WINDOW_W      = 600.0f;
static constexpr float WINDOW_H      = 1080.0f;
static constexpr float OBSERVER_H    = 130.0f;
static constexpr float SIGNAL_LOG_H  = 116.0f; // +26px for edge-effect status row
static constexpr float FOCUS_PANEL_H     = 214.0f;
static constexpr float SCROLL_Y          = OBSERVER_H + SIGNAL_LOG_H + FOCUS_PANEL_H; // 434
static constexpr float SCROLL_VIEW_H     = WINDOW_H - SCROLL_Y;                       // 646
static constexpr float FOCUS_SCROLL_PEEK = 40.0f;
static constexpr float KEY_SCROLL_STEP   = 150.0f; // ~1 item height
static constexpr int   ITEM_COUNT    = 15;
static constexpr int   ITEM_GROUP    = 5;    // items per group; gaps inserted between groups
static constexpr float ITEM_H        = 130.0f;
static constexpr float ITEM_SPACING  = 10.0f;
static constexpr float CONTENT_PAD   = 16.0f;
// Large spacers are inserted between item groups so the distance between the
// last item of one group and the first item of the next exceeds KEY_SCROLL_STEP.
// This makes key-scroll-step actually trigger at group boundaries.
static constexpr float GAP_H         = 320.0f; // >> KEY_SCROLL_STEP
static constexpr int   GAP_COUNT     = ITEM_COUNT / ITEM_GROUP - 1; // 2
// total content height: 2*pad + N*item + (N-1)*spacing + gaps
static constexpr float CONTENT_H     = CONTENT_PAD * 2
                                       + ITEM_COUNT * ITEM_H
                                       + (ITEM_COUNT - 1) * ITEM_SPACING
                                       + GAP_COUNT * GAP_H;

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
    auto positionSize = window.GetPositionSize();
    window.SetPositionSize(Dali::PositionSize(positionSize.x, positionSize.y, static_cast<uint32_t>(WINDOW_W), static_cast<uint32_t>(WINDOW_H)));
    window.SetBackgroundColor(Color::WHITE);
    window.KeyEventSignal().Connect(this, &ScrollViewController::OnKeyEvent);

    BuildObserverPanel(window);
    BuildSignalLogPanel(window);
    BuildFocusPanel(window);
    BuildScrollView(window);

    ConnectScrollViewSignals();

    // Highlight focus changes: update item visuals and the focus panel
    FocusManager::Get().FocusChangedSignal().Connect(this, &ScrollViewController::OnFocusChanged);

    // Sync panel labels with the initial ScrollView state
    RefreshFocusPanel();
  }

private:
  // ── Section 1: Scroll State (derived from ScrollView's public signals) ────

  void BuildObserverPanel(Window& window)
  {
    View panel = View::New();
    panel.SetBackgroundColor(COLOR_PANEL_BG);
    panel.SetRequestedWidth(WINDOW_W);
    panel.SetRequestedHeight(OBSERVER_H);
    panel.SetRequestedX(0.0f);
    panel.SetRequestedY(0.0f);

    Label title = Label::New("Scroll State");
    title.SetRequestedWidth(WINDOW_W);
    title.SetRequestedHeight(34.0f);
    title.SetRequestedX(0.0f);
    title.SetRequestedY(4.0f);
    title.SetTextColor(Color::WHITE);
    panel.Add(title);

    static constexpr float CHIP_W     = 140.0f;
    static constexpr float CHIP_H     = 48.0f;
    static constexpr float CHIP_GAP   = 8.0f;
    static constexpr float ROW_Y      = 44.0f;
    static constexpr float ROW_X0     = (WINDOW_W - (CHIP_W * 3 + CHIP_GAP * 2)) * 0.5f;

    const char* names[3] = {"Dragging", "Scrolling", "Flinging"};
    for(int i = 0; i < 3; ++i)
    {
      float x = ROW_X0 + i * (CHIP_W + CHIP_GAP);

      mObserverChips[i] = View::New();
      mObserverChips[i].SetBackgroundColor(COLOR_INACTIVE);
      mObserverChips[i].SetRequestedWidth(CHIP_W);
      mObserverChips[i].SetRequestedHeight(CHIP_H);
      mObserverChips[i].SetRequestedX(x);
      mObserverChips[i].SetRequestedY(ROW_Y);
      mObserverChips[i].SetProperty(View::Property::CORNER_RADIUS, Vector4(8.0f, 8.0f, 8.0f, 8.0f));
      panel.Add(mObserverChips[i]);

      Label nameLabel = Label::New(names[i]);
      nameLabel.SetRequestedWidth(CHIP_W);
      nameLabel.SetRequestedHeight(CHIP_H / 2.0f);
      nameLabel.SetRequestedX(0.0f);
      nameLabel.SetRequestedY(4.0f);
      nameLabel.SetTextColor(Color::WHITE);
      mObserverChips[i].Add(nameLabel);

      mObserverValues[i] = Label::New("OFF");
      mObserverValues[i].SetRequestedWidth(CHIP_W);
      mObserverValues[i].SetRequestedHeight(CHIP_H / 2.0f);
      mObserverValues[i].SetRequestedX(0.0f);
      mObserverValues[i].SetRequestedY(CHIP_H / 2.0f);
      mObserverValues[i].SetTextColor(Color::WHITE);
      mObserverChips[i].Add(mObserverValues[i]);
    }

    window.Add(panel);
  }

  void RefreshObserverPanel()
  {
    const bool states[3] = {
      mIsDragging,
      mIsScrolling,
      mIsScrolling && !mIsDragging, // flinging: scrolling without an active drag
    };
    for(int i = 0; i < 3; ++i)
    {
      mObserverChips[i].SetBackgroundColor(states[i] ? COLOR_ACTIVE : COLOR_INACTIVE);
      mObserverValues[i].SetText(states[i] ? "ON" : "OFF");
    }
  }

  // ── Section 2: ScrollView Signals ──────────────────────────────────────────

  void BuildSignalLogPanel(Window& window)
  {
    View panel = View::New();
    panel.SetBackgroundColor(COLOR_LOG_BG);
    panel.SetRequestedWidth(WINDOW_W);
    panel.SetRequestedHeight(SIGNAL_LOG_H);
    panel.SetRequestedX(0.0f);
    panel.SetRequestedY(OBSERVER_H);

    Label title = Label::New("ScrollView Signals");
    title.SetRequestedWidth(WINDOW_W);
    title.SetRequestedHeight(26.0f);
    title.SetRequestedX(0.0f);
    title.SetRequestedY(2.0f);
    title.SetTextColor(Color::WHITE);
    panel.Add(title);

    mLastSignalLabel = Label::New("Last:  —");
    mLastSignalLabel.SetRequestedWidth(WINDOW_W * 0.5f);
    mLastSignalLabel.SetRequestedHeight(26.0f);
    mLastSignalLabel.SetRequestedX(8.0f);
    mLastSignalLabel.SetRequestedY(28.0f);
    mLastSignalLabel.SetTextColor(Color::WHITE);
    panel.Add(mLastSignalLabel);

    mScrollPosLabel = Label::New("ScrollPos: (0, 0)");
    mScrollPosLabel.SetRequestedWidth(WINDOW_W * 0.5f);
    mScrollPosLabel.SetRequestedHeight(26.0f);
    mScrollPosLabel.SetRequestedX(WINDOW_W * 0.5f);
    mScrollPosLabel.SetRequestedY(28.0f);
    mScrollPosLabel.SetTextColor(Color::WHITE);
    panel.Add(mScrollPosLabel);

    mDragDeltaLabel = Label::New("Delta: (0.0, 0.0)");
    mDragDeltaLabel.SetRequestedWidth(WINDOW_W * 0.5f);
    mDragDeltaLabel.SetRequestedHeight(26.0f);
    mDragDeltaLabel.SetRequestedX(8.0f);
    mDragDeltaLabel.SetRequestedY(56.0f);
    mDragDeltaLabel.SetTextColor(Color::WHITE);
    panel.Add(mDragDeltaLabel);

    mFlingVelocityLabel = Label::New("FlingStart: (0, 0)");
    mFlingVelocityLabel.SetRequestedWidth(WINDOW_W * 0.5f);
    mFlingVelocityLabel.SetRequestedHeight(26.0f);
    mFlingVelocityLabel.SetRequestedX(WINDOW_W * 0.5f);
    mFlingVelocityLabel.SetRequestedY(56.0f);
    mFlingVelocityLabel.SetTextColor(Color::WHITE);
    panel.Add(mFlingVelocityLabel);

    mEdgeEffectLabel = Label::New("EdgeEffect: OFF  ( E to toggle )");
    mEdgeEffectLabel.SetRequestedWidth(WINDOW_W - 8.0f);
    mEdgeEffectLabel.SetRequestedHeight(26.0f);
    mEdgeEffectLabel.SetRequestedX(8.0f);
    mEdgeEffectLabel.SetRequestedY(84.0f);
    mEdgeEffectLabel.SetTextColor(Color::WHITE);
    panel.Add(mEdgeEffectLabel);

    window.Add(panel);
  }

  // ── Section 3: Focus Scroll ─────────────────────────────────────────────────

  void BuildFocusPanel(Window& window)
  {
    static constexpr float PANEL_Y = OBSERVER_H + SIGNAL_LOG_H;

    View panel = View::New();
    panel.SetBackgroundColor(COLOR_FOCUS_BG);
    panel.SetRequestedWidth(WINDOW_W);
    panel.SetRequestedHeight(FOCUS_PANEL_H);
    panel.SetRequestedX(0.0f);
    panel.SetRequestedY(PANEL_Y);

    Label title = Label::New("Focus Scroll");
    title.SetRequestedWidth(WINDOW_W);
    title.SetRequestedHeight(26.0f);
    title.SetRequestedX(0.0f);
    title.SetRequestedY(4.0f);
    title.SetTextColor(Color::WHITE);
    panel.Add(title);

    // Row 1: ScrollOnFocus toggle chip + Focus-Last button + Focused label
    static constexpr float CHIP_H        = 34.0f;
    static constexpr float CHIP_W        = 160.0f;
    static constexpr float FOCUS_LAST_W  = 150.0f;
    static constexpr float ROW1_Y        = 34.0f;

    mScrollOnFocusChip = View::New();
    mScrollOnFocusChip.SetBackgroundColor(COLOR_ACTIVE);
    mScrollOnFocusChip.SetRequestedWidth(CHIP_W);
    mScrollOnFocusChip.SetRequestedHeight(CHIP_H);
    mScrollOnFocusChip.SetRequestedX(8.0f);
    mScrollOnFocusChip.SetRequestedY(ROW1_Y);
    mScrollOnFocusChip.SetProperty(View::Property::CORNER_RADIUS, Vector4(6, 6, 6, 6));
    Label scrollOnFocusLabel = Label::New("ScrollOnFocus: ON");
    scrollOnFocusLabel.SetRequestedWidth(CHIP_W);
    scrollOnFocusLabel.SetRequestedHeight(CHIP_H);
    scrollOnFocusLabel.SetTextColor(Color::WHITE);
    mScrollOnFocusChip.Add(scrollOnFocusLabel);
    panel.Add(mScrollOnFocusChip);

    // Button: programmatically move focus to Item 15 (last) to test scroll-on-focus
    mFocusLastChip = View::New();
    mFocusLastChip.SetBackgroundColor(Vector4(0.55f, 0.25f, 0.75f, 1.0f));
    mFocusLastChip.SetRequestedWidth(FOCUS_LAST_W);
    mFocusLastChip.SetRequestedHeight(CHIP_H);
    mFocusLastChip.SetRequestedX(CHIP_W + 16.0f);
    mFocusLastChip.SetRequestedY(ROW1_Y);
    mFocusLastChip.SetProperty(View::Property::CORNER_RADIUS, Vector4(6, 6, 6, 6));
    Label focusLastLabel = Label::New("Focus Last (F)");
    focusLastLabel.SetRequestedWidth(FOCUS_LAST_W);
    focusLastLabel.SetRequestedHeight(CHIP_H);
    focusLastLabel.SetTextColor(Color::WHITE);
    mFocusLastChip.Add(focusLastLabel);
    mFocusLastChip.TouchEventSignal().Connect(this, &ScrollViewController::OnFocusLastChipTouched);
    panel.Add(mFocusLastChip);

    mFocusedLabel = Label::New("Focused: —");
    mFocusedLabel.SetRequestedWidth(WINDOW_W - CHIP_W - FOCUS_LAST_W - 32.0f);
    mFocusedLabel.SetRequestedHeight(CHIP_H);
    mFocusedLabel.SetRequestedX(CHIP_W + FOCUS_LAST_W + 24.0f);
    mFocusedLabel.SetRequestedY(ROW1_Y);
    mFocusedLabel.SetTextColor(Color::WHITE);
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
      mModeChips[i] = View::New();
      mModeChips[i].SetBackgroundColor(i == 0 ? COLOR_MODE_ON : COLOR_INACTIVE);
      mModeChips[i].SetRequestedWidth(MODE_CHIP_W);
      mModeChips[i].SetRequestedHeight(CHIP_H);
      mModeChips[i].SetRequestedX(x);
      mModeChips[i].SetRequestedY(ROW2_Y);
      mModeChips[i].SetProperty(View::Property::CORNER_RADIUS, Vector4(6, 6, 6, 6));
      Label modeLabel = Label::New(MODE_NAMES[i]);
      modeLabel.SetRequestedWidth(MODE_CHIP_W);
      modeLabel.SetRequestedHeight(CHIP_H);
      modeLabel.SetTextColor(Color::WHITE);
      mModeChips[i].Add(modeLabel);
      mModeChips[i].TouchEventSignal().Connect(this, &ScrollViewController::OnModeChipTouched);
      panel.Add(mModeChips[i]);
    }

    // Row 3: Peek label + -/+ buttons
    static constexpr float ROW3_Y   = ROW2_Y + CHIP_H + 8.0f;

    Label peekTitle = Label::New("Peek:");
    peekTitle.SetRequestedWidth(52.0f);
    peekTitle.SetRequestedHeight(CHIP_H);
    peekTitle.SetRequestedX(8.0f);
    peekTitle.SetRequestedY(ROW3_Y);
    peekTitle.SetTextColor(Color::WHITE);
    panel.Add(peekTitle);

    mPeekLabel = Label::New("0 px");
    mPeekLabel.SetRequestedWidth(80.0f);
    mPeekLabel.SetRequestedHeight(CHIP_H);
    mPeekLabel.SetRequestedX(64.0f);
    mPeekLabel.SetRequestedY(ROW3_Y);
    mPeekLabel.SetTextColor(Color::WHITE);
    panel.Add(mPeekLabel);

    Label peekHint = Label::New("( +/- keys to adjust )");
    peekHint.SetRequestedWidth(WINDOW_W - 64.0f - 80.0f - 8.0f);
    peekHint.SetRequestedHeight(CHIP_H);
    peekHint.SetRequestedX(64.0f + 80.0f + 8.0f);
    peekHint.SetRequestedY(ROW3_Y);
    peekHint.SetTextColor(Color::WHITE);
    panel.Add(peekHint);

    // Row 4: KeyScroll enable chip + step label
    static constexpr float ROW4_Y = ROW3_Y + CHIP_H + 8.0f;

    mKeyScrollChip = View::New();
    mKeyScrollChip.SetBackgroundColor(COLOR_INACTIVE);
    mKeyScrollChip.SetRequestedWidth(CHIP_W);
    mKeyScrollChip.SetRequestedHeight(CHIP_H);
    mKeyScrollChip.SetRequestedX(8.0f);
    mKeyScrollChip.SetRequestedY(ROW4_Y);
    mKeyScrollChip.SetProperty(View::Property::CORNER_RADIUS, Vector4(6, 6, 6, 6));
    Label keyScrollLabel = Label::New("KeyScroll: OFF");
    keyScrollLabel.SetRequestedWidth(CHIP_W);
    keyScrollLabel.SetRequestedHeight(CHIP_H);
    keyScrollLabel.SetTextColor(Color::WHITE);
    mKeyScrollChip.Add(keyScrollLabel);
    mKeyScrollChip.TouchEventSignal().Connect(this, &ScrollViewController::OnKeyScrollChipTouched);
    panel.Add(mKeyScrollChip);

    mKeyScrollStepLabel = Label::New("Step: 150 px");
    mKeyScrollStepLabel.SetRequestedWidth(WINDOW_W - CHIP_W - 24.0f);
    mKeyScrollStepLabel.SetRequestedHeight(CHIP_H);
    mKeyScrollStepLabel.SetRequestedX(CHIP_W + 16.0f);
    mKeyScrollStepLabel.SetRequestedY(ROW4_Y);
    mKeyScrollStepLabel.SetTextColor(Color::WHITE);
    panel.Add(mKeyScrollStepLabel);

    // Key hint row
    Label keyHint = Label::New("Keys: ↑↓ focus  F focus-last  S scroll-on-focus  1~4 mode  +/- peek  K key-scroll  [/] step  E edge-effect");
    keyHint.SetRequestedWidth(WINDOW_W - 8.0f);
    keyHint.SetRequestedHeight(22.0f);
    keyHint.SetRequestedX(8.0f);
    keyHint.SetRequestedY(ROW4_Y + CHIP_H + 2.0f);
    keyHint.SetTextColor(Color::WHITE);
    panel.Add(keyHint);

    window.Add(panel);
  }

  void RefreshFocusPanel()
  {
    // ScrollOnFocus chip
    bool on = mScrollView.GetScrollOnFocus();
    mScrollOnFocusChip.SetBackgroundColor(on ? COLOR_ACTIVE : COLOR_INACTIVE);
    std::ostringstream sv;
    sv << "ScrollOnFocus: " << (on ? "ON" : "OFF");
    Label::DownCast(mScrollOnFocusChip.GetChildViewAt(0)).SetText(sv.str().c_str());

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

    // KeyScroll chip
    bool ks = mScrollView.IsKeyScrollEnabled();
    mKeyScrollChip.SetBackgroundColor(ks ? COLOR_ACTIVE : COLOR_INACTIVE);
    std::ostringstream kl;
    kl << "KeyScroll: " << (ks ? "ON" : "OFF");
    Label::DownCast(mKeyScrollChip.GetChildViewAt(0)).SetText(kl.str().c_str());

    // Key scroll step label
    std::ostringstream sl;
    sl << "Step: " << static_cast<int>(mScrollView.GetKeyScrollStep()) << " px";
    mKeyScrollStepLabel.SetText(sl.str().c_str());
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
    mScrollView.TouchEventSignal().Connect(this,        &ScrollViewController::OnSVTouched);
  }

  bool OnSVTouched(Actor /*actor*/, TouchEvent touch)
  {
    static const char* STATE_NAMES[] = {"DOWN", "UP", "MOTION", "LEAVE", "STATIONARY", "INTERRUPTED"};
    PointState::Type   state         = touch.GetState(0);
    int                idx           = static_cast<int>(state);
    const char*        name          = (idx >= 0 && idx < 6) ? STATE_NAMES[idx] : "UNKNOWN";
    printf("[SV Touch] state=%s pos=(%.1f, %.1f)\n",
           name,
           touch.GetScreenPosition(0).x,
           touch.GetScreenPosition(0).y);
    fflush(stdout);
    return false;
  }

  void OnSVScrollStarted(ScrollView sv)
  {
    SetLastSignal("ScrollStarted");
    UpdateScrollPos(sv);
    mIsScrolling = true;
    RefreshObserverPanel();
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
    mIsScrolling = false;
    RefreshObserverPanel();
  }

  void OnSVDragStarted(ScrollView sv)
  {
    SetLastSignal("DragStarted");
    UpdateScrollPos(sv);
    mDragDeltaLabel.SetText("Delta: (0.0, 0.0)");
    mIsDragging = true;
    RefreshObserverPanel();
  }

  void OnSVDragging(ScrollView sv, float deltaX, float deltaY)
  {
    SetLastSignal("Dragging");
    UpdateScrollPos(sv);
    std::ostringstream oss;
    oss << "Delta: (" << deltaX << ", " << deltaY << ")";
    mDragDeltaLabel.SetText(oss.str().c_str());
    if(mEdgeEffectEnabled) RefreshEdgeEffectLabel();
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

    mIsDragging = false;
    RefreshObserverPanel();
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
      // Insert a non-focusable gap view between item groups.
      // The gap (GAP_H = 320px) is larger than KEY_SCROLL_STEP (150px), so
      // pressing ↓ from the last item in a group triggers step-scroll instead
      // of jumping focus directly to the first item of the next group.
      if(i > 0 && i % ITEM_GROUP == 0)
      {
        // Determine which gap number this is (1-based) for the label
        int gapNum = i / ITEM_GROUP;
        std::ostringstream gapOss;
        gapOss << "── gap " << gapNum << " (" << static_cast<int>(GAP_H) << "px) ──"
               << "  Key-scroll steps across here  ──";
        Label gapLabel = Label::New(gapOss.str().c_str());
        gapLabel.SetRequestedWidth(MATCH_PARENT);
        gapLabel.SetRequestedHeight(GAP_H * 0.3f);
        gapLabel.SetRequestedY(GAP_H * 0.35f);
        View gap = View::New();
        gap.SetBackgroundColor(Vector4(0.75f, 0.78f, 0.82f, 1.0f));
        gap.SetRequestedWidth(MATCH_PARENT);
        gap.SetRequestedHeight(GAP_H);
        gap.SetProperty(View::Property::CORNER_RADIUS, Vector4(8, 8, 8, 8));
        gap.SetProperty(Actor::Property::FOCUSABLE, false);
        gap.Add(gapLabel);
        content.Add(gap);
      }

      View item = View::New();
      item.SetBackgroundColor(i % 2 == 0 ? COLOR_ITEM_EVEN : COLOR_ITEM_ODD);
      item.SetRequestedWidth(MATCH_PARENT);
      item.SetRequestedHeight(ITEM_H);
      item.SetProperty(View::Property::CORNER_RADIUS, Vector4(10, 10, 10, 10));
      item.SetProperty(Actor::Property::FOCUSABLE, true);

      // Item number label (large, centered vertically)
      std::ostringstream nameOss;
      nameOss << "Item " << (i + 1);
      if(i == ITEM_GROUP - 1 || i == 2 * ITEM_GROUP - 1)
        nameOss << "  ← last in group";
      else if(i == ITEM_GROUP || i == 2 * ITEM_GROUP)
        nameOss << "  ← first in group";
      Label nameLabel = Label::New(nameOss.str().c_str());
      nameLabel.SetRequestedWidth(WINDOW_W - 2 * CONTENT_PAD);
      nameLabel.SetRequestedHeight(ITEM_H * 0.6f);
      nameLabel.SetRequestedX(0.0f);
      nameLabel.SetRequestedY(ITEM_H * 0.1f);
      item.Add(nameLabel);

      // Hint label (small, bottom)
      std::ostringstream hintOss;
      hintOss << "index " << i;
      if(i % ITEM_GROUP == ITEM_GROUP - 1 && i != ITEM_COUNT - 1)
        hintOss << "  [ ↓ triggers key-scroll across gap ]";
      else if(i % ITEM_GROUP == 0 && i != 0)
        hintOss << "  [ ↑ triggers key-scroll across gap ]";
      Label hintLabel = Label::New(hintOss.str().c_str());
      hintLabel.SetRequestedWidth(WINDOW_W - 2 * CONTENT_PAD);
      hintLabel.SetRequestedHeight(ITEM_H * 0.3f);
      hintLabel.SetRequestedX(0.0f);
      hintLabel.SetRequestedY(ITEM_H * 0.65f);
      item.Add(hintLabel);

      mFocusItems[i]      = item;
      mFocusItemNames[i]  = nameLabel;
      content.Add(item);
    }

    mScrollView = ScrollView::New();
    mScrollView.SetScrollDirection(ScrollDirection::Vertical);
    mScrollView.SetMaxFlingDistance(6000.0f);
    mScrollView.SetMinimumFlingDuration(200);
    mScrollView.SetMaximumFlingDuration(400);
    mScrollView.SetFlingSensitivity(1.0f);
    mScrollView.SetDecelerationRate(0.998f);
    mScrollView.SetOverScrollMode(OverScrollMode::ContentScrolls);
    mScrollView.SetScrollOnFocus(true);
    mScrollView.SetFocusScrollToPosition(ScrollToPosition::MakeVisible);
    mScrollView.SetFocusScrollPeek(FOCUS_SCROLL_PEEK);
    mScrollView.SetKeyScrollEnabled(true);
    mScrollView.SetKeyScrollStep(KEY_SCROLL_STEP);
    mScrollView.SetRequestedWidth(WINDOW_W);
    mScrollView.SetRequestedHeight(SCROLL_VIEW_H);
    mScrollView.SetRequestedX(0.0f);
    mScrollView.SetRequestedY(SCROLL_Y);
    mScrollView.SetBackgroundColor(Vector4(0.9f, 0.9f, 0.9f, 1.0f));
    mScrollView.SetContent(content);

    // Create bounce edge effects. Source is set automatically to the ScrollView's
    // content by SetStartEdgeEffect / SetEndEdgeEffect — do not call SetSource here.
    mTopEdgeEffect = BounceEdgeEffect::New(ScrollDirection::Vertical);
    mTopEdgeEffect.SetPullResistance(0.35f);
    mTopEdgeEffect.SetBounceDuration(0.40f);

    mBottomEdgeEffect = BounceEdgeEffect::New(ScrollDirection::Vertical);
    mBottomEdgeEffect.SetPullResistance(0.35f);
    mBottomEdgeEffect.SetBounceDuration(0.40f);

    // Connect Finished signals to refresh the status label
    mTopEdgeEffect.FinishedSignal().Connect(this, &ScrollViewController::OnEdgeEffectFinished);
    mBottomEdgeEffect.FinishedSignal().Connect(this, &ScrollViewController::OnEdgeEffectFinished);

    // Attach to ScrollView (edge effects are initially disabled until user presses E)
    RefreshEdgeEffects();

    window.Add(mScrollView);
  }

  // ── Focus Last chip touch ─────────────────────────────────────────────────

  bool OnFocusLastChipTouched(Actor /*actor*/, TouchEvent touch)
  {
    if(touch.GetState(0) != PointState::UP) return false;
    FocusManager::Get().SetCurrentFocusView(mFocusItems[ITEM_COUNT - 1]);
    return true;
  }

  // ── KeyScroll chip touch ───────────────────────────────────────────────────

  bool OnKeyScrollChipTouched(Actor /*actor*/, TouchEvent touch)
  {
    if(touch.GetState(0) != PointState::UP)
    {
      return false;
    }
    mScrollView.SetKeyScrollEnabled(!mScrollView.IsKeyScrollEnabled());
    RefreshFocusPanel();
    return true;
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
    if(from == to) return; // key-scroll returns same view to block FocusFinder
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
      // FocusManager handles MoveFocus(UP) internally via its own KeyEventSignal connection.
      // Only handle the "no focus yet" case to seed focus to the last item.
      if(mCurrentFocusIndex < 0)
      {
        FocusManager::Get().SetCurrentFocusView(mFocusItems[ITEM_COUNT - 1]);
      }
    }
    else if(key == "Down")
    {
      // FocusManager handles MoveFocus(DOWN) internally.
      // Only seed focus to the first item if nothing is focused yet.
      if(mCurrentFocusIndex < 0)
      {
        FocusManager::Get().SetCurrentFocusView(mFocusItems[0]);
      }
    }
    else if(key == "f" || key == "F")
    {
      FocusManager::Get().SetCurrentFocusView(mFocusItems[ITEM_COUNT - 1]);
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
    else if(key == "k" || key == "K")
    {
      mScrollView.SetKeyScrollEnabled(!mScrollView.IsKeyScrollEnabled());
      RefreshFocusPanel();
    }
    else if(key == "bracketleft")
    {
      float step = mScrollView.GetKeyScrollStep() - 20.0f;
      if(step < 20.0f) step = 20.0f;
      mScrollView.SetKeyScrollStep(step);
      RefreshFocusPanel();
    }
    else if(key == "bracketright")
    {
      mScrollView.SetKeyScrollStep(mScrollView.GetKeyScrollStep() + 20.0f);
      RefreshFocusPanel();
    }
    else if(key == "e" || key == "E")
    {
      mEdgeEffectEnabled = !mEdgeEffectEnabled;
      RefreshEdgeEffects();
    }
  }

  // ── Edge Effects ───────────────────────────────────────────────────────────

  void RefreshEdgeEffects()
  {
    if(mEdgeEffectEnabled)
    {
      mScrollView.SetStartEdgeEffect(mTopEdgeEffect);
      mScrollView.SetEndEdgeEffect(mBottomEdgeEffect);
    }
    else
    {
      mScrollView.SetStartEdgeEffect(EdgeEffect());
      mScrollView.SetEndEdgeEffect(EdgeEffect());
    }
    RefreshEdgeEffectLabel();
  }

  void RefreshEdgeEffectLabel()
  {
    std::ostringstream oss;
    if(!mEdgeEffectEnabled)
    {
      oss << "EdgeEffect: OFF  ( E to enable )";
    }
    else
    {
      auto stateStr = [](EdgeEffect::State s) -> const char* {
        switch(s)
        {
          case EdgeEffect::State::PULL:     return "PULL";
          case EdgeEffect::State::ABSORB:   return "ABSORB";
          case EdgeEffect::State::RECEDE:   return "RECEDE";
          case EdgeEffect::State::PULLDECAY:return "PULLDECAY";
          default:                          return "IDLE";
        }
      };
      oss << "EdgeEffect: ON  Top=" << stateStr(mTopEdgeEffect.GetState())
          << "  Bottom=" << stateStr(mBottomEdgeEffect.GetState())
          << "  ( E to disable )";
    }
    mEdgeEffectLabel.SetText(oss.str().c_str());
  }

  void OnEdgeEffectFinished()
  {
    RefreshEdgeEffectLabel();
  }

  // ── Members ────────────────────────────────────────────────────────────────

  Application& mApplication;
  ScrollView   mScrollView;

  // Observer panel
  View  mObserverChips[3];
  Label mObserverValues[3];
  bool  mIsDragging{false};
  bool  mIsScrolling{false};

  // Signal log panel
  Label mLastSignalLabel;
  Label mScrollPosLabel;
  Label mDragDeltaLabel;
  Label mFlingVelocityLabel;

  // Focus scroll panel
  View  mScrollOnFocusChip;
  View  mFocusLastChip;
  View  mModeChips[4];
  Label mFocusedLabel;
  Label mPeekLabel;
  View  mKeyScrollChip;
  Label mKeyScrollStepLabel;

  // Focusable items
  View  mFocusItems[ITEM_COUNT];
  Label mFocusItemNames[ITEM_COUNT];
  int   mCurrentFocusIndex{-1};

  // Edge effects
  BounceEdgeEffect mTopEdgeEffect;
  BounceEdgeEffect mBottomEdgeEffect;
  Label                             mEdgeEffectLabel;
  bool                              mEdgeEffectEnabled{false};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  ScrollViewController test(application);
  application.MainLoop();
  return 0;
}
