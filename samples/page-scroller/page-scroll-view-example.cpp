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
 * page-scroll-view-example.cpp
 *
 * Demonstrates PageScrollView with keyboard navigation.
 *
 *   [0,   0] 600x90   Info panel:
 *                       "Page N / M"  |  "ScrollPos: X"
 *   [0,  90] 600x990  PageScrollView — horizontal, 5 pages each 600×990 px
 *
 * Keyboard:
 *   ← / →    previous / next page  (animated)
 *   1 – 5    jump to page 0 – 4    (instant)
 *   ESC      quit
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/views/scroll/page-scroll-view.h>
#include <sstream>

using namespace Dali;
using namespace Dali::Ui;

// ─── layout constants ─────────────────────────────────────────────────────────
static constexpr float WINDOW_W       = 600.0f;
static constexpr float WINDOW_H       = 1080.0f;
static constexpr float INFO_H         = 90.0f;
static constexpr float SCROLL_Y       = INFO_H;
static constexpr float SCROLL_H       = WINDOW_H - SCROLL_Y;

static constexpr int   PAGE_COUNT     = 5;
static constexpr float PAGE_W         = WINDOW_W;
static constexpr float PAGE_H         = SCROLL_H;

// ─── colours ──────────────────────────────────────────────────────────────────
static const Vector4 COLOR_INFO_BG      (0.10f, 0.10f, 0.14f, 1.0f);

static const Vector4 PAGE_COLORS[PAGE_COUNT] = {
  Vector4(0.20f, 0.45f, 0.80f, 1.0f), // blue
  Vector4(0.20f, 0.70f, 0.45f, 1.0f), // green
  Vector4(0.75f, 0.45f, 0.20f, 1.0f), // orange
  Vector4(0.70f, 0.25f, 0.60f, 1.0f), // purple
  Vector4(0.25f, 0.65f, 0.75f, 1.0f), // teal
};

static const char* PAGE_NAMES[PAGE_COUNT] = {
  "Page 1 — Blue",
  "Page 2 — Green",
  "Page 3 — Orange",
  "Page 4 — Purple",
  "Page 5 — Teal",
};

class PageScrollViewController : public ConnectionTracker
{
public:
  explicit PageScrollViewController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &PageScrollViewController::Create);
  }

  ~PageScrollViewController() = default;

