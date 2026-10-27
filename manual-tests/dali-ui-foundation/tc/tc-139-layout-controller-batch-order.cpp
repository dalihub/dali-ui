/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <vector>
#include "layout-validation-transition-fixtures.h"
using namespace LayoutValidation;
using namespace LayoutValidation::TransitionFixtures;
class Tc139 : public Case
{
public:
  Tc139()
  : Case("TC139", "Controller batch and root ordering", "Check batched root requests and their ordering")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> cases;
    cases = {
      {"CT02.depth-and-dedup", "Duplicate and reverse-ordered root requests in one batch settle every root at its target", {{"mount", 4, [](Run& r)
    {
      auto s = NewState(r);
      s->child.SetLayoutMode(LayoutMode::STANDALONE);
      s->child.SetRequestedX(40);
      s->child.SetRequestedY(30);
      Mount(r, s);
    }},
                                                                                                                {"queue-reverse-order", 4, [](Run& r)
    {
      // The drain order and the de-duplication are internal to the controller; the black-box
      // result is that one pass settles the standalone child at its logical target.
      auto  s          = r.State<State>();
      auto& controller = LayoutController::Get(s->window);
      s->root.InvalidateMeasure();
      s->child.InvalidateMeasure();
      controller.RequestLayout(&GetImpl(s->child));
      controller.RequestLayout(&GetImpl(s->root));
      controller.RequestLayout(&GetImpl(s->child));
      controller.ProcessLayouts();
      r.Rect("batch.child.target", ArrangedBounds(s->child), {40, 30, 80, 40});
    }}}},
      {"CT02.deleted-pending-root", "Unregistering a queued off-scene root removes pending work without dangling access", {{"unregister", 1, [](Run& r)
    {
      auto  s          = NewState(r);
      auto& controller = LayoutController::Get(s->window);
      s->root          = View::New();
      s->root.SetMeasureCallback(MeasureCallback::New(s.get(), &State::Measure));
      controller.RequestLayout(&GetImpl(s->root));
      controller.UnregisterView(&GetImpl(s->root));
      // The pass after the unregistration must neither measure the removed root nor touch it.
      controller.ProcessLayouts();
      r.Equal("unregister.no-producer", s->measureCalls, 0);
    }}}}};
    // A standalone child of the vertical 300x200 stack root queues its own turn, then switches to
    // DEFAULT in the same batch: the stack lays it out after the 20-high sibling, as its event-side
    // target and as its logical arranged rect. The withdrawn root registration and the cancelled
    // queued turn are controller state and are not observed.
    cases.push_back({"CT02.mode-switch-cancels-self-turn", "Switching a standalone child to DEFAULT in the batch that queued its own turn lays it out as an ordinary stack child", {{"mount", 8, [](Run& r)
    {
      auto s = NewState(r, true);
      s->child.SetLayoutMode(LayoutMode::STANDALONE);
      s->child.SetRequestedX(40);
      s->child.SetRequestedY(30);
      s->root.Add(s->sibling);
      s->root.Add(s->child);
      r.Attach(s->root);
      r.AfterLayout({s->child, s->sibling}, [s](Run& v)
      {
        v.Rect("switch.standalone.target", v.Snapshot(s->child), {40, 30, 80, 40});
        v.Rect("switch.sibling.target", v.Snapshot(s->sibling), {0, 0, 60, 20});
      });
    }},
                                                                                                                                                                                                {"switch-before-turn", 8, [](Run& r)
    {
      auto s = r.State<State>();
      s->child.InvalidateMeasure();
      s->child.SetLayoutMode(LayoutMode::DEFAULT);
      LayoutController::Get(s->window).ProcessLayouts();
      r.Rect("switch.default.target", LayoutValidation::Bounds(s->child), {0, 20, 80, 40});
      r.Rect("switch.default.logical", ArrangedBounds(s->child), {0, 20, 80, 40});
    }},
                                                                                                                                                                                                {"steady", 4, [](Run& r)
    {
      auto s = r.State<State>();
      r.AfterFrame([s](Run& v)
      { v.Rect("switch.steady.current", Current(s->child), {0, 20, 80, 40}); }, {}, s->child);
    }}}});
    return cases;
  }
};
REGISTER_MANUAL_TEST(Tc139)
