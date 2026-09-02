#pragma once

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

#include <dali-ui-components/public-api/window/default-window-decoration.h>

#include <dali-ui-foundation/dali-ui-foundation.h>

#include <vector>

namespace Dali::Ui
{
namespace Internal
{

// Builds the ready-made decoration and keeps its state-dependent icons in step
// with the window. Owns no window policy; every command still travels through
// WindowFrame's own control roles.
class DefaultWindowDecorationImpl : public Dali::BaseObject, public Dali::ConnectionTracker
{
public:
  DefaultWindowDecorationImpl(WindowFrame windowFrame, const DefaultWindowDecorationOptions& options);

  DefaultWindowDecorationImpl(const DefaultWindowDecorationImpl&)            = delete;
  DefaultWindowDecorationImpl(DefaultWindowDecorationImpl&&)                 = delete;
  DefaultWindowDecorationImpl& operator=(const DefaultWindowDecorationImpl&) = delete;
  DefaultWindowDecorationImpl& operator=(DefaultWindowDecorationImpl&&)      = delete;

  /**
   * @brief Installs the built frame and connects state tracking.
   * @return Installation result
   */
  WindowFrameDecorationResult Install();

  WindowFrameDecoration GetDecoration() const;
  Dali::Ui::View        GetMoveRegion() const;
  Dali::Ui::View        GetTopArea() const;
  Dali::Ui::View        AddBarAction(Dali::StringView iconUrl, Dali::Callback<void()> onClick);
  Dali::Vector2         GetMinimumFrameSize() const;

protected:
  ~DefaultWindowDecorationImpl() override;

private:
  Dali::Ui::ImageView  MakeBuiltInIcon(const char* name, float size) const;
  Dali::Ui::ImageView  MakeIcon(const Dali::String& url, float size) const;
  Dali::Ui::View       MakeResizeHandle(bool leftCorner) const;
  Dali::Ui::GridLayout MakeControl(Dali::Ui::View icon) const;

  float GetEffectiveTopHeight() const;
  void  BuildDecoration();
  void  UpdateBarColumnLayout();
  void  UpdateStateDependentVisuals();
  void  OnWindowStateChanged(WindowFrame sender, WindowFrame::WindowState state);

  Dali::Ui::UiColor GetIconColor() const;

private:
  WindowFrame                    mWindowFrame;
  DefaultWindowDecorationOptions mOptions;
  WindowFrameDecoration          mDecoration;
  WindowFrameLayout              mLayout;
  Dali::Ui::View                 mMoveRegion;
  Dali::Ui::View                 mTopArea;
  Dali::Ui::ImageView            mMaximizeRestoreIcon;
  Dali::Ui::GridLayout           mBar;

  // Actions the application added, in the order it added them. Dali::Callback
  // is move-only, so the callback lives here and the button's handler reaches it
  // by index rather than capturing a copy.
  struct BarAction
  {
    Dali::Ui::GridLayout   button;
    Dali::Callback<void()> onClick;
  };
  std::vector<BarAction> mBarActions;
};

} // namespace Internal

inline Internal::DefaultWindowDecorationImpl& GetImplementation(DefaultWindowDecoration& handle)
{
  DALI_ASSERT_ALWAYS(handle && "DefaultWindowDecoration handle is empty");
  return static_cast<Internal::DefaultWindowDecorationImpl&>(handle.GetBaseObject());
}

inline const Internal::DefaultWindowDecorationImpl& GetImplementation(const DefaultWindowDecoration& handle)
{
  DALI_ASSERT_ALWAYS(handle && "DefaultWindowDecoration handle is empty");
  return static_cast<const Internal::DefaultWindowDecorationImpl&>(handle.GetBaseObject());
}

} // namespace Dali::Ui
