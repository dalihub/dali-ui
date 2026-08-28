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
 * LayoutTransition per-view override sample.
 *
 * The root container carries one CHANGE transition (0.25s) that governs all of
 * its children. Each of the three items then declares a different policy for
 * ITSELF, with a self transition or a LayoutTransitionMode:
 *
 *   - item A: no self transition — it inherits the root's 0.25s timing, and the
 *     root's ENTER / EXIT fade likewise drives it when added or removed.
 *   - item B: a slow 0.8s self transition with its own ENTER / EXIT effects, so
 *     it is visibly out of step with A on every layout change and fades /
 *     expands on its own timing when added or removed.
 *   - item C: LayoutTransitionMode::PASS_THROUGH — the explicit opt-out. Layout
 *     transitions pass through C, so it snaps to its new bounds while A and B
 *     animate, and "Remove C" unparents it instantly even though the root has
 *     an EXIT effect.
 *
 * A self transition wins wholesale: once set, it alone governs every slot of
 * that view, and the slots it leaves unconfigured simply do not animate. An
 * uninitialized handle is NOT an opt-out — it detaches and returns the view to
 * the parent's rules, which is what "Toggle B override" demonstrates; the
 * opt-out is the per-view policy View::SetLayoutTransitionMode, which is read
 * before any handle and is what C uses.
 *
 *   - Tap "Toggle layout": flip the spacer + item sizes. A, B and C all move,
 *     each on its own policy.
 *   - Tap "Add/Remove A": the root's ENTER / EXIT drive A — the inherited
 *     0.25s fade the other two are compared against.
 *   - Tap "Add/Remove B": B's own ENTER / EXIT drive it, not the root's.
 *   - Tap "Remove/Add C": C is opted out, so it appears and disappears
 *     instantly while the root's ENTER / EXIT would have animated it.
 *   - Tap "B: override / B: inherit": detach and re-attach B's self transition
 *     to see it fall back to the root's timing and back again.
 *   - Up / Down keys: same as Toggle layout.
 *   - Esc / Back: quit.
 */
