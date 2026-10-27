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
      out.push_back(Single(maximum ? "S08.maximum" : "S08.minimum", "Own min/max clamps the weighted slot at its start edge; the sibling keeps its share", 10, [maximum](Run& r) {
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
          r.Rendered("allocated-a", a, LayoutRect(0, 0, maximum ? 60 : 120, 10));
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
    out.push_back(Single("S15.clamped-container", "A stack laid out inside its own maximum places its children in the clamped bounds", 16, [](Run& r) {
      auto s = StackLayout::New(StackOrientation::VERTICAL);
      s.SetRequestedWidth(MATCH_PARENT);
      s.SetRequestedHeight(MATCH_PARENT);
      s.SetMaximumWidth(120);
      View g = Leaf(MATCH_PARENT, 30, "g"), h = Leaf(50, 40, "h");
      h.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::END));
      s.Add(g);
      s.Add(h);
      View stage = Mount(r, s, 300, 200);
      r.AfterRender({stage, s, g, h}, [=](Run& r) {
        r.Rendered("s", s, LayoutRect(0, 0, 120, 200));
        r.Size("s.measured", s.GetMeasuredSize(), MeasuredSize(120, 200));
        r.Rendered("g", g, LayoutRect(0, 0, 120, 30));
        r.Size("g.measured", g.GetMeasuredSize(), MeasuredSize(120, 30));
        r.Rendered("h", h, LayoutRect(70, 30, 50, 40));
      });
    }));
    out.push_back(Single("S16.weighted-wrap-exact", "A stretched wrapping stack distributes its weights in Measure", 14, [](Run& r) {
      auto outer = StackLayout::New(StackOrientation::VERTICAL);
      Fixed(outer, 200, 100);
      auto inner = StackLayout::New(StackOrientation::HORIZONTAL);
      inner.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
      View a = Leaf(120, 10, "a");
      auto f = FlexLayout::New();
      f.SetWrap(FlexWrap::WRAP);
      f.SetAlignItems(FlexAlign::FLEX_START);
      f.SetAlignContent(FlexAlign::FLEX_START);
      f.SetLayoutParams(StackLayoutParams::New().SetWeight(1));
      View first = Leaf(40, 10, "f0"), second = Leaf(40, 10, "f1"), third = Leaf(40, 10, "f2");
      f.Add(first);
      f.Add(second);
      f.Add(third);
      inner.Add(a);
      inner.Add(f);
      outer.Add(inner);
      View stage = Mount(r, outer, 200, 100);
      r.AfterRender({stage, outer, inner, a, f, third}, [=](Run& r) {
        r.Size("inner", inner.GetMeasuredSize(), MeasuredSize(200, 20));
        r.Rendered("inner.rendered", inner, LayoutRect(0, 0, 200, 20));
        r.Rendered("f.rendered", f, LayoutRect(120, 0, 80, 20));
        r.Rendered("f.third", third, LayoutRect(0, 10, 40, 10));
      });
    }));
    out.push_back(Single("S17.two-step-match", "A MATCH_PARENT cross child of a wrapping stack is re-measured exactly at the resolved cross extent", 12, [](Run& r) {
      auto outer = StackLayout::New(StackOrientation::VERTICAL);
      View seed  = Leaf(100, 0, "seed");
      auto wf    = FlexLayout::New();
      wf.SetRequestedWidth(MATCH_PARENT);
      wf.SetWrap(FlexWrap::WRAP);
      wf.SetAlignItems(FlexAlign::FLEX_START);
      wf.SetAlignContent(FlexAlign::FLEX_START);
      for(const char* name : {"w0", "w1", "w2"}) wf.Add(Leaf(40, 10, name));
      outer.Add(seed);
      outer.Add(wf);
      View stage = Mount(r, outer, 200, 100);
      r.AfterRender({stage, outer, seed, wf}, [=](Run& r) {
        r.Size("outer", outer.GetMeasuredSize(), MeasuredSize(100, 20));
        r.Rendered("outer.rendered", outer, LayoutRect(0, 0, 100, 20));
        r.Size("wf", wf.GetMeasuredSize(), MeasuredSize(100, 20));
        r.Rendered("wf.rendered", wf, LayoutRect(0, 0, 100, 20));
      });
    }));
    out.push_back(Single("S17.two-step-stretched", "A stretched wrapping child whose content follows its width is re-measured exactly at the resolved cross extent", 16, [](Run& r) {
      auto outer = StackLayout::New(StackOrientation::VERTICAL);
      View seed  = Leaf(100, 0, "seed");
      auto c     = StackLayout::New(StackOrientation::VERTICAL);
      c.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
      auto wf = FlexLayout::New();
      wf.SetRequestedWidth(MATCH_PARENT);
      wf.SetWrap(FlexWrap::WRAP);
      wf.SetAlignItems(FlexAlign::FLEX_START);
      wf.SetAlignContent(FlexAlign::FLEX_START);
      for(const char* name : {"w0", "w1", "w2"}) wf.Add(Leaf(40, 10, name));
      c.Add(wf);
      outer.Add(seed);
      outer.Add(c);
      View stage = Mount(r, outer, 200, 100);
      r.AfterRender({stage, outer, seed, c, wf}, [=](Run& r) {
        r.Size("outer", outer.GetMeasuredSize(), MeasuredSize(100, 20));
        r.Rendered("outer.rendered", outer, LayoutRect(0, 0, 100, 20));
        r.Size("c", c.GetMeasuredSize(), MeasuredSize(100, 20));
        r.Rendered("c.rendered", c, LayoutRect(0, 0, 100, 20));
        r.Rendered("wf.rendered", wf, LayoutRect(0, 0, 100, 20));
      });
    }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc105)
