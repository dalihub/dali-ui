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

#include "text-test-case.h"

#include <dali/devel-api/adaptor-framework/application.h>
#include <unistd.h>
#include <cstdio>
#include <string>
#include <utility>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
struct Options
{
  bool        automatic{false};
  bool        list{false};
  bool        help{false};
  std::string test;
  std::string report{"/tmp/dali-ui-text-" + std::to_string(getpid()) + ".log"};
};

bool ParseOptions(int& argc, char** argv, Options& options)
{
  int daliArgument = 1;
  for(int i = 1; i < argc; ++i)
  {
    const std::string argument(argv[i]);
    if(argument == "--auto-verify")
      options.automatic = true;
    else if(argument == "--list")
      options.list = true;
    else if(argument == "--help" || argument == "-h")
      options.help = true;
    else if(argument == "--test" || argument == "--report")
    {
      if(i + 1 == argc || argv[i + 1][0] == '\0' || argv[i + 1][0] == '-')
      {
        std::fprintf(stderr, "%s requires a value\n", argv[i]);
        return false;
      }
      (argument == "--test" ? options.test : options.report) = argv[++i];
    }
    else
      argv[daliArgument++] = argv[i]; // Preserve DALi window/platform options.
  }
  argc       = daliArgument;
  argv[argc] = nullptr;
  if(options.automatic && options.test.empty()) options.test = "text-layout";
  return true;
}

class TextTestLauncher : public ConnectionTracker
{
public:
  TextTestLauncher(Application& application, Options options)
  : mApplication(application),
    mOptions(std::move(options))
  {
    mApplication.InitSignal().Connect(this, &TextTestLauncher::OnInit);
  }

  ~TextTestLauncher()
  {
    LeaveTest();
  }

  int GetExitCode() const
  {
    // Also preserve a completed result if the native window closes before
    // the launcher has polled it. An unfinished run must never return PASS.
    if(mOptions.automatic && mActive && mExitCode == 0)
    {
      return mActive->IsAutomationFinished() ? mActive->GetExitCode() : 3;
    }
    return mExitCode;
  }

private:
  Label Text(const char* text, float fontSize = 18.0f)
  {
    auto label = Label::New(text);
    label.SetFontSize(fontSize);
    label.SetMultiLine(true);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(WRAP_CONTENT);
    label.SetTextColor(UiColor(0x172B4D));
    return label;
  }

  Label Header(const char* text, const char* id)
  {
    auto label = Text(text);
    label.SetRequestedHeight(48.0f);
    label.SetBackgroundColor(UiColor(0x1565C0));
    label.SetTextColor(UiColor(0xFFFFFF));
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    label.SetPadding(Insets(12.0f, 12.0f, 4.0f, 4.0f));
    label.SetAutomationId(id);
    return label;
  }

  StackLayout TestItem(TextManualTest::TestCase& test)
  {
    const std::string id   = test.GetId();
    auto              item = StackLayout::New(StackOrientation::VERTICAL);
    item.SetRequestedWidth(MATCH_PARENT);
    item.SetRequestedHeight(WRAP_CONTENT);
    item.SetPadding(Insets(18.0f, 18.0f, 14.0f, 14.0f));
    item.SetSpacing(6.0f);
    item.SetBackgroundColor(UiColor(0xFFFFFF));
    item.SetCornerRadius(12.0f);
    item.SetBorderlineWidth(1.0f);
    item.SetBorderlineColor(UiColor(0xE2E8F0));
    item.SetFocusable(true);
    item.SetAutomationId(id.c_str());
    item.SetAccessibilityName(test.GetName());
    item.SetAccessibilityDescription(test.GetDescription());
    item.SetStateEffect(OverlayEffect::ListItem());

    Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
    gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
    gradient.SetStopNodes({
      Gradient::StopNode(0.0f, UiColor(0x0284C7)),
      Gradient::StopNode(0.3f, UiColor(0x2563EB)),
      Gradient::StopNode(0.65f, UiColor(0x7C3AED)),
      Gradient::StopNode(1.0f, UiColor(0xDB2777)),
    });
    auto name = Text(test.GetName().CStr(), 20.0f);
    name.SetFontWeight(Text::FontWeight::BOLD);
    name.SetTextGradientBoundsMode(Text::GradientBoundsMode::CONTENT_BOUND);
    name.SetTextGradient(gradient);
    item.Add(name);

    auto description = Text(test.GetDescription().CStr(), 14.0f);
    description.SetTextColor(UiColor(0x6B7280));
    item.Add(description);

    auto interactive = item.AsInteractive();
    interactive.SetClickable(true);
    interactive.ClickedSignal().Connect(this, [this, id](View, InputEvent)
    { EnterTest(id); });
    return item;
  }

  void OnInit(Application application)
  {
    mWindow = application.GetWindow();
    // Command-line automation should not consume keys intended for another app.
    if(mOptions.automatic) mWindow.SetAcceptFocus(false);
    mWindow.SetBackgroundColor(UiColor(0xF5F5F5));
    mWindow.KeyEventSignal().Connect(this, &TextTestLauncher::OnKey);
    mRoot = StackLayout::New(StackOrientation::VERTICAL);
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(MATCH_PARENT);
    mWindow.Add(mRoot);
    if(mOptions.test.empty())
      ShowList();
    else
      EnterTest(mOptions.test);
  }

