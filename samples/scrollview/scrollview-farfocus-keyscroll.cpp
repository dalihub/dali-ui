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
 * scrollview-farfocus-keyscroll.cpp
 *
 * Verifies key-scroll behavior when the first page has no focusable items
 * and focusable items are far below.
 *
 * Content layout:
 *   - "Top Area" (NON-focusable, 130px)
 *   - 1500px non-focusable gap
 *   - "Bottom Item 1~5" (focusable)
 *
 * The ScrollView itself is KEYBOARD_FOCUSABLE=true and seeded with focus.
 * External focusable buttons sit above and below the ScrollView to verify
 * focus can exit in both directions.
 *
 * Expected ↓ behavior from ScrollView (Self() focused):
 *   1. OnKeyEvent fires → ScrollByKeyDirection (step 150px)
 *   2. Repeats until Bottom Item 1 is within mKeyScrollStep of viewport edge
 *   3. Focus jumps to Bottom Item 1  (OnKeyEvent sets focus explicitly)
 *   4. OnFocusNavigationRequested takes over for subsequent ↓ presses
 *   5. At scroll bottom boundary → OnKeyEvent returns false
 *      → FocusManager global traversal → focus exits to External Bottom Button
 *
 * Expected ↑ behavior from ScrollView (Self() focused, at top boundary):
 *   OnKeyEvent returns false → focus exits to External Top Button
 *
 * Layout:
 *   [0,   0] 160px  Info panel
 *   [0, 160]  50px  External Top Button  (focusable, OUTSIDE ScrollView)
 *   [0, 210] 700px  ScrollView
 *   [0, 910]  50px  External Bottom Button  (focusable, OUTSIDE ScrollView)
 *
 * Keyboard:
 *   ↑ / ↓   key-scroll / focus navigation
 *   R       reset — scroll to top, focus → ScrollView
 *   ESC     quit
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>
#include <sstream>

using namespace Dali;
using namespace Dali::Ui;

static constexpr float WINDOW_W        = 600.0f;
static constexpr float WINDOW_H        = 1080.0f;

static constexpr float INFO_H          = 160.0f;
static constexpr float EXT_BTN_H       = 50.0f;
static constexpr float EXT_TOP_Y       = INFO_H;                   // 160
static constexpr float SCROLL_Y        = EXT_TOP_Y + EXT_BTN_H;   // 210
static constexpr float SCROLL_H        = 700.0f;
static constexpr float EXT_BOT_Y       = SCROLL_Y + SCROLL_H;     // 910

static constexpr float ITEM_H          = 130.0f;
static constexpr float ITEM_SPACING    = 10.0f;
static constexpr float CONTENT_PAD     = 16.0f;
static constexpr float GAP_H           = 1500.0f;
static constexpr int   BOTTOM_COUNT    = 5;
static constexpr float KEY_SCROLL_STEP = 150.0f;

static constexpr float CONTENT_H = CONTENT_PAD * 2
                                  + ITEM_H + ITEM_SPACING
                                  + GAP_H  + ITEM_SPACING
                                  + BOTTOM_COUNT * ITEM_H
                                  + (BOTTOM_COUNT - 1) * ITEM_SPACING;

static const Vector4 COLOR_BG             (0.10f, 0.10f, 0.14f, 1.0f);
static const Vector4 COLOR_EXT_TOP        (0.80f, 0.55f, 0.15f, 1.0f);
static const Vector4 COLOR_EXT_TOP_FOCUS  (0.60f, 0.35f, 0.05f, 1.0f);
static const Vector4 COLOR_EXT_BOT        (0.55f, 0.15f, 0.80f, 1.0f);
static const Vector4 COLOR_EXT_BOT_FOCUS  (0.35f, 0.05f, 0.60f, 1.0f);
static const Vector4 COLOR_SCROLL_FOCUS   (0.20f, 0.50f, 0.80f, 1.0f);
static const Vector4 COLOR_SCROLL_IDLE    (0.90f, 0.90f, 0.90f, 1.0f);
static const Vector4 COLOR_TOP_ITEM       (0.75f, 0.78f, 0.82f, 1.0f);
static const Vector4 COLOR_BOTTOM_ITEM    (0.30f, 0.70f, 0.45f, 1.0f);
static const Vector4 COLOR_BOTTOM_FOCUS   (0.15f, 0.45f, 0.25f, 1.0f);
static const Vector4 COLOR_GAP            (0.85f, 0.87f, 0.90f, 1.0f);

class FarFocusKeyScrollTest : public ConnectionTracker
{
public:
  explicit FarFocusKeyScrollTest(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &FarFocusKeyScrollTest::Create);
  }

