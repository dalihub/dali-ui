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
// A probed leaf plus a sibling leaf; nudging the sibling forces a controller pass that
// does not invalidate the probe, so the probe's measure cache decides whether it runs.
struct Cached
{
  std::shared_ptr<Probe> probe, second;
  View                   stage, sibling, stack;
  uint32_t               measures{0}, secondMeasures{0};
  float                  nudge{10};
  void                   Nudge()
  {
    sibling.SetRequestedWidth(nudge += 1);
  }
};
std::shared_ptr<Cached> MountProbe(Run& r, float width, float height)
{
  auto s     = std::make_shared<Cached>();
  s->probe   = Probed(r);
  s->sibling = Leaf(10, 10, "sibling");
  s->sibling.SetRequestedY(60);
  s->stage = r.Stage(width, height);
  s->stage.Add(s->probe->view);
  s->stage.Add(s->sibling);
  r.SetState(s);
  return s;
}
} // namespace

class Tc115 : public Case
{
public:
  Tc115()
  : Case("TC115", "Measure cache", "Check measure cache hits, misses and invalidation")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    out.push_back(Steps("K01.same-key", "Cold miss, identical-key hit through an unrelated pass, and a height-key miss", {
      {"cold", 7, [](Run& r) {
         auto s = MountProbe(r, 100, 50);
         r.AfterRender({s->stage, s->probe->view, s->sibling}, [=](Run& r) {
           r.Size("cold", s->probe->view.GetMeasuredSize(), MeasuredSize(37, 19));
           r.Equal("producer-once", s->probe->measures, 1);
           r.Rendered("cold.rendered", s->probe->view, LayoutRect(0, 0, 37, 19));
         });
       }},
      {"hit", 3, [](Run& r) {
         auto s = r.State<Cached>();
         s->Nudge();
         r.AfterRender({s->stage, s->probe->view, s->sibling}, [=](Run& r) {
           r.Size("hit", s->probe->view.GetMeasuredSize(), MeasuredSize(37, 19));
           r.Equal("hit-producer", s->probe->measures, 1);
         });
       }},
      {"height-key", 1, [](Run& r) {
         auto s = r.State<Cached>();
         s->stage.SetRequestedHeight(51);
         r.AfterRender({s->stage, s->probe->view, s->sibling}, [=](Run& r) { r.Equal("height-key", s->probe->measures, 2); });
       }},
    }));
    out.push_back(Steps("K02.normalized-key", "Different raw constraints normalize to one key", {
      {"mount", 8, [](Run& r) {
         auto s = MountProbe(r, 40, 100);
         s->probe->view.SetMinimumWidth(120);
         s->probe->view.SetMaximumHeight(30);
         r.AfterRender({s->stage, s->probe->view, s->sibling}, [=](Run& r) {
           r.Near("normalized-width", s->probe->lastWidth, 120);
           r.Near("normalized-height", s->probe->lastHeight, 30);
           r.Size("clamped-result", s->probe->view.GetMeasuredSize(), MeasuredSize(120, 19));
           r.Rendered("clamped.rendered", s->probe->view, LayoutRect(0, 0, 120, 19));
         });
       }},
      {"resize", 1, [](Run& r) {
         auto s = r.State<Cached>();
         s->stage.SetRequestedWidth(50);
         s->stage.SetRequestedHeight(200);
         r.AfterRender({s->stage, s->probe->view, s->sibling}, [=](Run& r) { r.Equal("producer-once", s->probe->measures, 1); });
       }},
    }));
    out.push_back(Steps("K03.explicit-invalidation", "External content requires an explicit invalidation to reach the screen", {
      {"mount", 5, [](Run& r) {
         auto s = MountProbe(r, 100, 50);
         r.AfterRender({s->stage, s->probe->view, s->sibling}, [=](Run& r) {
           r.Equal("producer", s->probe->measures, 1);
           r.Rendered("initial", s->probe->view, LayoutRect(0, 0, 37, 19));
         });
       }},
      {"stale", 3, [](Run& r) {
         auto s          = r.State<Cached>();
         s->probe->width = 53;
         s->Nudge();
         r.AfterRender({s->stage, s->probe->view, s->sibling}, [=](Run& r) {
           r.Size("cached", s->probe->view.GetMeasuredSize(), MeasuredSize(37, 19));
           r.Equal("unchanged-producer", s->probe->measures, 1);
         });
       }},
      {"invalidated", 7, [](Run& r) {
         auto s = r.State<Cached>();
         s->probe->view.InvalidateMeasure();
         r.AfterRender({s->stage, s->probe->view, s->sibling}, [=](Run& r) {
           r.Size("invalidated", s->probe->view.GetMeasuredSize(), MeasuredSize(53, 19));
           r.Equal("fresh-producer", s->probe->measures, 2);
           r.Rendered("invalidated.rendered", s->probe->view, LayoutRect(0, 0, 53, 19));
         });
       }},
      {"fresh-cache", 1, [](Run& r) {
         auto s = r.State<Cached>();
         s->Nudge();
         r.AfterRender({s->stage, s->probe->view, s->sibling}, [=](Run& r) { r.Equal("fresh-cache", s->probe->measures, 2); });
       }},
    }));
    out.push_back(Steps("K04.sibling-isolation", "One dirty child preserves the clean sibling cache", {
      {"mount", 4, [](Run& r) {
         auto s    = std::make_shared<Cached>();
         s->stack  = StackLayout::New(StackOrientation::HORIZONTAL);
         s->probe  = Probed(r, 20, 10, false);
         s->second = Probed(r, 30, 10, false);
         s->stack.Add(s->probe->view);
         s->stack.Add(s->second->view);
         s->stage = Mount(r, s->stack, 200, 100);
         r.SetState(s);
         r.AfterRender({s->stage, s->stack, s->probe->view, s->second->view}, [=](Run& r) {
           s->measures       = s->probe->measures;
           s->secondMeasures = s->second->measures;
           r.Rendered("root", s->stack, LayoutRect(0, 0, 50, 10));
         });
       }},
      {"dirty-a", 8, [](Run& r) {
         auto s          = r.State<Cached>();
         s->probe->width = 40;
         s->probe->view.InvalidateMeasure();
         r.AfterRender({s->stage, s->stack, s->probe->view, s->second->view}, [=](Run& r) {
           r.Size("root", s->stack.GetMeasuredSize(), MeasuredSize(70, 10));
           r.Equal("dirty-producer", s->probe->measures, s->measures + 1);
           r.Equal("clean-producer", s->second->measures, s->secondMeasures);
           r.Rendered("root.rendered", s->stack, LayoutRect(0, 0, 70, 10));
         });
       }},
    }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc115)
