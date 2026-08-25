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

// INTERNAL INCLUDES
#include "manual-feed-scroller.h"
#include "sample-common.h"

namespace GeometryTouchGestureSample
{
namespace
{
const Dali::Vector4 RED_ACTOR_COLOR(0.72f, 0.12f, 0.12f, 1.0f);
const Dali::Vector4 BLUE_ACTOR_COLOR(0.08f, 0.24f, 0.76f, 1.0f);
const Dali::Vector4 LONG_PRESS_STARTED_COLOR(0.00f, 0.72f, 0.72f, 1.0f);
const Dali::Vector4 LONG_PRESS_CANCELLED_COLOR(0.72f, 0.08f, 0.62f, 1.0f);

/**
 * @brief Compares shared gesture detector handles with independent detector sets.
 */
class SharedDetectorScenario : public Scenario
{
public:
  SharedDetectorScenario(EventLog& eventLog, const Dali::Vector2& canvasSize)
  : Scenario(eventLog, canvasSize)
  {
  }

  ~SharedDetectorScenario() override = default;

  SharedDetectorScenario(const SharedDetectorScenario&)            = delete;
  SharedDetectorScenario& operator=(const SharedDetectorScenario&) = delete;

  const char* GetTitle() const override
  {
    return "Shared vs Independent Detectors";
  }

  const char* GetInstructions() const override
  {
    return "Left: parent and child feed the same Tap/Pan/Long detector. Right: separate Pan/Long detectors; "
           "dragging moves the detected Actor. LongPress is cyan while active and magenta when cancelled. "
           "Blue pan cancels competing detectors.";
  }

  Dali::Ui::View Build() override
  {
    mRoot = Dali::Ui::View::New();
    mRoot.SetRequestedWidth(GetCanvasSize().x);
    mRoot.SetRequestedHeight(GetCanvasSize().y);

    const float halfWidth   = GetCanvasSize().x * 0.5f;
    const float panelWidth  = halfWidth - 36.0f;
    const float panelHeight = std::max(120.0f, GetCanvasSize().y - 220.0f);

    mSharedTap  = Dali::TapGestureDetector::New();
    mSharedPan  = Dali::PanGestureDetector::New();
    mSharedLong = Dali::LongPressGestureDetector::New();
    mSharedPan.SetMaximumTouchesRequired(2u);
    mSharedTap.DetectedSignal().Connect(this, &SharedDetectorScenario::OnTap);
    mSharedPan.DetectedSignal().Connect(this, &SharedDetectorScenario::OnPan);
    mSharedLong.DetectedSignal().Connect(this, &SharedDetectorScenario::OnLongPress);

    mRedPan   = Dali::PanGestureDetector::New();
    mBluePan  = Dali::PanGestureDetector::New();
    mRedLong  = Dali::LongPressGestureDetector::New();
    mBlueLong = Dali::LongPressGestureDetector::New();
    mRedPan.SetMaximumTouchesRequired(2u);
    mBluePan.SetMaximumTouchesRequired(2u);
    mRedPan.DetectedSignal().Connect(this, &SharedDetectorScenario::OnIndependentPan);
    mBluePan.DetectedSignal().Connect(this, &SharedDetectorScenario::OnIndependentPan);
    mRedLong.DetectedSignal().Connect(this, &SharedDetectorScenario::OnIndependentLongPress);
    mBlueLong.DetectedSignal().Connect(this, &SharedDetectorScenario::OnIndependentLongPress);

    Dali::Ui::Label sharedTitle      = CreateLabel("ONE SHARED DETECTOR SET", 12.0f, 6.0f, halfWidth - 24.0f, 32.0f, 13.0f);
    Dali::Ui::Label independentTitle = CreateLabel("INDEPENDENT DETECTOR SETS", halfWidth + 12.0f, 6.0f, halfWidth - 24.0f, 32.0f, 13.0f);
    mRoot.Add(sharedTitle);
    mRoot.Add(independentTitle);

    mSharedRed  = CreateTestView("Shared.Red", "RED parent\nTap + Pan + Long", RED_ACTOR_COLOR, 18.0f, 46.0f, panelWidth, panelHeight);
    mSharedBlue = CreateTestView("Shared.Blue", "BLUE child\nsame detector handles", BLUE_ACTOR_COLOR, 34.0f, 82.0f, panelWidth - 74.0f, std::max(100.0f, panelHeight - 180.0f));
    mSharedRed.Add(mSharedBlue);
    mRoot.Add(mSharedRed);

    mIndependentRed  = CreateTestView("Independent.Red", "RED parent\nindependent Pan + Long", RED_ACTOR_COLOR, halfWidth + 18.0f, 46.0f, panelWidth, panelHeight);
    mIndependentBlue = CreateTestView("Independent.Blue", "BLUE child\nPan cancels others", BLUE_ACTOR_COLOR, 30.0f, 82.0f, panelWidth - 74.0f, std::max(100.0f, panelHeight - 180.0f));
    mBlocker         = CreateTestView("Independent.Blocker", "BLOCKER\nreturn true", Dali::Vector4(0.55f, 0.32f, 0.16f, 1.0f), panelWidth - 128.0f, panelHeight - 214.0f, 118.0f, 96.0f);
    mIndependentRed.Add(mIndependentBlue);
    mIndependentRed.Add(mBlocker);
    mRoot.Add(mIndependentRed);

    mSharedRed.TouchEventSignal().Connect(this, &SharedDetectorScenario::OnSharedTouch);
    mSharedBlue.TouchEventSignal().Connect(this, &SharedDetectorScenario::OnSharedTouch);
    mIndependentRed.TouchEventSignal().Connect(this, &SharedDetectorScenario::OnIndependentTouch);
    mIndependentBlue.TouchEventSignal().Connect(this, &SharedDetectorScenario::OnIndependentTouch);
    mBlocker.TouchEventSignal().Connect(this, &SharedDetectorScenario::OnBlockerTouch);

    Dali::Ui::Label hint = CreateLabel("Pan moves the detected Actor. LongPress: cyan=STARTED, original color=FINISHED, magenta=CANCELLED. RESET restores all.",
                                       12.0f,
                                       GetCanvasSize().y - 152.0f,
                                       GetCanvasSize().x - 24.0f,
                                       130.0f,
                                       13.0f);
    mRoot.Add(hint);
    return mRoot;
  }

private:
  bool OnSharedTouch(Dali::Actor actor, Dali::TouchEvent touch)
  {
    bool consumed = mSharedTap.HandleEvent(actor, touch);
    consumed |= mSharedPan.HandleEvent(actor, touch);
    consumed |= mSharedLong.HandleEvent(actor, touch);
    GetEventLog().AddTouch("TOUCH", actor, touch, std::string("shared detectors return=") + (consumed ? "true" : "false"));
    return consumed;
  }

