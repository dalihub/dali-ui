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

#include <dali-ui-components/dali-ui-components.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
namespace UiAccessibility = Dali::Ui::Accessibility;

constexpr uint32_t COLOR_HEADER_BG   = 0x1565C0;
constexpr uint32_t COLOR_HEADER_TEXT = 0xFFFFFF;
constexpr uint32_t COLOR_ITEM_BG     = 0xFFFFFF;
constexpr uint32_t COLOR_ITEM_NAME   = 0x212121;
constexpr uint32_t COLOR_ITEM_DESC   = 0x757575;
constexpr uint32_t COLOR_SEPARATOR   = 0xE0E0E0;
constexpr uint32_t COLOR_APP_BG      = 0xF5F5F5;

constexpr float HEADER_HEIGHT = 60.0f;
constexpr float SEPARATOR_H   = 1.0f;
constexpr float PADDING_H     = 20.0f;
constexpr float PADDING_V     = 14.0f;

constexpr float FONT_HEADER  = 22.0f;
constexpr float FONT_TC_NAME = 17.0f;
constexpr float FONT_TC_DESC = 13.0f;
} // namespace

class ManualTestLauncher : public ConnectionTracker
{
public:
  explicit ManualTestLauncher(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &ManualTestLauncher::OnCreate);
  }

private:
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

  void ShowListScreen()
  {
    mRootContainer.RemoveAllChildren();
    mActiveCase = nullptr;

    mRootContainer.Add(MakeHeader("컴포넌트 수동 테스트"));

    StackLayout listContent = StackLayout::New(StackOrientation::VERTICAL);
    listContent.SetRequestedWidth(MATCH_PARENT);
    listContent.SetRequestedHeight(WRAP_CONTENT);

    // Mark this as a list container in the tree to preserve the logical order of its items.
    // The container provides structural information only, not a separate announcement, so disable highlighting.
    listContent.SetAccessibilityRole(UiAccessibility::Role::LIST);
    listContent.SetAccessibilityCollectionContainer(true);
    listContent.SetAccessibilityHighlightable(false);

    if(mTestCases.empty())
    {
      Label emptyLabel = Label::New("등록된 테스트 항목이 없습니다.");
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

    // The HEADER role conveys the screen context more accurately than Label's default TEXT role.
    titleLabel.SetAccessibilityRole(UiAccessibility::Role::HEADER);

    // When the new list screen enters the accessibility tree, designate its contextual title
    // as the initial navigation target instead of moving the highlight after an arbitrary timeout.
    titleLabel.SetRequestInitialAccessibilityHighlight(true);

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

    // The two Labels below are visual children of the list action root. The root represents
    // their name and description, so hide the children to prevent duplicate announcements.
    nameLabel.SetAccessibilityHidden(true);

    Label descriptionLabel = Label::New(tc->GetDescription());
    descriptionLabel.SetTextColor(UiColor(COLOR_ITEM_DESC));
    descriptionLabel.SetFontSize(FONT_TC_DESC);
    descriptionLabel.SetRequestedWidth(MATCH_PARENT);
    descriptionLabel.SetRequestedHeight(WRAP_CONTENT);
    descriptionLabel.SetAccessibilityHidden(true);

    StackLayout item = StackLayout::New(StackOrientation::VERTICAL);
    item.SetRequestedWidth(MATCH_PARENT);
    item.SetRequestedHeight(WRAP_CONTENT);
    item.SetBackgroundColor(UiColor(COLOR_ITEM_BG));
    item.SetPadding(Extents(PADDING_H, PADDING_H, PADDING_V, PADDING_V));
    item.SetStateEffect(OverlayEffect::ListItem());

    // Setting focusable alone does not make an item available to Screen Reader. Each row opens
    // a TC screen, so give its root the BUTTON role plus an identifying name and description.
    // The collection index keeps AT-SPI order stable when the screen layout changes.
    item.SetAccessibilityRole(UiAccessibility::Role::BUTTON);
    item.SetAccessibilityName(tc->GetName());
    item.SetAccessibilityDescription(tc->GetDescription());
    item.SetAccessibilityCollectionIndex(static_cast<int32_t>(index));

    // In the new framework, default accessibility activation on a View with an interactive
    // trait checks enabled/clickable and then emits the same ClickedSignal. Therefore touch,
    // remote Enter, and Screen Reader double tap share one path without a dedicated
    // accessibility-only ViewImpl/OnAccessibilityActivate implementation.
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

    // The separator is purely decorative, so exclude it from the accessibility tree.
    separator.SetAccessibilityHidden(true);
    return separator;
  }

  void EnterTestCase(std::size_t index)
  {
    if(index >= mTestCases.size())
    {
      return;
    }

    mActiveCase = mTestCases[index].get();
    mRootContainer.RemoveAllChildren();

    Label backLabel = Label::New("< 뒤로");
    backLabel.SetTextColor(UiColor(COLOR_HEADER_TEXT));
    backLabel.SetFontSize(FONT_HEADER);
    backLabel.SetRequestedWidth(WRAP_CONTENT);
    backLabel.SetRequestedHeight(MATCH_PARENT);
    backLabel.SetPadding(Extents(PADDING_H, PADDING_H * 2, 0, 0));
    backLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    backLabel.SetStateEffect(OverlayEffect::ListItem());

    // The visible '<' is a directional decoration, so do not include it in the announced
    // name. Using the Label itself as the BUTTON action root with an explicit name takes
    // precedence over displayed-text fallback. AsInteractive()'s default accessibility
    // activation passes through to the ClickedSignal below.
    backLabel.SetAccessibilityRole(UiAccessibility::Role::BUTTON);
    backLabel.SetAccessibilityName("뒤로");
    backLabel.SetAccessibilityDescription("수동 테스트 목록으로 돌아갑니다.");

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
    titleLabel.SetAccessibilityRole(UiAccessibility::Role::HEADER);
    titleLabel.SetRequestInitialAccessibilityHighlight(true);

    StackLayout header = StackLayout::New(StackOrientation::HORIZONTAL);
    header.SetRequestedWidth(MATCH_PARENT);
    header.SetRequestedHeight(HEADER_HEIGHT);
    header.SetBackgroundColor(UiColor(COLOR_HEADER_BG));
    header.Add(backLabel);
    header.Add(titleLabel);
    mRootContainer.Add(header);

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

  void OnKeyEvent(Window, KeyEvent event)
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

  Application&                                       mApplication;
  StackLayout                                        mRootContainer;
  std::vector<std::unique_ptr<ManualTest::TestCase>> mTestCases;
  ManualTest::TestCase*                              mActiveCase{nullptr};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  Components::UiConfig config = Components::UiConfig::New();
  config.SetDefaultStateEffectForInteractive(OverlayEffect::Plain());
  config.Apply();
  ManualTestLauncher launcher(application);
  application.MainLoop();
  return 0;
}
