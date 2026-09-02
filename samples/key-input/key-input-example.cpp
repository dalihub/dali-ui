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

#include <dali/public-api/adaptor-framework/key.h>
#include <dali/public-api/adaptor-framework/keyboard.h>

#include <cstdint>
#include <deque>
#include <iomanip>
#include <sstream>
#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr size_t MAX_LOG_LINES = 9u;

std::string ToStdString(const Dali::String& value)
{
  return std::string(value.CStr(), value.Size());
}

std::string EscapeForDisplay(const Dali::String& value)
{
  std::string result;
  for(char character : ToStdString(value))
  {
    switch(character)
    {
      case '\n':
        result += "\\n";
        break;
      case '\r':
        result += "\\r";
        break;
      case '\t':
        result += "\\t";
        break;
      case '\b':
        result += "\\b";
        break;
      default:
        result += character;
        break;
    }
  }
  return result.empty() ? "<empty>" : result;
}

const char* GetStateName(KeyEvent::State state)
{
  return state == KeyEvent::DOWN ? "DOWN" : "UP";
}

const char* GetDeviceClassName(Device::Class::Type type)
{
  switch(type)
  {
    case Device::Class::NONE:
      return "NONE";
    case Device::Class::USER:
      return "USER";
    case Device::Class::KEYBOARD:
      return "KEYBOARD";
    case Device::Class::MOUSE:
      return "MOUSE";
    case Device::Class::TOUCH:
      return "TOUCH";
    case Device::Class::PEN:
      return "PEN";
    case Device::Class::POINTER:
      return "POINTER";
    case Device::Class::GAMEPAD:
      return "GAMEPAD";
  }
  return "UNKNOWN";
}

std::string GetCanonicalMatch(const KeyEvent& event)
{
  struct KeyMatch
  {
    Dali::KEY   key;
    const char* name;
  };

  const KeyMatch matches[] = {
    {DALI_KEY_ESCAPE, "DALI_KEY_ESCAPE"},
    {DALI_KEY_BACKSPACE, "DALI_KEY_BACKSPACE"},
    {DALI_KEY_SHIFT_LEFT, "DALI_KEY_SHIFT_LEFT"},
    {DALI_KEY_SHIFT_RIGHT, "DALI_KEY_SHIFT_RIGHT"},
    {DALI_KEY_CURSOR_UP, "DALI_KEY_CURSOR_UP"},
    {DALI_KEY_CURSOR_LEFT, "DALI_KEY_CURSOR_LEFT"},
    {DALI_KEY_CURSOR_RIGHT, "DALI_KEY_CURSOR_RIGHT"},
    {DALI_KEY_CURSOR_DOWN, "DALI_KEY_CURSOR_DOWN"},
    {DALI_KEY_BACK, "DALI_KEY_BACK"},
    {DALI_KEY_MENU, "DALI_KEY_MENU"},
    {DALI_KEY_HOME, "DALI_KEY_HOME"},
    {DALI_KEY_HOMEPAGE, "DALI_KEY_HOMEPAGE"},
    {DALI_KEY_MAIL, "DALI_KEY_MAIL"},
    {DALI_KEY_LANGUAGE, "DALI_KEY_LANGUAGE"},
    {DALI_KEY_VOLUME_UP, "DALI_KEY_VOLUME_UP"},
    {DALI_KEY_VOLUME_DOWN, "DALI_KEY_VOLUME_DOWN"},
    {Dali::DALI_KEY_DELETE, "DALI_KEY_DELETE"},
    {Dali::DALI_KEY_CONTROL_LEFT, "DALI_KEY_CONTROL_LEFT"},
    {Dali::DALI_KEY_CONTROL_RIGHT, "DALI_KEY_CONTROL_RIGHT"},
    {Dali::DALI_KEY_RETURN, "DALI_KEY_RETURN"},
    {Dali::DALI_KEY_KP_ENTER, "DALI_KEY_KP_ENTER"},
  };

  for(const auto& match : matches)
  {
    if(Dali::IsKey(event, match.key))
    {
      return match.name;
    }
  }
  return "<none>";
}