  bool OnIndependentTouch(Dali::Actor actor, Dali::TouchEvent touch)
  {
    Dali::PanGestureDetector&       panDetector  = actor == mIndependentBlue ? mBluePan : mRedPan;
    Dali::LongPressGestureDetector& longDetector = actor == mIndependentBlue ? mBlueLong : mRedLong;

    const bool panConsumed  = panDetector.HandleEvent(actor, touch);
    const bool longConsumed = longDetector.HandleEvent(actor, touch);
    if(actor == mIndependentBlue && panConsumed)
    {
      panDetector.CancelAllOtherGestureDetectors();
      GetEventLog().Add("ARBITRATION", "Independent.Blue pan selected; CancelAllOtherGestureDetectors() called");
    }

    const bool consumed = panConsumed || longConsumed;
    GetEventLog().AddTouch("TOUCH", actor, touch, std::string("independent detectors return=") + (consumed ? "true" : "false"));
    return consumed;
  }

  bool OnBlockerTouch(Dali::Actor actor, Dali::TouchEvent touch)
  {
    GetEventLog().AddTouch("TOUCH", actor, touch, "return=true (blocker)");
    return true;
  }

  void OnTap(Dali::Actor actor, Dali::TapGesture tap)
  {
    GetEventLog().Add("TAP", ActorName(actor) + " shared state=" + GestureStateToString(tap.GetState()));
  }

  void OnPan(Dali::Actor actor, Dali::PanGesture pan)
  {
    LogPanAndMove(actor, pan, "shared");
  }

  void OnLongPress(Dali::Actor actor, Dali::LongPressGesture longPress)
  {
    UpdateLongPressVisual(actor, longPress.GetState());
    GetEventLog().Add("LONG", ActorName(actor) + " shared state=" + GestureStateToString(longPress.GetState()) + LongPressVisualDescription(longPress.GetState()));
  }

