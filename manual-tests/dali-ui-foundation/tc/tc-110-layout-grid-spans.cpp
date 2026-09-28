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

class Tc110 : public Case
{
public:
  Tc110()
  : Case("TC110", "Grid spans", "Check GridLayout spans and cell alignment")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    out.push_back(Single("G05.span-spacing", "Spans include only internal gaps", 8, [](Run& r) {
      auto g = GridLayout::New();
      GridTracks(g, {20, 30, 40}, {30, 50, 70});
      g.SetRowSpacing(5);
      g.SetColumnSpacing(7);
      View a = Cell(g, 0, 0, 2, 2), b = Cell(g, 1, 1, 2, 2);
      View stage = Mount(r, g, 200, 150);
      r.AfterRender({stage, g, a, b}, [=](Run& r) {
        r.Rendered("span00", a, LayoutRect(0, 0, 87, 55));
        r.Rendered("span11", b, LayoutRect(37, 25, 127, 75));
      });
    }));
    out.push_back(Single("G06.auto-deficit", "Span deficit distributed to AUTO tracks", 8, [](Run& r) {
      auto g = GridLayout::New();
      g.AddColumnDefinition(GridLength::Auto());
      g.AddColumnDefinition(GridLength::Auto());
      g.AddRowDefinition(GridLength::Absolute(20));
      g.SetColumnSpacing(10);
      View a = Cell(g, 0, 0), b = Cell(g, 0, 1), span = Cell(g, 0, 0, 1, 2);
      Fixed(a, 20, 10);
      Fixed(b, 30, 10);
      Fixed(span, 100, 10);
      View stage = Mount(r, g, 200, 40);
      r.AfterRender({stage, g, a, b, span}, [=](Run& r) {
        r.Rendered("auto-a", a, LayoutRect(0, 0, 40, 20));
        r.Rendered("auto-b", b, LayoutRect(50, 0, 50, 20));
      });
    }));
    for(unsigned mode = 0; mode < 4; ++mode)
      out.push_back(Single(Id("G09.align", mode).c_str(), "Cell alignment of intrinsic extent", 4, [mode](Run& r) {
        auto g = GridLayout::New();
        GridTracks(g, {60}, {100});
        View a = Cell(g, 0, 0);
        Fixed(a, 20, 10);
        const LayoutAlignment al[] = {LayoutAlignment::START, LayoutAlignment::CENTER, LayoutAlignment::END, LayoutAlignment::FILL};
        a.SetLayoutParams(GridLayoutParams::New().SetHorizontalAlignment(al[mode]).SetVerticalAlignment(al[mode]));
        View stage = Mount(r, g, 100, 60);
        r.AfterRender({stage, g, a}, [=](Run& r) {
          r.Rendered("aligned", a, LayoutRect(mode == 1 ? 40 : mode == 2 ? 80 : 0, mode == 1 ? 25 : mode == 2 ? 50 : 0, mode == 3 ? 100 : 20, mode == 3 ? 60 : 10));
        });
      }));
    out.push_back(Single("G10.padding-margin", "Padding and cell-local margins", 4, [](Run& r) {
      auto g = GridLayout::New();
      Fixed(g, 120, 80);
      g.SetPadding(Insets(3, 5, 7, 11));
      View a = Cell(g, 0, 0);
      a.SetMargin(Insets(2, 4, 6, 8));
      View stage = Mount(r, g, 120, 80);
      r.AfterRender({stage, g, a}, [=](Run& r) { r.Rendered("cell", a, LayoutRect(5, 13, 106, 48)); });
    }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc110)
