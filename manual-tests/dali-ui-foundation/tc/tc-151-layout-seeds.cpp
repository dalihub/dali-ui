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
// Linear congruential generator shared by every seeded fixture (fixed algorithm, integer only).
inline uint32_t NextRandom(uint32_t& state)
{
  state = state * 1664525u + 1013904223u;
  return state;
}
struct Seeded
{
  View                    stage, primary, transposed;
  std::vector<View>       children, swapped;
  std::vector<float>      widths, heights;
  std::vector<LayoutRect> expected;
  float                   total{0}, maxCross{0}, width{0}, height{0};
  std::vector<View>       Fence() const
  {
    std::vector<View> views{stage, primary};
    views.insert(views.end(), children.begin(), children.end());
    return views;
  }
  std::vector<View> TransposedFence() const
  {
    std::vector<View> views{stage, transposed};
    views.insert(views.end(), swapped.begin(), swapped.end());
    return views;
  }
};
} // namespace

class Tc151 : public Case
{
public:
  Tc151()
  : Case("TC151", "Deterministic seeds", "Check seeded layouts under RTL/move/transpose")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    for(uint32_t seed : {1u, 17u, 73u, 257u, 65537u})
      out.push_back(Single(Id("R01.seed", seed).c_str(), "Seeded stack conservation and axis metamorphism rendered on screen", 130, [seed](Run& r) {
        auto s      = std::make_shared<Seeded>();
        auto h      = StackLayout::New(StackOrientation::HORIZONTAL);
        auto v      = StackLayout::New(StackOrientation::VERTICAL);
        s->primary  = h;
        s->transposed = v;
        h.SetSpacing(3);
        v.SetSpacing(3);
        uint32_t state = seed;
        for(unsigned i = 0; i < 16; ++i)
        {
          const float w  = 1 + (NextRandom(state) % 47);
          const float he = 1 + (NextRandom(state) % 31);
          s->widths.push_back(w);
          s->heights.push_back(he);
          s->children.push_back(Leaf(w, he, Id("h", i).c_str()));
          s->swapped.push_back(Leaf(he, w, Id("v", i).c_str()));
          h.Add(s->children.back());
          v.Add(s->swapped.back());
          s->total += w;
          s->maxCross = std::max(s->maxCross, he);
        }
        s->total += 45;
        v.SetRequestedY(s->maxCross + 10);
        s->stage = r.Stage(s->total, s->maxCross + 10 + s->total);
        s->stage.Add(h);
        s->stage.Add(v);
        auto fence = s->Fence();
        fence.push_back(v);
        fence.insert(fence.end(), s->swapped.begin(), s->swapped.end());
        r.AfterRender(fence, [=](Run& r) {
          r.Size("intrinsic", s->primary.GetMeasuredSize(), MeasuredSize(s->total, s->maxCross));
          float position = 0;
          for(unsigned i = 0; i < 16; ++i)
          {
            r.Rendered(Id("horizontal", i).c_str(), s->children[i], LayoutRect(position, 0, s->widths[i], s->heights[i]));
            r.Rendered(Id("vertical", i).c_str(), s->swapped[i], LayoutRect(0, position, s->heights[i], s->widths[i]));
            position += s->widths[i] + 3;
          }
        });
      }));
    for(uint32_t seed : {1u, 73u, 65537u})
    {
      auto flexForward = [](Run& r, const std::shared_ptr<Seeded>& s, const char* prefix) {
        float x = 0;
        for(unsigned i = 0; i < 8; ++i)
        {
          r.Rendered(Id(prefix, i).c_str(), s->children[i], LayoutRect(x, 0, s->widths[i] + 15, s->heights[i]));
          x += s->widths[i] + 15;
        }
      };
      out.push_back(Steps(Id("R02.flex-seed", seed), "Seeded grow conservation, reversal, RTL, translation and transpose rendered on screen", {
        {"forward", 32, [seed](Run& r) {
           auto     s     = std::make_shared<Seeded>();
           uint32_t state = seed;
           auto     f = FlexLayout::New(), v = FlexLayout::New();
           f.SetAlignItems(FlexAlign::FLEX_START);
           v.SetAlignItems(FlexAlign::FLEX_START);
           v.SetDirection(FlexDirection::COLUMN);
           s->total = 120;
           for(unsigned i = 0; i < 8; ++i)
           {
             const float w = 1 + NextRandom(state) % 47;
             const float h = 1 + NextRandom(state) % 31;
             s->widths.push_back(w);
             s->heights.push_back(h);
             s->total += w;
             View a = Leaf(w, h, Id("a", i).c_str()), b = Leaf(h, w, Id("b", i).c_str());
             a.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(w).SetFlexGrow(1));
             b.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(w).SetFlexGrow(1));
             f.Add(a);
             v.Add(b);
             s->children.push_back(a);
             s->swapped.push_back(b);
           }
           Fixed(f, s->total, 80);
           Fixed(v, 80, s->total);
           v.SetRequestedY(100);
           s->primary    = f;
           s->transposed = v;
           s->stage      = r.Stage(s->total + 40, 100 + s->total);
           s->stage.Add(f);
           s->stage.Add(v);
           r.SetState(s);
           r.AfterRender(s->Fence(), [=](Run& r) {
             float x = 0;
             for(unsigned i = 0; i < 8; ++i)
             {
               r.Rendered(Id("flex.forward", i).c_str(), s->children[i], LayoutRect(x, 0, s->widths[i] + 15, s->heights[i]));
               x += s->widths[i] + 15;
             }
           });
         }},
        {"reverse", 32, [](Run& r) {
           auto s = r.State<Seeded>();
           FlexLayout::DownCast(s->primary).SetDirection(FlexDirection::ROW_REVERSE);
           r.AfterRender(s->Fence(), [=](Run& r) {
             float x = 0;
             for(unsigned i = 0; i < 8; ++i)
             {
               r.Rendered(Id("flex.reverse", i).c_str(), s->children[i], LayoutRect(s->total - x - s->widths[i] - 15, 0, s->widths[i] + 15, s->heights[i]));
               x += s->widths[i] + 15;
             }
           });
         }},
        {"reverse-rtl", 32, [flexForward](Run& r) {
           auto s = r.State<Seeded>();
           s->primary.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
           r.AfterRender(s->Fence(), [=](Run& r) { flexForward(r, s, "flex.reverse-rtl"); });
         }},
        {"translated-parent", 32, [flexForward](Run& r) {
           auto s = r.State<Seeded>();
           s->primary.SetMargin(Insets(31, 0, 17, 0));
           r.AfterRender(s->Fence(), [=](Run& r) { flexForward(r, s, "flex.translated-parent"); });
         }},
        {"transposed", 32, [](Run& r) {
           auto s = r.State<Seeded>();
           s->transposed.InvalidateArrange(); // guarantees a pass that arranges (replays) the transposed tree
           r.AfterRender(s->TransposedFence(), [=](Run& r) {
             float x = 0;
             for(unsigned i = 0; i < 8; ++i)
             {
               r.Rendered(Id("flex.transposed", i).c_str(), s->swapped[i], LayoutRect(0, x, s->heights[i], s->widths[i] + 15));
               x += s->widths[i] + 15;
             }
           });
         }},
      }));
      out.push_back(Steps(Id("R04.grid-seed", seed), "Seeded prefix tracks, RTL, translation and transpose rendered on screen", {
        {"ltr", 48, [seed](Run& r) {
           auto               s     = std::make_shared<Seeded>();
           uint32_t           state = seed;
           std::vector<float> rows, columns;
           s->height = 9;
           s->width  = 10;
           for(unsigned i = 0; i < 4; ++i)
           {
             rows.push_back(20 + NextRandom(state) % 20);
             s->height += rows.back();
           }
           for(unsigned i = 0; i < 3; ++i)
           {
             columns.push_back(15 + NextRandom(state) % 20);
             s->width += columns.back();
           }
           auto g = GridLayout::New(), transpose = GridLayout::New();
           GridTracks(g, rows, columns);
           g.SetRowSpacing(3);
           g.SetColumnSpacing(5);
           GridTracks(transpose, columns, rows);
           transpose.SetRowSpacing(5);
           transpose.SetColumnSpacing(3);
           float y = 0;
           for(unsigned row = 0; row < 4; ++row)
           {
             float x = 0;
             for(unsigned col = 0; col < 3; ++col)
             {
               s->children.push_back(Cell(g, row, col));
               s->swapped.push_back(Cell(transpose, col, row));
               s->expected.emplace_back(x, y, columns[col], rows[row]);
               x += columns[col] + 5;
             }
             y += rows[row] + 3;
           }
           Fixed(g, s->width, s->height);
           Fixed(transpose, s->height, s->width);
           transpose.SetRequestedY(s->height + 30);
           s->primary    = g;
           s->transposed = transpose;
           s->stage      = r.Stage(s->width + 40, s->height + 30 + s->width);
           s->stage.Add(g);
           s->stage.Add(transpose);
           r.SetState(s);
           r.AfterRender(s->Fence(), [=](Run& r) {
             for(unsigned i = 0; i < 12; ++i) r.Rendered(Id("grid.ltr", i).c_str(), s->children[i], s->expected[i]);
           });
         }},
        {"rtl", 48, [](Run& r) {
           auto s = r.State<Seeded>();
           s->primary.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
           r.AfterRender(s->Fence(), [=](Run& r) {
             for(unsigned i = 0; i < 12; ++i)
             {
               auto e = s->expected[i];
               e.x    = s->width - e.x - e.width;
               r.Rendered(Id("grid.rtl", i).c_str(), s->children[i], e);
             }
           });
         }},
        {"translated-parent", 48, [](Run& r) {
           auto s = r.State<Seeded>();
           s->primary.SetMargin(Insets(31, 0, 17, 0));
           r.AfterRender(s->Fence(), [=](Run& r) {
             for(unsigned i = 0; i < 12; ++i)
             {
               auto e = s->expected[i];
               e.x    = s->width - e.x - e.width;
               r.Rendered(Id("grid.translated-parent", i).c_str(), s->children[i], e);
             }
           });
         }},
        {"transposed", 48, [](Run& r) {
           auto s = r.State<Seeded>();
           s->transposed.InvalidateArrange(); // guarantees a pass that arranges (replays) the transposed tree
           r.AfterRender(s->TransposedFence(), [=](Run& r) {
             for(unsigned i = 0; i < 12; ++i) r.Rendered(Id("grid.transposed", i).c_str(), s->swapped[i], SwapAxes(s->expected[i]));
           });
         }},
      }));
      out.push_back(Steps(Id("R05.absolute-seed", seed), "Seeded bounds and proportional positions with rigid transformations rendered on screen", {
        {"ltr", 48, [seed](Run& r) {
           auto     s     = std::make_shared<Seeded>();
           uint32_t state = seed;
           auto     a = AbsoluteLayout::New(), transpose = AbsoluteLayout::New();
           Fixed(a, 200, 120);
           Fixed(transpose, 120, 200);
           for(unsigned i = 0; i < 12; ++i)
           {
             const float w     = 1 + NextRandom(state) % 47;
             const float h     = 1 + NextRandom(state) % 31;
             const float x     = (i % 2) ? (static_cast<int>(NextRandom(state) % 7) - 1) * 0.25f : static_cast<int>(NextRandom(state) % 50) - 10;
             const float y     = static_cast<int>(NextRandom(state) % 30) - 5;
             View        child = Leaf(w, h, Id("a", i).c_str()), other = Leaf(h, w, Id("t", i).c_str());
             child.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(x, y, w, h)).SetFlags((i % 2) ? AbsoluteLayoutFlags::X_PROPORTIONAL : AbsoluteLayoutFlags::NONE));
             other.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(y, x, h, w)).SetFlags((i % 2) ? AbsoluteLayoutFlags::Y_PROPORTIONAL : AbsoluteLayoutFlags::NONE));
             a.Add(child);
             transpose.Add(other);
             s->children.push_back(child);
             s->swapped.push_back(other);
             s->expected.emplace_back((i % 2) ? (200 - w) * x : x, y, w, h);
           }
           transpose.SetRequestedY(150);
           s->primary    = a;
           s->transposed = transpose;
           s->stage      = r.Stage(240, 350);
           s->stage.Add(a);
           s->stage.Add(transpose);
           r.SetState(s);
           r.AfterRender(s->Fence(), [=](Run& r) {
             for(unsigned i = 0; i < 12; ++i) r.Rendered(Id("absolute.ltr", i).c_str(), s->children[i], s->expected[i]);
           });
         }},
        {"translated-parent", 48, [](Run& r) {
           auto s = r.State<Seeded>();
           s->primary.SetMargin(Insets(31, 0, 17, 0));
           r.AfterRender(s->Fence(), [=](Run& r) {
             for(unsigned i = 0; i < 12; ++i) r.Rendered(Id("absolute.translated-parent", i).c_str(), s->children[i], s->expected[i]);
           });
         }},
        {"rtl", 48, [](Run& r) {
           auto s = r.State<Seeded>();
           s->primary.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
           r.AfterRender(s->Fence(), [=](Run& r) {
             for(unsigned i = 0; i < 12; ++i)
             {
               auto e = s->expected[i];
               e.x    = 200 - e.x - e.width;
               r.Rendered(Id("absolute.rtl", i).c_str(), s->children[i], e);
             }
           });
         }},
        {"transposed", 48, [](Run& r) {
           auto s = r.State<Seeded>();
           s->transposed.InvalidateArrange(); // guarantees a pass that arranges (replays) the transposed tree
           r.AfterRender(s->TransposedFence(), [=](Run& r) {
             for(unsigned i = 0; i < 12; ++i) r.Rendered(Id("absolute.transposed", i).c_str(), s->swapped[i], SwapAxes(s->expected[i]));
           });
         }},
      }));
    }
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc151)
