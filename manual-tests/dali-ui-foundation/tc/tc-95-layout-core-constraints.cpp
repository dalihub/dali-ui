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
struct Mounted
{
  View stage;
  View view;
};
// Verifies, after the window has rendered, the controller-produced measured size and the
// rendered rectangle (parent-relative, from the last rendered frame) of the mounted view.
void ExpectMounted(Run& r, const char* sizeId, const char* rectId, const MeasuredSize& size, const LayoutRect& rect)
{
  auto s = r.State<Mounted>();
  r.AfterRender({s->stage, s->view}, [=](Run& r) {
    r.Size(sizeId, s->view.GetMeasuredSize(), size);
    r.Rendered(rectId, s->view, rect);
  });
}
void MountView(Run& r, View view, float width, float height)
{
  auto s   = std::make_shared<Mounted>();
  s->view  = view;
  s->stage = Mount(r, view, width, height);
  r.SetState(s);
}
void ResizeStage(Run& r, float width, float height)
{
  auto s = r.State<Mounted>();
  s->stage.SetRequestedWidth(width);
  s->stage.SetRequestedHeight(height);
}
} // namespace

class Tc95 : public Case
{
public:
  Tc95()
  : Case("TC95", "Core constraints", "Check basic sizing: empty, fixed, padding, clamp")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    for(unsigned kind = 0; kind < 5; ++kind)
    {
      out.push_back(Steps(Id("C01.empty", kind), "Empty layout and asymmetric padding rendered under a 200x100 stage", {
        {"empty", 6, [kind](Run& r) {
           MountView(r, Container(kind), 200, 100);
           ExpectMounted(r, "empty", "empty.rendered", MeasuredSize(0, 0), LayoutRect(0, 0, 0, 0));
         }},
        {"padded", 6, [](Run& r) {
           r.State<Mounted>()->view.SetPadding(Insets(3, 5, 7, 11));
           ExpectMounted(r, "padded", "padded.rendered", MeasuredSize(8, 18), LayoutRect(0, 0, 8, 18));
         }},
      }));
      for(float value : {0.0f, 1.0f, 37.5f, 120.0f})
        out.push_back(Single((Id("C02.fixed", kind) + "." + FloatId(value)).c_str(), "Fixed extent and authoritative slot rendered at margin (7,11)", 10, [kind, value](Run& r) {
          View v = Container(kind, value, value);
          v.SetMargin(Insets(7, 0, 11, 0));
          View stage = Mount(r, v, 300, 200);
          r.AfterRender({stage, v}, [=](Run& r) {
            r.Size("measured", v.GetMeasuredSize(), MeasuredSize(value, value));
            r.Rect("arranged", r.Snapshot(v), LayoutRect(7, 11, value, value));
            r.Rendered("rendered", v, LayoutRect(7, 11, value, value));
          });
        }));
      for(float value : {39.0f, 40.0f, 41.0f, 99.0f, 100.0f, 101.0f})
        out.push_back(Single((Id("C07.clamp", kind) + "." + FloatId(value)).c_str(), "Independent min/max clamp of the rendered extent", 6, [kind, value](Run& r) {
          View v = Container(kind, value, value);
          v.SetMinimumWidth(40);
          v.SetMaximumWidth(100);
          v.SetMinimumHeight(40);
          v.SetMaximumHeight(100);
          const float expected = std::min(100.0f, std::max(40.0f, value));
          MountView(r, v, 300, 300);
          ExpectMounted(r, "clamp", "clamp.rendered", MeasuredSize(expected, expected), LayoutRect(0, 0, expected, expected));
        }));
      out.push_back(Steps(Id("C08.equal-bounds", kind), "Equal minimum and maximum under a 1x1 stage, then under a 320x240 stage", {
        {"small-budget", 6, [kind](Run& r) {
           View v = Container(kind, 200, 0);
           v.SetMinimumWidth(60);
           v.SetMaximumWidth(60);
           v.SetMinimumHeight(30);
           v.SetMaximumHeight(30);
           MountView(r, v, 1, 1);
           ExpectMounted(r, "small-budget", "small-budget.rendered", MeasuredSize(60, 30), LayoutRect(0, 0, 60, 30));
         }},
        {"large-budget", 6, [](Run& r) {
           ResizeStage(r, 320, 240);
           ExpectMounted(r, "large-budget", "large-budget.rendered", MeasuredSize(60, 30), LayoutRect(0, 0, 60, 30));
         }},
      }));
    }
    out.push_back(Steps("C03.intrinsic", "Deterministic leaf versus the stage constraint: 100x100, 20x10, then 100x100 again", {
      {"free", 6, [](Run& r) {
         auto p = Probed(r);
         MountView(r, p->view, 100, 100);
         ExpectMounted(r, "free", "free.rendered", MeasuredSize(37, 19), LayoutRect(0, 0, 37, 19));
       }},
      {"limited", 6, [](Run& r) {
         ResizeStage(r, 20, 10);
         ExpectMounted(r, "limited", "limited.rendered", MeasuredSize(20, 10), LayoutRect(0, 0, 20, 10));
       }},
      {"restored", 6, [](Run& r) {
         ResizeStage(r, 100, 100);
         ExpectMounted(r, "restored", "restored.rendered", MeasuredSize(37, 19), LayoutRect(0, 0, 37, 19));
       }},
    }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc95)