Label MakeLabel(const char* text, float fontSize, uint32_t color, float height)
{
  Label label = Label::New(text);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(height);
  label.SetFontSize(fontSize);
  label.SetTextColor(UiColor(color));
  label.SetMultiLine(true);
  label.SetLineWrapMode(Text::LineWrapMode::CHARACTER);
  label.SetPadding(Insets(8.0f, 8.0f, 3.0f, 3.0f));
  return label;
}
} // namespace

class KeyInputController : public ConnectionTracker
{
public:
  explicit KeyInputController(Application& application)
  : mApplication(application),
    mEventCount(0u),
    mDownCount(0u),
    mUpCount(0u),
    mRepeatCount(0u)
  {
    mApplication.InitSignal().Connect(this, &KeyInputController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x10151D));

    Label title = MakeLabel("DALi Key Input / Keyboard & IME parity test", 19.0f, 0xFFFFFF, 42.0f);
    title.SetBackgroundColor(UiColor(0x26364A));

    float              repeatRate  = 0.0f;
    float              repeatDelay = 0.0f;
    std::ostringstream guideText;
    guideText << "Focus the field; test text, modifiers, navigation, F-keys, keypad, media keys and IME. Repeat API: ";
    if(Keyboard::GetRepeatInfo(repeatRate, repeatDelay))
    {
      guideText << std::fixed << std::setprecision(3)
                << "rate=" << repeatRate << "s delay=" << repeatDelay << "s (read-only)";
    }
    else
    {
      guideText << "not exposed by this backend";
    }
    Label guide = MakeLabel(guideText.str().c_str(), 11.0f, 0xB8C7D9, 42.0f);

    mInputField = InputField::New();
    mInputField.SetPlaceholder("Type here / 여기에 입력 (한글 IME 포함)");
    mInputField.SetFontSize(18.0f);
    mInputField.SetRequestedWidth(MATCH_PARENT);
    mInputField.SetRequestedHeight(52.0f);
    mInputField.SetBackgroundColor(UiColor(0xFFFFFF));
    mInputField.SetTextColor(UiColor(0x111111));
    mInputField.SetPlaceholderColor(UiColor(0x777777));
    mInputField.SetPadding(Insets(10.0f, 10.0f, 6.0f, 6.0f));
    mInputField.SetFocusable(true);
    mInputField.TextChangedSignal().Connect(this, &KeyInputController::OnTextChanged);

    mTextStatus = MakeLabel("Committed text / 확정 문자열: <empty>", 11.0f, 0x9BE7A5, 34.0f);
    mTextStatus.SetBackgroundColor(UiColor(0x18241B));

    mEventDetails = MakeLabel("Press a key to inspect all DALi KeyEvent fields.",
                              10.0f,
                              0xFFFFFF,
                              142.0f);
    mEventDetails.SetBackgroundColor(UiColor(0x1A2330));

    mCounters = MakeLabel("Events 0 | DOWN 0 | UP 0 | repeat 0", 11.0f, 0xFFD166, 30.0f);
    mCounters.SetBackgroundColor(UiColor(0x2B2517));

    mLog = MakeLabel("Recent events / 최근 이벤트", 10.0f, 0xC8D6E5, 0.0f);
    mLog.SetBackgroundColor(UiColor(0x111820));
    mLog.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetSpacing(4.0f);
    root.SetPadding(Insets(8.0f, 8.0f, 8.0f, 8.0f));
    root.Add(title);
    root.Add(guide);
    root.Add(mInputField);
    root.Add(mTextStatus);
    root.Add(mEventDetails);
    root.Add(mCounters);
    root.Add(mLog);
    window.Add(root);

