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
const char* KindName(unsigned kind)
{
  switch(kind)
  {
    case 1:
      return "stack";
    case 2:
      return "flex";
    case 3:
      return "grid";
    case 4:
      return "absolute";
    default:
      return "view";
  }
}
struct StablePixelFixture
{
  View stage, box;
};
} // namespace

// Verifies pixels from a separate subtree render task for each layout manager: three solid
// 40x30 boxes are laid out by a 200x100 container on the stage, the rendered rectangles are
// checked, and captured subtree pixels at each box centre and at the container corner must
// carry the box and container colours.
class Tc157 : public Case
{
public:
  Tc157()
  : Case("TC157", "Rendered pixels", "Check laid-out boxes render at expected pixels")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    for(unsigned kind = 0; kind < 5; ++kind)
      out.push_back(Single((std::string("P01.pixels.") + KindName(kind)).c_str(), "Three coloured boxes laid out by the manager are rendered at the expected pixels", 16, [kind](Run& r) {
        View container = Container(kind, 200, 100);
        if(kind == 1) StackLayout::DownCast(container).SetSpacing(20);
        if(kind == 2) FlexLayout::DownCast(container).SetAlignItems(FlexAlign::FLEX_START);
        if(kind == 3)
        {
          GridLayout grid = GridLayout::DownCast(container);
          GridTracks(grid, {100}, {40, 40, 40});
          grid.SetColumnSpacing(20);
        }
        std::vector<View>       boxes;
        std::vector<LayoutRect> expected;
        for(unsigned i = 0; i < 3; ++i)
        {
          View box = Leaf(40, 30, Id("box", i).c_str());
          box.SetBackgroundColor(Palette(i));
          const float x = kind == 2 ? 40.0f * i : (kind == 0 || kind == 4) ? 10.0f + 60.0f * i : 60.0f * i;
          const float y = (kind == 0 || kind == 4) ? 10.0f : 0.0f;
          if(kind == 0)
          {
            box.SetRequestedX(x);
            box.SetRequestedY(y);
          }
          if(kind == 3) box.SetLayoutParams(GridLayoutParams::New().SetColumn(i).SetVerticalAlignment(LayoutAlignment::START).SetHorizontalAlignment(LayoutAlignment::START));
          if(kind == 4) box.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(x, y, 40, 30)));
          container.Add(box);
          boxes.push_back(box);
          expected.emplace_back(x, y, 40, 30);
        }
        View stage = Mount(r, container, 200, 100);
        r.AfterRender({stage, container, boxes[0], boxes[1], boxes[2]}, [=](Run& r) {
          std::vector<PixelProbe> probes;
          for(unsigned i = 0; i < 3; ++i)
          {
            r.Rendered(Id("box", i).c_str(), boxes[i], expected[i]);
            probes.push_back({Id("box", i), expected[i].x + 20, expected[i].y + 15, Palette(i)});
          }
          probes.push_back({"container", 195, 95, FixtureColor(kind)});
          r.CapturePixels(stage, probes, [](Run&) {});
        });
      }));
    out.push_back({"P02.same-geometry-color", "A new color must reach a fresh frame without a layout change", {{"mount", 5, [](Run& r)
    {
      auto s   = std::make_shared<StablePixelFixture>();
      s->stage = r.Stage(80, 60);
      s->box   = Leaf(40, 30);
      s->box.SetRequestedX(10);
      s->box.SetRequestedY(10);
      s->box.SetBackgroundColor(Palette(0));
      s->stage.Add(s->box);
      r.SetState(s);
      r.AfterRender({s->stage, s->box}, [s](Run& done)
      {
        done.Rendered("stable.box", s->box, {10, 10, 40, 30});
        done.CapturePixels(s->stage, {{"stable.initial", 30, 25, Palette(0)}}, [](Run&) {});
      });
    }},
                                                                                                               {"color-only", 5, [](Run& r)
    {
      auto s = r.State<StablePixelFixture>();
      s->box.SetBackgroundColor(Palette(1));
      r.AfterFrame([s](Run& done)
      {
        done.Rendered("stable.box", s->box, {10, 10, 40, 30});
        done.CapturePixels(s->stage, {{"stable.changed", 30, 25, Palette(1)}}, [](Run&) {});
      }, {}, s->stage);
    }}}});
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc157)
