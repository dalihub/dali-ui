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

// Demonstrates GestureDeviceSelector: the same PanGestureDetector and TapGestureDetector give a
// mouse different recognition behaviour than a touchscreen, without the application inspecting the
// device on every event. Two layers are configured, once, at start-up:
//
//   - Detector-local Options (PanGestureDetector::Options / TapGestureDetector::Options):
//     the mouse profile restricts panning to roughly horizontal drags and allows a double-click.
//   - Application-wide GestureThresholds: the mouse profile needs a longer drag before a pan
//     starts, and a faster second click before it counts as a double-click.
//
// Drag the block with the mouse: a mostly-horizontal drag moves it; a mostly-vertical drag does
// nothing, because the pan never starts. On a touchscreen the block follows any drag direction.
// Click (or tap) the button once or twice to see the per-device tap count and timing in the log.

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali/devel-api/adaptor-framework/application.h>

#include <dali/public-api/events/device.h>
#include <dali/public-api/events/gesture-device-selector.h>
#include <dali/public-api/events/gesture-thresholds.h>
#include <dali/public-api/events/pan-gesture-detector.h>
#include <dali/public-api/events/tap-gesture-detector.h>

#include <algorithm>
#include <cstdint>
#include <deque>
#include <iostream>
#include <sstream>
#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float TRACK_X = 30.0f, TRACK_Y = 150.0f, TRACK_WIDTH = 840.0f, TRACK_HEIGHT = 170.0f;
constexpr float HANDLE_SIZE = 90.0f;

constexpr float TAP_BUTTON_X = 30.0f, TAP_BUTTON_Y = 440.0f, TAP_BUTTON_WIDTH = 220.0f, TAP_BUTTON_HEIGHT = 90.0f;

// The mouse needs to drag further before a pan starts, and a click needs to repeat sooner to
// count as a double-click, than the touch defaults (GestureThresholds::GetDefault*Thresholds()).
constexpr int32_t  MOUSE_PAN_MINIMUM_DISTANCE     = 40;
constexpr uint32_t MOUSE_TAP_MAXIMUM_MULTI_TAP_MS = 250u;
constexpr uint32_t TOUCH_TAP_MAXIMUM_MULTI_TAP_MS = 450u;

const char* DeviceClassName(Device::Class::Type deviceClass)
{
  switch(deviceClass)
  {
    case Device::Class::MOUSE:
      return "MOUSE";
    case Device::Class::TOUCH:
      return "TOUCH";
    case Device::Class::POINTER:
      return "POINTER";
    case Device::Class::PEN:
      return "PEN";
    case Device::Class::KEYBOARD:
      return "KEYBOARD";
    case Device::Class::GAMEPAD:
      return "GAMEPAD";
    case Device::Class::USER:
      return "USER";
    case Device::Class::NONE: // fall through
    default:
      return "NONE";
  }
}

Label MakeLabel(const char* text, float fontSize, UiColor color)
{
  Label label = Label::New(text);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(MATCH_PARENT);
  label.SetFontSize(fontSize);
  label.SetTextColor(color);
  label.SetMultiLine(true);
  label.SetSensitive(false);
  return label;
}
} // unnamed namespace

