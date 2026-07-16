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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali/public-api/adaptor-framework/drag-and-drop.h>
#include <dali/public-api/events/long-press-gesture-detector.h>
#include <dali/public-api/events/long-press-gesture.h>
#include <dali/public-api/events/touch-event.h>

#include <deque>
#include <sstream>
#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
const char  MIME_TYPE[]  = "text/plain";
const char* MIME_TYPES[] = {MIME_TYPE};
const char* DATA_SET[]   = {"BLUE CARD"};

const UiColor SOURCE_COLOR(0x1E88E5u);
const UiColor LEFT_TARGET_COLOR(0x388E3Cu);
const UiColor RIGHT_TARGET_COLOR(0x6A1B9Au);
const UiColor WINDOW_TARGET_COLOR(0x455A64u);
const UiColor DROP_HIGHLIGHT_COLOR(0xF9A825u);

constexpr float SOURCE_X       = 440.0f;
constexpr float LEFT_TARGET_X  = 740.0f;
constexpr float RIGHT_TARGET_X = 1040.0f;

const char* ToString(DragAndDrop::SourceEventType type)
{
  switch(type)
  {
    case DragAndDrop::SourceEventType::START:
      return "START";
    case DragAndDrop::SourceEventType::CANCEL:
      return "CANCEL";
    case DragAndDrop::SourceEventType::ACCEPT:
      return "ACCEPT";
    case DragAndDrop::SourceEventType::FINISH:
      return "FINISH";
  }
  return "UNKNOWN";
}

const char* ToString(DragAndDrop::DragType type)
{
  switch(type)
  {
    case DragAndDrop::DragType::ENTER:
      return "ENTER";
    case DragAndDrop::DragType::LEAVE:
      return "LEAVE";
    case DragAndDrop::DragType::MOVE:
      return "MOVE";
    case DragAndDrop::DragType::DROP:
      return "DROP";
  }
  return "UNKNOWN";
}

Label NewLabel(const char* text, float fontSize, const UiColor& color)
{
  Label label = Label::New(text);
  label.SetFontSize(fontSize);
  label.SetTextColor(color);
  label.SetMultiLine(true);
  return label;
}
} // unnamed namespace

class DragAndDropController : public ConnectionTracker
{
public:
  explicit DragAndDropController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &DragAndDropController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x20252Bu));

    View root = View::New();
    root.SetBackgroundColor(UiColor(0x20252Bu));
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);

    Label title = NewLabel("DALi UI public Drag & Drop", 24.0f, UiColor(0xFFFFFFu));
    title.SetRequestedPositionX(32_spx);
    title.SetRequestedPositionY(24_spx);
    root.Add(title);

    Label guide = NewLabel("Long press BLUE CARD and move the shadow onto a target, including the separate target window. A target turns blue when the drop succeeds.", 14.0f, UiColor(0xD5DEE8u));
    guide.SetRequestedPositionX(32_spx);
    guide.SetRequestedPositionY(66_spx);
    guide.SetRequestedWidth(900_spx);
    root.Add(guide);

    mSource = View::New();
    mSource.SetBackgroundColor(SOURCE_COLOR);
    mSource.SetRequestedPositionX(SOURCE_X);
    mSource.SetRequestedPositionY(126_spx);
    mSource.SetRequestedWidth(260_spx);
    mSource.SetRequestedHeight(150_spx);
    mSourceLabel = NewLabel("SOURCE\nBLUE CARD\nLong press to drag", 18.0f, UiColor(0xFFFFFFu));
    mSourceLabel.SetRequestedPositionX(24_spx);
    mSourceLabel.SetRequestedPositionY(28_spx);
    mSource.Add(mSourceLabel);
    root.Add(mSource);

    mLeftTarget  = NewTarget("TARGET A", LEFT_TARGET_X, LEFT_TARGET_COLOR, mLeftTargetLabel);
    mRightTarget = NewTarget("TARGET B", RIGHT_TARGET_X, RIGHT_TARGET_COLOR, mRightTargetLabel);
    root.Add(mLeftTarget);
    root.Add(mRightTarget);

    mLog = NewLabel("Waiting for drag events...", 14.0f, UiColor(0xEBF3F7u));
    mLog.SetBackgroundColor(UiColor(0x30363Du));
    mLog.SetRequestedPositionX(32_spx);
    mLog.SetRequestedPositionY(320_spx);
    mLog.SetRequestedWidth(900_spx);
    mLog.SetRequestedHeight(260_spx);
    root.Add(mLog);

    window.Add(root);

    CreateTargetWindow();
    ConnectSourceGesture();
    RegisterTargets();
  }

