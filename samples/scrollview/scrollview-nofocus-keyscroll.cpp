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
 * scrollview-nofocus-keyscroll.cpp
 *
 * Verifies key-scroll when content has no focusable items.
 *
 * Fix: OnFocusNavigationRequested() now has a special path for when the
 * ScrollView itself (Self()) is the current focused view:
 *
 *   if(currentFocusedView == View::DownCast(Self()))
 *   {
 *     if(!IsAtScrollBoundary(direction)) { ScrollByKeyDirection(); return Self(); }
 *     TriggerKeyEdgeFeedback(); return View();
 *   }
 *
 * The app opts in by setting KEYBOARD_FOCUSABLE=true on the ScrollView and
 * seeding focus to it.
 *
 * Layout:
 *   [0,   0] 600x180  Info panel
 *   [0, 180] 600x900  ScrollView — 20 non-focusable items (ScrollView itself is focusable)
 *
 * Keyboard:
 *   ↑ / ↓   key-scroll fires (scroll position changes)
 *   K       toggle KeyScrollEnabled
 *   ESC     quit
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>
#include <sstream>

using namespace Dali;
using namespace Dali::Ui;

static constexpr float WINDOW_W   = 600.0f;
static constexpr float WINDOW_H   = 1080.0f;
static constexpr float INFO_H     = 180.0f;
static constexpr float SCROLL_Y   = INFO_H;
static constexpr float SCROLL_H   = WINDOW_H - SCROLL_Y;

static constexpr int   ITEM_COUNT = 20;
static constexpr float ITEM_H     = 140.0f;
static constexpr float ITEM_GAP   = 8.0f;

static const Vector4 COLOR_BG    (0.10f, 0.10f, 0.14f, 1.0f);
static const Vector4 COLOR_ACTIVE(0.20f, 0.75f, 0.40f, 1.0f);
static const Vector4 COLOR_INACT (0.40f, 0.40f, 0.40f, 1.0f);
static const Vector4 ITEM_COLORS[2] = {
  Vector4(0.82f, 0.87f, 0.95f, 1.0f),
  Vector4(0.88f, 0.93f, 0.88f, 1.0f),
};

class NoFocusKeyScrollTest : public ConnectionTracker
{
public:
  explicit NoFocusKeyScrollTest(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &NoFocusKeyScrollTest::Create);
  }

