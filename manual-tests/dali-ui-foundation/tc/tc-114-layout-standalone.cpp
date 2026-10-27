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
struct Tree
{
  View stage, root, a, b;
};
struct StandaloneProbe
{
  std::shared_ptr<Probe> probe;
  View                   stage, parent, ordinary, s;
};
} // namespace

class Tc114 : public Case
{
public:
  Tc114()
  : Case("TC114", "Standalone layout", "Check STANDALONE child is laid out independently")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    for(unsigned kind = 0; kind < 5; ++kind)
      out.push_back(Single(Id("S10.standalone", kind).c_str(), "Standalone is excluded from accumulation and rendered from its own requests", 11, [kind](Run& r) {
        View root = Container(kind);
        View ordinary = Leaf(20, 10, "ordinary"), standalone = Leaf(100, 80, "standalone");
        standalone.SetLayoutMode(LayoutMode::STANDALONE);
        standalone.SetRequestedX(7);
        standalone.SetRequestedY(9);
        root.Add(ordinary);
        root.Add(standalone);
        View stage = Mount(r, root, 200, 100);
        r.AfterRender({stage, root, ordinary}, [=](Run& r) {
          r.Size("parent-measured", root.GetMeasuredSize(), MeasuredSize(20, 10));
          r.Rendered("ordinary", ordinary, LayoutRect(0, 0, 20, 10));
          r.Rendered("independent", standalone, LayoutRect(7, 9, 100, 80));
          r.Equal("logical-count", root.GetChildViewCount(), 2);
        });
      }));
    out.push_back(Steps("G14.mode-change", "Changing the layout mode invalidates the parent intrinsic extent on screen", {
      {"ordinary", 6, [](Run& r) {
         auto s  = std::make_shared<Tree>();
         s->root = StackLayout::New(StackOrientation::HORIZONTAL);
         StackLayout::DownCast(s->root).SetSpacing(7);
         s->a = Leaf(20, 10, "a");
         s->b = Leaf(30, 15, "b");
         s->root.Add(s->a);
         s->root.Add(s->b);
         s->stage = Mount(r, s->root, 200, 100);
         r.SetState(s);
         r.AfterRender({s->stage, s->root, s->a, s->b}, [=](Run& r) {
           r.Size("ordinary", s->root.GetMeasuredSize(), MeasuredSize(57, 15));
           r.Rendered("ordinary.rendered", s->root, LayoutRect(0, 0, 57, 15));
         });
       }},
      {"standalone", 6, [](Run& r) {
         auto s = r.State<Tree>();
         s->b.SetLayoutMode(LayoutMode::STANDALONE);
         r.AfterRender({s->stage, s->root, s->a}, [=](Run& r) {
           r.Size("excluded", s->root.GetMeasuredSize(), MeasuredSize(20, 10));
           r.Rendered("excluded.rendered", s->root, LayoutRect(0, 0, 20, 10));
         });
       }},
    }));
    for(unsigned kind = 0; kind < 5; ++kind)
      out.push_back(Steps(Id("S10.match-margin-clamp", kind), "Standalone MATCH_PARENT ignores parent padding and applies its own margin, then its own min/max", {
        {"match", 4, [kind](Run& r) {
           auto s  = std::make_shared<Tree>();
           s->root = Container(kind, 200, 100);
           s->root.SetPadding(Insets(20, 30, 40, 10));
           s->a = Leaf(20, 10, "ordinary");
           s->b = Leaf(MATCH_PARENT, MATCH_PARENT, "standalone");
           s->b.SetLayoutMode(LayoutMode::STANDALONE);
           s->b.SetRequestedX(5);
           s->b.SetRequestedY(9);
           s->b.SetMargin(Insets(7, 11, 13, 17));
           s->root.Add(s->a);
           s->root.Add(s->b);
           s->stage = Mount(r, s->root, 200, 100);
           r.SetState(s);
           r.AfterRender({s->stage, s->root, s->a}, [=](Run& r) { r.Rendered("full-parent-minus-own-margin", s->b, LayoutRect(12, 22, 182, 70)); });
         }},
        {"clamp", 4, [](Run& r) {
           auto s = r.State<Tree>();
           s->b.SetMinimumWidth(190);
           s->b.SetMaximumWidth(195);
           s->b.SetMinimumHeight(20);
           s->b.SetMaximumHeight(60);
           r.AfterRender({s->b}, [=](Run& r) { r.Rendered("own-minmax-after-match", s->b, LayoutRect(12, 22, 190, 60)); });
         }},
      }));
    // MATCH_PARENT standalone: available = parent extent less own margin (195,143), measured EXACTLY
    // at that extent clamped by its own maximum (60,45), placed at the margin offset (5,7).
    out.push_back(Steps("S10.match-measured", "A MATCH_PARENT standalone child is measured exactly at its clamped slot", {
      {"mount", 6, [](Run& r) {
         auto s    = std::make_shared<StandaloneProbe>();
         s->parent = Container(0, 200, 150);
         s->s      = Leaf(MATCH_PARENT, MATCH_PARENT, "standalone");
         s->s.SetLayoutMode(LayoutMode::STANDALONE);
         s->s.SetMargin(Insets(5, 0, 7, 0));
         s->s.SetMaximumWidth(60);
         s->s.SetMaximumHeight(45);
         s->parent.Add(s->s);
         s->stage = Mount(r, s->parent, 200, 150);
         r.SetState(s);
         r.AfterRender({s->stage, s->parent, s->s}, [=](Run& r) {
           r.Rendered("s", s->s, LayoutRect(5, 7, 60, 45));
           r.Size("s.measured", s->s.GetMeasuredSize(), MeasuredSize(60, 45));
         });
       }},
      {"arrange-only", 6, [](Run& r) {
         auto s = r.State<StandaloneProbe>();
         s->parent.InvalidateArrange();
         r.AfterRender({s->stage, s->parent, s->s}, [=](Run& r) {
           r.Rendered("s", s->s, LayoutRect(5, 7, 60, 45));
           r.Size("s.measured", s->s.GetMeasuredSize(), MeasuredSize(60, 45));
         });
       }},
    }));
    // WRAP_CONTENT standalone in a window: measured against the window less its own margin, not
    // against its 20x10 parent. The window size is an input read at check time.
    out.push_back(Single("S10.wrap-window-bound", "A WRAP_CONTENT standalone child is bounded by the window less its own margin, not by its parent", 8, [](Run& r) {
      View parent = Container(0, 20, 10);
      auto probe  = Probed(r, 150, 90, false);
      probe->view.SetLayoutMode(LayoutMode::STANDALONE);
      probe->view.SetRequestedX(3);
      probe->view.SetRequestedY(4);
      probe->view.SetMargin(Insets(1, 2, 3, 4));
      parent.Add(probe->view);
      View stage = Mount(r, parent, 200, 100);
      r.AfterRender({stage, parent, probe->view}, [=](Run& r) {
        const auto  window = r.GetWindow().GetPositionSize();
        const float w = static_cast<float>(window.width) - 3, h = static_cast<float>(window.height) - 7;
        r.Near("s.last-width", probe->lastWidth, w);
        r.Near("s.last-height", probe->lastHeight, h);
        r.Size("s.measured", probe->view.GetMeasuredSize(), MeasuredSize(std::min(150.0f, w), std::min(90.0f, h)));
        r.Rendered("s", probe->view, LayoutRect(4, 7, std::min(150.0f, w), std::min(90.0f, h)));
      });
    }));
    // An explicit standalone axis is measured at its requested size, whatever the parent; the margin
    // is outside it. Position (5+2, 6+3).
    out.push_back(Single("S10.explicit-requested", "An explicit standalone child is measured at its requested size and does not affect its parent", 10, [](Run& r) {
      View parent = Container(0);
      View leaf   = Leaf(20, 10, "ordinary");
      auto probe  = Probed(r, 100, 80, false);
      probe->view.SetRequestedWidth(40);
      probe->view.SetRequestedHeight(30);
      probe->view.SetLayoutMode(LayoutMode::STANDALONE);
      probe->view.SetRequestedX(5);
      probe->view.SetRequestedY(6);
      probe->view.SetMargin(Insets(2, 0, 3, 0));
      parent.Add(leaf);
      parent.Add(probe->view);
      View stage = Mount(r, parent, 200, 100);
      r.AfterRender({stage, parent, leaf, probe->view}, [=](Run& r) {
        r.Near("s.last-width", probe->lastWidth, 40);
        r.Near("s.last-height", probe->lastHeight, 30);
        r.Size("s.measured", probe->view.GetMeasuredSize(), MeasuredSize(40, 30));
        r.Rendered("s", probe->view, LayoutRect(7, 9, 40, 30));
        r.Size("parent.measured", parent.GetMeasuredSize(), MeasuredSize(20, 10));
      });
    }));
    // Mixed axes: the WRAP_CONTENT width is bounded by the window (150 fits), the MATCH_PARENT height
    // is measured EXACTLY at the parent's height (120, then 200).
    out.push_back(Steps("S10.mixed-axes", "A standalone child's WRAP_CONTENT axis follows the window while its MATCH_PARENT axis follows the parent", {
      {"parent-120", 7, [](Run& r) {
         auto s    = std::make_shared<StandaloneProbe>();
         s->parent = Container(0, 100, 120);
         s->probe  = Probed(r, 150, 90, false);
         s->probe->view.SetRequestedHeight(MATCH_PARENT);
         s->probe->view.SetLayoutMode(LayoutMode::STANDALONE);
         s->parent.Add(s->probe->view);
         s->stage = Mount(r, s->parent, 300, 250);
         r.SetState(s);
         r.AfterRender({s->stage, s->parent, s->probe->view}, [=](Run& r) {
           const float width = std::min(150.0f, static_cast<float>(r.GetWindow().GetPositionSize().width));
           r.Size("s.measured", s->probe->view.GetMeasuredSize(), MeasuredSize(width, 120));
           r.Near("s.last-height", s->probe->lastHeight, 120);
           r.Rendered("s", s->probe->view, LayoutRect(0, 0, width, 120));
         });
       }},
      {"parent-200", 7, [](Run& r) {
         auto s = r.State<StandaloneProbe>();
         s->parent.SetRequestedHeight(200);
         r.AfterRender({s->stage, s->parent, s->probe->view}, [=](Run& r) {
           const float width = std::min(150.0f, static_cast<float>(r.GetWindow().GetPositionSize().width));
           r.Size("s.measured", s->probe->view.GetMeasuredSize(), MeasuredSize(width, 200));
           r.Near("s.last-height", s->probe->lastHeight, 200);
           r.Rendered("s", s->probe->view, LayoutRect(0, 0, width, 200));
         });
       }},
    }));
    // The parent's measure hands the MATCH_PARENT standalone child its PROVISIONAL size (the 200x100
    // budget of a WRAP_CONTENT parent); the placement re-measures it at the parent's final 120x60.
    out.push_back(Single("S10.placement-remeasure", "A MATCH_PARENT standalone child is re-measured when its parent's final size differs from the provisional one", 10, [](Run& r) {
      View parent = Container(0);
      View leaf   = Leaf(120, 60, "ordinary");
      auto probe  = Probed(r, 37, 19, false);
      probe->view.SetRequestedWidth(MATCH_PARENT);
      probe->view.SetRequestedHeight(MATCH_PARENT);
      probe->view.SetLayoutMode(LayoutMode::STANDALONE);
      parent.Add(leaf);
      parent.Add(probe->view);
      View stage = Mount(r, parent, 200, 100);
      r.AfterRender({stage, parent, leaf, probe->view}, [=](Run& r) {
        r.Rendered("s", probe->view, LayoutRect(0, 0, 120, 60));
        r.Size("s.measured", probe->view.GetMeasuredSize(), MeasuredSize(120, 60));
        r.Near("s.last-width", probe->lastWidth, 120);
        r.Near("s.last-height", probe->lastHeight, 60);
        r.Size("parent.measured", parent.GetMeasuredSize(), MeasuredSize(120, 60));
      });
    }));
    // STANDALONE -> DEFAULT in the same action as a change that queued the child's own turn: the
    // child is laid out by the stack after a (20 + 7) and counts in the stack's measured size. The
    // released root registration itself is controller state and is not observed.
    out.push_back(Steps("G14.mode-release", "Switching a standalone child back to DEFAULT in the batch that queued its own turn lays it out as an ordinary stack child", {
      {"standalone", 6, [](Run& r) {
         auto s    = std::make_shared<StandaloneProbe>();
         s->parent = StackLayout::New(StackOrientation::HORIZONTAL);
         StackLayout::DownCast(s->parent).SetSpacing(7);
         s->ordinary = Leaf(20, 10, "a");
         s->s        = Leaf(30, 15, "standalone");
         s->s.SetLayoutMode(LayoutMode::STANDALONE);
         s->s.SetRequestedX(50);
         s->s.SetRequestedY(40);
         s->parent.Add(s->ordinary);
         s->parent.Add(s->s);
         s->stage = Mount(r, s->parent, 200, 100);
         r.SetState(s);
         r.AfterRender({s->stage, s->parent, s->ordinary, s->s}, [=](Run& r) {
           r.Rendered("s", s->s, LayoutRect(50, 40, 30, 15));
           r.Size("root.measured", s->parent.GetMeasuredSize(), MeasuredSize(20, 10));
         });
       }},
      {"release", 6, [](Run& r) {
         auto s = r.State<StandaloneProbe>();
         s->s.SetRequestedWidth(35);
         s->s.SetLayoutMode(LayoutMode::DEFAULT);
         r.AfterRender({s->stage, s->parent, s->ordinary, s->s}, [=](Run& r) {
           r.Rendered("s", s->s, LayoutRect(27, 0, 35, 15));
           r.Size("root.measured", s->parent.GetMeasuredSize(), MeasuredSize(62, 15));
         });
       }},
    }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc114)
