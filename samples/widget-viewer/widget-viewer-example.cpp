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

#include <bundle.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali/dali.h>
#include <dali/devel-api/adaptor-framework/application.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dlog.h>
#include <message_port.h>

#include <algorithm>
#include <string>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "DALI_WIDGET_SAMPLE"

#define LOG_I(fmt, ...) dlog_print(DLOG_INFO, LOG_TAG, "[VIEWER] " fmt, ##__VA_ARGS__)
#define LOG_E(fmt, ...) dlog_print(DLOG_ERROR, LOG_TAG, "[VIEWER] " fmt, ##__VA_ARGS__)

#define VIEWER_APP_ID "com.samsung.dali.ui.widget-viewer"
#define MESSAGE_PORT_NAME "dali_widget_sample_port"

using namespace Dali;
using namespace Dali::Ui;

namespace
{

constexpr const char* TOOLKIT_PROVIDER_APP_ID = "com.samsung.dali.widget";
constexpr const char* DALI_UI_PROVIDER_APP_ID = "com.samsung.dali.ui.widget";
constexpr const char* NUI_PROVIDER_APP_ID     = "Tizen.NUI.WidgetTest";
constexpr const char* WIDGET_CLASS1_ID        = "class1";
constexpr const char* WIDGET_CLASS2_ID        = "class2";

enum class ProviderType
{
  TOOLKIT,
  DALI_UI,
  NUI
};

const char* PointStateToString(PointState::Type state)
{
  switch(state)
  {
    case PointState::DOWN:
    {
      return "DOWN";
    }
    case PointState::UP:
    {
      return "UP";
    }
    case PointState::MOTION:
    {
      return "MOTION";
    }
    case PointState::LEAVE:
    {
      return "LEAVE";
    }
    case PointState::INTERRUPTED:
    {
      return "INTERRUPTED";
    }
    default:
    {
      return "OTHER";
    }
  }
}

} // namespace

class WidgetViewerApp : public ConnectionTracker
{
public:
  WidgetViewerApp(Application& app)
  : mApp(app)
  {
    app.InitSignal().Connect(this, &WidgetViewerApp::OnInit);
    app.TerminateSignal().Connect(this, &WidgetViewerApp::OnTerminate);
  }

