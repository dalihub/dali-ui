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
#include <array>
#include <sstream>
#include <vector>

// INTERNAL INCLUDES
#include "sample-common.h"

namespace GeometryTouchGestureSample
{
namespace
{
constexpr std::size_t ACTOR_COUNT  = 4u;
constexpr std::size_t PRESET_COUNT = 5u;

constexpr std::size_t TEST_ROOT_INDEX = 0u;
constexpr std::size_t BACK_INDEX      = 1u;
constexpr std::size_t PARENT_INDEX    = 2u;
constexpr std::size_t CHILD_INDEX     = 3u;

const Dali::Vector4 ENABLED_COLOR(0.15f, 0.58f, 0.35f, 1.0f);
const Dali::Vector4 DISABLED_COLOR(0.24f, 0.26f, 0.30f, 1.0f);
const Dali::Vector4 TRUE_COLOR(0.78f, 0.38f, 0.08f, 1.0f);
const Dali::Vector4 LOCKED_COLOR(0.14f, 0.15f, 0.17f, 1.0f);
const Dali::Vector4 OWNER_COLOR(0.08f, 0.72f, 0.28f, 1.0f);
const Dali::Vector4 INTERRUPTED_COLOR(0.78f, 0.05f, 0.08f, 1.0f);
const Dali::Vector4 INTERCEPT_COLOR(0.56f, 0.18f, 0.72f, 1.0f);
const Dali::Vector4 TOUCH_COLOR(0.08f, 0.56f, 0.72f, 1.0f);

/**
 * @brief Shows the intercept phase and coordinate touch phase for one combined Actor graph.
 */
class TouchRoutingScenario : public Scenario
{
public:
  TouchRoutingScenario(EventLog& eventLog, const Dali::Vector2& canvasSize)
  : Scenario(eventLog, canvasSize),
    mTraceEventTime(0u),
    mHasTraceEvent(false),
    mStreamActive(false),
    mControlsLocked(false),
    mSequence(0u),
    mTestAreaX(0.0f),
    mTestAreaY(0.0f),
    mTestAreaWidth(0.0f),
    mTestAreaHeight(0.0f)
  {
    mTouchCalled.fill(false);
  }

  ~TouchRoutingScenario() override = default;

  TouchRoutingScenario(const TouchRoutingScenario&)            = delete;
  TouchRoutingScenario& operator=(const TouchRoutingScenario&) = delete;

  const char* GetTitle() const override
  {
    return "Basic Touch & Intercept Flow";
  }

  const char* GetInstructions() const override
  {
    return "Touch the common zone. Intercept runs root-to-parent first; ordinary Touch then runs front-to-back, "
           "or interceptor-to-root when intercepted. Configure signals and return values between streams.";
  }

  Dali::Ui::View Build() override
  {
    mRoot = Dali::Ui::View::New();
    mRoot.SetProperty(Dali::Actor::Property::NAME, "TouchRoutingPage");
    mRoot.SetRequestedWidth(GetCanvasSize().x);
    mRoot.SetRequestedHeight(GetCanvasSize().y);

    BuildPresetBar();
    BuildTestArea();
    BuildConfigurationTable();
    ApplyPreset(0u);
    return mRoot;
  }

  bool OnSceneTouch(const Dali::TouchEvent& touch, bool insideCanvas) override
  {
    if(touch.GetPointCount() == 0u)
    {
      return false;
    }

    const Dali::PointState::Type state    = touch.GetState(0u);
    bool                         relevant = mStreamActive;
    if(state == Dali::PointState::STARTED)
    {
      relevant = insideCanvas && IsInsideTestArea(touch.GetScreenPosition(0u));
      if(relevant)
      {
        mStreamActive = true;
      }
    }

    if(!relevant)
    {
      return false;
    }

    BeginTraceEvent(touch);
    mSceneBoundary = std::string(PointStateToString(state)) + " (observer; cannot consume)";
    UpdateNotDelivered();

    if(state == Dali::PointState::STARTED)
    {
      SetControlsLocked(true);
    }
    else if(state == Dali::PointState::FINISHED || state == Dali::PointState::INTERRUPTED)
    {
      mStreamActive = false;
      SetControlsLocked(false);
    }

    RefreshFlowTrace();
    return true;
  }

private:
  struct ActorConfiguration
  {
    std::string    name;
    std::string    role;
    Dali::Ui::View view;
    Dali::Vector4  baseColor;
    bool           interceptCapable{false};
    bool           touchConnected{false};
    bool           touchConsumes{false};
    bool           interceptConnected{false};
    bool           interceptConsumes{false};
  };

