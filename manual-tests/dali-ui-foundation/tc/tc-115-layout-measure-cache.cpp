/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include <dali-ui-foundation/public-api/views/scroll/scroll-view.h>
#include "layout-validation-fixtures.h"

using namespace LayoutValidation;
using namespace LayoutValidation::Fixtures;

namespace
{
// A probed leaf plus a sibling leaf; nudging the sibling forces a controller pass that
// does not invalidate the probe, so the probe's measure cache decides whether it runs.
struct Cached
{
  std::shared_ptr<Probe> probe, second;
  View                   stage, sibling, stack;
  uint32_t               measures{0}, secondMeasures{0};
  float                  nudge{10};
  void                   Nudge()
  {
    sibling.SetRequestedWidth(nudge += 1);
  }
};
std::shared_ptr<Cached> MountProbe(Run& r, float width, float height)
{
  auto s     = std::make_shared<Cached>();
  s->probe   = Probed(r);
  s->sibling = Leaf(10, 10, "sibling");
  s->sibling.SetRequestedY(60);
  s->stage = r.Stage(width, height);
  s->stage.Add(s->probe->view);
  s->stage.Add(s->sibling);
  r.SetState(s);
  return s;
}
// One 30x10 probe as the only child of a 100x60 container of kind k (0 plain View, 1 vertical stack
// with a MATCH_PARENT width and weight 1, 2 vertical stack with FILL, 3 flex, 4 grid with one 100x60
// ABSOLUTE cell, 5 absolute layout with 50x40 bounds, 6 vertical ScrollView whose content is the
// probe with a MATCH_PARENT width) and what the pass measures it at.
struct ApplicationMeasure
{
  std::shared_ptr<Probe> probe;
  View                   stage, container;
  MeasuredSize           constraint, measured;
  LayoutRect             rendered;
};
std::shared_ptr<ApplicationMeasure> MountApplicationMeasure(Run& r, unsigned kind)
{
  auto s   = std::make_shared<ApplicationMeasure>();
  s->probe = Probed(r, 30, 10, false);
  View& c  = s->container;
  View  p  = s->probe->view;
  switch(kind)
  {
    case 0:
      c = Container(0, 100, 60);
      Fixed(p, MATCH_PARENT, MATCH_PARENT);
      s->measured = MeasuredSize(100, 60);
      break;
    case 1:
      c = StackLayout::New(StackOrientation::VERTICAL);
      p.SetRequestedWidth(MATCH_PARENT);
      p.SetLayoutParams(StackLayoutParams::New().SetWeight(1));
      s->measured = MeasuredSize(100, 60);
      break;
    case 2:
      c = StackLayout::New(StackOrientation::VERTICAL);
      p.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
      s->measured = MeasuredSize(100, 10);
      break;
    case 3:
      c           = Container(2);
      s->measured = MeasuredSize(30, 60);
      break;
    case 4:
      c = Container(3);
      GridTracks(GridLayout::DownCast(c), {60}, {100});
      s->measured = MeasuredSize(100, 60);
      break;
    case 5:
      c = Container(4);
      p.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(0, 0, 50, 40)));
      s->measured = MeasuredSize(50, 40);
      break;
    default:
    {
      // The scroll layout measures a MATCH_PARENT axis EXACTLY at the viewport and a WRAP_CONTENT
      // axis UNSPECIFIED at FLT_MAX: the probe reports the viewport width and its own height.
      ScrollView scroll = ScrollView::New();
      scroll.SetScrollDirection(ScrollDirection::Vertical);
      scroll.SetPanScrollEnabled(false);
      scroll.SetHorizontalScrollBarVisibility(ScrollBarVisibility::Never);
      scroll.SetVerticalScrollBarVisibility(ScrollBarVisibility::Never);
      c = scroll;
      p.SetRequestedWidth(MATCH_PARENT);
      s->measured = MeasuredSize(100, 10);
      break;
    }
  }
  Fixed(c, 100, 60);
  s->constraint = kind == 5 ? MeasuredSize(50, 40) : kind == 6 ? MeasuredSize(100, std::numeric_limits<float>::max()) : MeasuredSize(100, 60);
  s->rendered   = LayoutRect(0, 0, s->measured.width, s->measured.height);
  if(ScrollView scroll = ScrollView::DownCast(c))
  {
    scroll.SetContent(p);
  }
  else
  {
    c.Add(p);
  }
  s->stage = Mount(r, c, 100, 60);
  r.SetState(s);
  return s;
}
// The public mode-less Measure derives the modes the parent's layout passed in the pass, so an
// application call with the pass's numbers is a cache hit for every first-party parent kind. The
// probe is a callback leaf, so a cache hit is exactly a call that runs no producer.
Scenario ApplicationMeasureCase(unsigned kind)
{
  return Steps(Id("K11.application-measure", kind), "An application Measure with the pass's numbers derives the pass's modes and is served from the cache", {
    {"pass", 7, [kind](Run& r) {
       auto s = MountApplicationMeasure(r, kind);
       r.AfterRender({s->stage, s->container, s->probe->view}, [=](Run& r) {
         r.Size("measured", s->probe->view.GetMeasuredSize(), s->measured);
         r.Equal("producer-once", s->probe->measures, 1);
         r.Rendered("rendered", s->probe->view, s->rendered);
       });
     }},
    {"app-hit", 3, [](Run& r) {
       auto               s        = r.State<ApplicationMeasure>();
       const uint32_t     before   = s->probe->measures;
       const MeasuredSize returned = s->probe->view.Measure(s->constraint.width, s->constraint.height);
       r.Size("returned", returned, s->measured);
       r.Equal("producer-unchanged", s->probe->measures, before);
     }},
  });
}
} // namespace

