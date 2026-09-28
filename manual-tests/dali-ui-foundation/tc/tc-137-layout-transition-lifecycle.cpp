/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include <dali/public-api/object/weak-handle.h>
#include "layout-validation-transition-fixtures.h"
using namespace LayoutValidation;
using namespace LayoutValidation::TransitionFixtures;
namespace
{
struct TickMutation
{
  std::shared_ptr<State> state;
  bool exitReplacement{false};
  bool mutated{false};
  void OnTick(const LayoutAnimatorContext& context)
  {
    state->OnAnimator(context);
    if(context.GetSlot() == LayoutTransitionSlot::CHANGE && context.GetRawProgress() == 1 && !mutated)
    {
      mutated=true;
      state->mutationDone=true;
      state->root.Remove(state->child,exitReplacement?RemovePolicy::ANIMATE_EXIT:RemovePolicy::IMMEDIATE);
    }
  }
};
}
class Tc137 : public Case
{
public:
  Tc137()
  : Case("TC137", "Transition lifecycle mutations and cleanup", "Check cleanup when mutated during a transition")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> cases;
    for(unsigned mutation = 0; mutation < 6; ++mutation)
      cases.push_back({"TR11.lifecycle-" + std::to_string(mutation), "Lifecycle mutation, detach, mode and handle replacement", {{"mount", 4, [](Run& r)
      { Mount(r, NewState(r)); }},
                                                                                                                                 {"start-and-mutate", 1, [mutation](Run& r)
      {
        auto s = r.State<State>();
        s->Bind(LayoutTransition::New());
        s->transition.SetChangeTiming(Timing(.15f));
        s->root.SetLayoutTransition(s->transition);
        s->removeOnStart = mutation == 0;
        s->addOnFinish   = mutation == 1;
        SetFixtureBounds(s->child, {140, 30, 80, 40});
        LayoutController::Get(s->window).ProcessLayouts();
        r.Equal("lifecycle.started", s->starts[2], 1);
        if(mutation == 2) s->root.SetLayoutTransition(LayoutTransition::New());
        if(mutation == 3) s->root.Unparent();
        if(mutation == 4) s->child.SetLayoutTransitionMode(LayoutTransitionMode::PASS_THROUGH);
        if(mutation == 5) s->root.SetLayoutTransition({});
      }},
                                                                                                                                 {"observe-natural-completion", 4, [mutation](Run& r)
      {
        auto s = r.State<State>();
        r.Delay(750, [s, mutation](Run& v)
        {
          const bool cancelled = mutation == 0 || mutation == 3;
          v.Equal("lifecycle.normal-vs-cancelled", s->finishes[2], cancelled ? 0 : 1);
          v.Equal("lifecycle.no-second-start", s->starts[2], 1);
          v.Truth("lifecycle.target-parent", mutation == 0 ? !s->child.GetParent() : s->child.GetParent() == s->root);
          v.Truth("lifecycle.finish-added-child", mutation == 1 ? s->other.GetParent() == s->root : !s->mutationDone || mutation == 0);
        });
      }}}});
    cases.push_back({"TR12.mode-return-no-retro-enter", "Mode toggling does not invent an ENTER for an earlier add", {{"mount", 4, [](Run& r)
    { Mount(r, NewState(r)); }},
                                                                                                                      {"add-suppressed", 3, [](Run& r)
    {
      auto s = r.State<State>();
      CaptureLifecycle(s);
      s->child.SetLayoutTransitionMode(LayoutTransitionMode::PASS_THROUGH);
      s->root.Remove(s->child);
      s->root.Add(s->child);
      LayoutController::Get(s->window).ProcessLayouts();
      s->child.SetLayoutTransitionMode(LayoutTransitionMode::AUTO);
      s->root.InvalidateArrange();
      LayoutController::Get(s->window).ProcessLayouts();
      r.Equal("mode.no-retro-enter", s->starts[0], 0);
      r.Equal("mode.no-retro-finish", s->finishes[0], 0);
      r.Truth("mode.handle-kept", s->root.GetLayoutTransition() == s->transition);
    }},
                                                                                                                      {"fresh-add", 2, [](Run& r)
    {
      auto s = r.State<State>();
      s->root.Remove(s->child);
      s->root.Add(s->child);
      r.Delay(850, [s](Run& v)
      {
        v.Equal("fresh.enter.once", s->starts[0], 1);
        v.Equal("fresh.finish.once", s->finishes[0], 1);
      });
    }}}});
    cases.push_back({"TR13.apply-time-revalidation", "A shared spec mutated after registration is revalidated at dispatch", {{"mount", 4, [](Run& r)
    { Mount(r, NewState(r)); }},
                                                                                                                             {"mutate-registered-spec", 3, [](Run& r)
    {
      auto s      = r.State<State>();
      auto visual = ViewAnimationSpec::New();
      visual.Opacity(1, Duration(.2f));
      s->Bind(LayoutTransition::New());
      s->transition.SetEnterVisualSpec(visual);
      s->root.SetLayoutTransition(s->transition);
      visual.SizeWidth(10, Duration(.2f));
      s->root.Remove(s->child);
      s->root.Add(s->child);
      ExpectException(r, "apply.bounds-rejection",
                      "LayoutTransition visual spec entries must not target layout-owned bounds properties (POSITION_X/Y, SIZE_WIDTH/HEIGHT); use the bounds-effect channel",
                      [&]
      { LayoutController::Get(s->window).ProcessLayouts(); });
      r.Equal("rejected.no-start", s->starts[0], 0);
      s->transition.ClearEnterVisualSpec();
      s->root.InvalidateMeasure();
      LayoutController::Get(s->window).ProcessLayouts();
    }},
                                                                                                                             {"recovery", 4, [](Run& r)
    {
      auto s = r.State<State>();
      SetFixtureBounds(s->child, {42, 30, 80, 40});
      s->transition.ClearChangeTiming();
      r.AfterLayout({s->child}, [s](Run& v)
      { v.Rect("recovery.target", v.Snapshot(s->child), {42, 30, 80, 40}); });
    }}}});
    cases.push_back({"TR11.remove-later-target-on-start", "OnStart removes a later captured target without stale dispatch", {{"mount-two-targets", 8, [](Run& r)
    {
      auto s = NewState(r);
      SetFixtureBounds(s->sibling, {140, 30, 60, 20});
      s->root.Add(s->child);
      s->root.Add(s->sibling);
      r.Attach(s->root);
      r.AfterLayout({s->child, s->sibling}, [s](Run& v)
      {
        v.Rect("later.first", v.Snapshot(s->child), {40, 30, 80, 40});
        v.Rect("later.second", v.Snapshot(s->sibling), {140, 30, 60, 20});
      });
    }},
                                                                                                                             {"remove-later", 3, [](Run& r)
    {
      auto s = r.State<State>();
      s->Bind(LayoutTransition::New());
      s->transition.SetChangeTiming(Timing(.15f));
      s->root.SetLayoutTransition(s->transition);
      s->removeSiblingOnStart = true;
      SetFixtureBounds(s->child, {60, 30, 80, 40});
      SetFixtureBounds(s->sibling, {160, 30, 60, 20});
      LayoutController::Get(s->window).ProcessLayouts();
      r.Truth("later.listener.executed", s->mutationDone);
      r.Truth("later.target.removed", !s->sibling.GetParent());
      r.Equal("later.only-live-start", s->starts[2], 1);
    }},
                                                                                                                             {"live-target-finish", 5, [](Run& r)
    {
      auto s = r.State<State>();
      r.Delay(650, [s](Run& v)
      {
        v.Equal("later.only-live-finish", s->finishes[2], 1);
        v.Rect("later.live.final", Current(s->child), {60, 30, 80, 40}, .01);
      });
    }}}});
    for(bool exitReplacement : {false, true})
      cases.push_back({exitReplacement ? "TR11.tick-replaces-with-exit" : "TR11.tick-removes-self", "Robustness: a contract-violating animator callback cannot finalize stale state", {
        {"mount", 4, [](Run& r) { Mount(r, NewState(r)); }},
        {"mutating-tick", 2, [exitReplacement](Run& r)
        {
          auto s = r.State<State>();
          auto observer = std::make_shared<TickMutation>(); observer->state = s; observer->exitReplacement = exitReplacement;
          r.OnCleanup([observer] { observer->state.reset(); });
          s->Bind(LayoutTransition::New());
          s->transition.SetChangeAnimator(LayoutAnimatorCallback::New(observer.get(), &TickMutation::OnTick), AnimatorTiming(.1f));
          s->transition.SetExitAnimator(LayoutAnimatorCallback::New(observer.get(), &TickMutation::OnTick), AnimatorTiming(.2f));
          s->root.SetLayoutTransition(s->transition);
          // The pass ends with the first animator tick (raw 0); the real clock then drives the CHANGE
          // to raw 1, where the callback mutates the tree from inside the tick.
          SetFixtureBounds(s->child, {140,30,80,40}); LayoutController::Get(s->window).ProcessLayouts();
          r.Near("mutation.first-raw", FirstObservation(*s, 2, LayoutTransitionSlot::CHANGE).raw, 0, 0);
          r.Equal("mutation.old-start", s->starts[2], 1);
        }},
        {"drain", exitReplacement ? 7u : 6u, [exitReplacement](Run& r)
        {
          // Waits for the mutation and, for the EXIT replacement, the EXIT's natural finish (events);
          // the frame deadline is only a liveness bound.
          auto s=r.State<State>();
          r.AfterFrame([s, exitReplacement](Run& v)
          {
            v.Truth("mutation.callback-ran", s->mutationDone);
            v.Equal("mutation.final-old-finish",s->finishes[2],0);
            v.Equal("mutation.logical-removed", s->root.GetChildViewCount(),0);
            v.Equal("mutation.exit-start", s->starts[1],exitReplacement?1:0);
            v.Equal("mutation.final-exit-finish",s->finishes[1],exitReplacement?1:0);
            v.Truth("mutation.final-unparented",!s->child.GetParent());
            // The stale tick that removed the CHANGE must not finalize the fresh EXIT entry it
            // created: the EXIT is dispatched from raw 0.
            if(exitReplacement) v.Near("mutation.exit.first-raw", FirstObservation(*s, 2, LayoutTransitionSlot::EXIT).raw, 0, 0);
          }, [s, exitReplacement]
          { return s->mutationDone && (!exitReplacement || s->finishes[1] >= 1); }, s->root);
        }}
      }});
    for(bool animator : {false, true})
      cases.push_back({animator ? "TR12.parent-last-handle-animator" : "TR12.parent-last-handle-spec", "Inherited ghost cancellation precedes offscene parent destruction", {
        {"mount",4,[](Run& r)
        {
          auto s=NewState(r); s->nested=AbsoluteLayout::New(); s->nested.SetRequestedWidth(200);s->nested.SetRequestedHeight(100);
          SetFixtureBounds(s->nested,{20,10,200,100});SetFixtureBounds(s->child,{10,15,80,40});
          s->nested.Add(s->child);s->root.Add(s->nested);r.Attach(s->root);
          r.AfterLayout({s->child},[s](Run&v){v.Rect("lifetime.mount",v.Snapshot(s->child),{10,15,80,40});});
        }},
        {"destroy-parent",10,[animator](Run& r)
        {
          auto s=r.State<State>();
          s->Bind(LayoutTransition::New());s->transition.SetReflowScope(LayoutReflowScope::SUBTREE).ClearChangeTiming();
          if(animator)s->transition.SetExitAnimator(LayoutAnimatorCallback::New(s.get(),&State::OnAnimator),AnimatorTiming(.2f));
          else s->transition.SetExitBoundsEffect(LayoutBoundsEffect().SetTiming(Timing(.2f)).SetOffset(LayoutBoundsLength::Pixel(20),LayoutBoundsLength::Pixel(0)));
          s->root.SetLayoutTransition(s->transition);
          for(auto property:{Actor::Property::SENSITIVE,Actor::Property::FOCUSABLE,Actor::Property::FOCUS_ON_TOUCH})s->child.SetProperty(property,true);
          s->nested.Remove(s->child,RemovePolicy::ANIMATE_EXIT);
          r.Equal("lifetime.exit-start",s->starts[1],1);
          r.Equal("lifetime.logical-child-removed",s->nested.GetChildViewCount(),0);
          r.Truth("lifetime.ghost-parent",s->child.GetParent()==s->nested);
          r.Equal("lifetime.interaction-disabled",State::InteractionBits(s->child),0);
          WeakHandle<View> weak(s->nested);
          s->root.Remove(s->nested,RemovePolicy::IMMEDIATE);
          r.Truth("lifetime.offscene",!s->nested.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE));
          r.Equal("lifetime.cancel-restored-interaction",State::InteractionBits(s->child),7);
          s->nested.Reset();
          r.Truth("lifetime.parent-destroyed",!weak.GetHandle());
          r.Truth("lifetime.child-unparented",!s->child.GetParent());
          r.Equal("lifetime.cancel-silent",s->finishes[1],0);
          r.Truth("lifetime.child-still-valid",static_cast<bool>(s->child));
        }},
        {"late-finish-silent",3,[](Run&r)
        {
          auto s=r.State<State>();r.Delay(650,[s](Run&v){v.Equal("lifetime.no-late-finish",s->finishes[1],0);v.Equal("lifetime.interaction-remains-restored",State::InteractionBits(s->child),7);v.Truth("lifetime.parent-remains-empty",!s->child.GetParent());});
        }}
      }});
    return cases;
  }
};
REGISTER_MANUAL_TEST(Tc137)
