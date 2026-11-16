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
struct Absolute
{
  View           stage;
  AbsoluteLayout layout;
  View           child;
};
} // namespace

class Tc113 : public Case
{
public:
  Tc113()
  : Case("TC113", "Absolute proportions", "Check AbsoluteLayout proportional position/size")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    for(unsigned bits = 0; bits < 16; ++bits)
      out.push_back(Single(Id("A04.flags", bits).c_str(), "Independent proportional flags", 4, [bits](Run& r) {
        auto a = AbsoluteLayout::New();
        Fixed(a, 200, 100);
        View c = Leaf(10, 10, "child");
        c.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(0.25f, 0.5f, 0.5f, 0.25f)).SetFlags(static_cast<AbsoluteLayoutFlags>(bits)));
        a.Add(c);
        View stage = Mount(r, a, 200, 100);
        r.AfterRender({stage, a, c}, [=](Run& r) {
          const float w = bits & 4 ? 100 : 0.5f, h = bits & 8 ? 25 : 0.25f, x = bits & 1 ? (200 - w) * 0.25f : 0.25f, y = bits & 2 ? (100 - h) * 0.5f : 0.5f;
          r.Rendered("flags", c, LayoutRect(x, y, w, h));
        });
      }));
    for(float position : {-0.25f, 0.0f, 0.5f, 1.0f, 1.25f})
      out.push_back(Steps("A08.position." + FloatId(position), "Proportional positions outside the unit interval, mirrored on screen under RTL", {
        {"ltr", 4, [position](Run& r) {
           auto s    = std::make_shared<Absolute>();
           s->layout = AbsoluteLayout::New();
           Fixed(s->layout, 300, 100);
           s->child = Leaf(100, 20, "child");
           s->child.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(position, 0, 100, 20)).SetFlags(AbsoluteLayoutFlags::X_PROPORTIONAL));
           s->layout.Add(s->child);
           s->stage = Mount(r, s->layout, 300, 100);
           r.SetState(s);
           r.AfterRender({s->stage, s->layout, s->child}, [=](Run& r) { r.Rendered("ltr", s->child, LayoutRect(200 * position, 0, 100, 20)); });
         }},
        {"rtl", 8, [position](Run& r) {
           auto s = r.State<Absolute>();
           s->layout.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
           r.AfterRender({s->stage, s->layout, s->child}, [=](Run& r) {
             r.Rect("rtl.reported", r.Snapshot(s->child), LayoutRect(200 - 200 * position, 0, 100, 20));
             r.Rendered("rtl", s->child, LayoutRect(200 - 200 * position, 0, 100, 20));
           });
         }},
      }));
    out.push_back(Single("A05.all-flags", "ALL includes all known proportional dimensions", 4, [](Run& r) {
      auto a = AbsoluteLayout::New();
      Fixed(a, 200, 100);
      View c = Leaf(10, 10, "child");
      c.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(0.5f, 0.5f, 0.5f, 0.5f)).SetFlags(AbsoluteLayoutFlags::ALL));
      a.Add(c);
      View stage = Mount(r, a, 200, 100);
      r.AfterRender({stage, a, c}, [=](Run& r) { r.Rendered("all", c, LayoutRect(50, 25, 100, 50)); });
    }));
    out.push_back(Steps("A06.wrap-circularity", "Wrap keeps the determinate extent and drops only the circular contribution", {
      {"position-only", 6, [](Run& r) {
         auto s    = std::make_shared<Absolute>();
         s->layout = AbsoluteLayout::New();
         s->child  = Leaf(40, 20, "child");
         s->child.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(0.5f, 0, 40, 20)).SetFlags(AbsoluteLayoutFlags::X_PROPORTIONAL));
         s->layout.Add(s->child);
         s->stage = Mount(r, s->layout, 200, 100);
         r.SetState(s);
         r.AfterRender({s->stage, s->layout, s->child}, [=](Run& r) {
           r.Size("position-only", s->layout.GetMeasuredSize(), MeasuredSize(40, 20));
           r.Rendered("position-only.rendered", s->layout, LayoutRect(0, 0, 40, 20));
         });
       }},
      {"size-circular", 6, [](Run& r) {
         auto s = r.State<Absolute>();
         s->child.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(0, 0, 0.5f, 20)).SetFlags(AbsoluteLayoutFlags::WIDTH_PROPORTIONAL));
         r.AfterRender({s->stage, s->layout, s->child}, [=](Run& r) {
           r.Size("size-circular", s->layout.GetMeasuredSize(), MeasuredSize(0, 20));
           r.Rendered("size-circular.rendered", s->layout, LayoutRect(0, 0, 0, 20));
         });
       }},
    }));
    // A proportional width 0.5 x 300 = 150 is assigned EXACTLY and clamped by the maximum 100, from the
    // slot's start edge. The proportional x of b is resolved from the unclamped extent:
    // (300 - 150) x 1 = 150.
    out.push_back(Single("A10.proportional-maximum", "A proportional slot is clamped by the child's maximum; a proportional position keeps the unclamped extent", 12, [](Run& r) {
      auto layout = AbsoluteLayout::New();
      Fixed(layout, 300, 200);
      View a = View::New(), b = View::New();
      a.SetRequestedHeight(20);
      b.SetRequestedHeight(20);
      a.SetMaximumWidth(100);
      b.SetMaximumWidth(100);
      a.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(0, 0, 0.5f, -1)).SetFlags(AbsoluteLayoutFlags::WIDTH_PROPORTIONAL));
      b.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(1, 30, 0.5f, -1)).SetFlags(AbsoluteLayoutFlags::WIDTH_PROPORTIONAL | AbsoluteLayoutFlags::X_PROPORTIONAL));
      layout.Add(a);
      layout.Add(b);
      View stage = Mount(r, layout, 300, 200);
      r.AfterRender({stage, layout, a, b}, [=](Run& r) {
        r.Size("a", a.GetMeasuredSize(), MeasuredSize(100, 20));
        r.Rendered("a.rendered", a, LayoutRect(0, 0, 100, 20));
        r.Size("b", b.GetMeasuredSize(), MeasuredSize(100, 20));
        r.Rendered("b.rendered", b, LayoutRect(150, 30, 100, 20));
      });
    }));
    // A wrapping layout resolves its width from its other children (b: 100) first and only then
    // measures the proportional child at 0.5 x 100 = 50, exactly.
    out.push_back(Single("A11.proportional-last", "A wrapping layout measures a proportional child after its own width is resolved", 9, [](Run& r) {
      auto layout = AbsoluteLayout::New();
      auto a      = Probed(r, 37, 19, false);
      a->view.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(0, 0, 0.5f, -1)).SetFlags(AbsoluteLayoutFlags::WIDTH_PROPORTIONAL));
      View b = Leaf(100, 10, "b");
      layout.Add(a->view);
      layout.Add(b);
      View stage = Mount(r, layout, 200, 100);
      r.AfterRender({stage, layout, a->view, b}, [=](Run& r) {
        r.Near("a.last-width", a->lastWidth, 50);
        r.Size("a", a->view.GetMeasuredSize(), MeasuredSize(50, 19));
        r.Rendered("a.rendered", a->view, LayoutRect(0, 0, 50, 19));
        r.Size("layout", layout.GetMeasuredSize(), MeasuredSize(100, 19));
      });
    }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc113)
