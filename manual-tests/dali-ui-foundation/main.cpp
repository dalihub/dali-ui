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

  void OnCreate(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(COLOR_APP_BG));
    window.KeyEventSignal().Connect(this, &ManualTestLauncher::OnKeyEvent);

    mTestCases = ManualTest::Registry::Get().CreateAll();

    mRootContainer = StackLayout::New(StackOrientation::VERTICAL)
                       .SetRequestedWidth(MATCH_PARENT)
                       .SetRequestedHeight(MATCH_PARENT);
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
    StackLayout listContent = StackLayout::New(StackOrientation::VERTICAL)
                                .SetRequestedWidth(MATCH_PARENT)
                                .SetRequestedHeight(WRAP_CONTENT);

    if(mTestCases.empty())
    {
      listContent.Add(
        Label::New("No test cases registered.")
          .SetTextColor(UiColor(COLOR_ITEM_DESC))
          .SetFontSize(FONT_TC_NAME)
          .SetRequestedWidth(MATCH_PARENT)
          .SetRequestedHeight(WRAP_CONTENT)
          .SetPadding(Extents(PADDING_H, PADDING_H, PADDING_V * 2, PADDING_V * 2)));
    }

    for(std::size_t i = 0; i < mTestCases.size(); ++i)
    {
      listContent.Add(MakeListItem(i));
      listContent.Add(MakeSeparator());
    }

    mRootContainer.Add(
      ScrollView::New()
        .SetScrollDirection(ScrollDirection::Vertical)
        .SetRequestedWidth(MATCH_PARENT)
        .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
        .SetContent(listContent));
  }

  View MakeHeader(const Dali::String& title)
  {
    return StackLayout::New(StackOrientation::HORIZONTAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(HEADER_HEIGHT)
      .SetBackgroundColor(UiColor(COLOR_HEADER_BG))
      .SetPadding(Extents(PADDING_H, PADDING_H, 0, 0))
      .Children({
        Label::New(title)
          .SetTextColor(UiColor(COLOR_HEADER_TEXT))
          .SetFontSize(FONT_HEADER)
          .SetRequestedWidth(MATCH_PARENT)
          .SetRequestedHeight(MATCH_PARENT)
          .SetVerticalTextAlignment(Text::Alignment::CENTER),
      });
  }

  View MakeListItem(std::size_t index)
  {
    const auto& tc = mTestCases[index];

    return StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetBackgroundColor(UiColor(COLOR_ITEM_BG))
      .SetPadding(Extents(PADDING_H, PADDING_H, PADDING_V, PADDING_V))
      .SetFocusable(true)
      .AsInteractive([this, index](InteractiveTrait& trait)
    {
      trait.ClickedSignal().Connect(this, [this, index](View, const InputEvent&) -> bool
      {
        EnterTestCase(index);
        return true;
      });
    }).Children({
        Label::New(tc->GetName()).SetTextColor(UiColor(COLOR_ITEM_NAME)).SetFontSize(FONT_TC_NAME).SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(WRAP_CONTENT),
        Label::New(tc->GetDescription()).SetTextColor(UiColor(COLOR_ITEM_DESC)).SetFontSize(FONT_TC_DESC).SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(WRAP_CONTENT),
      });
  }

  View MakeSeparator()
  {
    return View::New()
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(SEPARATOR_H)
      .SetBackgroundColor(UiColor(COLOR_SEPARATOR));
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
    mRootContainer.Add(
      StackLayout::New(StackOrientation::HORIZONTAL)
        .SetRequestedWidth(MATCH_PARENT)
        .SetRequestedHeight(HEADER_HEIGHT)
        .SetBackgroundColor(UiColor(COLOR_HEADER_BG))
        .Children({
          Label::New("< Back")
            .SetTextColor(UiColor(COLOR_HEADER_TEXT))
            .SetFontSize(FONT_HEADER)
            .SetRequestedWidth(WRAP_CONTENT)
            .SetRequestedHeight(MATCH_PARENT)
            .SetPadding(Extents(PADDING_H, PADDING_H * 2, 0, 0))
            .SetVerticalTextAlignment(Text::Alignment::CENTER)
            .SetFocusable(true)
            .AsInteractive([this](InteractiveTrait& trait)
    {
      trait.ClickedSignal().Connect(this, [this](View, const InputEvent&) -> bool
      {
        BackToList();
        return true;
      });
    }),
          Label::New(mActiveCase->GetName()).SetTextColor(UiColor(COLOR_HEADER_TEXT)).SetFontSize(FONT_HEADER).SetRequestedWidth(MATCH_PARENT).SetRequestedHeight(MATCH_PARENT).SetVerticalTextAlignment(Text::Alignment::CENTER),
        }));

    // ── Content area: TC adds its views here ────────────────────────────────
    StackLayout contentArea = StackLayout::New(StackOrientation::VERTICAL)
                                .SetRequestedWidth(MATCH_PARENT)
                                .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
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

  void OnKeyEvent(const KeyEvent& event)
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
  UiConfig::New().Apply();
  ManualTestLauncher launcher(application);
  application.MainLoop();
  return 0;
}
