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

#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali;
using namespace Dali::Ui;

/**
 * LayoutTransition animator-mode sample.
 *
 * The application owns the per-frame interpolation: the framework sweeps
 * progress 0..1 across the supplied timing and invokes our callback every
 * frame. The callback writes SIZE / OPACITY itself, mirroring the
 * spec-mode sample's height-expand + opacity-fade contract but driving
 * the per-frame values from the application instead of a declarative
 * spec. All three slots share a single 0.4s EASE_IN_OUT_SINE timing.
 *
 *   - Tap "Click to ENTER": append a new colored child.
 *   - Tap "Click to EXIT": remove the last child.
 *   - Tap "Click to CHANGE": toggle every child's requested
 *     height between 80 and 160.
 *   - Up / Down keys: same as ENTER / EXIT buttons.
 *   - Esc / Back: quit.
 */
class LayoutTransitionAnimatorController : public ConnectionTracker
{
public:
  explicit LayoutTransitionAnimatorController(Application& application)
  : mApplication(application),
    mNextColorIndex(0),
    mExpanded(false)
  {
    mApplication.InitSignal().Connect(this, &LayoutTransitionAnimatorController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Three Clickable labels, one per LayoutTransition slot.
    mEnterButton = MakeClickableLabel("Click to ENTER");
    mEnterButton.TouchEventSignal().Connect(this, &LayoutTransitionAnimatorController::OnEnterTouched);

    mExitButton = MakeClickableLabel("Click to EXIT");
    mExitButton.TouchEventSignal().Connect(this, &LayoutTransitionAnimatorController::OnExitTouched);

    mChangeButton = MakeClickableLabel("Click to CHANGE");
    mChangeButton.TouchEventSignal().Connect(this, &LayoutTransitionAnimatorController::OnChangeTouched);

    mStack = StackLayout::New();
    mStack.SetRequestedWidth(MATCH_PARENT);
    mStack.SetRequestedHeight(MATCH_PARENT);
    mStack.SetSpacing(10.0f);

    // Single timing shared by ENTER / EXIT / CHANGE: 0.4s EASE_IN_OUT_SINE.
    LayoutAnimatorTiming timing;
    timing.duration = Duration(0.4f);
    timing.alpha    = AlphaFunction(AlphaFunction::EASE_IN_OUT_SINE);

    LayoutTransition transition = LayoutTransition::New();
    transition
      .SetEnterAnimator(LayoutAnimatorCallback::New(&LayoutTransitionAnimatorController::OnEnter), timing)
      .SetExitAnimator(LayoutAnimatorCallback::New(&LayoutTransitionAnimatorController::OnExit), timing)
      .SetChangeAnimator(LayoutAnimatorCallback::New(&LayoutTransitionAnimatorController::OnChange), timing);

    mStack.SetLayoutTransition(transition);

    // Seed with three children. The framework default suppresses ENTER on
    // the parent's first arrange pass; animator-mode ENTER is skipped
    // entirely (no property writes), so these initial children appear at
    // their default OPACITY = 1 and the layout-applied height immediately.
    // Runtime adds via the ENTER button expand + fade in normally.
    AppendChild();
    AppendChild();
    AppendChild();

    // Button row hosts the three Clickables; equal width via FlexGrow 1.
    FlexLayout buttonRow = FlexLayout::New();
    buttonRow.SetRequestedWidth(MATCH_PARENT);
    buttonRow.SetRequestedHeight(60.0f);
    buttonRow.SetDirection(FlexDirection::ROW);
    buttonRow.SetAlignItems(FlexAlign::STRETCH);
    buttonRow.Add(mEnterButton);
    buttonRow.Add(mExitButton);
    buttonRow.Add(mChangeButton);

    StackLayout root = StackLayout::New();
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.Add(buttonRow);
    root.Add(mStack);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &LayoutTransitionAnimatorController::OnKeyEvent);
  }

