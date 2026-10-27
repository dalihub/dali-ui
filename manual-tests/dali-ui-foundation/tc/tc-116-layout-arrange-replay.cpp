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
struct Replay
{
  std::shared_ptr<Probe> a, b;
  View                   stage, sibling, root;
  uint32_t               arrangesA{0}, arrangesB{0};
  float                  nudge{10};
  void                   Nudge()
  {
    sibling.SetRequestedWidth(nudge += 1);
  }
};
struct ClampedHit
{
  std::shared_ptr<Probe>      c;
  std::shared_ptr<LayoutRect> received;
  View                        stage, sibling;
  uint32_t                    arranges{0}, measures{0};
  float                       nudge{10};
};
} // namespace

class Tc116 : public Case
{
public:
  Tc116()
  : Case("TC116", "Arrange replay", "Check arrange cache replay restores geometry")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    out.push_back(Steps("K08.actor-repair", "A cache-hit replay restores externally clobbered Actor geometry on screen", {
      {"mount", 7, [](Run& r) {
         auto s = std::make_shared<Replay>();
         s->a   = Probed(r);
         s->a->view.SetMargin(Insets(7, 0, 9, 0));
         s->sibling = Leaf(10, 10, "sibling");
         s->sibling.SetRequestedY(60);
         s->stage = r.Stage(100, 100);
         s->stage.Add(s->a->view);
         s->stage.Add(s->sibling);
         r.SetState(s);
         r.AfterRender({s->stage, s->a->view, s->sibling}, [=](Run& r) {
           s->arrangesA = s->a->arranges;
           Actor raw    = s->a->view;
           r.Rendered("initial", s->a->view, LayoutRect(7, 9, 37, 19));
           r.Near("pivot-x", raw.GetProperty<Vector3>(Actor::Property::PIVOT).x, 0.5);
           r.Truth("placement-ignores-pivot", !raw.GetProperty<bool>(Actor::Property::POSITION_USES_PIVOT));
           r.Near("origin-x", raw.GetProperty<Vector3>(Actor::Property::PARENT_ORIGIN).x, 0);
         });
       }},
      {"clobber-and-replay", 5, [](Run& r) {
         auto  s   = r.State<Replay>();
         Actor raw = s->a->view;
         raw.SetProperty(Actor::Property::POSITION, Vector3(90, 80, 0));
         raw.SetProperty(Actor::Property::SIZE, Vector3(1, 2, 0));
         s->Nudge();
         r.AfterRender({s->stage, s->a->view, s->sibling}, [=](Run& r) {
           r.Rendered("repaired", s->a->view, LayoutRect(7, 9, 37, 19));
           r.Equal("producer-once", s->a->arranges, s->arrangesA);
         });
       }},
    }));
    out.push_back(Steps("K09.subtree-replay", "A cached parent replays its descendants without producers", {
      {"mount", 8, [](Run& r) {
         auto s  = std::make_shared<Replay>();
         s->root = StackLayout::New(StackOrientation::HORIZONTAL);
         Fixed(s->root, 200, 60);
         s->a = Probed(r, 20, 10);
         s->b = Probed(r, 30, 15);
         s->root.Add(s->a->view);
         s->root.Add(s->b->view);
         s->sibling = Leaf(10, 10, "sibling");
         s->sibling.SetRequestedY(70);
         s->stage = r.Stage(200, 100);
         s->stage.Add(s->root);
         s->stage.Add(s->sibling);
         r.SetState(s);
         r.AfterRender({s->stage, s->root, s->a->view, s->b->view, s->sibling}, [=](Run& r) {
           s->arrangesA = s->a->arranges;
           s->arrangesB = s->b->arranges;
           r.Rendered("initial-a", s->a->view, LayoutRect(0, 0, 20, 10));
           r.Rendered("initial-b", s->b->view, LayoutRect(20, 0, 30, 15));
         });
       }},
      {"clobber-and-replay", 10, [](Run& r) {
         auto  s   = r.State<Replay>();
         Actor raw = s->b->view;
         raw.SetProperty(Actor::Property::POSITION, Vector3(99, 88, 0));
         s->Nudge();
         r.AfterRender({s->stage, s->root, s->a->view, s->b->view, s->sibling}, [=](Run& r) {
           r.Rendered("a", s->a->view, LayoutRect(0, 0, 20, 10));
           r.Rendered("b", s->b->view, LayoutRect(20, 0, 30, 15));
           r.Equal("a-producer", s->a->arranges, s->arrangesA);
           r.Equal("b-producer", s->b->arranges, s->arrangesB);
         });
       }},
    }));
    out.push_back(Steps("K10.returned-bounds", "A valid producer result is authoritative and stays so on a cache hit", {
      {"mount", 8, [](Run& r) {
         auto s         = std::make_shared<Replay>();
         s->a           = Probed(r);
         s->a->returned = [](const LayoutRect&) { return LayoutRect(11, 13, 53, 29); };
         s->sibling     = Leaf(10, 10, "sibling");
         s->sibling.SetRequestedY(60);
         s->stage = r.Stage(100, 100);
         s->stage.Add(s->a->view);
         s->stage.Add(s->sibling);
         r.SetState(s);
         r.AfterRender({s->stage, s->a->view, s->sibling}, [=](Run& r) {
           s->arrangesA = s->a->arranges;
           r.Rect("returned", r.Snapshot(s->a->view), LayoutRect(11, 13, 53, 29));
           r.Rendered("returned.rendered", s->a->view, LayoutRect(11, 13, 53, 29));
         });
       }},
      {"hit", 5, [](Run& r) {
         auto s = r.State<Replay>();
         s->Nudge();
         r.AfterRender({s->stage, s->a->view, s->sibling}, [=](Run& r) {
           r.Rendered("hit", s->a->view, LayoutRect(11, 13, 53, 29));
           r.Equal("producer", s->a->arranges, s->arrangesA);
         });
       }},
    }));
    // A MATCH_PARENT probe with maximum 120x80 in a 300x200 stage: its arrange producer receives the
    // slot clamped from its start edge, (0,0,120,80), while the arrange cache keys on the raw slot
    // (0,0,300,200), so an Arrange with that raw slot is a hit that re-applies the clamped rect.
    out.push_back(Steps("K21.clamped-arrange-hit", "The arrange producer receives the clamped slot and the cache keys on the raw slot", {
      {"mount", 10, [](Run& r) {
         auto s      = std::make_shared<ClampedHit>();
         s->c        = Probed(r, 60, 40);
         s->received = std::make_shared<LayoutRect>(-1, -1, -1, -1);
         auto rec    = s->received;
         Fixed(s->c->view, MATCH_PARENT, MATCH_PARENT);
         s->c->view.SetMaximumWidth(120);
         s->c->view.SetMaximumHeight(80);
         s->c->returned = [rec](const LayoutRect& b) {
           *rec = b;
           return b;
         };
         s->sibling = Leaf(10, 10, "sibling");
         s->sibling.SetRequestedY(190);
         s->stage = r.Stage(300, 200);
         s->stage.Add(s->c->view);
         s->stage.Add(s->sibling);
         r.SetState(s);
         r.AfterRender({s->stage, s->c->view, s->sibling}, [=](Run& r) {
           r.Rect("received", *s->received, LayoutRect(0, 0, 120, 80));
           r.Size("measured", s->c->view.GetMeasuredSize(), MeasuredSize(120, 80));
           r.Rendered("c", s->c->view, LayoutRect(0, 0, 120, 80));
         });
       }},
      {"external-hit", 10, [](Run& r) {
         auto s      = r.State<ClampedHit>();
         s->arranges = s->c->arranges;
         s->measures = s->c->measures;
         Actor raw   = s->c->view;
         raw.SetProperty(Actor::Property::SIZE, Vector3(290, 200, 0));
         const LayoutRect hit = s->c->view.Arrange(LayoutRect(0, 0, 300, 200));
         r.Rect("hit", hit, LayoutRect(0, 0, 120, 80));
         r.Rect("repaired", s->c->view, LayoutRect(0, 0, 120, 80));
         r.Equal("no-arrange-producer", s->c->arranges, s->arranges);
         r.Equal("no-measure-producer", s->c->measures, s->measures);
       }},
      {"pass", 5, [](Run& r) {
         auto s = r.State<ClampedHit>();
         s->sibling.SetRequestedWidth(s->nudge += 1);
         r.AfterRender({s->stage, s->c->view, s->sibling}, [=](Run& r) {
           r.Rendered("c", s->c->view, LayoutRect(0, 0, 120, 80));
           r.Equal("pass.no-arrange-producer", s->c->arranges, s->arranges);
         });
       }},
    }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc116)
