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
#include <string>

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
constexpr uint32_t COLOR_SEARCH_TEXT = 0x374151; // Soft black
constexpr uint32_t COLOR_MATCH_TEXT  = 0x2563EB; // Blue

// Sizes (pixels)
constexpr float HEADER_HEIGHT       = 60.0f;
constexpr float SEARCH_HEIGHT = 48.0f;
constexpr float SEPARATOR_H         = 1.0f;
constexpr float PADDING_H           = 20.0f;
constexpr float PADDING_V           = 14.0f;

// Font sizes (pixels)
constexpr float FONT_HEADER  = 22.0f;
constexpr float FONT_SEARCH  = 16.0f;
constexpr float FONT_TC_NAME = 17.0f;
constexpr float FONT_TC_DESC = 13.0f;

struct SearchCache
{
  std::string nameKey;
  std::string descriptionKey;
};

std::string ToStdString(const Dali::String& text)
{
  return text.CStr();
}

std::string MakeSearchKey(const Dali::String& text)
{
  std::string key = ToStdString(text);
  for(char& ch : key)
  {
    if(ch >= 'A' && ch <= 'Z')
    {
      ch = static_cast<char>(ch - 'A' + 'a');
    }
  }
  return key;
}

bool ContainsSearchKey(const std::string& textKey, const std::string& searchKey)
{
  return searchKey.empty() || textKey.find(searchKey) != std::string::npos;
}

Gradient::Linear CreateSearchBackgroundGradient()
{
  Gradient::Linear gradient(Vector2(-0.5f, -0.5f), Vector2(0.5f, 0.5f));
  gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  gradient.SetStopNodes({
    Gradient::StopNode(0.0f, UiColor(0xFFFFFF)),
    Gradient::StopNode(0.52f, UiColor(0xF8FBFF)),
    Gradient::StopNode(1.0f, UiColor(0xF6F1FF)),
  });
  return gradient;
}

Gradient::Linear CreateSearchPlaceholderGradient()
{
  Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  gradient.SetStopNodes({
    Gradient::StopNode(0.00f, UiColor(0x4F7DF3)),
    Gradient::StopNode(0.45f, UiColor(0x8B5CF6)),
    Gradient::StopNode(1.00f, UiColor(0xEC4899)),
  });
  return gradient;
}

