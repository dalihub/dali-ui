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
#include <array>
#include <sstream>

// INTERNAL INCLUDES
#include "sample-common.h"

namespace GeometryTouchGestureSample
{
namespace
{
const Dali::Vector4 NAVIGATION_COLOR(0.08f, 0.11f, 0.16f, 1.0f);
const Dali::Vector4 ACTIVE_BUTTON_COLOR(0.12f, 0.55f, 0.86f, 1.0f);
const Dali::Vector4 INACTIVE_BUTTON_COLOR(0.18f, 0.25f, 0.34f, 1.0f);
const Dali::Vector4 INSTRUCTION_COLOR(0.12f, 0.15f, 0.20f, 1.0f);
const Dali::Vector4 CANVAS_COLOR(0.06f, 0.07f, 0.09f, 1.0f);
const Dali::Vector4 LOG_COLOR(0.03f, 0.04f, 0.06f, 1.0f);
} // unnamed namespace

/**
 * @brief Owns the diagnostic shell and replaces each scenario as it is selected.
 */
class GeometryTouchGestureController : public Dali::ConnectionTracker
{
public:
  explicit GeometryTouchGestureController(Dali::Application& application)
  : mApplication(application),
    mScenarioIndex(0u),
    mWindowWidth(DEFAULT_WINDOW_WIDTH),
    mWindowHeight(DEFAULT_WINDOW_HEIGHT),
    mCanvasHeight(DEFAULT_WINDOW_HEIGHT - NAVIGATION_HEIGHT - INSTRUCTION_HEIGHT - DEFAULT_LOG_HEIGHT),
    mLogHeight(DEFAULT_LOG_HEIGHT)
  {
    mApplication.InitSignal().Connect(this, &GeometryTouchGestureController::OnInit);
  }

  ~GeometryTouchGestureController() override = default;

  GeometryTouchGestureController(const GeometryTouchGestureController&)            = delete;
  GeometryTouchGestureController& operator=(const GeometryTouchGestureController&) = delete;

private:
  void OnInit(Dali::Application application)
  {
    mWindow                               = application.GetWindow();
    const Dali::PositionSize positionSize = mWindow.GetPositionSize();
    mWindowWidth                          = positionSize.width > 0 ? static_cast<float>(positionSize.width) : DEFAULT_WINDOW_WIDTH;
    mWindowHeight                         = positionSize.height > 0 ? static_cast<float>(positionSize.height) : DEFAULT_WINDOW_HEIGHT;

    const float availableContentHeight = std::max(0.0f, mWindowHeight - NAVIGATION_HEIGHT - INSTRUCTION_HEIGHT);
    const float scaledLogHeight        = DEFAULT_LOG_HEIGHT * mWindowHeight / DEFAULT_WINDOW_HEIGHT;
    mLogHeight                         = std::min(std::max(MINIMUM_LOG_HEIGHT, scaledLogHeight), availableContentHeight * 0.55f);
    mCanvasHeight                      = std::max(0.0f, availableContentHeight - mLogHeight);

    mWindow.SetBackgroundColor(LOG_COLOR);
    mWindow.KeyEventSignal().Connect(this, &GeometryTouchGestureController::OnKeyEvent);
    mWindow.TouchEventSignal().Connect(this, &GeometryTouchGestureController::OnSceneTouch);

    BuildShell();
    SelectScenario(0u);
  }

