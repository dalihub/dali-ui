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
 * page-scroll-view-dynamic-example.cpp
 *
 * Tests dynamic page insertion and removal while tracking the current page.
 *
 *   [0,   0] 600x80   Info panel  — "Page N/M"  live-updated via PageChangedSignal
 *   [0,  80] 600x776  PageScrollView  — initial 3 pages
 *   [0, 856] 600x224  Control panel:
 *                       Row A: [+End]  [+Before]  [+After]
 *                       Row B: [-End]  [-Current] [-Before]
 *                       Row C: Scenario / nav / [Tmpl] toggle
 *
 * All mutations go through NotifyPagesInserted / NotifyPagesRemoved so the
 * current-page index and PageIndicator stay in sync immediately (before the
 * next layout pass).
 *
 * Keyboard:
 *   ← / →    navigate pages
 *   E        add page at End
 *   B        add page Before current
 *   A        add page After current
 *   D        Delete current page
 *   P        delete Page before current
 *   ESC      quit
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/views/scroll/page-scroll-view.h>
#include <sstream>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

// ─── layout ──────────────────────────────────────────────────────────────────
static constexpr float WINDOW_W    = 600.0f;
static constexpr float WINDOW_H    = 1080.0f;
static constexpr float INFO_H      = 80.0f;
static constexpr float CTRL_H      = 224.0f;
static constexpr float SCROLL_Y    = INFO_H;
static constexpr float SCROLL_H    = WINDOW_H - SCROLL_Y - CTRL_H;
static constexpr float PAGE_W      = WINDOW_W;
static constexpr float PAGE_H      = SCROLL_H;

static constexpr float BTN_H    = 50.0f;
static constexpr float BTN_GAP  = 8.0f;
static constexpr float CTRL_PAD = 10.0f;

// ─── colour palette ───────────────────────────────────────────────────────────
static const Vector4 COLOR_INFO_BG    (0.10f, 0.10f, 0.14f, 1.0f);
static const Vector4 COLOR_CTRL_BG    (0.12f, 0.12f, 0.18f, 1.0f);
static const Vector4 COLOR_BTN_ADD    (0.18f, 0.55f, 0.25f, 1.0f);
static const Vector4 COLOR_BTN_REMOVE (0.65f, 0.22f, 0.22f, 1.0f);
static const Vector4 COLOR_BTN_NAV    (0.20f, 0.40f, 0.72f, 1.0f);
static const Vector4 COLOR_BTN_MASS   (0.50f, 0.35f, 0.10f, 1.0f);

// Eight pre-defined page colours; cycle with modulo when more pages are needed.
static const Vector4 PAGE_PALETTE[] = {
  Vector4(0.20f, 0.45f, 0.80f, 1.0f), // blue
  Vector4(0.20f, 0.70f, 0.45f, 1.0f), // green
  Vector4(0.75f, 0.45f, 0.20f, 1.0f), // orange
  Vector4(0.70f, 0.25f, 0.60f, 1.0f), // purple
  Vector4(0.25f, 0.65f, 0.75f, 1.0f), // teal
  Vector4(0.75f, 0.65f, 0.15f, 1.0f), // yellow
  Vector4(0.60f, 0.20f, 0.20f, 1.0f), // red
  Vector4(0.35f, 0.35f, 0.35f, 1.0f), // grey
};
static constexpr int PALETTE_SIZE = static_cast<int>(sizeof(PAGE_PALETTE) / sizeof(PAGE_PALETTE[0]));

class PageScrollDynamicController : public ConnectionTracker
{
public:
  explicit PageScrollDynamicController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &PageScrollDynamicController::Create);
  }

