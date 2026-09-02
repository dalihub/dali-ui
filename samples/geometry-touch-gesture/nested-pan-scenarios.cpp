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
 *
 */

// EXTERNAL INCLUDES
#include <algorithm>
#include <memory>
#include <sstream>
#include <vector>

// INTERNAL INCLUDES
#include "manual-feed-scroller.h"
#include "sample-common.h"

namespace GeometryTouchGestureSample
{
namespace
{
const Dali::Vector4 ITEM_COLOR(0.10f, 0.17f, 0.34f, 1.0f);
const Dali::Vector4 ITEM_LONG_PRESS_STARTED_COLOR(0.00f, 0.72f, 0.72f, 1.0f);
const Dali::Vector4 ITEM_LONG_PRESS_CANCELLED_COLOR(0.72f, 0.08f, 0.62f, 1.0f);

/**
 * @brief Exercises vertical and horizontal pan detectors nested around item gestures.
 */
class NestedOrthogonalPanScenario : public Scenario
{
public:
  NestedOrthogonalPanScenario(EventLog& eventLog, const Dali::Vector2& canvasSize)
  : Scenario(eventLog, canvasSize),
    mTapCount(0u),
    mLongPressCount(0u)
  {
  }

  ~NestedOrthogonalPanScenario() override = default;

  NestedOrthogonalPanScenario(const NestedOrthogonalPanScenario&)            = delete;
  NestedOrthogonalPanScenario& operator=(const NestedOrthogonalPanScenario&) = delete;

  const char* GetTitle() const override
  {
    return "Nested Orthogonal Pan";
  }

  const char* GetInstructions() const override
  {
    return "Vertical outer scroller contains horizontal rows. Items feed shared Tap/LongPress. "
           "LongPress is cyan while active and magenta when cancelled. A selected pan connects intercept; "
           "watch item INTERRUPTED/CANCELLED.";
  }

  Dali::Ui::View Build() override
  {
    mRoot = Dali::Ui::View::New();
    mRoot.SetRequestedWidth(GetCanvasSize().x);
    mRoot.SetRequestedHeight(GetCanvasSize().y);

    const float outerWidth       = GetCanvasSize().x - 60.0f;
    const float outerHeight      = std::max(300.0f, GetCanvasSize().y - 120.0f);
    const float outerContentSize = outerHeight + 270.0f;
    const float rowWidth         = outerWidth - 40.0f;
    const float rowContentSize   = rowWidth + 540.0f;

    mItemTapDetector       = Dali::TapGestureDetector::New();
    mItemLongPressDetector = Dali::LongPressGestureDetector::New();
    mItemTapDetector.DetectedSignal().Connect(this, &NestedOrthogonalPanScenario::OnItemTap);
    mItemLongPressDetector.DetectedSignal().Connect(this, &NestedOrthogonalPanScenario::OnItemLongPress);

    mOuterScroller = std::make_unique<ManualFeedScroller>(GetEventLog(),
                                                          "Nested.OuterVertical",
                                                          ManualFeedScroller::Direction::VERTICAL,
                                                          outerWidth,
                                                          outerHeight,
                                                          outerContentSize,
                                                          true,
                                                          false);
    mOuterScroller->SetPosition(30.0f, 28.0f);
    mOuterScroller->SetMaximumTouchesRequired(10u);

    const Dali::Vector4 rowColors[4] = {
      Dali::Vector4(0.72f, 0.28f, 0.08f, 1.0f),
      Dali::Vector4(0.08f, 0.30f, 0.72f, 1.0f),
      Dali::Vector4(0.62f, 0.12f, 0.18f, 1.0f),
      Dali::Vector4(0.08f, 0.58f, 0.58f, 1.0f),
    };

    for(uint32_t row = 0u; row < 4u; ++row)
    {
      std::ostringstream rowName;
      rowName << "Nested.Row" << row;
      std::unique_ptr<ManualFeedScroller> rowScroller = std::make_unique<ManualFeedScroller>(GetEventLog(),
                                                                                             rowName.str(),
                                                                                             ManualFeedScroller::Direction::HORIZONTAL,
                                                                                             rowWidth,
                                                                                             150.0f,
                                                                                             rowContentSize,
                                                                                             true,
                                                                                             false);
      rowScroller->SetPosition(20.0f, 20.0f + static_cast<float>(row) * 185.0f);
      rowScroller->SetMaximumTouchesRequired(10u);
      rowScroller->GetContent().SetBackgroundColor(rowColors[row]);

      const uint32_t itemCount = std::max(9u, static_cast<uint32_t>(rowContentSize / 112.0f));
      for(uint32_t column = 0u; column < itemCount; ++column)
      {
        std::ostringstream itemName;
        itemName << "Item." << row << '.' << column;
        Dali::Ui::View item = CreateTestView(itemName.str(), itemName.str(), ITEM_COLOR,
                                             18.0f + static_cast<float>(column) * 112.0f,
                                             28.0f,
                                             92.0f,
                                             94.0f);
        item.TouchEventSignal().Connect(this, &NestedOrthogonalPanScenario::OnItemTouch);
        rowScroller->Add(item);
      }

      mOuterScroller->Add(rowScroller->GetViewport());
      mRowScrollers.push_back(std::move(rowScroller));
    }

    mRoot.Add(mOuterScroller->GetViewport());
    mStatusLabel = CreateLabel("", 30.0f, GetCanvasSize().y - 82.0f, GetCanvasSize().x - 60.0f, 70.0f, 12.0f);
    mRoot.Add(mStatusLabel);
    mOuterScroller->SetOffsetChangedCallback([this](float)
    {
      RefreshStatus();
    });
    RefreshStatus();
    return mRoot;
  }

private:
  bool OnItemTouch(Dali::Actor actor, Dali::TouchEvent touch)
  {
    bool consumed = mItemTapDetector.HandleEvent(actor, touch);
    consumed |= mItemLongPressDetector.HandleEvent(actor, touch);
    GetEventLog().AddTouch("ITEM TOUCH", actor, touch, std::string("Tap|Long return=") + (consumed ? "true" : "false"));
    return consumed;
  }

