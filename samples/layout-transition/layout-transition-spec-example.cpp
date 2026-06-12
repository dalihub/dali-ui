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
 * LayoutTransition spec-mode sample.
 *
 * A vertical StackLayout owns a LayoutTransition with declarative ENTER /
 * EXIT / CHANGE specs sharing a single 0.4s EASE_IN_OUT_SINE timing.
 * ENTER expands height 0 → child height and fades opacity 0 → 1; EXIT
 * shrinks height back to 0 and fades opacity to 0; CHANGE reflows
 * existing children when their requested height changes.
 *
 *   - Tap "Click to ENTER": append a new colored child.
 *   - Tap "Click to EXIT": remove the last child.
 *   - Tap "Click to CHANGE": toggle every child's requested
 *     height between 80 and 160.
 *   - Up / Down keys: same as ENTER / EXIT buttons.
 *   - Esc / Back: quit.
 */
class LayoutTransitionSpecController : public ConnectionTracker
{
public:
  explicit LayoutTransitionSpecController(Application& application)
  : mApplication(application),
    mNextColorIndex(0),
    mExpanded(false)
  {
    mApplication.InitSignal().Connect(this, &LayoutTransitionSpecController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Three Clickable labels, one per LayoutTransition slot.
    mEnterButton = MakeClickableLabel("Click to ENTER");
    mEnterButton.TouchedSignal().Connect(this, &LayoutTransitionSpecController::OnEnterTouched);

    mExitButton = MakeClickableLabel("Click to EXIT");
    mExitButton.TouchedSignal().Connect(this, &LayoutTransitionSpecController::OnExitTouched);

    mChangeButton = MakeClickableLabel("Click to CHANGE");
    mChangeButton.TouchedSignal().Connect(this, &LayoutTransitionSpecController::OnChangeTouched);

    // Root: vertical stack with a LayoutTransition attached.
    mStack = StackLayout::New();
    mStack.SetRequestedWidth(MATCH_PARENT);
    mStack.SetRequestedHeight(MATCH_PARENT);
    mStack.SetSpacing(10.0f);

    // Single timing shared by ENTER / EXIT / CHANGE: 0.4s EASE_IN_OUT_SINE.
    LayoutTransitionTiming timing{Duration(0.4f),
                                  AlphaFunction(AlphaFunction::EASE_IN_OUT_SINE),
                                  Duration()};

    // ENTER: opacity 0 → 1. Spec mode declares only the target; dali-core
    // captures the actor's current opacity as the start when Play() runs
    // at ENTER dispatch. AppendChild pre-sets OPACITY = 0 before
    // mStack.Add() so the spec animates the actor from 0 to 1.
    ViewAnimationSpec enterSpec = ViewAnimationSpec::New();
    enterSpec.Opacity(1.0f, Duration(0.4f), AlphaFunction(AlphaFunction::EASE_IN_OUT_SINE));

    // EXIT: opacity 1 → 0.
    ViewAnimationSpec exitSpec = ViewAnimationSpec::New();
    exitSpec.Opacity(0.0f, Duration(0.4f), AlphaFunction(AlphaFunction::EASE_IN_OUT_SINE));

    // ENTER / EXIT bounds: ExpandFrom collapses height to 0 anchored at
    // the TOP edge and grows down to the layout-applied size. ShrinkTo
    // is the mirror for EXIT (full → 0 collapsed at TOP).
    LayoutTransition transition = LayoutTransition::New();
    transition.SetEnterVisualSpec(enterSpec)
              .SetExitVisualSpec(exitSpec)
              .SetEnterBoundsEffect(LayoutBoundsEffects::ExpandFrom(
                LayoutBoundsEdge::TOP, timing))
              .SetExitBoundsEffect(LayoutBoundsEffects::ShrinkTo(
                LayoutBoundsEdge::TOP, timing))
              .SetChangeTiming(timing);

    mStack.SetLayoutTransition(transition);

    // Seed with three children so the user can immediately watch CHANGE.
    // The framework default suppresses ENTER for children present at the
    // parent's first arrange pass, but the declarative ENTER spec
    // (Opacity 1.0) is still settled; the children that pre-set
    // OPACITY = 0 in AppendChild land at opacity 1.0 on the first frame
    // without animation. Runtime adds via the ENTER button fade in normally.
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

    // Outer container: button row above the transition stack. The outer
    // view has no LayoutTransition so toggling the bar's hit does not
    // loop animations on itself.
    StackLayout root = StackLayout::New();
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.Add(buttonRow);
    root.Add(mStack);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &LayoutTransitionSpecController::OnKeyEvent);
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
    // Drive the spec-mode ENTER fade-in by pre-setting opacity to 0 BEFORE
    // attaching the child to the stack. The ENTER spec then animates the
    // actor's OPACITY property up to its declared target (1.0). dali-core
    // captures this 0 as the AnimateTo's starting value at Play() time, so
    // the resulting on-screen effect is opacity 0 → 1 over the slot timing.
    child.SetProperty(Actor::Property::OPACITY, 0.0f);
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
    View last = mStack.GetChildAt(count - 1);
    // Use View::Remove so the EXIT slot is honoured (deferred-remove).
    mStack.Remove(last, RemovePolicy::ANIMATE_EXIT);
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
  Application                    application = Application::New(&argc, &argv);
  LayoutTransitionSpecController controller(application);
  application.MainLoop();
  return 0;
}
