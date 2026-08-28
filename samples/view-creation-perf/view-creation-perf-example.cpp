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

#include <chrono>
#include <cstdio>
#include <cstdint>
#include <string>

namespace
{
using namespace Dali;
using namespace Dali::Ui;

void SetStandaloneGeometry(View view, float x, float y, float width, float height)
{
  view.SetLayoutMode(LayoutMode::STANDALONE);
  view.SetRequestedX(x);
  view.SetRequestedY(y);
  view.SetRequestedWidth(width);
  view.SetRequestedHeight(height);
}

void SetRequestedGeometry(View view, float x, float y, float width, float height)
{
  view.SetRequestedX(x);
  view.SetRequestedY(y);
  view.SetRequestedWidth(width);
  view.SetRequestedHeight(height);
}

// The timing labels keep their bottom edge pinned and grow upwards, so the most
// recent entry is always the bottom line and older entries scroll off the top.
constexpr float LOG_LABEL_WIDTH         = 900.0f;
constexpr float LOG_LABEL_BOTTOM_MARGIN = 20.0f;

class ViewCreationPerf : public ConnectionTracker
{
public:
  explicit ViewCreationPerf(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &ViewCreationPerf::Create);
  }

  void Create(Application application)
  {
    mWindow = application.GetWindow();
    mWindow.SetBackgroundColor(Color::WHITE);

    const PositionSize windowPositionSize = mWindow.GetPositionSize();
    mWindowSize = Vector2(static_cast<float>(windowPositionSize.width),
                          static_cast<float>(windowPositionSize.height));

    mDefaultView = View::New();
    SetStandaloneGeometry(mDefaultView, 0.0f, 0.0f, 10.0f, 10.0f);
    mDefaultView.SetBackgroundColor(UiColor(Color::RED));
    mWindow.Add(mDefaultView);

    CreateRoot();

    CreateMarkers();

    mLogTimeString = "Time Log";
    mLogTimeLabel  = Label::New(mLogTimeString.c_str());
    ConfigureLogLabel(mLogTimeLabel, mWindowSize.x - 920.0f);
    mWindow.Add(mLogTimeLabel);

    mAverageTimeLabel = Label::New();
    ConfigureLogLabel(mAverageTimeLabel, 20.0f);
    UpdateAverageLabel();
    mWindow.Add(mAverageTimeLabel);

    mWindow.GetRootLayer().TouchEventSignal().Connect(this, &ViewCreationPerf::OnTouch);
    mWindow.KeyEventSignal().Connect(this, &ViewCreationPerf::OnKeyEvent);
  }

  bool OnTouch(Actor, TouchEvent)
  {
    CreateMarkers();
    mWindow.Add(mStartMarkerView);
    mWindow.Add(mEndMarkerView);
    return true;
  }

  void OnKeyEvent(Window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::DOWN)
    {
      return;
    }

    if(IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK) || IsKey(event, DALI_KEY_BACKSPACE))
    {
      BeginQuit();
      return;
    }

    ResetScene();

    const std::string key = event.GetKeyString().CStr();
    if(key == "9")
    {
      ToggleLogLabels();
      return;
    }

    const bool create100 = key == "1" || key == "3" || key == "5";
    if(key == "1" || key == "2")
    {
      CreatePlainViews(create100);
    }
    else if(key == "3" || key == "4")
    {
      CreateViewsWithRenderer(create100);
    }
    else if(key == "5" || key == "6")
    {
      CreateViewsWithColor(create100);
    }

    UpdateAverageLabel();
  }

