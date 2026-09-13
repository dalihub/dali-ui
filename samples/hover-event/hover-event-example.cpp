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

#include <dali-ui-foundation/dali-ui-foundation.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <iomanip>
#include <sstream>
#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float CARD_WIDTH  = 300.0f;
constexpr float CARD_HEIGHT = 210.0f;

const std::array<const char*, 3u> VIEW_NAMES  = {"A", "B", "C"};
const std::array<UiColor, 3u>     IDLE_COLORS = {
  UiColor(0x365B8C),
  UiColor(0x39725B),
  UiColor(0x7A4D7E),
};
const std::array<UiColor, 3u> HOVER_COLORS = {
  UiColor(0x4D8FE8),
  UiColor(0x4DBD85),
  UiColor(0xC16BCC),
};
const std::array<bool, 3u> CONSUME_HOVER = {true, false, true};

const char* PointStateName(PointState::Type state)
{
  switch(state)
  {
    case PointState::STARTED:
      return "STARTED";
    case PointState::FINISHED:
      return "FINISHED";
    case PointState::MOTION:
      return "MOTION";
    case PointState::LEAVE:
      return "LEAVE";
    case PointState::STATIONARY:
      return "STATIONARY";
    case PointState::INTERRUPTED:
      return "INTERRUPTED";
  }
  return "UNKNOWN";
}

Label MakeLabel(const char* text, float fontSize, UiColor color)
{
  Label label = Label::New(text);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(MATCH_PARENT);
  label.SetFontSize(fontSize);
  label.SetTextColor(color);
  label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  label.SetMultiLine(true);
  label.SetSensitive(false);
  return label;
}
} // unnamed namespace

class HoverEventController : public ConnectionTracker
{
public:
  explicit HoverEventController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &HoverEventController::OnInit);
  }

