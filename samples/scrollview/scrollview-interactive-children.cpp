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
 * scrollview-interactive-children.cpp
 *
 * Demonstrates that ScrollView correctly scrolls even when child items
 * each consume touch via ClickableTrait.
 *
 * Layout (explicit pixel sizes, same pattern as scrollview-example.cpp)
 * ──────
 *   Window (480 x 800)
 *   ├── Status bar  View  (0,   0)  480 x 60
 *   └── ScrollView        (0,  60)  480 x 740
 *       └── Content StackLayout     480 x (ITEM_COUNT * (HEIGHT+SPACING) + SPACING)
 *           ├── Item 0  (clickable)
 *           ├── Item 1  (clickable)
 *           └── …
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <sstream>

using namespace Dali;
using namespace Dali::Ui;

// ─── tunables ────────────────────────────────────────────────────────────────
static constexpr int   ITEM_COUNT        = 20;
static constexpr float ITEM_HEIGHT       = 80.0f;
static constexpr float ITEM_SPACING      = 8.0f;
static constexpr float STATUS_BAR_HEIGHT = 60.0f;
static constexpr float WINDOW_WIDTH      = 480.0f;
static constexpr float WINDOW_HEIGHT     = 800.0f;

static const Vector4 COLOR_ITEM_A(0.85f, 0.92f, 1.00f, 1.0f);
static const Vector4 COLOR_ITEM_B(0.70f, 0.82f, 0.95f, 1.0f);
static const Vector4 COLOR_PRESSED(0.40f, 0.65f, 1.00f, 1.0f);
static const Vector4 COLOR_STATUS_BG(0.15f, 0.15f, 0.15f, 1.0f);
// ─────────────────────────────────────────────────────────────────────────────

// Helper: log a view's size and visibility
static void LogViewInfo(const char* tag, View view)
{
  float w      = view.GetProperty<float>(Actor::Property::SIZE_WIDTH);
  float h      = view.GetProperty<float>(Actor::Property::SIZE_HEIGHT);
  float lx     = view.GetProperty<float>(Actor::Property::POSITION_X);
  float ly     = view.GetProperty<float>(Actor::Property::POSITION_Y);
  float wx     = view.GetProperty<float>(Actor::Property::WORLD_POSITION_X);
  float wy     = view.GetProperty<float>(Actor::Property::WORLD_POSITION_Y);
  bool  vis    = view.GetProperty<bool>(Actor::Property::VISIBLE);
  Vector2 scr  = view.GetProperty<Vector2>(Actor::Property::SCREEN_POSITION);
  int clip     = view.GetProperty<int>(Actor::Property::CLIPPING_MODE);
  DALI_LOG_RELEASE_INFO("[ScrollInteractiveSample][Layout] %-24s  size=%5.0fx%-5.0f  local=(%5.0f,%5.0f)  world=(%6.0f,%6.0f)  screen=(%5.0f,%5.0f)  vis=%s  clip=%d\n",
                        tag, w, h, lx, ly, wx, wy, scr.x, scr.y, vis ? "Y" : "N", clip);
}

class ScrollInteractiveChildrenController : public ConnectionTracker
{
public:
  explicit ScrollInteractiveChildrenController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &ScrollInteractiveChildrenController::Create);
  }

  ~ScrollInteractiveChildrenController() = default;

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);
    window.KeyEventSignal().Connect(this, &ScrollInteractiveChildrenController::OnKeyEvent);

    // ── Status bar (placed at top, explicit size) ─────────────────────────────
    mStatusBar = View::New()
      .SetBackgroundColor(COLOR_STATUS_BG)
      .SetRequestedWidth(WINDOW_WIDTH)
      .SetRequestedHeight(STATUS_BAR_HEIGHT)
      .SetRequestedPositionX(0.0f)
      .SetRequestedPositionY(0.0f);

    mStatusLabel = Label::New("Tap an item or scroll the list")
      .SetRequestedWidth(WINDOW_WIDTH - 20.0f)
      .SetRequestedHeight(STATUS_BAR_HEIGHT)
      .SetRequestedPositionX(10.0f)
      .SetRequestedPositionY(0.0f);
    mStatusBar.Add(mStatusLabel);
    window.Add(mStatusBar);

    // ── Content StackLayout (explicit height, larger than viewport) ───────────
    // Content height is explicit because ScrollView does not pass a height
    // constraint to its content; WRAP_CONTENT would resolve to 0.
    const float contentHeight = ITEM_COUNT * (ITEM_HEIGHT + ITEM_SPACING) + ITEM_SPACING;

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(WINDOW_WIDTH)
      .SetRequestedHeight(contentHeight)
      .SetSpacing(ITEM_SPACING)
      .SetPadding(Extents(0, 0, ITEM_SPACING, ITEM_SPACING));
    mContent = content;

    for(int i = 0; i < ITEM_COUNT; ++i)
    {
      View item = CreateItem(i);
      content.Add(item);
      if(i < 3)
      {
        mFirstItems[i] = item; // keep refs for size logging
      }
    }

    // ── ScrollView (explicit size, below status bar) ──────────────────────────
    const float scrollViewHeight = WINDOW_HEIGHT - STATUS_BAR_HEIGHT;

    mScrollView = ScrollView::New()
      .SetScrollDirection(ScrollDirection::Vertical)
      .SetOverScrollMode(OverScrollMode::ContentScrolls)
      .SetRequestedWidth(WINDOW_WIDTH)
      .SetRequestedHeight(scrollViewHeight)
      .SetRequestedPositionX(0.0f)
      .SetRequestedPositionY(STATUS_BAR_HEIGHT)
      .SetContent(content);

    mScrollView.ScrollStartedSignal().Connect(this, &ScrollInteractiveChildrenController::OnScrollStarted);
    mScrollView.ScrollingSignal().Connect(this, &ScrollInteractiveChildrenController::OnScrolling);
    mScrollView.ScrollFinishedSignal().Connect(this, &ScrollInteractiveChildrenController::OnScrollFinished);

    window.Add(mScrollView);

    DALI_LOG_RELEASE_INFO("[ScrollInteractiveSample] Create() done — scheduling layout dump in 1 s\n");

    // Fire a one-shot timer to log actual resolved sizes after layout
    mDebugTimer = Timer::New(1000);
    mDebugTimer.TickSignal().Connect(this, &ScrollInteractiveChildrenController::OnDebugTimer);
    mDebugTimer.Start();
  }

