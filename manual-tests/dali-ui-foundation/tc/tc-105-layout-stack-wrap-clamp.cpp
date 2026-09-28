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
struct Stacked
{
  View stage, stack, a, b;
};
} // namespace

class Tc105 : public Case
{
public:
  Tc105()
  : Case("TC105", "Stack wrap and clamp", "Check WRAP_CONTENT StackLayout sizing and clamps")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    for(bool maximum : {false, true})
      out.push_back(Single(maximum ? "S08.maximum" : "S08.minimum", "Measure clamp is distinct from the rendered allocation", 10, [maximum](Run& r) {
        auto s = StackLayout::New(StackOrientation::HORIZONTAL);
        Fixed(s, 200, 60);
        View a = Leaf(maximum ? 160 : 20, 10, "a"), b = Leaf(20, 10, "b");
        a.SetLayoutParams(StackLayoutParams::New().SetWeight(1));
        b.SetLayoutParams(StackLayoutParams::New().SetWeight(1));
        if(maximum)
          a.SetMaximumWidth(60);
        else
          a.SetMinimumWidth(120);
        s.Add(a);
        s.Add(b);
        View stage = Mount(r, s, 200, 60);
        r.AfterRender({stage, s, a, b}, [=](Run& r) {
          r.Size("measured", a.GetMeasuredSize(), MeasuredSize(maximum ? 60 : 120, 10));
          r.Rendered("allocated-a", a, LayoutRect(0, 0, 100, 10));
          r.Rendered("allocated-b", b, LayoutRect(100, 0, 100, 10));
        });
      }));
    out.push_back(Steps("S06.wrap-weight", "Wrap intrinsic fallback, then a minimum width on the wrapping stack", {
      {"wrap", 6, [](Run& r) {
         auto s   = std::make_shared<Stacked>();
         s->stack = StackLayout::New(StackOrientation::HORIZONTAL);
         StackLayout::DownCast(s->stack).SetSpacing(10);
         s->a = Leaf(20, 10, "a");
         s->b = Leaf(30, 15, "b");
         s->b.SetLayoutParams(StackLayoutParams::New().SetWeight(1));
         s->stack.Add(s->a);
         s->stack.Add(s->b);
         s->stage = Mount(r, s->stack, 300, 100);
         r.SetState(s);
         r.AfterRender({s->stage, s->stack, s->a, s->b}, [=](Run& r) {
           r.Size("wrap", s->stack.GetMeasuredSize(), MeasuredSize(60, 15));
           r.Rendered("wrap.rendered", s->stack, LayoutRect(0, 0, 60, 15));
         });
       }},
      {"minimum", 6, [](Run& r) {
         auto s = r.State<Stacked>();
         s->stack.SetMinimumWidth(100);
         r.AfterRender({s->stage, s->stack, s->a, s->b}, [=](Run& r) {
           r.Size("minimum", s->stack.GetMeasuredSize(), MeasuredSize(100, 15));
           r.Rendered("minimum.rendered", s->stack, LayoutRect(0, 0, 100, 15));
         });
       }},
    }));
    out.push_back(Steps("S07.match-overflow", "Unweighted MATCH_PARENT overflows; weighted MATCH_PARENT takes the remaining space", {
      {"unweighted", 4, [](Run& r) {
         auto s   = std::make_shared<Stacked>();
         s->stack = StackLayout::New(StackOrientation::HORIZONTAL);
         Fixed(s->stack, 100, 40);
         s->a = Leaf(20, 10, "a");
         s->b = Leaf(MATCH_PARENT, 10, "b");
         s->stack.Add(s->a);
         s->stack.Add(s->b);
         s->stage = Mount(r, s->stack, 100, 40);
         r.SetState(s);
         r.AfterRender({s->stage, s->stack, s->a, s->b}, [=](Run& r) { r.Rendered("unweighted", s->b, LayoutRect(20, 0, 100, 10)); });
       }},
      {"weighted", 4, [](Run& r) {
         auto s = r.State<Stacked>();
         s->b.SetLayoutParams(StackLayoutParams::New().SetWeight(1));
         r.AfterRender({s->stage, s->stack, s->a, s->b}, [=](Run& r) { r.Rendered("weighted", s->b, LayoutRect(20, 0, 80, 10)); });
       }},
    }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc105)
