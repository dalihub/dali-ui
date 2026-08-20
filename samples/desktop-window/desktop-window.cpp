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

#include "desktop-window.h"

#include <dali/integration-api/debug.h>

#include <algorithm>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
// The product's window chrome. One edit here reaches every application.
constexpr float TOP_AREA_HEIGHT  = 44.0f;
constexpr float BAR_HEIGHT       = 50.0f;
constexpr float EDGE_THICKNESS   = 5.0f;
constexpr float CORNER_RADIUS    = 12.0f;
constexpr float CONTROL_WIDTH    = 44.0f;
constexpr float RESIZE_HANDLE    = 50.0f;
constexpr float ICON_SIZE        = 40.0f;
constexpr float HANDLE_ICON_SIZE = 40.0f;
constexpr float SHADOW_BLUR      = 12.0f;
constexpr float SHADOW_OFFSET_Y  = 3.0f;

// The move region has to keep a strip wide enough to grab, so it counts towards
// the smallest frame that still lays out. See RequiredFrameSize().
constexpr float MINIMUM_MOVE_REGION_WIDTH = 44.0f;

// The size to come back to after a full-size launch. Used only then; see the
// constructor.
constexpr float RESTORE_FRAME_WIDTH  = 400.0f;
constexpr float RESTORE_FRAME_HEIGHT = 260.0f;

const UiColor CHROME_COLOR = UiColor(0x1F2933);
const UiColor TITLE_COLOR  = UiColor(0xF5F7FA);

// Columns before the actions: the corner handle and the move region.
constexpr uint32_t COLUMN_MOVE_REGION = 1u;
constexpr uint32_t FIXED_CONTROLS     = 3u;

Dali::String ResourceUrl(const std::string& resourceName)
{
  return Dali::String(RESOURCES_DIR) + resourceName.c_str();
}
} // unnamed namespace

DesktopWindow::DesktopWindow(Dali::Window window, CloseCallback onClose)
: mConfiguredMinimum(Vector2::ZERO)
{
  WindowFrameOptions options;
  options.SetInitialStatePolicy(WindowFrameInitialStatePolicy::AUTO);

  // A window that launches at screen size is maximized from the start, so there
  // is no earlier normal geometry for the window system to restore to. Supply
  // one. It is consulted only for that first restore - ordinary maximize and
  // restore geometry stays compositor owned, which is what keeps repeated
  // toggling from drifting.
  options.SetInitialRestoreFrameSize(Vector2(RESTORE_FRAME_WIDTH, RESTORE_FRAME_HEIGHT));

  // A product that does not offer a control turns its feature off here, so the
  // decision is made once rather than per application.
  mWindowFrame = WindowFrame::New(window, std::move(onClose), options);

  // Product policy sits in front of the default commands.
  mWindowFrame.SetCommandInterceptor(
    WindowFrame::CommandInterceptor::New(this, &DesktopWindow::OnCommand));

  // The chrome follows the window state; applications never do this themselves.
  mWindowFrame.WindowStateChangedSignal().Connect(this, [this](WindowFrame, WindowFrame::WindowState)
  {
    UpdateStateDependentVisuals();
  });

  BuildDecoration();
  mWindowFrame.Attach();
  UpdateStateDependentVisuals();
  ApplyMinimumFrameSize();
}

DesktopWindow::~DesktopWindow()
{
  // The interceptor is a plain callback bound to this object, not a tracked
  // signal connection, so it has to be dropped by hand. WindowFrame would
  // otherwise keep calling into a destroyed wrapper if anything else still
  // holds a handle to the window.
  if(mWindowFrame)
  {
    mWindowFrame.SetCommandInterceptor(WindowFrame::CommandInterceptor());
  }
}

Dali::Ui::View DesktopWindow::GetContentRoot() const
{
  return mWindowFrame.GetContentRoot();
}

Dali::Ui::View DesktopWindow::GetTopArea() const
{
  return mTopArea;
}

void DesktopWindow::SetTitle(const std::string& title)
{
  if(mTitle)
  {
    mTitle.SetText(Dali::String(title.c_str()));
  }
}

void DesktopWindow::SetMinimumFrameSize(const Dali::Vector2& size)
{
  mConfiguredMinimum = size;
  ApplyMinimumFrameSize();
}

Dali::Ui::View DesktopWindow::AddBarAction(const std::string& iconResource, ActionCallback onClick)
{
  const std::size_t    index  = mBarActions.size();
  Dali::Ui::GridLayout button = MakeIconButton(iconResource, ICON_SIZE, [this, index]
  {
    if(index < mBarActions.size() && mBarActions[index].onClick)
    {
      mBarActions[index].onClick.Invoke();
    }
  });

  mBarActions.push_back(BarAction{button, std::move(onClick)});
  mBar.Add(button);

  // A new column shifts the controls along and widens what the bar needs.
  UpdateBarColumnLayout();
  ApplyMinimumFrameSize();
  return button;
}