class DeviceGestureOptionsController : public ConnectionTracker
{
public:
  explicit DeviceGestureOptionsController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &DeviceGestureOptionsController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x111820));
    window.KeyEventSignal().Connect(this, &DeviceGestureOptionsController::OnKeyEvent);

    AbsoluteLayout root = AbsoluteLayout::New();
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);

    Label title = MakeLabel("Device Gesture Options: Mouse vs Touch", 24.0f, UiColor(0xFFFFFF));
    title.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(30.0f, 16.0f, 840.0f, 36.0f)));
    root.Add(title);

    Label guide = MakeLabel(
      "One PanGestureDetector and one TapGestureDetector; only their per-device Options and "
      "GestureThresholds differ. Mouse and touch see different recognition behaviour below.",
      14.0f,
      UiColor(0xB9C9DA));
    guide.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(30.0f, 56.0f, 840.0f, 54.0f)));
    root.Add(guide);

    SetupPanZone(root);
    SetupTapZone(root);

    Label legend = MakeLabel("", 13.0f, UiColor(0x8FA3B8));
    legend.SetBackgroundColor(UiColor(0x1A2531));
    legend.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(30.0f, 590.0f, 840.0f, 100.0f)));
    std::ostringstream legendText;
    legendText << "Configured per device (registered once at start-up, resolved from the device that pressed down):\n"
               << "  MOUSE  pan: horizontal-only, min distance " << MOUSE_PAN_MINIMUM_DISTANCE << "px   |   tap: up to 2 taps, multi-tap interval "
               << MOUSE_TAP_MAXIMUM_MULTI_TAP_MS << "ms\n"
               << "  TOUCH  pan: any direction, min distance " << GestureThresholds::GetDefaultPanThresholds().GetMinimumDistance()
               << "px (default)   |   tap: up to 2 taps, multi-tap interval " << TOUCH_TAP_MAXIMUM_MULTI_TAP_MS << "ms";
    legend.SetText(legendText.str().c_str());
    root.Add(legend);

    window.Add(root);

    std::cout << "[device-gesture-options] ready: drag the block and click/tap the button with your "
                 "mouse (and a touchscreen, if available) to compare device-specific recognition."
              << std::endl;
  }

  void SetupPanZone(AbsoluteLayout root)
  {
    Label sectionLabel = MakeLabel("PAN ZONE  —  drag the block", 16.0f, UiColor(0xE7EEF5));
    sectionLabel.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(30.0f, 120.0f, 400.0f, 26.0f)));
    root.Add(sectionLabel);

    View track = View::New();
    track.SetName("PanTrack");
    track.SetBackgroundColor(UiColor(0x1A2531));
    track.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(TRACK_X, TRACK_Y, TRACK_WIDTH, TRACK_HEIGHT)));
    track.SetSensitive(false);
    root.Add(track);

    mHandleX = TRACK_X + (TRACK_WIDTH - HANDLE_SIZE) * 0.5f;
    mHandleY = TRACK_Y + (TRACK_HEIGHT - HANDLE_SIZE) * 0.5f;

    mHandle = View::New();
    mHandle.SetName("PanHandle");
    mHandle.SetBackgroundColor(UiColor(0x4D8FE8));
    mHandle.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(mHandleX, mHandleY, HANDLE_SIZE, HANDLE_SIZE)));
    root.Add(mHandle);

    // Touch keeps the built-in defaults (any direction, GestureThresholds::GetDefaultPanThresholds());
    // registering them explicitly here just makes that intent visible in the code.
    mPanDetector = PanGestureDetector::New();
    mPanDetector.SetDeviceOptions(GestureDeviceSelector::ByDeviceClass(Device::Class::TOUCH), mPanDetector.GetDefaultOptions());
    GestureThresholds::SetPanThresholds(GestureDeviceSelector::ByDeviceClass(Device::Class::TOUCH), GestureThresholds::GetDefaultPanThresholds());

    // Mouse: only start the pan for a roughly horizontal drag, and require a longer drag first.
    PanGestureDetector::Options mouseOptions = mPanDetector.GetDefaultOptions();
    mouseOptions.AddDirection(PanGestureDetector::DIRECTION_HORIZONTAL);
    mPanDetector.SetDeviceOptions(GestureDeviceSelector::ByDeviceClass(Device::Class::MOUSE), mouseOptions);

    GestureThresholds::PanThresholds mouseThresholds = GestureThresholds::GetDefaultPanThresholds();
    mouseThresholds.SetMinimumDistance(MOUSE_PAN_MINIMUM_DISTANCE);
    GestureThresholds::SetPanThresholds(GestureDeviceSelector::ByDeviceClass(Device::Class::MOUSE), mouseThresholds);

    mPanDetector.Attach(mHandle);
    mPanDetector.DetectedSignal().Connect(this, &DeviceGestureOptionsController::OnPan);

    mPanStatusLabel = MakeLabel("Pan: waiting for the first drag.", 14.0f, UiColor(0xE7EEF5));
    mPanStatusLabel.SetBackgroundColor(UiColor(0x1A2531));
    mPanStatusLabel.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(30.0f, 330.0f, 840.0f, 70.0f)));
    root.Add(mPanStatusLabel);
  }

  void SetupTapZone(AbsoluteLayout root)
  {
    Label sectionLabel = MakeLabel("TAP ZONE  —  click or tap once or twice", 16.0f, UiColor(0xE7EEF5));
    sectionLabel.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(30.0f, 410.0f, 400.0f, 26.0f)));
    root.Add(sectionLabel);

    View button = View::New();
    button.SetName("TapButton");
    button.SetBackgroundColor(UiColor(0x39725B));
    button.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(TAP_BUTTON_X, TAP_BUTTON_Y, TAP_BUTTON_WIDTH, TAP_BUTTON_HEIGHT)));

    Label buttonLabel = MakeLabel("tap me", 16.0f, UiColor(0xFFFFFF));
    button.Add(buttonLabel);

    mTapDetector = TapGestureDetector::New();

    // Both devices allow up to a double tap; only the multi-tap interval differs.
    TapGestureDetector::Options doubleTapOptions = mTapDetector.GetDefaultOptions();
    doubleTapOptions.SetMaximumTapsRequired(2u);
    mTapDetector.SetDeviceOptions(GestureDeviceSelector::ByDeviceClass(Device::Class::MOUSE), doubleTapOptions);
    mTapDetector.SetDeviceOptions(GestureDeviceSelector::ByDeviceClass(Device::Class::TOUCH), doubleTapOptions);

    GestureThresholds::TapThresholds mouseTapThresholds = GestureThresholds::GetDefaultTapThresholds();
    mouseTapThresholds.SetMaximumMultiTapInterval(MOUSE_TAP_MAXIMUM_MULTI_TAP_MS);
    GestureThresholds::SetTapThresholds(GestureDeviceSelector::ByDeviceClass(Device::Class::MOUSE), mouseTapThresholds);

    GestureThresholds::TapThresholds touchTapThresholds = GestureThresholds::GetDefaultTapThresholds();
    touchTapThresholds.SetMaximumMultiTapInterval(TOUCH_TAP_MAXIMUM_MULTI_TAP_MS);
    GestureThresholds::SetTapThresholds(GestureDeviceSelector::ByDeviceClass(Device::Class::TOUCH), touchTapThresholds);

    mTapDetector.Attach(button);
    mTapDetector.DetectedSignal().Connect(this, &DeviceGestureOptionsController::OnTap);
    root.Add(button);

    mTapStatusLabel = MakeLabel("Tap log:\n(nothing yet)", 13.0f, UiColor(0xE7EEF5));
    mTapStatusLabel.SetBackgroundColor(UiColor(0x1A2531));
    mTapStatusLabel.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(270.0f, 440.0f, 600.0f, 140.0f)));
    root.Add(mTapStatusLabel);
  }

  void OnPan(Actor /*actor*/, PanGesture gesture)
  {
    const Device::Class::Type deviceClass = gesture.GetDeviceClass();
    if(deviceClass == Device::Class::MOUSE)
    {
      ++mMousePanCount;
    }
    else if(deviceClass == Device::Class::TOUCH)
    {
      ++mTouchPanCount;
    }
    else
    {
      ++mOtherPanCount;
    }

    if(gesture.GetState() == GestureState::STARTED || gesture.GetState() == GestureState::CONTINUING)
    {
      const Vector2& displacement = gesture.GetDisplacement();
      mHandleX                    = std::min(std::max(mHandleX + displacement.x, TRACK_X), TRACK_X + TRACK_WIDTH - HANDLE_SIZE);
      mHandleY                    = std::min(std::max(mHandleY + displacement.y, TRACK_Y), TRACK_Y + TRACK_HEIGHT - HANDLE_SIZE);
      mHandle.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(mHandleX, mHandleY, HANDLE_SIZE, HANDLE_SIZE)));
    }

    std::ostringstream text;
    text << "Pan " << DeviceClassName(deviceClass) << ": started " << mMousePanCount << " (mouse) / "
         << mTouchPanCount << " (touch) / " << mOtherPanCount << " (other) times so far.\n"
         << "A mouse drag only starts within 45 degrees of horizontal; a touch drag starts in any direction.";
    mPanStatusLabel.SetText(text.str().c_str());
  }

  void OnTap(Actor /*actor*/, TapGesture gesture)
  {
    const Device::Class::Type deviceClass = gesture.GetDeviceClass();
    if(deviceClass == Device::Class::MOUSE)
    {
      ++mMouseTapCount;
    }
    else if(deviceClass == Device::Class::TOUCH)
    {
      ++mTouchTapCount;
    }
    else
    {
      ++mOtherTapCount;
    }

    std::ostringstream entry;
    entry << DeviceClassName(deviceClass) << ": " << gesture.GetNumberOfTaps() << " tap(s)";
    mTapTrace.push_back(entry.str());
    if(mTapTrace.size() > MAX_TAP_TRACE)
    {
      mTapTrace.pop_front();
    }

    std::ostringstream text;
    text << "Tap log (mouse " << mMouseTapCount << " / touch " << mTouchTapCount << " / other " << mOtherTapCount << "):\n";
    for(const std::string& line : mTapTrace)
    {
      text << line << '\n';
    }
    mTapStatusLabel.SetText(text.str().c_str());
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
  static constexpr std::size_t MAX_TAP_TRACE = 6u;

  Application& mApplication;

  PanGestureDetector mPanDetector;
  View               mHandle;
  float              mHandleX{0.0f};
  float              mHandleY{0.0f};
  Label              mPanStatusLabel;
  uint32_t           mMousePanCount{0u};
  uint32_t           mTouchPanCount{0u};
  uint32_t           mOtherPanCount{0u};

  TapGestureDetector      mTapDetector;
  Label                   mTapStatusLabel;
  std::deque<std::string> mTapTrace;
  uint32_t                mMouseTapCount{0u};
  uint32_t                mTouchTapCount{0u};
  uint32_t                mOtherTapCount{0u};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  DeviceGestureOptionsController controller(application);
  application.MainLoop();
  return 0;
}