  void BuildShell()
  {
    mRoot = Dali::Ui::View::New();
    mRoot.SetRequestedWidth(mWindowWidth);
    mRoot.SetRequestedHeight(mWindowHeight);

    Dali::Ui::View navigation = Dali::Ui::View::New();
    navigation.SetBackgroundColor(NAVIGATION_COLOR);
    navigation.SetRequestedWidth(mWindowWidth);
    navigation.SetRequestedHeight(NAVIGATION_HEIGHT);
    mRoot.Add(navigation);

    mTitleLabel = CreateLabel("", 0.0f, 0.0f, mWindowWidth, 34.0f, 18.0f);
    navigation.Add(mTitleLabel);

    navigation.Add(CreateButton(this, "<", 8.0f, 38.0f, 46.0f, 38.0f, [this]()
    {
      SelectPreviousScenario();
    }));

    for(std::size_t index = 0u; index < SCENARIO_COUNT; ++index)
    {
      std::ostringstream label;
      label << index + 1u;
      mScenarioButtons[index] = CreateButton(this,
                                             label.str(),
                                             60.0f + static_cast<float>(index) * 54.0f,
                                             38.0f,
                                             48.0f,
                                             38.0f,
                                             [this, index]()
      {
        SelectScenario(index);
      });
      navigation.Add(mScenarioButtons[index]);
    }

    navigation.Add(CreateButton(this, "RESET", mWindowWidth - 210.0f, 38.0f, 96.0f, 38.0f, [this]()
    {
      SelectScenario(mScenarioIndex);
    }));
    navigation.Add(CreateButton(this, ">", mWindowWidth - 108.0f, 38.0f, 46.0f, 38.0f, [this]()
    {
      SelectNextScenario();
    }));
    navigation.Add(CreateButton(this, "CLEAR", mWindowWidth - 56.0f, 38.0f, 48.0f, 38.0f, [this]()
    {
      mEventLog.Clear();
    }));

    Dali::Ui::View instructionPanel = Dali::Ui::View::New();
    instructionPanel.SetBackgroundColor(INSTRUCTION_COLOR);
    instructionPanel.SetRequestedY(NAVIGATION_HEIGHT);
    instructionPanel.SetRequestedWidth(mWindowWidth);
    instructionPanel.SetRequestedHeight(INSTRUCTION_HEIGHT);
    mRoot.Add(instructionPanel);

    mInstructionLabel = CreateLabel("", 8.0f, 0.0f, mWindowWidth - 16.0f, INSTRUCTION_HEIGHT, 12.0f);
    instructionPanel.Add(mInstructionLabel);

    mCanvas = Dali::Ui::View::New();
    mCanvas.SetProperty(Dali::Actor::Property::NAME, "ScenarioCanvas");
    mCanvas.SetBackgroundColor(CANVAS_COLOR);
    mCanvas.SetRequestedY(NAVIGATION_HEIGHT + INSTRUCTION_HEIGHT);
    mCanvas.SetRequestedWidth(mWindowWidth);
    mCanvas.SetRequestedHeight(mCanvasHeight);
    mRoot.Add(mCanvas);

    Dali::Ui::View logPanel = Dali::Ui::View::New();
    logPanel.SetBackgroundColor(LOG_COLOR);
    logPanel.SetRequestedY(NAVIGATION_HEIGHT + INSTRUCTION_HEIGHT + mCanvasHeight);
    logPanel.SetRequestedWidth(mWindowWidth);
    logPanel.SetRequestedHeight(mLogHeight);
    mRoot.Add(logPanel);

    Dali::Ui::Label logTitle = CreateLabel("EVENT LOG (latest 9)", 8.0f, 0.0f, mWindowWidth - 16.0f, 32.0f, 15.0f);
    logTitle.SetHorizontalTextAlignment(Dali::Ui::Text::Alignment::START);
    logPanel.Add(logTitle);

    mLogLabel = CreateLabel("", 8.0f, 34.0f, mWindowWidth - 16.0f, mLogHeight - 40.0f, 12.0f, Dali::Vector4(0.74f, 0.90f, 0.78f, 1.0f));
    mLogLabel.SetHorizontalTextAlignment(Dali::Ui::Text::Alignment::START);
    mLogLabel.SetVerticalTextAlignment(Dali::Ui::Text::Alignment::START);
    logPanel.Add(mLogLabel);
    mEventLog.Attach(mLogLabel);

    mWindow.Add(mRoot);
  }

