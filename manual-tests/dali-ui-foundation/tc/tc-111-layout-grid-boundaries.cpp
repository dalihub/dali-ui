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
struct Gridded
{
  View       stage;
  GridLayout grid;
  View       a;
};
} // namespace

class Tc111 : public Case
{
public:
  Tc111()
  : Case("TC111", "Grid boundaries", "Check GridLayout index/span clamping at limits")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    for(uint32_t index : {0u, 1u, UINT32_MAX})
      out.push_back(Single(Id("G12.index", index).c_str(), "First, last and clamped indexes", 4, [index](Run& r) {
        auto g = GridLayout::New();
        GridTracks(g, {20, 30}, {30, 50});
        g.SetRowSpacing(5);
        g.SetColumnSpacing(7);
        View a     = Cell(g, index, index);
        View stage = Mount(r, g, 100, 70);
        r.AfterRender({stage, g, a}, [=](Run& r) { r.Rendered("cell", a, index == 0 ? LayoutRect(0, 0, 30, 20) : LayoutRect(37, 25, 50, 30)); });
      }));
    for(uint32_t span : {0u, 1u, 2u, UINT32_MAX})
      out.push_back(Single(Id("G12.span", span).c_str(), "Minimum and clamped span", 6, [span](Run& r) {
        auto g = GridLayout::New();
        GridTracks(g, {20, 30}, {30, 50});
        g.SetRowSpacing(5);
        g.SetColumnSpacing(7);
        View             a = Cell(g, 0, 0, span, span);
        GridLayoutParams p;
        r.Truth("params", a.TryGetLayoutParams(p));
        r.Equal("normalized-span", p.GetRowSpan(), span == 0 ? 1 : span);
        View stage = Mount(r, g, 100, 70);
        r.AfterRender({stage, g, a}, [=](Run& r) { r.Rendered("span", a, span <= 1 ? LayoutRect(0, 0, 30, 20) : LayoutRect(0, 0, 87, 55)); });
      }));
    out.push_back(Steps("G13.definition-clear", "Clear and restore tracks", {
      {"defined", 4, [](Run& r) {
         auto s  = std::make_shared<Gridded>();
         s->grid = GridLayout::New();
         Fixed(s->grid, 100, 60);
         GridTracks(s->grid, {20}, {30});
         s->a     = Cell(s->grid, UINT32_MAX, UINT32_MAX);
         s->stage = Mount(r, s->grid, 100, 60);
         r.SetState(s);
         r.AfterRender({s->stage, s->grid, s->a}, [=](Run& r) { r.Rendered("defined", s->a, LayoutRect(0, 0, 30, 20)); });
       }},
      {"cleared", 4, [](Run& r) {
         auto s = r.State<Gridded>();
         s->grid.ClearRowDefinitions();
         s->grid.ClearColumnDefinitions();
         r.AfterRender({s->stage, s->grid, s->a}, [=](Run& r) { r.Rendered("cleared", s->a, LayoutRect(0, 0, 100, 60)); });
       }},
    }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc111)
