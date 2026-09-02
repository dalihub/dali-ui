#pragma once

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
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/public-api/signals/slot-delegate.h>
#include <string>
#include <memory>
#include <utility>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui/ui-scene-holder-impl.h>
#include <dali-ui/ui-window.h>

namespace Dali
{
using PositionSize = Dali::BoundsInteger;

namespace Internal
{
namespace Adaptor
{
class Window : public SceneHolder
{
public:
  Window(const PositionSize& positionSize);
  virtual ~Window() = default;
  static Window* New(const PositionSize& positionSize, const String& name, const String& className, bool isTransparent);

  PositionSize                 GetPositionSize() const;
  std::string                  GetNativeResourceId() const;
  void                         KeepRendering(float durationSeconds);

  void SetPositionSize(PositionSize positionSize);

  Dali::Window::KeyEventSignalType&   KeyEventSignal();
  Dali::Window::TouchEventSignalType& TouchEventSignal();

  void OnKeyEvent(Dali::Integration::SceneHolder sceneHolder, Dali::KeyEvent event);
  void OnTouchEvent(Dali::Integration::SceneHolder sceneHolder, Dali::TouchEvent event);

  bool IsMaximized() const;
  void Maximize(bool maximize);
  bool IsMinimized() const;
  void Minimize(bool minimize);
  void SetMinimumSize(Dali::Window::WindowSize size);
  void SetMaximumSize(Dali::Window::WindowSize size);
  void AddFramePresentedCallback(Dali::CallbackBase* callback, int32_t frameId);

  /**
   * @brief Invokes and clears every queued frame-presented callback.
   *
   * Lets a test drive the presentation step that the window system would
   * normally report.
   */
  void EmitFramePresented();

  FocusChangedSignalType                      mFocusChangedSignal;
  ResizedSignalType                           mResizedSignal;
  Dali::Window::MovedSignalType                mMovedSignal;
  Dali::Window::MovedSignalType                mMoveCompletedSignal;
  Dali::Window::ResizedSignalType              mResizeCompletedSignal;
  int                                        mRotationAngle;
  bool                                       mVisible;
  Dali::Window::VisibilityChangedSignalType mVisibilityChangedSignal;
  Dali::Window::KeyEventSignalType          mKeyEventSignal;
  Dali::Window::TouchEventSignalType        mTouchEventSignal;
  Dali::Window::WheelEventSignalType        mWheelEventSignal;
  Dali::SlotDelegate<Window>                mSlotDelegate;
  Dali::Window::WindowSize                  mMinimumSize{0, 0};
  Dali::Window::WindowSize                  mMaximumSize{0, 0};
  std::vector<std::pair<std::unique_ptr<Dali::CallbackBase>, int32_t>> mFramePresentedCallbacks;
  bool                                      mMaximized{false};
  bool                                      mMinimized{false};
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