private:
  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetPositionSize(PositionSize(0, 0,
      static_cast<uint32_t>(WINDOW_W), static_cast<uint32_t>(WINDOW_H)));
    window.SetBackgroundColor(Color::BLACK);
    window.KeyEventSignal().Connect(this, &NoFocusKeyScrollTest::OnKeyEvent);

    BuildInfoPanel(window);
    BuildScrollView(window);

    mScrollView.ScrollingSignal().Connect(this, &NoFocusKeyScrollTest::OnScrolling);
    FocusManager::Get().FocusChangedSignal().Connect(this, &NoFocusKeyScrollTest::OnFocusChanged);

    // Seed focus to the ScrollView itself — activates Self() key-scroll path
    FocusManager::Get().SetCurrentFocusView(mScrollView);

    RefreshInfo();
  }

  void BuildInfoPanel(Window& window)
  {
    View panel = View::New();
    panel.SetBackgroundColor(COLOR_BG);
    panel.SetRequestedWidth(WINDOW_W);
    panel.SetRequestedHeight(INFO_H);
    panel.SetRequestedPositionX(0.0f);
    panel.SetRequestedPositionY(0.0f);

    Label title = Label::New("Test: Key-scroll with no focusable items in content");
    title.SetRequestedWidth(WINDOW_W - 8.0f);
    title.SetRequestedHeight(28.0f);
    title.SetRequestedPositionX(8.0f);
    title.SetRequestedPositionY(4.0f);
    title.SetTextColor(Color::WHITE);
    panel.Add(title);

    mKeyScrollChip = View::New();
    mKeyScrollChip.SetBackgroundColor(COLOR_ACTIVE);
    mKeyScrollChip.SetRequestedWidth(180.0f);
    mKeyScrollChip.SetRequestedHeight(32.0f);
    mKeyScrollChip.SetRequestedPositionX(8.0f);
    mKeyScrollChip.SetRequestedPositionY(36.0f);
    mKeyScrollChip.SetProperty(View::Property::CORNER_RADIUS, Vector4(6, 6, 6, 6));
    mKeyScrollChip.TouchEventSignal().Connect(this, &NoFocusKeyScrollTest::OnKeyScrollChipTouched);
    Label ksLabel = Label::New("KeyScroll: ON");
    ksLabel.SetRequestedWidth(180.0f);
    ksLabel.SetRequestedHeight(32.0f);
    ksLabel.SetTextColor(Color::WHITE);
    mKeyScrollChip.Add(ksLabel);
    panel.Add(mKeyScrollChip);

    mScrollPosLabel = Label::New("ScrollPos: 0");
    mScrollPosLabel.SetRequestedWidth(WINDOW_W - 196.0f);
    mScrollPosLabel.SetRequestedHeight(32.0f);
    mScrollPosLabel.SetRequestedPositionX(196.0f);
    mScrollPosLabel.SetRequestedPositionY(36.0f);
    mScrollPosLabel.SetTextColor(Color::WHITE);
    panel.Add(mScrollPosLabel);

    mArrowCountLabel = Label::New("↑↓ key presses: 0   (scroll pos unchanged = key-scroll not firing)");
    mArrowCountLabel.SetRequestedWidth(WINDOW_W - 8.0f);
    mArrowCountLabel.SetRequestedHeight(32.0f);
    mArrowCountLabel.SetRequestedPositionX(8.0f);
    mArrowCountLabel.SetRequestedPositionY(72.0f);
    mArrowCountLabel.SetTextColor(Color::WHITE);
    panel.Add(mArrowCountLabel);

    mFocusLabel = Label::New("Focused: —  (no focusable items in content)");
    mFocusLabel.SetRequestedWidth(WINDOW_W - 8.0f);
    mFocusLabel.SetRequestedHeight(32.0f);
    mFocusLabel.SetRequestedPositionX(8.0f);
    mFocusLabel.SetRequestedPositionY(108.0f);
    mFocusLabel.SetTextColor(Color::WHITE);
    panel.Add(mFocusLabel);

    Label hint = Label::New("ScrollView is focusable + focused. Press ↑↓ — scroll pos should change. K toggles KeyScroll.");
    hint.SetRequestedWidth(WINDOW_W - 8.0f);
    hint.SetRequestedHeight(30.0f);
    hint.SetRequestedPositionX(8.0f);
    hint.SetRequestedPositionY(144.0f);
    hint.SetTextColor(Vector4(1.0f, 0.9f, 0.2f, 1.0f));
    panel.Add(hint);

    window.Add(panel);
  }

  void BuildScrollView(Window& window)
  {
    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(WINDOW_W);
    content.SetSpacing(ITEM_GAP);

    for(int i = 0; i < ITEM_COUNT; ++i)
    {
      View item = View::New();
      item.SetBackgroundColor(ITEM_COLORS[i % 2]);
      item.SetRequestedWidth(MATCH_PARENT);
      item.SetRequestedHeight(ITEM_H);
      item.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, false);
      item.SetProperty(View::Property::CORNER_RADIUS, Vector4(8, 8, 8, 8));

      std::ostringstream oss;
      oss << "Item " << (i + 1) << "  [KEYBOARD_FOCUSABLE = false]";
      Label lbl = Label::New(oss.str().c_str());
      lbl.SetRequestedWidth(MATCH_PARENT);
      lbl.SetRequestedHeight(ITEM_H);
      item.Add(lbl);

      content.Add(item);
    }

    mScrollView = ScrollView::New();
    mScrollView.SetScrollDirection(ScrollDirection::Vertical);
    mScrollView.SetOverScrollMode(OverScrollMode::ContentScrolls);
    mScrollView.SetKeyScrollEnabled(true);
    mScrollView.SetKeyScrollStep(150.0f);
    mScrollView.SetRequestedWidth(WINDOW_W);
    mScrollView.SetRequestedHeight(SCROLL_H);
    mScrollView.SetRequestedPositionX(0.0f);
    mScrollView.SetRequestedPositionY(SCROLL_Y);
    mScrollView.SetBackgroundColor(Vector4(0.9f, 0.9f, 0.9f, 1.0f));
    mScrollView.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true); // opt-in for Self() key-scroll
    mScrollView.SetContent(content);

    window.Add(mScrollView);
  }

  void RefreshInfo()
  {
    bool ks = mScrollView.IsKeyScrollEnabled();
    mKeyScrollChip.SetBackgroundColor(ks ? COLOR_ACTIVE : COLOR_INACT);
    Label::DownCast(mKeyScrollChip.GetChildViewAt(0)).SetText(ks ? "KeyScroll: ON" : "KeyScroll: OFF");

    int scrollY = static_cast<int>(mScrollView.GetScrollPosition().y);
    std::ostringstream sp;
    sp << "ScrollPos: " << scrollY;
    mScrollPosLabel.SetText(sp.str().c_str());

    std::ostringstream ac;
    ac << "↑↓ key presses: " << mArrowPressCount
       << "   scroll pos: " << scrollY
       << "   (unchanged = key-scroll not firing)";
    mArrowCountLabel.SetText(ac.str().c_str());
  }

  void OnScrolling(ScrollView sv)
  {
    // Fired by touch scroll — show that touch still works
    int scrollY = static_cast<int>(sv.GetScrollPosition().y);
    std::ostringstream sp;
    sp << "ScrollPos: " << scrollY << "  (touch scroll)";
    mScrollPosLabel.SetText(sp.str().c_str());
  }

  void OnFocusChanged(View /*from*/, View to)
  {
    if(to == mScrollView)
      mFocusLabel.SetText("Focused: ScrollView  (key-scroll active)");
    else if(to)
      mFocusLabel.SetText("Focused: unexpected view — investigate!");
    else
      mFocusLabel.SetText("Focused: —");
  }

  bool OnKeyScrollChipTouched(Actor /*actor*/, TouchEvent touch)
  {
    if(touch.GetState(0) != PointState::UP) return false;
    mScrollView.SetKeyScrollEnabled(!mScrollView.IsKeyScrollEnabled());
    RefreshInfo();
    return true;
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

    if(key == "Up" || key == "Down")
    {
      ++mArrowPressCount;
      RefreshInfo();
    }
    else if(key == "k" || key == "K")
    {
      mScrollView.SetKeyScrollEnabled(!mScrollView.IsKeyScrollEnabled());
      RefreshInfo();
    }
  }

  Application& mApplication;
  ScrollView   mScrollView;

  View  mKeyScrollChip;
  Label mScrollPosLabel;
  Label mArrowCountLabel;
  Label mFocusLabel;

  int mArrowPressCount{0};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  NoFocusKeyScrollTest test(application);
  application.MainLoop();
  return 0;
}
