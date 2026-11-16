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
struct OutOfBand
{
  std::shared_ptr<Probe> probe;
  View                   stage, parent, sibling, a, b;
  uint32_t               measures{0};
  float                  nudge{10};
  void                   Nudge()
  {
    sibling.SetRequestedWidth(nudge += 1);
  }
};
// An on-screen horizontal stack (200x80) holding the probe, next to a sibling leaf used to
// force later controller passes without touching the stack subtree.
std::shared_ptr<OutOfBand> MountStack(Run& r, std::shared_ptr<Probe> probe)
{
  auto s     = std::make_shared<OutOfBand>();
  s->probe   = probe;
  s->parent  = StackLayout::New(StackOrientation::HORIZONTAL);
  Fixed(s->parent, 200, 80);
  s->parent.Add(probe->view);
  s->sibling = Leaf(10, 10, "sibling");
  s->sibling.SetRequestedY(90);
  s->stage = r.Stage(200, 110);
  s->stage.Add(s->parent);
  s->stage.Add(s->sibling);
  r.SetState(s);
  return s;
}
// The slot of the WRAP_CONTENT x WRAP_CONTENT standalone probe (natural 300x90, requested
// position (7,9), no margin): every path measures it against the window it is shown in, not
// against its 200x80 parent. The window size is an input read at check time.
LayoutRect StandaloneSlot(Run& r)
{
  const auto window = r.GetWindow().GetPositionSize();
  return LayoutRect(7, 9, std::min(300.0f, static_cast<float>(window.width)), std::min(90.0f, static_cast<float>(window.height)));
}
} // namespace