Text::StyledText BuildHighlightedText(const Dali::String& text, const std::string& textKey, const std::string& searchKey)
{
  Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(text);
  if(searchKey.empty())
  {
    return builder.Build();
  }

  const Dali::StringView sourceView(text);

  std::size_t position = textKey.find(searchKey);
  while(position != std::string::npos)
  {
    uint32_t utf32Start = 0u;
    uint32_t utf32End   = 0u;
    if(Text::Utf8ToUtf32Range(
         sourceView,
         static_cast<uint32_t>(position),
         static_cast<uint32_t>(position + searchKey.size()),
         utf32Start,
         utf32End))
    {
      builder.SetSpan(Text::ForegroundColorSpan::New(UiColor(COLOR_MATCH_TEXT)), utf32Start, utf32End);
    }
    position = textKey.find(searchKey, position + searchKey.size());
  }
  return builder.Build();
}
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
    mSearchCache.reserve(mTestCases.size());
    for(const auto& tc : mTestCases)
    {
      mSearchCache.push_back({MakeSearchKey(tc->GetName()), MakeSearchKey(tc->GetDescription())});
    }

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
    mRootContainer.Add(MakeSearchField());

    // ── TC list ─────────────────────────────────────────────────────────────
    mListContent = StackLayout::New(StackOrientation::VERTICAL);
    mListContent.SetRequestedWidth(MATCH_PARENT);
    mListContent.SetRequestedHeight(WRAP_CONTENT);
    RefreshListContent();

    ScrollView scrollView = ScrollView::New();
    scrollView.SetScrollDirection(ScrollDirection::Vertical);
    scrollView.SetRequestedWidth(MATCH_PARENT);
    scrollView.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    scrollView.SetContent(mListContent);
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

  View MakeSearchField()
  {
    mSearchField = InputField::New();
    mSearchField.SetRequestedWidth(MATCH_PARENT);
    mSearchField.SetRequestedHeight(SEARCH_HEIGHT);
    mSearchField.SetBackgroundColor(UiColor(0xFFFFFF, 0.72f));
    mSearchField.SetBorderlineWidth(1.0f);
    mSearchField.SetBorderlineColor(UiColor(0xD9E5FF));
    mSearchField.SetMargin(Extents(8.0f, 8.0f, 8.0f, 8.0f));
    mSearchField.SetPadding(Extents(16.0f, 16.0f, 0.0f, 0.0f));
    mSearchField.SetFontSize(FONT_SEARCH);
    mSearchField.SetTextColor(UiColor(COLOR_SEARCH_TEXT));
    mSearchField.SetPlaceholder("What are you looking for?");
    mSearchField.SetPlaceholderTextGradient(CreateSearchPlaceholderGradient());
    mSearchField.SetBackgroundGradient(CreateSearchBackgroundGradient());
    mSearchField.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mSearchField.SetText(mSearchText);
    mSearchField.TextChangedSignal().Connect(this, &ManualTestLauncher::OnSearchTextChanged);
    return mSearchField;
  }

  void RefreshListContent()
  {
    if(!mListContent)
    {
      return;
    }

    mListContent.RemoveAllChildren();
    const std::string searchKey = MakeSearchKey(mSearchText);

    if(mTestCases.empty())
    {
      Label emptyLabel = Label::New("No test cases registered.");
      emptyLabel.SetTextColor(UiColor(COLOR_ITEM_DESC));
      emptyLabel.SetFontSize(FONT_TC_NAME);
      emptyLabel.SetRequestedWidth(MATCH_PARENT);
      emptyLabel.SetRequestedHeight(WRAP_CONTENT);
      emptyLabel.SetPadding(Extents(PADDING_H, PADDING_H, PADDING_V * 2, PADDING_V * 2));
      mListContent.Add(emptyLabel);
      return;
    }

    std::size_t visibleCount = 0u;
    for(std::size_t i = 0; i < mTestCases.size(); ++i)
    {
      const auto& cache = mSearchCache[i];
      if(ContainsSearchKey(cache.nameKey, searchKey) || ContainsSearchKey(cache.descriptionKey, searchKey))
      {
        mListContent.Add(MakeListItem(i, searchKey));
        mListContent.Add(MakeSeparator());
        ++visibleCount;
      }
    }

    if(visibleCount == 0u)
    {
      Label emptyLabel = Label::New("No matching test cases.");
      emptyLabel.SetTextColor(UiColor(COLOR_ITEM_DESC));
      emptyLabel.SetFontSize(FONT_TC_NAME);
      emptyLabel.SetRequestedWidth(MATCH_PARENT);
      emptyLabel.SetRequestedHeight(WRAP_CONTENT);
      emptyLabel.SetPadding(Extents(PADDING_H, PADDING_H, PADDING_V * 2, PADDING_V * 2));
      mListContent.Add(emptyLabel);
    }
  }

  View MakeListItem(std::size_t index, const std::string& searchKey)
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

    if(!searchKey.empty())
    {
      const auto& cache = mSearchCache[index];
      nameLabel.SetStyledText(BuildHighlightedText(tc->GetName(), cache.nameKey, searchKey));
      descriptionLabel.SetStyledText(BuildHighlightedText(tc->GetDescription(), cache.descriptionKey, searchKey));
    }

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

  void OnSearchTextChanged(View)
  {
    mSearchText = mSearchField.GetText();
    RefreshListContent();
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
  StackLayout                                        mListContent;
  InputField                                         mSearchField;
  Dali::String                                       mSearchText;
  std::vector<SearchCache>                           mSearchCache;
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