private:
  struct HoverState
  {
    bool             hovered{false};
    uint32_t         changeCount{0u};
    uint32_t         rawEventCount{0u};
    PointState::Type lastPointState{PointState::FINISHED};
    Vector2          lastScreenPosition{Vector2::ZERO};
  };

  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x111820));
    window.KeyEventSignal().Connect(this, &HoverEventController::OnKeyEvent);

    AbsoluteLayout root = AbsoluteLayout::New();
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);

    Label title = MakeLabel("Hover Event: overlapping A / B / C views", 24.0f, UiColor(0xFFFFFF));
    title.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(30.0f, 16.0f, 820.0f, 42.0f)));
    root.Add(title);

    Label guide = MakeLabel("GEOMETRY order: C > B > A. A/C return true; B returns false and continues to the next hit candidate.",
                            14.0f,
                            UiColor(0xB9C9DA));
    guide.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(30.0f, 58.0f, 820.0f, 38.0f)));
    root.Add(guide);

    // Added in A, B, C order so the overlap hit-test order is C, then B, then A.
    AddHoverView(root, 0u, 70.0f, 115.0f);
    AddHoverView(root, 1u, 245.0f, 180.0f);
    AddHoverView(root, 2u, 420.0f, 245.0f);

    mSummary = MakeLabel("", 14.0f, UiColor(0xFFFFFF));
    mSummary.SetBackgroundColor(UiColor(0x202B38));
    mSummary.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(45.0f, 480.0f, 760.0f, 180.0f)));
    root.Add(mSummary);

    UpdateLabels();
    window.Add(root);
  }

  void AddHoverView(AbsoluteLayout root, std::size_t index, float x, float y)
  {
    View card = View::New();
    card.SetName(VIEW_NAMES[index]);
    card.SetBackgroundColor(IDLE_COLORS[index]);
    card.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(x, y, CARD_WIDTH, CARD_HEIGHT)));

    Label label = MakeLabel("", 22.0f, UiColor(0xFFFFFF));
    label.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(0.0f, 0.0f, CARD_WIDTH, CARD_HEIGHT)));
    card.Add(label);

    card.HoverEventSignal().Connect(this, [this, index](Actor, const HoverEvent& event)
    {
      return OnRawHoverEvent(index, event);
    });

    mViews[index]  = card;
    mLabels[index] = label;
    root.Add(card);
  }

  bool OnRawHoverEvent(std::size_t index, const HoverEvent& event)
  {
    if(event.GetPointCount() == 0u)
    {
      return false;
    }

    HoverState& state        = mStates[index];
    state.lastPointState     = event.GetState(0u);
    state.lastScreenPosition = event.GetScreenPosition(0u);
    ++state.rawEventCount;

    bool hovered = state.hovered;
    if(state.lastPointState == PointState::STARTED)
    {
      hovered = true;
    }
    else if(state.lastPointState == PointState::FINISHED ||
            state.lastPointState == PointState::LEAVE ||
            state.lastPointState == PointState::INTERRUPTED)
    {
      hovered = false;
    }
    if(state.hovered != hovered)
    {
      state.hovered = hovered;
      ++state.changeCount;
      mViews[index].SetBackgroundColor(hovered ? HOVER_COLORS[index] : IDLE_COLORS[index]);
    }

    std::ostringstream entry;
    entry << '#' << ++mEventSequence << ' ' << VIEW_NAMES[index] << ' ' << PointStateName(state.lastPointState)
          << " return " << (CONSUME_HOVER[index] ? "true" : "false");
    mRecentEvents.push_back(entry.str());
    if(mRecentEvents.size() > MAX_RECENT_EVENTS)
    {
      mRecentEvents.pop_front();
    }

    UpdateLabels();
    return CONSUME_HOVER[index];
  }

  void UpdateLabels()
  {
    for(std::size_t index = 0u; index < mStates.size(); ++index)
    {
      const HoverState&  state = mStates[index];
      std::ostringstream text;
      text << "VIEW " << VIEW_NAMES[index]
           << "\n\nHOVER: " << (state.hovered ? "ON" : "OFF")
           << "\nconsume: " << (CONSUME_HOVER[index] ? "true" : "false")
           << "\nraw: " << (state.rawEventCount == 0u ? "NONE" : PointStateName(state.lastPointState))
           << " (" << state.rawEventCount << ')'
           << "\nhover changes: " << state.changeCount;
      mLabels[index].SetText(text.str().c_str());
    }

    std::size_t        hoveredCount = 0u;
    std::ostringstream summary;
    summary << "Current state   ";
    for(std::size_t index = 0u; index < mStates.size(); ++index)
    {
      hoveredCount += mStates[index].hovered ? 1u : 0u;
      summary << VIEW_NAMES[index] << ": " << (mStates[index].hovered ? "ON " : "OFF");
      if(index + 1u < mStates.size())
      {
        summary << "   |   ";
      }
    }

    summary << "\nLast raw event  ";
    for(std::size_t index = 0u; index < mStates.size(); ++index)
    {
      const HoverState& state = mStates[index];
      summary << VIEW_NAMES[index] << ": ";
      if(state.rawEventCount == 0u)
      {
        summary << "NONE";
      }
      else
      {
        summary << PointStateName(state.lastPointState)
                << " @ (" << std::fixed << std::setprecision(0)
                << state.lastScreenPosition.x << ", " << state.lastScreenPosition.y << ")";
      }
      if(index + 1u < mStates.size())
      {
        summary << "   |   ";
      }
    }

    summary << "\nRecent raw      ";
    if(mRecentEvents.empty())
    {
      summary << "NONE";
    }
    else
    {
      bool first = true;
      for(const std::string& entry : mRecentEvents)
      {
        if(!first)
        {
          summary << " -> ";
        }
        summary << entry;
        first = false;
      }
    }

    summary << "\nVisited prefix  ";
    if(hoveredCount > 1u)
    {
      summary << hoveredCount << " active targets (expected when a front candidate returns false)";
    }
    else
    {
      summary << hoveredCount << " active target" << (hoveredCount == 1u ? "" : "s");
    }
    mSummary.SetText(summary.str().c_str());
  }

  void OnKeyEvent(Window, KeyEvent event)
  {
    if(event.GetState() == KeyEvent::DOWN &&
       (IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK)))
    {
      mApplication.Quit();
    }
  }

private:
  static constexpr std::size_t MAX_RECENT_EVENTS = 6u;

  Application&              mApplication;
  std::array<View, 3>       mViews;
  std::array<Label, 3>      mLabels;
  std::array<HoverState, 3> mStates;
  std::deque<std::string>   mRecentEvents;
  uint32_t                  mEventSequence{0u};
  Label                     mSummary;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  HoverEventController controller(application);
  application.MainLoop();
  return 0;
}
