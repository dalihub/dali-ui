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
struct Tree
{
  View stage, p, a, b;
};
struct TwoStep
{
  View                   stage, p, seed, m;
  std::shared_ptr<Probe> inner;
};
} // namespace

class Tc97 : public Case
{
public:
  Tc97()
  : Case("TC97", "Default View", "Check default View wrap and match-parent sizing")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    out.push_back(Steps("C10.C12.local-coordinate", "Parent offset is not duplicated into the rendered child rectangle", {
      {"mount", 8, [](Run& r) {
         auto s = std::make_shared<Tree>();
         s->p   = Leaf(200, 150);
         s->a   = Leaf(37, 19);
         s->p.SetPadding(Insets(3, 5, 7, 11));
         s->p.SetMargin(Insets(70, 0, 90, 0));
         s->a.SetMargin(Insets(2, 4, 6, 8));
         s->a.SetRequestedX(11);
         s->a.SetRequestedY(13);
         s->p.Add(s->a);
         s->stage = Mount(r, s->p, 300, 270);
         r.SetState(s);
         r.AfterRender({s->stage, s->p, s->a}, [=](Run& r) {
           r.Rendered("parent", s->p, LayoutRect(70, 90, 200, 150));
           r.Rendered("child", s->a, LayoutRect(16, 26, 37, 19));
         });
       }},
      {"move-parent", 8, [](Run& r) {
         auto s = r.State<Tree>();
         s->p.SetMargin(Insets(100, 0, 120, 0));
         r.AfterRender({s->stage, s->p, s->a}, [=](Run& r) {
           r.Rendered("parent-moved", s->p, LayoutRect(100, 120, 200, 150));
           r.Rendered("child-after-parent-move", s->a, LayoutRect(16, 26, 37, 19));
         });
       }},
    }));
    out.push_back(Steps("C04.wrap-bounds", "Default View wraps the child bounding extent on screen", {
      {"mount", 6, [](Run& r) {
         auto s = std::make_shared<Tree>();
         s->p   = View::New();
         s->a   = Leaf(20, 10);
         s->b   = Leaf(30, 15);
         s->a.SetRequestedX(5);
         s->a.SetRequestedY(7);
         s->b.SetRequestedX(40);
         s->b.SetRequestedY(20);
         s->p.Add(s->a);
         s->p.Add(s->b);
         s->stage = Mount(r, s->p, 200, 100);
         r.SetState(s);
         r.AfterRender({s->stage, s->p, s->a, s->b}, [=](Run& r) {
           r.Size("wrap", s->p.GetMeasuredSize(), MeasuredSize(70, 35));
           r.Rendered("wrap.rendered", s->p, LayoutRect(0, 0, 70, 35));
         });
       }},
      {"widen-b", 10, [](Run& r) {
         auto s = r.State<Tree>();
         s->b.SetRequestedWidth(40);
         r.AfterRender({s->stage, s->p, s->a, s->b}, [=](Run& r) {
           r.Size("changed", s->p.GetMeasuredSize(), MeasuredSize(80, 35));
           r.Rendered("changed.rendered", s->p, LayoutRect(0, 0, 80, 35));
           r.Rendered("b", s->b, LayoutRect(40, 20, 40, 15));
         });
       }},
    }));
    out.push_back(Single("C05.C11.match-budget", "Match parent fills the padding and margin adjusted slot on screen", 6, [](Run& r) {
      View p = Leaf(100, 60), c = Leaf(MATCH_PARENT, MATCH_PARENT);
      p.SetPadding(Insets(3, 5, 7, 11));
      c.SetMargin(Insets(2, 4, 6, 8));
      p.Add(c);
      View stage = Mount(r, p, 100, 60);
      r.AfterRender({stage, p, c}, [=](Run& r) {
        r.Size("exact-slot", c.GetMeasuredSize(), MeasuredSize(86, 28));
        r.Rendered("match-slot", c, LayoutRect(5, 13, 86, 28));
      });
    }));
    out.push_back(Steps("C06.wrap-match", "A match child contributes its minimum to a wrapping parent", {
      {"mount", 6, [](Run& r) {
         auto s = std::make_shared<Tree>();
         s->p   = View::New();
         s->a   = Leaf(MATCH_PARENT, MATCH_PARENT);
         s->a.SetMinimumWidth(30);
         s->a.SetMinimumHeight(20);
         s->p.Add(s->a);
         s->stage = Mount(r, s->p, 200, 100);
         r.SetState(s);
         r.AfterRender({s->stage, s->p, s->a}, [=](Run& r) {
           r.Size("wrap-minimum", s->p.GetMeasuredSize(), MeasuredSize(30, 20));
           r.Rendered("wrap-minimum.rendered", s->p, LayoutRect(0, 0, 30, 20));
         });
       }},
      {"reduce-minimum", 6, [](Run& r) {
         auto s = r.State<Tree>();
         s->a.SetMinimumWidth(10);
         r.AfterRender({s->stage, s->p, s->a}, [=](Run& r) {
           r.Size("reduced-minimum", s->p.GetMeasuredSize(), MeasuredSize(10, 20));
           r.Rendered("reduced-minimum.rendered", s->p, LayoutRect(0, 0, 10, 20));
         });
       }},
    }));
    out.push_back(Single("C11.exhausted-padding", "Padding and margin exhaust the content budget", 6, [](Run& r) {
      View parent = Leaf(10, 10), child = Leaf(MATCH_PARENT, MATCH_PARENT);
      parent.SetPadding(Insets(8, 8, 8, 8));
      child.SetMargin(Insets(3, 3, 3, 3));
      parent.Add(child);
      View stage = Mount(r, parent, 10, 10);
      r.AfterRender({stage, parent, child}, [=](Run& r) {
        r.Size("zero-measure", child.GetMeasuredSize(), MeasuredSize(0, 0));
        r.Rendered("zero-content", child, LayoutRect(11, 11, 0, 0));
      });
    }));
    out.push_back(Steps("C06.two-step", "A MATCH_PARENT child of a wrapping parent is re-measured exactly at the width the parent resolves", {
      {"mount", 16, [](Run& r) {
         auto s   = std::make_shared<TwoStep>();
         s->p     = View::New();
         s->seed  = Leaf(30, 0, "seed");
         s->m     = View::New();
         s->m.SetRequestedWidth(MATCH_PARENT);
         s->inner           = Probed(r);
         s->inner->wrapping = true;
         s->m.Add(s->inner->view);
         s->p.Add(s->seed);
         s->p.Add(s->m);
         s->stage = Mount(r, s->p, 200, 100);
         r.SetState(s);
         r.AfterRender({s->stage, s->p, s->seed, s->m, s->inner->view}, [=](Run& r) {
           r.Size("p", s->p.GetMeasuredSize(), MeasuredSize(30, 38));
           r.Rendered("p.rendered", s->p, LayoutRect(0, 0, 30, 38));
           r.Size("m", s->m.GetMeasuredSize(), MeasuredSize(30, 38));
           r.Rendered("m.rendered", s->m, LayoutRect(0, 0, 30, 38));
           r.Rendered("inner.rendered", s->inner->view, LayoutRect(0, 0, 30, 38));
         });
       }},
      {"widen-seed", 16, [](Run& r) {
         auto s = r.State<TwoStep>();
         s->seed.SetRequestedWidth(50);
         r.AfterRender({s->stage, s->p, s->seed, s->m, s->inner->view}, [=](Run& r) {
           r.Size("p", s->p.GetMeasuredSize(), MeasuredSize(50, 19));
           r.Rendered("p.rendered", s->p, LayoutRect(0, 0, 50, 19));
           r.Size("m", s->m.GetMeasuredSize(), MeasuredSize(50, 19));
           r.Rendered("m.rendered", s->m, LayoutRect(0, 0, 50, 19));
           r.Rendered("inner.rendered", s->inner->view, LayoutRect(0, 0, 37, 19));
         });
       }},
    }));
    out.push_back(Single("C11.explicit-clamped-content", "An explicit view measures its content inside its own minimum-clamped size", 13, [](Run& r) {
      View p = Leaf(30, 50, "parent");
      p.SetMinimumWidth(60);
      auto inner      = Probed(r);
      inner->wrapping = true;
      p.Add(inner->view);
      View stage = Mount(r, p, 200, 100);
      r.AfterRender({stage, p, inner->view}, [=](Run& r) {
        r.Size("p", p.GetMeasuredSize(), MeasuredSize(60, 50));
        r.Rendered("p.rendered", p, LayoutRect(0, 0, 60, 50));
        r.Near("inner.last-width", inner->lastWidth, 60);
        r.Size("inner", inner->view.GetMeasuredSize(), MeasuredSize(37, 19));
        r.Rendered("inner.rendered", inner->view, LayoutRect(0, 0, 37, 19));
      });
    }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc97)
