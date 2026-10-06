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

// EXTERNAL INCLUDES
#include <algorithm>
#include <array>
#include <cstdint>
#include <deque>
#include <sstream>
#include <string>

// INTERNAL INCLUDES
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/extension-api/focus-manager.h>
#include <dali/devel-api/adaptor-framework/application.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/adaptor-framework/key-grab.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr std::size_t MAX_LOG_LINES = 10u;

// The enum is named KEY in older device SDKs and Key in current DALi.
using GrabKeyType = decltype(DALI_KEY_CURSOR_UP);

struct GrabDefinition
{
  GrabKeyType key;
  const char* name;
};

constexpr std::array<GrabDefinition, 6u> GRAB_KEYS = {{{DALI_KEY_CURSOR_UP, "Up"},
                                                       {DALI_KEY_CURSOR_DOWN, "Down"},
                                                       {DALI_KEY_CURSOR_LEFT, "Left"},
                                                       {DALI_KEY_CURSOR_RIGHT, "Right"},
                                                       {DALI_KEY_RETURN, "Return"},
                                                       {DALI_KEY_KP_ENTER, "KP_Enter"}}};

struct EventCounts
{
  uint64_t intercept{0u};
  uint64_t window{0u};
  uint64_t view{0u};
  uint64_t parent{0u};
};

Label MakeLabel(const char* text, float height, float fontSize = 13.0f)
{
  Label label = Label::New(text);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(height);
  label.SetMultiLine(true);
  label.SetFontSize(fontSize);
  label.SetTextColor(UiColor(0xE5EDF5));
  return label;
}

const char* GetViewName(View view)
{
  // Names are fixed so callers never retain a pointer into a temporary String.
  if(!view)
  {
    return "none";
  }
  const Dali::String name = view.GetProperty<Dali::String>(Actor::Property::NAME);
  if(name == "main.view")
  {
    return "main.view";
  }
  if(name == "sub.view")
  {
    return "sub.view";
  }
  if(name == "main.next")
  {
    return "main.next";
  }
  if(name == "sub.next")
  {
    return "sub.next";
  }
  return "other";
}

/**
 * @brief Observes native key delivery independently of Window and View focus.
 *
 * The main Window remains active when the sub Window is shown. All observers
 * run on the application thread and leave key delivery to DALi.
 */
class FocusKeyGrabController final : public ConnectionTracker
{
public:
  FocusKeyGrabController() = delete;

  explicit FocusKeyGrabController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &FocusKeyGrabController::OnInit);
    mApplication.TerminateSignal().Connect(this, &FocusKeyGrabController::OnTerminate);
  }

  ~FocusKeyGrabController() override
  {
    // Disconnect before member handles are destroyed and can emit signals.
    DisconnectAll();
  }

  FocusKeyGrabController(const FocusKeyGrabController&)            = delete;
  FocusKeyGrabController& operator=(const FocusKeyGrabController&) = delete;