  void OnIndependentPan(Dali::Actor actor, Dali::PanGesture pan)
  {
    LogPanAndMove(actor, pan, "independent");
  }

  void LogPanAndMove(Dali::Actor actor, Dali::PanGesture pan, const char* detectorType)
  {
    const Dali::Vector2& displacement = pan.GetScreenDisplacement();
    std::ostringstream   stream;
    stream << ActorName(actor) << ' ' << detectorType << " state=" << GestureStateToString(pan.GetState())
           << " displacement=(" << displacement.x << ',' << displacement.y << ')';

    if(pan.GetState() == Dali::GestureState::STARTED || pan.GetState() == Dali::GestureState::CONTINUING)
    {
      const Dali::Vector2 position = MovePanActor(actor, displacement);
      stream << " position=(" << position.x << ',' << position.y << ')';
    }
    GetEventLog().Add("PAN", stream.str());
  }

  Dali::Vector2 MovePanActor(Dali::Actor actor, const Dali::Vector2& displacement)
  {
    Dali::Ui::View view;
    float          containerWidth  = GetCanvasSize().x;
    float          containerHeight = GetCanvasSize().y;

    if(actor == mSharedRed)
    {
      view = mSharedRed;
    }
    else if(actor == mSharedBlue)
    {
      view            = mSharedBlue;
      containerWidth  = mSharedRed.GetRequestedWidth();
      containerHeight = mSharedRed.GetRequestedHeight();
    }
    else if(actor == mIndependentRed)
    {
      view = mIndependentRed;
    }
    else if(actor == mIndependentBlue)
    {
      view            = mIndependentBlue;
      containerWidth  = mIndependentRed.GetRequestedWidth();
      containerHeight = mIndependentRed.GetRequestedHeight();
    }

    if(!view)
    {
      return Dali::Vector2::ZERO;
    }

    constexpr float MINIMUM_VISIBLE_LENGTH = 96.0f;
    const float     width                  = view.GetRequestedWidth();
    const float     height                 = view.GetRequestedHeight();
    const float     visibleWidth           = std::min(width, MINIMUM_VISIBLE_LENGTH);
    const float     visibleHeight          = std::min(height, MINIMUM_VISIBLE_LENGTH);
    const float     minimumX               = -width + visibleWidth;
    const float     maximumX               = containerWidth - visibleWidth;
    const float     minimumY               = -height + visibleHeight;
    const float     maximumY               = containerHeight - visibleHeight;
    const float     x                      = std::clamp(view.GetRequestedX() + displacement.x, minimumX, maximumX);
    const float     y                      = std::clamp(view.GetRequestedY() + displacement.y, minimumY, maximumY);

    view.SetRequestedX(x);
    view.SetRequestedY(y);
    return Dali::Vector2(x, y);
  }

  void OnIndependentLongPress(Dali::Actor actor, Dali::LongPressGesture longPress)
  {
    UpdateLongPressVisual(actor, longPress.GetState());
    GetEventLog().Add("LONG", ActorName(actor) + " independent state=" + GestureStateToString(longPress.GetState()) + LongPressVisualDescription(longPress.GetState()));
  }

  void UpdateLongPressVisual(Dali::Actor actor, Dali::GestureState state)
  {
    Dali::Ui::View view = GetGestureView(actor);
    if(!view)
    {
      return;
    }

    if(state == Dali::GestureState::STARTED)
    {
      view.SetBackgroundColor(LONG_PRESS_STARTED_COLOR);
    }
    else if(state == Dali::GestureState::CANCELLED)
    {
      view.SetBackgroundColor(LONG_PRESS_CANCELLED_COLOR);
    }
    else if(state == Dali::GestureState::FINISHED)
    {
      view.SetBackgroundColor(actor == mSharedRed || actor == mIndependentRed ? RED_ACTOR_COLOR : BLUE_ACTOR_COLOR);
    }
  }

  const char* LongPressVisualDescription(Dali::GestureState state) const
  {
    if(state == Dali::GestureState::STARTED)
    {
      return " color=cyan";
    }
    if(state == Dali::GestureState::CANCELLED)
    {
      return " color=magenta";
    }
    if(state == Dali::GestureState::FINISHED)
    {
      return " color=restored";
    }
    return "";
  }