  void BuildPresetBar()
  {
    const char* presetNames[PRESET_COUNT] = {
      "BASIC PASS",
      "BASIC CONSUME",
      "INTERCEPT PASS",
      "INTERCEPT ONLY",
      "INTERCEPT + OWNER",
    };

    const float margin      = 8.0f;
    const float gap         = 6.0f;
    const float buttonWidth = (GetCanvasSize().x - 2.0f * margin - gap * static_cast<float>(PRESET_COUNT - 1u)) / static_cast<float>(PRESET_COUNT);
    for(std::size_t index = 0u; index < PRESET_COUNT; ++index)
    {
      mPresetButtons[index] = CreateButton(this,
                                           presetNames[index],
                                           margin + static_cast<float>(index) * (buttonWidth + gap),
                                           8.0f,
                                           buttonWidth,
                                           40.0f,
                                           [this, index]()
      {
        ApplyPreset(index);
      });
      mRoot.Add(mPresetButtons[index]);
    }
  }

  void BuildTestArea()
  {
    const float contentTop    = 56.0f;
    const float contentHeight = GetCanvasSize().y - contentTop - 8.0f;
    const float leftWidth     = GetCanvasSize().x * 0.48f - 12.0f;

    mTestAreaX      = 8.0f;
    mTestAreaY      = contentTop;
    mTestAreaWidth  = leftWidth;
    mTestAreaHeight = contentHeight * 0.62f;

    InitializeActor(TEST_ROOT_INDEX, "TestRoot", "ancestor of all", Dali::Vector4(0.16f, 0.18f, 0.22f, 1.0f), true);
    InitializeActor(BACK_INDEX, "BackSibling", "unrelated sibling", Dali::Vector4(0.08f, 0.28f, 0.72f, 1.0f), false);
    InitializeActor(PARENT_INDEX, "Parent", "parent of Child", Dali::Vector4(0.72f, 0.12f, 0.14f, 1.0f), true);
    InitializeActor(CHILD_INDEX, "Child", "front target", Dali::Vector4(0.94f, 0.40f, 0.06f, 1.0f), false);

    mActors[TEST_ROOT_INDEX].view = CreateTestView("TestRoot",
                                                   "TEST ROOT",
                                                   mActors[TEST_ROOT_INDEX].baseColor,
                                                   mTestAreaX,
                                                   mTestAreaY,
                                                   mTestAreaWidth,
                                                   mTestAreaHeight);
    mActors[BACK_INDEX].view      = CreateTestView("BackSibling",
                                                   "BACK SIBLING\nTouch candidate behind Parent",
                                                   mActors[BACK_INDEX].baseColor,
                                                   mTestAreaWidth * 0.05f,
                                                   mTestAreaHeight * 0.17f,
                                                   mTestAreaWidth * 0.76f,
                                                   mTestAreaHeight * 0.66f);
    mActors[PARENT_INDEX].view    = CreateTestView("Parent",
                                                   "PARENT\nIntercept ancestor",
                                                   mActors[PARENT_INDEX].baseColor,
                                                   mTestAreaWidth * 0.25f,
                                                   mTestAreaHeight * 0.09f,
                                                   mTestAreaWidth * 0.70f,
                                                   mTestAreaHeight * 0.76f);

    const float parentWidth   = mTestAreaWidth * 0.70f;
    const float parentHeight  = mTestAreaHeight * 0.76f;
    mActors[CHILD_INDEX].view = CreateTestView("Child",
                                               "CHILD (front)\nTOUCH HERE",
                                               mActors[CHILD_INDEX].baseColor,
                                               parentWidth * 0.23f,
                                               parentHeight * 0.28f,
                                               parentWidth * 0.55f,
                                               parentHeight * 0.45f);

    mActors[PARENT_INDEX].view.Add(mActors[CHILD_INDEX].view);
    mActors[TEST_ROOT_INDEX].view.Add(mActors[BACK_INDEX].view);
    mActors[TEST_ROOT_INDEX].view.Add(mActors[PARENT_INDEX].view);
    mRoot.Add(mActors[TEST_ROOT_INDEX].view);

    const float    traceY      = mTestAreaY + mTestAreaHeight + 8.0f;
    const float    traceHeight = GetCanvasSize().y - traceY - 8.0f;
    Dali::Ui::View tracePanel  = Dali::Ui::View::New();
    tracePanel.SetBackgroundColor(Dali::Vector4(0.08f, 0.09f, 0.12f, 1.0f));
    tracePanel.SetRequestedX(mTestAreaX);
    tracePanel.SetRequestedY(traceY);
    tracePanel.SetRequestedWidth(mTestAreaWidth);
    tracePanel.SetRequestedHeight(traceHeight);
    mRoot.Add(tracePanel);

    mFlowLabel = CreateLabel("", 8.0f, 4.0f, mTestAreaWidth - 16.0f, traceHeight - 8.0f, 11.0f, Dali::Vector4(0.78f, 0.90f, 1.0f, 1.0f));
    mFlowLabel.SetHorizontalTextAlignment(Dali::Ui::Text::Alignment::START);
    mFlowLabel.SetVerticalTextAlignment(Dali::Ui::Text::Alignment::START);
    tracePanel.Add(mFlowLabel);
  }

