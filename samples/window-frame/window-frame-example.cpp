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
#include <dali-ui-components/public-api/window/window-frame.h>

#include <dali-ui-foundation/dali-ui-foundation.h>

#include <algorithm>
#include <functional>
#include <sstream>
#include <string>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float BORDER_CORNER_RADIUS = 14.0f;

// What this application's own content needs. The installed frame can require
// more, so ApplyFrameMinimumSize() takes the larger of the two.
constexpr float APP_MINIMUM_FRAME_WIDTH  = 200.0f;
constexpr float APP_MINIMUM_FRAME_HEIGHT = 130.0f;

// Fixed columns of the custom bottom bar. Each is reserved whatever the frame
// width is, so they decide how narrow the custom frame can get.
constexpr float CUSTOM_RESIZE_HANDLE_SIZE = 36.0f;
constexpr float CUSTOM_CONTROL_WIDTH      = 40.0f;
constexpr float CUSTOM_CONTROL_COUNT      = 3.0f;
constexpr float CUSTOM_MOVE_REGION_MIN    = 40.0f;
constexpr float CUSTOM_BAR_HEIGHT_MAX     = 68.0f;
constexpr float CUSTOM_FOOTER_HEIGHT_MAX  = 58.0f;

Dali::String ResourceUrl(const char* resourceName)
{
  return Dali::String(RESOURCES_DIR) + resourceName;
}

Label MakeLabel(const std::string& text, float fontSize, const UiColor& color)
{
  Label label = Label::New(Dali::String(text.c_str()));
  label.SetFontSize(fontSize);
  label.SetTextColor(color);
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  return label;
}

ImageView MakeResizeHandle(const char* resourceName)
{
  return ImageView::New(ResourceUrl(resourceName));
}

const char* CommandName(WindowFrameCommand command)
{
  switch(command)
  {
    case WindowFrameCommand::MOVE:
      return "move";
    case WindowFrameCommand::RESIZE:
      return "resize";
    case WindowFrameCommand::MINIMIZE:
      return "minimize";
    case WindowFrameCommand::MAXIMIZE:
      return "maximize";
    case WindowFrameCommand::RESTORE:
      return "restore";
    case WindowFrameCommand::CLOSE:
      return "close";
  }
  return "unknown";
}

const char* WindowStateName(WindowFrame::WindowState state)
{
  switch(state)
  {
    case WindowFrame::WindowState::NORMAL:
      return "normal";
    case WindowFrame::WindowState::MAXIMIZED:
      return "maximized";
    case WindowFrame::WindowState::MINIMIZED:
      return "minimized";
  }
  return "unknown";
}

WindowFrameInteractiveResizePolicy NextResizePolicy(WindowFrameInteractiveResizePolicy policy)
{
  switch(policy)
  {
    case WindowFrameInteractiveResizePolicy::FREE:
      return WindowFrameInteractiveResizePolicy::KEEP_ASPECT_RATIO;
    case WindowFrameInteractiveResizePolicy::KEEP_ASPECT_RATIO:
      return WindowFrameInteractiveResizePolicy::DISABLED;
    case WindowFrameInteractiveResizePolicy::DISABLED:
      return WindowFrameInteractiveResizePolicy::FREE;
  }
  return WindowFrameInteractiveResizePolicy::FREE;
}

const char* ResizePolicyName(WindowFrameInteractiveResizePolicy policy)
{
  switch(policy)
  {
    case WindowFrameInteractiveResizePolicy::FREE:
      return "resize free";
    case WindowFrameInteractiveResizePolicy::KEEP_ASPECT_RATIO:
      return "resize keep ratio";
    case WindowFrameInteractiveResizePolicy::DISABLED:
      return "resize disabled";
  }
  return "resize unknown";
}

const char* DecorationStyleName(uint32_t style)
{
  switch(style)
  {
    case 0u:
      return "default decoration";
    case 1u:
      return "custom 1";
    default:
      return "custom 2";
  }
}

const char* FrameInstallResultName(WindowFrameDecorationResult result)
{
  switch(result)
  {
    case WindowFrameDecorationResult::INSTALLED:
      return "installed";
    case WindowFrameDecorationResult::SLOT_ALREADY_PARENTED:
      return "slot-already-parented";
    case WindowFrameDecorationResult::DUPLICATE_SLOT:
      return "duplicate-slot";
    case WindowFrameDecorationResult::DUPLICATE_ROLE:
      return "duplicate-role";
    case WindowFrameDecorationResult::ROLE_OUTSIDE_DECORATION:
      return "role-outside-decoration";
  }
  return "unknown";
}
} // unnamed namespace