  Dali::Ui::View GetGestureView(Dali::Actor actor) const
  {
    if(actor == mSharedRed)
    {
      return mSharedRed;
    }
    if(actor == mSharedBlue)
    {
      return mSharedBlue;
    }
    if(actor == mIndependentRed)
    {
      return mIndependentRed;
    }
    if(actor == mIndependentBlue)
    {
      return mIndependentBlue;
    }
    return Dali::Ui::View();
  }

private:
  Dali::Ui::View                 mRoot;
  Dali::Ui::View                 mSharedRed;
  Dali::Ui::View                 mSharedBlue;
  Dali::Ui::View                 mIndependentRed;
  Dali::Ui::View                 mIndependentBlue;
  Dali::Ui::View                 mBlocker;
  Dali::TapGestureDetector       mSharedTap;
  Dali::PanGestureDetector       mSharedPan;
  Dali::LongPressGestureDetector mSharedLong;
  Dali::PanGestureDetector       mRedPan;
  Dali::PanGestureDetector       mBluePan;
  Dali::LongPressGestureDetector mRedLong;
  Dali::LongPressGestureDetector mBlueLong;
};

/**
 * @brief Exercises tap and long-press competition through a three-level hierarchy.
 */
class TapLongPressScenario : public Scenario
{
public:
  TapLongPressScenario(EventLog& eventLog, const Dali::Vector2& canvasSize)
  : Scenario(eventLog, canvasSize),
    mParentTapCount(0u),
    mChildTapCount(0u),
    mGrandchildTapCount(0u),
    mLongPressCount(0u)
  {
  }

  ~TapLongPressScenario() override = default;

  TapLongPressScenario(const TapLongPressScenario&)            = delete;
  TapLongPressScenario& operator=(const TapLongPressScenario&) = delete;

  const char* GetTitle() const override
  {
    return "Tap vs LongPress";
  }

  const char* GetInstructions() const override
  {
    return "Quick tap vs hold in Parent->Child->Grandchild. Grandchild feeds Tap but always returns false; "
           "ancestors can continue receiving the coordinate stream. All enabled detectors are fed before returning.";
  }

  Dali::Ui::View Build() override
  {
    mRoot = Dali::Ui::View::New();
    mRoot.SetRequestedWidth(GetCanvasSize().x);
    mRoot.SetRequestedHeight(GetCanvasSize().y);

    const float parentWidth  = GetCanvasSize().x - 40.0f;
    const float parentHeight = std::max(260.0f, GetCanvasSize().y - 140.0f);
    const float childWidth   = std::max(220.0f, parentWidth - 140.0f);
    const float childHeight  = std::max(160.0f, parentHeight - 190.0f);

    mTapDetector       = Dali::TapGestureDetector::New();
    mLongPressDetector = Dali::LongPressGestureDetector::New();
    mTapDetector.DetectedSignal().Connect(this, &TapLongPressScenario::OnTap);
    mLongPressDetector.DetectedSignal().Connect(this, &TapLongPressScenario::OnLongPress);

    mParent     = CreateTestView("TapLong.Parent", "PARENT\nTap + LongPress", Dali::Vector4(0.88f, 0.43f, 0.08f, 1.0f), 20.0f, 24.0f, parentWidth, parentHeight);
    mChild      = CreateTestView("TapLong.Child", "CHILD\nTap", Dali::Vector4(0.12f, 0.68f, 0.72f, 1.0f), 70.0f, 92.0f, childWidth, childHeight);
    mGrandchild = CreateTestView("TapLong.Grandchild", "GRANDCHILD\nTap feed; return false", Dali::Vector4(0.82f, 0.12f, 0.16f, 1.0f), 120.0f, 98.0f, childWidth * 0.5f, 140.0f);
    mChild.Add(mGrandchild);
    mParent.Add(mChild);
    mRoot.Add(mParent);

    mParent.TouchEventSignal().Connect(this, &TapLongPressScenario::OnParentTouch);
    mChild.TouchEventSignal().Connect(this, &TapLongPressScenario::OnChildTouch);
    mGrandchild.TouchEventSignal().Connect(this, &TapLongPressScenario::OnGrandchildTouch);

    mStatusLabel = CreateLabel("", 20.0f, GetCanvasSize().y - 104.0f, GetCanvasSize().x - 40.0f, 86.0f, 14.0f);
    mRoot.Add(mStatusLabel);
    RefreshStatus();
    return mRoot;
  }

private:
  bool OnParentTouch(Dali::Actor actor, Dali::TouchEvent touch)
  {
    bool consumed = mTapDetector.HandleEvent(actor, touch);
    consumed |= mLongPressDetector.HandleEvent(actor, touch);
    GetEventLog().AddTouch("TOUCH", actor, touch, std::string("Tap|Long return=") + (consumed ? "true" : "false"));
    return consumed;
  }

