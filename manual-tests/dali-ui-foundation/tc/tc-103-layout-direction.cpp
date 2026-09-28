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
struct Directed
{
  View stage, stack, a, b;
};
struct DirectedKind
{
  View stage, container, a, b, standalone;
};
inline LayoutRect Mirrored(const LayoutRect& v)
{
  return LayoutRect(200 - v.x - v.width, v.y, v.width, v.height);
}
} // namespace

class Tc103 : public Case
{
public:
  Tc103()
  : Case("TC103", "Direction inheritance", "Check LTR/RTL inheritance and single mirroring")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    std::vector<Step>     phases;
    for(unsigned phase = 0; phase < 3; ++phase)
    {
      const bool rtl = phase == 1;
      phases.push_back({Id(rtl ? "rtl" : "ltr", phase), rtl ? 16u : 8u, [phase, rtl](Run& r) {
                          std::shared_ptr<Directed> s;
                          if(phase == 0)
                          {
                            s        = std::make_shared<Directed>();
                            s->stack = StackLayout::New(StackOrientation::HORIZONTAL);
                            Fixed(s->stack, 200, 60);
                            StackLayout::DownCast(s->stack).SetSpacing(7);
                            s->a = Leaf(20, 10, "a");
                            s->b = Leaf(30, 15, "b");
                            s->stack.Add(s->a);
                            s->stack.Add(s->b);
                            s->stage = Mount(r, s->stack, 200, 60);
                            r.SetState(s);
                          }
                          else
                          {
                            s = r.State<Directed>();
                          }
                          s->stack.SetLayoutDirection(rtl ? Dali::LayoutDirection::RIGHT_TO_LEFT : Dali::LayoutDirection::LEFT_TO_RIGHT);
                          r.AfterRender({s->stage, s->stack, s->a, s->b}, [=](Run& r) {
                            r.Rendered(Id("a", phase).c_str(), s->a, LayoutRect(rtl ? 180 : 0, 0, 20, 10));
                            r.Rendered(Id("b", phase).c_str(), s->b, LayoutRect(rtl ? 143 : 27, 0, 30, 15));
                            if(rtl)
                            {
                              // The completion report carries the final (post-RTL) actor bounds.
                              r.Rect("a.reported", r.Snapshot(s->a), LayoutRect(180, 0, 20, 10));
                              r.Rect("b.reported", r.Snapshot(s->b), LayoutRect(143, 0, 30, 15));
                            }
                          });
                        }});
    }
    out.push_back(Steps("C15.LTR-RTL-LTR", "The parent mirror is applied once on screen, reported as final bounds, and reversible", phases));
    out.push_back(Single("C15.explicit-child", "Explicit child direction preserves its own subtree rule on screen", 8, [](Run& r) {
      View        root  = Leaf(300, 100, "root");
      StackLayout child = StackLayout::New(StackOrientation::HORIZONTAL);
      Fixed(child, 100, 50);
      child.SetLayoutDirection(Dali::LayoutDirection::LEFT_TO_RIGHT);
      View leaf = Leaf(20, 10, "leaf");
      child.Add(leaf);
      root.Add(child);
      root.SetLayoutDirection(Dali::LayoutDirection::RIGHT_TO_LEFT);
      View stage = Mount(r, root, 300, 100);
      r.AfterRender({stage, root, child, leaf}, [=](Run& r) {
        r.Rendered("child-mirrored-by-parent", child, LayoutRect(200, 0, 100, 50));
        r.Rendered("leaf-explicit-ltr", leaf, LayoutRect(0, 0, 20, 10));
      });
    }));
    for(unsigned kind = 0; kind < 5; ++kind)
    {
      // LTR rects per container kind inside a 200x60 container; RTL mirrors x, the standalone child never moves.
      const bool       explicitPosition = kind == 0 || kind == 4;
      const LayoutRect la               = explicitPosition ? LayoutRect(5, 7, 20, 10) : LayoutRect(0, 0, 20, 10);
      const LayoutRect lb               = explicitPosition ? LayoutRect(40, 20, 30, 15) : kind == 3 ? LayoutRect(0, 0, 30, 15) : LayoutRect(20, 0, 30, 15);
      const LayoutRect ls(150, 40, 10, 10);
      const char*      ids[] = {"ltr", "rtl", "replay", "ltr-again"};
      std::vector<Step> steps;
      for(unsigned phase = 0; phase < 4; ++phase)
        steps.push_back({ids[phase], 12, [kind, phase, la, lb, ls](Run& r) {
                           std::shared_ptr<DirectedKind> s;
                           if(phase == 0)
                           {
                             s            = std::make_shared<DirectedKind>();
                             s->container = Container(kind, 200, 60);
                             if(kind == 2) FlexLayout::DownCast(s->container).SetAlignItems(FlexAlign::FLEX_START);
                             s->a = Leaf(20, 10, "a");
                             s->b = Leaf(30, 15, "b");
                             if(kind == 0)
                             {
                               s->a.SetRequestedX(la.x);
                               s->a.SetRequestedY(la.y);
                               s->b.SetRequestedX(lb.x);
                               s->b.SetRequestedY(lb.y);
                             }
                             if(kind == 3)
                             {
                               s->a.SetLayoutParams(GridLayoutParams::New().SetHorizontalAlignment(LayoutAlignment::START).SetVerticalAlignment(LayoutAlignment::START));
                               s->b.SetLayoutParams(GridLayoutParams::New().SetHorizontalAlignment(LayoutAlignment::START).SetVerticalAlignment(LayoutAlignment::START));
                             }
                             if(kind == 4)
                             {
                               s->a.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(la));
                               s->b.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(lb));
                             }
                             s->standalone = Leaf(10, 10, "standalone");
                             s->standalone.SetLayoutMode(LayoutMode::STANDALONE);
                             s->standalone.SetRequestedX(ls.x);
                             s->standalone.SetRequestedY(ls.y);
                             s->container.Add(s->a);
                             s->container.Add(s->b);
                             s->container.Add(s->standalone);
                             s->stage = Mount(r, s->container, 200, 60);
                             r.SetState(s);
                           }
                           else
                           {
                             s = r.State<DirectedKind>();
                           }
                           if(phase == 1) s->container.SetLayoutDirection(Dali::LayoutDirection::RIGHT_TO_LEFT);
                           // Same inputs under RTL: the children take the cache-hit replay path, which must
                           // keep the mirror folded in rather than mirror again or undo it.
                           if(phase == 2) s->container.InvalidateArrange();
                           if(phase == 3) s->container.SetLayoutDirection(Dali::LayoutDirection::LEFT_TO_RIGHT);
                           const bool rtl = phase == 1 || phase == 2;
                           r.AfterRender({s->stage, s->container, s->a, s->b, s->standalone}, [=](Run& r) {
                             r.Rendered(Id("a", phase).c_str(), s->a, rtl ? Mirrored(la) : la);
                             r.Rendered(Id("b", phase).c_str(), s->b, rtl ? Mirrored(lb) : lb);
                             r.Rendered(Id("standalone", phase).c_str(), s->standalone, ls);
                           });
                         }});
      out.push_back(Steps(Id("C15.standalone-replay", kind), "Standalone children are excluded from the mirror and the cache-hit replay keeps mirrored geometry", steps));
    }
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc103)