  void BuildConfigurationTable()
  {
    const float panelX      = mTestAreaX + mTestAreaWidth + 12.0f;
    const float panelY      = mTestAreaY;
    const float panelWidth  = GetCanvasSize().x - panelX - 8.0f;
    const float panelHeight = GetCanvasSize().y - panelY - 8.0f;

    Dali::Ui::View panel = Dali::Ui::View::New();
    panel.SetBackgroundColor(Dali::Vector4(0.10f, 0.12f, 0.15f, 1.0f));
    panel.SetRequestedX(panelX);
    panel.SetRequestedY(panelY);
    panel.SetRequestedWidth(panelWidth);
    panel.SetRequestedHeight(panelHeight);
    mRoot.Add(panel);

    const float nameX                = 8.0f;
    const float nameWidth            = panelWidth * 0.20f;
    const float touchSignalX         = nameX + nameWidth;
    const float touchSignalWidth     = panelWidth * 0.18f;
    const float touchReturnX         = touchSignalX + touchSignalWidth;
    const float touchReturnWidth     = panelWidth * 0.18f;
    const float interceptSignalX     = touchReturnX + touchReturnWidth;
    const float interceptSignalWidth = panelWidth * 0.20f;
    const float interceptReturnX     = interceptSignalX + interceptSignalWidth;
    const float interceptReturnWidth = panelWidth - interceptReturnX - 8.0f;

    panel.Add(CreateLabel("ACTOR", nameX, 4.0f, nameWidth, 34.0f, 11.0f));
    panel.Add(CreateLabel("TOUCH SIGNAL", touchSignalX, 4.0f, touchSignalWidth, 34.0f, 11.0f));
    panel.Add(CreateLabel("TOUCH RETURN", touchReturnX, 4.0f, touchReturnWidth, 34.0f, 11.0f));
    panel.Add(CreateLabel("INTERCEPT SIGNAL", interceptSignalX, 4.0f, interceptSignalWidth, 34.0f, 11.0f));
    panel.Add(CreateLabel("INTERCEPT RETURN", interceptReturnX, 4.0f, interceptReturnWidth, 34.0f, 11.0f));

    const float rowStart  = 42.0f;
    const float rowHeight = 64.0f;
    for(std::size_t index = 0u; index < ACTOR_COUNT; ++index)
    {
      const float y = rowStart + static_cast<float>(index) * rowHeight;
      panel.Add(CreateLabel(mActors[index].name + "\n" + mActors[index].role, nameX, y, nameWidth, 52.0f, 11.0f));

      mTouchSignalButtons[index] = CreateButton(this, "", touchSignalX + 4.0f, y + 5.0f, touchSignalWidth - 8.0f, 42.0f, [this, index]()
      {
        ToggleTouchSignal(index);
      });
      mTouchSignalLabels[index]  = Dali::Ui::Label::DownCast(mTouchSignalButtons[index].GetChildAt(0u));
      panel.Add(mTouchSignalButtons[index]);

      mTouchReturnButtons[index] = CreateButton(this, "", touchReturnX + 4.0f, y + 5.0f, touchReturnWidth - 8.0f, 42.0f, [this, index]()
      {
        ToggleTouchReturn(index);
      });
      mTouchReturnLabels[index]  = Dali::Ui::Label::DownCast(mTouchReturnButtons[index].GetChildAt(0u));
      panel.Add(mTouchReturnButtons[index]);

      if(mActors[index].interceptCapable)
      {
        mInterceptSignalButtons[index] = CreateButton(this, "", interceptSignalX + 4.0f, y + 5.0f, interceptSignalWidth - 8.0f, 42.0f, [this, index]()
        {
          ToggleInterceptSignal(index);
        });
        mInterceptSignalLabels[index]  = Dali::Ui::Label::DownCast(mInterceptSignalButtons[index].GetChildAt(0u));
        panel.Add(mInterceptSignalButtons[index]);

        mInterceptReturnButtons[index] = CreateButton(this, "", interceptReturnX + 4.0f, y + 5.0f, interceptReturnWidth - 8.0f, 42.0f, [this, index]()
        {
          ToggleInterceptReturn(index);
        });
        mInterceptReturnLabels[index]  = Dali::Ui::Label::DownCast(mInterceptReturnButtons[index].GetChildAt(0u));
        panel.Add(mInterceptReturnButtons[index]);
      }
      else
      {
        panel.Add(CreateLabel("N/A", interceptSignalX, y, interceptSignalWidth, 52.0f, 11.0f, Dali::Vector4(0.50f, 0.52f, 0.56f, 1.0f)));
        panel.Add(CreateLabel("N/A", interceptReturnX, y, interceptReturnWidth, 52.0f, 11.0f, Dali::Vector4(0.50f, 0.52f, 0.56f, 1.0f)));
      }
    }

    const float     legendY = rowStart + static_cast<float>(ACTOR_COUNT) * rowHeight + 6.0f;
    Dali::Ui::Label legend  = CreateLabel(
      "COMMON ZONE EXPECTED\nINTERCEPT: TestRoot -> Parent\nTOUCH: Child -> Parent -> BackSibling -> TestRoot\n"
       "Intercept true restricts the Touch phase; it does not create an owner.",
      12.0f,
      legendY,
      panelWidth - 24.0f,
      panelHeight - legendY - 8.0f,
      12.0f,
      Dali::Vector4(0.84f, 0.88f, 0.96f, 1.0f));
    legend.SetHorizontalTextAlignment(Dali::Ui::Text::Alignment::START);
    panel.Add(legend);
  }