  void LeaveTest()
  {
    if(mCompletionTimer)
    {
      mCompletionTimer.Stop();
      mCompletionTimer.Reset();
    }
    if(mActive)
    {
      mActive->OnExit();
      mActive.reset();
    }
  }

  void ShowList()
  {
    LeaveTest();
    mRoot.RemoveAll();
    auto title = Header("DALi UI Text · Manual Tests", "text-tests-title");
    mRoot.Add(title);
    auto content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(WRAP_CONTENT);
    content.SetPadding(Insets(10.0f, 10.0f, 10.0f, 10.0f));
    content.SetSpacing(8.0f);
    auto cases = TextManualTest::CreateOrderedCases();
    for(auto& item : cases)
    {
      auto* test = dynamic_cast<TextManualTest::TestCase*>(item.get());
      if(!test) continue;
      content.Add(TestItem(*test));
    }
    auto scroll = ScrollView::New();
    scroll.SetRequestedWidth(MATCH_PARENT);
    scroll.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    scroll.SetScrollDirection(ScrollDirection::Vertical);
    scroll.SetContent(content);
    mRoot.Add(scroll);
  }

  void EnterTest(const std::string& id)
  {
    LeaveTest();
    auto cases = TextManualTest::CreateOrderedCases();
    for(auto& item : cases)
    {
      auto* test = dynamic_cast<TextManualTest::TestCase*>(item.get());
      if(test && id == test->GetId())
      {
        mActive.reset(static_cast<TextManualTest::TestCase*>(item.release()));
        break;
      }
    }
    if(!mActive)
    {
      std::fprintf(stderr, "Unknown test: %s\n", id.c_str());
      mExitCode = 2;
      QuitOnNextTick();
      return;
    }
    mRoot.RemoveAll();
    auto back = Header(("< Back · " + std::string(mActive->GetName().CStr())).c_str(), "text-tests-back");
    back.SetFocusable(true);
    back.SetStateEffect(OverlayEffect::Plain());
    auto backInteractive = back.AsInteractive();
    backInteractive.SetClickable(true);
    backInteractive.ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      if(mOptions.automatic)
      {
        mExitCode = 3;
        mApplication.Quit();
      }
      else
        ShowList();
    });
    mRoot.Add(back);
    auto content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    mRoot.Add(content);
    mActive->Configure(mWindow, mOptions.report);
    mActive->OnEnter(content);
    if(mOptions.automatic)
    {
      mActive->StartAutomation();
      mCompletionTimer = Timer::New(100u);
      mCompletionTimer.TickSignal().Connect(this, &TextTestLauncher::OnCompletion);
      mCompletionTimer.Start();
    }
  }

  bool OnCompletion()
  {
    if(mActive && !mActive->IsAutomationFinished()) return true;
    if(mActive) mExitCode = mActive->GetExitCode();
    mApplication.Quit();
    return false;
  }

  void QuitOnNextTick()
  {
    mCompletionTimer = Timer::New(1u);
    mCompletionTimer.TickSignal().Connect(this, &TextTestLauncher::OnCompletion);
    mCompletionTimer.Start();
  }

  void OnKey(Window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::UP) return;
    if(IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK))
    {
      if(mOptions.automatic)
      {
        mExitCode = 3;
        mApplication.Quit();
      }
      else if(mActive)
        ShowList();
      else
        mApplication.Quit();
      return;
    }
    if(mActive) mActive->OnKeyEvent(event);
  }

  Application&                              mApplication;
  Options                                   mOptions;
  Window                                    mWindow;
  StackLayout                               mRoot;
  Timer                                     mCompletionTimer;
  std::unique_ptr<TextManualTest::TestCase> mActive;
  int                                       mExitCode{0};
};
} // namespace

int DALI_EXPORT_API main(int argc, char** argv)
{
  Options options;
  if(!ParseOptions(argc, argv, options)) return 2;
  if(options.help)
  {
    std::puts(
      "Usage: manual-test-dali-ui-text [--list] [--test ID] [--auto-verify] [--report PATH]\n"
      "GUI: select a test, click Run auto or press V (Sync + Async).\n"
      "--auto-verify: run the same checks and exit 0=PASS, 1=FAIL, 2=error, 3=cancel.\n"
      "Requires a graphical session for rendering, including automatic mode.");
    return 0;
  }
  if(options.list)
  {
    auto cases = TextManualTest::CreateOrderedCases();
    for(const auto& item : cases)
    {
      auto* test = dynamic_cast<TextManualTest::TestCase*>(item.get());
      if(test) std::printf("%s\t%s\n", test->GetId(), test->GetName().CStr());
    }
    return 0;
  }
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  TextTestLauncher launcher(application, options);
  application.MainLoop();
  return launcher.GetExitCode();
}
