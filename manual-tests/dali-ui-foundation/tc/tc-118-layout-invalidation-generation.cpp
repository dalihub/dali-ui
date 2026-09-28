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
struct Generation
{
  std::shared_ptr<Probe> probe;
  View                   stage, root;
  uint32_t               measures{0};
};
} // namespace

class Tc118 : public Case
{
public:
  Tc118()
  : Case("TC118", "Invalidation generation", "Check measure vs arrange-only invalidation scope")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    out.push_back(Steps("K06.generation", "A child measure invalidation reaches its WRAP ancestor, which the next controller pass re-lays out", {
      {"mount", 4, [](Run& r) {
         auto s   = std::make_shared<Generation>();
         s->root  = StackLayout::New(StackOrientation::HORIZONTAL);
         s->probe = Probed(r, 20, 10, false);
         s->root.Add(s->probe->view);
         s->stage = Mount(r, s->root, 100, 50);
         r.SetState(s);
         r.AfterRender({s->stage, s->root, s->probe->view}, [=](Run& r) {
           s->measures = s->probe->measures;
           r.Rendered("probe", s->probe->view, LayoutRect(0, 0, 20, 10));
         });
       }},
      {"invalidate", 7, [](Run& r) {
         auto s          = r.State<Generation>();
         s->probe->width = 30;
         s->probe->view.InvalidateMeasure();
         r.AfterRender({s->stage, s->root, s->probe->view}, [=](Run& r) {
           // The WRAP x WRAP root can only report the new width if the invalidation made it re-measure.
           r.Truth("producer-reran", s->probe->measures > s->measures);
           r.Size("root.measured", s->root.GetMeasuredSize(), MeasuredSize(30, 10));
           r.Rendered("probe.rendered", s->probe->view, LayoutRect(0, 0, 30, 10));
         });
       }},
    }));
    out.push_back(Steps("K07.arrange-only", "Arrange invalidation re-runs only the arrange producer, and a settled arrange cache serves an unchanged re-arrange", {
      {"mount", 4, [](Run& r) {
         auto s   = std::make_shared<Generation>();
         s->probe = Probed(r);
         s->stage = Mount(r, s->probe->view, 100, 50);
         r.SetState(s);
         r.AfterRender({s->stage, s->probe->view}, [=](Run& r) { r.Rendered("probe", s->probe->view, LayoutRect(0, 0, 37, 19)); });
       }},
      {"invalidate-arrange", 2, [](Run& r) {
         auto s = r.State<Generation>();
         s->probe->view.InvalidateArrange();
         r.AfterRender({s->stage, s->probe->view}, [=](Run& r) {
           r.Equal("measure-producer", s->probe->measures, 1);
           r.Equal("arrange-producer", s->probe->arranges, 2);
         });
       }},
      {"repeat", 2, [](Run& r) {
         auto s = r.State<Generation>();
         // The stage re-arranges the probe at the same bounds: the settled cache serves it.
         s->stage.InvalidateArrange();
         r.AfterRender({s->stage, s->probe->view}, [=](Run& r) {
           r.Equal("repeat.measure-producer", s->probe->measures, 1);
           r.Equal("repeat.arrange-producer", s->probe->arranges, 2);
         });
       }},
    }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc118)
