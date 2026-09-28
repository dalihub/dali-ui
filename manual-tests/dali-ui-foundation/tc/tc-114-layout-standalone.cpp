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
  View stage, root, a, b;
};
} // namespace

class Tc114 : public Case
{
public:
  Tc114()
  : Case("TC114", "Standalone layout", "Check STANDALONE child is laid out independently")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    for(unsigned kind = 0; kind < 5; ++kind)
      out.push_back(Single(Id("S10.standalone", kind).c_str(), "Standalone is excluded from accumulation and rendered from its own requests", 11, [kind](Run& r) {
        View root = Container(kind);
        View ordinary = Leaf(20, 10, "ordinary"), standalone = Leaf(100, 80, "standalone");
        standalone.SetLayoutMode(LayoutMode::STANDALONE);
        standalone.SetRequestedX(7);
        standalone.SetRequestedY(9);
        root.Add(ordinary);
        root.Add(standalone);
        View stage = Mount(r, root, 200, 100);
        r.AfterRender({stage, root, ordinary}, [=](Run& r) {
          r.Size("parent-measured", root.GetMeasuredSize(), MeasuredSize(20, 10));
          r.Rendered("ordinary", ordinary, LayoutRect(0, 0, 20, 10));
          r.Rendered("independent", standalone, LayoutRect(7, 9, 100, 80));
          r.Equal("logical-count", root.GetChildViewCount(), 2);
        });
      }));
    out.push_back(Steps("G14.mode-change", "Changing the layout mode invalidates the parent intrinsic extent on screen", {
      {"ordinary", 6, [](Run& r) {
         auto s  = std::make_shared<Tree>();
         s->root = StackLayout::New(StackOrientation::HORIZONTAL);
         StackLayout::DownCast(s->root).SetSpacing(7);
         s->a = Leaf(20, 10, "a");
         s->b = Leaf(30, 15, "b");
         s->root.Add(s->a);
         s->root.Add(s->b);
         s->stage = Mount(r, s->root, 200, 100);
         r.SetState(s);
         r.AfterRender({s->stage, s->root, s->a, s->b}, [=](Run& r) {
           r.Size("ordinary", s->root.GetMeasuredSize(), MeasuredSize(57, 15));
           r.Rendered("ordinary.rendered", s->root, LayoutRect(0, 0, 57, 15));
         });
       }},
      {"standalone", 6, [](Run& r) {
         auto s = r.State<Tree>();
         s->b.SetLayoutMode(LayoutMode::STANDALONE);
         r.AfterRender({s->stage, s->root, s->a}, [=](Run& r) {
           r.Size("excluded", s->root.GetMeasuredSize(), MeasuredSize(20, 10));
           r.Rendered("excluded.rendered", s->root, LayoutRect(0, 0, 20, 10));
         });
       }},
    }));
    for(unsigned kind = 0; kind < 5; ++kind)
      out.push_back(Steps(Id("S10.match-margin-clamp", kind), "Standalone MATCH_PARENT ignores parent padding and applies its own margin, then its own min/max", {
        {"match", 4, [kind](Run& r) {
           auto s  = std::make_shared<Tree>();
           s->root = Container(kind, 200, 100);
           s->root.SetPadding(Insets(20, 30, 40, 10));
           s->a = Leaf(20, 10, "ordinary");
           s->b = Leaf(MATCH_PARENT, MATCH_PARENT, "standalone");
           s->b.SetLayoutMode(LayoutMode::STANDALONE);
           s->b.SetRequestedX(5);
           s->b.SetRequestedY(9);
           s->b.SetMargin(Insets(7, 11, 13, 17));
           s->root.Add(s->a);
           s->root.Add(s->b);
           s->stage = Mount(r, s->root, 200, 100);
           r.SetState(s);
           r.AfterRender({s->stage, s->root, s->a}, [=](Run& r) { r.Rendered("full-parent-minus-own-margin", s->b, LayoutRect(12, 22, 182, 70)); });
         }},
        {"clamp", 4, [](Run& r) {
           auto s = r.State<Tree>();
           s->b.SetMinimumWidth(190);
           s->b.SetMaximumWidth(195);
           s->b.SetMinimumHeight(20);
           s->b.SetMaximumHeight(60);
           r.AfterRender({s->b}, [=](Run& r) { r.Rendered("own-minmax-after-match", s->b, LayoutRect(12, 22, 190, 60)); });
         }},
      }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc114)
