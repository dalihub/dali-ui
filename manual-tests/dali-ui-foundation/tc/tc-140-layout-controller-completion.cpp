/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include "layout-validation-transition-fixtures.h"
using namespace LayoutValidation;
using namespace LayoutValidation::TransitionFixtures;
class Tc140 : public Case
{
public:
  Tc140()
  : Case("TC140", "Controller completion snapshots and ordering", "Check View-before-Window completion ordering")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    return {
      {"CT03.view-before-window", "All target View snapshots precede the Window fence", {{"mount-observed-tree", 11, [](Run& r)
    {
      auto s = NewState(r);
      SetFixtureBounds(s->sibling, {150, 30, 60, 20});
      s->root.Add(s->child);
      s->root.Add(s->sibling);
      s->child.LayoutFinishedSignal().Connect(s.get(), &State::OnViewFinished);
      s->sibling.LayoutFinishedSignal().Connect(s.get(), &State::OnViewFinished);
      LayoutController::Get(s->window).LayoutFinishedSignal().Connect(s.get(), &State::OnWindowFinished);
      r.Attach(s->root);
      r.AfterLayout({s->child, s->sibling}, [s](Run& v)
      {
        v.Rect("completion.child.target", v.Snapshot(s->child), {40, 30, 80, 40});
        v.Rect("completion.sibling.target", v.Snapshot(s->sibling), {150, 30, 60, 20});
        v.Truth("completion.window.last", s->completionCount >= 3 && s->completionOrder[s->completionCount - 1] == 100);
        v.Truth("completion.children.present", s->completionCount >= 3 &&
                                                 ((s->completionOrder[0] == 2 && s->completionOrder[1] == 3) ||
                                                  (s->completionOrder[0] == 3 && s->completionOrder[1] == 2)));
        v.Equal("completion.window.once", s->windowCompletions, 1);
      });
    }},
                                                                                         {"manual-process-defers-emit", 6, [](Run& r)
    {
      auto       s      = r.State<State>();
      const auto before = s->windowCompletions;
      SetFixtureBounds(s->child, {60, 30, 80, 40});
      r.AfterLayout({s->child}, [s, before](Run& v)
      {
        v.Rect("manual.latest.target", v.Snapshot(s->child), {60, 30, 80, 40});
        v.Equal("manual.eventual.fence", s->windowCompletions, before + 1);
      });
      LayoutController::Get(s->window).ProcessLayouts();
      r.Equal("manual.no-synchronous-emit", s->windowCompletions, before);
    }},
                                                                                         {"noop-no-new-signal", 4, [](Run& r)
    {
      auto       s            = r.State<State>();
      const auto before       = s->completionCount;
      const auto windowBefore = s->windowCompletions;
      LayoutController::Get(s->window).ProcessLayouts();
      r.Equal("noop.immediate.signal-count", s->completionCount, before);
      r.Near("noop.geometry.x", LayoutValidation::Bounds(s->child).x, 60);
      // Completion emits are deferred to the post-process phase: a no-op pass must not queue
      // any View or Window completion, observed after the deferred phase had a chance to run.
      r.Delay(200, [s, before, windowBefore](Run& v) {
        v.Equal("noop.settled.signal-count", s->completionCount, before);
        v.Equal("noop.settled.window-count", s->windowCompletions, windowBefore);
      });
    }}}},
      {"CT03.latest-and-unsubscribe", "Multiple manual passes retain only the latest target; unsubscription suppresses a queued listener", {{"mount", 4, [](Run& r)
    { Mount(r, NewState(r)); }},
                                                                                                                                            {"two-passes-one-delivery", 6, [](Run& r)
    {
      auto s = r.State<State>();
      s->child.LayoutFinishedSignal().Connect(s.get(), &State::OnViewFinished);
      SetFixtureBounds(s->child, {70, 30, 80, 40});
      LayoutController::Get(s->window).ProcessLayouts();
      SetFixtureBounds(s->child, {90, 30, 80, 40});
      LayoutController::Get(s->window).ProcessLayouts();
      r.Equal("coalesced.no-sync-delivery", s->completionCount, 0);
      r.AfterLayout({s->child}, [s](Run& v)
      {
        v.Rect("coalesced.latest", v.Snapshot(s->child), {90, 30, 80, 40});
        v.Equal("coalesced.view.once", s->completionCount, 1);
      });
    }},
                                                                                                                                            {"unsubscribe-before-post", 2, [](Run& r)
    {
      auto       s      = r.State<State>();
      const auto before = s->completionCount;
      SetFixtureBounds(s->child, {100, 30, 80, 40});
      LayoutController::Get(s->window).ProcessLayouts();
      s->DisconnectAll();
      r.Delay(100, [s, before](Run& v)
      {
        v.Equal("unsubscribe.silent", s->completionCount, before);
        v.Near("unsubscribe.geometry", Current(s->child).x, 100, .01);
      });
    }}}}};
  }
};
REGISTER_MANUAL_TEST(Tc140)