class Tc120 : public Case
{
public:
  Tc120()
  : Case("TC120", "Out of band layout", "Check recovery after direct Measure/Arrange calls")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    out.push_back(Steps("K14.external-measure", "After an out-of-band Measure on a rendered tree the next pass re-measures the child at its slot and publishes fresh caches", {
      {"mount", 4, [](Run& r) {
         auto probe = Probed(r, 30, 10, false);
         probe->view.SetRequestedWidth(MATCH_PARENT);
         probe->view.SetRequestedHeight(MATCH_PARENT);
         auto s = MountStack(r, probe);
         r.AfterRender({s->stage, s->parent, s->probe->view, s->sibling}, [=](Run& r) {
           s->measures = s->probe->measures;
           r.Rendered("before.slot", s->probe->view, LayoutRect(0, 0, 200, 80));
         });
       }},
      {"external", 9, [](Run& r) {
         auto s = r.State<OutOfBand>();
         s->probe->view.Measure(70, 40);
         r.Equal("external.producer", s->probe->measures, s->measures + 1);
         r.Size("external.constraint", MeasuredSize(s->probe->lastWidth, s->probe->lastHeight), MeasuredSize(70, 40));
         s->Nudge();
         r.AfterRender({s->stage, s->parent, s->probe->view, s->sibling}, [=](Run& r) {
           // The next pass has to measure the child at its 200x80 slot again. Whether the out-of-band
           // Measure cleared the ancestor caches is internal state and is not observed here: the child's
           // own arrange entry is dropped as well, which alone makes the parent re-measure it.
           r.Size("restored.constraint", MeasuredSize(s->probe->lastWidth, s->probe->lastHeight), MeasuredSize(200, 80));
           r.Rendered("restored", s->probe->view, LayoutRect(0, 0, 200, 80));
           s->measures = s->probe->measures;
         });
       }},
      {"repeat", 1, [](Run& r) {
         // The restoring pass published fresh caches again, so a sibling-only pass runs no child producer.
         auto s = r.State<OutOfBand>();
         s->Nudge();
         r.AfterRender({s->stage, s->parent, s->probe->view, s->sibling}, [=](Run& r) { r.Equal("repeat.no-producer", s->probe->measures, s->measures); });
       }},
    }));
    out.push_back(Steps("K14.external-standalone-measure", "A standalone slot corrected out of band is consumed by the parent's next Arrange", {
      {"mount", 5, [](Run& r) {
         auto probe = Probed(r, 300, 90, false);
         probe->view.SetLayoutMode(LayoutMode::STANDALONE);
         probe->view.SetRequestedX(7);
         probe->view.SetRequestedY(9);
         auto s = MountStack(r, probe);
         r.AfterRender({s->stage, s->parent, s->sibling}, [=](Run& r) {
           s->measures = s->probe->measures;
           r.Rendered("before.slot", s->probe->view, StandaloneSlot(r));
           r.Equal("mount.producer-once", s->probe->measures, 1);
         });
       }},
      // Every path measures the probe at the same window key: the parent's measure
      // (MeasureStandaloneChildren) is the only producer run, and the placement and the probe's
      // own layout-root pass are cache hits. This step re-bases the producer count on one more
      // parent pass; the external step starts from that state.
      {"warmup", 4, [](Run& r) {
         auto s = r.State<OutOfBand>();
         s->Nudge();
         r.AfterRender({s->stage, s->parent, s->sibling}, [=](Run& r) {
           r.Rendered("warmup.slot", s->probe->view, StandaloneSlot(r));
           s->measures = s->probe->measures;
         });
       }},
      {"external", 7, [](Run& r) {
         auto s = r.State<OutOfBand>();
         r.Size("external.measure-result", s->probe->view.Measure(70, 40), MeasuredSize(70, 40));
         s->Nudge();
         r.AfterRender({s->stage, s->parent, s->sibling}, [=](Run& r) {
           // The external Measure plus exactly one corrective measure at the parent's slot.
           r.Equal("corrective-child-producer", s->probe->measures, s->measures + 2);
           r.Rendered("corrected.slot", s->probe->view, StandaloneSlot(r));
         });
       }},
      {"repeat", 1, [](Run& r) {
         auto s = r.State<OutOfBand>();
         s->Nudge();
         r.AfterRender({s->stage, s->parent, s->sibling}, [=](Run& r) { r.Equal("repeat.no-child-producer", s->probe->measures, s->measures + 2); });
       }},
      // The slot does not depend on which path measured last: the parent's own measure pass
      // measures the probe at the same window key (a cache hit) and places it at the same slot.
      {"path-independence", 5, [](Run& r) {
         auto s = r.State<OutOfBand>();
         s->parent.InvalidateMeasure();
         r.AfterRender({s->stage, s->parent, s->sibling}, [=](Run& r) {
           r.Rendered("path-independence.slot", s->probe->view, StandaloneSlot(r));
           r.Equal("path-independence.no-producer", s->probe->measures, s->measures + 2);
         });
       }},
    }));
    out.push_back(Steps("K14.external-arrange", "A direct child Arrange on a rendered tree cannot leave a stale ancestor replay", {
      {"mount", 8, [](Run& r) {
         auto s    = std::make_shared<OutOfBand>();
         s->parent = StackLayout::New(StackOrientation::HORIZONTAL);
         Fixed(s->parent, 100, 50);
         s->a = Leaf(20, 10, "a");
         s->b = Leaf(30, 10, "b");
         s->parent.Add(s->a);
         s->parent.Add(s->b);
         s->sibling = Leaf(10, 10, "sibling");
         s->sibling.SetRequestedY(60);
         s->stage = r.Stage(100, 80);
         s->stage.Add(s->parent);
         s->stage.Add(s->sibling);
         r.SetState(s);
         r.AfterRender({s->stage, s->parent, s->a, s->b, s->sibling}, [=](Run& r) {
           r.Rendered("a", s->a, LayoutRect(0, 0, 20, 10));
           r.Rendered("b", s->b, LayoutRect(20, 0, 30, 10));
         });
       }},
      {"external-arrange", 8, [](Run& r) {
         auto s = r.State<OutOfBand>();
         s->b.Arrange(LayoutRect(75, 20, 10, 5));
         r.Rect("external", LayoutValidation::Bounds(s->b), LayoutRect(75, 20, 10, 5));
         s->Nudge();
         r.AfterRender({s->stage, s->parent, s->a, s->b, s->sibling}, [=](Run& r) { r.Rendered("restored", s->b, LayoutRect(20, 0, 30, 10)); });
       }},
    }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc120)