    // Intercept observes events before InputField consumes them. Returning
    // false preserves normal text-control behavior.
    window.InterceptKeyEventSignal().Connect(this, &KeyInputController::OnKeyEvent);
    FocusManager::Get().SetCurrentFocusView(mInputField);
  }

  bool OnKeyEvent(Window, KeyEvent event)
  {
    ++mEventCount;
    if(event.GetState() == KeyEvent::DOWN)
    {
      ++mDownCount;
    }
    else
    {
      ++mUpCount;
    }
    if(event.IsRepeat())
    {
      ++mRepeatCount;
    }

    const uint32_t receiveTime = event.GetReceiveTime();
    const auto     eventTime   = static_cast<uint32_t>(event.GetTime());
    const uint32_t latency     = receiveTime >= eventTime ? receiveTime - eventTime : 0u;

    std::ostringstream details;
    details << "state=" << GetStateName(event.GetState())
            << "  repeat=" << (event.IsRepeat() ? "true" : "false")
            << "  canonical=" << GetCanonicalMatch(event) << '\n'
            << "keyName=\"" << EscapeForDisplay(event.GetKeyName())
            << "\"  logicalKey=\"" << EscapeForDisplay(event.GetLogicalKey()) << "\"\n"
            << "keyString=\"" << EscapeForDisplay(event.GetKeyString())
            << "\"  compose=\"" << EscapeForDisplay(event.GetCompose()) << "\"\n"
            << "keyCode=" << event.GetKeyCode()
            << "  modifier=0x" << std::hex << std::uppercase << event.GetKeyModifier() << std::dec
            << " [S=" << event.IsShiftModifier()
            << " C=" << event.IsCtrlModifier()
            << " A=" << event.IsAltModifier() << "]\n"
            << "time=" << event.GetTime()
            << "  receiveTime=" << receiveTime
            << "  latency=" << latency << "ms"
            << "  windowId=" << event.GetWindowId() << '\n'
            << "device=\"" << EscapeForDisplay(event.GetDeviceName())
            << "\"  class=" << GetDeviceClassName(event.GetDeviceClass())
            << "  subclass=" << static_cast<int>(event.GetDeviceSubclass());
    mEventDetails.SetText(details.str().c_str());

    std::ostringstream summary;
    summary << '#' << mEventCount << ' '
            << (event.GetState() == KeyEvent::DOWN ? 'D' : 'U')
            << (event.IsRepeat() ? 'R' : '-')
            << " name=" << EscapeForDisplay(event.GetKeyName())
            << " logical=" << EscapeForDisplay(event.GetLogicalKey())
            << " text=" << EscapeForDisplay(event.GetKeyString())
            << " code=" << event.GetKeyCode()
            << " mod=0x" << std::hex << std::uppercase << event.GetKeyModifier();
    mRecentEvents.push_front(summary.str());
    if(mRecentEvents.size() > MAX_LOG_LINES)
    {
      mRecentEvents.pop_back();
    }
    UpdateLog();

    std::ostringstream counters;
    counters << "Events " << mEventCount
             << " | DOWN " << mDownCount
             << " | UP " << mUpCount
             << " | repeat " << mRepeatCount;
    mCounters.SetText(counters.str().c_str());

    return false;
  }

  void OnTextChanged(View view)
  {
    InputField field = InputField::DownCast(view);
    if(field)
    {
      std::ostringstream status;
      const Dali::String text = field.GetText();
      status << "Committed text / 확정 문자열 (" << text.Size() << " bytes): "
             << EscapeForDisplay(text);
      mTextStatus.SetText(status.str().c_str());
    }
  }

  void UpdateLog()
  {
    std::ostringstream log;
    log << "Recent events / 최근 이벤트";
    for(const auto& event : mRecentEvents)
    {
      log << '\n'
          << event;
    }
    mLog.SetText(log.str().c_str());
  }

private:
  Application& mApplication;
  InputField   mInputField;
  Label        mTextStatus;
  Label        mEventDetails;
  Label        mCounters;
  Label        mLog;

  uint64_t                mEventCount;
  uint64_t                mDownCount;
  uint64_t                mUpCount;
  uint64_t                mRepeatCount;
  std::deque<std::string> mRecentEvents;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  KeyInputController controller(application);
  application.MainLoop();
  return 0;
}