  void OnItemTap(Dali::Actor actor, Dali::TapGesture tap)
  {
    if(tap.GetState() == Dali::GestureState::STARTED)
    {
      ++mTapCount;
      RefreshStatus();
    }
    GetEventLog().Add("ITEM TAP", ActorName(actor) + " state=" + GestureStateToString(tap.GetState()));
  }

  void OnItemLongPress(Dali::Actor actor, Dali::LongPressGesture longPress)
  {
    if(longPress.GetState() == Dali::GestureState::STARTED)
    {
      ++mLongPressCount;
      RefreshStatus();
    }
    const char* visualState = UpdateItemLongPressVisual(actor, longPress.GetState());
    GetEventLog().Add("ITEM LONG", ActorName(actor) + " state=" + GestureStateToString(longPress.GetState()) + visualState);
  }

  const char* UpdateItemLongPressVisual(Dali::Actor actor, Dali::GestureState state)
  {
    Dali::Ui::View item = Dali::Ui::View::DownCast(actor);
    if(!item)
    {
      return "";
    }

    if(state == Dali::GestureState::STARTED)
    {
      item.SetBackgroundColor(ITEM_LONG_PRESS_STARTED_COLOR);
      return " color=cyan";
    }
    if(state == Dali::GestureState::CANCELLED)
    {
      item.SetBackgroundColor(ITEM_LONG_PRESS_CANCELLED_COLOR);
      return " color=magenta";
    }
    if(state == Dali::GestureState::FINISHED)
    {
      item.SetBackgroundColor(ITEM_COLOR);
      return " color=restored";
    }
    return "";
  }

  void RefreshStatus()
  {
    std::ostringstream stream;
    stream << "Outer offset=" << (mOuterScroller ? mOuterScroller->GetOffset() : 0.0f)
           << " | Item taps=" << mTapCount << " LongPress starts=" << mLongPressCount
           << " | LongPress cyan=active magenta=cancelled | Horizontal row offsets:";
    for(const std::unique_ptr<ManualFeedScroller>& row : mRowScrollers)
    {
      stream << ' ' << row->GetOffset();
    }
    mStatusLabel.SetText(stream.str().c_str());
  }

private:
  Dali::Ui::View                                   mRoot;
  Dali::Ui::Label                                  mStatusLabel;
  Dali::TapGestureDetector                         mItemTapDetector;
  Dali::LongPressGestureDetector                   mItemLongPressDetector;
  std::unique_ptr<ManualFeedScroller>              mOuterScroller;
  std::vector<std::unique_ptr<ManualFeedScroller>> mRowScrollers;
  uint32_t                                         mTapCount;
  uint32_t                                         mLongPressCount;
};

/**
 * @brief Demonstrates same-direction pan propagation from an inner boundary to its parent.
 */
class SameDirectionHandoffScenario : public Scenario
{
public:
  SameDirectionHandoffScenario(EventLog& eventLog, const Dali::Vector2& canvasSize)
  : Scenario(eventLog, canvasSize)
  {
  }

  ~SameDirectionHandoffScenario() override = default;

  SameDirectionHandoffScenario(const SameDirectionHandoffScenario&)            = delete;
  SameDirectionHandoffScenario& operator=(const SameDirectionHandoffScenario&) = delete;

  const char* GetTitle() const override
  {
    return "Same-direction Pan Handoff";
  }

  const char* GetInstructions() const override
  {
    return "Both scrollers are vertical. Start a new drag while the inner is at its movement boundary: "
           "inner sets NeedGesturePropagation and the outer detector receives the same stream.";
  }