// ---------------------------------------------------------------------------
// Frame construction. This is the only place the product's look is decided.
// ---------------------------------------------------------------------------

void DesktopWindow::BuildDecoration()
{
  WindowFrameLayout layout;
  layout.SetDecorationInsets(Insets(EDGE_THICKNESS, EDGE_THICKNESS, TOP_AREA_HEIGHT, BAR_HEIGHT));
  layout.SetBottomLeftResizeHandleSize(Vector2(RESIZE_HANDLE, RESIZE_HANDLE));
  layout.SetBottomRightResizeHandleSize(Vector2(RESIZE_HANDLE, RESIZE_HANDLE));

  // Top area: a strip the application fills. Unlike the move region it stays
  // sensitive while maximized, so anything interactive belongs here.
  mTopArea = GridLayout::New();
  mTopArea.SetBackgroundColor(CHROME_COLOR);
  mTopArea.AddRowDefinition(GridLength::Star());
  mTopArea.AddColumnDefinition(GridLength::Star());
  mTitle = Label::New(Dali::String(""));
  mTitle.SetFontSize(15.0f);
  mTitle.SetTextColor(TITLE_COLOR);
  mTitle.SetVerticalTextAlignment(Text::Alignment::CENTER);
  mTitle.SetMargin(Extents(14u, 0u, 0u, 0u));
  mTitle.SetSensitive(false);
  mTopArea.Add(mTitle);

  // Bottom bar. Columns are assigned by UpdateBarColumnLayout(), because an action
  // added later shifts every control along.
  mBar = GridLayout::New();
  mBar.SetBackgroundColor(CHROME_COLOR);
  mBar.AddRowDefinition(GridLength::Star());

  mMoveRegion = GridLayout::New();
  mBar.Add(mMoveRegion);

  mMinimizeControl        = MakeIconButton("minimize.png", ICON_SIZE);
  mMaximizeRestoreControl = MakeIconButton("maximize.png", ICON_SIZE, {}, &mMaximizeIcon);
  mCloseControl           = MakeIconButton("close.png", ICON_SIZE);
  mBar.Add(mMinimizeControl);
  mBar.Add(mMaximizeRestoreControl);
  mBar.Add(mCloseControl);

  View left = View::New();
  left.SetBackgroundColor(CHROME_COLOR);
  left.SetSensitive(false);
  View right = View::New();
  right.SetBackgroundColor(CHROME_COLOR);
  right.SetSensitive(false);

  WindowFrameStyle style;
  style.SetShadowOutsets(Insets(SHADOW_BLUR, SHADOW_BLUR, SHADOW_BLUR - SHADOW_OFFSET_Y, SHADOW_BLUR + SHADOW_OFFSET_Y));
  style.SetShadow(Shadow(SHADOW_BLUR,
                         Vector2(0.0f, SHADOW_OFFSET_Y),
                         UiColor(0x000000, 0.45f),
                         Vector2::ZERO,
                         CutoutPolicy::CUTOUT_VIEW_WITH_CORNER_RADIUS));

  // The decoration paints itself, so the frame stays transparent and the area
  // the application draws into is never tinted.
  style.SetFrameBackgroundColor(UiColor(0x000000, 0.0f));

  WindowFrameDecoration decoration;
  decoration.SetTopSlot(mTopArea);
  decoration.SetBottomSlot(mBar);
  decoration.SetLeftSlot(left);
  decoration.SetRightSlot(right);
  decoration.SetMoveRegion(mMoveRegion);
  decoration.SetMinimizeControl(mMinimizeControl);
  decoration.SetMaximizeRestoreControl(mMaximizeRestoreControl);
  decoration.SetCloseControl(mCloseControl);
  decoration.SetBottomLeftResizeHandle(MakeIconButton("resize-bottom-left.png", HANDLE_ICON_SIZE));
  decoration.SetBottomRightResizeHandle(MakeIconButton("resize-bottom-right.png", HANDLE_ICON_SIZE));

  UpdateBarColumnLayout();

  // Topology is validated before anything is installed, so a rejected frame
  // leaves the previous one untouched. Worth checking while a frame is being
  // developed; a product can drop it once the layout is settled.
  const WindowFrameDecorationResult result =
    mWindowFrame.SetDecoration(decoration, layout, style, WindowFrameSizePolicy::KEEP_WINDOW_SIZE);
  if(result != WindowFrameDecorationResult::INSTALLED)
  {
    DALI_LOG_ERROR("[DesktopWindow] the frame was rejected: %d\n", static_cast<int>(result));
  }
}

