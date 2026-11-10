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
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/widget-application.h>
#include <dali/devel-api/adaptor-framework/widget-impl.h>
#include <dali/devel-api/adaptor-framework/widget.h>
#include <dali/public-api/common/dali-string-view.h>
#include <dlog.h>
#include <message_port.h>
#include <unistd.h>

#include <algorithm>
#include <string>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "DALI_WIDGET_SAMPLE"

#define LOG_I(fmt, ...) \
  dlog_print(DLOG_INFO, LOG_TAG, "[PROVIDER] " fmt, ##__VA_ARGS__)
#define LOG_E(fmt, ...) \
  dlog_print(DLOG_ERROR, LOG_TAG, "[PROVIDER] " fmt, ##__VA_ARGS__)

#define APP_ID "com.samsung.dali.ui.widget"
#define WIDGET_CLASS_ID_CLS1 "class1@" APP_ID
#define WIDGET_CLASS_ID_CLS2 "class2@" APP_ID

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float       MOVE_STEP           = 24.0f;
constexpr float       MOVE_LABEL_WIDTH    = 160.0f;
constexpr float       MOVE_LABEL_HEIGHT   = 80.0f;
constexpr float       STATUS_LABEL_HEIGHT = 150.0f;
constexpr const char* MESSAGE_PORT_NAME   = "dali_widget_sample_port";

const Vector4 COLORS[] = {
  Vector4(0.95f, 0.20f, 0.18f, 1.0f),
  Vector4(0.16f, 0.58f, 0.95f, 1.0f),
  Vector4(0.16f, 0.74f, 0.42f, 1.0f),
  Vector4(0.97f, 0.72f, 0.15f, 1.0f),
};

bool IsArrowLeft(const KeyEvent& event)
{
  return event.GetKeyName() == "Left" || event.GetKeyName() == "XF86Left";
}

bool IsArrowRight(const KeyEvent& event)
{
  return event.GetKeyName() == "Right" || event.GetKeyName() == "XF86Right";
}

bool IsArrowUp(const KeyEvent& event)
{
  return event.GetKeyName() == "Up" || event.GetKeyName() == "XF86Up";
}

bool IsArrowDown(const KeyEvent& event)
{
  return event.GetKeyName() == "Down" || event.GetKeyName() == "XF86Down";
}

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

const char* KeyStateToString(KeyEvent::State state)
{
  switch(state)
  {
    case KeyEvent::DOWN:
    {
      return "DOWN";
    }
    case KeyEvent::UP:
    {
      return "UP";
    }
    default:
    {
      return "OTHER";
    }
  }
}

} // namespace

class SampleWidget : public Dali::Internal::Adaptor::Widget
{
public:
  SampleWidget(const Vector4& bgColor, const char* labelText)
  : mInitialColor(bgColor),
    mLabelText(labelText)
  {
  }

  void OnCreate(const Dali::String& contentInfo, Dali::Window window) override
  {
    const Dali::String countValue = DecodeCountFromBundle(contentInfo);
    LOG_I("OnCreate: label='%s' COUNT=%s", mLabelText, countValue.CStr());

    SetUsingKeyEvent(true);

    mWindow = window;
    mWindow.SetBackgroundColor(mInitialColor);
    mWindow.KeyEventSignal().Connect(this, &SampleWidget::OnKeyEvent);
    UpdateWidgetSize(window);

    mRootView = AbsoluteLayout::New();
    mRootView.SetBackgroundColor(mInitialColor);
    DevelActor::SetResizePolicy(mRootView, ResizePolicy::FILL_TO_PARENT,
                                Dimension::ALL_DIMENSIONS);
    mRootView.SetRequestedWidth(MATCH_PARENT);
    mRootView.SetRequestedHeight(MATCH_PARENT);
    mRootView.TouchEventSignal().Connect(this, &SampleWidget::OnTouched);
    window.Add(mRootView);

    mStatusLabel = Label::New();
    mStatusLabel.SetProperty(Label::Property::HORIZONTAL_ALIGNMENT, "CENTER");
    mStatusLabel.SetProperty(Label::Property::VERTICAL_ALIGNMENT, "TOP");
    mStatusLabel.SetProperty(Label::Property::TEXT_COLOR, Color::BLACK);
    mStatusLabel.SetProperty(Label::Property::FONT_SIZE, 14.0f);
    mStatusLabel.SetProperty(Label::Property::MULTI_LINE, true);
    mStatusLabel.SetLayoutParams(
      AbsoluteLayoutParams::New()
        .SetBounds(LayoutRect(0.0f, 14.0f, 1.0f, STATUS_LABEL_HEIGHT))
        .SetFlags(AbsoluteLayoutFlags::WIDTH_PROPORTIONAL));
    mRootView.Add(mStatusLabel);

    mMoveLabel = Label::New("MOVE");
    mMoveLabel.SetProperty(Label::Property::HORIZONTAL_ALIGNMENT, "CENTER");
    mMoveLabel.SetProperty(Label::Property::VERTICAL_ALIGNMENT, "CENTER");
    mMoveLabel.SetProperty(Label::Property::TEXT_COLOR, Color::WHITE);
    mMoveLabel.SetProperty(Label::Property::FONT_SIZE, 20.0f);
    UpdateMoveLabelPosition();
    mRootView.Add(mMoveLabel);

    mAnimation               = Animation::New(1.0f);
    KeyFrames scaleKeyFrames = KeyFrames::New();
    scaleKeyFrames.Add(0.0f, Vector3(1.0f, 1.0f, 1.0f));
    scaleKeyFrames.Add(0.5f, Vector3(1.18f, 1.18f, 1.0f));
    scaleKeyFrames.Add(1.0f, Vector3(1.0f, 1.0f, 1.0f));
    mAnimation.AnimateBetween(Property(mMoveLabel, Actor::Property::SCALE),
                              scaleKeyFrames);
    mAnimation.SetLoopCount(Animation::INFINITE_LOOP);
    mAnimation.Play();

    mUpdateTimer = Dali::Timer::New(5000);
    mUpdateTimer.TickSignal().Connect(this, &SampleWidget::OnUpdateTimerTick);
    mUpdateTimer.Start();

    RegisterMessagePort(this);
    UpdateStatus();
  }

  void OnTerminate(const Dali::String&,
                   Dali::Widget::Termination type) override
  {
    LOG_I("OnTerminate: label='%s' type=%d", mLabelText,
          static_cast<int>(type));

    UnregisterMessagePort(this);

    if(mUpdateTimer)
    {
      mUpdateTimer.Stop();
      mUpdateTimer.Reset();
    }
    if(mAnimation)
    {
      mAnimation.Stop();
      mAnimation.Reset();
    }
    if(mMoveLabel)
    {
      mMoveLabel.Unparent();
      mMoveLabel.Reset();
    }
    if(mStatusLabel)
    {
      mStatusLabel.Unparent();
      mStatusLabel.Reset();
    }
    if(mRootView)
    {
      mRootView.Unparent();
      mRootView.Reset();
    }
  }

  void OnPause() override
  {
  }
  void OnResume() override
  {
  }

  void OnResize(Dali::Window window) override
  {
    UpdateWidgetSize(window);
    ClampTextPosition();
    UpdateMoveLabelPosition();
    UpdateStatus();
  }

  void OnUpdate(const Dali::String& contentInfo, int force) override
  {
    LOG_I("OnUpdate: force=%d, contentInfo=%s", force, contentInfo.CStr());
  }

private:
  void UpdateWidgetSize(Dali::Window window)
  {
    const auto    windowBounds = window.GetPositionSize();
    const Vector2 size(static_cast<float>(windowBounds.width), static_cast<float>(windowBounds.height));
    if(size.x > 0.0f && size.y > 0.0f)
    {
      mSize = size;
    }
  }

  bool OnTouched(Actor, TouchEvent event)
  {
    if(event.GetPointCount() == 0)
    {
      LOG_I("OnTouched: label='%s' pointCount=0", mLabelText);
      return false;
    }

    const PointState::Type state  = event.GetState(0);
    const Vector2          local  = event.GetLocalPosition(0);
    const Vector2          screen = event.GetScreenPosition(0);
    LOG_I(
      "OnTouched: label='%s' state=%s pointCount=%u local=(%.1f, %.1f) "
      "screen=(%.1f, %.1f) deviceClass=%d deviceSubclass=%d time=%lu",
      mLabelText, PointStateToString(state), event.GetPointCount(), local.x,
      local.y, screen.x, screen.y,
      static_cast<int>(event.GetDeviceClass(0)),
      static_cast<int>(event.GetDeviceSubclass(0)),
      static_cast<unsigned long>(event.GetTime()));

    if(state == PointState::DOWN)
    {
      ++mTouchCount;
      mColorIndex = (mColorIndex + 1u) % (sizeof(COLORS) / sizeof(COLORS[0]));
      mWindow.SetBackgroundColor(COLORS[mColorIndex]);
      mRootView.SetBackgroundColor(COLORS[mColorIndex]);
      LOG_I("Touch accepted: label='%s' touchCount=%u colorIndex=%u",
            mLabelText, mTouchCount, mColorIndex);
      UpdateStatus();
    }
    return true;
  }

  void OnKeyEvent(Dali::Window, KeyEvent event)
  {
    LOG_I(
      "OnKeyEvent: label='%s' state=%s keyName=%s keyCode=%d "
      "deviceClass=%d deviceSubclass=%d time=%lu",
      mLabelText, KeyStateToString(event.GetState()),
      event.GetKeyName().CStr(), event.GetKeyCode(),
      static_cast<int>(event.GetDeviceClass()),
      static_cast<int>(event.GetDeviceSubclass()),
      static_cast<unsigned long>(event.GetTime()));

    if(event.GetState() != KeyEvent::DOWN)
    {
      return;
    }

    bool moved = true;
    if(IsArrowLeft(event))
    {
      mTextPosition.x -= MOVE_STEP;
      mLastKey = "Left";
    }
    else if(IsArrowRight(event))
    {
      mTextPosition.x += MOVE_STEP;
      mLastKey = "Right";
    }
    else if(IsArrowUp(event))
    {
      mTextPosition.y -= MOVE_STEP;
      mLastKey = "Up";
    }
    else if(IsArrowDown(event))
    {
      mTextPosition.y += MOVE_STEP;
      mLastKey = "Down";
    }
    else
    {
      moved    = false;
      mLastKey = event.GetKeyName().CStr();
    }

    if(moved)
    {
      ClampTextPosition();
      UpdateMoveLabelPosition();
    }
    LOG_I("Key accepted: label='%s' lastKey=%s moved=%d position=(%.1f, %.1f)",
          mLabelText, mLastKey.c_str(), moved, mTextPosition.x,
          mTextPosition.y);
    UpdateStatus();
  }

  bool OnUpdateTimerTick()
  {
    ++mUpdateCount;

    bundle* b = bundle_create();
    if(b)
    {
      std::string countStr = std::to_string(mUpdateCount);
      bundle_add_str(b, "COUNT", countStr.c_str());

      bundle_raw* raw = nullptr;
      int         len = 0;
      if(bundle_encode(b, &raw, &len) == BUNDLE_ERROR_NONE && raw && len > 0)
      {
        Dali::String encoded(Dali::StringView(reinterpret_cast<const char*>(raw),
                                              static_cast<uint32_t>(len)));
        SetContentInfo(encoded);
        free(raw);
        LOG_I("SetContentInfo called with COUNT=%d", mUpdateCount);
      }
      bundle_free(b);
    }
    return true;
  }

  void ClampTextPosition()
  {
    const float maxX = std::max(0.0f, (mSize.x - MOVE_LABEL_WIDTH) * 0.5f);
    const float maxY = std::max(0.0f, (mSize.y - MOVE_LABEL_HEIGHT) * 0.5f);
    mTextPosition.x  = std::max(-maxX, std::min(maxX, mTextPosition.x));
    mTextPosition.y  = std::max(-maxY, std::min(maxY, mTextPosition.y));
  }

  void UpdateMoveLabelPosition()
  {
    if(mMoveLabel)
    {
      mMoveLabel.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(
        LayoutRect((mSize.x - MOVE_LABEL_WIDTH) * 0.5f + mTextPosition.x,
                   (mSize.y - MOVE_LABEL_HEIGHT) * 0.5f + mTextPosition.y,
                   MOVE_LABEL_WIDTH, MOVE_LABEL_HEIGHT)));
    }
  }

  void UpdateStatus()
  {
    const std::string text =
      std::string("[DALI UI] ") + mLabelText + "\nTouch: color/count" +
      "\nArrow: move text" + "\nTouch count: " + std::to_string(mTouchCount) +
      "\nLast key: " + mLastKey +
      "\nSize: " + std::to_string(static_cast<int>(mSize.x)) + "x" +
      std::to_string(static_cast<int>(mSize.y)) + "\nMsg: " + mMessage;
    mStatusLabel.SetProperty(Label::Property::TEXT, text.c_str());
  }

  static void RegisterMessagePort(SampleWidget* widget)
  {
    if(gMessagePortId < 0)
    {
      gMessagePortId = message_port_register_local_port(
        MESSAGE_PORT_NAME, OnMessageReceived, nullptr);
      LOG_I("Registered local message port '%s' id=%d", MESSAGE_PORT_NAME,
            gMessagePortId);
    }
    if(!gCommandWidget)
    {
      gCommandWidget = widget;
    }
  }

  static void UnregisterMessagePort(SampleWidget* widget)
  {
    if(gCommandWidget == widget)
    {
      gCommandWidget = nullptr;
    }
    if(gMessagePortId >= 0 && !gCommandWidget)
    {
      message_port_unregister_local_port(gMessagePortId);
      gMessagePortId = -1;
    }
  }

  static void OnMessageReceived(int, const char* remoteAppId, const char*,
                                bool, bundle*    message, void*)
  {
    char* command = nullptr;
    if(bundle_get_str(message, "command", &command) == BUNDLE_ERROR_NONE &&
       command)
    {
      LOG_I("Command received from %s: %s",
            remoteAppId ? remoteAppId : "unknown", command);
      if(std::string(command) == "fault")
      {
        _exit(1);
      }
    }

    char* msg = nullptr;
    if(gCommandWidget &&
       bundle_get_str(message, "message", &msg) == BUNDLE_ERROR_NONE && msg)
    {
      gCommandWidget->mMessage = msg;
      gCommandWidget->UpdateStatus();
    }
  }

  static Dali::String DecodeCountFromBundle(const Dali::String& contentInfo)
  {
    if(contentInfo.Empty())
    {
      return Dali::String("(none)");
    }
    bundle* b =
      bundle_decode(reinterpret_cast<const bundle_raw*>(contentInfo.CStr()),
                    static_cast<int>(contentInfo.Size()));
    if(!b)
    {
      return Dali::String("(decode-failed)");
    }
    char*        count = nullptr;
    Dali::String result("(missing)");
    if(bundle_get_str(b, "COUNT", &count) == BUNDLE_ERROR_NONE && count)
    {
      result = count;
    }
    bundle_free(b);
    return result;
  }

  inline static int           gMessagePortId{-1};
  inline static SampleWidget* gCommandWidget{nullptr};

  Vector4        mInitialColor;
  const char*    mLabelText{nullptr};
  Window         mWindow;
  AbsoluteLayout mRootView;
  Label          mStatusLabel;
  Label          mMoveLabel;
  Animation      mAnimation;
  Dali::Timer    mUpdateTimer;
  Vector2        mSize{360.0f, 360.0f};
  Vector2        mTextPosition{0.0f, 0.0f};
  std::string    mLastKey{"none"};
  std::string    mMessage{"none"};
  int            mUpdateCount{1};
  uint32_t       mColorIndex{0u};
  uint32_t       mTouchCount{0u};
};

Dali::Widget CreateRedWidget(const Dali::String&)
{
  return Dali::Widget(new SampleWidget(Color::RED, "Red Widget"));
}

Dali::Widget CreateBlueWidget(const Dali::String&)
{
  return Dali::Widget(new SampleWidget(Color::BLUE, "Blue Widget"));
}

class ProviderController : public Dali::ConnectionTracker
{
public:
  ProviderController(Dali::WidgetApplication& app)
  : mApp(app)
  {
    mApp.InitSignal().Connect(this, &ProviderController::OnInit);
  }

  void OnInit(Dali::Application)
  {
    mApp.RegisterWidgetCreatingFunction(WIDGET_CLASS_ID_CLS1, &CreateRedWidget);
    mApp.RegisterWidgetCreatingFunction(WIDGET_CLASS_ID_CLS2,
                                        &CreateBlueWidget);
  }

private:
  Dali::WidgetApplication& mApp;
};

__attribute__((visibility("default"))) int main(int argc, char** argv)
{
  Dali::WidgetApplication app    = Dali::WidgetApplication::New(&argc, &argv, "");
  UiConfig                config = UiConfig::New();
  config.Apply();
  ProviderController controller(app);
  app.MainLoop();
  return 0;
}