  Label MakeClickableLabel(const Dali::String& text)
  {
    Label label = Label::New();
    label.SetText(text);
    label.SetTextColor(Color::WHITE);
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    label.SetBackgroundColor(Color::BLACK);
    label.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f));
    // 5px on each side gives a 10px visual gap between adjacent buttons.
    label.SetMargin(Extents(5, 5, 0, 0));
    return label;
  }

  /// ENTER animator: height 0 → toBounds.height anchored at the TOP edge,
  /// combined with opacity 0 → 1. Width and position stay at the
  /// layout-applied values so the child grows down into its allocated slot.
  static void OnEnter(const LayoutAnimatorContext& ctx)
  {
    Actor actor = ctx.view;
    if(!actor)
    {
      return;
    }
    const float p = ctx.progress;
    actor.SetProperty(Actor::Property::OPACITY, p);
    actor.SetProperty(Actor::Property::SIZE_HEIGHT, ctx.toBounds.height * p);
  }

  /// EXIT animator: height fromBounds.height → 0 anchored at the TOP edge,
  /// combined with opacity 1 → 0.
  static void OnExit(const LayoutAnimatorContext& ctx)
  {
    Actor actor = ctx.view;
    if(!actor)
    {
      return;
    }
    const float p = ctx.progress;
    actor.SetProperty(Actor::Property::OPACITY, 1.0f - p);
    actor.SetProperty(Actor::Property::SIZE_HEIGHT, ctx.fromBounds.height * (1.0f - p));
  }

  /// CHANGE animator: linear interpolation of arranged bounds.
  static void OnChange(const LayoutAnimatorContext& ctx)
  {
    Actor actor = ctx.view;
    if(!actor)
    {
      return;
    }
    const float p   = ctx.progress;
    const float x   = ctx.fromBounds.x      + (ctx.toBounds.x - ctx.fromBounds.x) * p;
    const float y   = ctx.fromBounds.y      + (ctx.toBounds.y - ctx.fromBounds.y) * p;
    const float w   = ctx.fromBounds.width  + (ctx.toBounds.width  - ctx.fromBounds.width)  * p;
    const float h   = ctx.fromBounds.height + (ctx.toBounds.height - ctx.fromBounds.height) * p;
    actor.SetProperty(Actor::Property::POSITION_X, x);
    actor.SetProperty(Actor::Property::POSITION_Y, y);
    actor.SetProperty(Actor::Property::SIZE_WIDTH, w);
    actor.SetProperty(Actor::Property::SIZE_HEIGHT, h);
  }

  void AppendChild()
  {
    static constexpr Vector4 palette[] = {
      Color::RED, Color::GREEN, Color::BLUE,
      Color::YELLOW, Color::CYAN, Color::MAGENTA};
    constexpr uint32_t paletteSize = sizeof(palette) / sizeof(palette[0]);

    View child = View::New();
    child.SetBackgroundColor(palette[mNextColorIndex % paletteSize]);
    child.SetRequestedWidth(MATCH_PARENT);
    child.SetRequestedHeight(mExpanded ? 160.0f : 80.0f);
    // Leave OPACITY and SIZE_HEIGHT at their actor / layout defaults.
    // The OnEnter animator drives OPACITY from 0 and SIZE_HEIGHT from 0
    // up to the layout-applied values for runtime adds. On initial mount
    // the dispatcher suppresses animator ENTER entirely, so initial
    // children appear at defaults without animation; pre-setting either
    // here would leave initial-mount children stuck at the pre-set value.
    mStack.Add(child);
    ++mNextColorIndex;
  }

  void RemoveLastChild()
  {
    const uint32_t count = mStack.GetChildCount();
    if(count == 0)
    {
      return;
    }
    mStack.Remove(mStack.GetChildAt(count - 1), RemovePolicy::ANIMATE_EXIT);
  }

  bool OnEnterTouched(Actor /*actor*/, TouchEvent touch)
  {
    if(touch.GetState(0) != PointState::STARTED)
    {
      return false;
    }
    AppendChild();
    return true;
  }

  bool OnExitTouched(Actor /*actor*/, TouchEvent touch)
  {
    if(touch.GetState(0) != PointState::STARTED)
    {
      return false;
    }
    RemoveLastChild();
    return true;
  }

  bool OnChangeTouched(Actor /*actor*/, TouchEvent touch)
  {
    if(touch.GetState(0) != PointState::STARTED)
    {
      return false;
    }
    mExpanded             = !mExpanded;
    const float newHeight = mExpanded ? 160.0f : 80.0f;
    const uint32_t count  = mStack.GetChildCount();
    for(uint32_t i = 0; i < count; ++i)
    {
      mStack.GetChildAt(i).SetRequestedHeight(newHeight);
    }
    return true;
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
    else if(IsKey(event, Dali::DALI_KEY_CURSOR_UP))
    {
      AppendChild();
    }
    else if(IsKey(event, Dali::DALI_KEY_CURSOR_DOWN))
    {
      RemoveLastChild();
    }
  }

private:
  Application& mApplication;
  StackLayout  mStack;
  Label        mEnterButton;
  Label        mExitButton;
  Label        mChangeButton;
  uint32_t     mNextColorIndex;
  bool         mExpanded;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application                        application = Application::New(&argc, &argv);
  LayoutTransitionAnimatorController controller(application);
  application.MainLoop();
  return 0;
}
