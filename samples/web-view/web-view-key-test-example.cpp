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

/**
 * @file web-view-key-test-example.cpp
 * @brief Offline TV remote test using res/key-test.html.
 *
 * Remote arrows move the HTML selection; Enter activates it; Back exits.
 * 1/W focuses the WebView, 2/N focuses a native target, 3/R reloads the page,
 * and 0/S logs a focus snapshot. Commands run after key dispatch completes.
 * Window and View counters diagnose dispatch before the DOM event log.
 * Focus changes use FocusManager; no key injection or per-key repair is used.
 */

#include <deque>
#include <fstream>
#include <iterator>
#include <string>

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/extension-api/focus-manager.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali-ui-foundation/public-api/views/web/web-view.h>
#include <dali/devel-api/adaptor-framework/application.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/adaptor-framework/key.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
const char* HTML_PATH = RESOURCES_DIR "key-test.html";

Label MakeStatusLabel(const char* text, float height = 32.0f)
{
  Label label = Label::New(text);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(height);
  label.SetTextColor(Color::WHITE);
  label.SetFontSize(18.0f);
  label.SetMultiLine(true);
  return label;
}
} // namespace

class WebViewKeyTestController : public ConnectionTracker
{
public:
  explicit WebViewKeyTestController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &WebViewKeyTestController::OnInit);
    mApplication.TerminateSignal().Connect(this, &WebViewKeyTestController::OnTerminate);
  }

  ~WebViewKeyTestController() override
  {
    // Disconnect before member handles are destroyed and can emit signals.
    DisconnectAll();
    if(mCommandTimer)
    {
      mCommandTimer.Stop();
    }
  }

