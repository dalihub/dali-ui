/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 */
#include <dali/devel-api/adaptor-framework/widget-engine/widget-engine-plugin.h>

#include <algorithm>
#include <array>
#include <utility>
#include <vector>

namespace
{
// Created engines, destroyed engines, created instances, destroyed instances,
// manual retries, active engines.
std::array<int, 6> counts{};

class TestInstance : public Dali::WidgetEngineInstancePlugin
{
public:
  TestInstance(std::string widgetId, std::string content, int width, int height, float period)
  : mWidgetId(std::move(widgetId)),
    mContent(std::move(content)),
    mInstanceId("test-instance-" + std::to_string(++counts[2])),
    mDimensions{width, height, 0},
    mPeriod(period)
  {
  }
  ~TestInstance() override { ++counts[3]; }
  const std::string& GetWidgetId() const override { return mWidgetId; }
  const std::string& GetInstanceId() const override { return mInstanceId; }
  const std::string& GetContentInfo() const override { return mContent; }
  const std::string& GetTitle() const override { return mWidgetId; }
  std::string GetPreviewImagePath() override { return {}; }
  int GetProcessId() const override { return -1; }
  float GetUpdatePeriod() const override { return mPeriod; }
  bool IsFaulted() const override { return mFaulted; }
  bool Pause() override { mPaused = true; return true; }
  bool Resume() override { mPaused = false; return true; }
  bool IsPaused() const { return mPaused; }
  bool               CancelTouchEvent() override
  {
    ++mCancelledTouchCount;
    return true;
  }
  int GetCancelledTouchCount() const
  {
    return mCancelledTouchCount;
  }
  void ActivateFaultedWidget() override
  {
    if(mFaulted)
    {
      ++counts[4];
      Emit(EventType::ADDED);
    }
  }
  void Resize(int width, int height) override
  {
    mDimensions[0] = width;
    mDimensions[1] = height;
    ++mDimensions[2];
  }
  int GetDimension(int index) const { return mDimensions.at(index); }
  void SetVisible(bool) override {}
  bool FeedTouchEvent(const Dali::TouchEvent&) override
  {
    ++mTouchEventCount;
    return true;
  }
  int GetTouchEventCount() const { return mTouchEventCount; }
  bool FeedKeyEvent(const Dali::KeyEvent& event) override
  {
    mLastKey = event.GetKeyName();
    ++mKeyEventCount;
    return true;
  }
  bool FeedWheelEvent(const Dali::WheelEvent& event) override
  {
    mLastWheelDelta = event.GetDelta();
    ++mWheelEventCount;
    return true;
  }
  int GetKeyEventCount() const
  {
    return mKeyEventCount;
  }
  int GetWheelEventCount() const
  {
    return mWheelEventCount;
  }
  const Dali::String& GetLastKey() const
  {
    return mLastKey;
  }
  int GetLastWheelDelta() const
  {
    return mLastWheelDelta;
  }
  void SetEventCallback(EventCallback callback) override
  {
    mEventCallback = std::move(callback);
    if(mInitialEvent)
    {
      mInitialEvent = false;
      Emit(EventType::ADDED);
    }
  }
  void SetFrameUpdatedCallback(FrameUpdatedCallback callback) override { mFrameCallback = std::move(callback); }
  void EmitFrame(Dali::NativeImagePtr image)
  {
    auto callback = mFrameCallback;
    if(callback) callback(std::move(image));
  }
  void ResetCallbacks() override
  {
    mInitialEvent = false;
    mEventCallback = {};
    mFrameCallback = {};
  }
  void Emit(EventType event)
  {
    if(event == EventType::FAULTED) mFaulted = true;
    if(event == EventType::ADDED) mFaulted = false;
    // A callback may reset itself while being dispatched.
    auto callback = mEventCallback;
    if(callback) callback(event);
  }
private:
  std::string          mWidgetId;
  std::string          mContent;
  std::string          mInstanceId;
  Dali::String         mLastKey;
  EventCallback        mEventCallback;
  FrameUpdatedCallback mFrameCallback;
  std::array<int, 3>    mDimensions; // Width, height, resize requests.
  int                  mTouchEventCount{0};
  int                  mCancelledTouchCount{0};
  int                  mKeyEventCount{0};
  int                  mWheelEventCount{0};
  int                  mLastWheelDelta{0};
  float                mPeriod;
  bool                 mFaulted{false};
  bool                 mInitialEvent{true};
  bool                 mPaused{false};
};

std::vector<TestInstance*> instances;

class TestEngine : public Dali::WidgetEnginePlugin
{
public:
  TestEngine() { ++counts[0]; ++counts[5]; }
  ~TestEngine() override { ++counts[1]; --counts[5]; }
  bool Initialize(const std::string& appId) override
  {
    // Like the platform service, only one engine may be active at a time.
    return counts[5] == 1 && appId == "test.widget.viewer";
  }
  Dali::WidgetEngineInstancePlugin* CreateWidget(const std::string& widgetId, const std::string& content, int width, int height, float period) override
  {
    if(widgetId.empty() || width <= 0 || height <= 0) return nullptr;
    auto* instance = new TestInstance(widgetId, content, width, height, period);
    instances.push_back(instance);
    return instance;
  }
  void DestroyWidget(Dali::WidgetEngineInstancePlugin* instance) override
  {
    instances.erase(std::find(instances.begin(), instances.end(), instance));
    delete instance;
  }
};
} // namespace

extern "C" Dali::WidgetEnginePlugin* CreateWidgetEnginePlugin() { return new TestEngine(); }
extern "C" void DestroyWidgetEnginePlugin(Dali::WidgetEnginePlugin* engine) { delete engine; }
extern "C" int GetWidgetTestCount(int index) { return counts.at(index); }
extern "C" void EmitWidgetTestEvent(int index, Dali::WidgetEngineInstancePlugin::EventType event) { instances.at(index)->Emit(event); }
extern "C" bool IsWidgetTestPaused(int index) { return instances.at(index)->IsPaused(); }
extern "C" int GetWidgetTestDimension(int index, int dimension) { return instances.at(index)->GetDimension(dimension); }
extern "C" int GetWidgetTestTouchEventCount(int index) { return instances.at(index)->GetTouchEventCount(); }
extern "C" int GetWidgetTestCancelledTouchCount(int index)
{
  return instances.at(index)->GetCancelledTouchCount();
}
extern "C" int GetWidgetTestKeyEventCount(int index)
{
  return instances.at(index)->GetKeyEventCount();
}
extern "C" int GetWidgetTestWheelEventCount(int index)
{
  return instances.at(index)->GetWheelEventCount();
}
extern "C" const Dali::String& GetWidgetTestLastKey(int index)
{
  return instances.at(index)->GetLastKey();
}
extern "C" int GetWidgetTestLastWheelDelta(int index)
{
  return instances.at(index)->GetLastWheelDelta();
}
extern "C" void EmitWidgetTestFrame(int index, Dali::NativeImagePtr image)
{
  instances.at(index)->EmitFrame(std::move(image));
}
