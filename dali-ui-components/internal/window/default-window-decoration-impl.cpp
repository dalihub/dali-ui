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

#include "default-window-decoration-impl.h"

#include <dali/devel-api/adaptor-framework/environment-variable.h>
#include <dali/integration-api/debug.h>

#include <algorithm>

namespace Dali::Ui
{
namespace Internal
{
namespace
{
constexpr float CONTROL_WIDTH = 44.0f;

// The bar reserves an absolute column per control and per resize handle. Below
// their total the move region collapses and the last control slides under the
// corner handle, so the border declares that total as its minimum.
constexpr float MINIMUM_MOVE_REGION_WIDTH = CONTROL_WIDTH;

// Control icons follow the bar height, clamped so they cannot outgrow the
// control they sit in. Resize handle icons follow the handle instead.
constexpr float CONTROL_ICON_RATIO = 0.64f;
constexpr float HANDLE_ICON_RATIO  = 0.72f;

// The shadow sits in surface space outside the frame. Its outsets follow the
// blur so the blurred edge is never clipped by the surface.
constexpr float SHADOW_BLUR     = 12.0f;
constexpr float SHADOW_OFFSET_Y = 3.0f;

// The bar starts with the corner handle, then the move region. Application
// actions and the window controls follow, so their columns depend on how many
// actions were added and are assigned by UpdateBarColumnLayout().
constexpr uint32_t COLUMN_MOVE_REGION = 1u;

/**
 * @brief Returns the directory holding the default decoration icons.
 *
 * DALI_UI_COMPONENTS_IMAGE_DIR is either a path or zero. Zero means the build
 * does not use the default resource directory, and the path comes from the
 * environment instead, matching how every other DALi asset directory resolves.
 */
const std::string& GetIconDirectory()
{
  static const std::string directory = (nullptr == DALI_UI_COMPONENTS_IMAGE_DIR)
                                         ? Dali::EnvironmentVariable::GetEnvironmentVariable("DALI_UI_COMPONENTS_IMAGE_DIR")
                                         : DALI_UI_COMPONENTS_IMAGE_DIR;
  return directory;
}

Dali::String GetBuiltInIconUrl(const char* name)
{
  return Dali::String((GetIconDirectory() + "border/" + name).c_str());
}
} // unnamed namespace

DefaultWindowDecorationImpl::DefaultWindowDecorationImpl(WindowFrame windowFrame, const DefaultWindowDecorationOptions& options)
: mWindowFrame(windowFrame),
  mOptions(options)
{
  BuildDecoration();
}

DefaultWindowDecorationImpl::~DefaultWindowDecorationImpl() = default;

Dali::Ui::UiColor DefaultWindowDecorationImpl::GetIconColor() const
{
  return mOptions.GetTheme() == DefaultWindowDecorationTheme::LIGHT ? Dali::Ui::UiColor(0x1F1F1F) : Dali::Ui::UiColor(0xFFFFFF);
}

Dali::Ui::ImageView DefaultWindowDecorationImpl::MakeBuiltInIcon(const char* name, float size) const
{
  // The shipped assets are white glyphs with an alpha channel, so one set serves
  // both themes: the view colour multiplies through to the requested icon
  // colour. An application icon gets neither treatment - see MakeIcon().
  Dali::Ui::ImageView icon = MakeIcon(GetBuiltInIconUrl(name), size);
  icon.SetColor(GetIconColor());
  return icon;
}

Dali::Ui::ImageView DefaultWindowDecorationImpl::MakeIcon(const Dali::String& url, float size) const
{
  Dali::Ui::ImageView icon = Dali::Ui::ImageView::New(url);
  icon.SetRequestedWidth(size);
  icon.SetRequestedHeight(size);
  icon.SetSensitive(false);
  icon.SetLayoutParams(Dali::Ui::GridLayoutParams::New()
                         .SetHorizontalAlignment(Dali::Ui::LayoutAlignment::CENTER)
                         .SetVerticalAlignment(Dali::Ui::LayoutAlignment::CENTER));
  return icon;
}

Dali::Ui::GridLayout DefaultWindowDecorationImpl::MakeControl(Dali::Ui::View icon) const
{
  // The column is assigned by UpdateBarColumnLayout(), since an action added later
  // shifts every control along.
  Dali::Ui::GridLayout control = Dali::Ui::GridLayout::New();
  control.AddRowDefinition(Dali::Ui::GridLength::Star());
  control.AddColumnDefinition(Dali::Ui::GridLength::Star());
  control.Add(icon);
  return control;
}

Dali::Ui::View DefaultWindowDecorationImpl::MakeResizeHandle(bool leftCorner) const
{
  const float          handleSize = mOptions.GetBarHeight();
  Dali::Ui::GridLayout handle     = Dali::Ui::GridLayout::New();
  handle.AddRowDefinition(Dali::Ui::GridLength::Star());
  handle.AddColumnDefinition(Dali::Ui::GridLength::Star());
  handle.SetRequestedWidth(handleSize);
  handle.SetRequestedHeight(handleSize);
  handle.Add(MakeBuiltInIcon(leftCorner ? "resize-bottom-left.png" : "resize-bottom-right.png", handleSize * HANDLE_ICON_RATIO));
  return handle;
}

float DefaultWindowDecorationImpl::GetEffectiveTopHeight() const
{
  // A configured top area replaces the thin edge, but never shrinks below it:
  // the frame still needs a border on that side.
  return std::max(mOptions.GetEdgeThickness(), mOptions.GetTopAreaHeight());
}

void DefaultWindowDecorationImpl::BuildDecoration()
{
  const float barHeight     = mOptions.GetBarHeight();
  const float edgeThickness = mOptions.GetEdgeThickness();
  const float topHeight     = GetEffectiveTopHeight();
  const float handleWidth   = mOptions.IsResizeHandlesEnabled() ? barHeight : 0.0f;
  const float iconSize      = std::min(barHeight * CONTROL_ICON_RATIO, CONTROL_WIDTH * 0.8f);

  // The top edge matches the sides, so the frame is enclosed on every side,
  // unless the caller asked for a usable top area.
  mLayout.SetDecorationInsets(Dali::Ui::Insets(edgeThickness, edgeThickness, topHeight, barHeight));
  mLayout.SetBottomLeftResizeHandleSize(Dali::Vector2(handleWidth, handleWidth));
  mLayout.SetBottomRightResizeHandleSize(Dali::Vector2(handleWidth, handleWidth));

  mBarActions.clear();
  Dali::Ui::GridLayout bar = Dali::Ui::GridLayout::New();
  bar.AddRowDefinition(Dali::Ui::GridLength::Star());
  mBar = bar;

  // The whole free stretch of the bar starts a move and can contain a title.
  Dali::Ui::GridLayout moveRegion = Dali::Ui::GridLayout::New();
  moveRegion.AddRowDefinition(Dali::Ui::GridLength::Star());
  moveRegion.AddColumnDefinition(Dali::Ui::GridLength::Star());
  bar.Add(moveRegion);
  mMoveRegion = moveRegion;

  mDecoration.SetBottomSlot(bar);
  mDecoration.SetMoveRegion(moveRegion);

  mTopArea.Reset();
  if(topHeight > 0.0f)
  {
    Dali::Ui::View top = Dali::Ui::View::New();

    // A thin edge is decoration and must not swallow input. An area the caller
    // fills has to take it, and an insensitive parent would hide whatever they
    // put inside from hit testing.
    const bool isArea = mOptions.GetTopAreaHeight() > 0.0f;
    top.SetSensitive(isArea);
    mDecoration.SetTopSlot(top);
    if(isArea)
    {
      mTopArea = top;
    }
  }
  if(edgeThickness > 0.0f)
  {
    Dali::Ui::View left = Dali::Ui::View::New();
    left.SetSensitive(false);
    Dali::Ui::View right = Dali::Ui::View::New();
    right.SetSensitive(false);
    mDecoration.SetLeftSlot(left);
    mDecoration.SetRightSlot(right);
  }

  if(mOptions.IsMinimizeControlEnabled())
  {
    Dali::Ui::GridLayout minimize = MakeControl(MakeBuiltInIcon("minimize.png", iconSize));
    bar.Add(minimize);
    mDecoration.SetMinimizeControl(minimize);
  }

  if(mOptions.IsMaximizeRestoreControlEnabled())
  {
    mMaximizeRestoreIcon          = MakeBuiltInIcon("maximize.png", iconSize);
    Dali::Ui::GridLayout maximize = MakeControl(mMaximizeRestoreIcon);
    bar.Add(maximize);
    mDecoration.SetMaximizeRestoreControl(maximize);
  }

  if(mOptions.IsCloseControlEnabled())
  {
    Dali::Ui::GridLayout close = MakeControl(MakeBuiltInIcon("close.png", iconSize));
    bar.Add(close);
    mDecoration.SetCloseControl(close);
  }

  if(mOptions.IsResizeHandlesEnabled())
  {
    mDecoration.SetBottomLeftResizeHandle(MakeResizeHandle(true));
    mDecoration.SetBottomRightResizeHandle(MakeResizeHandle(false));
  }

  UpdateBarColumnLayout();
}

void DefaultWindowDecorationImpl::UpdateBarColumnLayout()
{
  if(!mBar)
  {
    return;
  }

  // GridLayout can only append a column, so an action added later is placed by
  // redefining every column on the existing bar. Rebuilding the bar instead
  // would discard whatever the application put in the move region.
  const float handleWidth = mOptions.IsResizeHandlesEnabled() ? mOptions.GetBarHeight() : 0.0f;

  mBar.ClearColumnDefinitions();
  mBar.AddColumnDefinition(Dali::Ui::GridLength::Absolute(handleWidth));
  mBar.AddColumnDefinition(Dali::Ui::GridLength::Star());
  for(std::size_t action = 0u; action < mBarActions.size(); ++action)
  {
    mBar.AddColumnDefinition(Dali::Ui::GridLength::Absolute(CONTROL_WIDTH));
  }
  mBar.AddColumnDefinition(Dali::Ui::GridLength::Absolute(mOptions.IsMinimizeControlEnabled() ? CONTROL_WIDTH : 0.0f));
  mBar.AddColumnDefinition(Dali::Ui::GridLength::Absolute(mOptions.IsMaximizeRestoreControlEnabled() ? CONTROL_WIDTH : 0.0f));
  mBar.AddColumnDefinition(Dali::Ui::GridLength::Absolute(mOptions.IsCloseControlEnabled() ? CONTROL_WIDTH : 0.0f));
  mBar.AddColumnDefinition(Dali::Ui::GridLength::Absolute(handleWidth));

  uint32_t column = COLUMN_MOVE_REGION;
  if(mMoveRegion)
  {
    mMoveRegion.SetLayoutParams(Dali::Ui::GridLayoutParams::New().SetRow(0u).SetColumn(column));
  }
  ++column;
  for(BarAction& action : mBarActions)
  {
    action.button.SetLayoutParams(Dali::Ui::GridLayoutParams::New().SetRow(0u).SetColumn(column++));
  }
  // A disabled control has a zero-width column, so it still takes an index.
  for(Dali::Ui::View control : {mDecoration.GetMinimizeControl(), mDecoration.GetMaximizeRestoreControl(), mDecoration.GetCloseControl()})
  {
    if(control)
    {
      control.SetLayoutParams(Dali::Ui::GridLayoutParams::New().SetRow(0u).SetColumn(column));
    }
    ++column;
  }
}

Dali::Ui::View DefaultWindowDecorationImpl::AddBarAction(Dali::StringView iconUrl, Dali::Callback<void()> onClick)
{
  if(!mBar)
  {
    return Dali::Ui::View();
  }

  // The URL is the application's, so it is used as given: no component asset
  // directory in front of it and no theme colour multiplied through it.
  const float          iconSize = std::min(mOptions.GetBarHeight() * CONTROL_ICON_RATIO, CONTROL_WIDTH * 0.8f);
  Dali::Ui::ImageView  icon     = MakeIcon(Dali::String(iconUrl), iconSize);
  Dali::Ui::GridLayout button   = MakeControl(icon);

  const std::size_t index = mBarActions.size();
  button.AsInteractive().ConnectClickedSignal(this, [this, index](Dali::Ui::View, Dali::Ui::InputEvent) -> bool
  {
    // The callback is the application's and can release the last DefaultWindowDecoration
    // handle, so this object holds one on itself until the call returns.
    Dali::BaseHandle self(this);

    // BuildDecoration() clears the actions, so an index is checked rather than
    // trusted.
    if(index < mBarActions.size() && mBarActions[index].onClick)
    {
      mBarActions[index].onClick.Invoke();
    }
    return true;
  });

  mBarActions.push_back(BarAction{button, std::move(onClick)});
  mBar.Add(button);
  UpdateBarColumnLayout();
  return button;
}

WindowFrameDecorationResult DefaultWindowDecorationImpl::Install()
{
  const Dali::Ui::UiColor background = mOptions.GetBackgroundColor();

  const float radius = mOptions.GetCornerRadius();

  WindowFrameStyle style;
  if(mOptions.IsShadowEnabled())
  {
    // The cutout matters here: the frame is transparent, so a shadow without
    // one would darken the area the application draws into.
    style.SetShadowOutsets(Dali::Ui::Insets(SHADOW_BLUR, SHADOW_BLUR, SHADOW_BLUR - SHADOW_OFFSET_Y, SHADOW_BLUR + SHADOW_OFFSET_Y));
    style.SetShadow(Dali::Ui::Shadow(SHADOW_BLUR,
                                     Dali::Vector2(0.0f, SHADOW_OFFSET_Y),
                                     Dali::Ui::UiColor(0x000000, 0.45f),
                                     Dali::Vector2::ZERO,
                                     Dali::Ui::CutoutPolicy::CUTOUT_VIEW_WITH_CORNER_RADIUS));
  }
  // Only the decoration paints. The frame background stays transparent so the
  // area the application draws into is not tinted by the border.
  style.SetFrameBackgroundColor(Dali::Ui::UiColor(0x000000, 0.0f));
  style.SetFrameCornerRadius(Dali::Vector4::ONE * radius);
  style.SetFrameCornerRadiusPolicy(Dali::Ui::CornerRadiusPolicy::ABSOLUTE);

  const WindowFrameDecorationResult result = mWindowFrame.SetDecoration(mDecoration, mLayout, style, WindowFrameSizePolicy::KEEP_WINDOW_SIZE);
  if(result != WindowFrameDecorationResult::INSTALLED)
  {
    return result;
  }

  // Each decoration slot paints itself, and the outer corners are rounded on
  // the slots that own them, because the transparent frame no longer supplies
  // the rounded shape behind them. UpdateStateDependentVisuals() sets the radius, since it
  // has to follow the maximized state.
  if(mDecoration.GetBottomSlot())
  {
    mDecoration.GetBottomSlot().SetBackgroundColor(background);
    mDecoration.GetBottomSlot().SetCornerRadiusPolicy(Dali::Ui::CornerRadiusPolicy::ABSOLUTE);
  }
  if(mDecoration.GetTopSlot())
  {
    mDecoration.GetTopSlot().SetBackgroundColor(background);
    mDecoration.GetTopSlot().SetCornerRadiusPolicy(Dali::Ui::CornerRadiusPolicy::ABSOLUTE);
  }
  if(mDecoration.GetLeftSlot())
  {
    mDecoration.GetLeftSlot().SetBackgroundColor(background);
  }
  if(mDecoration.GetRightSlot())
  {
    mDecoration.GetRightSlot().SetBackgroundColor(background);
  }

  mWindowFrame.WindowStateChangedSignal().Connect(this, &DefaultWindowDecorationImpl::OnWindowStateChanged);
  UpdateStateDependentVisuals();
  return result;
}

void DefaultWindowDecorationImpl::UpdateStateDependentVisuals()
{
  if(!mWindowFrame)
  {
    return;
  }

  const bool maximized = mWindowFrame.IsMaximized();

  if(mMaximizeRestoreIcon)
  {
    const Dali::String url = GetBuiltInIconUrl(maximized ? "restore.png" : "maximize.png");
    if(mMaximizeRestoreIcon.GetResourceUrl() != url)
    {
      mMaximizeRestoreIcon.SetResourceUrl(url);
    }
  }

  // A maximized window is square. The decoration owns the outer corners here,
  // so squaring only the frame root would leave rounded notches cut out of a
  // maximized frame.
  const float radius = maximized ? 0.0f : mOptions.GetCornerRadius();
  if(mDecoration.GetBottomSlot())
  {
    mDecoration.GetBottomSlot().SetCornerRadius(Dali::Vector4(0.0f, 0.0f, radius, radius));
  }
  if(mDecoration.GetTopSlot())
  {
    mDecoration.GetTopSlot().SetCornerRadius(Dali::Vector4(radius, radius, 0.0f, 0.0f));
  }
}

void DefaultWindowDecorationImpl::OnWindowStateChanged(WindowFrame /*sender*/, WindowFrame::WindowState /*state*/)
{
  UpdateStateDependentVisuals();
}

Dali::Vector2 DefaultWindowDecorationImpl::GetMinimumFrameSize() const
{
  const float barHeight   = mOptions.GetBarHeight();
  const float handleWidth = mOptions.IsResizeHandlesEnabled() ? barHeight : 0.0f;

  float width = 2.0f * handleWidth + MINIMUM_MOVE_REGION_WIDTH;
  width += static_cast<float>(mBarActions.size()) * CONTROL_WIDTH;
  width += mOptions.IsMinimizeControlEnabled() ? CONTROL_WIDTH : 0.0f;
  width += mOptions.IsMaximizeRestoreControlEnabled() ? CONTROL_WIDTH : 0.0f;
  width += mOptions.IsCloseControlEnabled() ? CONTROL_WIDTH : 0.0f;

  return Dali::Vector2(width, barHeight + GetEffectiveTopHeight());
}

WindowFrameDecoration DefaultWindowDecorationImpl::GetDecoration() const
{
  return mDecoration;
}

Dali::Ui::View DefaultWindowDecorationImpl::GetTopArea() const
{
  return mTopArea;
}

Dali::Ui::View DefaultWindowDecorationImpl::GetMoveRegion() const
{
  return mMoveRegion;
}

} // namespace Internal
} // namespace Dali::Ui
