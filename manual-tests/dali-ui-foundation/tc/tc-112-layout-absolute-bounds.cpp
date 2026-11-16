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

class Tc112 : public Case
{
public:
  Tc112()
  : Case("TC112", "Absolute bounds", "Check AbsoluteLayout explicit bounds and wrapping")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    out.push_back(Steps("A01.explicit-local", "Explicit bounds keep their local origin when the parent moves", {
      {"mount", 8, [](Run& r) {
         auto s    = std::make_shared<Absolute>();
         s->layout = AbsoluteLayout::New();
         Fixed(s->layout, 200, 100);
         s->layout.SetMargin(Insets(70, 0, 90, 0));
         s->child = Leaf(10, 10, "child");
         s->child.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(11, 13, 37, 19)));
         s->layout.Add(s->child);
         s->stage = Mount(r, s->layout, 300, 220);
         r.SetState(s);
         r.AfterRender({s->stage, s->layout, s->child}, [=](Run& r) {
           r.Rendered("parent", s->layout, LayoutRect(70, 90, 200, 100));
           r.Rendered("child", s->child, LayoutRect(11, 13, 37, 19));
         });
       }},
      {"move-parent", 8, [](Run& r) {
         auto s = r.State<Absolute>();
         s->layout.SetMargin(Insets(100, 0, 120, 0));
         r.AfterRender({s->stage, s->layout, s->child}, [=](Run& r) {
           r.Rendered("parent-moved", s->layout, LayoutRect(100, 120, 200, 100));
           r.Rendered("moved-parent", s->child, LayoutRect(11, 13, 37, 19));
         });
       }},
    }));
    for(float extent : {0.0f, 1.0f, 37.0f, 250.0f})
      out.push_back(Single((std::string("A02.extent.") + std::to_string(static_cast<int>(extent))).c_str(), "Explicit extent includes zero and overflow", 4, [extent](Run& r) {
        auto a = AbsoluteLayout::New();
        Fixed(a, 100, 60);
        View c = Leaf(10, 10, "child");
        c.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(-20, 7, extent, 19)));
        a.Add(c);
        View stage = Mount(r, a, 100, 60);
        r.AfterRender({stage, a, c}, [=](Run& r) { r.Rendered("explicit", c, LayoutRect(-20, 7, extent, 19)); });
      }));
    out.push_back(Steps("A03.intrinsic-match", "Negative extent delegates to the requested size, then to MATCH_PARENT", {
      {"intrinsic", 4, [](Run& r) {
         auto s    = std::make_shared<Absolute>();
         s->layout = AbsoluteLayout::New();
         Fixed(s->layout, 100, 60);
         s->child = Leaf(37, 19, "child");
         s->child.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(5, 7, -1, -1)));
         s->layout.Add(s->child);
         s->stage = Mount(r, s->layout, 100, 60);
         r.SetState(s);
         r.AfterRender({s->stage, s->layout, s->child}, [=](Run& r) { r.Rendered("intrinsic", s->child, LayoutRect(5, 7, 37, 19)); });
       }},
      {"match", 4, [](Run& r) {
         auto s = r.State<Absolute>();
         Fixed(s->child, MATCH_PARENT, MATCH_PARENT);
         r.AfterRender({s->stage, s->layout, s->child}, [=](Run& r) { r.Rendered("match", s->child, LayoutRect(5, 7, 100, 60)); });
       }},
    }));
    out.push_back(Single("A07.wrap-negative-position", "Wrap uses the positive far edge", 6, [](Run& r) {
      auto a = AbsoluteLayout::New();
      View c = Leaf(100, 20, "child");
      c.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(-20, -5, 100, 20)));
      a.Add(c);
      View stage = Mount(r, a, 300, 100);
      r.AfterRender({stage, a, c}, [=](Run& r) {
        r.Size("wrap", a.GetMeasuredSize(), MeasuredSize(80, 15));
        r.Rendered("wrap.rendered", a, LayoutRect(0, 0, 80, 15));
      });
    }));
    // An explicit bounds extent is assigned EXACTLY (the 37x19 probe is measured at 70x30 and
    // reports it); a MATCH_PARENT extent without one is assigned the layout's content extent.
    out.push_back(Single("A09.bounds-exact", "Explicit bounds extents and MATCH_PARENT axes are measured exactly at the extent they are laid out at", 14, [](Run& r) {
      auto a = AbsoluteLayout::New();
      Fixed(a, 200, 100);
      auto c = Probed(r, 37, 19, false);
      c->view.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(10, 20, 70, 30)));
      View m = View::New();
      Fixed(m, MATCH_PARENT, MATCH_PARENT);
      m.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(5, 7, -1, -1)));
      a.Add(c->view);
      a.Add(m);
      View stage = Mount(r, a, 200, 100);
      r.AfterRender({stage, a, c->view, m}, [=](Run& r) {
        r.Size("c", c->view.GetMeasuredSize(), MeasuredSize(70, 30));
        r.Near("c.last-width", c->lastWidth, 70);
        r.Near("c.last-height", c->lastHeight, 30);
        r.Rendered("c.rendered", c->view, LayoutRect(10, 20, 70, 30));
        r.Size("m", m.GetMeasuredSize(), MeasuredSize(200, 100));
        r.Rendered("m.rendered", m, LayoutRect(5, 7, 200, 100));
      });
    }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc112)
