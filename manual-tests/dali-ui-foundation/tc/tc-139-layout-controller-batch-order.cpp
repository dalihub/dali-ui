/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include <dali-ui-foundation/public-api/views/view-impl.h>
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
    return cases;
  }
};
REGISTER_MANUAL_TEST(Tc139)
