/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include <array>
#include <deque>
#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr uint32_t COLOR_BACKGROUND = 0xF4F6F8;
constexpr uint32_t COLOR_ITEM = 0xDCE6F2;
constexpr uint32_t COLOR_HOVERED = 0x4CAF50;
constexpr uint32_t COLOR_TEXT = 0x263238;
constexpr uint32_t COLOR_SUB_TEXT = 0x546E7A;
constexpr uint32_t COLOR_WHITE = 0xFFFFFF;
constexpr float PADDING = 16.0f;
constexpr float SPACING = 8.0f;
constexpr float ITEM_HEIGHT = 96.0f;
constexpr float STATUS_HEIGHT = 84.0f;
constexpr float LOG_HEIGHT = 124.0f;
constexpr float FONT_BODY = 15.0f;
constexpr float FONT_ITEM = 19.0f;
constexpr uint32_t ITEM_COUNT = 10u;

Dali::String Number(uint32_t value)
{
  return Dali::String(std::to_string(value).c_str());
}

} // namespace

/**
 * @brief Verifies hover retargeting while ScrollView moves under a stationary pointer.
 */
class TcScrollHoverRetarget : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "Scroll Hover Retarget";
  }

  Dali::String GetDescription() const override
  {
    return "Checks HOVERED state transitions while scrolling items beneath a stationary pointer";
  }

  void OnEnter(View contentArea) override
  {
    ResetState();

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetBackgroundColor(UiColor(COLOR_BACKGROUND));
    root.SetPadding(Insets(PADDING, PADDING, PADDING, PADDING));
    root.SetSpacing(SPACING);

    Label instructions = Label::New("Hover one row, keep the pointer still, then scroll manually with the mouse wheel or touchpad.");
    instructions.SetMultiLine(true);
    instructions.SetFontSize(FONT_BODY);
    instructions.SetTextColor(UiColor(COLOR_SUB_TEXT));
    instructions.SetRequestedWidth(MATCH_PARENT);
    instructions.SetRequestedHeight(WRAP_CONTENT);
    root.Add(instructions);

    mStatus = Label::New();
    mStatus.SetMultiLine(true);
    mStatus.SetFontSize(FONT_BODY);
    mStatus.SetTextColor(UiColor(COLOR_TEXT));
    mStatus.SetRequestedWidth(MATCH_PARENT);
    mStatus.SetRequestedHeight(STATUS_HEIGHT);
    mStatus.SetBackgroundColor(UiColor(COLOR_WHITE));
    mStatus.SetPadding(Insets(10.0f, 10.0f, 8.0f, 8.0f));
    root.Add(mStatus);

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(ITEM_COUNT * ITEM_HEIGHT + (ITEM_COUNT - 1u) * SPACING);
    content.SetSpacing(SPACING);

    for(uint32_t index = 0u; index < ITEM_COUNT; ++index)
    {
      content.Add(CreateItem(index));
    }

    mScrollView = ScrollView::New();
    mScrollView.SetScrollDirection(ScrollDirection::Vertical);
    mScrollView.SetRequestedWidth(MATCH_PARENT);
    mScrollView.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    mScrollView.SetContent(content);
    root.Add(mScrollView);

    mLog = Label::New();
    mLog.SetMultiLine(true);
    mLog.SetFontSize(FONT_BODY);
    mLog.SetTextColor(UiColor(COLOR_TEXT));
    mLog.SetRequestedWidth(MATCH_PARENT);
    mLog.SetRequestedHeight(LOG_HEIGHT);
    mLog.SetBackgroundColor(UiColor(COLOR_WHITE));
    mLog.SetPadding(Insets(10.0f, 10.0f, 8.0f, 8.0f));
    root.Add(mLog);

    UpdateStatus();
    UpdateLog();
    contentArea.Add(root);
  }

