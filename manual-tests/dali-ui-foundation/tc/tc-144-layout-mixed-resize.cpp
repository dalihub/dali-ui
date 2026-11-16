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
struct Mixed
{
  StackLayout       root;
  GridLayout        grid;
  FlexLayout        flex;
  AbsoluteLayout    absolute;
  std::vector<View> cells;
  std::vector<View> flexItems;
  View              proportional;
  View              negative;
  std::vector<View> Nodes() const
  {
    std::vector<View> nodes{root, grid, flex, absolute};
    nodes.insert(nodes.end(), cells.begin(), cells.end());
    nodes.insert(nodes.end(), flexItems.begin(), flexItems.end());
    nodes.push_back(proportional);
    nodes.push_back(negative);
    return nodes;
  }
};
std::shared_ptr<Mixed> MakeMixed()
{
  auto s  = std::make_shared<Mixed>();
  s->root = StackLayout::New(StackOrientation::VERTICAL);
  s->root.SetLayoutDirection(Dali::LayoutDirection::LEFT_TO_RIGHT);
  s->root.SetRequestedHeight(WRAP_CONTENT);
  s->root.SetPadding(Insets(7, 11, 5, 9));
  s->root.SetSpacing(13);
  s->grid = GridLayout::New();
  s->grid.SetRequestedWidth(MATCH_PARENT);
  s->grid.SetRequestedHeight(90);
  s->grid.SetColumnSpacing(4);
  s->grid.SetRowSpacing(3);
  s->grid.AddColumnDefinition(GridLength::Absolute(40));
  s->grid.AddColumnDefinition(GridLength::Star(1));
  s->grid.AddColumnDefinition(GridLength::Star(2));
  s->grid.AddRowDefinition(GridLength::Absolute(20));
  s->grid.AddRowDefinition(GridLength::Star(1));
  for(uint32_t i = 0; i < 6; ++i)
  {
    auto cell = Leaf(MATCH_PARENT, MATCH_PARENT, Id("cell", i).c_str());
    cell.SetLayoutParams(GridLayoutParams::New().SetRow(i / 3).SetColumn(i % 3));
    s->grid.Add(cell);
    s->cells.push_back(cell);
  }
  s->flex = FlexLayout::New();
  s->flex.SetDirection(FlexDirection::ROW);
  s->flex.SetWrap(FlexWrap::NO_WRAP);
  s->flex.SetJustifyContent(FlexJustify::SPACE_BETWEEN);
  s->flex.SetAlignItems(FlexAlign::CENTER);
  s->flex.SetRequestedWidth(MATCH_PARENT);
  s->flex.SetRequestedHeight(60);
  for(uint32_t i = 0; i < 3; ++i)
  {
    auto item = Leaf(20 + 10 * i, 10, Id("flex", i).c_str());
    item.SetLayoutParams(FlexLayoutParams::New().SetFlexShrink(0));
    s->flex.Add(item);
    s->flexItems.push_back(item);
  }
  s->absolute = AbsoluteLayout::New();
  s->absolute.SetRequestedWidth(MATCH_PARENT);
  s->absolute.SetRequestedHeight(70);
  s->proportional = Leaf(20, 10, "proportional");
  s->proportional.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(.5f, 1, 20, 10)).SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL));
  s->negative = Leaf(30, 12, "negative");
  s->negative.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(-7, 4, 30, 12)));
  s->absolute.Add(s->proportional);
  s->absolute.Add(s->negative);
  s->root.Add(s->grid);
  s->root.Add(s->flex);
  s->root.Add(s->absolute);
  return s;
}
// 60 controller-snapshot rows plus 60 rendered rows for the same nodes.
void CheckMixed(Run& r, Mixed& s, float width)
{
  const float c     = width - 18;
  const float unit  = (c - 48) / 3;
  const float xs[]  = {0, 44, 48 + unit};
  const float ws[]  = {40, unit, 2 * unit};
  const float gap   = (c - 90) / 2;
  auto        check = [&](const std::string& id, View view, const LayoutRect& expected) {
    r.Rect(id.c_str(), r.Snapshot(view), expected);
    r.Rendered((id + ".rendered").c_str(), view, expected);
  };
  check("root.window", s.root, LayoutRect(0, 0, width, 260));
  check("grid", s.grid, LayoutRect(7, 5, c, 90));
  check("flex", s.flex, LayoutRect(7, 108, c, 60));
  check("absolute", s.absolute, LayoutRect(7, 181, c, 70));
  for(uint32_t i = 0; i < 6; ++i) check(Id("cell", i), s.cells[i], LayoutRect(xs[i % 3], i < 3 ? 0 : 23, ws[i % 3], i < 3 ? 20 : 67));
  check("flex.0", s.flexItems[0], LayoutRect(0, 25, 20, 10));
  check("flex.1", s.flexItems[1], LayoutRect(20 + gap, 25, 30, 10));
  check("flex.2", s.flexItems[2], LayoutRect(c - 40, 25, 40, 10));
  check("absolute.proportional", s.proportional, LayoutRect((c - 20) * .5f, 60, 20, 10));
  check("absolute.negative", s.negative, LayoutRect(-7, 4, 30, 12));
}
} // namespace

class Tc144 : public Case
{
public:
  Tc144()
  : Case("TC144", "Mixed resize", "Check mixed tree resize round trip has no drift")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    Scenario window{"window-roundtrip", "Rendered geometry of four nested layout managers across A-B-C-A root widths", {}};
    for(float width : {160.f, 263.f, 512.f, 160.f})
    {
      const bool first = window.steps.empty();
      window.steps.push_back({first ? "mount-160" : ("resize-" + std::to_string(static_cast<int>(width))), 120, [width, first](Run& r) {
                                auto s = first ? MakeMixed() : r.State<Mixed>();
                                if(first) r.SetState(s);
                                r.AfterRender(s->Nodes(), [s, width](Run& done) { CheckMixed(done, *s, width); });
                                s->root.SetRequestedWidth(width);
                                if(first) r.Attach(s->root);
                              }});
    }
    return {window};
  }
};

REGISTER_MANUAL_TEST(Tc144)