  void InitializeActor(std::size_t        index,
                       const std::string& name,
                       const std::string& role,
                       Dali::Vector4      color,
                       bool               interceptCapable)
  {
    ActorConfiguration& configuration = mActors[index];
    configuration.name                = name;
    configuration.role                = role;
    configuration.baseColor           = color;
    configuration.interceptCapable    = interceptCapable;
  }

  void ToggleTouchSignal(std::size_t index)
  {
    if(mControlsLocked)
    {
      return;
    }
    SetTouchConnected(index, !mActors[index].touchConnected);
    RefreshControls();
  }

  void ToggleTouchReturn(std::size_t index)
  {
    if(mControlsLocked)
    {
      return;
    }
    mActors[index].touchConsumes = !mActors[index].touchConsumes;
    RefreshControls();
  }

  void ToggleInterceptSignal(std::size_t index)
  {
    if(mControlsLocked || !mActors[index].interceptCapable)
    {
      return;
    }
    SetInterceptConnected(index, !mActors[index].interceptConnected);
    RefreshControls();
  }

  void ToggleInterceptReturn(std::size_t index)
  {
    if(mControlsLocked || !mActors[index].interceptCapable)
    {
      return;
    }
    mActors[index].interceptConsumes = !mActors[index].interceptConsumes;
    RefreshControls();
  }

  void SetTouchConnected(std::size_t index, bool connected)
  {
    ActorConfiguration& configuration = mActors[index];
    if(configuration.touchConnected == connected)
    {
      return;
    }

    if(connected)
    {
      configuration.view.TouchEventSignal().Connect(this, &TouchRoutingScenario::OnTouch);
    }
    else
    {
      configuration.view.TouchEventSignal().Disconnect(this, &TouchRoutingScenario::OnTouch);
    }
    configuration.touchConnected = connected;
  }

