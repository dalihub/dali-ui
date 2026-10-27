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
struct FillMaximum
{
  View stage, stack, a, b;
};
} // namespace

class Tc104 : public Case
{
public:
  Tc104()
  : Case("TC104", "Stack allocation", "Check StackLayout spacing, weights and alignment")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    for(bool vertical : {false, true})
    {
      out.push_back(Single(vertical ? "S01.vertical" : "S01.horizontal", "Stack order, gaps and weighted allocation rendered on screen", 12, [vertical](Run& r) {
        auto s = StackLayout::New(vertical ? StackOrientation::VERTICAL : StackOrientation::HORIZONTAL);
        s.SetSpacing(10);
        View a = Leaf(vertical ? 10 : 60, vertical ? 60 : 10, "a"), b = Leaf(20, 20, "b"), c = Leaf(30, 30, "c");
        b.SetLayoutParams(StackLayoutParams::New().SetWeight(1));
        c.SetLayoutParams(StackLayoutParams::New().SetWeight(2));
        s.Add(a);
        s.Add(b);
        s.Add(c);
        const float width = vertical ? 80 : 300, height = vertical ? 300 : 80;
        Fixed(s, width, height);
        View stage = Mount(r, s, width, height);
        r.AfterRender({stage, s, a, b, c}, [=](Run& r) {
          LayoutRect ea(0, 0, 60, 10), eb(70, 0, 220.0f / 3, 20), ec(80 + 220.0f / 3, 0, 440.0f / 3, 30);
          r.Rendered("fixed", a, vertical ? SwapAxes(ea) : ea);
          r.Rendered("weight1", b, vertical ? SwapAxes(eb) : eb);
          r.Rendered("weight2", c, vertical ? SwapAxes(ec) : ec);
        });
      }));
    }
    for(unsigned alignment = 0; alignment < 4; ++alignment)
      out.push_back(Single(Id("S04.cross", alignment).c_str(), "Cross alignment with an intrinsic child", 4, [alignment](Run& r) {
        auto s = StackLayout::New(StackOrientation::HORIZONTAL);
        Fixed(s, 100, 60);
        auto                  p        = Probed(r, 20, 10, false);
        const LayoutAlignment aligns[] = {LayoutAlignment::START, LayoutAlignment::CENTER, LayoutAlignment::END, LayoutAlignment::FILL};
        p->view.SetLayoutParams(StackLayoutParams::New().SetAlignment(aligns[alignment]));
        s.Add(p->view);
        View stage = Mount(r, s, 100, 60);
        r.AfterRender({stage, s, p->view}, [=](Run& r) {
          r.Rendered("aligned", p->view, LayoutRect(0, alignment == 1 ? 25 : alignment == 2 ? 50 : 0, 20, alignment == 3 ? 60 : 10));
        });
      }));
    out.push_back(Single("S04.fill-fixed-child", "FILL does not expand a child with a fixed cross size", 8, [](Run& r) {
      auto s = StackLayout::New(StackOrientation::HORIZONTAL);
      Fixed(s, 100, 60);
      View fixed = Leaf(20, 10, "fixed"), wrap = View::New();
      wrap.SetRequestedWidth(30);
      wrap.SetRequestedHeight(WRAP_CONTENT);
      wrap.SetMinimumHeight(5);
      wrap.SetProperty(Actor::Property::NAME, Dali::String("wrap"));
      wrap.SetBackgroundColor(Palette(5));
      fixed.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
      wrap.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
      s.Add(fixed);
      s.Add(wrap);
      View stage = Mount(r, s, 100, 60);
      r.AfterRender({stage, s, fixed, wrap}, [=](Run& r) {
        r.Rendered("fixed-keeps-size", fixed, LayoutRect(0, 0, 20, 10));
        r.Rendered("wrap-fills", wrap, LayoutRect(20, 0, 30, 60));
      });
    }));
    out.push_back(Single("S05.margin-gap-overflow", "Margins and negative free space", 8, [](Run& r) {
      auto s = StackLayout::New(StackOrientation::HORIZONTAL);
      Fixed(s, 40, 50);
      s.SetSpacing(7);
      View a = Leaf(30, 10, "a"), b = Leaf(20, 15, "b");
      a.SetMargin(Insets(2, 3, 4, 5));
      s.Add(a);
      s.Add(b);
      View stage = Mount(r, s, 40, 50);
      r.AfterRender({stage, s, a, b}, [=](Run& r) {
        r.Rendered("margin", a, LayoutRect(2, 4, 30, 10));
        r.Rendered("overflow", b, LayoutRect(42, 0, 20, 15));
      });
    }));
    // FILL on the determined 300 cross extent measures the WRAP_CONTENT child EXACTLY at 300, which
    // its maximum clamps to 120; the 300-wide slot is clamped from its start edge (x = 0), and under
    // RTL from its right edge (x = 300 - 120 = 180).
    out.push_back(Steps("S04.fill-maximum", "A FILL child is laid out at its maximum from the slot's start edge, the right edge under RTL", {
      {"ltr", 10, [](Run& r) {
         auto s   = std::make_shared<FillMaximum>();
         s->stack = StackLayout::New(StackOrientation::VERTICAL);
         Fixed(s->stack, 300, 100);
         s->a = View::New();
         s->a.SetRequestedWidth(WRAP_CONTENT);
         s->a.SetRequestedHeight(20);
         s->a.SetMaximumWidth(120);
         s->a.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
         s->b = Leaf(50, 10, "b");
         s->stack.Add(s->a);
         s->stack.Add(s->b);
         s->stage = Mount(r, s->stack, 300, 100);
         r.SetState(s);
         r.AfterRender({s->stage, s->stack, s->a, s->b}, [=](Run& r) {
           r.Size("a", s->a.GetMeasuredSize(), MeasuredSize(120, 20));
           r.Rendered("a.rendered", s->a, LayoutRect(0, 0, 120, 20));
           r.Rendered("b.rendered", s->b, LayoutRect(0, 20, 50, 10));
         });
       }},
      {"rtl", 12, [](Run& r) {
         auto s = r.State<FillMaximum>();
         s->stack.SetLayoutDirection(Dali::LayoutDirection::RIGHT_TO_LEFT);
         r.AfterRender({s->stage, s->stack, s->a, s->b}, [=](Run& r) {
           r.Rendered("a.rtl", s->a, LayoutRect(180, 0, 120, 20));
           r.Rect("a.logical", ArrangedBounds(s->a), LayoutRect(0, 0, 120, 20));
           r.Rendered("b.rtl", s->b, LayoutRect(250, 20, 50, 10));
         });
       }},
    }));
    out.push_back(Single("S11.match-report", "A MATCH_PARENT cross axis reports the stack's cross extent; a MATCH_PARENT main axis without weight reports its minimum", 12, [](Run& r) {
      auto s = StackLayout::New(StackOrientation::VERTICAL);
      Fixed(s, 300, 200);
      View c = Leaf(MATCH_PARENT, 20, "c"), d = Leaf(MATCH_PARENT, MATCH_PARENT, "d");
      c.SetMargin(Insets(10, 20, 5, 0));
      s.Add(c);
      s.Add(d);
      View stage = Mount(r, s, 300, 200);
      // c: cross EXACTLY at 300 - 10 - 20 = 270; d: cross EXACTLY at 300, main AT_MOST (minimum 0),
      // laid out at the available 200 below c (y = 5 + 20), overflowing the stack.
      r.AfterRender({stage, s, c, d}, [=](Run& r) {
        r.Size("c", c.GetMeasuredSize(), MeasuredSize(270, 20));
        r.Rendered("c.rendered", c, LayoutRect(10, 5, 270, 20));
        r.Size("d", d.GetMeasuredSize(), MeasuredSize(300, 0));
        r.Rendered("d.rendered", d, LayoutRect(0, 25, 300, 200));
      });
    }));
    out.push_back(Single("S12.weight-exact-share", "A weight child is measured exactly at its share in Measure", 7, [](Run& r) {
      auto s = StackLayout::New(StackOrientation::HORIZONTAL);
      Fixed(s, 300, 80);
      s.SetSpacing(10);
      View a = Leaf(60, 10, "a");
      auto b = Probed(r, 37, 19, false);
      b->view.SetLayoutParams(StackLayoutParams::New().SetWeight(1));
      s.Add(a);
      s.Add(b->view);
      View stage = Mount(r, s, 300, 80);
      // Share = 300 - 60 - 10 = 230 (main EXACTLY); the cross axis keeps the probe's 19.
      r.AfterRender({stage, s, a, b->view}, [=](Run& r) {
        r.Size("b", b->view.GetMeasuredSize(), MeasuredSize(230, 19));
        r.Rendered("b.rendered", b->view, LayoutRect(70, 0, 230, 19));
        r.Near("b.last-width", b->lastWidth, 230);
      });
    }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc104)