  void OnInit(Application app)
  {
    mWindow = app.GetWindow();
    DevelWindow::SetFullScreen(mWindow, true);
    mWindow.SetBackgroundColor(Color::WHITE);
    mWindow.InterceptKeyEventSignal().Connect(this, &WidgetViewerApp::OnKeyEvent);
    mWindow.ResizedSignal().Connect(this, &WidgetViewerApp::OnWindowResized);

    const auto    windowBounds = mWindow.GetPositionSize();
    const Vector2 windowSize(static_cast<float>(windowBounds.width), static_cast<float>(windowBounds.height));

    mRootView = StackLayout::New(StackOrientation::VERTICAL);
    mRootView.SetBackgroundColor(Color::WHITE);
    mRootView.SetRequestedWidth(MATCH_PARENT);
    mRootView.SetRequestedHeight(MATCH_PARENT);
    mWindow.Add(mRootView);

    mTitle = Label::New();
    mTitle.SetProperty(Label::Property::TEXT_COLOR, Color::BLACK);
    mTitle.SetProperty(Label::Property::HORIZONTAL_ALIGNMENT, "CENTER");
    mTitle.SetProperty(Label::Property::VERTICAL_ALIGNMENT, "CENTER");
    mTitle.SetProperty(Label::Property::MULTI_LINE, true);
    mTitle.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
    UpdateTitle();
    mRootView.Add(mTitle);

    mProviderSwitchButton = Label::New();
    mProviderSwitchButton.SetProperty(Label::Property::TEXT_COLOR, Color::WHITE);
    mProviderSwitchButton.SetProperty(Label::Property::HORIZONTAL_ALIGNMENT, "CENTER");
    mProviderSwitchButton.SetProperty(Label::Property::VERTICAL_ALIGNMENT, "CENTER");
    mProviderSwitchButton.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::CENTER));
    mProviderSwitchButton.SetAccessibilityRole(Dali::Ui::Accessibility::Role::BUTTON);
    mProviderSwitchButton.TouchEventSignal().Connect(this, &WidgetViewerApp::OnProviderSwitchTouched);
    UpdateProviderSwitchButton();
    mRootView.Add(mProviderSwitchButton);

    mWidgetArea = AbsoluteLayout::New();
    mWidgetArea.SetBackgroundColor(UiColor(0.96f, 0.97f, 0.99f, 1.0f));
    mWidgetArea.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    mRootView.Add(mWidgetArea);

    mFaultText = Label::New("Widget faulted");
    mFaultText.SetProperty(Label::Property::TEXT_COLOR, Color::RED);
    mFaultText.SetProperty(Label::Property::HORIZONTAL_ALIGNMENT, "CENTER");
    mFaultText.SetProperty(Label::Property::VERTICAL_ALIGNMENT, "CENTER");
    mFaultText.SetProperty(Label::Property::MULTI_LINE, true);
    mFaultText.SetProperty(Actor::Property::VISIBLE, false);
    mWidgetArea.Add(mFaultText);

    mStatus = Label::New();
    mStatus.SetProperty(Label::Property::TEXT_COLOR, Color::BLACK);
    mStatus.SetProperty(Label::Property::HORIZONTAL_ALIGNMENT, "CENTER");
    mStatus.SetProperty(Label::Property::VERTICAL_ALIGNMENT, "CENTER");
    mStatus.SetProperty(Label::Property::MULTI_LINE, true);
    mStatus.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
    mRootView.Add(mStatus);

    ApplyResponsiveLayout(windowSize.x, windowSize.y);

    mBlueToggleTimer = Dali::Timer::New(4000);
    mBlueToggleTimer.TickSignal().Connect(this, &WidgetViewerApp::OnBlueToggleTick);
    SetBlueAutoToggleEnabled(false);
    CreateProviderWidgets();
  }

  void OnTerminate(Application)
  {
    if(mBlueToggleTimer)
    {
      mBlueToggleTimer.Stop();
      mBlueToggleTimer.Reset();
    }
    RemoveWidgetView(mWidgetView1);
    RemoveWidgetView(mWidgetView2);
  }

  bool OnKeyEvent(Dali::Window, KeyEvent event)
  {
    LOG_I("Window key event: state=%d keyName=%s keyCode=%d focusedWidget=%d",
          static_cast<int>(event.GetState()),
          event.GetKeyName().CStr(),
          event.GetKeyCode(),
          mFocusedWidgetIndex);

    const bool viewerShortcut = IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK) ||
                                event.GetKeyName() == "1" || event.GetKeyName() == "Return" ||
                                event.GetKeyName() == "2" || event.GetKeyName() == "3" ||
                                event.GetKeyName() == "4" || event.GetKeyName() == "f" ||
                                event.GetKeyName() == "F" || event.GetKeyName() == "p" ||
                                event.GetKeyName() == "P" || event.GetKeyName() == "d" ||
                                event.GetKeyName() == "r" || event.GetKeyName() == "c" ||
                                event.GetKeyName() == "m";
    if(!viewerShortcut)
    {
      return false;
    }

    if(event.GetState() != KeyEvent::DOWN)
    {
      return true;
    }

    if(IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK))
    {
      mApp.Quit();
    }
    else if(event.GetKeyName() == "1" || event.GetKeyName() == "Return")
    {
      ResizeAndSendMessage();
    }
    else if(event.GetKeyName() == "3")
    {
      FocusWidget(mWidgetView1, "red", 1);
    }
    else if(event.GetKeyName() == "2")
    {
      SetBlueAutoToggleEnabled(!mBlueAutoToggleEnabled);
    }
    else if(event.GetKeyName() == "4")
    {
      FocusBlueWidget();
    }
    else if(event.GetKeyName() == "d" && mWidgetView1)
    {
      if(mWidgetView1.GetParent())
      {
        mWidgetView1.Unparent();
        LOG_I("Widget regression: red detached while paused\n");
      }
      else
      {
        mWidgetArea.Add(mWidgetView1);
        LOG_I("Widget regression: red reattached and resumed\n");
      }
    }
    else if(event.GetKeyName() == "r" && mWidgetView1)
    {
      mWidgetView1.ResumeWidget();
      FocusWidget(mWidgetView1, "red", 1);
    }
    else if(event.GetKeyName() == "c" && mWidgetView1)
    {
      mWidgetView1.SetCornerRadius(80.0f);
      LOG_I("Widget regression: updated red corner shader\n");
    }
    else if(event.GetKeyName() == "m")
    {
      // Exercise shared-engine creation and destruction before CREATE arrives.
      for(int cycle = 0; cycle < 2; ++cycle)
      {
        const auto widgetId = BuildWidgetId(WIDGET_CLASS2_ID);
        if(Dali::Ui::WidgetView::New("different-viewer", widgetId.c_str(), "", mBlueWidgetSize, mBlueWidgetSize, 0.0f))
        {
          LOG_E("Widget regression: different viewer ID was accepted");
          break;
        }
        auto probe = Dali::Ui::WidgetView::New(VIEWER_APP_ID, widgetId.c_str(), Dali::String(EncodeBundle().c_str()), mBlueWidgetSize, mBlueWidgetSize, 0.0f);
        if(!probe)
        {
          LOG_E("Widget regression: creation failed");
          break;
        }
        probe.Reset();
        LOG_I("Widget regression: cycle %d passed", cycle + 1);
      }
    }
    else if(event.GetKeyName() == "f" || event.GetKeyName() == "F")
    {
      SendFaultCommand();
    }
    else if(event.GetKeyName() == "p" || event.GetKeyName() == "P")
    {
      SwitchProvider();
    }
    return true;
  }

  void OnWindowResized(Window, Window::WindowSize windowSize)
  {
    ApplyResponsiveLayout(static_cast<float>(windowSize.GetWidth()), static_cast<float>(windowSize.GetHeight()));
  }