  void SetInterceptConnected(std::size_t index, bool connected)
  {
    ActorConfiguration& configuration = mActors[index];
    if(!configuration.interceptCapable || configuration.interceptConnected == connected)
    {
      return;
    }

    if(connected)
    {
      configuration.view.InterceptTouchEventSignal().Connect(this, &TouchRoutingScenario::OnInterceptTouch);
    }
    else
    {
      configuration.view.InterceptTouchEventSignal().Disconnect(this, &TouchRoutingScenario::OnInterceptTouch);
    }
    configuration.interceptConnected = connected;
  }

  void ApplyPreset(std::size_t presetIndex)
  {
    if(mControlsLocked || presetIndex >= PRESET_COUNT)
    {
      return;
    }

    for(std::size_t index = 0u; index < ACTOR_COUNT; ++index)
    {
      SetTouchConnected(index, false);
      SetInterceptConnected(index, false);
      mActors[index].touchConsumes     = false;
      mActors[index].interceptConsumes = false;
    }
    for(std::size_t index = 0u; index < ACTOR_COUNT; ++index)
    {
      SetTouchConnected(index, true);
    }

    switch(presetIndex)
    {
      case 0u:
        break;
      case 1u:
        mActors[PARENT_INDEX].touchConsumes = true;
        break;
      case 2u:
        SetInterceptConnected(TEST_ROOT_INDEX, true);
        SetInterceptConnected(PARENT_INDEX, true);
        break;
      case 3u:
        SetInterceptConnected(TEST_ROOT_INDEX, true);
        SetInterceptConnected(PARENT_INDEX, true);
        mActors[PARENT_INDEX].interceptConsumes = true;
        break;
      case 4u:
        SetInterceptConnected(TEST_ROOT_INDEX, true);
        SetInterceptConnected(PARENT_INDEX, true);
        mActors[PARENT_INDEX].interceptConsumes = true;
        mActors[PARENT_INDEX].touchConsumes     = true;
        break;
    }

    mOwner.Reset();
    mInterceptor.clear();
    mHasTraceEvent = false;
    mStreamActive  = false;
    ClearTrace("Preset loaded. Touch the CHILD common zone.");
    RestoreActorColors();
    RefreshControls();
  }

  bool OnInterceptTouch(Dali::Actor actor, Dali::TouchEvent touch)
  {
    const std::size_t index = FindActorIndex(actor);
    if(index >= ACTOR_COUNT)
    {
      return false;
    }

    BeginTraceEvent(touch);
    const bool consumed = mActors[index].interceptConsumes;
    ++mSequence;

    std::ostringstream entry;
    entry << '#' << mSequence << ' ' << mActors[index].name << ' ' << PointStateToString(touch.GetState(0u))
          << " -> " << (consumed ? "true" : "false");
    mInterceptTrace.push_back(entry.str());
    mActors[index].view.SetBackgroundColor(INTERCEPT_COLOR);
    if(consumed)
    {
      mInterceptor = mActors[index].name;
    }

    GetEventLog().AddTouch("INTERCEPT", actor, touch, std::string("return=") + (consumed ? "true" : "false"));
    RefreshFlowTrace();
    return consumed;
  }

  bool OnTouch(Dali::Actor actor, Dali::TouchEvent touch)
  {
    const std::size_t index = FindActorIndex(actor);
    if(index >= ACTOR_COUNT)
    {
      return false;
    }

    BeginTraceEvent(touch);
    const Dali::PointState::Type state    = touch.GetState(0u);
    const bool                   consumed = mActors[index].touchConsumes;
    ++mSequence;
    mTouchCalled[index] = true;

    std::ostringstream entry;
    entry << '#' << mSequence << ' ' << mActors[index].name << ' ' << PointStateToString(state);
    if(state == Dali::PointState::INTERRUPTED)
    {
      entry << " (terminated)";
      mActors[index].view.SetBackgroundColor(INTERRUPTED_COLOR);
    }
    else
    {
      entry << " -> " << (consumed ? "true" : "false");
      if(consumed)
      {
        mOwner = actor;
        mActors[index].view.SetBackgroundColor(OWNER_COLOR);
      }
      else
      {
        mActors[index].view.SetBackgroundColor(TOUCH_COLOR);
      }
    }
    mTouchTrace.push_back(entry.str());

    GetEventLog().AddTouch("TOUCH", actor, touch, std::string("return=") + (consumed ? "true" : "false"));
    RefreshFlowTrace();
    return consumed;
  }