  bool OnChildTouch(Dali::Actor actor, Dali::TouchEvent touch)
  {
    const bool consumed = mTapDetector.HandleEvent(actor, touch);
    GetEventLog().AddTouch("TOUCH", actor, touch, std::string("Tap return=") + (consumed ? "true" : "false"));
    return consumed;
  }

  bool OnGrandchildTouch(Dali::Actor actor, Dali::TouchEvent touch)
  {
    const bool detectorConsumed = mTapDetector.HandleEvent(actor, touch);
    GetEventLog().AddTouch("TOUCH", actor, touch, std::string("Tap detector=") + (detectorConsumed ? "true" : "false") + " callback return=false");
    return false;
  }

  void OnTap(Dali::Actor actor, Dali::TapGesture tap)
  {
    if(tap.GetState() == Dali::GestureState::STARTED)
    {
      if(actor == mParent)
      {
        ++mParentTapCount;
      }
      else if(actor == mChild)
      {
        ++mChildTapCount;
      }
      else if(actor == mGrandchild)
      {
        ++mGrandchildTapCount;
      }
      RefreshStatus();
    }
    GetEventLog().Add("TAP", ActorName(actor) + " state=" + GestureStateToString(tap.GetState()));
  }

  void OnLongPress(Dali::Actor actor, Dali::LongPressGesture longPress)
  {
    if(longPress.GetState() == Dali::GestureState::STARTED)
    {
      ++mLongPressCount;
      RefreshStatus();
    }
    GetEventLog().Add("LONG", ActorName(actor) + " state=" + GestureStateToString(longPress.GetState()));
  }

  void RefreshStatus()
  {
    std::ostringstream stream;
    stream << "Tap counts: Parent=" << mParentTapCount << " Child=" << mChildTapCount
           << " Grandchild=" << mGrandchildTapCount << " | Parent LongPress=" << mLongPressCount;
    mStatusLabel.SetText(stream.str().c_str());
  }

private:
  Dali::Ui::View                 mRoot;
  Dali::Ui::View                 mParent;
  Dali::Ui::View                 mChild;
  Dali::Ui::View                 mGrandchild;
  Dali::Ui::Label                mStatusLabel;
  Dali::TapGestureDetector       mTapDetector;
  Dali::LongPressGestureDetector mLongPressDetector;
  uint32_t                       mParentTapCount;
  uint32_t                       mChildTapCount;
  uint32_t                       mGrandchildTapCount;
  uint32_t                       mLongPressCount;
};

/**
 * @brief Exercises a shared tap detector against a directional scrolling pan detector.
 */
class TapPanScenario : public Scenario
{
public:
  TapPanScenario(EventLog& eventLog, const Dali::Vector2& canvasSize)
  : Scenario(eventLog, canvasSize),
    mBackgroundTapCount(0u),
    mOverlayTapCount(0u)
  {
  }

  ~TapPanScenario() override = default;

  TapPanScenario(const TapPanScenario&)            = delete;
  TapPanScenario& operator=(const TapPanScenario&) = delete;

  const char* GetTitle() const override
  {
    return "Tap vs Pan";
  }

  const char* GetInstructions() const override
  {
    return "Orange background and red overlay feed one Tap detector. The cyan horizontal viewport feeds Pan. "
           "Tap an area or drag the card strip and observe Tap cancellation/ownership.";
  }

