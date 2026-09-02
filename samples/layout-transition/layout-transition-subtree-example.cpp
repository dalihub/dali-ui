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
 * LayoutTransition reflow-scope sample.
 *
 * A single LayoutTransition is attached to the ROOT container with a CHANGE
 * timing. The root holds a spacer and a nested card; the card holds three
 * inner items (grand-children of the root) and has NO transition of its own.
 *
 * With LayoutReflowScope::SUBTREE one transition reflows the whole subtree:
 * toggling the layout moves the card (via the spacer) AND reflows the inner
 * items inside it, all driven by the root's single transition. Switch the
 * scope back to DIRECT_CHILDREN to see the inner items snap to their final
 * positions while only the card animates.
 *
 *   - Tap "Toggle layout": flip the spacer + inner-item sizes.
 *   - Tap "Scope: ...": toggle the root transition between SUBTREE and
 *     DIRECT_CHILDREN.
 *   - Tap "Add/Remove item": add or remove a magenta grand-child inside the
 *     card. Under SUBTREE the root's inherited ENTER/EXIT animates it (opacity
 *     fade + height expand/shrink); under DIRECT it snaps in/out (visible).
 *   - Up / Down keys: same as Toggle layout.
 *   - Esc / Back: quit.
 */
class LayoutTransitionSubtreeController : public ConnectionTracker
{
public:
  explicit LayoutTransitionSubtreeController(Application& application)
  : mApplication(application),
    mExpanded(false),
    mSubtree(true)
  {
    mApplication.InitSignal().Connect(this, &LayoutTransitionSubtreeController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    mToggleButton = MakeClickableLabel("Toggle layout");
    mToggleButton.TouchEventSignal().Connect(this, &LayoutTransitionSubtreeController::OnToggleTouched);

    mScopeButton = MakeClickableLabel("Scope: SUBTREE");
    mScopeButton.TouchEventSignal().Connect(this, &LayoutTransitionSubtreeController::OnScopeTouched);

    mItemButton = MakeClickableLabel("Add/Remove item");
    mItemButton.TouchEventSignal().Connect(this, &LayoutTransitionSubtreeController::OnItemTouched);

    // Root: a single transition is attached here and governs the whole
    // subtree under SUBTREE scope.
    mRoot = StackLayout::New();
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(MATCH_PARENT);
    mRoot.SetSpacing(10.0f);
    mRoot.SetLayoutTransition(MakeTransition(mSubtree));

    // Spacer above the card. Its height toggles, so the card slides — that
    // movement is animated by the root's transition (the card is a direct
    // child of the root).
    mSpacer = View::New();
    mSpacer.SetBackgroundColor(Color::LIGHT_GRAY);
    mSpacer.SetRequestedWidth(MATCH_PARENT);
    mSpacer.SetRequestedHeight(40.0f);

    // Card: an intermediate container with NO transition of its own. Under
    // SUBTREE scope its inner items still reflow, driven by the root.
    mCard = StackLayout::New();
    mCard.SetRequestedWidth(MATCH_PARENT);
    mCard.SetRequestedHeight(WRAP_CONTENT);
    mCard.SetSpacing(10.0f);
    mCard.SetBackgroundColor(Color::BLACK);

    static constexpr Vector4 palette[] = {Color::RED, Color::GREEN, Color::BLUE};
    for(uint32_t i = 0; i < 3; ++i)
    {
      View item = View::New();
      item.SetBackgroundColor(palette[i]);
      item.SetRequestedWidth(MATCH_PARENT);
      item.SetRequestedHeight(60.0f);
      mCard.Add(item);
    }

    mRoot.Add(mSpacer);
    mRoot.Add(mCard);

    // Button row above the root; no transition so its own layout does not
    // animate.
    FlexLayout buttonRow = FlexLayout::New();
    buttonRow.SetRequestedWidth(MATCH_PARENT);
    buttonRow.SetRequestedHeight(60.0f);
    buttonRow.SetDirection(FlexDirection::ROW);
    buttonRow.SetAlignItems(FlexAlign::STRETCH);
    buttonRow.Add(mToggleButton);
    buttonRow.Add(mScopeButton);
    buttonRow.Add(mItemButton);

    StackLayout outer = StackLayout::New();
    outer.SetRequestedWidth(MATCH_PARENT);
    outer.SetRequestedHeight(MATCH_PARENT);
    outer.Add(buttonRow);
    outer.Add(mRoot);

    window.Add(outer);
    window.KeyEventSignal().Connect(this, &LayoutTransitionSubtreeController::OnKeyEvent);
  }

  LayoutTransition MakeTransition(bool subtree)
  {
    LayoutTransitionTiming timing{Duration(0.4f),
                                  AlphaFunction(AlphaFunction::EASE_IN_OUT_SINE),
                                  Duration()};

    // ENTER fades a newly added item in (opacity 0->1) AND expands its height
    // from 0 to full (ExpandFrom TOP); EXIT mirrors it (fade out + shrink to 0).
    // Under SUBTREE scope these reach grand-children inside the no-transition
    // card, all driven by this single root transition.
    ViewAnimationSpec enterSpec = ViewAnimationSpec::New();
    enterSpec.Opacity(1.0f, Duration(0.3f));
    ViewAnimationSpec exitSpec = ViewAnimationSpec::New();
    exitSpec.Opacity(0.0f, Duration(0.3f));

    LayoutTransition transition = LayoutTransition::New();
    transition.SetChangeTiming(timing)
              .SetEnterVisualSpec(enterSpec)
              .SetExitVisualSpec(exitSpec)
              .SetEnterBoundsEffect(LayoutBoundsEffects::ExpandFrom(
                LayoutBoundsEdge::TOP, timing))
              .SetExitBoundsEffect(LayoutBoundsEffects::ShrinkTo(
                LayoutBoundsEdge::TOP, timing))
              .SetReflowScope(subtree ? LayoutReflowScope::SUBTREE
                                      : LayoutReflowScope::DIRECT_CHILDREN);
    return transition;
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
    label.SetMargin(Insets(5.0f, 5.0f, 0.0f, 0.0f));
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

  bool OnScopeTouched(Actor /*actor*/, TouchEvent touch)
  {
    if(touch.GetState(0) != PointState::STARTED)
    {
      return false;
    }
    mSubtree = !mSubtree;
    mRoot.SetLayoutTransition(MakeTransition(mSubtree));
    mScopeButton.SetText(mSubtree ? "Scope: SUBTREE" : "Scope: DIRECT");
    return true;
  }

  bool OnItemTouched(Actor /*actor*/, TouchEvent touch)
  {
    if(touch.GetState(0) != PointState::STARTED)
    {
      return false;
    }
    // The card has NO transition of its own. Under SUBTREE scope the add /
    // remove of this grand-child is animated by the ROOT transition's ENTER /
    // EXIT slot (opacity fade + height expand / shrink). Under DIRECT scope the
    // grand-child is outside the root's governed set, so it snaps in / out
    // instantly (visible, no animation).
    if(mExtraItem)
    {
      mCard.Remove(mExtraItem, RemovePolicy::ANIMATE_EXIT); // inherited EXIT fades it out, then unparents
      mExtraItem.Reset();
    }
    else
    {
      View item = View::New();
      item.SetBackgroundColor(Color::MAGENTA);
      item.SetRequestedWidth(MATCH_PARENT);
      item.SetRequestedHeight(60.0f);
      if(mSubtree)
      {
        // SUBTREE: the root's inherited ENTER fades this grand-child from 0->1
        // (and now expands its height). Pre-set opacity to 0 so the fade has a
        // start value. Under DIRECT scope no inherited ENTER reaches this
        // grand-child, so leave opacity at 1 and let it snap in visible.
        item.SetProperty(Actor::Property::OPACITY, 0.0f);
      }
      mCard.Add(item);
      mExtraItem = item;
    }
    return true;
  }

  void ToggleLayout()
  {
    mExpanded = !mExpanded;
    // Spacer drives the card's position; inner items drive their own
    // positions inside the card. One root transition animates both levels
    // under SUBTREE scope.
    mSpacer.SetRequestedHeight(mExpanded ? 160.0f : 40.0f);
    const float    itemHeight = mExpanded ? 100.0f : 60.0f;
    const uint32_t count      = mCard.GetChildViewCount();
    for(uint32_t i = 0; i < count; ++i)
    {
      mCard.GetChildViewAt(i).SetRequestedHeight(itemHeight);
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
    else if(IsKey(event, Dali::DALI_KEY_CURSOR_UP) || IsKey(event, Dali::DALI_KEY_CURSOR_DOWN))
    {
      ToggleLayout();
    }
  }

private:
  Application& mApplication;
  StackLayout  mRoot;
  StackLayout  mCard;
  View         mSpacer;
  Label        mToggleButton;
  Label        mScopeButton;
  Label        mItemButton;
  View         mExtraItem;
  bool         mExpanded;
  bool         mSubtree;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application                       application = Application::New(&argc, &argv);
  LayoutTransitionSubtreeController controller(application);
  application.MainLoop();
  return 0;
}
