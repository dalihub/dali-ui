/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include "layout-validation-transition-fixtures.h"
using namespace LayoutValidation;
using namespace LayoutValidation::TransitionFixtures;
class Tc131 : public Case
{
public:
  Tc131()
  : Case("TC131", "Transition scope and owner resolution", "Check transition scope and owner resolution")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> cases;
    // 0 direct-only, 1 subtree, 2 isolate, 3 pass-through, 4 closest owner,
    // 5 self owner, 6 standalone boundary, 7 self pass-through.
    for(unsigned mode = 0; mode < 8; ++mode)
      cases.push_back({"TR04.scope-" + std::to_string(mode), "Direct/subtree/self ownership and scope boundaries", {{"mount", 4, [mode](Run& r)
      {
        auto s    = NewState(r);
        s->nested = AbsoluteLayout::New();
        s->nested.SetRequestedWidth(200);
        s->nested.SetRequestedHeight(100);
        SetFixtureBounds(s->nested, {20, 10, 200, 100});
        SetFixtureBounds(s->child, {10, 15, 80, 40});
        s->nested.Add(s->child);
        s->root.Add(s->nested);
        CaptureLifecycle(s);
        s->transition.SetReflowScope(mode == 0 ? LayoutReflowScope::DIRECT_CHILDREN : LayoutReflowScope::SUBTREE);
        if(mode == 2) s->nested.SetLayoutTransitionMode(LayoutTransitionMode::ISOLATE_SUBTREE);
        if(mode == 3) s->nested.SetLayoutTransitionMode(LayoutTransitionMode::PASS_THROUGH);
        if(mode == 6) s->nested.SetLayoutMode(LayoutMode::STANDALONE);
        if(mode == 4 || mode == 5 || mode == 7)
        {
          s->secondTransition = LayoutTransition::New();
          s->secondTransition.SetChangeAnimator(LayoutAnimatorCallback::New(s.get(), &State::OnSecondaryAnimator), AnimatorTiming());
          if(mode == 4)
            s->nested.SetLayoutTransition(s->secondTransition);
          else
            s->child.SetSelfLayoutTransition(s->secondTransition);
        }
        if(mode == 7) s->child.SetLayoutTransitionMode(LayoutTransitionMode::PASS_THROUGH);
        r.Attach(s->root);
        r.AfterLayout({s->child}, [s](Run& v)
        { v.Rect("scope.mount", v.Snapshot(s->child), {10, 15, 80, 40}); });
      }},
                                                                                                                    {"change-grandchild", 6, [mode](Run& r)
      {
        auto s = r.State<State>();
        s->ResetObservations();
        s->secondaryCalls = 0;
        SetFixtureBounds(s->child, {40, 15, 80, 40});
        // The manual pass ends with the animator tick: every fresh animator is dispatched exactly
        // once, at raw progress 0, before ProcessLayouts() returns.
        LayoutController::Get(s->window).ProcessLayouts();
        uint32_t inherited = 0;
        for(std::size_t i = 0; i < s->observationCount; ++i)
          if(s->observations[i].viewId == 2) ++inherited;
        const bool primary   = mode == 1 || mode == 3;
        const bool secondary = mode == 4 || mode == 5;
        r.Equal("owner.ancestor.calls", inherited, primary ? 1 : 0);
        r.Equal("owner.closest-or-self.calls", s->secondaryCalls, secondary ? 1 : 0);
        r.Rect("owner.logical.target", ArrangedBounds(s->child), {40, 15, 80, 40});
      }}}});
    cases.push_back({"TR04.self-root-inert", "Self transition on an actual root has no parent-driven slot", {{"self-root", 5, [](Run& r)
    {
      auto s = NewState(r);
      s->Bind(LayoutTransition::New());
      s->transition.SetChangeAnimator(LayoutAnimatorCallback::New(s.get(), &State::OnAnimator), AnimatorTiming());
      // Standalone root is still attached beneath the fixture host; directly
      // exercise the documented root path through a dedicated Window.
      s->extraWindow = Window::New(PositionSize(20, 20, 320, 240), "TC131 self root");
      s->window      = s->extraWindow;
      s->root.SetSelfLayoutTransition(s->transition);
      s->extraWindow.Add(s->root);
      LayoutController::Get(s->window).ProcessLayouts();
      s->root.SetRequestedWidth(280);
      LayoutController::Get(s->window).ProcessLayouts();
      r.Rect("self.root.target", LayoutValidation::Bounds(s->root), {0, 0, 280, 200});
      r.Equal("self.root.start", s->starts[2], 0);
    }}}});
    cases.push_back({"TR04.inherited-enter-exit-parent-units", "Inherited effect uses the real direct parent's extent", {{"mount-inherited", 4, [](Run& r)
    {
      auto s    = NewState(r);
      s->nested = AbsoluteLayout::New();
      s->nested.SetRequestedWidth(200);
      s->nested.SetRequestedHeight(100);
      SetFixtureBounds(s->nested, {20, 10, 200, 100});
      SetFixtureBounds(s->child, {10, 15, 80, 40});
      s->nested.Add(s->child);
      s->root.Add(s->nested);
      r.Attach(s->root);
      r.AfterLayout({s->child}, [s](Run& v)
      { v.Rect("inherited.mount", v.Snapshot(s->child), {10, 15, 80, 40}); });
    }},
                                                                                                                         {"inherited-enter", 1, [](Run& r)
    {
      auto s = r.State<State>();
      s->Bind(LayoutTransition::New());
      auto effect = LayoutBoundsEffect().SetTiming(Timing()).SetOffset(LayoutBoundsLength::ParentFraction(.5f), LayoutBoundsLength::ParentFraction(.5f));
      s->transition.SetReflowScope(LayoutReflowScope::SUBTREE).SetEnterBoundsEffect(effect).SetExitBoundsEffect(effect).ClearChangeTiming();
      s->root.SetLayoutTransition(s->transition);
      s->nested.Remove(s->child);
      s->nested.Add(s->child);
      LayoutController::Get(s->window).ProcessLayouts();
      r.Equal("inherited.enter.start", s->starts[0], 1);
    }},
                                                                                                                         {"inherited-enter-finish", 6, [](Run& r)
    {
      // Waits for the natural ENTER finish (an event); the frame deadline is only a liveness bound.
      auto s = r.State<State>();
      r.AfterFrame([s](Run& v)
      {
        v.Rect("inherited.enter.final", Current(s->child), {10, 15, 80, 40}, .01);
        v.Equal("inherited.enter.start", s->starts[0], 1);
        v.Equal("inherited.enter.finish", s->finishes[0], 1);
      }, [s]
      { return s->finishes[0] >= 1; }, s->root);
    }},
                                                                                                                         {"inherited-exit", 3, [](Run& r)
    {
      auto s = r.State<State>();
      s->nested.Remove(s->child, RemovePolicy::ANIMATE_EXIT);
      r.Equal("inherited.ghost.logical", s->nested.GetChildViewCount(), 0);
      r.Truth("inherited.ghost.direct-parent", s->child.GetParent() == s->nested);
      r.Equal("inherited.exit.start", s->starts[1], 1);
    }},
                                                                                                                         {"inherited-exit-finish", 7, [](Run& r)
    {
      // The EXIT endpoint is offset by half of the DIRECT parent (200x100) from the base (10,15):
      // (110,65). Half of the owning root's 300x200 extent would give (160,115).
      auto s = r.State<State>();
      r.AfterFrame([s](Run& v)
      {
        v.Rect("inherited.exit.end", s->exitEnd, {110, 65, 80, 40}, .01);
        v.Equal("inherited.exit.start", s->starts[1], 1);
        v.Equal("inherited.exit.finish", s->finishes[1], 1);
        v.Truth("inherited.exit.unparented", !s->child.GetParent());
      }, [s]
      { return s->finishes[1] >= 1; }, s->root);
    }}}});
    // A MATCH_PARENT standalone child of a MATCH_PARENT view whose CHANGE animator holds its actor at
    // the previous size: the child's own layout pass in the same batch follows the parent's layout
    // target (300x150), not the parent's held actor SIZE (200x100). The recording animator writes no
    // geometry and every check runs in the action of the manual pass, so the held SIZE is still the
    // CHANGE's from value; the pass ends with the animator tick that records the parent's context.
    cases.push_back({"TR04.standalone-parent-target", "A standalone child's own pass follows its parent's layout target, not its animated size", {{"mount", 8, [](Run& r)
    {
      auto s    = NewState(r);
      s->root   = View::New();
      s->nested = View::New();
      s->child  = View::New();
      s->root.SetRequestedWidth(200);
      s->root.SetRequestedHeight(100);
      s->nested.SetRequestedWidth(MATCH_PARENT);
      s->nested.SetRequestedHeight(MATCH_PARENT);
      s->child.SetRequestedWidth(MATCH_PARENT);
      s->child.SetRequestedHeight(MATCH_PARENT);
      s->child.SetLayoutMode(LayoutMode::STANDALONE);
      s->nested.Add(s->child);
      s->root.Add(s->nested);
      r.Attach(s->root);
      r.AfterLayout({s->nested, s->child}, [s](Run& v)
      {
        v.Rect("target.mount.parent", v.Snapshot(s->nested), {0, 0, 200, 100});
        v.Rect("target.mount.standalone", v.Snapshot(s->child), {0, 0, 200, 100});
      });
    }},
                                                                                                                                                                        {"change-held", 15, [](Run& r)
    {
      auto s = r.State<State>();
      CaptureLifecycle(s);
      s->root.SetRequestedWidth(300);
      s->root.SetRequestedHeight(150);
      s->child.InvalidateMeasure();
      LayoutController::Get(s->window).ProcessLayouts();
      r.Rect("target.parent.actor-held", LayoutValidation::Bounds(s->nested), {0, 0, 200, 100});
      r.Rect("target.parent.layout-target", ArrangedBounds(s->nested), {0, 0, 300, 150});
      r.Rect("target.standalone.bounds", LayoutValidation::Bounds(s->child), {0, 0, 300, 150});
      r.Size("target.standalone.measured", s->child.GetMeasuredSize(), MeasuredSize(300, 150));
      r.Truth("target.parent.animator-context", FirstObservation(*s, 4, LayoutTransitionSlot::CHANGE).viewId == 4u);
    }}}});
    return cases;
  }
};
REGISTER_MANUAL_TEST(Tc131)