private:
  void Create(Application application)
  {
    Window window = application.GetWindow();
    auto positionSize = window.GetPositionSize();
    window.SetPositionSize(Dali::PositionSize(positionSize.x, positionSize.y, WINDOW_W, WINDOW_H));
    window.SetBackgroundColor(Color::BLACK);
    window.KeyEventSignal().Connect(this, &PageScrollDynamicController::OnKeyEvent);

    BuildInfoPanel(window);
    BuildScrollView(window);
    BuildControlPanel(window);

    mPageScrollView.PageChangedSignal().Connect(this, &PageScrollDynamicController::OnPageChanged);

    RefreshInfo();
  }

  // ── Info panel ──────────────────────────────────────────────────────────────

  void BuildInfoPanel(Window& window)
  {
    View panel = View::New();
    panel.SetBackgroundColor(COLOR_INFO_BG);
    panel.SetRequestedWidth(WINDOW_W);
    panel.SetRequestedHeight(INFO_H);
    panel.SetRequestedX(0.0f);
    panel.SetRequestedY(0.0f);

    mPageLabel = Label::New("Page 0/0");
    mPageLabel.SetRequestedWidth(300.0f);
    mPageLabel.SetRequestedHeight(40.0f);
    mPageLabel.SetRequestedX(12.0f);
    mPageLabel.SetRequestedY(4.0f);
    mPageLabel.SetTextColor(Color::WHITE);
    panel.Add(mPageLabel);

    mEventLabel = Label::New("—");
    mEventLabel.SetRequestedWidth(WINDOW_W - 16.0f);
    mEventLabel.SetRequestedHeight(34.0f);
    mEventLabel.SetRequestedX(12.0f);
    mEventLabel.SetRequestedY(42.0f);
    mEventLabel.SetTextColor(Color::WHITE);
    panel.Add(mEventLabel);

    window.Add(panel);
  }

  void RefreshInfo()
  {
    int cur   = mPageScrollView.GetCurrentPage();
    int total = mPageScrollView.GetPageCount();

    std::ostringstream pg;
    if(total <= 0)
      pg << "Empty  (0 pages)";
    else
      pg << "Page " << (cur + 1) << " / " << total << "  (index " << cur << ")";
    mPageLabel.SetText(pg.str().c_str());
  }

  void LogEvent(const std::string& msg)
  {
    mEventLabel.SetText(msg.c_str());
  }

  // ── PageScrollView and initial content ──────────────────────────────────────

  void BuildScrollView(Window& window)
  {
    // Content container — we manage mPages ourselves so insert/remove is easy.
    mContent = StackLayout::New(StackOrientation::HORIZONTAL);
    mContent.SetRequestedWidth(PAGE_W * 3);
    mContent.SetRequestedHeight(PAGE_H);

    for(int i = 0; i < 3; ++i)
      AppendPageToContent(i);

    mPageScrollView = PageScrollView::New();
    mPageScrollView.SetScrollDirection(ScrollDirection::Horizontal);
    mPageScrollView.SetOverScrollMode(OverScrollMode::ContentScrolls);
    mPageScrollView.SetVerticalScrollBarVisibility(ScrollBarVisibility::Never);
    mPageScrollView.SetHorizontalScrollBarVisibility(ScrollBarVisibility::Never);
    mPageScrollView.SetMaxFlingDistance(PAGE_W * 20);
    mPageScrollView.SetRequestedWidth(WINDOW_W);
    mPageScrollView.SetRequestedHeight(SCROLL_H);
    mPageScrollView.SetRequestedX(0.0f);
    mPageScrollView.SetRequestedY(SCROLL_Y);
    mPageScrollView.SetBackgroundColor(Vector4(0.08f, 0.08f, 0.08f, 1.0f));
    mPageScrollView.SetContent(mContent);

    window.Add(mPageScrollView);
  }

  // ── Control panel ────────────────────────────────────────────────────────────

  View MakeButton(const char* text, const Vector4& color)
  {
    View btn = View::New();
    btn.SetBackgroundColor(color);
    btn.SetProperty(View::Property::CORNER_RADIUS, Vector4(6, 6, 6, 6));

    Label lbl = Label::New(text);
    lbl.SetRequestedWidth(1.0f); // will stretch via MATCH_PARENT
    lbl.SetRequestedHeight(BTN_H);
    lbl.SetRequestedWidth(MATCH_PARENT);
    lbl.SetTextColor(Color::WHITE);
    btn.Add(lbl);

    return btn;
  }

  void BuildControlPanel(Window& window)
  {
    float ctrlY = SCROLL_Y + SCROLL_H;

    View panel = View::New();
    panel.SetBackgroundColor(COLOR_CTRL_BG);
    panel.SetRequestedWidth(WINDOW_W);
    panel.SetRequestedHeight(CTRL_H);
    panel.SetRequestedX(0.0f);
    panel.SetRequestedY(ctrlY);

    auto addRow = [&](float y, std::vector<std::pair<const char*, Vector4>> items) {
      int   n       = static_cast<int>(items.size());
      float btnW    = (WINDOW_W - CTRL_PAD * 2 - BTN_GAP * (n - 1)) / n;
      for(int i = 0; i < n; ++i)
      {
        View btn = MakeButton(items[i].first, items[i].second);
        btn.SetRequestedWidth(btnW);
        btn.SetRequestedHeight(BTN_H);
        btn.SetRequestedX(CTRL_PAD + i * (btnW + BTN_GAP));
        btn.SetRequestedY(y);
        btn.TouchEventSignal().Connect(this, &PageScrollDynamicController::OnButtonTouched);
        mButtons.push_back({btn, std::string(items[i].first)});
        panel.Add(btn);
      }
    };

    // Row A: Insert
    addRow(6.0f, {
      {"+End",     COLOR_BTN_ADD},
      {"+Before",  COLOR_BTN_ADD},
      {"+After",   COLOR_BTN_ADD},
    });
    // Row B: Remove
    addRow(6.0f + BTN_H + BTN_GAP, {
      {"-End",     COLOR_BTN_REMOVE},
      {"-Current", COLOR_BTN_REMOVE},
      {"-Before",  COLOR_BTN_REMOVE},
    });
    // Row C: Scenarios + nav
    addRow(6.0f + (BTN_H + BTN_GAP) * 2, {
      {"++3 at 0", COLOR_BTN_MASS},
      {"--All",    COLOR_BTN_MASS},
      {"Nav <",    COLOR_BTN_NAV},
      {"Nav >",    COLOR_BTN_NAV},
    });

    window.Add(panel);
  }

  // ── Page content helpers ─────────────────────────────────────────────────────

  View MakePageView(int sequenceNum, int paletteIndex)
  {
    View page = View::New();
    page.SetBackgroundColor(PAGE_PALETTE[paletteIndex % PALETTE_SIZE]);
    page.SetRequestedWidth(PAGE_W);
    page.SetRequestedHeight(PAGE_H);

    std::ostringstream name;
    name << "Page #" << sequenceNum;
    Label nameLabel = Label::New(name.str().c_str());
    nameLabel.SetRequestedWidth(PAGE_W - 40.0f);
    nameLabel.SetRequestedHeight(80.0f);
    nameLabel.SetRequestedX(20.0f);
    nameLabel.SetRequestedY(PAGE_H * 0.4f);
    nameLabel.SetTextColor(Color::WHITE);
    nameLabel.SetProperty(Label::Property::FONT_SIZE, 28.0f);
    page.Add(nameLabel);

    return page;
  }

  void AppendPageToContent(int paletteHint)
  {
    View page = MakePageView(++mPageSequence, paletteHint);
    mContent.Add(page);
    mPages.push_back(page);
    ResizeContent();
  }

  void InsertPageIntoContent(int atIndex, int paletteHint)
  {
    View page = MakePageView(++mPageSequence, paletteHint);
    // StackLayout doesn't expose InsertAt — use a rebuild approach:
    // remove all pages from atIndex onward, add the new one, re-add the rest.
    std::vector<View> tail(mPages.begin() + atIndex, mPages.end());
    for(auto& v : tail)
      mContent.Remove(v);

    mContent.Add(page);
    for(auto& v : tail)
      mContent.Add(v);

    mPages.insert(mPages.begin() + atIndex, page);
    ResizeContent();
  }

  void RemovePageFromContent(int atIndex)
  {
    if(atIndex < 0 || atIndex >= static_cast<int>(mPages.size())) return;
    mContent.Remove(mPages[static_cast<size_t>(atIndex)]);
    mPages.erase(mPages.begin() + atIndex);
    ResizeContent();
  }

  void ResizeContent()
  {
    float w = PAGE_W * static_cast<float>(mPages.size());
    mContent.SetRequestedWidth(std::max(PAGE_W, w));
  }

  // ── Mutations ────────────────────────────────────────────────────────────────

  void AddPageAtEnd()
  {
    int atIndex = static_cast<int>(mPages.size()); // = old page count
    AppendPageToContent(atIndex);
    mPageScrollView.NotifyPagesInserted(atIndex, 1);
    std::ostringstream msg;
    msg << "+End → inserted at " << atIndex << "  total=" << mPageScrollView.GetPageCount();
    LogEvent(msg.str());
    RefreshInfo();
  }

  void AddPageBefore()
  {
    int cur     = mPageScrollView.GetCurrentPage();
    int atIndex = cur;
    InsertPageIntoContent(atIndex, mPageSequence % PALETTE_SIZE);
    mPageScrollView.NotifyPagesInserted(atIndex, 1);
    std::ostringstream msg;
    msg << "+Before → inserted at " << atIndex << "  cur=" << mPageScrollView.GetCurrentPage()
        << "  total=" << mPageScrollView.GetPageCount();
    LogEvent(msg.str());
    RefreshInfo();
  }

  void AddPageAfter()
  {
    int cur     = mPageScrollView.GetCurrentPage();
    int atIndex = cur + 1;
    if(atIndex > static_cast<int>(mPages.size()))
      atIndex = static_cast<int>(mPages.size());
    InsertPageIntoContent(atIndex, mPageSequence % PALETTE_SIZE);
    mPageScrollView.NotifyPagesInserted(atIndex, 1);
    std::ostringstream msg;
    msg << "+After → inserted at " << atIndex << "  cur=" << mPageScrollView.GetCurrentPage()
        << "  total=" << mPageScrollView.GetPageCount();
    LogEvent(msg.str());
    RefreshInfo();
  }

  void RemovePageAtEnd()
  {
    if(mPages.empty()) { LogEvent("nothing to remove"); return; }
    int atIndex = static_cast<int>(mPages.size()) - 1;
    RemovePageFromContent(atIndex);
    mPageScrollView.NotifyPagesRemoved(atIndex, 1);
    std::ostringstream msg;
    msg << "-End → removed " << atIndex << "  cur=" << mPageScrollView.GetCurrentPage()
        << "  total=" << mPageScrollView.GetPageCount();
    LogEvent(msg.str());
    RefreshInfo();
  }

  void RemoveCurrentPage()
  {
    if(mPages.empty()) { LogEvent("nothing to remove"); return; }
    int atIndex = mPageScrollView.GetCurrentPage();
    RemovePageFromContent(atIndex);
    mPageScrollView.NotifyPagesRemoved(atIndex, 1);
    std::ostringstream msg;
    msg << "-Current (was " << atIndex << ") → cur=" << mPageScrollView.GetCurrentPage()
        << "  total=" << mPageScrollView.GetPageCount();
    LogEvent(msg.str());
    RefreshInfo();
  }

  void RemovePageBefore()
  {
    int cur = mPageScrollView.GetCurrentPage();
    if(cur == 0) { LogEvent("no page before current"); return; }
    int atIndex = cur - 1;
    RemovePageFromContent(atIndex);
    mPageScrollView.NotifyPagesRemoved(atIndex, 1);
    std::ostringstream msg;
    msg << "-Before (removed " << atIndex << ") → cur=" << mPageScrollView.GetCurrentPage()
        << "  total=" << mPageScrollView.GetPageCount();
    LogEvent(msg.str());
    RefreshInfo();
  }

  void MassInsertAt0()
  {
    // Insert 3 pages at position 0 — current page should shift right by 3
    for(int i = 0; i < 3; ++i)
      InsertPageIntoContent(i, (mPageSequence + i) % PALETTE_SIZE);
    mPageScrollView.NotifyPagesInserted(0, 3);
    std::ostringstream msg;
    msg << "++3 at 0 → cur=" << mPageScrollView.GetCurrentPage()
        << "  total=" << mPageScrollView.GetPageCount();
    LogEvent(msg.str());
    RefreshInfo();
  }

  void RemoveAll()
  {
    if(mPages.empty()) { LogEvent("already empty"); return; }
    int oldCount = static_cast<int>(mPages.size());
    for(int i = oldCount - 1; i >= 0; --i)
      RemovePageFromContent(i);
    // newTotal = max(0, oldCount - oldCount) = 0 — truly empty state
    mPageScrollView.NotifyPagesRemoved(0, oldCount);
    std::ostringstream msg;
    msg << "--All → empty  total=" << mPageScrollView.GetPageCount();
    LogEvent(msg.str());
    RefreshInfo();
  }

  // ── Signals ──────────────────────────────────────────────────────────────────

  void OnPageChanged(int currentPage, int pageCount)
  {
    (void)currentPage;
    (void)pageCount;
    RefreshInfo();
  }

  // ── Button touch ─────────────────────────────────────────────────────────────

  bool OnButtonTouched(Actor actor, TouchEvent touch)
  {
    if(touch.GetState(0) != PointState::UP) return false;

    for(auto& entry : mButtons)
    {
      if(entry.first == actor)
      {
        const std::string& lbl = entry.second;
        if(lbl == "+End")         AddPageAtEnd();
        else if(lbl == "+Before") AddPageBefore();
        else if(lbl == "+After")  AddPageAfter();
        else if(lbl == "-End")    RemovePageAtEnd();
        else if(lbl == "-Current")RemoveCurrentPage();
        else if(lbl == "-Before") RemovePageBefore();
        else if(lbl == "++3 at 0")  MassInsertAt0();
        else if(lbl == "--All")     RemoveAll();
        else if(lbl == "Nav <")     mPageScrollView.ScrollToPage(mPageScrollView.GetCurrentPage() - 1, true);
        else if(lbl == "Nav >")     mPageScrollView.ScrollToPage(mPageScrollView.GetCurrentPage() + 1, true);
        return true;
      }
    }
    return false;
  }

  // ── Key events ───────────────────────────────────────────────────────────────

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
      mPageScrollView.ScrollToPage(mPageScrollView.GetCurrentPage() - 1, true);
    else if(key == "Right")
      mPageScrollView.ScrollToPage(mPageScrollView.GetCurrentPage() + 1, true);
    else if(key == "e" || key == "E") AddPageAtEnd();
    else if(key == "b" || key == "B") AddPageBefore();
    else if(key == "a" || key == "A") AddPageAfter();
    else if(key == "d" || key == "D") RemoveCurrentPage();
    else if(key == "p" || key == "P") RemovePageBefore();
  }

  // ── Members ──────────────────────────────────────────────────────────────────

  Application&        mApplication;
  PageScrollView      mPageScrollView;
  StackLayout         mContent;

  // Page tracking
  std::vector<View>   mPages;
  int                 mPageSequence{0}; // monotonic counter for page labels

  // Button registry: (view, label-string)
  std::vector<std::pair<View, std::string>> mButtons;

  // Info panel
  Label mPageLabel;
  Label mEventLabel;
};

// ─────────────────────────────────────────────────────────────────────────────

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  PageScrollDynamicController example(application);
  application.MainLoop();
  return 0;
}
