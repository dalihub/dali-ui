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
// A probed leaf next to a sibling whose width can be nudged to force a controller pass
// without touching the probe (so the probe's caches decide whether producers run again).
struct Probed2
{
  std::shared_ptr<Probe> probe;
  View                   stage, sibling;
  uint32_t               measures{0};
  uint32_t               arranges{0};
  float                  nudge{10};
  void                   Nudge()
  {
    sibling.SetRequestedWidth(nudge += 1);
  }
  std::vector<View> Fence() const
  {
    return {stage, probe->view, sibling};
  }
};
std::shared_ptr<Probed2> MountProbe(Run& r, float stageWidth, float stageHeight)
{
  auto s     = std::make_shared<Probed2>();
  s->probe   = Probed(r);
  s->sibling = Leaf(10, 10, "sibling");
  s->sibling.SetRequestedY(60);
  s->stage = r.Stage(stageWidth, stageHeight);
  s->stage.Add(s->probe->view);
  s->stage.Add(s->sibling);
  r.SetState(s);
  return s;
}
} // namespace

class Tc101 : public Case
{
public:
  Tc101()
  : Case("TC101", "Numeric boundaries", "Check epsilon thresholds for cache and re-layout")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    out.push_back(Steps("C20.measure-tolerance", "Sub-epsilon constraint changes hit the measure cache; above-epsilon and normalized constraints behave as specified", {
      {"mount", 4, [](Run& r) {
         auto s = MountProbe(r, 128, 100);
         r.AfterRender(s->Fence(), [=](Run& r) {
           s->measures = s->probe->measures;
           r.Rendered("probe", s->probe->view, LayoutRect(0, 0, 37, 19));
         });
       }},
      {"below-epsilon", 1, [](Run& r) {
         auto s = r.State<Probed2>();
         s->stage.SetRequestedWidth(128.0005f);
         s->Nudge();
         r.AfterRender(s->Fence(), [=](Run& r) { r.Equal("below-epsilon-hit", s->probe->measures, s->measures); });
       }},
      {"above-epsilon", 1, [](Run& r) {
         auto s = r.State<Probed2>();
         s->stage.SetRequestedWidth(128.002f);
         r.AfterRender(s->Fence(), [=](Run& r) {
           r.Equal("above-epsilon-miss", s->probe->measures, s->measures + 1);
           s->measures = s->probe->measures;
         });
       }},
      {"normalized-minimum", 1, [](Run& r) {
         auto s = r.State<Probed2>();
         s->probe->view.SetMinimumWidth(200);
         s->stage.SetRequestedWidth(120);
         r.AfterRender(s->Fence(), [=](Run& r) {
           r.Near("normalized-producer-input", s->probe->lastWidth, 200, 0);
           s->measures = s->probe->measures;
         });
       }},
      {"normalized-hit", 1, [](Run& r) {
         auto s = r.State<Probed2>();
         s->stage.SetRequestedWidth(130);
         r.AfterRender(s->Fence(), [=](Run& r) { r.Equal("normalized-hit", s->probe->measures, s->measures); });
       }},
    }));
    out.push_back(Single("C20.pixel-setter", "Pixel setters preserve sub-epsilon stored values and render the accepted width", 6, [](Run& r) {
      View v = Leaf(128, 19);
      v.SetRequestedWidth(128.0005f);
      r.Near("below-epsilon", v.GetRequestedWidth(), 128, 0);
      v.SetRequestedWidth(128.002f);
      r.Near("above-epsilon", v.GetRequestedWidth(), 128.002f, 0);
      View stage = Mount(r, v, 200, 100);
      r.AfterRender({stage, v}, [=](Run& r) { r.Rendered("rendered", v, LayoutRect(0, 0, 128.002f, 19)); });
    }));
    out.push_back(Steps("C20.exact-params", "A small parameter mode change is not absorbed by an epsilon comparison", {
      {"mount", 4, [](Run& r) {
         auto s = MountProbe(r, 200, 100);
         s->probe->view.SetLayoutParams(StackLayoutParams::New().SetWeight(0));
         r.AfterRender(s->Fence(), [=](Run& r) {
           s->measures = s->probe->measures;
           r.Rendered("probe", s->probe->view, LayoutRect(0, 0, 37, 19));
         });
       }},
      {"mode-change", 3, [](Run& r) {
         auto s = r.State<Probed2>();
         s->probe->view.SetLayoutParams(StackLayoutParams::New().SetWeight(0.0005f));
         r.AfterRender(s->Fence(), [=](Run& r) {
           r.Equal("mode-change-producer", s->probe->measures, s->measures + 1);
           StackLayoutParams params;
           r.Truth("params-present", s->probe->view.TryGetLayoutParams(params));
           r.Near("exact-stored-weight", params.GetWeight(), 0.0005f, 0);
         });
       }},
    }));
    out.push_back(Steps("C20.requested-position-epsilon", "A sub-epsilon requested position is inert; an above-epsilon change re-arranges and renders exactly", {
      {"mount", 4, [](Run& r) {
         auto s = MountProbe(r, 200, 100);
         s->probe->view.SetRequestedX(100);
         r.AfterRender(s->Fence(), [=](Run& r) {
           s->arranges = s->probe->arranges;
           r.Rendered("probe", s->probe->view, LayoutRect(100, 0, 37, 19));
         });
       }},
      {"sub-epsilon", 2, [](Run& r) {
         auto s = r.State<Probed2>();
         // The next float above 100 (100 + 2^-17) lies inside the ranged epsilon the setter
         // applies below 200 (100 * FLT_EPSILON), so the request must be ignored. At 128 one
         // ULP already exceeds that epsilon, so no representable sub-epsilon value exists.
         s->probe->view.SetRequestedX(std::nextafter(100.0f, 200.0f));
         s->Nudge();
         r.AfterRender(s->Fence(), [=](Run& r) {
           r.Equal("unchanged-producer", s->probe->arranges, s->arranges);
           r.Near("exact-position", Run::RenderedBounds(s->probe->view).x, 100);
         });
       }},
      {"above-epsilon", 2, [](Run& r) {
         auto s      = r.State<Probed2>();
         s->arranges = s->probe->arranges; // baseline at this step, independent of the previous verdict
         s->probe->view.SetRequestedX(100.002f);
         r.AfterRender(s->Fence(), [=](Run& r) {
           r.Equal("changed-producer", s->probe->arranges, s->arranges + 1);
           r.Near("moved-position", Run::RenderedBounds(s->probe->view).x, 100.002f);
         });
       }},
    }));
    out.push_back(Steps("C20.first-pass-cache", "The first controller pass runs each producer once; an unrelated pass is served from both caches", {
      {"mount", 6, [](Run& r) {
         auto s = MountProbe(r, 200, 100);
         r.AfterRender(s->Fence(), [=](Run& r) {
           r.Equal("first.measure-producer", s->probe->measures, 1);
           r.Equal("first.arrange-producer", s->probe->arranges, 1);
           r.Rendered("probe", s->probe->view, LayoutRect(0, 0, 37, 19));
         });
       }},
      {"unrelated-pass", 2, [](Run& r) {
         auto s = r.State<Probed2>();
         s->Nudge();
         r.AfterRender(s->Fence(), [=](Run& r) {
           r.Equal("repeat.measure-cache", s->probe->measures, 1);
           r.Equal("repeat.arrange-cache", s->probe->arranges, 1);
         });
       }},
    }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc101)