void DesktopWindow::UpdateBarColumnLayout()
{
  // Column order: corner handle, move region, application actions, window
  // controls, corner handle. GridLayout can only append a column, so adding an
  // action redefines them all on the existing bar rather than rebuilding it -
  // rebuilding would discard whatever the application put in the move region.
  mBar.ClearColumnDefinitions();
  mBar.AddColumnDefinition(GridLength::Absolute(RESIZE_HANDLE));
  mBar.AddColumnDefinition(GridLength::Star());
  for(std::size_t action = 0u; action < mBarActions.size(); ++action)
  {
    mBar.AddColumnDefinition(GridLength::Absolute(CONTROL_WIDTH));
  }
  for(uint32_t control = 0u; control < FIXED_CONTROLS; ++control)
  {
    mBar.AddColumnDefinition(GridLength::Absolute(CONTROL_WIDTH));
  }
  mBar.AddColumnDefinition(GridLength::Absolute(RESIZE_HANDLE));

  uint32_t column = COLUMN_MOVE_REGION;
  mMoveRegion.SetLayoutParams(GridLayoutParams::New().SetRow(0u).SetColumn(column++));
  for(BarAction& action : mBarActions)
  {
    action.button.SetLayoutParams(GridLayoutParams::New().SetRow(0u).SetColumn(column++));
  }
  mMinimizeControl.SetLayoutParams(GridLayoutParams::New().SetRow(0u).SetColumn(column++));
  mMaximizeRestoreControl.SetLayoutParams(GridLayoutParams::New().SetRow(0u).SetColumn(column++));
  mCloseControl.SetLayoutParams(GridLayoutParams::New().SetRow(0u).SetColumn(column++));
}

Dali::Ui::GridLayout DesktopWindow::MakeIconButton(const std::string&    iconResource,
                                                   float                 iconSize,
                                                   std::function<void()> onClick,
                                                   Dali::Ui::ImageView*  iconHandle)
{
  GridLayout button = GridLayout::New();
  button.AddRowDefinition(GridLength::Star());
  button.AddColumnDefinition(GridLength::Star());
  if(onClick)
  {
    button.AsInteractive().ConnectClickedSignal(this, [onClick = std::move(onClick)](View, InputEvent) -> bool
    {
      onClick();
      return true;
    });
  }

  ImageView icon = ImageView::New(ResourceUrl(iconResource));
  icon.SetRequestedWidth(iconSize);
  icon.SetRequestedHeight(iconSize);

  // The visual is not the button: leaving it out of hit testing makes the whole
  // column one uniform touch target.
  icon.SetSensitive(false);
  icon.SetLayoutParams(GridLayoutParams::New()
                         .SetHorizontalAlignment(LayoutAlignment::CENTER)
                         .SetVerticalAlignment(LayoutAlignment::CENTER));
  button.Add(icon);
  if(iconHandle)
  {
    *iconHandle = icon;
  }
  return button;
}

// ---------------------------------------------------------------------------
// State, sizing and policy
// ---------------------------------------------------------------------------

void DesktopWindow::UpdateStateDependentVisuals()
{
  const bool maximized = mWindowFrame.IsMaximized();

  if(mMaximizeIcon)
  {
    mMaximizeIcon.SetResourceUrl(ResourceUrl(maximized ? "restore.png" : "maximize.png"));
  }

  // A maximized window is square. The decoration owns the outer corners, so
  // squaring only the frame root would leave rounded notches cut out of it.
  const float radius = maximized ? 0.0f : CORNER_RADIUS;
  mTopArea.SetCornerRadius(Vector4(radius, radius, 0.0f, 0.0f));
  mTopArea.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);
  mBar.SetCornerRadius(Vector4(0.0f, 0.0f, radius, radius));
  mBar.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);
}

Dali::Vector2 DesktopWindow::RequiredFrameSize() const
{
  // Every column below is a fixed width the bar reserves whatever the frame
  // size is. Below their total the move region collapses and the close control
  // slides under the corner handle, where the handle takes the touch and a
  // resize starts instead of a close.
  const float controls = static_cast<float>(FIXED_CONTROLS + mBarActions.size()) * CONTROL_WIDTH;
  return Vector2(2.0f * RESIZE_HANDLE + controls + MINIMUM_MOVE_REGION_WIDTH,
                 TOP_AREA_HEIGHT + BAR_HEIGHT);
}

void DesktopWindow::ApplyMinimumFrameSize()
{
  const Vector2 required = RequiredFrameSize();
  mWindowFrame.SetMinimumFrameSize(Vector2(std::max(mConfiguredMinimum.width, required.width),
                                           std::max(mConfiguredMinimum.height, required.height)));
}

Dali::Ui::WindowFrameCommandDisposition DesktopWindow::OnCommand(Dali::Ui::WindowFrame /*sender*/, const Dali::Ui::WindowFrameCommandRequest& /*request*/)
{
  // Where a product puts rules an application cannot bypass: a confirmation
  // before closing, a profile that forbids minimizing, telemetry. Returning
  // HANDLED means this wrapper owns the command from here and WindowFrame
  // performs no default operation for it.
  return WindowFrameCommandDisposition::CONTINUE_DEFAULT;
}
