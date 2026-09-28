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
struct Tracked
{
  std::shared_ptr<Probe> probe;
  View                   stage, sibling;
  uint32_t               measures{0};
  float                  nudge{10};
};
struct Hidden
{
  View stage, stack, a, b;
};
} // namespace

class Tc100 : public Case
{
public:
  Tc100()
  : Case("TC100", "Tracked setters", "Check only changed setters trigger a re-layout")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario>                        out;
    const std::vector<std::function<void(View)>> changes{
      [](View v) { v.SetRequestedWidth(80); },
      [](View v) { v.SetPadding(Insets(2, 3, 4, 5)); },
      [](View v) { v.SetMargin(Insets(1, 2, 3, 4)); },
      [](View v) { v.SetMinimumWidth(5); },
      [](View v) { v.SetLayoutParams(StackLayoutParams::New().SetWeight(2)); },
      [](View v) { v.SetRequestedHeight(60); },
      [](View v) { v.SetMinimumHeight(5); },
      [](View v) { v.SetMaximumWidth(100); },
      [](View v) { v.SetMaximumHeight(80); },
      [](View v) { v.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(2)); },
      [](View v) { v.SetLayoutParams(GridLayoutParams::New().SetRowSpan(2)); },
      [](View v) { v.SetLayoutParams(AbsoluteLayoutParams::New().SetX(7)); }};
    for(unsigned i = 0; i < changes.size(); ++i)
      out.push_back(Steps(Id("K05.tracked", i), "A changed setter re-runs the producer through the controller; the identical value is inert", {
        {"mount", 5, [](Run& r) {
           auto s     = std::make_shared<Tracked>();
           s->probe   = Probed(r);
           s->sibling = Leaf(10, 10, "sibling");
           s->sibling.SetRequestedY(60);
           s->stage = r.Stage(200, 100);
           s->stage.Add(s->probe->view);
           s->stage.Add(s->sibling);
           r.SetState(s);
           r.AfterRender({s->stage, s->probe->view, s->sibling}, [=](Run& r) {
             s->measures = s->probe->measures;
             r.Truth("mounted-producer", s->measures >= 1);
             r.Rendered("probe", s->probe->view, LayoutRect(0, 0, 37, 19));
           });
         }},
        {"changed", 2, [change = changes[i]](Run& r) {
           auto s = r.State<Tracked>();
           change(s->probe->view);
           r.AfterRender({s->stage, s->probe->view, s->sibling}, [=](Run& r) {
             r.Equal("changed-producer", s->probe->measures, s->measures + 1);
             r.Near("measured-width", s->probe->view.GetMeasuredSize().width, 37);
             s->measures = s->probe->measures;
           });
         }},
        {"same", 1, [change = changes[i]](Run& r) {
           auto s = r.State<Tracked>();
           change(s->probe->view);
           s->sibling.SetRequestedWidth(s->nudge += 1);
           r.AfterRender({s->stage, s->probe->view, s->sibling}, [=](Run& r) { r.Equal("same-producer", s->probe->measures, s->measures); });
         }},
      }));
    out.push_back(Steps("C19.hidden-participates", "Visibility does not collapse layout participation on screen", {
      {"visible", 8, [](Run& r) {
         auto s   = std::make_shared<Hidden>();
         s->stack = StackLayout::New(StackOrientation::HORIZONTAL);
         Fixed(s->stack, 200, 50);
         StackLayout::DownCast(s->stack).SetSpacing(7);
         s->a = Leaf(20, 10, "a");
         s->b = Leaf(30, 15, "b");
         s->stack.Add(s->a);
         s->stack.Add(s->b);
         s->stage = Mount(r, s->stack, 200, 50);
         r.SetState(s);
         r.AfterRender({s->stage, s->stack, s->a, s->b}, [=](Run& r) {
           r.Rendered("visible-a", s->a, LayoutRect(0, 0, 20, 10));
           r.Rendered("visible-b", s->b, LayoutRect(27, 0, 30, 15));
         });
       }},
      {"hidden", 8, [](Run& r) {
         auto s = r.State<Hidden>();
         s->a.SetVisible(false);
         s->stack.InvalidateMeasure();
         r.AfterRender({s->stage, s->stack, s->a, s->b}, [=](Run& r) {
           r.Rendered("hidden-a", s->a, LayoutRect(0, 0, 20, 10));
           r.Rendered("hidden-b", s->b, LayoutRect(27, 0, 30, 15));
         });
       }},
    }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc100)
