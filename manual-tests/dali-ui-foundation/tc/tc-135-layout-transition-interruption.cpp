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
namespace
{
void MountIndependentFixture(Run& r)
{
  auto s = NewState(r);
  s->root.Add(s->child);
  s->root.SetProperty(Actor::Property::SENSITIVE, false);
  r.AttachToWindow(s->root);
  // AfterRender, not AfterLayout: the interruption steps read the child's current (update-side)
  // geometry as the continuity start, so the update thread must have applied the mount layout.
  r.AfterRender({s->child}, [s](Run& v)
  { v.Rect("mount.target", v.Snapshot(s->child), {40, 30, 80, 40}); });
}
} // namespace

class Tc135 : public Case
{
public:
  Tc135()
  : Case("TC135", "Transition interruption and continuity", "Check interrupted transition keeps visual bounds")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> cases;
    // Every interruption starts the first transition and interrupts it in ONE synchronous action:
    // no frame runs in between, so the first transition is provably in flight and its visual bounds
    // are still its start. Finishes are awaited as events; the frame deadline is only a liveness bound.
    for(unsigned mode = 0; mode < 4; ++mode)
      cases.push_back({"TR08.same-slot-" + std::to_string(mode), "Spec and Animator replacement continue from the interrupted transition's visual bounds", {{"mount", 4, [](Run& r)
      { MountIndependentFixture(r); }},
                                                                                                                                                 {"replace", (mode & 2) ? 11u : 7u, [mode](Run& r)
      {
        auto s = r.State<State>();
        s->Bind(LayoutTransition::New());
        s->transition.SetChangeTiming(Timing());
        if(mode & 1) s->transition.SetChangeAnimator(LayoutAnimatorCallback::New(s.get(), &State::OnAnimator), AnimatorTiming());
        s->root.SetLayoutTransition(s->transition);
        SetFixtureBounds(s->child, {140, 30, 80, 40});
        LayoutController::Get(s->window).ProcessLayouts();
        r.Equal("first.start", s->starts[2], 1);
        if(mode & 2)
          s->transition.SetChangeAnimator(LayoutAnimatorCallback::New(s.get(), &State::OnAnimator), AnimatorTiming());
        else
          s->transition.ClearChangeAnimator();
        SetFixtureBounds(s->child, {240, 30, 80, 40});
        LayoutController::Get(s->window).ProcessLayouts();
        r.Equal("replacement.start", s->starts[2], 2);
        r.Equal("cancelled.finish.silent", s->finishes[2], 0);
        if(mode & 2)
        {
          // The fresh animator's first context: it continues from the interrupted transition's
          // visual bounds (40,30); continuing from its target would report x = 140.
          const Observation& replacement = LastObservation(*s);
          r.Rect("replacement.from", replacement.from, {40, 30, 80, 40});
          r.Rect("replacement.to", replacement.to, {240, 30, 80, 40});
        }
        else
          r.Rect("replacement.target", LayoutValidation::Bounds(s->child), {240, 30, 80, 40});
      }},
                                                                                                                                                 {"complete-replacement", (mode & 2) ? 3u : 6u, [mode](Run& r)
      {
        auto s = r.State<State>();
        r.AfterFrame([s, mode](Run& v)
        {
          v.Equal("normal.finish.only-new", s->finishes[2], 1);
          v.Truth("replacement.not-exit", static_cast<bool>(s->child.GetParent()));
          if(mode & 2)
            v.Near("replacement.last-raw", LastObservation(*s).raw, 1, .00001);
          else
            v.Rect("replacement.final", Current(s->child), {240, 30, 80, 40}, .01);
        }, [s]
        { return s->finishes[2] >= 1; }, s->root);
      }}}});
    for(bool entering : {false, true})
      cases.push_back({entering ? "TR08.enter-to-exit" : "TR08.change-to-exit", "Cross-slot cancellation preserves visual state and suppresses old completion", {{"mount", 4, [](Run& r)
      { MountIndependentFixture(r); }},
                                                                                                                                                                 {"cross-slot", 7, [entering](Run& r)
      {
        auto s = r.State<State>();
        CaptureLifecycle(s);
        if(entering)
        {
          s->root.Remove(s->child);
          s->root.Add(s->child);
        }
        else
          SetFixtureBounds(s->child, {140, 30, 80, 40});
        LayoutController::Get(s->window).ProcessLayouts();
        // The ENTER or CHANGE animator was dispatched once at raw 0, so its last interpolated
        // bounds are its start (40,30); the arranged bounds of the CHANGE are already at x = 140.
        s->root.Remove(s->child, RemovePolicy::ANIMATE_EXIT);
        // This pass ends with the animator tick that dispatches the fresh EXIT once.
        LayoutController::Get(s->window).ProcessLayouts();
        const Observation& exitContext = LastObservation(*s);
        r.Rect("exit.continuity", exitContext.from, {40, 30, 80, 40});
        r.Equal("exit.slot", static_cast<int>(exitContext.slot), static_cast<int>(LayoutTransitionSlot::EXIT));
        r.Equal("old.finish.silent", s->finishes[entering ? 0 : 2], 0);
        r.Equal("exit.start.once", s->starts[1], 1);
      }},
                                                                                                                                                                 {"exit-finish", 3, [entering](Run& r)
      {
        auto s = r.State<State>();
        r.AfterFrame([s, entering](Run& v)
        {
          v.Equal("old.finish.remains-silent", s->finishes[entering ? 0 : 2], 0);
          v.Equal("exit.finish.once", s->finishes[1], 1);
          v.Truth("exit.detached", !s->child.GetParent());
        }, [s]
        { return s->finishes[1] >= 1; }, s->root);
      }}}});
    for(bool exiting : {false, true})
      cases.push_back({exiting ? "TR08.spec-enter-to-exit" : "TR08.spec-enter-to-change", "Native Spec successor preserves or settles ENTER visual channels", {
        {"mount", 4, [](Run& r) { MountIndependentFixture(r); }},
        {"enter-and-successor", 4, [exiting](Run& r)
        {
          auto s = r.State<State>();
          s->root.Remove(s->child, RemovePolicy::IMMEDIATE);
          s->child.SetProperty(Actor::Property::OPACITY, 0.f);
          s->Bind(LayoutTransition::New());
          auto enter = ViewAnimationSpec::New();
          enter.Opacity(1, Duration(.4f), AlphaFunction(AlphaFunction::LINEAR));
          auto exit = ViewAnimationSpec::New();
          exit.Opacity(0, Duration(.4f), AlphaFunction(AlphaFunction::LINEAR));
          s->transition.SetEnterVisualSpec(enter).SetEnterBoundsEffect(LayoutBoundsEffect().SetTiming(Timing()).SetSizeFactor(.5f, .5f))
            .SetExitVisualSpec(exit).SetExitBoundsEffect(LayoutBoundsEffect().SetTiming(Timing()).SetOffset(LayoutBoundsLength::Pixel(20), LayoutBoundsLength::Pixel(0))).SetChangeTiming(Timing());
          s->root.SetLayoutTransition(s->transition);
          s->root.Add(s->child);
          LayoutController::Get(s->window).ProcessLayouts();
          r.Equal("cross.enter.started", s->starts[0], 1);
          r.Equal("cross.enter.not-finished", s->finishes[0], 0);
          if(exiting) s->root.Remove(s->child, RemovePolicy::ANIMATE_EXIT);
          else { SetFixtureBounds(s->child, {140,30,80,40}); LayoutController::Get(s->window).ProcessLayouts(); }
          r.Equal("cross.old-finish-silent", s->finishes[0], 0);
          r.Equal("cross.new-start-once", s->starts[exiting ? 1 : 2], 1);
        }},
        {"successor-finish", exiting ? 4u : 8u, [exiting](Run& r)
        {
          // A CHANGE successor settles the ENTER channels to their final values (opacity 1) and ends
          // at its target; an EXIT successor keeps fading from the current opacity to 0. The EXIT
          // end geometry depends on the update-side state at the interruption and is not asserted.
          auto s = r.State<State>();
          r.AfterFrame([s, exiting](Run& v)
          {
            v.Equal("cross.finish.old-silent", s->finishes[0], 0);
            v.Equal("cross.finish.new-once", s->finishes[exiting ? 1 : 2], 1);
            v.Truth("cross.finish.parent", exiting ? !s->child.GetParent() : s->child.GetParent() == s->root);
            if(!exiting) v.Rect("cross.finish.current", Current(s->child), {140, 30, 80, 40}, .01);
            v.Near("cross.finish.opacity", s->child.GetCurrentProperty<float>(Actor::Property::OPACITY), exiting ? 0 : 1, .01);
          }, [s, exiting]
          { return s->finishes[exiting ? 1 : 2] >= 1; }, s->root);
        }}
      }});
    return cases;
  }
};
REGISTER_MANUAL_TEST(Tc135)