class Tc115 : public Case
{
public:
  Tc115()
  : Case("TC115", "Measure cache", "Check measure cache hits, misses and invalidation")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    out.push_back(Steps("K01.same-key", "Cold miss, identical-key hit through an unrelated pass, and a height-key miss", {
      {"cold", 7, [](Run& r) {
         auto s = MountProbe(r, 100, 50);
         r.AfterRender({s->stage, s->probe->view, s->sibling}, [=](Run& r) {
           r.Size("cold", s->probe->view.GetMeasuredSize(), MeasuredSize(37, 19));
           r.Equal("producer-once", s->probe->measures, 1);
           r.Rendered("cold.rendered", s->probe->view, LayoutRect(0, 0, 37, 19));
         });
       }},
      {"hit", 3, [](Run& r) {
         auto s = r.State<Cached>();
         s->Nudge();
         r.AfterRender({s->stage, s->probe->view, s->sibling}, [=](Run& r) {
           r.Size("hit", s->probe->view.GetMeasuredSize(), MeasuredSize(37, 19));
           r.Equal("hit-producer", s->probe->measures, 1);
         });
       }},
      {"height-key", 1, [](Run& r) {
         auto s = r.State<Cached>();
         s->stage.SetRequestedHeight(51);
         r.AfterRender({s->stage, s->probe->view, s->sibling}, [=](Run& r) { r.Equal("height-key", s->probe->measures, 2); });
       }},
    }));
    out.push_back(Steps("K02.normalized-key", "Different raw constraints normalize to one key", {
      {"mount", 8, [](Run& r) {
         auto s = MountProbe(r, 40, 100);
         s->probe->view.SetMinimumWidth(120);
         s->probe->view.SetMaximumHeight(30);
         r.AfterRender({s->stage, s->probe->view, s->sibling}, [=](Run& r) {
           r.Near("normalized-width", s->probe->lastWidth, 120);
           r.Near("normalized-height", s->probe->lastHeight, 30);
           r.Size("clamped-result", s->probe->view.GetMeasuredSize(), MeasuredSize(120, 19));
           r.Rendered("clamped.rendered", s->probe->view, LayoutRect(0, 0, 120, 19));
         });
       }},
      {"resize", 1, [](Run& r) {
         auto s = r.State<Cached>();
         s->stage.SetRequestedWidth(50);
         s->stage.SetRequestedHeight(200);
         r.AfterRender({s->stage, s->probe->view, s->sibling}, [=](Run& r) { r.Equal("producer-once", s->probe->measures, 1); });
       }},
    }));
    out.push_back(Steps("K03.explicit-invalidation", "External content requires an explicit invalidation to reach the screen", {
      {"mount", 5, [](Run& r) {
         auto s = MountProbe(r, 100, 50);
         r.AfterRender({s->stage, s->probe->view, s->sibling}, [=](Run& r) {
           r.Equal("producer", s->probe->measures, 1);
           r.Rendered("initial", s->probe->view, LayoutRect(0, 0, 37, 19));
         });
       }},
      {"stale", 3, [](Run& r) {
         auto s          = r.State<Cached>();
         s->probe->width = 53;
         s->Nudge();
         r.AfterRender({s->stage, s->probe->view, s->sibling}, [=](Run& r) {
           r.Size("cached", s->probe->view.GetMeasuredSize(), MeasuredSize(37, 19));
           r.Equal("unchanged-producer", s->probe->measures, 1);
         });
       }},
      {"invalidated", 7, [](Run& r) {
         auto s = r.State<Cached>();
         s->probe->view.InvalidateMeasure();
         r.AfterRender({s->stage, s->probe->view, s->sibling}, [=](Run& r) {
           r.Size("invalidated", s->probe->view.GetMeasuredSize(), MeasuredSize(53, 19));
           r.Equal("fresh-producer", s->probe->measures, 2);
           r.Rendered("invalidated.rendered", s->probe->view, LayoutRect(0, 0, 53, 19));
         });
       }},
      {"fresh-cache", 1, [](Run& r) {
         auto s = r.State<Cached>();
         s->Nudge();
         r.AfterRender({s->stage, s->probe->view, s->sibling}, [=](Run& r) { r.Equal("fresh-cache", s->probe->measures, 2); });
       }},
    }));
    out.push_back(Steps("K04.sibling-isolation", "One dirty child preserves the clean sibling cache", {
      {"mount", 4, [](Run& r) {
         auto s    = std::make_shared<Cached>();
         s->stack  = StackLayout::New(StackOrientation::HORIZONTAL);
         s->probe  = Probed(r, 20, 10, false);
         s->second = Probed(r, 30, 10, false);
         s->stack.Add(s->probe->view);
         s->stack.Add(s->second->view);
         s->stage = Mount(r, s->stack, 200, 100);
         r.SetState(s);
         r.AfterRender({s->stage, s->stack, s->probe->view, s->second->view}, [=](Run& r) {
           s->measures       = s->probe->measures;
           s->secondMeasures = s->second->measures;
           r.Rendered("root", s->stack, LayoutRect(0, 0, 50, 10));
         });
       }},
      {"dirty-a", 8, [](Run& r) {
         auto s          = r.State<Cached>();
         s->probe->width = 40;
         s->probe->view.InvalidateMeasure();
         r.AfterRender({s->stage, s->stack, s->probe->view, s->second->view}, [=](Run& r) {
           r.Size("root", s->stack.GetMeasuredSize(), MeasuredSize(70, 10));
           r.Equal("dirty-producer", s->probe->measures, s->measures + 1);
           r.Equal("clean-producer", s->second->measures, s->secondMeasures);
           r.Rendered("root.rendered", s->stack, LayoutRect(0, 0, 70, 10));
         });
       }},
    }));
    // The public mode-less Measure derives the modes the parent's layout passed in the pass, so an
    // application call with the pass's numbers is a cache hit for every first-party parent kind.
    for(unsigned kind = 0; kind < 6; ++kind)
      out.push_back(ApplicationMeasureCase(kind));
    // On its own layout root (no HUD pass), a MATCH_PARENT child is measured once, EXACTLY at the
    // 100x60 stage, and the arrange-time re-measure finds that measurement and is skipped.
    out.push_back(Steps("K12.arrange-remeasure-skip", "The arrange-time re-measure of a MATCH_PARENT child already measured at its slot runs no second producer", {
      {"mount", 6, [](Run& r) {
         auto s   = std::make_shared<Cached>();
         s->probe = Probed(r, 30, 10, false);
         Fixed(s->probe->view, MATCH_PARENT, MATCH_PARENT);
         s->stage = r.StageOnWindow(100, 60);
         s->stage.Add(s->probe->view);
         r.SetState(s);
         r.AfterRender({s->probe->view}, [=](Run& r) {
           r.Size("measured", s->probe->view.GetMeasuredSize(), MeasuredSize(100, 60));
           r.Rendered("rendered", s->probe->view, LayoutRect(0, 0, 100, 60));
         });
       }},
      {"invalidate", 3, [](Run& r) {
         // The pass measures the probe once; whether the arrange-time re-measure is skipped or served
         // from the cache is not observable, but a re-measure that runs the producer adds a second run.
         auto s      = r.State<Cached>();
         s->measures = s->probe->measures;
         s->probe->view.InvalidateMeasure();
         r.AfterRender({s->probe->view}, [=](Run& r) {
           r.Equal("measure-producer", s->probe->measures, s->measures + 1);
           r.Size("measured", s->probe->view.GetMeasuredSize(), MeasuredSize(100, 60));
         });
       }},
    }));
    // The ScrollView parent kind, after K12 so that the earlier scenarios keep their order: the
    // application call Measure(100, FLT_MAX) derives (EXACTLY, UNSPECIFIED) like the scroll layout.
    out.push_back(ApplicationMeasureCase(6));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc115)