private:
  enum class Command
  {
    WEB_FOCUS,
    NATIVE_FOCUS,
    RELOAD,
    SNAPSHOT,
    QUIT
  };

  void OnInit(Application application)
  {
    mWindow = application.GetWindow();
    mWindow.SetClass("WebView key focus test", "web-view-key-test");
    mWindow.SetBackgroundColor(Color::BLACK);

    mStatus       = MakeStatusLabel("Loading offline TV remote test.");
    Label guide   = MakeStatusLabel("1/W: WebView | 2/N: native | 3/R: reload | 0/S: snapshot | arrows / Enter | Back/Esc: exit");
    mFocusStatus  = MakeStatusLabel("WebView focus: waiting", 52.0f);
    mWindowKeys   = MakeStatusLabel("Window keys: 0");
    mViewKeys     = MakeStatusLabel("WebView keys: 0");
    mNativeTarget = MakeStatusLabel("Native keys: 0 | press 2/N to test WebView focus loss", 36.0f);
    mNativeTarget.SetProperty(Actor::Property::NAME, "key-test.native");
    mNativeTarget.SetFocusable(true);
    mNativeTarget.SetBackgroundColor(UiColor(0x29384D));
    mNativeTarget.FocusChangedSignal().Connect(this, &WebViewKeyTestController::OnNativeFocusChanged);
    mNativeTarget.KeyEventSignal().Connect(this, &WebViewKeyTestController::OnNativeKey);

    mWebView = WebView::New();
    mWebView.SetProperty(Actor::Property::NAME, "key-test.web");
    mWebView.SetRequestedWidth(MATCH_PARENT);
    mWebView.SetRequestedHeight(0.0f);
    mWebView.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    mWebView.SetFocusable(true);
    mWebView.SetKeyEventsEnabled(true);
    mWebView.FocusChangedSignal().Connect(this, &WebViewKeyTestController::OnWebFocusChanged);
    mWebView.KeyEventSignal().Connect(this, &WebViewKeyTestController::OnWebKey);
    mWebView.PageLoadStartedSignal().Connect(this, &WebViewKeyTestController::OnPageLoading);
    mWebView.PageLoadFinishedSignal().Connect(this, &WebViewKeyTestController::OnPageLoaded);
    mWebView.PageLoadErrorSignal().Connect(this, &WebViewKeyTestController::OnPageError);
    mWebView.WebProcessCrashedSignal().Connect(this, &WebViewKeyTestController::OnWebProcessCrashed);

    // Even if a backend leaves an arrow unconsumed, DALi navigation must not
    // move to the native target while testing navigation inside the HTML page.
    FocusManager::Get().SetAsFocusGroup(mWebView, true);
    mRequestedTarget = mWebView;

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetPadding(Insets(12.0f, 12.0f, 12.0f, 12.0f));
    root.SetSpacing(4.0f);
    root.Add(mStatus);
    root.Add(guide);
    root.Add(mFocusStatus);
    root.Add(mWindowKeys);
    root.Add(mViewKeys);
    root.Add(mNativeTarget);
    root.Add(mWebView);
    mWindow.Add(root);
    mWindow.InterceptKeyEventSignal().Connect(this, &WebViewKeyTestController::OnInterceptKey);
    mWindow.FocusChangedSignal().Connect(this, &WebViewKeyTestController::OnWindowFocusChanged);

    mCommandTimer = Timer::New(1u);
    mCommandTimer.TickSignal().Connect(this, &WebViewKeyTestController::OnCommandTick);
    UpdateFocusStatus();
    LoadTestPage();
  }

  void OnTerminate(Application)
  {
    if(mCommandTimer)
    {
      mCommandTimer.Stop();
    }
    mCommands.clear();
    DisconnectAll();
  }

  void LoadTestPage()
  {
    mPageLoaded      = false;
    mRequestedTarget = mWebView;
    mStatus.SetText("Loading key-test.html from sample resources...");
    // Read the installed resource without backend-specific file URL rules.
    std::ifstream file(HTML_PATH, std::ios::binary);
    if(!file)
    {
      mStatus.SetText("Cannot open key-test.html; check sample resources.");
      DALI_LOG_ERROR("[WebViewKeyTest] Cannot open %s\n", HTML_PATH);
      return;
    }
    std::string html((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    if(file.bad() || html.empty())
    {
      mStatus.SetText("Cannot read key-test.html or the resource is empty.");
      DALI_LOG_ERROR("[WebViewKeyTest] Invalid HTML resource: %s\n", HTML_PATH);
      return;
    }
    DALI_LOG_RELEASE_INFO("[WebViewKeyTest] Loading %s (%zu bytes)\n", HTML_PATH, html.size());
    mWebView.LoadHtmlString(Dali::String(html.c_str()));
  }

  const char* TargetName(View view) const
  {
    return view == mWebView ? "WebView" : view == mNativeTarget ? "native"
                                        : view                  ? "other"
                                                                : "none";
  }

  void UpdateFocusStatus()
  {
    View        actual    = FocusManager::Get().GetCurrentFocusView(mWindow);
    const char* keyTarget = Extension::FocusManager::IsKeyInputTarget(mWebView)        ? "WebView"
                            : Extension::FocusManager::IsKeyInputTarget(mNativeTarget) ? "native"
                                                                                       : "none/other";
    std::string text      = std::string("Window: ") + (mWindow.IsFocused() ? "ACTIVE" : "INACTIVE") +
                       " | navigation: " + TargetName(actual) + " | key target: " + keyTarget +
                       "\nWebView focus: " + (mWebFocused ? "GAINED" : "LOST") +
                       " | gains=" + std::to_string(mWebFocusGains) + " losses=" + std::to_string(mWebFocusLosses);
    mFocusStatus.SetText(text.c_str());
  }

  void LogSnapshot()
  {
    UpdateFocusStatus();
    DALI_LOG_RELEASE_INFO("[WebViewKeyTest] Snapshot windowFocused=%d requested=%s actual=%s webKeyTarget=%d nativeKeyTarget=%d webFocused=%d gains=%u losses=%u loaded=%d WindowKeys=%u WebViewKeys=%u NativeKeys=%u\n",
                          mWindow.IsFocused(), TargetName(mRequestedTarget),
                          TargetName(FocusManager::Get().GetCurrentFocusView(mWindow)),
                          Extension::FocusManager::IsKeyInputTarget(mWebView),
                          Extension::FocusManager::IsKeyInputTarget(mNativeTarget),
                          mWebFocused, mWebFocusGains, mWebFocusLosses, mPageLoaded,
                          mWindowKeyCount, mViewKeyCount, mNativeKeyCount);
  }

  void RequestFocus(View target)
  {
    mRequestedTarget = target;
    if(target == mWebView && !mPageLoaded)
    {
      mStatus.SetText("WebView focus waits for page load. Press 3/R to retry loading.");
      return;
    }
    auto        manager  = FocusManager::Get();
    bool        accepted = manager.SetCurrentFocusView(target);
    bool        actual   = manager.GetCurrentFocusView(mWindow) == target;
    std::string message  = std::string(TargetName(target)) +
                          (!accepted ? " focus request REJECTED."
                           : actual  ? " focus applied. Use arrows / Enter; 1/W returns to WebView."
                                     : " focus reserved; waiting for Window activation.");
    mStatus.SetText(message.c_str());
    DALI_LOG_RELEASE_INFO("[WebViewKeyTest] Focus request target=%s accepted=%d actual=%d\n",
                          TargetName(target), accepted, actual);
    LogSnapshot();
  }

  void ShowKey(Label label, const char* stage, unsigned int count, const KeyEvent& event)
  {
    std::string text = stage;
    text += " keys: " + std::to_string(count) + " | ";
    text += event.GetKeyName().CStr();
    text += " code=" + std::to_string(event.GetKeyCode());
    text += event.GetState() == KeyEvent::DOWN ? " DOWN" : " UP";
    label.SetText(text.c_str());
    DALI_LOG_RELEASE_INFO("[WebViewKeyTest] %s time=%u event.windowId=%u windowFocused=%d webKeyTarget=%d\n",
                          text.c_str(), event.GetTime(), event.GetWindowId(), mWindow.IsFocused(),
                          Extension::FocusManager::IsKeyInputTarget(mWebView));
  }

  bool OnInterceptKey(Window, KeyEvent event)
  {
    ShowKey(mWindowKeys, "Window", ++mWindowKeyCount, event);
    Command             command;
    const Dali::String& name = event.GetKeyName();
    if(IsKey(event, DALI_KEY_BACK) || IsKey(event, DALI_KEY_ESCAPE))
    {
      command = Command::QUIT;
    }
    else if(name == "1" || name == "KP_1" || name == "w" || name == "W")
    {
      command = Command::WEB_FOCUS;
    }
    else if(name == "2" || name == "KP_2" || name == "n" || name == "N")
    {
      command = Command::NATIVE_FOCUS;
    }
    else if(name == "3" || name == "KP_3" || name == "r" || name == "R")
    {
      command = Command::RELOAD;
    }
    else if(name == "0" || name == "KP_0" || name == "s" || name == "S")
    {
      command = Command::SNAPSHOT;
    }
    else
    {
      // Observe only. Arrows and Enter follow normal dispatch to the focused View.
      return false;
    }

    // Consume both states, then change focus only after the command's UP event.
    if(event.GetState() == KeyEvent::UP)
    {
      mCommands.push_back(command);
      mCommandTimer.Start();
    }
    return true;
  }

  bool OnCommandTick()
  {
    while(!mCommands.empty())
    {
      Command command = mCommands.front();
      mCommands.pop_front();
      switch(command)
      {
        case Command::WEB_FOCUS:
          RequestFocus(mWebView);
          break;
        case Command::NATIVE_FOCUS:
          RequestFocus(mNativeTarget);
          break;
        case Command::RELOAD:
          LoadTestPage();
          break;
        case Command::SNAPSHOT:
          LogSnapshot();
          break;
        case Command::QUIT:
          mCommands.clear();
          mApplication.Quit();
          return false;
      }
    }
    return false;
  }

  bool OnWebKey(View, KeyEvent event)
  {
    ShowKey(mViewKeys, "WebView", ++mViewKeyCount, event);
    return false;
  }

  bool OnNativeKey(View, KeyEvent event)
  {
    ShowKey(mNativeTarget, "Native", ++mNativeKeyCount, event);
    // Keep arrows/Enter here until an explicit 1/W command returns to WebView.
    return true;
  }

  void OnWebFocusChanged(View, bool focused)
  {
    mWebFocused = focused;
    if(focused)
    {
      ++mWebFocusGains;
    }
    else
    {
      ++mWebFocusLosses;
    }
    UpdateFocusStatus();
    DALI_LOG_RELEASE_INFO("[WebViewKeyTest] WebView focus %s\n", focused ? "GAINED" : "LOST");
  }

  void OnNativeFocusChanged(View, bool focused)
  {
    mNativeTarget.SetBackgroundColor(UiColor(focused ? 0x665C2D : 0x29384D));
    UpdateFocusStatus();
    DALI_LOG_RELEASE_INFO("[WebViewKeyTest] Native focus %s\n", focused ? "GAINED" : "LOST");
  }

  void OnWindowFocusChanged(Window, bool focused)
  {
    DALI_LOG_RELEASE_INFO("[WebViewKeyTest] Window focus %s\n", focused ? "GAINED" : "LOST");
    if(focused && (mRequestedTarget != mWebView || mPageLoaded))
    {
      RequestFocus(mRequestedTarget);
    }
    UpdateFocusStatus();
  }

  void OnPageLoading(WebView, const Dali::String& url)
  {
    mPageLoaded = false;
    mStatus.SetText("Loading offline page...");
    DALI_LOG_RELEASE_INFO("[WebViewKeyTest] Page load started: %s\n", url.CStr());
  }

  void OnPageLoaded(WebView, const Dali::String& url)
  {
    mPageLoaded = true;
    DALI_LOG_RELEASE_INFO("[WebViewKeyTest] Page load finished: %s\n", url.CStr());
    RequestFocus(mRequestedTarget);
  }

  void OnPageError(WebView, const WebViewPageLoadError& error)
  {
    mPageLoaded = false;
    mStatus.SetText(error.GetDescription());
    DALI_LOG_ERROR("[WebViewKeyTest] Page load error: url=%s code=%d description=%s\n",
                   error.GetUrl().CStr(), static_cast<int>(error.GetCode()), error.GetDescription().CStr());
  }

  void OnWebProcessCrashed(WebView)
  {
    mPageLoaded = false;
    mStatus.SetText("Web process crashed. Press 3/R to retry loading.");
    DALI_LOG_ERROR("[WebViewKeyTest] Web process crashed\n");
  }

  Application&        mApplication;
  Window              mWindow;
  WebView             mWebView;
  Label               mStatus;
  Label               mFocusStatus;
  Label               mWindowKeys;
  Label               mViewKeys;
  Label               mNativeTarget;
  View                mRequestedTarget;
  Timer               mCommandTimer;
  std::deque<Command> mCommands;
  unsigned int        mWindowKeyCount{0};
  unsigned int        mViewKeyCount{0};
  unsigned int        mNativeKeyCount{0};
  unsigned int        mWebFocusGains{0};
  unsigned int        mWebFocusLosses{0};
  bool                mWebFocused{false};
  bool                mPageLoaded{false};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig    config      = UiConfig::New();
  config.Apply();
#if defined(_WIN32)
  WebView::SetWebEngineType(WebEngineType::LWE);
#else
  WebView::SetWebEngineType(WebEngineType::CHROMIUM);
#endif
  WebViewKeyTestController controller(application);
  application.MainLoop();
  return 0;
}
