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
// Every scenario observes the native spec transition through events and end states only: the
// lifecycle start in the action that starts it, the natural finish (awaited as an event; the frame
// deadline is only a liveness bound), the ENTER final current geometry and the EXIT endpoint the
// view holds when OnFinished(EXIT) runs. No transition is seeked or paused.
//
// replay: the ENTER action also runs a same-bounds ancestor pass while the ENTER is provably in
// flight (same synchronous action), which must neither restart nor cancel it.
Scenario BoundsScenario(const std::string& id, LayoutBoundsEffect effect, LayoutRect endpoint, bool rtl = false, bool independentRoot = true, bool replay = false)
{
  const LayoutRect base{rtl ? 180.f : 40.f, 30, 80, 40};
  Scenario         scenario{id, "ENTER endpoint to target; EXIT current bounds to endpoint", {}};
  scenario.steps.push_back({"mount-parent", 4, [rtl, independentRoot](Run& r)
  {
    auto s = NewState(r);
    if(rtl) s->root.SetProperty(Actor::Property::LAYOUT_DIRECTION, LayoutDirection::RIGHT_TO_LEFT);
    s->root.Add(s->sibling);
    // Keep result-label invalidation outside the animated fixture tree.
    // Both roots still use the same real Window/controller.
    if(independentRoot)
    {
      s->root.SetProperty(Actor::Property::SENSITIVE, false);
      r.AttachToWindow(s->root);
    }
    else
      r.Attach(s->root);
    r.AfterLayout({s->sibling}, [s, rtl](Run& v)
    { v.Rect("anchor.target", v.Snapshot(s->sibling), {rtl ? 240.f : 0.f, 0, 60, 20}); });
  }});
  scenario.steps.push_back({replay ? "enter-and-replay" : "enter", replay ? 4u : 1u, [effect, replay](Run& r)
  {
    auto s = r.State<State>();
    s->Bind(LayoutTransition::New());
    s->transition.SetEnterBoundsEffect(effect).SetExitBoundsEffect(effect).ClearChangeTiming();
    s->root.SetLayoutTransition(s->transition);
    s->root.Add(s->child);
    LayoutController::Get(s->window).ProcessLayouts();
    r.Equal("enter.start.once", s->starts[0], 1);
    if(replay)
    {
      // The ENTER is in flight the way an application's transition is: PLAYING, with the target
      // (40,30,80,40) on the event side since Play(). A same-bounds ancestor pass reaches the
      // child and must leave it alone: the target stays, the ENTER is not restarted, and a
      // cancellation (which settles silently) would show as a missing finish in the next action.
      r.Near("ancestor-replay.before.event-y", s->child.GetProperty<float>(Actor::Property::POSITION_Y), 30);
      r.Host().InvalidateArrange();
      LayoutController::Get(s->window).ProcessLayouts();
      r.Near("ancestor-replay.after.event-y", s->child.GetProperty<float>(Actor::Property::POSITION_Y), 30);
      r.Equal("ancestor-replay.no-restart", s->starts[0], 1);
    }
  }});
  scenario.steps.push_back({"enter-natural-finish", 6, [base](Run& r)
  {
    auto s = r.State<State>();
    r.AfterFrame([s, base](Run& v)
    {
      v.Rect("enter.final", Current(s->child), base, .01);
      v.Equal("enter.start.once", s->starts[0], 1);
      v.Equal("enter.finish.once", s->finishes[0], 1);
    }, [s]
    { return s->finishes[0] >= 1; }, s->root);
  }});
  scenario.steps.push_back({"exit", 3, [](Run& r)
  {
    auto s = r.State<State>();
    s->root.Remove(s->child, RemovePolicy::ANIMATE_EXIT);
    r.Equal("exit.logical.count", s->root.GetChildViewCount(), 1);
    r.Truth("exit.actor.parent", s->child.GetParent() == s->root);
    r.Equal("exit.start.once", s->starts[1], 1);
  }});
  scenario.steps.push_back({"exit-natural-finish", 8, [endpoint](Run& r)
  {
    // The EXIT starts from the arranged bounds the finished ENTER left, so the view ends at the
    // effect endpoint. The child is unparented at the finish, so the frame source is the root.
    auto s = r.State<State>();
    r.AfterFrame([s, endpoint](Run& v)
    {
      v.Rect("exit.endpoint", s->exitEnd, endpoint, .01);
      v.Truth("exit.unparented", !s->child.GetParent());
      v.Equal("exit.start.once", s->starts[1], 1);
      v.Equal("exit.finish.once", s->finishes[1], 1);
      v.Equal("exit.finish.parent.empty", s->parentAtFinish[1], 0);
    }, [s]
    { return s->finishes[1] >= 1; }, s->root);
  }});
  return scenario;
}
} //namespace
class Tc132 : public Case
{
public:
  Tc132()
  : Case("TC132", "Transition bounds units and edges", "Check LayoutBoundsEffect edges, units and signs")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> cases;
    for(unsigned edge = 0; edge < 4; ++edge)
      for(unsigned unit = 0; unit < 3; ++unit)
        for(int sign : {-1, 1})
        {
          const bool  horizontal = edge >= 2;
          const float value      = (unit == 0 ? 17.f : .25f) * sign;
          const float magnitude  = unit == 0 ? 17.f : (unit == 1 ? (horizontal ? 80.f : 40.f) : (horizontal ? 300.f : 200.f)) * .25f;
          const float offset     = magnitude * sign * ((edge == 0 || edge == 2) ? -1.f : 1.f);
          LayoutRect  endpoint{40, 30, 80, 40};
          (horizontal ? endpoint.x : endpoint.y) += offset;
          auto effect = LayoutBoundsEffects::SlideFrom(static_cast<LayoutBoundsEdge>(edge),
                                                       LayoutBoundsLength{value, static_cast<LayoutBoundsUnit>(unit)}, Timing(.4f));
          cases.push_back(BoundsScenario("TR05.slide-e" + std::to_string(edge) + "-u" + std::to_string(unit) + "-s" + std::to_string(sign), effect, endpoint));
        }
    for(unsigned edge = 0; edge < 4; ++edge)
    {
      const LayoutRect endpoint[] = {{40, 30, 80, 0}, {40, 70, 80, 0}, {40, 30, 0, 40}, {120, 30, 0, 40}};
      cases.push_back(BoundsScenario("TR05.expand-e" + std::to_string(edge),
                                     LayoutBoundsEffects::ExpandFrom(static_cast<LayoutBoundsEdge>(edge), Timing()), endpoint[edge]));
    }
    for(float factor : {.5f, 1.f, 1.5f})
    {
      // One axis remains nonidentity so factor == 1 still exercises the timed path.
      auto effect = LayoutBoundsEffect().SetTiming(Timing()).SetSizeFactor(factor, .5f).SetAnchor(.5f, .5f);
      cases.push_back(BoundsScenario("TR05.factor-" + std::to_string(factor), effect,
                                     {40 + (80 - 80 * factor) * .5f, 40, 80 * factor, 20}));
    }
    auto mixed = LayoutBoundsEffect().SetTiming(Timing()).SetSizeFactor(.5f, 1.5f).SetAnchor(1, .5f).SetOffset(LayoutBoundsLength::ParentFraction(.1f), LayoutBoundsLength::SelfFraction(-.5f));
    cases.push_back(BoundsScenario("TR05.mixed-units-anchor", mixed, {110, 0, 40, 60}));
    cases.push_back(BoundsScenario("TR14.physical-left-rtl", LayoutBoundsEffects::SlideFrom(LayoutBoundsEdge::LEFT, Timing()), {100, 30, 80, 40}, true));
    cases.push_back(BoundsScenario("TR14.ancestor-replay-during-enter",
                                   LayoutBoundsEffects::SlideFrom(LayoutBoundsEdge::TOP, LayoutBoundsLength::Pixel(-17), Timing(.4f)),
                                   {40, 47, 80, 40}, false, false, true));
    return cases;
  }
};
REGISTER_MANUAL_TEST(Tc132)