private:
  void BeginQuit()
  {
    // Release the potentially large scene before Application shuts Core down.
    // Waiting one tick gives Core a chance to process the node removals.
    mStartMarkerView.Reset();
    mEndMarkerView.Reset();

    if(mRoot)
    {
      mRoot.Unparent();
      mRoot.Reset();
    }
    if(mDefaultView)
    {
      mDefaultView.Unparent();
      mDefaultView.Reset();
    }
    if(mLogTimeLabel)
    {
      mLogTimeLabel.Unparent();
      mLogTimeLabel.Reset();
    }
    if(mAverageTimeLabel)
    {
      mAverageTimeLabel.Unparent();
      mAverageTimeLabel.Reset();
    }

    mQuitTimer = Timer::New(1u);
    mQuitTimer.TickSignal().Connect(this, &ViewCreationPerf::QuitAfterSceneCleanup);
    mQuitTimer.Start();
  }

  bool QuitAfterSceneCleanup()
  {
    mApplication.Quit();
    return false;
  }

  void ConfigureLogLabel(Label label, float x)
  {
    // WRAP_CONTENT height: the label is as tall as its text, never clipping it.
    SetStandaloneGeometry(label, x, 0.0f, LOG_LABEL_WIDTH, WRAP_CONTENT);
    label.SetMultiLine(true);
    label.SetTextColor(UiColor(Color::BLACK));
    label.SetFontSize(18.0f);
    label.SetHorizontalTextAlignment(Text::Alignment::START);
    label.SetVerticalTextAlignment(Text::Alignment::END);
    PinLabelBottom(label);
  }

  // Keeps the label's bottom edge a fixed distance from the window bottom, so
  // added lines push the earlier ones upwards instead of overflowing downwards.
  void PinLabelBottom(Label label)
  {
    if(!label)
    {
      return;
    }
    const float height = label.GetHeightForWidth(LOG_LABEL_WIDTH);
    label.SetRequestedY(mWindowSize.y - LOG_LABEL_BOTTOM_MARGIN - height);
  }

  // A WRAP_CONTENT height is still capped at the parent's height, and text past
  // that point would be ellipsized away - which would drop the newest entries.
  // Discard the oldest lines instead, so the newest one always stays visible on
  // the bottom line and earlier ones scroll off the top.
  void TrimLogToWindow()
  {
    const float available = mWindowSize.y - LOG_LABEL_BOTTOM_MARGIN;
    while(mLogTimeLabel.GetHeightForWidth(LOG_LABEL_WIDTH) > available)
    {
      const std::size_t lineEnd = mLogTimeString.find('\n');
      if(lineEnd == std::string::npos)
      {
        break;
      }
      mLogTimeString.erase(0, lineEnd + 1);
      mLogTimeLabel.SetText(mLogTimeString.c_str());
    }
  }

  void CreateMarkers()
  {
    mStartMarkerView = View::New();
    SetStandaloneGeometry(mStartMarkerView, 0.0f, 0.0f, 10.0f, 10.0f);
    mStartMarkerView.SetBackgroundColor(UiColor(Color::BLUE));

    mEndMarkerView = View::New();
    SetStandaloneGeometry(mEndMarkerView, 0.0f, 0.0f, 10.0f, 10.0f);
    mEndMarkerView.SetBackgroundColor(UiColor(Color::RED));
  }

  void CreateRoot()
  {
    mRoot = View::New();
    SetStandaloneGeometry(mRoot, 0.0f, 0.0f, 10.0f, 10.0f);
    mWindow.Add(mRoot);
  }

  void ResetScene()
  {
    mStartMarkerView.Unparent();
    mEndMarkerView.Unparent();
    mDefaultView.Unparent();
    mRoot.Unparent();
    mRoot.Reset();

    mWindow.Add(mDefaultView);
    CreateRoot();

    if(mLogTimeLabel)
    {
      mLogTimeLabel.Unparent();
      mWindow.Add(mLogTimeLabel);
    }
    if(mAverageTimeLabel)
    {
      mAverageTimeLabel.Unparent();
      mWindow.Add(mAverageTimeLabel);
    }
  }

  void AttachMarkers(View firstView, View lastView)
  {
    if(firstView)
    {
      firstView.Add(mStartMarkerView);
    }
    if(lastView)
    {
      lastView.Add(mEndMarkerView);
    }
  }

  void CreatePlainViews(bool create100)
  {
    View    firstView;
    View    lastView;
    int     lowCount = create100 ? 10 : 100;
    Vector2 position(50.0f, 50.0f);

    const auto start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < lowCount; ++i)
    {
      position.x = 50.0f;
      for(int j = 0; j < lowCount; ++j)
      {
        View view = View::New();
        SetRequestedGeometry(view, position.x, position.y, 10.0f, 10.0f);
        mRoot.Add(view);

        if(!firstView)
        {
          firstView = view;
        }
        lastView = view;
        position.x += 10.0f;
      }
      position.y += 10.0f;
    }
    const auto end = std::chrono::high_resolution_clock::now();

    const uint64_t duration = DurationMicroseconds(start, end);
    AttachMarkers(firstView, lastView);
    AppendResult("View", create100, duration);
    if(create100)
    {
      mTimeView100 += duration;
      ++mCountView100;
    }
    else
    {
      mTimeView10000 += duration;
      ++mCountView10000;
    }
  }

  void CreateViewsWithRenderer(bool create100)
  {
    Renderer defaultRenderer = mDefaultView.GetRendererAt(0u);
    Geometry geometry        = defaultRenderer.GetGeometry();
    Shader   shader          = defaultRenderer.GetShader();
    View     firstView;
    View     lastView;
    int      lowCount = create100 ? 10 : 100;
    Vector2  position(50.0f, 50.0f);

    const auto start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < lowCount; ++i)
    {
      position.x = 50.0f;
      for(int j = 0; j < lowCount; ++j)
      {
        View view = View::New();
        SetRequestedGeometry(view, position.x, position.y, 10.0f, 10.0f);
        Renderer renderer = Renderer::New(geometry, shader);
        renderer.SetProperty(Renderer::Property::MIX_COLOR, Color::BEIGE);
        view.AddRenderer(renderer);
        mRoot.Add(view);

        if(!firstView)
        {
          firstView = view;
        }
        lastView = view;
        position.x += 10.0f;
      }
      position.y += 10.0f;
    }
    const auto end = std::chrono::high_resolution_clock::now();

    const uint64_t duration = DurationMicroseconds(start, end);
    AttachMarkers(firstView, lastView);
    AppendResult("View with Renderer", create100, duration);
    if(create100)
    {
      mTimeRenderer100 += duration;
      ++mCountRenderer100;
    }
    else
    {
      mTimeRenderer10000 += duration;
      ++mCountRenderer10000;
    }
  }

  void CreateViewsWithColor(bool create100)
  {
    View    firstView;
    View    lastView;
    int     lowCount = create100 ? 10 : 100;
    Vector2 position(50.0f, 50.0f);

    const auto start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < lowCount; ++i)
    {
      position.x = 50.0f;
      for(int j = 0; j < lowCount; ++j)
      {
        View view = View::New();
        SetRequestedGeometry(view, position.x, position.y, 10.0f, 10.0f);
        view.SetBackgroundColor(UiColor(Color::GREEN));
        mRoot.Add(view);

        if(!firstView)
        {
          firstView = view;
        }
        lastView = view;
        position.x += 10.0f;
      }
      position.y += 10.0f;
    }
    const auto end = std::chrono::high_resolution_clock::now();

    const uint64_t duration = DurationMicroseconds(start, end);
    AttachMarkers(firstView, lastView);
    AppendResult("View with Color", create100, duration);
    if(create100)
    {
      mTimeColor100 += duration;
      ++mCountColor100;
    }
    else
    {
      mTimeColor10000 += duration;
      ++mCountColor10000;
    }
  }

  template<typename TimePoint>
  uint64_t DurationMicroseconds(const TimePoint& start, const TimePoint& end) const
  {
    return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
  }

  static std::string FormatMilliseconds(uint64_t microseconds)
  {
    char buffer[32];
    std::snprintf(buffer, sizeof(buffer), "%.3f", static_cast<double>(microseconds) / 1000.0);
    return buffer;
  }

  void AppendResult(const char* type, bool create100, uint64_t duration)
  {
    std::fprintf(stderr,
                 "[DALI_UI_VIEW_CREATION_PERF] %s %s : %.3f ms (%llu us)\n",
                 type,
                 create100 ? "100" : "10000",
                 static_cast<double>(duration) / 1000.0,
                 static_cast<unsigned long long>(duration));

    if(mLogTimeLabel)
    {
      mLogTimeString += std::string("\n") + type + (create100 ? " 100 : " : " 10000 : ") + FormatMilliseconds(duration) + " ms";
      mLogTimeLabel.SetText(mLogTimeString.c_str());
      TrimLogToWindow();
      PinLabelBottom(mLogTimeLabel);
    }
  }

  static uint64_t Average(uint64_t total, uint32_t count)
  {
    return count == 0u ? 0u : total / count;
  }

  void UpdateAverageLabel()
  {
    if(!mAverageTimeLabel)
    {
      return;
    }

    const std::string average =
      std::string("Average Time") +
      "\nCreate View 100 : " + FormatMilliseconds(Average(mTimeView100, mCountView100)) + " ms" +
      "\nCreate View 10000 : " + FormatMilliseconds(Average(mTimeView10000, mCountView10000)) + " ms" +
      "\nCreate View with Renderer 100 : " + FormatMilliseconds(Average(mTimeRenderer100, mCountRenderer100)) + " ms" +
      "\nCreate View with Renderer 10000 : " + FormatMilliseconds(Average(mTimeRenderer10000, mCountRenderer10000)) + " ms" +
      "\nCreate View with Color 100 : " + FormatMilliseconds(Average(mTimeColor100, mCountColor100)) + " ms" +
      "\nCreate View with Color 10000 : " + FormatMilliseconds(Average(mTimeColor10000, mCountColor10000)) + " ms";
    mAverageTimeLabel.SetText(average.c_str());
    PinLabelBottom(mAverageTimeLabel);
  }

  void ToggleLogLabels()
  {
    if(mLogTimeLabel)
    {
      mLogTimeString.clear();
      mLogTimeLabel.Unparent();
      mLogTimeLabel.Reset();
    }
    else
    {
      mLogTimeString = "Time Log";
      mLogTimeLabel  = Label::New(mLogTimeString.c_str());
      ConfigureLogLabel(mLogTimeLabel, mWindowSize.x - 920.0f);
      mWindow.Add(mLogTimeLabel);
    }

    if(mAverageTimeLabel)
    {
      mAverageTimeLabel.Unparent();
      mAverageTimeLabel.Reset();
    }
    else
    {
      mAverageTimeLabel = Label::New();
      ConfigureLogLabel(mAverageTimeLabel, 20.0f);
      UpdateAverageLabel();
      mWindow.Add(mAverageTimeLabel);
    }
  }

private:
  Application& mApplication;
  Window       mWindow;
  Vector2      mWindowSize;
  View         mRoot;
  View         mDefaultView;
  View         mStartMarkerView;
  View         mEndMarkerView;
  std::string  mLogTimeString;
  Label        mLogTimeLabel;
  Label        mAverageTimeLabel;
  Timer        mQuitTimer;

  uint64_t mTimeView100{0u};
  uint64_t mTimeView10000{0u};
  uint64_t mTimeRenderer100{0u};
  uint64_t mTimeRenderer10000{0u};
  uint64_t mTimeColor100{0u};
  uint64_t mTimeColor10000{0u};
  uint32_t mCountView100{0u};
  uint32_t mCountView10000{0u};
  uint32_t mCountRenderer100{0u};
  uint32_t mCountRenderer10000{0u};
  uint32_t mCountColor100{0u};
  uint32_t mCountColor10000{0u};
};
} // unnamed namespace

int DALI_EXPORT_API main(int argc, char** argv)
{
  Dali::Application application = Dali::Application::New(&argc, &argv);
  ViewCreationPerf sample(application);
  application.MainLoop();
  return 0;
}
