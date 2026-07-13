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

#include "manual-test-case.h"

#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
// Colors
constexpr uint32_t COLOR_HEADER_BG   = 0x1565C0; // Dark blue
constexpr uint32_t COLOR_HEADER_TEXT = 0xFFFFFF; // White
constexpr uint32_t COLOR_ITEM_BG     = 0xFFFFFF; // White
constexpr uint32_t COLOR_ITEM_NAME   = 0x212121; // Near-black
constexpr uint32_t COLOR_ITEM_DESC   = 0x757575; // Gray
constexpr uint32_t COLOR_SEPARATOR   = 0xE0E0E0; // Light gray
constexpr uint32_t COLOR_APP_BG      = 0xF5F5F5; // Light gray

// Sizes (pixels)
constexpr float HEADER_HEIGHT = 60.0f;
constexpr float SEPARATOR_H   = 1.0f;
constexpr float PADDING_H     = 20.0f;
constexpr float PADDING_V     = 14.0f;

// Font sizes (pixels)
constexpr float FONT_HEADER  = 22.0f;
constexpr float FONT_TC_NAME = 17.0f;
constexpr float FONT_TC_DESC = 13.0f;
} // namespace

/**
 * @brief Manual test launcher application.
 *
 * Displays a scrollable list of all registered test cases.  Tapping a row
 * navigates into that test case; the hardware/software Back key (or the
 * on-screen Back label) returns to the list.
 */
class ManualTestLauncher : public ConnectionTracker
{
public:
  explicit ManualTestLauncher(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &ManualTestLauncher::OnCreate);
  }