private:
  void Create(Application application)
  {
    Window window = application.GetWindow();
    auto positionSize = window.GetPositionSize();
    window.SetPositionSize(Dali::PositionSize(positionSize.x, positionSize.y, WINDOW_W, WINDOW_H));
    window.SetBackgroundColor(Color::BLACK);
    window.KeyEventSignal().Connect(this, &PageScrollViewController::OnKeyEvent);

    BuildInfoPanel(window);
    BuildPageScrollView(window);

    // PageChangedSignal updates the info panel
    mPageScrollView.PageChangedSignal().Connect(this, &PageScrollViewController::OnPageChanged);

    // Show initial page info
    RefreshInfoPanel();
  }

  // ── Info panel ──────────────────────────────────────────────────────────────

  void BuildInfoPanel(Window& window)
  {
    View panel = View::New();
    panel.SetBackgroundColor(COLOR_INFO_BG);
    panel.SetRequestedWidth(WINDOW_W);
    panel.SetRequestedHeight(INFO_H);
    panel.SetRequestedPositionX(0.0f);
    panel.SetRequestedPositionY(0.0f);

    // Page counter label (top-left)
    mPageLabel = Label::New("Page 1 / 5");
    mPageLabel.SetRequestedWidth(220.0f);
    mPageLabel.SetRequestedHeight(40.0f);
    mPageLabel.SetRequestedPositionX(12.0f);
    mPageLabel.SetRequestedPositionY(4.0f);
    mPageLabel.SetTextColor(Color::WHITE);
    panel.Add(mPageLabel);

    // Scroll position label (top-right area)
    mScrollPosLabel = Label::New("ScrollPos: 0");
    mScrollPosLabel.SetRequestedWidth(250.0f);
    mScrollPosLabel.SetRequestedHeight(40.0f);
    mScrollPosLabel.SetRequestedPositionX(240.0f);
    mScrollPosLabel.SetRequestedPositionY(4.0f);
    mScrollPosLabel.SetTextColor(Color::WHITE);
    panel.Add(mScrollPosLabel);

    // Keyboard hint (bottom)
    Label hint = Label::New("← → nav   1-5 jump   ESC");
    hint.SetRequestedWidth(WINDOW_W - 24.0f);
    hint.SetRequestedHeight(34.0f);
    hint.SetRequestedPositionX(12.0f);
    hint.SetRequestedPositionY(50.0f);
    hint.SetTextColor(Color::WHITE);
    panel.Add(hint);

    window.Add(panel);
  }

  void RefreshInfoPanel()
  {
    // Page label
    std::ostringstream pg;
    pg << "Page " << (mPageScrollView.GetCurrentPage() + 1)
       << " / " << mPageScrollView.GetPageCount();
    mPageLabel.SetText(pg.str().c_str());

    // Scroll pos
    Vector2 pos = mPageScrollView.GetScrollPosition();
    std::ostringstream sp;
    sp << "ScrollPos: " << static_cast<int>(pos.x);
    mScrollPosLabel.SetText(sp.str().c_str());
  }

  // ── PageScrollView ──────────────────────────────────────────────────────────

  void BuildPageScrollView(Window& window)
  {
    // Content: a horizontal stack of PAGE_COUNT pages
    StackLayout content = StackLayout::New(StackOrientation::HORIZONTAL);
    content.SetRequestedWidth(PAGE_W * PAGE_COUNT);
    content.SetRequestedHeight(PAGE_H);

    for(int i = 0; i < PAGE_COUNT; ++i)
    {
      View page = View::New();
      page.SetBackgroundColor(PAGE_COLORS[i]);
      page.SetRequestedWidth(PAGE_W);
      page.SetRequestedHeight(PAGE_H);

      // Large centered page name label
      Label nameLabel = Label::New(PAGE_NAMES[i]);
      nameLabel.SetRequestedWidth(PAGE_W - 40.0f);
      nameLabel.SetRequestedHeight(80.0f);
      nameLabel.SetRequestedPositionX(20.0f);
      nameLabel.SetRequestedPositionY(PAGE_H * 0.4f);
      nameLabel.SetTextColor(Color::WHITE);
      nameLabel.SetProperty(Label::Property::FONT_SIZE, 32.0f);
      page.Add(nameLabel);

      // Navigation hint label
      std::ostringstream hint;
      hint << "← → to navigate    drag to scroll";
      Label hintLabel = Label::New(hint.str().c_str());
      hintLabel.SetRequestedWidth(PAGE_W - 40.0f);
      hintLabel.SetRequestedHeight(40.0f);
      hintLabel.SetRequestedPositionX(20.0f);
      hintLabel.SetRequestedPositionY(PAGE_H * 0.5f + 50.0f);
      hintLabel.SetTextColor(Color::WHITE);
      page.Add(hintLabel);

      content.Add(page);
    }

    mPageScrollView = PageScrollView::New();
    mPageScrollView.SetScrollDirection(ScrollDirection::Horizontal);
    mPageScrollView.SetOverScrollMode(OverScrollMode::ContentScrolls);
    mPageScrollView.SetVerticalScrollBarVisibility(ScrollBarVisibility::Never);
    mPageScrollView.SetHorizontalScrollBarVisibility(ScrollBarVisibility::Never);
    mPageScrollView.SetMaxFlingDistance(PAGE_W * PAGE_COUNT);
    mPageScrollView.SetRequestedWidth(WINDOW_W);
    mPageScrollView.SetRequestedHeight(SCROLL_H);
    mPageScrollView.SetRequestedPositionX(0.0f);
    mPageScrollView.SetRequestedPositionY(SCROLL_Y);
    mPageScrollView.SetContent(content);

    window.Add(mPageScrollView);

    // Connect scroll position updates
    mPageScrollView.ScrollingSignal().Connect(this, &PageScrollViewController::OnScrolling);
    mPageScrollView.DraggingSignal().Connect(this, &PageScrollViewController::OnDragging);
  }

  // ── Signal callbacks ────────────────────────────────────────────────────────

  void OnPageChanged(int /*currentPage*/, int /*pageCount*/)
  {
    RefreshInfoPanel();
  }

  void OnScrolling(ScrollView sv)
  {
    Vector2 pos = sv.GetScrollPosition();
    std::ostringstream sp;
    sp << "ScrollPos: " << static_cast<int>(pos.x);
    mScrollPosLabel.SetText(sp.str().c_str());
  }

  void OnDragging(ScrollView sv, float /*deltaX*/, float /*deltaY*/)
  {
    Vector2 pos = sv.GetScrollPosition();
    std::ostringstream sp;
    sp << "ScrollPos: " << static_cast<int>(pos.x);
    mScrollPosLabel.SetText(sp.str().c_str());
  }

  // ── Key events ──────────────────────────────────────────────────────────────

  void OnKeyEvent(Window /*window*/, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::DOWN) return;

    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
      return;
    }

    const Dali::String& key = event.GetKeyName();

    if(key == "Left")
    {
      int target = mPageScrollView.GetCurrentPage() - 1;
      if(target >= 0)
        mPageScrollView.ScrollToPage(target, true);
    }
    else if(key == "Right")
    {
      int target = mPageScrollView.GetCurrentPage() + 1;
      if(target < mPageScrollView.GetPageCount())
        mPageScrollView.ScrollToPage(target, true);
    }
    else if(key == "1") { mPageScrollView.ScrollToPage(0, false); }
    else if(key == "2") { mPageScrollView.ScrollToPage(1, false); }
    else if(key == "3") { mPageScrollView.ScrollToPage(2, false); }
    else if(key == "4") { mPageScrollView.ScrollToPage(3, false); }
    else if(key == "5") { mPageScrollView.ScrollToPage(4, false); }
    RefreshInfoPanel();
  }

  // ── Members ─────────────────────────────────────────────────────────────────

  Application&   mApplication;
  PageScrollView mPageScrollView;

  // Info panel widgets
  Label mPageLabel;
  Label mScrollPosLabel;
};

// ─────────────────────────────────────────────────────────────────────────────

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  PageScrollViewController example(application);
  application.MainLoop();
  return 0;
}