private:
  void OnInit(Application application)
  {
    mMainWindow = application.GetWindow();
    mMainWindow.SetClass("Focus key grab: MAIN", "focus-key-grab-main");
    mMainWindow.SetBackgroundColor(UiColor(0x11263A));
    ConnectWindow(mMainWindow);

    FocusManager::Get().FocusChangedSignal().Connect(this, &FocusKeyGrabController::OnManagerFocusChanged);
    FocusManager::Get().WindowFocusChangedSignal().Connect(this, &FocusKeyGrabController::OnScopedFocusChanged);
    BuildPanel(mMainWindow, false);

    const PositionSize mainBounds = mMainWindow.GetPositionSize();
    const int          width      = std::max(1, mainBounds.width);
    const int          height     = std::max(1, mainBounds.height);
    PositionSize       subBounds(mainBounds.x + width / 3, mainBounds.y + height / 3,
                                 width * 2 / 3, height * 2 / 3);
    // Public Window::New creates a hidden Window. Set focus-skip before Show.
    mSubWindow = Window::New(subBounds, "Focus key grab: SUB");
    if(mSubWindow)
    {
      mSubWindow.SetAcceptFocus(false);
      mSubWindow.SetBackgroundColor(UiColor(0x49321C));
      ConnectWindow(mSubWindow);
      BuildPanel(mSubWindow, true);
    }
    else
    {
      AddLog("ERROR: this backend does not support multiple Windows");
    }

#if !defined(FOCUS_KEY_GRAB_TIZEN)
    AddLog("Desktop preview: native key grab is disabled; use TIZEN=ON on device");
#endif
    AddLog("Waiting for main Window focus-in before starting the scenario");
    mStatusTimer = Timer::New(250u);
    mStatusTimer.TickSignal().Connect(this, &FocusKeyGrabController::OnStatusTick);
    mStatusTimer.Start();
    mCommandTimer = Timer::New(1u);
    mCommandTimer.TickSignal().Connect(this, &FocusKeyGrabController::OnCommandTick);
    mMainWindow.Activate();
    UpdateStatus();
  }

  void ConnectWindow(Window window)
  {
    window.FocusChangedSignal().Connect(this, &FocusKeyGrabController::OnWindowFocusChanged);
    window.InterceptKeyEventSignal().Connect(this, &FocusKeyGrabController::OnInterceptKey);
    window.KeyEventSignal().Connect(this, &FocusKeyGrabController::OnWindowKey);
  }

  void BuildPanel(Window window, bool sub)
  {
    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetProperty(Actor::Property::NAME, sub ? "sub.root" : "main.root");
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetPadding(Insets(12.0f, 12.0f, 12.0f, 12.0f));
    root.SetSpacing(5.0f);
    root.KeyEventSignal().Connect(this, &FocusKeyGrabController::OnParentKey);

    root.Add(MakeLabel(sub ? "SUB: acceptsFocus=false / grabbed keys" : "MAIN: active Window / original focus", 30.0f, 17.0f));
    root.Add(MakeLabel(
      "1/Menu: show + request sub focus | 2: hide\n"
      "3: main focus | 4: sub focus | 5: grab on/off\n"
      "6: View consume | 7: independent focus on/off\n"
      "8: clear sub | 9: show only | 0: snapshot | Back/Esc: quit",
      90.0f, 11.0f));

    const std::size_t index = sub ? 1u : 0u;
    mStatusLabels[index]    = MakeLabel("Waiting for Window focus...", 160.0f, 11.0f);
    root.Add(mStatusLabels[index]);

    View view = View::New();
    view.SetProperty(Actor::Property::NAME, sub ? "sub.view" : "main.view");
    view.SetRequestedWidth(MATCH_PARENT);
    view.SetRequestedHeight(48.0f);
    view.SetFocusable(true);
    view.SetBackgroundColor(UiColor(sub ? 0xA06D35 : 0x286791));
    view.KeyEventSignal().Connect(this, &FocusKeyGrabController::OnViewKey);
    view.FocusChangedSignal().Connect(this, &FocusKeyGrabController::OnViewFocusChanged);
    Label name = MakeLabel(sub ? "sub.view: requested focus target" : "main.view: initial focus target", MATCH_PARENT, 14.0f);
    name.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    name.SetVerticalTextAlignment(Text::Alignment::CENTER);
    view.Add(name);
    root.Add(view);
    View next = View::New();
    next.SetProperty(Actor::Property::NAME, sub ? "sub.next" : "main.next");
    next.SetRequestedWidth(MATCH_PARENT);
    next.SetRequestedHeight(48.0f);
    next.SetFocusable(true);
    next.SetBackgroundColor(UiColor(sub ? 0x805D35 : 0x285771));
    next.KeyEventSignal().Connect(this, &FocusKeyGrabController::OnViewKey);
    next.FocusChangedSignal().Connect(this, &FocusKeyGrabController::OnViewFocusChanged);
    next.Add(MakeLabel(sub ? "sub.next: Right / Down" : "main.next: Right / Down", MATCH_PARENT));
    root.Add(next);
    view.SetProperty(View::Property::RIGHT_FOCUSABLE_VIEW_ID, next.GetProperty<int>(Actor::Property::ID));
    view.SetProperty(View::Property::DOWN_FOCUSABLE_VIEW_ID, next.GetProperty<int>(Actor::Property::ID));
    next.SetProperty(View::Property::LEFT_FOCUSABLE_VIEW_ID, view.GetProperty<int>(Actor::Property::ID));
    next.SetProperty(View::Property::UP_FOCUSABLE_VIEW_ID, view.GetProperty<int>(Actor::Property::ID));
    if(sub)
    {
      mSubView     = view;
      mSubNextView = next;
    }
    else
    {
      mMainView     = view;
      mMainNextView = next;
    }

    mLogLabels[index] = MakeLabel("Event log", 0.0f, 10.0f);
    mLogLabels[index].SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    root.Add(mLogLabels[index]);
    window.Add(root);
  }

  bool OnStatusTick()
  {
    if(!mScenarioStarted && mMainWindow.IsFocused())
    {
      mScenarioStarted = true;
      RequestFocus(mMainView);
      ShowSubWindow();
    }
    if(mPendingSubFocus && mSubWindow && mSubWindow.IsVisible())
    {
      mPendingSubFocus = false;
      RequestFocus(mSubView);
    }
    UpdateStatus();
    return true;
  }

  void RequestFocus(View view)
  {
    if(view)
    {
      const bool         accepted = FocusManager::Get().SetCurrentFocusView(view);
      std::ostringstream message;
      message << "SetCurrentFocusView(" << GetViewName(view) << ") accepted=" << accepted
              << " actual=" << GetViewName(FocusManager::Get().GetCurrentFocusView());
      AddLog(message.str());
      LogSnapshot();
    }
  }

  void ShowSubWindow()
  {
    if(!mSubWindow)
    {
      return;
    }
    mSubWindow.SetAcceptFocus(false);
    mSubWindow.Show();
    mSubWindow.Raise();
    if(mGrabRequested)
    {
      GrabKeys();
    }
    mPendingSubFocus = true;
    AddLog("Sub shown and raised; no sub Window activation requested");
    LogSnapshot();
  }

  void GrabKeys()
  {
#if defined(FOCUS_KEY_GRAB_TIZEN)
    for(std::size_t index = 0u; index < GRAB_KEYS.size(); ++index)
    {
      if(!mGrabbed[index])
      {
        mGrabbed[index] = KeyGrab::GrabKey(mSubWindow, GRAB_KEYS[index].key, KeyGrab::EXCLUSIVE);
        std::ostringstream message;
        message << "GrabKey(sub, " << GRAB_KEYS[index].name << ", EXCLUSIVE) result=" << mGrabbed[index];
        AddLog(message.str());
      }
    }
#else
    AddLog("GrabKey skipped: desktop preview has no Tizen key grab");
#endif
  }

  void ReleaseKeys()
  {
#if defined(FOCUS_KEY_GRAB_TIZEN)
    for(std::size_t index = 0u; index < GRAB_KEYS.size(); ++index)
    {
      if(mGrabbed[index])
      {
        const bool         released = KeyGrab::UngrabKey(mSubWindow, GRAB_KEYS[index].key);
        std::ostringstream message;
        message << "UngrabKey(sub, " << GRAB_KEYS[index].name << ") result=" << released;
        AddLog(message.str());
        if(released)
        {
          mGrabbed[index] = false;
        }
      }
    }
#endif
  }

  bool OnInterceptKey(Window window, KeyEvent event)
  {
    ++mCounts[window == mSubWindow ? 1u : 0u].intercept;
    LogKey(window == mSubWindow ? "sub.intercept" : "main.intercept", event, false);
    // Defer control commands until this event has completed normal dispatch.
    if(event.GetState() == KeyEvent::UP)
    {
      const Dali::String& name = event.GetKeyName();
      if(name == "0" || name == "1" || name == "2" || name == "3" || name == "4" ||
         name == "5" || name == "6" || name == "7" || name == "8" || name == "9" || name == "m" || IsKey(event, DALI_KEY_MENU) ||
         IsKey(event, DALI_KEY_BACK) || IsKey(event, DALI_KEY_ESCAPE))
      {
        mCommands.push_back(event);
        mCommandTimer.Start();
      }
    }
    return false;
  }

  void OnWindowKey(Window window, KeyEvent event)
  {
    ++mCounts[window == mSubWindow ? 1u : 0u].window;
    LogKey(window == mSubWindow ? "sub.window" : "main.window", event, false);
  }

  bool OnViewKey(View view, KeyEvent event)
  {
    ++mCounts[Window::Get(view) == mSubWindow ? 1u : 0u].view;
    LogKey(GetViewName(view), event, mConsumeViewKeys);
    return mConsumeViewKeys;
  }

  bool OnParentKey(View view, KeyEvent event)
  {
    const bool sub = Window::Get(view) == mSubWindow;
    ++mCounts[sub ? 1u : 0u].parent;
    LogKey(sub ? "sub.root" : "main.root", event, false);
    return false;
  }

  bool OnCommandTick()
  {
    while(!mCommands.empty())
    {
      const KeyEvent event = mCommands.front();
      mCommands.pop_front();
      const Dali::String& name = event.GetKeyName();
      if(IsKey(event, DALI_KEY_BACK) || IsKey(event, DALI_KEY_ESCAPE))
      {
        mCommands.clear();
        mApplication.Quit();
        return false;
      }
      if(name == "1" || name == "m" || IsKey(event, DALI_KEY_MENU))
      {
        ShowSubWindow();
      }
      else if(name == "2" && mSubWindow)
      {
        ReleaseKeys();
        mPendingSubFocus = false;
        mSubWindow.Hide();
        AddLog("Sub hidden; main focus should remain unchanged");
      }
      else if(name == "3")
      {
        RequestFocus(mMainView);
      }
      else if(name == "4")
      {
        RequestFocus(mSubView);
      }
      else if(name == "5" && mSubWindow)
      {
        mGrabRequested = !mGrabRequested;
        if(mGrabRequested && mSubWindow.IsVisible())
        {
          GrabKeys();
        }
        else
        {
          ReleaseKeys();
        }
      }
      else if(name == "6")
      {
        mConsumeViewKeys = !mConsumeViewKeys;
        AddLog(mConsumeViewKeys ? "View KeyEventSignal returns true" : "View KeyEventSignal returns false");
      }
      else if(name == "7" && mSubWindow)
      {
        FocusManager manager  = FocusManager::Get();
        const bool   enabled  = !manager.IsIndependentFocusEnabled(mSubWindow);
        const bool   accepted = manager.SetIndependentFocusEnabled(mSubWindow, enabled);
        AddLog(std::string("SetIndependentFocusEnabled(sub, ") + (enabled ? "true" : "false") + ") accepted=" + (accepted ? "1" : "0"));
        // Enabling alone does not focus the stored target. Use 4 explicitly.
      }
      else if(name == "8" && mSubWindow)
      {
        FocusManager::Get().ClearFocus(mSubWindow);
        AddLog("ClearFocus(sub)");
      }
      else if(name == "9" && mSubWindow)
      {
        mPendingSubFocus = false;
        mSubWindow.Show();
        mSubWindow.Raise();
        if(mGrabRequested)
        {
          GrabKeys();
        }
        AddLog("Sub shown without a focus request");
      }
      LogSnapshot();
    }
    return false;
  }

  void OnWindowFocusChanged(Window window, bool focused)
  {
    std::ostringstream message;
    message << (window == mSubWindow ? "sub" : "main") << ".WindowFocus=" << focused;
    AddLog(message.str());
    // FocusManager may process focus changes after this observer. The periodic
    // status refresh and next key log also capture the settled state.
    LogSnapshot();
  }

  void OnViewFocusChanged(View view, bool focused)
  {
    std::ostringstream message;
    message << GetViewName(view) << ".ViewFocus=" << focused;
    AddLog(message.str());
  }

  void OnManagerFocusChanged(View previous, View current)
  {
    std::ostringstream message;
    message << "FocusManager: " << GetViewName(previous) << " -> " << GetViewName(current);
    AddLog(message.str());
    UpdateStatus();
  }

  void OnScopedFocusChanged(Window window, View previous, View current)
  {
    std::ostringstream message;
    message << "WindowFocus[" << (window == mSubWindow ? "sub" : "main") << "]: "
            << GetViewName(previous) << " -> " << GetViewName(current);
    AddLog(message.str());
  }

  std::string GetSnapshot()
  {
    std::ostringstream status;
    status << "main.active=" << mMainWindow.IsFocused()
           << " sub.active=" << (mSubWindow && mSubWindow.IsFocused())
           << " sub.accept=" << (mSubWindow && mSubWindow.IsFocusAcceptable())
           << " sub.visible=" << (mSubWindow && mSubWindow.IsVisible())
           << " actual=" << GetViewName(FocusManager::Get().GetCurrentFocusView())
           << " main.actual=" << GetViewName(FocusManager::Get().GetCurrentFocusView(mMainWindow))
           << " sub.actual=" << GetViewName(FocusManager::Get().GetCurrentFocusView(mSubWindow))
           << " independent=" << FocusManager::Get().IsIndependentFocusEnabled(mSubWindow)
           << " main.view.focused=" << mMainView.GetState().Contains(ViewState::FOCUSED)
           << " main.next.focused=" << (mMainNextView && mMainNextView.GetState().Contains(ViewState::FOCUSED))
           << " sub.view.focused=" << (mSubView && mSubView.GetState().Contains(ViewState::FOCUSED))
           << " sub.next.focused=" << (mSubNextView && mSubNextView.GetState().Contains(ViewState::FOCUSED));
    status << " main.view.key=" << Extension::FocusManager::IsKeyInputTarget(mMainView)
           << " main.next.key=" << Extension::FocusManager::IsKeyInputTarget(mMainNextView)
           << " sub.view.key=" << Extension::FocusManager::IsKeyInputTarget(mSubView)
           << " sub.next.key=" << Extension::FocusManager::IsKeyInputTarget(mSubNextView);
    return status.str();
  }

  void UpdateStatus()
  {
    if(!mMainView)
    {
      return;
    }
    std::ostringstream status;
    status << "MAIN active=" << mMainWindow.IsFocused() << " nativeId=" << mMainWindow.GetNativeId() << '\n'
           << "SUB active=" << (mSubWindow && mSubWindow.IsFocused())
           << " accept=" << (mSubWindow && mSubWindow.IsFocusAcceptable())
           << " visible=" << (mSubWindow && mSubWindow.IsVisible())
           << " nativeId=" << (mSubWindow ? mSubWindow.GetNativeId() : -1) << '\n'
           << "actual=" << GetViewName(FocusManager::Get().GetCurrentFocusView())
           << " main.focused=" << mMainView.GetState().Contains(ViewState::FOCUSED)
           << " sub.focused=" << (mSubView && mSubView.GetState().Contains(ViewState::FOCUSED)) << '\n'
           << "main.actual=" << GetViewName(FocusManager::Get().GetCurrentFocusView(mMainWindow))
           << " sub.actual=" << GetViewName(FocusManager::Get().GetCurrentFocusView(mSubWindow))
           << " independent=" << FocusManager::Get().IsIndependentFocusEnabled(mSubWindow) << '\n'
           << "grab.request=" << mGrabRequested << " consume=" << mConsumeViewKeys << '\n'
           << "Grab EXCLUSIVE: ";
    for(std::size_t index = 0u; index < GRAB_KEYS.size(); ++index)
    {
      status << GRAB_KEYS[index].name << '=' << mGrabbed[index] << ' ';
    }
    for(std::size_t index = 0u; index < mCounts.size(); ++index)
    {
      const EventCounts& counts = mCounts[index];
      status << '\n'
             << (index == 0u ? "MAIN" : "SUB")
             << " intercept=" << counts.intercept << " view=" << counts.view
             << " root=" << counts.parent << " window=" << counts.window;
    }
    const std::string text = status.str();
    for(Label label : mStatusLabels)
    {
      if(label)
      {
        label.SetText(text.c_str());
      }
    }
  }

  void LogSnapshot()
  {
    AddLog("STATE " + GetSnapshot());
    UpdateStatus();
  }

  void LogKey(const char* source, const KeyEvent& event, bool consumed)
  {
    std::ostringstream message;
    message << source << " key=" << event.GetKeyName().CStr()
            << " logical=" << event.GetLogicalKey().CStr()
            << ' ' << (event.GetState() == KeyEvent::DOWN ? "DOWN" : "UP")
            << " code=" << event.GetKeyCode() << " event.windowId=" << event.GetWindowId()
            << " time=" << event.GetTime() << " repeat=" << event.IsRepeat()
            << " consumed=" << consumed << ' ' << GetSnapshot();
    AddLog(message.str());
    UpdateStatus();
  }

  void AddLog(const std::string& message)
  {
    std::ostringstream line;
    line << '#' << ++mLogSequence << ' ' << message;
    DALI_LOG_RELEASE_INFO("FocusKeyGrab %s\n", line.str().c_str());
    mRecentLogs.push_back(line.str());
    if(mRecentLogs.size() > MAX_LOG_LINES)
    {
      mRecentLogs.pop_front();
    }
    // Keep full state in dlog; shorten on-screen rows to leave room for routing.
    std::ostringstream text;
    for(const std::string& recent : mRecentLogs)
    {
      text << recent.substr(0u, 115u) << '\n';
    }
    const std::string logText = text.str();
    for(Label label : mLogLabels)
    {
      if(label)
      {
        label.SetText(logText.c_str());
      }
    }
  }

  void OnTerminate(Application /*application*/)
  {
    if(mStatusTimer)
    {
      mStatusTimer.Stop();
    }
    if(mCommandTimer)
    {
      mCommandTimer.Stop();
    }
    ReleaseKeys();
    if(mSubWindow)
    {
      FocusManager::Get().SetIndependentFocusEnabled(mSubWindow, false);
    }
  }