private:
  // ── Item factory ─────────────────────────────────────────────────────────
  View CreateItem(int index)
  {
    const Vector4 normalColor = index % 2 == 0 ? COLOR_ITEM_A : COLOR_ITEM_B;

    View item = View::New()
      .SetRequestedWidth(WINDOW_WIDTH)
      .SetRequestedHeight(ITEM_HEIGHT)
      .SetBackgroundColor(normalColor);

    std::ostringstream oss;
    oss << "Item " << index << "  (tap me)";
    Label text = Label::New(oss.str().c_str())
      .SetRequestedWidth(WINDOW_WIDTH - 32.0f)
      .SetRequestedHeight(ITEM_HEIGHT)
      .SetRequestedPositionX(16.0f)
      .SetFocusable(true);
    item.Add(text);

    item.AsInteractive([this, index, item, normalColor](InteractiveTrait& trait) mutable {
      trait.PressedChangedSignal().Connect(
        this,
        [item, normalColor](View, bool isPressed, InputEvent) mutable {
          item.SetBackgroundColor(isPressed ? COLOR_PRESSED : normalColor);
        });

      trait.ClickedSignal().Connect(
        this,
        [this, index](View, InputEvent) {
          OnItemClicked(index);
        });
    });

    return item;
  }

  // ── Diagnostics ───────────────────────────────────────────────────────────
  bool OnDebugTimer()
  {
    DALI_LOG_RELEASE_INFO("[ScrollInteractiveSample][Layout] ── post-layout size dump ──\n");
    LogViewInfo("StatusBar",   mStatusBar);
    LogViewInfo("StatusLabel", mStatusLabel);
    LogViewInfo("ScrollView",  mScrollView);
    LogViewInfo("Content",     mContent);
    for(int i = 0; i < 3; ++i)
    {
      if(mFirstItems[i])
      {
        std::ostringstream tag;
        tag << "Item[" << i << "]";
        LogViewInfo(tag.str().c_str(), mFirstItems[i]);
      }
    }
    return false; // one-shot: do not repeat
  }

  // ── Signal callbacks ──────────────────────────────────────────────────────
  void OnItemClicked(int index)
  {
    std::ostringstream msg;
    msg << "Clicked: Item " << index;
    mStatusLabel.SetText(msg.str().c_str());
    DALI_LOG_RELEASE_INFO("[ScrollInteractiveSample] Click  - %s\n", msg.str().c_str());
  }

  void OnScrollStarted(ScrollView scrollView)
  {
    std::ostringstream msg;
    msg << "Scroll started  pos=("
        << static_cast<int>(scrollView.GetScrollPosition().x) << ", "
        << static_cast<int>(scrollView.GetScrollPosition().y) << ")";
    mStatusLabel.SetText("Scrolling...");
    DALI_LOG_RELEASE_INFO("[ScrollInteractiveSample] %s\n", msg.str().c_str());
  }

  void OnScrolling(ScrollView scrollView)
  {
    DALI_LOG_RELEASE_INFO("[ScrollInteractiveSample] Scroll - pos=(%d, %d)\n",
                          static_cast<int>(scrollView.GetScrollPosition().x),
                          static_cast<int>(scrollView.GetScrollPosition().y));
  }

  void OnScrollFinished(ScrollView scrollView)
  {
    std::ostringstream msg;
    msg << "Scroll finished  pos=("
        << static_cast<int>(scrollView.GetScrollPosition().x) << ", "
        << static_cast<int>(scrollView.GetScrollPosition().y) << ")";
    mStatusLabel.SetText(msg.str().c_str());
    DALI_LOG_RELEASE_INFO("[ScrollInteractiveSample] %s\n", msg.str().c_str());
  }

  void OnKeyEvent(KeyEvent event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

  Application& mApplication;
  View         mStatusBar;
  Label        mStatusLabel;
  ScrollView   mScrollView;
  View         mContent;
  View         mFirstItems[3];
  Timer        mDebugTimer;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  ScrollInteractiveChildrenController controller(application);
  application.MainLoop();
  return 0;
}