private:
  View CreateItem(uint32_t index)
  {
    InteractiveView item = InteractiveView::New();
    item.SetRequestedWidth(MATCH_PARENT);
    item.SetRequestedHeight(ITEM_HEIGHT);
    item.SetBackgroundColor(UiColor(COLOR_ITEM));
    item.SetPadding(Insets(16.0f, 16.0f, 8.0f, 8.0f));

    Label label = Label::New(Dali::String("Item ") + Number(index + 1u));
    label.SetFontSize(FONT_ITEM);
    label.SetTextColor(UiColor(COLOR_TEXT));
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(MATCH_PARENT);
    item.Add(label);

    item.StateChangedSignal().Connect(this, [this, index](View, StateEvent event)
    {
      OnItemStateChanged(index, event);
    });

    mItems[index] = item;
    return item;
  }

  void OnItemStateChanged(uint32_t index, StateEvent event)
  {
    if(event.Added(ViewState::HOVERED))
    {
      mItems[index].SetBackgroundColor(UiColor(COLOR_HOVERED));
      if(mObservationStarted)
      {
        ++mHoveredAddedDuringScroll;
        if(mActiveItem >= 0)
        {
          ++mOrderingFailures;
        }
      }
      mActiveItem = static_cast<int32_t>(index);

      if(!mObservationStarted)
      {
        mObservationStarted = true;
        AppendLog(Dali::String("Item ") + Number(index + 1u) + " HOVERED added; scroll manually");
      }
      else
      {
        AppendLog(Dali::String("Item ") + Number(index + 1u) + " HOVERED added");
      }
    }
    else if(event.Removed(ViewState::HOVERED))
    {
      mItems[index].SetBackgroundColor(UiColor(COLOR_ITEM));
      if(mObservationStarted)
      {
        ++mHoveredRemovedDuringScroll;
      }
      if(mActiveItem == static_cast<int32_t>(index))
      {
        mActiveItem = -1;
      }
      AppendLog(Dali::String("Item ") + Number(index + 1u) + " HOVERED removed");
    }
    else
    {
      return;
    }

    UpdateStatus();
  }

  void UpdateStatus()
  {
    Dali::String result = "TESTING";
    if(!mObservationStarted)
    {
      result = "WAITING FOR HOVER";
    }
    else if(mOrderingFailures > 0u)
    {
      result = "FAIL";
    }
    else if(mHoveredAddedDuringScroll > 0u && mHoveredRemovedDuringScroll > 0u)
    {
      result = "PASS";
    }

    mStatus.SetText(
      Dali::String("Result: ") + result +
      "\nDuring scroll: HOVERED added=" + Number(mHoveredAddedDuringScroll) +
      ", removed=" + Number(mHoveredRemovedDuringScroll) +
      ", ordering failures=" + Number(mOrderingFailures));
  }

  void AppendLog(const Dali::String& line)
  {
    constexpr std::size_t MAX_LOG_LINES = 6u;
    mLogLines.push_back(line);
    if(mLogLines.size() > MAX_LOG_LINES)
    {
      mLogLines.pop_front();
    }
    UpdateLog();
  }

  void UpdateLog()
  {
    Dali::String text = "Recent events:\n";
    for(const auto& line : mLogLines)
    {
      text += line + "\n";
    }
    mLog.SetText(text);
  }

  void ResetState()
  {
    mActiveItem = -1;
    mHoveredAddedDuringScroll = 0u;
    mHoveredRemovedDuringScroll = 0u;
    mOrderingFailures = 0u;
    mObservationStarted = false;
    mLogLines.clear();
  }

private:
  ScrollView mScrollView;
  Label mStatus;
  Label mLog;
  std::array<InteractiveView, ITEM_COUNT> mItems;
  std::deque<Dali::String> mLogLines;
  int32_t mActiveItem{-1};
  uint32_t mHoveredAddedDuringScroll{0u};
  uint32_t mHoveredRemovedDuringScroll{0u};
  uint32_t mOrderingFailures{0u};
  bool mObservationStarted{false};
};

REGISTER_MANUAL_TEST(TcScrollHoverRetarget)