  void SelectScenario(std::size_t index)
  {
    if(index >= SCENARIO_COUNT)
    {
      return;
    }

    if(mScenarioRoot)
    {
      mCanvas.Remove(mScenarioRoot);
      mScenarioRoot.Reset();
    }
    mScenario.reset();

    mScenarioIndex = index;
    mEventLog.Clear();
    mScenario = CreateScenario(mScenarioIndex, mEventLog, Dali::Vector2(mWindowWidth, mCanvasHeight));
    if(!mScenario)
    {
      mEventLog.Add("ERROR", "Scenario factory returned an empty scenario");
      return;
    }

    std::ostringstream title;
    title << (mScenarioIndex + 1u) << '/' << SCENARIO_COUNT << "  " << mScenario->GetTitle();
    mTitleLabel.SetText(title.str().c_str());
    mInstructionLabel.SetText(mScenario->GetInstructions());

    for(std::size_t buttonIndex = 0u; buttonIndex < SCENARIO_COUNT; ++buttonIndex)
    {
      mScenarioButtons[buttonIndex].SetBackgroundColor(buttonIndex == mScenarioIndex ? ACTIVE_BUTTON_COLOR : INACTIVE_BUTTON_COLOR);
    }

    mScenarioRoot = mScenario->Build();
    mCanvas.Add(mScenarioRoot);
    mEventLog.Add("SCENARIO", std::string("loaded ") + mScenario->GetTitle());
  }

  void SelectPreviousScenario()
  {
    SelectScenario((mScenarioIndex + SCENARIO_COUNT - 1u) % SCENARIO_COUNT);
  }

  void SelectNextScenario()
  {
    SelectScenario((mScenarioIndex + 1u) % SCENARIO_COUNT);
  }

  void OnKeyEvent(Dali::Window, Dali::KeyEvent event)
  {
    if(event.GetState() != Dali::KeyEvent::DOWN)
    {
      return;
    }

    if(Dali::IsKey(event, Dali::DALI_KEY_ESCAPE) || Dali::IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
      return;
    }

    const Dali::String& key = event.GetKeyName();
    if(key == "Left")
    {
      SelectPreviousScenario();
    }
    else if(key == "Right")
    {
      SelectNextScenario();
    }
    else if(key == "r" || key == "R")
    {
      SelectScenario(mScenarioIndex);
    }
    else if(key == "c" || key == "C")
    {
      mEventLog.Clear();
    }
    else if(key == "1" || key == "2" || key == "3" || key == "4" || key == "5" || key == "6")
    {
      SelectScenario(static_cast<std::size_t>(key[0] - '1'));
    }
  }

  void OnSceneTouch(Dali::Window, Dali::TouchEvent touch)
  {
    if(touch.GetPointCount() == 0u)
    {
      return;
    }

    const float canvasTop          = NAVIGATION_HEIGHT + INSTRUCTION_HEIGHT;
    const float canvasBottom       = canvasTop + mCanvasHeight;
    const float screenY            = touch.GetScreenPosition(0u).y;
    const bool  insideCanvas       = screenY >= canvasTop && screenY < canvasBottom;
    const bool  relevantToScenario = mScenario ? mScenario->OnSceneTouch(touch, insideCanvas) : insideCanvas;
    if(relevantToScenario)
    {
      mEventLog.AddTouch("SCENE BOUNDARY", Dali::Actor(), touch, "observer; cannot consume");
    }
  }

private:
  Dali::Application&                                    mApplication;
  Dali::Window                                          mWindow;
  Dali::Ui::View                                        mRoot;
  Dali::Ui::View                                        mCanvas;
  Dali::Ui::View                                        mScenarioRoot;
  Dali::Ui::Label                                       mTitleLabel;
  Dali::Ui::Label                                       mInstructionLabel;
  Dali::Ui::Label                                       mLogLabel;
  std::array<Dali::Ui::InteractiveView, SCENARIO_COUNT> mScenarioButtons;
  EventLog                                              mEventLog;
  std::unique_ptr<Scenario>                             mScenario;
  std::size_t                                           mScenarioIndex;
  float                                                 mWindowWidth;
  float                                                 mWindowHeight;
  float                                                 mCanvasHeight;
  float                                                 mLogHeight;
};

} // namespace GeometryTouchGestureSample

int DALI_EXPORT_API main(int argc, char** argv)
{
  Dali::Application  application = Dali::Application::New(&argc, &argv);
  Dali::Ui::UiConfig config      = Dali::Ui::UiConfig::New();
  config.SetDefaultStateEffectForInteractive(Dali::Ui::OverlayEffect::Plain());
  config.Apply();

  GeometryTouchGestureSample::GeometryTouchGestureController controller(application);
  application.MainLoop();
  return 0;
}