  Dali::Ui::View Build() override
  {
    mRoot = Dali::Ui::View::New();
    mRoot.SetRequestedWidth(GetCanvasSize().x);
    mRoot.SetRequestedHeight(GetCanvasSize().y);

    const float    outerWidth       = GetCanvasSize().x - 60.0f;
    const float    outerHeight      = std::max(300.0f, GetCanvasSize().y - 110.0f);
    const float    outerContentSize = outerHeight + 540.0f;
    const float    innerWidth       = outerWidth - 60.0f;
    const float    innerHeight      = std::max(180.0f, outerHeight - 160.0f);
    const float    innerContentSize = innerHeight + 400.0f;
    const float    outerCardStartY  = 140.0f + innerHeight;
    const uint32_t innerItemCount   = std::max(7u, static_cast<uint32_t>(innerContentSize / 104.0f));

    mOuterScroller = std::make_unique<ManualFeedScroller>(GetEventLog(),
                                                          "Handoff.OuterVertical",
                                                          ManualFeedScroller::Direction::VERTICAL,
                                                          outerWidth,
                                                          outerHeight,
                                                          outerContentSize,
                                                          false,
                                                          false);
    mOuterScroller->SetPosition(30.0f, 24.0f);
    mOuterScroller->GetContent().SetBackgroundColor(Dali::Vector4(0.46f, 0.20f, 0.08f, 1.0f));

    Dali::Ui::Label outerHeader = CreateLabel("OUTER VERTICAL (starts at offset -220)", 20.0f, 8.0f, outerWidth - 40.0f, 54.0f, 14.0f);
    mOuterScroller->Add(outerHeader);

    mInnerScroller = std::make_unique<ManualFeedScroller>(GetEventLog(),
                                                          "Handoff.InnerVertical",
                                                          ManualFeedScroller::Direction::VERTICAL,
                                                          innerWidth,
                                                          innerHeight,
                                                          innerContentSize,
                                                          false,
                                                          true);
    mInnerScroller->SetPosition(30.0f, 100.0f);
    mInnerScroller->GetContent().SetBackgroundColor(Dali::Vector4(0.08f, 0.34f, 0.64f, 1.0f));

    for(uint32_t index = 0u; index < innerItemCount; ++index)
    {
      std::ostringstream itemName;
      itemName << "InnerCard" << index;
      Dali::Ui::View item = CreateTestView(itemName.str(), itemName.str(), Dali::Vector4(0.12f, 0.52f, 0.68f, 1.0f),
                                           30.0f,
                                           20.0f + static_cast<float>(index) * 104.0f,
                                           innerWidth - 60.0f,
                                           88.0f);
      mInnerScroller->Add(item);
    }
    mOuterScroller->Add(mInnerScroller->GetViewport());

    for(uint32_t index = 0u; index < 4u; ++index)
    {
      std::ostringstream itemName;
      itemName << "OuterCard" << index;
      Dali::Ui::View item = CreateTestView(itemName.str(), itemName.str(), Dali::Vector4(0.66f, 0.30f, 0.10f, 1.0f),
                                           45.0f,
                                           outerCardStartY + static_cast<float>(index) * 128.0f,
                                           outerWidth - 90.0f,
                                           108.0f);
      mOuterScroller->Add(item);
    }

    mRoot.Add(mOuterScroller->GetViewport());
    mStatusLabel = CreateLabel("", 30.0f, GetCanvasSize().y - 78.0f, GetCanvasSize().x - 60.0f, 66.0f, 13.0f);
    mRoot.Add(mStatusLabel);

    mOuterScroller->SetOffsetChangedCallback([this](float)
    {
      RefreshStatus();
    });
    mInnerScroller->SetOffsetChangedCallback([this](float)
    {
      RefreshStatus();
    });
    mOuterScroller->SetOffset(-220.0f);
    mInnerScroller->SetOffset(0.0f);
    RefreshStatus();
    return mRoot;
  }

private:
  void RefreshStatus()
  {
    std::ostringstream stream;
    stream << "Outer offset=" << (mOuterScroller ? mOuterScroller->GetOffset() : 0.0f)
           << " (min " << (mOuterScroller ? mOuterScroller->GetMinimumOffset() : 0.0f) << ')'
           << " | Inner offset=" << (mInnerScroller ? mInnerScroller->GetOffset() : 0.0f)
           << " (min " << (mInnerScroller ? mInnerScroller->GetMinimumOffset() : 0.0f) << ")\n"
           << "At inner top, drag downward. At inner bottom, start a new upward drag. Look for propagate-to-parent=true.";
    mStatusLabel.SetText(stream.str().c_str());
  }

private:
  Dali::Ui::View                      mRoot;
  Dali::Ui::Label                     mStatusLabel;
  std::unique_ptr<ManualFeedScroller> mOuterScroller;
  std::unique_ptr<ManualFeedScroller> mInnerScroller;
};

} // unnamed namespace

std::unique_ptr<Scenario> CreateNestedOrthogonalPanScenario(EventLog& eventLog, const Dali::Vector2& canvasSize)
{
  return std::make_unique<NestedOrthogonalPanScenario>(eventLog, canvasSize);
}

std::unique_ptr<Scenario> CreateSameDirectionHandoffScenario(EventLog& eventLog, const Dali::Vector2& canvasSize)
{
  return std::make_unique<SameDirectionHandoffScenario>(eventLog, canvasSize);
}

} // namespace GeometryTouchGestureSample