private:
  Application&                       mApplication;
  Window                             mMainWindow;
  Window                             mSubWindow;
  View                               mMainView;
  View                               mSubView;
  View                               mMainNextView;
  View                               mSubNextView;
  std::array<Label, 2u>              mStatusLabels;
  std::array<Label, 2u>              mLogLabels;
  std::array<EventCounts, 2u>        mCounts;
  std::array<bool, GRAB_KEYS.size()> mGrabbed{};
  Timer                              mStatusTimer;
  Timer                              mCommandTimer;
  std::deque<KeyEvent>               mCommands;
  std::deque<std::string>            mRecentLogs;
  uint64_t                           mLogSequence{0u};
  bool                               mScenarioStarted{false};
  bool                               mGrabRequested{true};
  bool                               mConsumeViewKeys{false};
  bool                               mPendingSubFocus{false};
};
} // namespace

int DALI_EXPORT_API main(int argc, char** argv)
{
  WindowData windowData;
  windowData.SetTransparency(false);
#if !defined(FOCUS_KEY_GRAB_TIZEN)
  PositionSize bounds(0, 0, 960, 900);
  windowData.SetPositionSize(bounds);
#endif
  Application            application = Application::New(&argc, &argv, "", false, windowData);
  FocusKeyGrabController controller(application);
  application.MainLoop();
  return 0;
}
