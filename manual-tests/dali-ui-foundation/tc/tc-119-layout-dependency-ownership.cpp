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
      out.push_back(Steps(Id("K13.owner", kind), "The final-slot measure of a MATCH_PARENT child runs inside its arranging parent's Arrange and leaves the parent's caches valid", {
        {"run", 7, [kind](Run& r) {
           // A wrapping parent with a 120x60 minimum measures the child at the 300x100 stage
           // constraint and then re-measures it at the 120x60 final slot inside Arrange.
           auto s    = std::make_shared<Owned>();
           s->parent = Container(kind);
           s->parent.SetMinimumWidth(120);
           s->parent.SetMinimumHeight(60);
           s->probe = Probed(r, 30, 10, false);
           s->probe->view.SetRequestedWidth(MATCH_PARENT);
           s->probe->view.SetRequestedHeight(MATCH_PARENT);
           s->parent.Add(s->probe->view);
           // Arrange-time criterion: the parent publishes its measured size only after its own
           // producer, so a child measure whose constraint already equals that published size is
           // the re-measure the parent's Arrange makes at the final slot.
           auto   owned     = std::make_shared<uint32_t>(0);
           Probe* probe     = s->probe.get();
           View   parent    = s->parent;
           probe->onMeasure = [owned, probe, parent]() {
             const MeasuredSize published = parent.GetMeasuredSize();
             if(std::abs(published.width - probe->lastWidth) <= GEOMETRY_TOLERANCE && std::abs(published.height - probe->lastHeight) <= GEOMETRY_TOLERANCE) ++*owned;
           };
           s->stage   = Mount(r, s->parent, 300, 100);
           s->sibling = Leaf(10, 10, "sibling");
           s->sibling.SetRequestedY(80);
           s->stage.Add(s->sibling);
           r.SetState(s);
           r.AfterRender({s->stage, s->parent, s->probe->view, s->sibling}, [=](Run& r) {
             s->probe->onMeasure = {};
             s->measures         = s->probe->measures;
             r.Truth("owned-producer-observed", *owned > 0);
             r.Size("final-measure-constraint", MeasuredSize(s->probe->lastWidth, s->probe->lastHeight), MeasuredSize(120, 60));
             r.Rendered("final-slot", s->probe->view, LayoutRect(0, 0, 120, 60));
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
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc119)