class LayoutTransitionSelfOverrideController : public ConnectionTracker
{
public:
  explicit LayoutTransitionSelfOverrideController(Application& application)
  : mApplication(application),
    mExpanded(false),
    mOverrideB(true)
  {
    mApplication.InitSignal().Connect(this, &LayoutTransitionSelfOverrideController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    mToggleButton = MakeClickableLabel("Toggle layout");
    mToggleButton.TouchEventSignal().Connect(this, &LayoutTransitionSelfOverrideController::OnToggleTouched);

    mItemAButton = MakeClickableLabel("Add/Remove A");
    mItemAButton.TouchEventSignal().Connect(this, &LayoutTransitionSelfOverrideController::OnItemATouched);

    mItemBButton = MakeClickableLabel("Add/Remove B");
    mItemBButton.TouchEventSignal().Connect(this, &LayoutTransitionSelfOverrideController::OnItemBTouched);

    mItemCButton = MakeClickableLabel("Remove/Add C");
    mItemCButton.TouchEventSignal().Connect(this, &LayoutTransitionSelfOverrideController::OnItemCTouched);

    mOverrideButton = MakeClickableLabel("B: override");
    mOverrideButton.TouchEventSignal().Connect(this, &LayoutTransitionSelfOverrideController::OnOverrideTouched);

    // Root: one transition governs every direct child unless the child
    // overrides it for itself.
    mRoot = StackLayout::New();
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(MATCH_PARENT);
    mRoot.SetSpacing(10.0f);
    mRoot.SetLayoutTransition(MakeRootTransition());

    // Spacer above the items. Its height toggles, so every item below it
    // moves in one pass -- each on the policy it declared for itself.
    mSpacer = View::New();
    mSpacer.SetBackgroundColor(Color::LIGHT_GRAY);
    mSpacer.SetRequestedWidth(MATCH_PARENT);
    mSpacer.SetRequestedHeight(40.0f);

    // A: inherits the root transition (no self transition attached).
    mItemA = MakeItem(Color::RED);

    // B: overrides with a slow transition of its own, ENTER / EXIT included.
    mItemB = MakeItem(Color::GREEN);
    mItemB.SetSelfLayoutTransition(MakeSlowSelfTransition());

    // C: explicit opt-out. The policy is read before any handle is resolved, so
    // transitions pass through C: it snaps on CHANGE, is unparented instantly on
    // EXIT, and skips ENTER.
    mItemC = MakeItem(Color::BLUE);
    mItemC.SetLayoutTransitionMode(LayoutTransitionMode::PASS_THROUGH);

    mRoot.Add(mSpacer);
    mRoot.Add(mItemA);
    mRoot.Add(mItemB);
    mRoot.Add(mItemC);

    // Button row above the root; no transition so its own layout does not
    // animate.
    FlexLayout buttonRow = FlexLayout::New();
    buttonRow.SetRequestedWidth(MATCH_PARENT);
    buttonRow.SetRequestedHeight(60.0f);
    buttonRow.SetDirection(FlexDirection::ROW);
    buttonRow.SetAlignItems(FlexAlign::STRETCH);
    buttonRow.Add(mToggleButton);
    buttonRow.Add(mItemAButton);
    buttonRow.Add(mItemBButton);
    buttonRow.Add(mItemCButton);
    buttonRow.Add(mOverrideButton);

    StackLayout outer = StackLayout::New();
    outer.SetRequestedWidth(MATCH_PARENT);
    outer.SetRequestedHeight(MATCH_PARENT);
    outer.Add(buttonRow);
    outer.Add(mRoot);

    window.Add(outer);
    window.KeyEventSignal().Connect(this, &LayoutTransitionSelfOverrideController::OnKeyEvent);
  }

  LayoutTransition MakeRootTransition()
  {
    LayoutTransitionTiming timing{Duration(0.25f),
                                  AlphaFunction(AlphaFunction::EASE_IN_OUT_SINE),
                                  Duration()};

    ViewAnimationSpec enterSpec = ViewAnimationSpec::New();
    enterSpec.Opacity(1.0f, Duration(0.25f));
    ViewAnimationSpec exitSpec = ViewAnimationSpec::New();
    exitSpec.Opacity(0.0f, Duration(0.25f));

    LayoutTransition transition = LayoutTransition::New();
    transition.SetChangeTiming(timing)
      .SetEnterVisualSpec(enterSpec)
      .SetExitVisualSpec(exitSpec);
    return transition;
  }

  LayoutTransition MakeSlowSelfTransition()
  {
    LayoutTransitionTiming timing{Duration(0.8f),
                                  AlphaFunction(AlphaFunction::EASE_IN_OUT_SINE),
                                  Duration()};

    // B's own ENTER / EXIT: a slower fade paired with a height expand / shrink.
    // These override the root's slots entirely -- the root's 0.25s fade never
    // runs for B.
    ViewAnimationSpec enterSpec = ViewAnimationSpec::New();
    enterSpec.Opacity(1.0f, Duration(0.6f));
    ViewAnimationSpec exitSpec = ViewAnimationSpec::New();
    exitSpec.Opacity(0.0f, Duration(0.6f));

    LayoutTransition transition = LayoutTransition::New();
    transition.SetChangeTiming(timing)
      .SetEnterVisualSpec(enterSpec)
      .SetExitVisualSpec(exitSpec)
      .SetEnterBoundsEffect(LayoutBoundsEffects::ExpandFrom(LayoutBoundsEdge::TOP, timing))
      .SetExitBoundsEffect(LayoutBoundsEffects::ShrinkTo(LayoutBoundsEdge::TOP, timing));
    return transition;
  }

  View MakeItem(const Vector4& color)
  {
    View item = View::New();
    item.SetBackgroundColor(color);
    item.SetRequestedWidth(MATCH_PARENT);
    item.SetRequestedHeight(mExpanded ? 100.0f : 60.0f);
    return item;
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
    label.SetMargin(Extents(5, 5, 0, 0));
    return label;
  }

  bool OnToggleTouched(Actor /*actor*/, TouchEvent touch)
  {
    if(touch.GetState(0) != PointState::STARTED)
    {
      return false;
    }
    ToggleLayout();
    return true;
  }

  bool OnItemATouched(Actor /*actor*/, TouchEvent touch)
  {
    if(touch.GetState(0) != PointState::STARTED)
    {
      return false;
    }
    // A declares nothing for itself, so the root's ENTER / EXIT drive the add /
    // remove -- the inherited 0.25s fade that B's slower own effects and C's
    // instant opt-out are compared against.
    if(mItemA.GetParent())
    {
      mRoot.Remove(mItemA, RemovePolicy::ANIMATE_EXIT);
    }
    else
    {
      mItemA.SetProperty(Actor::Property::OPACITY, 0.0f);
      mRoot.Add(mItemA);
      RestoreItemOrder();
    }
    return true;
  }

  bool OnItemBTouched(Actor /*actor*/, TouchEvent touch)
  {
    if(touch.GetState(0) != PointState::STARTED)
    {
      return false;
    }
    // B's own ENTER / EXIT drives the add / remove, not the root's, whenever the
    // override is attached. With the override detached the root's faster fade
    // takes over instead.
    if(mItemB.GetParent())
    {
      mRoot.Remove(mItemB, RemovePolicy::ANIMATE_EXIT);
    }
    else
    {
      mItemB.SetProperty(Actor::Property::OPACITY, 0.0f);
      mRoot.Add(mItemB);
      RestoreItemOrder();
    }
    return true;
  }

  bool OnItemCTouched(Actor /*actor*/, TouchEvent touch)
  {
    if(touch.GetState(0) != PointState::STARTED)
    {
      return false;
    }
    // C carries LayoutTransitionMode::PASS_THROUGH: ANIMATE_EXIT unparents it
    // immediately even though the root carries an EXIT effect, and the add
    // skips ENTER, so C appears at full opacity at once.
    if(mItemC.GetParent())
    {
      mRoot.Remove(mItemC, RemovePolicy::ANIMATE_EXIT);
    }
    else
    {
      mRoot.Add(mItemC);
    }
    return true;
  }

  bool OnOverrideTouched(Actor /*actor*/, TouchEvent touch)
  {
    if(touch.GetState(0) != PointState::STARTED)
    {
      return false;
    }
    mOverrideB = !mOverrideB;
    // An uninitialized handle detaches and returns B to the root's rules; it is
    // not an opt-out (that is what C's LayoutTransitionMode::PASS_THROUGH is for).
    mItemB.SetSelfLayoutTransition(mOverrideB ? MakeSlowSelfTransition() : LayoutTransition());
    mOverrideButton.SetText(mOverrideB ? "B: override" : "B: inherit");
    return true;
  }

  void RestoreItemOrder()
  {
    // Add appends, so raise the later items back above the earlier ones -- B
    // first, then C -- to rebuild the A, B, C reading order. Raising an item
    // already on top moves nothing, and an item currently removed by its own
    // button has no parent to be raised within, so each raise is guarded.
    if(mItemB.GetParent())
    {
      mItemB.RaiseToTop(LayoutOrderPolicy::UPDATE);
    }
    if(mItemC.GetParent())
    {
      mItemC.RaiseToTop(LayoutOrderPolicy::UPDATE);
    }
  }

  void ToggleLayout()
  {
    mExpanded = !mExpanded;
    // One layout pass moves the spacer and resizes all three items. A follows
    // the root's 0.25s timing, B its own 0.8s timing, and C snaps.
    mSpacer.SetRequestedHeight(mExpanded ? 200.0f : 40.0f);
    const float itemHeight = mExpanded ? 100.0f : 60.0f;
    mItemA.SetRequestedHeight(itemHeight);
    mItemB.SetRequestedHeight(itemHeight);
    mItemC.SetRequestedHeight(itemHeight);
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
    else if(IsKey(event, Dali::DALI_KEY_CURSOR_UP) || IsKey(event, Dali::DALI_KEY_CURSOR_DOWN))
    {
      ToggleLayout();
    }
  }

private:
  Application& mApplication;
  StackLayout  mRoot;
  View         mSpacer;
  View         mItemA;
  View         mItemB;
  View         mItemC;
  Label        mToggleButton;
  Label        mItemAButton;
  Label        mItemBButton;
  Label        mItemCButton;
  Label        mOverrideButton;
  bool         mExpanded;
  bool         mOverrideB;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application                           application = Application::New(&argc, &argv);
  LayoutTransitionSelfOverrideController controller(application);
  application.MainLoop();
  return 0;
}