private:
  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetSize(Dali::Window::WindowSize(
      static_cast<uint32_t>(WINDOW_W), static_cast<uint32_t>(WINDOW_H)));
    window.SetBackgroundColor(Color::BLACK);
    window.KeyEventSignal().Connect(this, &FarFocusKeyScrollTest::OnKeyEvent);

    BuildInfoPanel(window);
    BuildExternalTopButton(window);
    BuildScrollView(window);
    BuildExternalBottomButton(window);

    mScrollView.ScrollingSignal().Connect(this, &FarFocusKeyScrollTest::OnScrolling);
    FocusManager::Get().FocusChangedSignal().Connect(this, &FarFocusKeyScrollTest::OnFocusChanged);

    FocusManager::Get().SetCurrentFocusView(mScrollView);
    RefreshInfo();
  }

  // ── Info panel ─────────────────────────────────────────────────────────────
  void BuildInfoPanel(Window& window)
  {
    View panel = View::New();
    panel.SetBackgroundColor(COLOR_BG);
    panel.SetRequestedWidth(WINDOW_W);
    panel.SetRequestedHeight(INFO_H);
    panel.SetRequestedPositionX(0.0f);
    panel.SetRequestedPositionY(0.0f);

    Label title = Label::New("Test: Key-scroll from non-focusable first page + focus exit");
    title.SetRequestedWidth(WINDOW_W - 8.0f);
    title.SetRequestedHeight(28.0f);
    title.SetRequestedPositionX(8.0f);
    title.SetRequestedPositionY(4.0f);
    title.SetTextColor(Color::WHITE);
    panel.Add(title);

    mFocusLabel = Label::New("Focused: ScrollView  (OnKeyEvent path)");
    mFocusLabel.SetRequestedWidth(WINDOW_W - 8.0f);
    mFocusLabel.SetRequestedHeight(28.0f);
    mFocusLabel.SetRequestedPositionX(8.0f);
    mFocusLabel.SetRequestedPositionY(36.0f);
    mFocusLabel.SetTextColor(Color::WHITE);
    panel.Add(mFocusLabel);

    mScrollPosLabel = Label::New("ScrollPos: 0");
    mScrollPosLabel.SetRequestedWidth(WINDOW_W - 8.0f);
    mScrollPosLabel.SetRequestedHeight(28.0f);
    mScrollPosLabel.SetRequestedPositionX(8.0f);
    mScrollPosLabel.SetRequestedPositionY(68.0f);
    mScrollPosLabel.SetTextColor(Color::WHITE);
    panel.Add(mScrollPosLabel);

    mStepLabel = Label::New("Key-scroll steps: 0  |  Focus changes: 0");
    mStepLabel.SetRequestedWidth(WINDOW_W - 8.0f);
    mStepLabel.SetRequestedHeight(28.0f);
    mStepLabel.SetRequestedPositionX(8.0f);
    mStepLabel.SetRequestedPositionY(100.0f);
    mStepLabel.SetTextColor(Color::WHITE);
    panel.Add(mStepLabel);

    Label hint = Label::New("↓ step-scrolls then jumps to Bottom Item 1. At boundary: focus exits to External Button.  R=reset");
    hint.SetRequestedWidth(WINDOW_W - 8.0f);
    hint.SetRequestedHeight(28.0f);
    hint.SetRequestedPositionX(8.0f);
    hint.SetRequestedPositionY(130.0f);
    hint.SetTextColor(Vector4(1.0f, 0.9f, 0.2f, 1.0f));
    panel.Add(hint);

    window.Add(panel);
  }

  // ── External buttons (outside ScrollView) ──────────────────────────────────
  void BuildExternalTopButton(Window& window)
  {
    mExtTopBtn = View::New();
    mExtTopBtn.SetBackgroundColor(COLOR_EXT_TOP);
    mExtTopBtn.SetRequestedWidth(WINDOW_W);
    mExtTopBtn.SetRequestedHeight(EXT_BTN_H);
    mExtTopBtn.SetRequestedPositionX(0.0f);
    mExtTopBtn.SetRequestedPositionY(EXT_TOP_Y);
    mExtTopBtn.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);

    Label lbl = Label::New("External Top Button  [focusable — OUTSIDE ScrollView]");
    lbl.SetRequestedWidth(WINDOW_W);
    lbl.SetRequestedHeight(EXT_BTN_H);
    mExtTopBtn.Add(lbl);

    window.Add(mExtTopBtn);
  }

  void BuildExternalBottomButton(Window& window)
  {
    mExtBotBtn = View::New();
    mExtBotBtn.SetBackgroundColor(COLOR_EXT_BOT);
    mExtBotBtn.SetRequestedWidth(WINDOW_W);
    mExtBotBtn.SetRequestedHeight(EXT_BTN_H);
    mExtBotBtn.SetRequestedPositionX(0.0f);
    mExtBotBtn.SetRequestedPositionY(EXT_BOT_Y);
    mExtBotBtn.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);

    Label lbl = Label::New("External Bottom Button  [focusable — OUTSIDE ScrollView]");
    lbl.SetRequestedWidth(WINDOW_W);
    lbl.SetRequestedHeight(EXT_BTN_H);
    mExtBotBtn.Add(lbl);

    window.Add(mExtBotBtn);
  }

  // ── ScrollView ──────────────────────────────────────────────────────────────
  void BuildScrollView(Window& window)
  {
    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(WINDOW_W);
    content.SetRequestedHeight(CONTENT_H);
    content.SetSpacing(ITEM_SPACING);
    content.SetPadding(Extents(
      static_cast<uint16_t>(CONTENT_PAD), static_cast<uint16_t>(CONTENT_PAD),
      static_cast<uint16_t>(CONTENT_PAD), static_cast<uint16_t>(CONTENT_PAD)));

    // Top area — NON-focusable
    View topArea = View::New();
    topArea.SetBackgroundColor(COLOR_TOP_ITEM);
    topArea.SetRequestedWidth(MATCH_PARENT);
    topArea.SetRequestedHeight(ITEM_H);
    topArea.SetProperty(View::Property::CORNER_RADIUS, Vector4(10, 10, 10, 10));
    topArea.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, false);
    Label topLbl = Label::New("Top Area  [NON-focusable]  — ScrollView itself has focus");
    topLbl.SetRequestedWidth(MATCH_PARENT);
    topLbl.SetRequestedHeight(ITEM_H);
    topArea.Add(topLbl);
    content.Add(topArea);

    // Gap — 1500px, non-focusable
    View gap = View::New();
    gap.SetBackgroundColor(COLOR_GAP);
    gap.SetRequestedWidth(MATCH_PARENT);
    gap.SetRequestedHeight(GAP_H);
    gap.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, false);
    gap.SetProperty(View::Property::CORNER_RADIUS, Vector4(10, 10, 10, 10));
    std::ostringstream gapOss;
    gapOss << "── 1500px non-focusable gap ──\n"
           << "step-scrolls ~" << static_cast<int>(GAP_H / KEY_SCROLL_STEP)
           << " times  (GAP=" << static_cast<int>(GAP_H)
           << " / STEP=" << static_cast<int>(KEY_SCROLL_STEP) << ")";
    Label gapLbl = Label::New(gapOss.str().c_str());
    gapLbl.SetRequestedWidth(MATCH_PARENT);
    gapLbl.SetRequestedHeight(GAP_H * 0.15f);
    gapLbl.SetRequestedPositionY(GAP_H * 0.42f);
    gap.Add(gapLbl);
    content.Add(gap);

    // Bottom items — focusable
    for(int i = 0; i < BOTTOM_COUNT; ++i)
    {
      View item = View::New();
      item.SetBackgroundColor(COLOR_BOTTOM_ITEM);
      item.SetRequestedWidth(MATCH_PARENT);
      item.SetRequestedHeight(ITEM_H);
      item.SetProperty(View::Property::CORNER_RADIUS, Vector4(10, 10, 10, 10));
      item.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);

      std::ostringstream oss;
      oss << "Bottom Item " << (i + 1) << "  [focusable]";
      if(i == 0) oss << "  ← focus jumps here first";
      Label lbl = Label::New(oss.str().c_str());
      lbl.SetRequestedWidth(MATCH_PARENT);
      lbl.SetRequestedHeight(ITEM_H);
      item.Add(lbl);

      mBottomItems[i] = item;
      content.Add(item);
    }

    mScrollView = ScrollView::New();
    mScrollView.SetScrollDirection(ScrollDirection::Vertical);
    mScrollView.SetOverScrollMode(OverScrollMode::ContentScrolls);
    mScrollView.SetScrollOnFocus(true);
    mScrollView.SetFocusScrollToPosition(ScrollToPosition::MakeVisible);
    mScrollView.SetKeyScrollEnabled(true);
    mScrollView.SetKeyScrollStep(KEY_SCROLL_STEP);
    mScrollView.SetRequestedWidth(WINDOW_W);
    mScrollView.SetRequestedHeight(SCROLL_H);
    mScrollView.SetRequestedPositionX(0.0f);
    mScrollView.SetRequestedPositionY(SCROLL_Y);
    mScrollView.SetBackgroundColor(COLOR_SCROLL_IDLE);
    mScrollView.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);
    mScrollView.SetContent(content);

    window.Add(mScrollView);
  }

  // ── Callbacks ───────────────────────────────────────────────────────────────
  void RefreshInfo()
  {
    int scrollY = static_cast<int>(mScrollView.GetScrollPosition().y);
    std::ostringstream sp;
    sp << "ScrollPos: " << scrollY;
    mScrollPosLabel.SetText(sp.str().c_str());

    std::ostringstream st;
    st << "Key-scroll steps: " << mStepCount
       << "  |  Focus changes: " << mFocusChangeCount;
    mStepLabel.SetText(st.str().c_str());
  }

  void OnScrolling(ScrollView /*sv*/)
  {
    ++mStepCount;
    RefreshInfo();
  }

  void OnFocusChanged(View from, View to)
  {
    if(from == to) return;

    mStepCount = 0;
    ++mFocusChangeCount;

    // Restore previous colors
    if(from == mExtTopBtn)    mExtTopBtn.SetBackgroundColor(COLOR_EXT_TOP);
    if(from == mExtBotBtn)    mExtBotBtn.SetBackgroundColor(COLOR_EXT_BOT);
    if(from == mScrollView)   mScrollView.SetBackgroundColor(COLOR_SCROLL_IDLE);
    for(int i = 0; i < BOTTOM_COUNT; ++i)
      if(from == mBottomItems[i]) mBottomItems[i].SetBackgroundColor(COLOR_BOTTOM_ITEM);

    // Apply new focus color
    if(to == mExtTopBtn)    mExtTopBtn.SetBackgroundColor(COLOR_EXT_TOP_FOCUS);
    if(to == mExtBotBtn)    mExtBotBtn.SetBackgroundColor(COLOR_EXT_BOT_FOCUS);
    if(to == mScrollView)   mScrollView.SetBackgroundColor(COLOR_SCROLL_FOCUS);
    for(int i = 0; i < BOTTOM_COUNT; ++i)
      if(to == mBottomItems[i]) mBottomItems[i].SetBackgroundColor(COLOR_BOTTOM_FOCUS);

    // Update label
    if(to == mExtTopBtn)
      mFocusLabel.SetText("Focused: External Top Button  ← focus exited ScrollView ↑");
    else if(to == mExtBotBtn)
      mFocusLabel.SetText("Focused: External Bottom Button  ← focus exited ScrollView ↓");
    else if(to == mScrollView)
      mFocusLabel.SetText("Focused: ScrollView  (OnKeyEvent path)");
    else
    {
      for(int i = 0; i < BOTTOM_COUNT; ++i)
      {
        if(to == mBottomItems[i])
        {
          std::ostringstream oss;
          oss << "Focused: Bottom Item " << (i + 1)
              << "  ← OnKeyEvent jumped focus here";
          mFocusLabel.SetText(oss.str().c_str());
          break;
        }
      }
    }

    RefreshInfo();
  }

  void ResetTest()
  {
    mScrollView.ScrollTo(Vector2(0.0f, 0.0f), false);
    mStepCount        = 0;
    mFocusChangeCount = 0;

    mExtTopBtn.SetBackgroundColor(COLOR_EXT_TOP);
    mExtBotBtn.SetBackgroundColor(COLOR_EXT_BOT);
    mScrollView.SetBackgroundColor(COLOR_SCROLL_IDLE);
    for(int i = 0; i < BOTTOM_COUNT; ++i)
      mBottomItems[i].SetBackgroundColor(COLOR_BOTTOM_ITEM);

    FocusManager::Get().SetCurrentFocusView(mScrollView);
    mFocusLabel.SetText("Focused: ScrollView  (reset)");
    RefreshInfo();
  }

  void OnKeyEvent(Window /*window*/, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::DOWN) return;
    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
      return;
    }
    const Dali::String& key = event.GetKeyName();
    if(key == "r" || key == "R") ResetTest();
  }

  Application& mApplication;
  ScrollView   mScrollView;

  View mExtTopBtn;
  View mExtBotBtn;
  View mBottomItems[BOTTOM_COUNT];

  Label mFocusLabel;
  Label mScrollPosLabel;
  Label mStepLabel;

  int mStepCount{0};
  int mFocusChangeCount{0};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  FarFocusKeyScrollTest test(application);
  application.MainLoop();
  return 0;
}