  Dali::Ui::View Build() override
  {
    mRoot = Dali::Ui::View::New();
    mRoot.SetRequestedWidth(GetCanvasSize().x);
    mRoot.SetRequestedHeight(GetCanvasSize().y);

    const float backgroundWidth  = GetCanvasSize().x - 24.0f;
    const float backgroundHeight = GetCanvasSize().y - 40.0f;
    const float scrollerWidth    = GetCanvasSize().x - 80.0f;
    const float contentLength    = scrollerWidth + 540.0f;

    mTapDetector = Dali::TapGestureDetector::New();
    mTapDetector.DetectedSignal().Connect(this, &TapPanScenario::OnTap);

    mBackground = CreateTestView("TapPan.Background", "ORANGE BACKGROUND\nTap", Dali::Vector4(0.88f, 0.43f, 0.08f, 1.0f), 12.0f, 16.0f, backgroundWidth, backgroundHeight);
    mBackground.TouchEventSignal().Connect(this, &TapPanScenario::OnTapTouch);
    mRoot.Add(mBackground);

    mScroller = std::make_unique<ManualFeedScroller>(GetEventLog(),
                                                     "TapPan.HorizontalScroller",
                                                     ManualFeedScroller::Direction::HORIZONTAL,
                                                     scrollerWidth,
                                                     220.0f,
                                                     contentLength,
                                                     false,
                                                     false);
    mScroller->SetPosition(28.0f, 120.0f);
    const uint32_t cardCount = std::max(8u, static_cast<uint32_t>(contentLength / 128.0f));
    for(uint32_t index = 0u; index < cardCount; ++index)
    {
      std::ostringstream name;
      name << "PanCard" << index;
      Dali::Ui::View card = CreateTestView(name.str(), name.str(), Dali::Vector4(0.12f + 0.06f * static_cast<float>(index % 3u), 0.45f, 0.72f, 1.0f),
                                           20.0f + static_cast<float>(index) * 128.0f,
                                           38.0f,
                                           108.0f,
                                           144.0f);
      mScroller->Add(card);
    }
    mBackground.Add(mScroller->GetViewport());

    mOverlay = CreateTestView("TapPan.Overlay", "RED OVERLAY\nTap", Dali::Vector4(0.82f, 0.12f, 0.16f, 1.0f), backgroundWidth - 216.0f, backgroundHeight - 208.0f, 180.0f, 142.0f);
    mOverlay.TouchEventSignal().Connect(this, &TapPanScenario::OnTapTouch);
    mBackground.Add(mOverlay);

    mStatusLabel = CreateLabel("", 28.0f, backgroundHeight - 60.0f, backgroundWidth - 56.0f, 62.0f, 13.0f);
    mBackground.Add(mStatusLabel);
    mScroller->SetOffsetChangedCallback([this](float)
    {
      RefreshStatus();
    });
    RefreshStatus();
    return mRoot;
  }

private:
  bool OnTapTouch(Dali::Actor actor, Dali::TouchEvent touch)
  {
    const bool consumed = mTapDetector.HandleEvent(actor, touch);
    GetEventLog().AddTouch("TOUCH", actor, touch, std::string("Tap return=") + (consumed ? "true" : "false"));
    return consumed;
  }

  void OnTap(Dali::Actor actor, Dali::TapGesture tap)
  {
    if(tap.GetState() == Dali::GestureState::STARTED)
    {
      if(actor == mBackground)
      {
        ++mBackgroundTapCount;
      }
      else if(actor == mOverlay)
      {
        ++mOverlayTapCount;
      }
      RefreshStatus();
    }
    GetEventLog().Add("TAP", ActorName(actor) + " state=" + GestureStateToString(tap.GetState()));
  }

  void RefreshStatus()
  {
    std::ostringstream stream;
    stream << "Background taps=" << mBackgroundTapCount << " | Overlay taps=" << mOverlayTapCount
           << " | Horizontal offset=" << (mScroller ? mScroller->GetOffset() : 0.0f);
    mStatusLabel.SetText(stream.str().c_str());
  }

private:
  Dali::Ui::View                      mRoot;
  Dali::Ui::View                      mBackground;
  Dali::Ui::View                      mOverlay;
  Dali::Ui::Label                     mStatusLabel;
  Dali::TapGestureDetector            mTapDetector;
  std::unique_ptr<ManualFeedScroller> mScroller;
  uint32_t                            mBackgroundTapCount;
  uint32_t                            mOverlayTapCount;
};

} // unnamed namespace

std::unique_ptr<Scenario> CreateSharedDetectorScenario(EventLog& eventLog, const Dali::Vector2& canvasSize)
{
  return std::make_unique<SharedDetectorScenario>(eventLog, canvasSize);
}

std::unique_ptr<Scenario> CreateTapLongPressScenario(EventLog& eventLog, const Dali::Vector2& canvasSize)
{
  return std::make_unique<TapLongPressScenario>(eventLog, canvasSize);
}

std::unique_ptr<Scenario> CreateTapPanScenario(EventLog& eventLog, const Dali::Vector2& canvasSize)
{
  return std::make_unique<TapPanScenario>(eventLog, canvasSize);
}

} // namespace GeometryTouchGestureSample
