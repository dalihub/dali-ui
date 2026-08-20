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
 */

#include <dali-ui-components/dali-ui-components.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali/dali.h>

#include <functional>
#include <string>
#include <vector>

/**
 * @brief A product window chrome built on WindowFrame.
 *
 * This is a starting point to copy into a product framework, not a library.
 * It owns the border - decoration actors, layout configuration and frame style - so
 * the product decides how every window in it looks, and one edit here reaches
 * every application.
 *
 * Applications see four calls:
 *
 * @code
 * mWindow = std::make_unique<DesktopWindow>(window, DesktopWindow::CloseCallback::New(this, &App::Quit));
 * mWindow->SetTitle("Gallery");
 * mWindow->GetContentRoot().Add(myContent);
 * mWindow->AddBarAction("share.png", DesktopWindow::ActionCallback::New(this, &App::Share));
 * @endcode
 *
 * They never subtract border thickness, never track window state and never hold
 * a decoration handle alive. WindowFrame does the hard parts - geometry, native
 * requests, move and resize interaction, overlay and size constraints - so what
 * is left here is layout plus product policy.
 */
class DesktopWindow : public Dali::ConnectionTracker
{
public:
  using CloseCallback  = Dali::Ui::Callback<void()>;
  using ActionCallback = Dali::Ui::Callback<void()>;

  /**
   * @brief Builds the chrome and attaches it to a window.
   * @param[in] window Window to decorate
   * @param[in] onClose Invoked once the close control is confirmed
   */
  DesktopWindow(Dali::Window window, CloseCallback onClose);
  ~DesktopWindow();

  DesktopWindow(const DesktopWindow&)            = delete;
  DesktopWindow& operator=(const DesktopWindow&) = delete;

  // ---- what applications use ----

  /**
   * @brief Returns the host for application content.
   *
   * Border thickness is already subtracted, and the host survives every frame
   * change, so an application adds its root here once.
   */
  Dali::Ui::View GetContentRoot() const;

  /**
   * @brief Adds an application action to the bottom bar, before the built-in
   *        minimize, maximize/restore, and close buttons.
   *
   * The button takes a column beside the move region rather than sitting inside
   * it. WindowFrame makes the move region insensitive while the window is
   * maximized and an insensitive actor hides its children from hit testing, so a
   * button placed inside would stop responding there.
   *
   * The new column widens what the bar needs, so the minimum frame size is
   * recomputed and reapplied.
   *
   * @param[in] iconResource Icon file name in this sample's res/ directory
   * @param[in] onClick Invoked when the button is clicked
   * @return The installed button, for styling
   */
  Dali::Ui::View AddBarAction(const std::string& iconResource, ActionCallback onClick);

  /**
   * @brief Returns the strip along the top edge, for a title or branding.
   *
   * Not the move region, so its children keep working while maximized.
   */
  Dali::Ui::View GetTopArea() const;

  /**
   * @brief Sets the text shown in the top area.
   */
  void SetTitle(const std::string& title);

  // ---- what the product configures ----

  /**
   * @brief Sets the smallest window the product allows.
   *
   * The frame needs a floor of its own - the corner handles and every control
   * column - and the larger of the two is applied. Adding a bar action raises
   * the frame's floor, so this is reapplied automatically.
   */
  void SetMinimumFrameSize(const Dali::Vector2& size);

private:
  void BuildDecoration();
  void UpdateBarColumnLayout();
  void UpdateStateDependentVisuals();
  void ApplyMinimumFrameSize();

  Dali::Vector2 RequiredFrameSize() const;

  Dali::Ui::GridLayout MakeIconButton(const std::string&    iconResource,
                                      float                 iconSize,
                                      std::function<void()> onClick    = {},
                                      Dali::Ui::ImageView*  iconHandle = nullptr);

  Dali::Ui::WindowFrameCommandDisposition OnCommand(Dali::Ui::WindowFrame sender, const Dali::Ui::WindowFrameCommandRequest& request);

private:
  // Dali::Ui::Callback is move-only, so a click handler cannot capture one. The
  // callbacks live here and each button's handler reaches its own by index.
  struct BarAction
  {
    Dali::Ui::GridLayout button;
    ActionCallback       onClick;
  };

  Dali::Ui::WindowFrame  mWindowFrame;
  Dali::Ui::GridLayout   mBar;
  Dali::Ui::GridLayout   mMoveRegion;
  Dali::Ui::GridLayout   mTopArea;
  Dali::Ui::Label        mTitle;
  Dali::Ui::ImageView    mMaximizeIcon;
  Dali::Ui::GridLayout   mMinimizeControl;
  Dali::Ui::GridLayout   mMaximizeRestoreControl;
  Dali::Ui::GridLayout   mCloseControl;
  std::vector<BarAction> mBarActions;
  Dali::Vector2          mConfiguredMinimum;
};
