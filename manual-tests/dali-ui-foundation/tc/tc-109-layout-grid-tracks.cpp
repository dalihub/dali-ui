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
  View       a, b;
};
} // namespace

class Tc109 : public Case
{
public:
  Tc109()
  : Case("TC109", "Grid tracks", "Check GridLayout ABSOLUTE/STAR/AUTO track sizes")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    out.push_back(Single("G01.absolute", "Absolute tracks and spacing; the wrapping grid measures its track sums", 14, [](Run& r) {
      auto g = GridLayout::New();
      GridTracks(g, {20, 30}, {40, 50});
      g.SetRowSpacing(5);
      g.SetColumnSpacing(7);
      View a = Cell(g, 0, 0), b = Cell(g, 0, 1), c = Cell(g, 1, 1);
      View stage = Mount(r, g, 200, 100);
      r.AfterRender({stage, g, a, b, c}, [=](Run& r) {
        r.Size("wrapped", g.GetMeasuredSize(), MeasuredSize(97, 55));
        r.Rendered("a", a, LayoutRect(0, 0, 40, 20));
        r.Rendered("b", b, LayoutRect(47, 0, 50, 20));
        r.Rendered("c", c, LayoutRect(47, 25, 50, 30));
      });
    }));
    out.push_back(Single("G02.star", "Fixed plus star conservation", 12, [](Run& r) {
      auto g = GridLayout::New();
      Fixed(g, 330, 50);
      g.AddColumnDefinition(GridLength::Absolute(50));
      g.AddColumnDefinition(GridLength::Star(1));
      g.AddColumnDefinition(GridLength::Star(2));
      g.SetColumnSpacing(10);
      View a = Cell(g, 0, 0), b = Cell(g, 0, 1), c = Cell(g, 0, 2);
      View stage = Mount(r, g, 330, 50);
      r.AfterRender({stage, g, a, b, c}, [=](Run& r) {
        r.Rendered("a", a, LayoutRect(0, 0, 50, 50));
        r.Rendered("b", b, LayoutRect(60, 0, 260.0f / 3, 50));
        r.Rendered("c", c, LayoutRect(70 + 260.0f / 3, 0, 520.0f / 3, 50));
      });
    }));
    out.push_back(Single("G03.auto", "Auto floors and remaining star", 8, [](Run& r) {
      auto g = GridLayout::New();
      Fixed(g, 200, 50);
      g.AddColumnDefinition(GridLength::Auto());
      g.AddColumnDefinition(GridLength::Star());
      g.SetColumnSpacing(10);
      View a = Cell(g, 0, 0), b = Cell(g, 0, 1);
      Fixed(a, 40, 10);
      View stage = Mount(r, g, 200, 50);
      r.AfterRender({stage, g, a, b}, [=](Run& r) {
        r.Rendered("auto", a, LayoutRect(0, 0, 40, 50));
        r.Rendered("star", b, LayoutRect(50, 0, 150, 50));
      });
    }));
    out.push_back(Single("G08.zero-star-overflow", "Zero star budget and fixed overflow", 8, [](Run& r) {
      auto g = GridLayout::New();
      Fixed(g, 40, 50);
      g.AddColumnDefinition(GridLength::Absolute(60));
      g.AddColumnDefinition(GridLength::Star(0));
      g.SetColumnSpacing(7);
      View a = Cell(g, 0, 0), b = Cell(g, 0, 1);
      View stage = Mount(r, g, 40, 50);
      r.AfterRender({stage, g, a, b}, [=](Run& r) {
        r.Rendered("absolute", a, LayoutRect(0, 0, 60, 50));
        r.Rendered("zero", b, LayoutRect(67, 0, 0, 50));
      });
    }));
    out.push_back(Steps("G04.implicit", "Implicit single track, then definitions replacement", {
      {"implicit", 4, [](Run& r) {
         auto s  = std::make_shared<Gridded>();
         s->grid = GridLayout::New();
         Fixed(s->grid, 100, 60);
         s->a     = Cell(s->grid, 0, 0);
         s->stage = Mount(r, s->grid, 100, 60);
         r.SetState(s);
         r.AfterRender({s->stage, s->grid, s->a}, [=](Run& r) { r.Rendered("implicit", s->a, LayoutRect(0, 0, 100, 60)); });
       }},
      {"explicit", 4, [](Run& r) {
         auto s = r.State<Gridded>();
         GridTracks(s->grid, {20}, {30});
         r.AfterRender({s->stage, s->grid, s->a}, [=](Run& r) { r.Rendered("explicit", s->a, LayoutRect(0, 0, 30, 20)); });
       }},
    }));
    out.push_back(Single("G14.mixed-span-deficit", "A span across an absolute and an auto track grows only the auto track", 8, [](Run& r) {
      auto g = GridLayout::New();
      g.AddColumnDefinition(GridLength::Absolute(30));
      g.AddColumnDefinition(GridLength::Auto());
      g.AddRowDefinition(GridLength::Absolute(20));
      g.AddRowDefinition(GridLength::Absolute(20));
      g.SetColumnSpacing(10);
      View span = Cell(g, 0, 0, 1, 2), b = Cell(g, 1, 1);
      Fixed(span, 100, 10);
      Fixed(b, 20, 10);
      View stage = Mount(r, g, 200, 60);
      // Default grid alignment is FILL, so each rendered rect is its cell: the span covers
      // 30 + 10 + 60 = 100 and the auto track is 60 wide after the deficit is added to it.
      r.AfterRender({stage, g, span, b}, [=](Run& r) {
        r.Rendered("span", span, LayoutRect(0, 0, 100, 20));
        r.Rendered("auto-track", b, LayoutRect(40, 20, 60, 20));
      });
    }));
    out.push_back(Single("G15.wrap-minimum-star", "A minimum width extends a WRAP_CONTENT grid and the star column takes the extra at arrange time", 10, [](Run& r) {
      auto g = GridLayout::New();
      Fixed(g, WRAP_CONTENT, 20);
      g.SetMinimumWidth(100);
      g.AddColumnDefinition(GridLength::Absolute(30));
      g.AddColumnDefinition(GridLength::Star(1));
      View a = Cell(g, 0, 0), b = Cell(g, 0, 1);
      View stage = Mount(r, g, 200, 20);
      r.AfterRender({stage, g, a, b}, [=](Run& r) {
        r.Size("minimum.measured", g.GetMeasuredSize(), MeasuredSize(100, 20));
        r.Rendered("minimum.a", a, LayoutRect(0, 0, 30, 20));
        r.Rendered("minimum.b", b, LayoutRect(30, 0, 70, 20));
      });
    }));
    // A MATCH_PARENT child of an ABSOLUTE cell is measured EXACTLY at the cell. In a STAR column of a
    // grid of determined width the width is a cell in Measure (300 - 50 = 250); a STAR row is never a
    // cell before the children are measured, so the height is measured exactly at its 60 cell in
    // Arrange.
    out.push_back(Single("G17.match-cell-measured", "MATCH_PARENT children report their cells: ABSOLUTE tracks and STAR columns in Measure, STAR rows in Arrange", 12, [](Run& r) {
      auto g = GridLayout::New();
      Fixed(g, 300, 100);
      g.AddColumnDefinition(GridLength::Absolute(50));
      g.AddColumnDefinition(GridLength::Star(1));
      g.AddRowDefinition(GridLength::Absolute(40));
      g.AddRowDefinition(GridLength::Star(1));
      View m1 = Cell(g, 0, 0), m2 = Cell(g, 1, 1);
      Fixed(m1, MATCH_PARENT, MATCH_PARENT);
      Fixed(m2, MATCH_PARENT, MATCH_PARENT);
      View stage = Mount(r, g, 300, 100);
      r.AfterRender({stage, g, m1, m2}, [=](Run& r) {
        r.Size("m1", m1.GetMeasuredSize(), MeasuredSize(50, 40));
        r.Rendered("m1.rendered", m1, LayoutRect(0, 0, 50, 40));
        r.Size("m2", m2.GetMeasuredSize(), MeasuredSize(250, 60));
        r.Rendered("m2.rendered", m2, LayoutRect(50, 40, 250, 60));
      });
    }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc109)
