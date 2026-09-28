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

class Tc108 : public Case
{
public:
  Tc108()
  : Case("TC108", "Flex alignment", "Check FlexLayout justify, align and direction")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    for(unsigned mode = 0; mode < 6; ++mode)
      out.push_back(Single(Id("F04.justify", mode).c_str(), "Positive free space justify distribution", 8, [mode](Run& r) {
        auto f = FlexLayout::New();
        Fixed(f, 140, 60);
        f.SetAlignItems(FlexAlign::FLEX_START);
        f.SetJustifyContent(static_cast<FlexJustify>(mode));
        View a = Leaf(20, 10, "a"), b = Leaf(40, 10, "b");
        f.Add(a);
        f.Add(b);
        View stage = Mount(r, f, 140, 60);
        r.AfterRender({stage, f, a, b}, [=](Run& r) {
          const float ax[] = {0, 80, 40, 0, 20, 80.0f / 3}, bx[] = {20, 100, 60, 100, 80, 20 + 160.0f / 3};
          r.Rendered("a", a, LayoutRect(ax[mode], 0, 20, 10));
          r.Rendered("b", b, LayoutRect(bx[mode], 0, 40, 10));
        });
      }));
    for(unsigned mode = 0; mode < 5; ++mode)
      out.push_back(Single(Id("F05.align-self", mode).c_str(), "Cross alignment and align-self override", 8, [mode](Run& r) {
        auto f = FlexLayout::New();
        Fixed(f, 140, 60);
        f.SetAlignItems(FlexAlign::FLEX_END);
        View            a       = Leaf(20, 10, "a"), b = Leaf(40, 10, "b");
        const FlexAlign align[] = {FlexAlign::AUTO, FlexAlign::FLEX_START, FlexAlign::FLEX_END, FlexAlign::CENTER, FlexAlign::STRETCH};
        a.SetLayoutParams(FlexLayoutParams::New().SetAlignSelf(align[mode]));
        f.Add(a);
        f.Add(b);
        View stage = Mount(r, f, 140, 60);
        r.AfterRender({stage, f, a, b}, [=](Run& r) {
          const float y = mode == 0 || mode == 2 ? 50 : mode == 3 ? 25 : 0;
          r.Rendered("self", a, LayoutRect(0, y, 20, mode == 4 ? 60 : 10));
          r.Rendered("inherited", b, LayoutRect(20, 50, 40, 10));
        });
      }));
    for(unsigned direction = 0; direction < 4; ++direction)
      out.push_back(Single(Id("F01.direction", direction).c_str(), "Four directions", 8, [direction](Run& r) {
        auto f = FlexLayout::New();
        f.SetDirection(static_cast<FlexDirection>(direction));
        f.SetAlignItems(FlexAlign::FLEX_START);
        Fixed(f, 100, 100);
        const bool vertical = direction >= 2, reverse = direction % 2;
        View       a = Leaf(vertical ? 10 : 20, vertical ? 20 : 10, "a"), b = Leaf(vertical ? 10 : 30, vertical ? 30 : 10, "b");
        f.Add(a);
        f.Add(b);
        View stage = Mount(r, f, 100, 100);
        r.AfterRender({stage, f, a, b}, [=](Run& r) {
          LayoutRect ea(reverse ? 80 : 0, 0, 20, 10), eb(reverse ? 50 : 20, 0, 30, 10);
          r.Rendered("a", a, vertical ? SwapAxes(ea) : ea);
          r.Rendered("b", b, vertical ? SwapAxes(eb) : eb);
        });
      }));
    for(unsigned reverse = 0; reverse < 2; ++reverse)
      out.push_back(Single(Id("F01.rtl", reverse).c_str(), "Right-to-left mirrors the row on screen and the completion report carries the mirrored rects", 16, [reverse](Run& r) {
        auto f = FlexLayout::New();
        f.SetDirection(reverse ? FlexDirection::ROW_REVERSE : FlexDirection::ROW);
        f.SetAlignItems(FlexAlign::FLEX_START);
        f.SetLayoutDirection(Dali::LayoutDirection::RIGHT_TO_LEFT);
        Fixed(f, 100, 100);
        View a = Leaf(20, 10, "a"), b = Leaf(30, 10, "b");
        f.Add(a);
        f.Add(b);
        View stage = Mount(r, f, 100, 100);
        r.AfterRender({stage, f, a, b}, [=](Run& r) {
          const LayoutRect la(reverse ? 80 : 0, 0, 20, 10), lb(reverse ? 50 : 20, 0, 30, 10);
          // Logical (pre-RTL) rects la/lb; the completion report and the render carry the mirrored rects.
          const LayoutRect ma(100 - la.x - la.width, 0, 20, 10), mb(100 - lb.x - lb.width, 0, 30, 10);
          r.Rect("a.reported", r.Snapshot(a), ma);
          r.Rect("b.reported", r.Snapshot(b), mb);
          r.Rendered("a", a, ma);
          r.Rendered("b", b, mb);
        });
      }));
    for(unsigned mode : {3u, 4u, 5u})
      out.push_back(Single(Id("F12.overflow", mode).c_str(), "Negative free space does not create negative gaps", 8, [mode](Run& r) {
        auto f = FlexLayout::New();
        Fixed(f, 40, 50);
        f.SetAlignItems(FlexAlign::FLEX_START);
        f.SetJustifyContent(static_cast<FlexJustify>(mode));
        View a = Leaf(30, 10, "a"), b = Leaf(30, 10, "b");
        a.SetLayoutParams(FlexLayoutParams::New().SetFlexShrink(0));
        b.SetLayoutParams(FlexLayoutParams::New().SetFlexShrink(0));
        f.Add(a);
        f.Add(b);
        View stage = Mount(r, f, 40, 50);
        r.AfterRender({stage, f, a, b}, [=](Run& r) {
          const float start = mode == 3 ? 0 : -10;
          r.Rendered("a", a, LayoutRect(start, 0, 30, 10));
          r.Rendered("b", b, LayoutRect(start + 30, 0, 30, 10));
        });
      }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc108)
