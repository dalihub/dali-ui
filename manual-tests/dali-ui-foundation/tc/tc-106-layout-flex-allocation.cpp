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

class Tc106 : public Case
{
public:
  Tc106()
  : Case("TC106", "Flex allocation", "Check FlexLayout basis, grow and shrink shares")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    for(bool shrink : {false, true})
      out.push_back(Single(shrink ? "F07.shrink" : "F06.grow", "Basis weighted grow and shrink rendered on screen", 8, [shrink](Run& r) {
        auto f = FlexLayout::New();
        f.SetAlignItems(FlexAlign::FLEX_START);
        const float width = shrink ? 250 : 150;
        Fixed(f, width, 50);
        View a = Leaf(10, 10, "a"), b = Leaf(10, 10, "b");
        a.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(shrink ? 100 : 20).SetFlexGrow(1).SetFlexShrink(1));
        b.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(shrink ? 200 : 40).SetFlexGrow(2).SetFlexShrink(2));
        f.Add(a);
        f.Add(b);
        View stage = Mount(r, f, width, 50);
        r.AfterRender({stage, f, a, b}, [=](Run& r) {
          const float aw = shrink ? 90 : 50, bw = shrink ? 160 : 100;
          r.Rendered("a", a, LayoutRect(0, 0, aw, 10));
          r.Rendered("b", b, LayoutRect(aw, 0, bw, 10));
        });
      }));
    for(bool shrink : {false, true})
      out.push_back(Single(shrink ? "F11.minimum" : "F10.maximum", "Clamped measure and allocated basis remain distinct", 10, [shrink](Run& r) {
        auto f = FlexLayout::New();
        f.SetAlignItems(FlexAlign::FLEX_START);
        const float width = shrink ? 100 : 200;
        Fixed(f, width, 50);
        View a = Leaf(shrink ? 20 : 160, 10, "a"), b = Leaf(20, 10, "b");
        if(shrink)
          a.SetMinimumWidth(80);
        else
          a.SetMaximumWidth(60);
        a.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(shrink ? 100 : 50).SetFlexGrow(1).SetFlexShrink(1));
        b.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(shrink ? 100 : 50).SetFlexGrow(1).SetFlexShrink(1));
        f.Add(a);
        f.Add(b);
        View stage = Mount(r, f, width, 50);
        r.AfterRender({stage, f, a, b}, [=](Run& r) {
          const float w = shrink ? 50 : 100;
          r.Size("measured-a", a.GetMeasuredSize(), MeasuredSize(shrink ? 80 : 60, 10));
          r.Rendered("allocated-a", a, LayoutRect(0, 0, w, 10));
          r.Rendered("allocated-b", b, LayoutRect(w, 0, w, 10));
        });
      }));
    out.push_back(Single("F09.match-grow", "MATCH_PARENT consumes the remaining share", 4, [](Run& r) {
      auto f = FlexLayout::New();
      f.SetAlignItems(FlexAlign::FLEX_START);
      Fixed(f, 200, 50);
      View a = Leaf(50, 10, "a"), b = Leaf(MATCH_PARENT, 10, "b");
      b.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1));
      f.Add(a);
      f.Add(b);
      View stage = Mount(r, f, 200, 50);
      r.AfterRender({stage, f, a, b}, [=](Run& r) { r.Rendered("match", b, LayoutRect(50, 0, 150, 10)); });
    }));
    out.push_back(Single("F08.zero-basis", "Zero basis is not intrinsic basis", 8, [](Run& r) {
      auto f = FlexLayout::New();
      f.SetAlignItems(FlexAlign::FLEX_START);
      Fixed(f, 100, 50);
      View a = Leaf(40, 10, "a"), b = Leaf(30, 10, "b");
      a.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(0));
      b.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(0).SetFlexGrow(1));
      f.Add(a);
      f.Add(b);
      View stage = Mount(r, f, 100, 50);
      r.AfterRender({stage, f, a, b}, [=](Run& r) {
        r.Rendered("zero", a, LayoutRect(0, 0, 0, 10));
        r.Rendered("growing", b, LayoutRect(0, 0, 100, 10));
      });
    }));
    out.push_back(Single("F15.shrink-clamp", "Individual shrink clamps at zero without redistribution", 8, [](Run& r) {
      auto f = FlexLayout::New();
      Fixed(f, 10, 50);
      f.SetAlignItems(FlexAlign::FLEX_START);
      View a = Leaf(1, 10, "a"), b = Leaf(100, 10, "b");
      a.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(1).SetFlexShrink(100));
      b.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(100).SetFlexShrink(1));
      f.Add(a);
      f.Add(b);
      View stage = Mount(r, f, 10, 50);
      r.AfterRender({stage, f, a, b}, [=](Run& r) {
        r.Rendered("clamped", a, LayoutRect(0, 0, 0, 10));
        r.Rendered("remaining", b, LayoutRect(0, 0, 54.5f, 10));
      });
    }));
    out.push_back(Single("F16.margin-padding", "Container padding and child margin shrink the growing child's rendered slot", 4, [](Run& r) {
      auto f = FlexLayout::New();
      Fixed(f, 120, 80);
      f.SetPadding(Insets(3, 5, 7, 11));
      f.SetAlignItems(FlexAlign::STRETCH);
      View a = Leaf(10, 10, "a");
      a.SetMargin(Insets(2, 4, 6, 8));
      a.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1));
      f.Add(a);
      View stage = Mount(r, f, 120, 80);
      r.AfterRender({stage, f, a}, [=](Run& r) { r.Rendered("padded-margin", a, LayoutRect(5, 13, 106, 48)); });
    }));
    out.push_back(Single("F17.wrap-minimum-grow", "A minimum width extends a WRAP_CONTENT flex and the extra space is distributed at arrange time", 10, [](Run& r) {
      auto f = FlexLayout::New();
      Fixed(f, WRAP_CONTENT, 30);
      f.SetMinimumWidth(100);
      f.SetAlignItems(FlexAlign::FLEX_START);
      View a = Leaf(10, 10, "a"), b = Leaf(10, 10, "b");
      a.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(20).SetFlexGrow(1));
      b.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(30).SetFlexGrow(1));
      f.Add(a);
      f.Add(b);
      View stage = Mount(r, f, 200, 30);
      r.AfterRender({stage, f, a, b}, [=](Run& r) {
        r.Size("minimum.measured", f.GetMeasuredSize(), MeasuredSize(100, 30));
        r.Rendered("minimum.a", a, LayoutRect(0, 0, 45, 10));
        r.Rendered("minimum.b", b, LayoutRect(45, 0, 55, 10));
      });
    }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc106)
