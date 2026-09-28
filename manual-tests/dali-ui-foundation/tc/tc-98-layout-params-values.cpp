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
struct Weighted
{
  View stage, stack, a, b;
};
} // namespace

class Tc98 : public Case
{
public:
  Tc98()
  : Case("TC98", "LayoutParams value semantics", "Check LayoutParams are copied by value")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    out.push_back(Single("C16.stack-copy", "Stack params are independent values", 6, [](Run& r) {
      View a = View::New(), b = View::New();
      auto original = StackLayoutParams::New().SetWeight(1);
      a.SetLayoutParams(original);
      b.SetLayoutParams(original);
      original.SetWeight(9);
      StackLayoutParams pa, pb;
      r.Truth("read-a", a.TryGetLayoutParams(pa));
      r.Truth("read-b", b.TryGetLayoutParams(pb));
      r.Near("a-original-copy", pa.GetWeight(), 1, 0);
      r.Near("b-original-copy", pb.GetWeight(), 1, 0);
      pa.SetWeight(3);
      StackLayoutParams before;
      a.TryGetLayoutParams(before);
      r.Near("snapshot-is-copy", before.GetWeight(), 1, 0);
      a.SetLayoutParams(pa);
      a.TryGetLayoutParams(before);
      r.Near("explicit-reassign", before.GetWeight(), 3, 0);
    }));
    out.push_back(Steps("C16.rendered-copy", "The stored copy, not the mutated original handle, drives the rendered weights", {
      {"mount", 8, [](Run& r) {
         auto s   = std::make_shared<Weighted>();
         s->stack = StackLayout::New(StackOrientation::HORIZONTAL);
         Fixed(s->stack, 200, 50);
         s->a          = Leaf(0, 50, "a");
         s->b          = Leaf(0, 50, "b");
         auto original = StackLayoutParams::New().SetWeight(1);
         s->a.SetLayoutParams(original);
         s->b.SetLayoutParams(original);
         original.SetWeight(9);
         s->stack.Add(s->a);
         s->stack.Add(s->b);
         s->stage = Mount(r, s->stack, 200, 50);
         r.SetState(s);
         r.AfterRender({s->stage, s->stack, s->a, s->b}, [=](Run& r) {
           r.Rendered("a", s->a, LayoutRect(0, 0, 100, 50));
           r.Rendered("b", s->b, LayoutRect(100, 0, 100, 50));
         });
       }},
      {"reassign-a", 8, [](Run& r) {
         auto s = r.State<Weighted>();
         s->a.SetLayoutParams(StackLayoutParams::New().SetWeight(3));
         r.AfterRender({s->stage, s->stack, s->a, s->b}, [=](Run& r) {
           r.Rendered("a-reweighted", s->a, LayoutRect(0, 0, 150, 50));
           r.Rendered("b-reweighted", s->b, LayoutRect(150, 0, 50, 50));
         });
       }},
    }));
    out.push_back(Single("C16.flex-copy", "Flex params copy every field", 6, [](Run& r) {
      View v = View::New();
      auto p = FlexLayoutParams::New().SetFlexBasis(17).SetFlexGrow(2).SetFlexShrink(3).SetAlignSelf(FlexAlign::CENTER);
      v.SetLayoutParams(p);
      p.SetFlexBasis(99);
      FlexLayoutParams q;
      r.Truth("read", v.TryGetLayoutParams(q));
      r.Near("basis", q.GetFlexBasis(), 17, 0);
      r.Near("grow", q.GetFlexGrow(), 2, 0);
      r.Near("shrink", q.GetFlexShrink(), 3, 0);
      r.Equal("align", int(q.GetAlignSelf()), int(FlexAlign::CENTER));
      q.SetFlexGrow(8);
      FlexLayoutParams fresh;
      v.TryGetLayoutParams(fresh);
      r.Near("snapshot-independent", fresh.GetFlexGrow(), 2, 0);
    }));
    out.push_back(Single("C16.grid-copy", "Grid params copy index span and alignment", 6, [](Run& r) {
      View v = View::New();
      auto p = GridLayoutParams::New().SetRow(2).SetColumn(3).SetRowSpan(4).SetColumnSpan(5).SetHorizontalAlignment(LayoutAlignment::END);
      v.SetLayoutParams(p);
      p.SetRow(9);
      GridLayoutParams q;
      r.Truth("read", v.TryGetLayoutParams(q));
      r.Equal("row", q.GetRow(), 2);
      r.Equal("column", q.GetColumn(), 3);
      r.Equal("row-span", q.GetRowSpan(), 4);
      r.Equal("column-span", q.GetColumnSpan(), 5);
      r.Equal("alignment", int(q.GetHorizontalAlignment()), int(LayoutAlignment::END));
    }));
    out.push_back(Single("C16.absolute-copy", "Absolute rect and flags are copied", 6, [](Run& r) {
      View v = View::New();
      auto p = AbsoluteLayoutParams::New().SetBounds(LayoutRect(1, 2, 30, 40)).SetFlags(AbsoluteLayoutFlags::X_PROPORTIONAL);
      v.SetLayoutParams(p);
      p.SetX(9);
      AbsoluteLayoutParams q;
      r.Truth("read", v.TryGetLayoutParams(q));
      r.Rect("bounds", q.GetBounds(), LayoutRect(1, 2, 30, 40));
      r.Equal("flags", int(q.GetFlags()), int(AbsoluteLayoutFlags::X_PROPORTIONAL));
    }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc98)
