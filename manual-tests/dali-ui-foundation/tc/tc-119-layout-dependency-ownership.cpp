/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include "layout-validation-fixtures.h"

using namespace LayoutValidation;
using namespace LayoutValidation::Fixtures;

namespace
{
struct Owned
{
  std::shared_ptr<Probe> probe;
  View                   stage, parent, sibling;
  uint32_t               measures{0};
};
struct OwnedCalls
{
  uint32_t     calls{0};
  bool         budgetSeen{false};
  MeasuredSize budget{-1, -1}; ///< Constraint of the child's first producer call
  MeasuredSize owned{-1, -1};  ///< Constraint of the last producer call made inside the parent's Arrange
};
/// Records one producer call of the probed child. Arrange-time criterion: the parent publishes
/// its measured size only after its own producer, so a child measure whose constraint already
/// equals that published size is the re-measure the parent's Arrange makes at the final slot.
void CountOwnedCall(OwnedCalls& calls, const Probe& probe, View parent)
{
  const MeasuredSize constraint(probe.lastWidth, probe.lastHeight);
  if(!calls.budgetSeen)
  {
    calls.budgetSeen = true;
    calls.budget     = constraint;
  }
  const MeasuredSize published = parent.GetMeasuredSize();
  if(std::abs(published.width - constraint.width) <= GEOMETRY_TOLERANCE && std::abs(published.height - constraint.height) <= GEOMETRY_TOLERANCE)
  {
    ++calls.calls;
    calls.owned = constraint;
  }
}
} // namespace

class Tc119 : public Case
{
public:
  Tc119()
  : Case("TC119", "Dependency ownership", "Check where a MATCH_PARENT child is re-measured")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    for(unsigned kind : {0u, 1u, 2u, 3u, 4u})
      out.push_back(Steps(Id("K13.owner", kind), "The arrange-time final-slot measure of a MATCH_PARENT child whose parent has no size of its own runs inside the arranging parent's Arrange and leaves the parent's caches valid", {
        {"run", 11, [kind](Run& r) {
           // A wrapping parent whose only child is MATCH_PARENT has no size of its own: it keeps the
           // child's at-most probe at the 300x100 stage constraint, measures 30x10 and re-measures the
           // child at the 30x10 final slot inside Arrange.
           auto s    = std::make_shared<Owned>();
           s->parent = Container(kind);
           s->probe  = Probed(r, 30, 10, false);
           s->probe->view.SetRequestedWidth(MATCH_PARENT);
           s->probe->view.SetRequestedHeight(MATCH_PARENT);
           s->parent.Add(s->probe->view);
           auto   calls     = std::make_shared<OwnedCalls>();
           Probe* probe     = s->probe.get();
           View   parent    = s->parent;
           probe->onMeasure = [calls, probe, parent]() { CountOwnedCall(*calls, *probe, parent); };
           s->stage         = Mount(r, s->parent, 300, 100);
           s->sibling       = Leaf(10, 10, "sibling");
           s->sibling.SetRequestedY(80);
           s->stage.Add(s->sibling);
           r.SetState(s);
           r.AfterRender({s->stage, s->parent, s->probe->view, s->sibling}, [=](Run& r) {
             s->probe->onMeasure = {};
             s->measures         = s->probe->measures;
             r.Truth("owned-producer-observed", calls->calls > 0);
             r.Size("budget-constraint", calls->budget, MeasuredSize(300, 100));
             r.Size("owned-constraint", calls->owned, MeasuredSize(30, 10));
             r.Size("measured", s->probe->view.GetMeasuredSize(), MeasuredSize(30, 10));
             r.Rendered("final-slot", s->probe->view, LayoutRect(0, 0, 30, 10));
           });
         }},
        {"stable", 1, [](Run& r) {
           // A sibling-only pass: the parent's caches stay valid, so the child producer must not run.
           // Had the arrange-time measure been attributed outside the parent, the parent would
           // re-measure the child at the stage constraint here.
           auto s = r.State<Owned>();
           s->sibling.SetRequestedWidth(11);
           r.AfterRender({s->stage, s->parent, s->probe->view, s->sibling}, [=](Run& r) { r.Equal("stable.no-producer", s->probe->measures, s->measures); });
         }},
      }));
    for(unsigned kind : {0u, 4u})
      out.push_back(Single(Id("K13.measure-pass-slot", kind).c_str(), "A parent whose own minimum gives it a size measures its MATCH_PARENT child at the final slot in the measure pass; Arrange owns no re-measure", 10, [kind](Run& r) {
        // The 120x60 minimum gives the wrapping parent a size of its own, so the child is probed at
        // the 300x100 stage constraint and then measured EXACTLY at the 120x60 slot inside Measure;
        // Arrange finds that measurement and skips the re-measure. Both calls happen before the
        // parent publishes its measured size, so the arrange-time criterion counts neither.
        View parent = Container(kind);
        parent.SetMinimumWidth(120);
        parent.SetMinimumHeight(60);
        auto p = Probed(r, 30, 10, false);
        p->view.SetRequestedWidth(MATCH_PARENT);
        p->view.SetRequestedHeight(MATCH_PARENT);
        parent.Add(p->view);
        auto   calls = std::make_shared<OwnedCalls>();
        View   child = p->view;
        Probe* probe = p.get();
        p->onMeasure = [calls, probe, parent]() { CountOwnedCall(*calls, *probe, parent); };
        View stage   = Mount(r, parent, 300, 100);
        r.AfterRender({stage, parent, child}, [=](Run& r) {
          p->onMeasure = {};
          r.Equal("owned-calls", calls->calls, 0);
          r.Equal("child-producer", p->measures, 2);
          r.Size("final-measure-constraint", MeasuredSize(p->lastWidth, p->lastHeight), MeasuredSize(120, 60));
          r.Size("measured", child.GetMeasuredSize(), MeasuredSize(120, 60));
          r.Rendered("child", child, LayoutRect(0, 0, 120, 60));
        });
      }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc119)