private:
  View NewTarget(const char* name, float x, const UiColor& color, Label& label)
  {
    View target = View::New();
    target.SetBackgroundColor(color);
    target.SetRequestedPositionX(x);
    target.SetRequestedPositionY(126_spx);
    target.SetRequestedWidth(260_spx);
    target.SetRequestedHeight(150_spx);
    label = NewLabel(std::string(name).append("\nDrop BLUE CARD here").c_str(), 18.0f, UiColor(0xFFFFFFu));
    label.SetRequestedPositionX(24_spx);
    label.SetRequestedPositionY(54_spx);
    target.Add(label);
    return target;
  }

  void CreateShadowWindow()
  {
    mShadowWindow = Window::New(PositionSize(0, 0, 260, 150), "dali-ui-dnd-shadow", true);
    mShadowCard = View::New();
    mShadowCard.SetBackgroundColor(SOURCE_COLOR);
    mShadowCard.SetRequestedWidth(MATCH_PARENT);
    mShadowCard.SetRequestedHeight(MATCH_PARENT);
    mShadowLabel = NewLabel("DRAGGING\nBLUE CARD", 18.0f, UiColor(0xFFFFFFu));
    mShadowLabel.SetRequestedPositionX(12_spx);
    mShadowLabel.SetRequestedPositionY(20_spx);
    mShadowCard.Add(mShadowLabel);
    mShadowWindow.Add(mShadowCard);
  }

  void CreateTargetWindow()
  {
    mTargetWindow = Window::New(PositionSize(1180, 260, 360, 280), "dali-ui-dnd-target-window", false);
    mTargetWindow.SetBackgroundColor(UiColor(0x20252Bu));

    View root = View::New();
    root.SetBackgroundColor(UiColor(0x20252Bu));
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);

    Label title = NewLabel("Separate target window", 18.0f, UiColor(0xFFFFFFu));
    title.SetRequestedPositionX(24_spx);
    title.SetRequestedPositionY(22_spx);
    root.Add(title);

    mWindowTargetCard = View::New();
    mWindowTargetCard.SetBackgroundColor(WINDOW_TARGET_COLOR);
    mWindowTargetCard.SetRequestedPositionX(24_spx);
    mWindowTargetCard.SetRequestedPositionY(68_spx);
    mWindowTargetCard.SetRequestedWidth(312_spx);
    mWindowTargetCard.SetRequestedHeight(170_spx);
    mWindowTargetLabel = NewLabel("TARGET WINDOW\nDrop BLUE CARD here", 18.0f, UiColor(0xFFFFFFu));
    mWindowTargetLabel.SetRequestedPositionX(24_spx);
    mWindowTargetLabel.SetRequestedPositionY(58_spx);
    mWindowTargetCard.Add(mWindowTargetLabel);
    root.Add(mWindowTargetCard);

    mTargetWindow.Add(root);
    mTargetWindow.SetPositionSize(PositionSize(1180, 260, 360, 280));
  }

  void ConnectSourceGesture()
  {
    mSource.TouchEventSignal().Connect(this, &DragAndDropController::OnSourceTouch);
    mLongPressDetector = LongPressGestureDetector::New();
    mLongPressDetector.SetTouchesRequired(1);
    mLongPressDetector.DetectedSignal().Connect(this, &DragAndDropController::OnLongPress);
  }

  void RegisterTargets()
  {
    mDnd = DragAndDrop::Get();
    const bool leftRegistered = mDnd.AddListener(mLeftTarget, MIME_TYPE, [this](const DragAndDrop::DragEvent& event) { OnTargetEvent("TARGET A", mLeftTarget, mLeftTargetLabel, mLeftTargetColor, event); });
    const bool rightRegistered = mDnd.AddListener(mRightTarget, MIME_TYPE, [this](const DragAndDrop::DragEvent& event) { OnTargetEvent("TARGET B", mRightTarget, mRightTargetLabel, mRightTargetColor, event); });
    const bool windowRegistered = mDnd.AddListener(mTargetWindow, MIME_TYPE, [this](const DragAndDrop::DragEvent& event) { OnWindowTargetEvent(event); });
    AddLog(std::string("target registration: A=") + (leftRegistered ? "ok" : "failed") + ", B=" + (rightRegistered ? "ok" : "failed") + ", window=" + (windowRegistered ? "ok" : "failed"));
  }

  bool OnSourceTouch(Actor actor, TouchEvent touch)
  {
    if(touch.GetPointCount() == 0u)
    {
      return false;
    }
    mLongPressDetector.HandleEvent(actor, touch);
    return true;
  }

  void OnLongPress(Actor actor, LongPressGesture gesture)
  {
    if(gesture.GetState() != GestureState::STARTED)
    {
      return;
    }

    // The shadow window is created only when a drag starts, so it never covers
    // the source card while the sample is idle.
    CreateShadowWindow();

    DragAndDrop::DragData dragData;
    dragData.SetMimeTypes(MIME_TYPES, 1);
    dragData.SetDataSet(DATA_SET, 1);

    const bool started = mDnd.StartDragAndDrop(actor, mShadowWindow, dragData, [this](DragAndDrop::SourceEventType type) { OnSourceEvent(type); });
    if(!started)
    {
      DestroyShadowWindow();
    }
    AddLog(std::string("StartDragAndDrop: ") + (started ? "ok" : "failed"));
  }

  void DestroyShadowWindow()
  {
    // DragAndDrop releases its own shadow-window handle after a terminal
    // source event. Release the sample's handle too so the native window can
    // be destroyed as soon as the drag ends.
    mShadowWindow.Reset();
    mShadowCard.Reset();
    mShadowLabel.Reset();
  }

  void OnSourceEvent(DragAndDrop::SourceEventType type)
  {
    if(type == DragAndDrop::SourceEventType::START)
    {
      mSource.SetBackgroundColor(UiColor(0x1565C0u));
      mSourceLabel.SetText("SOURCE\nDRAGGING...");
    }
    else if(type == DragAndDrop::SourceEventType::CANCEL || type == DragAndDrop::SourceEventType::FINISH)
    {
      mSource.SetBackgroundColor(SOURCE_COLOR);
      mSourceLabel.SetText("SOURCE\nBLUE CARD\nLong press to drag");
      DestroyShadowWindow();
    }
    AddLog(std::string("source: ") + ToString(type));
  }

  void OnTargetEvent(const char* targetName, View target, Label label, UiColor& targetColor, const DragAndDrop::DragEvent& event)
  {
    std::ostringstream stream;
    stream << targetName << ": " << ToString(event.GetAction())
           << " position=(" << event.GetPosition().x << ", " << event.GetPosition().y << ")";

    if(event.GetAction() == DragAndDrop::DragType::ENTER)
    {
      target.SetBackgroundColor(DROP_HIGHLIGHT_COLOR);
      label.SetText(std::string(targetName).append("\nRelease to drop").c_str());
    }
    else if(event.GetAction() == DragAndDrop::DragType::LEAVE)
    {
      target.SetBackgroundColor(targetColor);
      label.SetText(std::string(targetName).append("\nDrop BLUE CARD here").c_str());
    }
    else if(event.GetAction() == DragAndDrop::DragType::DROP && event.GetData())
    {
      targetColor = SOURCE_COLOR;
      target.SetBackgroundColor(targetColor);
      label.SetText(std::string(targetName).append("\nDROPPED\n").append(event.GetData()).c_str());
      stream << " data=" << event.GetData();
    }
    AddLog(stream.str());
  }

  void OnWindowTargetEvent(const DragAndDrop::DragEvent& event)
  {
    std::ostringstream stream;
    stream << "TARGET WINDOW: " << ToString(event.GetAction())
           << " position=(" << event.GetPosition().x << ", " << event.GetPosition().y << ")";

    if(event.GetAction() == DragAndDrop::DragType::ENTER)
    {
      mWindowTargetCard.SetBackgroundColor(DROP_HIGHLIGHT_COLOR);
      mWindowTargetLabel.SetText("TARGET WINDOW\nRelease to drop");
    }
    else if(event.GetAction() == DragAndDrop::DragType::LEAVE)
    {
      mWindowTargetCard.SetBackgroundColor(mWindowTargetColor);
      mWindowTargetLabel.SetText("TARGET WINDOW\nDrop BLUE CARD here");
    }
    else if(event.GetAction() == DragAndDrop::DragType::DROP && event.GetData())
    {
      mWindowTargetColor = SOURCE_COLOR;
      mWindowTargetCard.SetBackgroundColor(mWindowTargetColor);
      mWindowTargetLabel.SetText(std::string("TARGET WINDOW\nDROPPED\n").append(event.GetData()).c_str());
      stream << " data=" << event.GetData();
    }
    AddLog(stream.str());
  }

  void AddLog(const std::string& line)
  {
    mLogs.push_front(line);
    while(mLogs.size() > 8u)
    {
      mLogs.pop_back();
    }

    std::ostringstream stream;
    for(const auto& log : mLogs)
    {
      stream << log << '\n';
    }
    mLog.SetText(stream.str().c_str());
  }

private:
  Application&             mApplication;
  DragAndDrop              mDnd;
  View                     mSource;
  Label                    mSourceLabel;
  View                     mLeftTarget;
  Label                    mLeftTargetLabel;
  UiColor                  mLeftTargetColor{LEFT_TARGET_COLOR};
  View                     mRightTarget;
  Label                    mRightTargetLabel;
  UiColor                  mRightTargetColor{RIGHT_TARGET_COLOR};
  Window                   mShadowWindow;
  View                     mShadowCard;
  Label                    mShadowLabel;
  Window                   mTargetWindow;
  View                     mWindowTargetCard;
  Label                    mWindowTargetLabel;
  UiColor                  mWindowTargetColor{WINDOW_TARGET_COLOR};
  Label                    mLog;
  LongPressGestureDetector mLongPressDetector;
  std::deque<std::string>  mLogs;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig    config      = UiConfig::New();
  config.Apply();

  DragAndDropController controller(application);
  application.MainLoop();
  return 0;
}