class WindowFrameExample : public ConnectionTracker
{
public:
  explicit WindowFrameExample(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &WindowFrameExample::OnInit);
  }

  virtual ~WindowFrameExample() = default;

protected:
  virtual WindowFrameCommandDisposition OnCommand(WindowFrame /*sender*/, const WindowFrameCommandRequest& /*request*/)
  {
    return WindowFrameCommandDisposition::CONTINUE_DEFAULT;
  }

  virtual void OnStatePresented(WindowFrame::WindowState state)
  {
    UpdateStatus(std::string("presented-") + WindowStateName(state));
  }

private:
  void OnInit(Application application)
  {
    mWindow = application.GetWindow();
    mWindow.SetBackgroundColor(Color::TRANSPARENT);
    mWindow.SetTransparency(true);
    mWindow.SetPositionSize(PositionSize(80, 80, 400, 260));
    mWindow.KeyEventSignal().Connect(this, &WindowFrameExample::OnKeyEvent);

    WindowFrameOptions options;
    options.SetInitialRestoreFrameSize(Vector2(400.0f, 260.0f));
    mWindowFrame = WindowFrame::New(mWindow, WindowFrame::CloseCallback::New(this, &WindowFrameExample::OnCloseRequested), options);
    mWindowFrame.WindowStateChangedSignal().Connect(this, [this](WindowFrame, WindowFrame::WindowState state)
    { UpdateStatus(std::string("native-") + WindowStateName(state)); });
    mWindowFrame.WindowStatePresentedSignal().Connect(this, [this](WindowFrame, WindowFrame::WindowState state)
    { OnStatePresented(state); });
    mWindow.FocusChangedSignal().Connect(this, [this](Window, bool activated)
    {
      mWindowActivated = activated;
      UpdateStatus(activated ? "activated" : "deactivated");
    });
    mWindowFrame.GeometryChangedSignal().Connect(this, [this](WindowFrame, const WindowFrameGeometry& geometry)
    {
      static_cast<void>(geometry);
      UpdateStatus("geometry-changed");
    });
    mWindowFrame.GeometryChangeCompletedSignal().Connect(this, [this](WindowFrame, const WindowFrameGeometry& geometry)
    {
      static_cast<void>(geometry);
      UpdateStatus("geometry-completed");
    });
    mWindowFrame.DecorationVisibilityChangedSignal().Connect(this, [this](WindowFrame, bool visible)
    {
      mOverlayVisible = visible;
      UpdateStatus("decoration-visibility-changed");
    });
    mWindowFrame.SetCommandInterceptor(WindowFrame::CommandInterceptor::New(this, &WindowFrameExample::OnCommand));
    mWindowFrame.CommandProcessedSignal().Connect(this, [this](WindowFrame, const WindowFrameCommandRequest& request, WindowFrameCommandResult /*result*/)
    {
      mLastCommand = CommandName(request.GetCommand());
      UpdateStatus("command-result");
    });
    InstallFrame(WindowFrameSizePolicy::KEEP_WINDOW_SIZE);
    mWindowFrame.RequestFrameResize(Vector2(400.0f, 260.0f));
    mWindowFrame.Attach();
    mWindowFrame.SetMaximumFrameSize(Vector2(1400.0f, 900.0f));
    ApplyFrameMinimumSize();

    CreateContent();
    UpdateStatus("ready");
  }

  void OnCloseRequested()
  {
    mApplication.Quit();
  }

  void CreateContent()
  {
    AbsoluteLayout content = AbsoluteLayout::New();
    content.SetBackgroundColor(UiColor(0xF3F5F7));
    content.SetLayoutParams(AbsoluteLayoutParams::New()
                              .SetBounds(LayoutRect(0.0f, 0.0f, 1.0f, 1.0f))
                              .SetFlags(AbsoluteLayoutFlags::SIZE_PROPORTIONAL));

    mContentHeading = MakeLabel("WindowFrame", 26.0f, UiColor(0x17212B));
    mContentHeading.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(24.0f, 16.0f, 320.0f, 38.0f)));
    content.Add(mContentHeading);

    mContentDescription = MakeLabel("Application content root", 14.0f, UiColor(0x52606D));
    mContentDescription.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(24.0f, 52.0f, 320.0f, 28.0f)));
    content.Add(mContentDescription);

    mContentAccent = View::New();
    mContentAccent.SetBackgroundColor(UiColor(0x00A878));
    mContentAccent.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(24.0f, 88.0f, 120.0f, 5.0f)));
    content.Add(mContentAccent);

    mContentStatus = MakeLabel("", 13.0f, UiColor(0x263238));
    mContentStatus.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(24.0f, 100.0f, 340.0f, 28.0f)));
    content.Add(mContentStatus);

    // Application content drives the frame: the border is replaced from here,
    // not from a key, so the whole flow stays inside the content root.
    mStyleButton = MakeButton(std::string("Style: ") + DecorationStyleName(mDecorationStyle),
                              UiColor(0x344451),
                              [this]
    {
      CycleDecorationStyle();
    },
                              &mStyleButtonLabel);
    mStyleButton.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(24.0f, 136.0f, 200.0f, 36.0f)));
    content.Add(mStyleButton);

    mContentRoot = content;
    mWindowFrame.GetContentRoot().Add(content);

    // The application side of the seam: it supplies an icon and what to do, and
    // the wrapper decides where the button goes. Toggling the content theme is
    // the application's own business, not a window operation, so it is not a
    // command and does not go through SetCommandInterceptor().
    AddBarAction("theme.png", [this]
    {
      ToggleContentTheme();
    });
  }

  void ToggleContentTheme()
  {
    mContentDark = !mContentDark;
    if(mContentRoot)
    {
      mContentRoot.SetBackgroundColor(mContentDark ? UiColor(0x1B242B) : UiColor(0xF3F5F7));
    }
    if(mContentHeading)
    {
      mContentHeading.SetTextColor(mContentDark ? UiColor(0xF3F5F7) : UiColor(0x17212B));
    }
    if(mContentDescription)
    {
      mContentDescription.SetTextColor(mContentDark ? UiColor(0xB8C4CE) : UiColor(0x52606D));
    }
    if(mContentStatus)
    {
      mContentStatus.SetTextColor(mContentDark ? UiColor(0xD9E2EC) : UiColor(0x263238));
    }
    UpdateStatus(mContentDark ? "content-theme-dark" : "content-theme-light");
  }

  GridLayout MakeButton(const std::string& text, const UiColor& background, std::function<void()> callback, Label* labelHandle = nullptr)
  {
    GridLayout button = GridLayout::New();
    button.SetBackgroundColor(background);
    button.SetCornerRadius(8.0f);
    button.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);
    button.AsInteractive().ConnectClickedSignal(this, [callback = std::move(callback)](View /*view*/, InputEvent /*event*/) -> bool
    {
      callback();
      return true;
    });

    Label label = MakeLabel(text, 12.0f, UiColor(0xFFFFFF));
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetLayoutParams(GridLayoutParams::New());
    button.Add(label);
    if(labelHandle)
    {
      *labelHandle = label;
    }
    return button;
  }

  GridLayout MakeIconButton(const char* resourceName, const char* controlName, const UiColor& background, std::function<void()> callback = {}, ImageView* iconHandle = nullptr)
  {
    GridLayout hitTarget = GridLayout::New();
    hitTarget.SetProperty(Actor::Property::NAME, controlName);
    if(callback)
    {
      hitTarget.AsInteractive().ConnectClickedSignal(this, [callback = std::move(callback)](View /*view*/, InputEvent /*event*/) -> bool
      {
        callback();
        return true;
      });
    }

    GridLayout buttonVisual = GridLayout::New();
    buttonVisual.SetBackgroundColor(background);
    buttonVisual.SetCornerRadius(8.0f);
    buttonVisual.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);
    buttonVisual.SetRequestedWidth(34.0f);
    buttonVisual.SetRequestedHeight(34.0f);
    buttonVisual.SetSensitive(false);
    buttonVisual.SetLayoutParams(GridLayoutParams::New()
                                   .SetHorizontalAlignment(LayoutAlignment::CENTER)
                                   .SetVerticalAlignment(LayoutAlignment::CENTER));

    ImageView icon = ImageView::New(ResourceUrl(resourceName));
    icon.SetRequestedWidth(24.0f);
    icon.SetRequestedHeight(24.0f);
    icon.SetLayoutParams(GridLayoutParams::New()
                           .SetHorizontalAlignment(LayoutAlignment::CENTER)
                           .SetVerticalAlignment(LayoutAlignment::CENTER));
    buttonVisual.Add(icon);
    hitTarget.Add(buttonVisual);
    if(iconHandle)
    {
      *iconHandle = icon;
    }
    return hitTarget;
  }

  WindowFrameDecoration BuildDecoration(WindowFrameLayout& layout)
  {
    const bool alternate = mDecorationStyle == 2u;
    layout.SetDecorationInsets(alternate ? Insets(10.0f, 10.0f, 68.0f, 58.0f) : Insets(8.0f, 8.0f, 64.0f, 56.0f));
    layout.SetBottomLeftResizeHandleSize(Vector2(36.0f, 36.0f));
    layout.SetBottomRightResizeHandleSize(Vector2(36.0f, 36.0f));

    const UiColor topColor       = alternate ? UiColor(0x263238) : UiColor(0x17212B);
    const UiColor bottomColor    = alternate ? UiColor(0x334E68) : UiColor(0x202B33);
    const UiColor sideColor      = alternate ? UiColor(0xE05A47) : UiColor(0x00A878);
    const UiColor actionColor    = alternate ? UiColor(0x486581) : UiColor(0x344451);
    const UiColor closeColor     = UiColor(0xC43D3D);
    const UiColor secondaryColor = alternate ? UiColor(0xD9E2EC) : UiColor(0xB8C4CE);

    GridLayout top = GridLayout::New();
    top.SetBackgroundColor(topColor);
    top.SetCornerRadius(Vector4(BORDER_CORNER_RADIUS, BORDER_CORNER_RADIUS, 0.0f, 0.0f));
    top.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);
    top.AddRowDefinition(GridLength::Star());
    top.AddColumnDefinition(GridLength::Star());
    top.AddColumnDefinition(GridLength::Absolute(54.0f));
    top.AddColumnDefinition(GridLength::Absolute(64.0f));
    top.AddColumnDefinition(GridLength::Absolute(58.0f));
    top.AddColumnDefinition(GridLength::Absolute(8.0f));

    GridLayout titleArea = GridLayout::New();
    titleArea.AddRowDefinition(GridLength::Star());
    titleArea.AddColumnDefinition(GridLength::Absolute(8.0f));
    titleArea.AddColumnDefinition(GridLength::Star());
    titleArea.SetLayoutParams(GridLayoutParams::New().SetRow(0u).SetColumn(0u));
    mMoveRegion      = titleArea;
    View titleAccent = View::New();
    titleAccent.SetBackgroundColor(sideColor);
    titleAccent.SetCornerRadius(4.0f);
    titleAccent.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);
    titleAccent.SetMargin(Extents(12u, 0u, 14u, 14u));
    titleAccent.SetLayoutParams(GridLayoutParams::New().SetRow(0u).SetColumn(0u));
    titleArea.Add(titleAccent);
    Label title = MakeLabel(alternate ? "Studio Frame" : "WindowFrame", 19.0f, UiColor(0xFFFFFF));
    title.SetMargin(Extents(12u, 8u, 0u, 0u));
    title.SetLayoutParams(GridLayoutParams::New().SetRow(0u).SetColumn(1u));
    titleArea.Add(title);
    top.Add(titleArea);

    AddTopButton(top, 1u, "Style", actionColor, [this]
    {
      CycleDecorationStyle();
    });
    AddTopButton(top, 2u, "Overlay", actionColor, [this]
    {
      mWindowFrame.SetOverlayEnabled(!mWindowFrame.IsOverlayEnabled());
    });
    AddTopButton(top, 3u, "Resize", actionColor, [this]
    {
      mWindowFrame.SetInteractiveResizePolicy(NextResizePolicy(mWindowFrame.GetInteractiveResizePolicy()));
    });

    // Column order: resize handle, move region, application actions, window
    // controls, resize handle. The action buttons are siblings of the move
    // region rather than children of it, which is what keeps them clickable
    // while maximized. See AddBarAction().
    GridLayout bottom = GridLayout::New();
    bottom.SetBackgroundColor(bottomColor);
    bottom.SetCornerRadius(Vector4(0.0f, 0.0f, BORDER_CORNER_RADIUS, BORDER_CORNER_RADIUS));
    bottom.SetCornerRadiusPolicy(CornerRadiusPolicy::ABSOLUTE);
    bottom.AddRowDefinition(GridLength::Star());
    bottom.AddColumnDefinition(GridLength::Absolute(layout.GetBottomLeftResizeHandleSize().width));
    bottom.AddColumnDefinition(GridLength::Star());
    for(std::size_t action = 0u; action < mBarActions.size(); ++action)
    {
      bottom.AddColumnDefinition(GridLength::Absolute(CUSTOM_CONTROL_WIDTH));
    }
    bottom.AddColumnDefinition(GridLength::Absolute(CUSTOM_CONTROL_WIDTH));
    bottom.AddColumnDefinition(GridLength::Absolute(CUSTOM_CONTROL_WIDTH));
    bottom.AddColumnDefinition(GridLength::Absolute(CUSTOM_CONTROL_WIDTH));
    bottom.AddColumnDefinition(GridLength::Absolute(layout.GetBottomRightResizeHandleSize().width));

    GridLayout moveRegion = GridLayout::New();
    moveRegion.SetBackgroundColor(bottomColor);
    moveRegion.SetLayoutParams(GridLayoutParams::New().SetRow(0u).SetColumn(1u));
    mFooterStatus = MakeLabel("", 13.0f, secondaryColor);
    mFooterStatus.SetMargin(Extents(14u, 10u, 0u, 0u));
    moveRegion.Add(mFooterStatus);
    bottom.Add(moveRegion);

    uint32_t column = 2u;
    for(const BarAction& action : mBarActions)
    {
      GridLayout actionButton = MakeIconButton(action.iconResource.c_str(), "ApplicationAction", sideColor, action.callback);
      actionButton.SetLayoutParams(GridLayoutParams::New().SetRow(0u).SetColumn(column++));
      bottom.Add(actionButton);
    }

    GridLayout minimizeButton = MakeIconButton("minimize.png", "MinimizeControl", actionColor);
    minimizeButton.SetLayoutParams(GridLayoutParams::New()
                                     .SetRow(0u)
                                     .SetColumn(column++));
    bottom.Add(minimizeButton);

    mMaximizeRestoreControl.Reset();
    mMaximizeIcon.Reset();
    GridLayout maximizeButton = MakeIconButton("maximize.png", "MaximizeRestoreControl", actionColor, {}, &mMaximizeIcon);
    maximizeButton.SetLayoutParams(GridLayoutParams::New()
                                     .SetRow(0u)
                                     .SetColumn(column++));
    bottom.Add(maximizeButton);
    mMaximizeRestoreControl = maximizeButton;

    GridLayout closeButton = MakeIconButton("close.png", "CloseControl", closeColor);
    closeButton.SetLayoutParams(GridLayoutParams::New()
                                  .SetRow(0u)
                                  .SetColumn(column++));
    bottom.Add(closeButton);

    View left = View::New();
    left.SetBackgroundColor(sideColor);
    View right = View::New();
    right.SetBackgroundColor(sideColor);

    WindowFrameDecoration decoration;
    decoration.SetTopSlot(top);
    decoration.SetBottomSlot(bottom);
    decoration.SetLeftSlot(left);
    decoration.SetRightSlot(right);
    decoration.SetMoveRegion(moveRegion);
    decoration.SetMinimizeControl(minimizeButton);
    decoration.SetMaximizeRestoreControl(maximizeButton);
    decoration.SetCloseControl(closeButton);
    decoration.SetBottomLeftResizeHandle(MakeResizeHandle("resize-bottom-left.png"));
    decoration.SetBottomRightResizeHandle(MakeResizeHandle("resize-bottom-right.png"));
    return decoration;
  }

  /**
   * @brief Adds an application action to the bottom bar, before the built-in
   *        minimize, maximize/restore, and close buttons.
   *
   * This is the seam a product wrapper offers its applications. WindowFrame has
   * no GetDecoration(), so an application holding only the window handle cannot
   * reach the installed actors; whoever builds the decoration has to hand out an
   * entry point like this one.
   *
   * The button gets its own column beside the move region rather than being
   * added inside it. WindowFrame makes the move region insensitive while the
   * window is maximized, and an insensitive actor hides its children from hit
   * testing, so a button parented to the move region - or to
   * DefaultWindowDecoration::GetMoveRegion(), which is the same actor - stops responding as
   * soon as the window is maximized.
   *
   * Actions are stored rather than added to the live frame, because the frame is
   * rebuilt whenever the style changes and every rebuild has to reproduce them.
   *
   * @param[in] iconResource Icon file name in this sample's res/ directory
   * @param[in] callback Invoked when the button is clicked
   */
  void AddBarAction(const char* iconResource, std::function<void()> callback)
  {
    mBarActions.push_back(BarAction{iconResource, std::move(callback)});

    // This showcase stores actions in its custom-frame model, so an action only
    // appears on those frames. DefaultWindowDecoration users can add one directly through
    // DefaultWindowDecoration::AddBarAction(). Reinstalling is useful only when a custom
    // frame is active; otherwise the next BuildDecoration() picks up the stored
    // action. Keeping the window size means the content absorbs the new column.
    if(mWindowFrame && !mDefaultDecoration)
    {
      InstallFrame(WindowFrameSizePolicy::KEEP_WINDOW_SIZE);
    }
  }

  /**
   * @brief Returns the smallest frame the custom decoration lays out correctly.
   *
   * Same reasoning as DefaultWindowDecoration::GetMinimumFrameSize(): the bar reserves an
   * absolute column per control, per action and per corner handle, so below
   * their total the move region collapses and the last control slides under the
   * corner handle.
   */
  Vector2 CustomFrameMinimumSize() const
  {
    const float width = 2.0f * CUSTOM_RESIZE_HANDLE_SIZE +
                        (CUSTOM_CONTROL_COUNT + static_cast<float>(mBarActions.size())) * CUSTOM_CONTROL_WIDTH +
                        CUSTOM_MOVE_REGION_MIN;
    return Vector2(width, CUSTOM_BAR_HEIGHT_MAX + CUSTOM_FOOTER_HEIGHT_MAX);
  }

  void AddTopButton(GridLayout top, uint32_t column, const std::string& text, const UiColor& color, std::function<void()> callback)
  {
    GridLayout button = MakeButton(text, color, std::move(callback));
    button.SetMargin(Extents(2u, 2u, 8u, 8u));
    button.SetLayoutParams(GridLayoutParams::New().SetRow(0u).SetColumn(column));
    top.Add(button);
  }

  WindowFrameStyle BuildFrameStyle() const
  {
    const bool       alternate = mDecorationStyle == 2u;
    WindowFrameStyle style;
    style.SetShadowOutsets(alternate ? Insets(18.0f, 18.0f, 18.0f, 22.0f) : Insets(14.0f, 14.0f, 14.0f, 18.0f));
    style.SetShadow(Shadow(alternate ? 18.0f : 14.0f,
                           Vector2(0.0f, alternate ? 4.0f : 3.0f),
                           UiColor(0x000000, alternate ? 0.56f : 0.48f),
                           Vector2::ZERO,
                           CutoutPolicy::CUTOUT_VIEW_WITH_CORNER_RADIUS));
    style.SetFrameBackgroundColor(alternate ? UiColor(0x263238) : UiColor(0x17212B));
    style.SetFrameCornerRadius(Vector4(BORDER_CORNER_RADIUS, BORDER_CORNER_RADIUS, BORDER_CORNER_RADIUS, BORDER_CORNER_RADIUS));
    return style;
  }

  void InstallDefaultDecoration()
  {
    // The complete decoration in one call. The returned handle keeps the maximize and
    // restore icon in step with the window, so it has to stay alive.
    mDefaultDecoration = DefaultWindowDecoration::New(mWindowFrame);
    if(!mDefaultDecoration)
    {
      UpdateStatus("default-window-decoration-rejected");
      return;
    }

    // Application content goes into the free stretch of the bar, which also
    // starts a window move. The label still lets the drag through because it
    // is not interactive.
    Label title = MakeLabel("WindowFrame", 15.0f, UiColor(0x1F1F1F));
    title.SetMargin(Extents(14u, 0u, 0u, 0u));
    mDefaultDecoration.GetMoveRegion().Add(title);

    // The custom-frame handles below are not part of this frame.
    mMoveRegion.Reset();
    mFooterStatus.Reset();
    mMaximizeRestoreControl.Reset();
    mMaximizeIcon.Reset();
    UpdateStatus("default-window-decoration");
  }

  /**
   * @brief Applies a minimum the installed frame can actually lay out.
   *
   * Each frame has its own floor, so the minimum is set after installing rather
   * than once at startup. DefaultWindowDecoration reports its own through
   * GetMinimumFrameSize(); the custom frame below computes its own the same way,
   * and both grow past what the application content alone would need.
   */
  void ApplyFrameMinimumSize()
  {
    if(!mWindowFrame)
    {
      return;
    }
    Vector2       minimum(APP_MINIMUM_FRAME_WIDTH, APP_MINIMUM_FRAME_HEIGHT);
    const Vector2 required = mDefaultDecoration ? mDefaultDecoration.GetMinimumFrameSize() : CustomFrameMinimumSize();
    minimum.width          = std::max(minimum.width, required.width);
    minimum.height         = std::max(minimum.height, required.height);
    mWindowFrame.SetMinimumFrameSize(minimum);
  }

  void InstallFrame(WindowFrameSizePolicy sizePolicy = WindowFrameSizePolicy::KEEP_CONTENT_SIZE)
  {
    if(mDecorationStyle == 0u)
    {
      InstallDefaultDecoration();
      ApplyFrameMinimumSize();
      return;
    }

    // Releasing the handle stops the default decoration tracking the window before
    // its actors are replaced.
    mDefaultDecoration.Reset();
    WindowFrameLayout                 layout;
    WindowFrameDecoration             decoration = BuildDecoration(layout);
    const WindowFrameDecorationResult result     = mWindowFrame.SetDecoration(decoration, layout, BuildFrameStyle(), sizePolicy);
    ApplyFrameMinimumSize();
    UpdateStatus(std::string("frame-") + FrameInstallResultName(result));
  }

  void CycleDecorationStyle()
  {
    mDecorationStyle = (mDecorationStyle + 1u) % 3u;
    InstallFrame();
  }

  void CycleWindowFrameLayoutResult()
  {
    // Only the layout configuration changes, so the installed slot actors, their
    // contents and their signal connections all stay alive.
    mCompactLayout = !mCompactLayout;
    WindowFrameLayout layout;
    layout.SetDecorationInsets(mCompactLayout ? Insets(6.0f, 6.0f, 44.0f, 44.0f) : Insets(8.0f, 8.0f, 64.0f, 56.0f));
    layout.SetBottomLeftResizeHandleSize(Vector2(36.0f, 36.0f));
    layout.SetBottomRightResizeHandleSize(Vector2(36.0f, 36.0f));
    mWindowFrame.SetDecorationLayout(layout, WindowFrameSizePolicy::KEEP_WINDOW_SIZE);
    UpdateStatus(mCompactLayout ? "layout-compact" : "layout-normal");
  }

  void UpdateStatus(const std::string& state)
  {
    if(!mWindowFrame)
    {
      return;
    }

    UpdateWindowControlIcons();
    UpdateContentInsets();

    const Vector2 frameSize = mWindowFrame.GetFrameSize();
    if(mMoveRegion)
    {
      mMoveRegion.SetVisible(frameSize.width >= 300.0f);
    }

    const Vector2 contentSize = mWindowFrame.GetContentSize();
    if(mContentHeading) mContentHeading.SetVisible(contentSize.width >= 344.0f && contentSize.height >= 54.0f);
    if(mContentDescription) mContentDescription.SetVisible(contentSize.width >= 344.0f && contentSize.height >= 80.0f);
    if(mContentAccent) mContentAccent.SetVisible(contentSize.width >= 144.0f && contentSize.height >= 93.0f);
    if(mContentStatus) mContentStatus.SetVisible(contentSize.width >= 364.0f && contentSize.height >= 128.0f);
    if(mStyleButton) mStyleButton.SetVisible(contentSize.width >= 224.0f && contentSize.height >= 172.0f);
    if(mStyleButtonLabel) mStyleButtonLabel.SetText(Dali::String((std::string("Style: ") + DecorationStyleName(mDecorationStyle)).c_str()));
    if(mFooterStatus) mFooterStatus.SetVisible(frameSize.width >= 360.0f);

    std::ostringstream text;
    text << "state: " << state << "   content: " << static_cast<int>(contentSize.width) << " x " << static_cast<int>(contentSize.height);
    if(!mLastCommand.empty())
    {
      text << "   command: " << mLastCommand;
    }
    if(mContentStatus) mContentStatus.SetText(Dali::String(text.str().c_str()));

    std::ostringstream footer;
    footer << (mWindowActivated ? "active" : "inactive") << " | obscured " << static_cast<int>(mObscuredTop) << " | "
           << (mWindowFrame.IsOverlayEnabled() ? "overlay" : "inset") << " | "
           << (mOverlayVisible ? "border visible" : "border hidden") << " | "
           << ResizePolicyName(mWindowFrame.GetInteractiveResizePolicy()) << " | "
           << DecorationStyleName(mDecorationStyle);
    if(mFooterStatus) mFooterStatus.SetText(Dali::String(footer.str().c_str()));
  }

  void PlaceContentRow(View view, float x, float baseY, float width, float height, const Insets& obscured)
  {
    if(view)
    {
      view.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(x + obscured.start, baseY + obscured.top, width, height)));
    }
  }

  void UpdateContentInsets()
  {
    // In overlay layout the content keeps the whole frame and decoration is
    // drawn on top of it, so the background deliberately stays full size and
    // extends under the title bar. Only the text that has to stay readable is
    // moved clear of the decoration.
    const Insets obscured = mWindowFrame.GetGeometry().GetDecorationOverlayInsets();
    PlaceContentRow(mContentHeading, 24.0f, 16.0f, 320.0f, 38.0f, obscured);
    PlaceContentRow(mContentDescription, 24.0f, 52.0f, 320.0f, 28.0f, obscured);
    PlaceContentRow(mContentAccent, 24.0f, 88.0f, 120.0f, 5.0f, obscured);
    PlaceContentRow(mContentStatus, 24.0f, 100.0f, 340.0f, 28.0f, obscured);
    PlaceContentRow(mStyleButton, 24.0f, 136.0f, 200.0f, 36.0f, obscured);
    mObscuredTop = obscured.top;
  }

  void UpdateWindowControlIcons()
  {
    if(!mMaximizeRestoreControl)
    {
      return;
    }

    const bool maximized = mWindowFrame.IsMaximized();
    if(mMaximizeIcon)
    {
      mMaximizeIcon.SetResourceUrl(ResourceUrl(maximized ? "restore.png" : "maximize.png"));
    }
  }

  void OnKeyEvent(Window /*window*/, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::DOWN)
    {
      return;
    }

    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
    }
    else if(event.GetKeyName() == "b" || event.GetKeyName() == "B")
    {
      mWindowFrame.SetDecorationVisible(!mWindowFrame.IsDecorationVisible());
    }
    else if(event.GetKeyName() == "c" || event.GetKeyName() == "C")
    {
      CycleWindowFrameLayoutResult();
    }
    else if(event.GetKeyName() == "m" || event.GetKeyName() == "M")
    {
      mWindowFrame.ToggleMaximize();
    }
  }

private:
  /// An application action the wrapper places in the border bar.
  struct BarAction
  {
    std::string           iconResource;
    std::function<void()> callback;
  };

private:
  Application&            mApplication;
  Window                  mWindow;
  WindowFrame             mWindowFrame;
  DefaultWindowDecoration mDefaultDecoration;
  Label                   mContentStatus;
  AbsoluteLayout          mContentRoot;
  Label                   mContentHeading;
  Label                   mContentDescription;
  View                    mContentAccent;
  GridLayout              mStyleButton;
  Label                   mStyleButtonLabel;
  Label                   mFooterStatus;
  View                    mMoveRegion;
  View                    mMaximizeRestoreControl;
  ImageView               mMaximizeIcon;
  std::vector<BarAction>  mBarActions;
  std::string             mLastCommand;
  bool                    mContentDark{false};
  bool                    mOverlayVisible{true};
  bool                    mWindowActivated{true};
  float                   mObscuredTop{0.0f};
  bool                    mCompactLayout{false};
  uint32_t                mDecorationStyle{0u};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig    config      = UiConfig::New();
  config.SetDefaultStateEffectForInteractive(OverlayEffect::Plain());
  config.Apply();
  WindowFrameExample controller(application);
  application.MainLoop();
  return 0;
}