private:
  // -------------------------------------------------------------------------
  // Initialisation
  // -------------------------------------------------------------------------

  void OnCreate(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(COLOR_APP_BG));
    window.KeyEventSignal().Connect(this, &ManualTestLauncher::OnKeyEvent);

    mTestCases = ManualTest::Registry::Get().CreateAll();

    mRootContainer = StackLayout::New(StackOrientation::VERTICAL);
    mRootContainer.SetRequestedWidth(MATCH_PARENT);
    mRootContainer.SetRequestedHeight(MATCH_PARENT);
    window.Add(mRootContainer);

    ShowListScreen();
  }

  // -------------------------------------------------------------------------
  // TC List Screen
  // -------------------------------------------------------------------------

  void ShowListScreen()
  {
    mRootContainer.RemoveAllChildren();
    mActiveCase = nullptr;

    // ── Header ──────────────────────────────────────────────────────────────
    mRootContainer.Add(MakeHeader("Manual Tests"));

    // ── TC list ─────────────────────────────────────────────────────────────
    StackLayout listContent = StackLayout::New(StackOrientation::VERTICAL);
    listContent.SetRequestedWidth(MATCH_PARENT);
    listContent.SetRequestedHeight(WRAP_CONTENT);

    if(mTestCases.empty())
    {
      Label emptyLabel = Label::New("No test cases registered.");
      emptyLabel.SetTextColor(UiColor(COLOR_ITEM_DESC));
      emptyLabel.SetFontSize(FONT_TC_NAME);
      emptyLabel.SetRequestedWidth(MATCH_PARENT);
      emptyLabel.SetRequestedHeight(WRAP_CONTENT);
      emptyLabel.SetPadding(Extents(PADDING_H, PADDING_H, PADDING_V * 2, PADDING_V * 2));
      listContent.Add(emptyLabel);
    }

    for(std::size_t i = 0; i < mTestCases.size(); ++i)
    {
      listContent.Add(MakeListItem(i));
      listContent.Add(MakeSeparator());
    }

    ScrollView scrollView = ScrollView::New();
    scrollView.SetScrollDirection(ScrollDirection::Vertical);
    scrollView.SetRequestedWidth(MATCH_PARENT);
    scrollView.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    scrollView.SetContent(listContent);
    mRootContainer.Add(scrollView);
  }

  View MakeHeader(const Dali::String& title)
  {
    Label titleLabel = Label::New(title);
    titleLabel.SetTextColor(UiColor(COLOR_HEADER_TEXT));
    titleLabel.SetFontSize(FONT_HEADER);
    titleLabel.SetRequestedWidth(MATCH_PARENT);
    titleLabel.SetRequestedHeight(MATCH_PARENT);
    titleLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);

    StackLayout header = StackLayout::New(StackOrientation::HORIZONTAL);
    header.SetRequestedWidth(MATCH_PARENT);
    header.SetRequestedHeight(HEADER_HEIGHT);
    header.SetBackgroundColor(UiColor(COLOR_HEADER_BG));
    header.SetPadding(Extents(PADDING_H, PADDING_H, 0, 0));
    header.Add(titleLabel);
    return header;
  }

  View MakeListItem(std::size_t index)
  {
    const auto& tc = mTestCases[index];

    Label nameLabel = Label::New(tc->GetName());
    nameLabel.SetTextColor(UiColor(COLOR_ITEM_NAME));
    nameLabel.SetFontSize(FONT_TC_NAME);
    nameLabel.SetRequestedWidth(MATCH_PARENT);
    nameLabel.SetRequestedHeight(WRAP_CONTENT);

    Label descriptionLabel = Label::New(tc->GetDescription());
    descriptionLabel.SetTextColor(UiColor(COLOR_ITEM_DESC));
    descriptionLabel.SetFontSize(FONT_TC_DESC);
    descriptionLabel.SetRequestedWidth(MATCH_PARENT);
    descriptionLabel.SetRequestedHeight(WRAP_CONTENT);

    StackLayout item = StackLayout::New(StackOrientation::VERTICAL);
    item.SetRequestedWidth(MATCH_PARENT);
    item.SetRequestedHeight(WRAP_CONTENT);
    item.SetBackgroundColor(UiColor(COLOR_ITEM_BG));
    item.SetPadding(Extents(PADDING_H, PADDING_H, PADDING_V, PADDING_V));
    item.SetFocusable(true);
    item.SetStateEffect(OverlayEffect::ListItem());
    InteractiveTrait interactive = item.AsInteractive();
    interactive.ClickedSignal().Connect(this, [this, index](View, InputEvent) -> bool
    {
      EnterTestCase(index);
      return true;
    });
    item.Add(nameLabel);
    item.Add(descriptionLabel);
    return item;
  }

  View MakeSeparator()
  {
    View separator = View::New();
    separator.SetRequestedWidth(MATCH_PARENT);
    separator.SetRequestedHeight(SEPARATOR_H);
    separator.SetBackgroundColor(UiColor(COLOR_SEPARATOR));
    return separator;
  }

  // -------------------------------------------------------------------------
  // TC Content Screen
  // -------------------------------------------------------------------------

  void EnterTestCase(std::size_t index)
  {
    if(index >= mTestCases.size())
    {
      return;
    }

    mActiveCase = mTestCases[index].get();
    mRootContainer.RemoveAllChildren();

    // ── Navigation header ───────────────────────────────────────────────────
    Label backLabel = Label::New("< Back");
    backLabel.SetTextColor(UiColor(COLOR_HEADER_TEXT));
    backLabel.SetFontSize(FONT_HEADER);
    backLabel.SetRequestedWidth(WRAP_CONTENT);
    backLabel.SetRequestedHeight(MATCH_PARENT);
    backLabel.SetPadding(Extents(PADDING_H, PADDING_H * 2, 0, 0));
    backLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    backLabel.SetFocusable(true);
    backLabel.SetStateEffect(OverlayEffect::ListItem());
    InteractiveTrait backInteractive = backLabel.AsInteractive();
    backInteractive.ClickedSignal().Connect(this, [this](View, InputEvent) -> bool
    {
      BackToList();
      return true;
    });

    Label titleLabel = Label::New(mActiveCase->GetName());
    titleLabel.SetTextColor(UiColor(COLOR_HEADER_TEXT));
    titleLabel.SetFontSize(FONT_HEADER);
    titleLabel.SetRequestedWidth(MATCH_PARENT);
    titleLabel.SetRequestedHeight(MATCH_PARENT);
    titleLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);

    StackLayout header = StackLayout::New(StackOrientation::HORIZONTAL);
    header.SetRequestedWidth(MATCH_PARENT);
    header.SetRequestedHeight(HEADER_HEIGHT);
    header.SetBackgroundColor(UiColor(COLOR_HEADER_BG));
    header.Add(backLabel);
    header.Add(titleLabel);
    mRootContainer.Add(header);

    // ── Content area: TC adds its views here ────────────────────────────────
    StackLayout contentArea = StackLayout::New(StackOrientation::VERTICAL);
    contentArea.SetRequestedWidth(MATCH_PARENT);
    contentArea.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    mRootContainer.Add(contentArea);

    mActiveCase->OnEnter(contentArea);
  }

  void BackToList()
  {
    if(mActiveCase)
    {
      mActiveCase->OnExit();
      mActiveCase = nullptr;
    }
    ShowListScreen();
  }

  // -------------------------------------------------------------------------
  // Key event
  // -------------------------------------------------------------------------

  void OnKeyEvent(Window window, KeyEvent event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_BACK))
      {
        if(mActiveCase)
        {
          BackToList();
        }
        else
        {
          mApplication.Quit();
        }
      }
      else if(IsKey(event, Dali::DALI_KEY_ESCAPE))
      {
        mApplication.Quit();
      }
    }
  }

  // -------------------------------------------------------------------------
  // Members
  // -------------------------------------------------------------------------

  Application&                                       mApplication;
  StackLayout                                        mRootContainer;
  std::vector<std::unique_ptr<ManualTest::TestCase>> mTestCases;
  ManualTest::TestCase*                              mActiveCase{nullptr};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig config = UiConfig::New();
  config.SetDefaultStateEffectForInteractive(OverlayEffect::Plain());
  config.Apply();
  ManualTestLauncher launcher(application);
  application.MainLoop();
  return 0;
}