private:
  void UpdateTitle()
  {
    const std::string text = std::string("Widget Sample - ") + GetProviderDisplayName() +
                             "\nTouch widget: color/count | Arrow keys: move focused widget" +
                             "\n1/Return: resize + IPC | 2: auto add/remove " +
                             (mBlueAutoToggleEnabled ? "on" : "off") +
                             " | 3/4: focus | Tap button: provider | F: fault";
    mTitle.SetProperty(Label::Property::TEXT, text.c_str());
  }

  void ApplyResponsiveLayout(float windowWidth, float windowHeight)
  {
    const float scale = std::clamp(std::min(windowWidth / 1280.0f, windowHeight / 800.0f), 0.85f, 2.0f);

    mOuterPadding     = 32.0f * scale;
    mLayoutSpacing    = 14.0f * scale;
    mWidgetGap        = 48.0f * scale;
    mContentWidth     = std::max(1.0f, windowWidth - 2.0f * mOuterPadding);
    mWidgetAreaHeight = std::max(240.0f,
                                 windowHeight - 2.0f * mOuterPadding -
                                   (86.0f + 52.0f + 48.0f) * scale - 3.0f * mLayoutSpacing);

    const uint16_t padding = static_cast<uint16_t>(mOuterPadding);
    mRootView.SetPadding(Insets(padding, padding, padding, padding));
    mRootView.SetSpacing(mLayoutSpacing);

    mTitle.SetRequestedHeight(86.0f * scale);
    mTitle.SetProperty(Label::Property::FONT_SIZE, 14.0f * scale);

    mProviderSwitchButton.SetRequestedWidth(std::clamp(mContentWidth * 0.58f, 480.0f * scale, 1200.0f));
    mProviderSwitchButton.SetRequestedHeight(52.0f * scale);
    mProviderSwitchButton.SetCornerRadius(12.0f * scale);
    mProviderSwitchButton.SetProperty(Label::Property::FONT_SIZE, 14.0f * scale);

    mStatus.SetRequestedHeight(48.0f * scale);
    mStatus.SetProperty(Label::Property::FONT_SIZE, 12.0f * scale);
    mFaultText.SetProperty(Label::Property::FONT_SIZE, 18.0f * scale);
    mWidgetArea.SetCornerRadius(18.0f * scale);
    mWidgetCornerRadius = 40.0f * scale;

    const float slotWidth    = std::max(1.0f, (mContentWidth - mWidgetGap) * 0.5f);
    const float idealSize    = std::min(slotWidth * 0.72f, mWidgetAreaHeight * 0.78f);
    const int   previousBase = mBaseWidgetSize;
    mBaseWidgetSize          = std::max(20, static_cast<int>(std::clamp(idealSize, 300.0f, 640.0f) / 20.0f) * 20);
    mMaximumWidgetSize       = std::max(mBaseWidgetSize,
                                        static_cast<int>(std::min({slotWidth * 0.92f, mWidgetAreaHeight * 0.90f, 760.0f})));
    mWidgetResizeStep        = std::min(std::max(60, mBaseWidgetSize / 5),
                                        mMaximumWidgetSize - mBaseWidgetSize);

    if(!mWidgetView1 || (mWidgetWidth == previousBase && mWidgetHeight == previousBase))
    {
      mWidgetWidth  = mBaseWidgetSize;
      mWidgetHeight = mBaseWidgetSize;
    }
    else
    {
      mWidgetWidth  = std::min(mWidgetWidth, mMaximumWidgetSize);
      mWidgetHeight = std::min(mWidgetHeight, mMaximumWidgetSize);
    }
    mBlueWidgetSize = mBaseWidgetSize;

    UpdateWidgetBounds();
  }

  void UpdateWidgetBounds()
  {
    const float slotWidth = std::max(1.0f, (mContentWidth - mWidgetGap) * 0.5f);

    if(mWidgetView1)
    {
      mWidgetView1.SetCornerRadius(mWidgetCornerRadius);
      mWidgetView1.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);
      const float x = std::max(0.0f, (slotWidth - static_cast<float>(mWidgetWidth)) * 0.5f);
      const float y = std::max(0.0f, (mWidgetAreaHeight - static_cast<float>(mWidgetHeight)) * 0.5f);
      mWidgetView1.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(
        LayoutRect(x, y, static_cast<float>(mWidgetWidth), static_cast<float>(mWidgetHeight))));
    }

    if(mWidgetView2)
    {
      mWidgetView2.SetCornerRadius(mWidgetCornerRadius);
      mWidgetView2.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);
      const float x = slotWidth + mWidgetGap + std::max(0.0f, (slotWidth - static_cast<float>(mBlueWidgetSize)) * 0.5f);
      const float y = std::max(0.0f, (mWidgetAreaHeight - static_cast<float>(mBlueWidgetSize)) * 0.5f);
      mWidgetView2.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(
        LayoutRect(x, y, static_cast<float>(mBlueWidgetSize), static_cast<float>(mBlueWidgetSize))));
    }

    if(mFaultText)
    {
      const float faultHeight = std::min(mWidgetAreaHeight, 180.0f * std::max(1.0f, mOuterPadding / 32.0f));
      mFaultText.SetLayoutParams(AbsoluteLayoutParams::New()
                                   .SetBounds(LayoutRect(0.0f, (mWidgetAreaHeight - faultHeight) * 0.5f, 1.0f, faultHeight))
                                   .SetFlags(AbsoluteLayoutFlags::WIDTH_PROPORTIONAL));
    }
  }

  const char* GetProviderAppId() const
  {
    switch(mProviderType)
    {
      case ProviderType::TOOLKIT:
      {
        return TOOLKIT_PROVIDER_APP_ID;
      }
      case ProviderType::DALI_UI:
      {
        return DALI_UI_PROVIDER_APP_ID;
      }
      case ProviderType::NUI:
      {
        return NUI_PROVIDER_APP_ID;
      }
    }
    return TOOLKIT_PROVIDER_APP_ID;
  }

  const char* GetProviderDisplayName() const
  {
    switch(mProviderType)
    {
      case ProviderType::TOOLKIT:
      {
        return "DALi Toolkit Provider";
      }
      case ProviderType::DALI_UI:
      {
        return "DALI UI Provider";
      }
      case ProviderType::NUI:
      {
        return "NUI Provider";
      }
    }
    return "Unknown Provider";
  }

  bool SupportsSampleProtocol() const
  {
    return mProviderType != ProviderType::NUI;
  }

  ProviderType GetNextProviderType() const
  {
    switch(mProviderType)
    {
      case ProviderType::TOOLKIT:
      {
        return ProviderType::DALI_UI;
      }
      case ProviderType::DALI_UI:
      {
        return ProviderType::NUI;
      }
      case ProviderType::NUI:
      {
        return ProviderType::TOOLKIT;
      }
    }
    return ProviderType::TOOLKIT;
  }

  const char* GetNextProviderDisplayName() const
  {
    switch(GetNextProviderType())
    {
      case ProviderType::TOOLKIT:
      {
        return "DALi Toolkit";
      }
      case ProviderType::DALI_UI:
      {
        return "DALI UI";
      }
      case ProviderType::NUI:
      {
        return "NUI";
      }
    }
    return "Unknown";
  }

  std::string BuildWidgetId(const char* classId) const
  {
    return std::string(classId) + "@" + GetProviderAppId();
  }

  void UpdateProviderSwitchButton()
  {
    const std::string text = std::string("Active: ") + GetProviderDisplayName() +
                             "  |  Tap for " + GetNextProviderDisplayName();
    mProviderSwitchButton.SetProperty(Label::Property::TEXT, text.c_str());
    const UiColor color = mProviderType == ProviderType::TOOLKIT ? UiColor(0.10f, 0.42f, 0.78f, 1.0f) : mProviderType == ProviderType::DALI_UI ? UiColor(0.10f, 0.62f, 0.38f, 1.0f)
                                                                                                                                               : UiColor(0.50f, 0.28f, 0.72f, 1.0f);
    mProviderSwitchButton.SetBackgroundColor(color);
  }

  bool OnProviderSwitchTouched(Actor, TouchEvent event)
  {
    if(event.GetPointCount() == 0 || event.GetState(0) != PointState::DOWN)
    {
      return false;
    }

    SwitchProvider();
    return true;
  }

  void SwitchProvider()
  {
    if(mBlueToggleTimer)
    {
      mBlueToggleTimer.Stop();
    }
    RemoveWidgetView(mWidgetView1);
    RemoveWidgetView(mWidgetView2);
    mBlueCreated        = false;
    mFaulted            = false;
    mFocusedWidgetIndex = 1;
    mFaultText.SetProperty(Actor::Property::VISIBLE, false);

    mProviderType = GetNextProviderType();
    UpdateProviderSwitchButton();
    UpdateTitle();
    CreateProviderWidgets();

    if(mBlueAutoToggleEnabled && mBlueToggleTimer)
    {
      mBlueToggleTimer.Start();
    }
    LOG_I("Switched to %s (%s)", GetProviderDisplayName(), GetProviderAppId());
  }

  void CreateProviderWidgets()
  {
    const std::string encodedBundle = EncodeBundle();
    const std::string redWidgetId   = BuildWidgetId(WIDGET_CLASS1_ID);

    mWidgetView1 = Dali::Ui::WidgetView::New(VIEWER_APP_ID, redWidgetId.c_str(), Dali::String(encodedBundle.c_str()), mWidgetWidth, mWidgetHeight, 0.0f);
    if(!mWidgetView1)
    {
      LOG_E("Failed to add widget: %s", redWidgetId.c_str());
      ShowFaultState("Failed to add provider widget");
      return;
    }
    ConnectWidgetSignals(mWidgetView1);
    mWidgetArea.Add(mWidgetView1);
    UpdateWidgetBounds();
    FocusWidget(mWidgetView1, "red", 1);

    mBlueCreated = AddBlueWidget();
    SetStatus(std::string("Creating widgets from ") + GetProviderAppId(), false);
  }

  void SetBlueAutoToggleEnabled(bool enabled)
  {
    mBlueAutoToggleEnabled = enabled;

    if(mBlueToggleTimer)
    {
      if(enabled)
      {
        if(!mFaulted)
        {
          mBlueToggleTimer.Start();
        }
      }
      else
      {
        mBlueToggleTimer.Stop();
      }
    }

    UpdateTitle();
    LOG_I("Blue widget auto toggle %s", enabled ? "enabled" : "disabled");
  }

  bool AddBlueWidget()
  {
    const std::string encodedBundle = EncodeBundle();
    const std::string blueWidgetId  = BuildWidgetId(WIDGET_CLASS2_ID);
    mWidgetView2                    = Dali::Ui::WidgetView::New(VIEWER_APP_ID, blueWidgetId.c_str(), Dali::String(encodedBundle.c_str()), mBlueWidgetSize, mBlueWidgetSize, 0.0f);
    if(!mWidgetView2)
    {
      LOG_E("Failed to add widget: %s", blueWidgetId.c_str());
      ShowFaultState("Failed to add provider widget");
      return false;
    }
    ConnectWidgetSignals(mWidgetView2);
    mWidgetArea.Add(mWidgetView2);
    UpdateWidgetBounds();
    LOG_I("Blue widget added");
    return true;
  }

  bool OnBlueToggleTick()
  {
    if(mFaulted)
    {
      return false;
    }

    if(mBlueCreated)
    {
      if(mFocusedWidgetIndex == 2)
      {
        LOG_I("Blue widget auto-remove skipped while focused");
        return true;
      }
      RemoveWidgetView(mWidgetView2);
      mBlueCreated = false;
    }
    else
    {
      mBlueCreated = AddBlueWidget();
    }
    return true;
  }

  void FocusBlueWidget()
  {
    if(!mWidgetView2 && !mFaulted)
    {
      mBlueCreated = AddBlueWidget();
    }
    FocusWidget(mWidgetView2, "blue", 2);
  }

  void FocusWidget(Dali::Ui::WidgetView widgetView, const char* label, int focusIndex)
  {
    if(mFaulted || !widgetView)
    {
      LOG_I("FocusWidget skipped: label=%s faulted=%d valid=%d", label, mFaulted, static_cast<bool>(widgetView));
      return;
    }

    FocusManager::Get().SetCurrentFocusView(widgetView);
    mFocusedWidgetIndex = focusIndex;
    LOG_I("Focused %s widget for key event test", label);
  }

  void ResizeAndSendMessage()
  {
    if(mFaulted || !mWidgetView1)
    {
      return;
    }

    mWidgetWidth += mWidgetResizeStep;
    mWidgetHeight += mWidgetResizeStep;
    if(mWidgetWidth > mMaximumWidgetSize || mWidgetHeight > mMaximumWidgetSize)
    {
      mWidgetWidth  = mBaseWidgetSize;
      mWidgetHeight = mBaseWidgetSize;
    }

    LOG_I("Resizing WidgetView1 to %d x %d", mWidgetWidth, mWidgetHeight);
    UpdateWidgetBounds();
    FocusWidget(mWidgetView1, "red", 1);

    if(SupportsSampleProtocol())
    {
      bundle* b = bundle_create();
      if(b)
      {
        std::string msg = "Viewer resized width:" + std::to_string(mWidgetWidth);
        bundle_add_str(b, "message", msg.c_str());
        message_port_send_message(GetProviderAppId(), MESSAGE_PORT_NAME, b);
        bundle_free(b);
      }
    }
    SetStatus("Resized red widget to " + std::to_string(mWidgetWidth) + "x" + std::to_string(mWidgetHeight), false);
  }

  void SendFaultCommand()
  {
    if(mFaulted)
    {
      return;
    }

    if(!SupportsSampleProtocol())
    {
      SetStatus("Fault injection is not supported by the selected provider.", true);
      return;
    }

    bundle* b = bundle_create();
    if(b)
    {
      bundle_add_str(b, "command", "fault");
      message_port_send_message(GetProviderAppId(), MESSAGE_PORT_NAME, b);
      bundle_free(b);
      LOG_I("Sent fault command");
    }
  }

  void ConnectWidgetSignals(Dali::Ui::WidgetView widgetView)
  {
    widgetView.TouchEventSignal().Connect(this, &WidgetViewerApp::OnWidgetViewTouched);
    widgetView.WidgetAddedSignal().Connect(this, &WidgetViewerApp::OnWidgetAdded);
    widgetView.WidgetDeletedSignal().Connect(this, &WidgetViewerApp::OnWidgetDeleted);
    widgetView.WidgetCreationAbortedSignal().Connect(this, &WidgetViewerApp::OnWidgetCreationAborted);
    widgetView.WidgetContentUpdatedSignal().Connect(this, &WidgetViewerApp::OnWidgetContentUpdated);
    widgetView.WidgetUpdatePeriodChangedSignal().Connect(this, &WidgetViewerApp::OnWidgetUpdatePeriodChanged);
    widgetView.WidgetFaultedSignal().Connect(this, &WidgetViewerApp::OnWidgetFaulted);
  }

  void RemoveWidgetView(Dali::Ui::WidgetView& view)
  {
    if(view)
    {
      mWidgetArea.Remove(view);
      view.Reset();
    }
  }

  bool OnWidgetViewTouched(Dali::Actor actor, Dali::TouchEvent event)
  {
    Dali::Ui::WidgetView widgetView = Dali::Ui::WidgetView::DownCast(actor);
    if(event.GetPointCount() == 0)
    {
      LOG_I("WidgetView touched: widgetId=%s pointCount=0", widgetView ? widgetView.GetWidgetId().CStr() : "unknown");
      return false;
    }

    const Vector2 local  = event.GetLocalPosition(0);
    const Vector2 screen = event.GetScreenPosition(0);
    LOG_I("WidgetView touched: widgetId=%s state=%s pointCount=%u local=(%.1f, %.1f) screen=(%.1f, %.1f) deviceClass=%d deviceSubclass=%d time=%lu",
          widgetView ? widgetView.GetWidgetId().CStr() : "unknown",
          PointStateToString(event.GetState(0)),
          event.GetPointCount(),
          local.x,
          local.y,
          screen.x,
          screen.y,
          static_cast<int>(event.GetDeviceClass(0)),
          static_cast<int>(event.GetDeviceSubclass(0)),
          static_cast<unsigned long>(event.GetTime()));
    return false;
  }

  void OnWidgetAdded(Dali::Ui::WidgetView widgetView)
  {
    LOG_I("WidgetAdded: widgetId=%s", widgetView.GetWidgetId().CStr());
    SetStatus(std::string("Widget added: ") + widgetView.GetWidgetId().CStr(), false);
  }

  void OnWidgetDeleted(Dali::Ui::WidgetView widgetView)
  {
    LOG_I("Widget content lost: widgetId=%s", widgetView.GetWidgetId().CStr());
    SetStatus(std::string("Widget content lost: ") + widgetView.GetWidgetId().CStr(), false);
  }

  void OnWidgetCreationAborted(Dali::Ui::WidgetView widgetView)
  {
    LOG_E("WidgetCreationAborted: widgetId=%s", widgetView.GetWidgetId().CStr());
    SetStatus("Widget creation aborted", true);
  }

  void OnWidgetContentUpdated(Dali::Ui::WidgetView widgetView)
  {
    LOG_I("WidgetContentUpdated: widgetId=%s", widgetView.GetWidgetId().CStr());

    const Dali::String contentInfo = widgetView.GetContentInfo();
    if(!contentInfo.Empty())
    {
      bundle* b = bundle_decode(reinterpret_cast<const bundle_raw*>(contentInfo.CStr()), static_cast<int>(contentInfo.Size()));
      if(b)
      {
        char* count = nullptr;
        if(bundle_get_str(b, "COUNT", &count) == BUNDLE_ERROR_NONE && count)
        {
          LOG_I("WidgetContentUpdated -> COUNT: %s", count);
          SetStatus(std::string("ContentInfo updated: COUNT=") + count, false);
        }
        bundle_free(b);
      }
    }
  }

  void OnWidgetUpdatePeriodChanged(Dali::Ui::WidgetView widgetView)
  {
    const float period = widgetView.GetUpdatePeriod();
    LOG_I("WidgetUpdatePeriodChanged: widgetId=%s period=%.1f", widgetView.GetWidgetId().CStr(), period);
    SetStatus("Update period changed: " + std::to_string(period), false);
  }

  void OnWidgetFaulted(Dali::Ui::WidgetView widgetView)
  {
    LOG_E("WidgetFaulted: widgetId=%s", widgetView.GetWidgetId().CStr());
    // Keep the views visible so a relaunched provider can resume and send frames.
    SetStatus("Widget faulted", true);
  }

  void ShowFaultState(const char* message)
  {
    if(mFaulted)
    {
      return;
    }

    mFaulted = true;
    if(mBlueToggleTimer)
    {
      mBlueToggleTimer.Stop();
    }
    if(mWidgetView1)
    {
      mWidgetView1.SetProperty(Actor::Property::VISIBLE, false);
    }
    if(mWidgetView2)
    {
      mWidgetView2.SetProperty(Actor::Property::VISIBLE, false);
    }

    const std::string text = std::string(message) + "\nWidgetView is hidden.\nTap the provider button to switch or press Back to exit.";
    mFaultText.SetProperty(Label::Property::TEXT, text.c_str());
    mFaultText.SetProperty(Actor::Property::VISIBLE, true);
    SetStatus(message, true);
  }

  void SetStatus(const std::string& text, bool error)
  {
    mStatus.SetProperty(Label::Property::TEXT, text.c_str());
    mStatus.SetProperty(Label::Property::TEXT_COLOR, error ? Color::RED : Color::BLACK);
    LOG_I("%s", text.c_str());
  }

  std::string EncodeBundle()
  {
    std::string encoded;
    bundle*     b = bundle_create();
    if(!b)
    {
      LOG_E("EncodeBundle: bundle_create failed");
      return encoded;
    }
    bundle_add_str(b, "COUNT", "1");

    bundle_raw* raw = nullptr;
    int         len = 0;
    if(bundle_encode(b, &raw, &len) == BUNDLE_ERROR_NONE && raw)
    {
      encoded.assign(reinterpret_cast<const char*>(raw), static_cast<size_t>(len));
      free(raw);
    }
    bundle_free(b);
    return encoded;
  }

  Application&         mApp;
  Window               mWindow;
  StackLayout          mRootView;
  AbsoluteLayout       mWidgetArea;
  Label                mTitle;
  Label                mProviderSwitchButton;
  Label                mFaultText;
  Label                mStatus;
  Dali::Ui::WidgetView mWidgetView1;
  Dali::Ui::WidgetView mWidgetView2;
  Dali::Timer          mBlueToggleTimer;
  int                  mWidgetWidth{360};
  int                  mWidgetHeight{360};
  int                  mBlueWidgetSize{360};
  int                  mBaseWidgetSize{360};
  int                  mMaximumWidgetSize{760};
  int                  mWidgetResizeStep{120};
  int                  mFocusedWidgetIndex{1};
  float                mOuterPadding{32.0f};
  float                mLayoutSpacing{14.0f};
  float                mWidgetGap{48.0f};
  float                mWidgetCornerRadius{40.0f};
  float                mContentWidth{1280.0f};
  float                mWidgetAreaHeight{560.0f};
  ProviderType         mProviderType{ProviderType::DALI_UI};

  bool mBlueCreated{false};
  bool mBlueAutoToggleEnabled{false};
  bool mFaulted{false};
};

__attribute__((visibility("default"))) int main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig    config      = UiConfig::New();
  config.Apply();
  WidgetViewerApp viewer(application);
  application.MainLoop();
  return 0;
}
