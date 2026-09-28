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
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc104)
