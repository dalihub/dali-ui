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

class Tc107 : public Case
{
public:
  Tc107()
  : Case("TC107", "Flex lines", "Check FlexLayout wrapping and line distribution")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    for(bool reverse : {false, true})
      for(float width : {100.0f, 99.0f})
        out.push_back(Single((std::string("F02.wrap.") + (reverse ? "reverse." : "normal.") + std::to_string(static_cast<int>(width))).c_str(), "Exact wrap boundary rendered on screen", 12, [reverse, width](Run& r) {
          auto f = FlexLayout::New();
          Fixed(f, width, 80);
          f.SetWrap(reverse ? FlexWrap::WRAP_REVERSE : FlexWrap::WRAP);
          f.SetAlignItems(FlexAlign::FLEX_START);
          f.SetAlignContent(FlexAlign::FLEX_START);
          View a = Leaf(40, 10, "a"), b = Leaf(60, 20, "b"), c = Leaf(30, 15, "c");
          f.Add(a);
          f.Add(b);
          f.Add(c);
          View stage = Mount(r, f, width, 80);
          r.AfterRender({stage, f, a, b, c}, [=](Run& r) {
            if(width == 100)
            {
              r.Rendered("a", a, LayoutRect(0, reverse ? 15 : 0, 40, 10));
              r.Rendered("b", b, LayoutRect(40, reverse ? 15 : 0, 60, 20));
              r.Rendered("c", c, LayoutRect(0, reverse ? 0 : 20, 30, 15));
            }
            else
            {
              r.Rendered("a", a, LayoutRect(0, reverse ? 20 : 0, 40, 10));
              r.Rendered("b", b, LayoutRect(0, reverse ? 0 : 10, 60, 20));
              r.Rendered("c", c, LayoutRect(60, reverse ? 0 : 10, 30, 15));
            }
          });
        }));
    out.push_back(Single("F03.wrap-measure", "A wrapping flex container measures max line main and summed cross", 6, [](Run& r) {
      auto f = FlexLayout::New();
      f.SetWrap(FlexWrap::WRAP);
      f.SetAlignItems(FlexAlign::FLEX_START);
      f.SetAlignContent(FlexAlign::FLEX_START);
      View a = Leaf(40, 10, "a"), b = Leaf(60, 20, "b"), c = Leaf(30, 15, "c");
      f.Add(a);
      f.Add(b);
      f.Add(c);
      View stage = Mount(r, f, 100, 80);
      r.AfterRender({stage, f, a, b, c}, [=](Run& r) {
        r.Size("wrapped", f.GetMeasuredSize(), MeasuredSize(100, 35));
        r.Rendered("wrapped.rendered", f, LayoutRect(0, 0, 100, 35));
      });
    }));
    for(unsigned mode = 0; mode < 4; ++mode)
      out.push_back(Single(Id("F13.content", mode).c_str(), "Wrapped line cross distribution", 8, [mode](Run& r) {
        auto f = FlexLayout::New();
        Fixed(f, 60, 100);
        f.SetWrap(FlexWrap::WRAP);
        f.SetAlignItems(FlexAlign::FLEX_START);
        const FlexAlign align[] = {FlexAlign::FLEX_START, FlexAlign::CENTER, FlexAlign::FLEX_END, FlexAlign::STRETCH};
        f.SetAlignContent(align[mode]);
        View a = Leaf(40, 10, "a"), b = Leaf(40, 20, "b");
        f.Add(a);
        f.Add(b);
        View stage = Mount(r, f, 60, 100);
        r.AfterRender({stage, f, a, b}, [=](Run& r) {
          const float y0 = mode == 1 ? 35 : mode == 2 ? 70 : 0, y1 = mode == 3 ? 45 : y0 + 10;
          r.Rendered("line0", a, LayoutRect(0, y0, 40, 10));
          r.Rendered("line1", b, LayoutRect(0, y1, 40, 20));
        });
      }));
    out.push_back(Single("F02.wrap-grow-lines", "Grow and shrink are resolved per wrapped line on screen", 20, [](Run& r) {
      auto f = FlexLayout::New();
      Fixed(f, 100, 60);
      f.SetWrap(FlexWrap::WRAP);
      f.SetAlignItems(FlexAlign::FLEX_START);
      f.SetAlignContent(FlexAlign::FLEX_START);
      View a = Leaf(10, 10, "a"), b = Leaf(10, 10, "b"), c = Leaf(10, 20, "c"), d = Leaf(10, 20, "d"), e = Leaf(10, 10, "e");
      a.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(40).SetFlexGrow(1));
      b.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(50).SetFlexGrow(1));
      c.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(30).SetFlexGrow(2));
      d.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(30));
      e.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(120).SetFlexShrink(1));
      f.Add(a);
      f.Add(b);
      f.Add(c);
      f.Add(d);
      f.Add(e);
      View stage = Mount(r, f, 100, 60);
      // Lines: [40,50] free 10 -> 45/55; [30,30] free 40 to grow 2 -> 70/30; [120] alone shrinks to 100.
      r.AfterRender({stage, f, a, b, c, d, e}, [=](Run& r) {
        r.Rendered("a", a, LayoutRect(0, 0, 45, 10));
        r.Rendered("b", b, LayoutRect(45, 0, 55, 10));
        r.Rendered("c", c, LayoutRect(0, 10, 70, 20));
        r.Rendered("d", d, LayoutRect(70, 10, 30, 20));
        r.Rendered("e", e, LayoutRect(0, 30, 100, 10));
      });
    }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc107)