  void BeginTraceEvent(const Dali::TouchEvent& touch)
  {
    const uint32_t eventTime = touch.GetTime();
    if(mHasTraceEvent && mTraceEventTime == eventTime)
    {
      return;
    }

    mHasTraceEvent  = true;
    mTraceEventTime = eventTime;
    mSequence       = 0u;
    mInterceptTrace.clear();
    mTouchTrace.clear();
    mSceneBoundary.clear();
    mNotDelivered.clear();
    mInterceptor.clear();
    mTouchCalled.fill(false);

    if(touch.GetPointCount() > 0u && touch.GetState(0u) == Dali::PointState::STARTED)
    {
      mOwner.Reset();
      mStreamActive = true;
    }
    RestoreActorColors();
  }

  void ClearTrace(const std::string& message)
  {
    mSequence = 0u;
    mInterceptTrace.clear();
    mTouchTrace.clear();
    mSceneBoundary = message;
    mNotDelivered.clear();
    mTouchCalled.fill(false);
    RefreshFlowTrace();
  }

  void UpdateNotDelivered()
  {
    std::ostringstream stream;
    for(std::size_t index = 0u; index < ACTOR_COUNT; ++index)
    {
      if(mActors[index].touchConnected && !mTouchCalled[index])
      {
        if(stream.tellp() > 0)
        {
          stream << ", ";
        }
        stream << mActors[index].name;
      }
    }
    mNotDelivered = stream.str();
  }

  void RefreshFlowTrace()
  {
    if(!mFlowLabel)
    {
      return;
    }

    std::ostringstream stream;
    stream << "INTERCEPT PHASE (root -> target ancestor)\n";
    if(mInterceptTrace.empty())
    {
      stream << "  (no callback)\n";
    }
    else
    {
      for(const std::string& entry : mInterceptTrace)
      {
        stream << "  " << entry << '\n';
      }
    }

    stream << "TOUCH PHASE\n";
    if(mTouchTrace.empty())
    {
      stream << "  (no callback)\n";
    }
    else
    {
      for(const std::string& entry : mTouchTrace)
      {
        stream << "  " << entry << '\n';
      }
    }

    stream << "RESULT  interceptor=" << (mInterceptor.empty() ? "<none>" : mInterceptor)
           << " owner=" << (mOwner ? ActorName(mOwner) : "<none>");
    if(!mNotDelivered.empty())
    {
      stream << " | configured but not called: " << mNotDelivered;
    }
    if(!mSceneBoundary.empty())
    {
      stream << "\nSCENE BOUNDARY  " << mSceneBoundary;
    }
    mFlowLabel.SetText(stream.str().c_str());
  }

  void RefreshControls()
  {
    for(std::size_t index = 0u; index < ACTOR_COUNT; ++index)
    {
      SetToggleAppearance(mTouchSignalButtons[index],
                          mTouchSignalLabels[index],
                          mActors[index].touchConnected,
                          mActors[index].touchConnected ? "ON" : "OFF");
      SetToggleAppearance(mTouchReturnButtons[index],
                          mTouchReturnLabels[index],
                          mActors[index].touchConsumes,
                          mActors[index].touchConsumes ? "TRUE" : "FALSE");

      if(mActors[index].interceptCapable)
      {
        SetToggleAppearance(mInterceptSignalButtons[index],
                            mInterceptSignalLabels[index],
                            mActors[index].interceptConnected,
                            mActors[index].interceptConnected ? "ON" : "OFF");
        SetToggleAppearance(mInterceptReturnButtons[index],
                            mInterceptReturnLabels[index],
                            mActors[index].interceptConsumes,
                            mActors[index].interceptConsumes ? "TRUE" : "FALSE");
      }
    }

    for(Dali::Ui::InteractiveView& presetButton : mPresetButtons)
    {
      presetButton.SetProperty(Dali::Actor::Property::SENSITIVE, !mControlsLocked);
      if(mControlsLocked)
      {
        presetButton.SetBackgroundColor(LOCKED_COLOR);
      }
      else
      {
        presetButton.SetBackgroundColor(DISABLED_COLOR);
      }
    }
  }

  void SetToggleAppearance(Dali::Ui::InteractiveView button,
                           Dali::Ui::Label           label,
                           bool                      enabled,
                           const char*               text)
  {
    if(!button || !label)
    {
      return;
    }
    label.SetText(text);
    button.SetProperty(Dali::Actor::Property::SENSITIVE, !mControlsLocked);
    const bool trueReturn = text[0] == 'T';
    button.SetBackgroundColor(mControlsLocked ? LOCKED_COLOR : (enabled ? (trueReturn ? TRUE_COLOR : ENABLED_COLOR) : DISABLED_COLOR));
  }

  void SetControlsLocked(bool locked)
  {
    if(mControlsLocked == locked)
    {
      return;
    }
    mControlsLocked = locked;
    RefreshControls();
  }

  void RestoreActorColors()
  {
    for(ActorConfiguration& configuration : mActors)
    {
      configuration.view.SetBackgroundColor(configuration.baseColor);
    }
    if(mOwner)
    {
      const std::size_t ownerIndex = FindActorIndex(mOwner);
      if(ownerIndex < ACTOR_COUNT)
      {
        mActors[ownerIndex].view.SetBackgroundColor(OWNER_COLOR);
      }
    }
  }

  std::size_t FindActorIndex(Dali::Actor actor) const
  {
    for(std::size_t index = 0u; index < ACTOR_COUNT; ++index)
    {
      if(mActors[index].view == actor)
      {
        return index;
      }
    }
    return ACTOR_COUNT;
  }

  bool IsInsideTestArea(const Dali::Vector2& screenPosition) const
  {
    const float canvasY = screenPosition.y - NAVIGATION_HEIGHT - INSTRUCTION_HEIGHT;
    return screenPosition.x >= mTestAreaX && screenPosition.x < mTestAreaX + mTestAreaWidth &&
           canvasY >= mTestAreaY && canvasY < mTestAreaY + mTestAreaHeight;
  }

private:
  Dali::Ui::View                                      mRoot;
  Dali::Ui::Label                                     mFlowLabel;
  std::array<ActorConfiguration, ACTOR_COUNT>         mActors;
  std::array<Dali::Ui::InteractiveView, ACTOR_COUNT>  mTouchSignalButtons;
  std::array<Dali::Ui::InteractiveView, ACTOR_COUNT>  mTouchReturnButtons;
  std::array<Dali::Ui::InteractiveView, ACTOR_COUNT>  mInterceptSignalButtons;
  std::array<Dali::Ui::InteractiveView, ACTOR_COUNT>  mInterceptReturnButtons;
  std::array<Dali::Ui::Label, ACTOR_COUNT>            mTouchSignalLabels;
  std::array<Dali::Ui::Label, ACTOR_COUNT>            mTouchReturnLabels;
  std::array<Dali::Ui::Label, ACTOR_COUNT>            mInterceptSignalLabels;
  std::array<Dali::Ui::Label, ACTOR_COUNT>            mInterceptReturnLabels;
  std::array<Dali::Ui::InteractiveView, PRESET_COUNT> mPresetButtons;
  std::array<bool, ACTOR_COUNT>                       mTouchCalled;
  std::vector<std::string>                            mInterceptTrace;
  std::vector<std::string>                            mTouchTrace;
  std::string                                         mSceneBoundary;
  std::string                                         mNotDelivered;
  std::string                                         mInterceptor;
  Dali::Actor                                         mOwner;
  uint32_t                                            mTraceEventTime;
  bool                                                mHasTraceEvent;
  bool                                                mStreamActive;
  bool                                                mControlsLocked;
  uint32_t                                            mSequence;
  float                                               mTestAreaX;
  float                                               mTestAreaY;
  float                                               mTestAreaWidth;
  float                                               mTestAreaHeight;
};

} // unnamed namespace

std::unique_ptr<Scenario> CreateTouchRoutingScenario(EventLog& eventLog, const Dali::Vector2& canvasSize)
{
  return std::make_unique<TouchRoutingScenario>(eventLog, canvasSize);
}

} // namespace GeometryTouchGestureSample
