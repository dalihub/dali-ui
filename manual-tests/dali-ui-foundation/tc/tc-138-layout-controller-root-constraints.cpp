/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <vector>
#include "layout-validation-transition-fixtures.h"
using namespace LayoutValidation;
using namespace LayoutValidation::TransitionFixtures;
class Tc138 : public Case
{
public:
  Tc138()
  : Case("TC138", "Controller root constraints", "Check root view constraints and Window resize")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> cases;
    for(unsigned mode = 0; mode < 4; ++mode)
      cases.push_back(Single(("CT01.constraint-" + std::to_string(mode)).c_str(), "Fixed, parent, no-parent and zero-parent constraints with margins", 8, [mode](Run& r)
      {
        auto s         = NewState(r);
        s->extraWindow = Window::New(PositionSize(30, 30, 320, 240), "TC138 root constraints");
        s->window      = s->extraWindow;
        s->root        = View::New();
        s->root.SetBackgroundColor(FixtureColor(3));
        s->root.SetMeasureCallback(MeasureCallback::New(s.get(), &State::Measure));
        s->root.SetRequestedWidth(mode == 0 ? 300 : MATCH_PARENT);
        s->root.SetRequestedHeight(mode == 0 ? 200 : MATCH_PARENT);
        s->root.SetRequestedX(11);
        s->root.SetRequestedY(13);
        s->root.SetMargin(Insets(3, 7, 5, 11));
        if(mode == 1 || mode == 3)
        {
          // An ordinary Actor establishes an explicitly sized visual parent.
          Actor parent = Actor::New();
          parent.SetSize(Vector3(mode == 3 ? 0.f : 250.f, mode == 3 ? 0.f : 180.f, 0));
          parent.Add(s->root);
          s->extraWindow.Add(parent);
          r.OnCleanup([parent]() mutable
          { parent.Unparent(); });
        }
        else if(mode == 0)
          s->extraWindow.Add(s->root);
        auto& controller = LayoutController::Get(s->window);
        controller.OnWindowResize(320, 240);
        controller.RequestLayout(&GetImpl(s->root));
        controller.ProcessLayouts();
        // Mode 2 has no parent: the constraint is the realized window size minus the margins
        // (3+7 and 5+11), read back from the window rather than assumed to be 320x240.
        const auto  realized = s->extraWindow.GetPositionSize();
        const float width    = mode == 0 ? 290.f : mode == 1 ? 240.f : mode == 2 ? static_cast<float>(realized.width) - 10.f : 0.f;
        const float height   = mode == 0 ? 184.f : mode == 1 ? 164.f : mode == 2 ? static_cast<float>(realized.height) - 16.f : 0.f;
        r.Size("root.producer.constraint", s->measuredConstraint, {width, height});
        r.Rect("root.bounds", LayoutValidation::Bounds(s->root), {14, 18, mode == 0 ? 80.f : width, mode == 0 ? 40.f : height});
        r.Truth("root.producer.called", s->measureCalls > 0);
        r.Truth("root.parent.branch", mode == 2 ? !s->root.GetParent() : static_cast<bool>(s->root.GetParent()));
      }));
    cases.push_back({"CT01.real-window-resize", "A real Window size change reaches MATCH_PARENT roots", {{"create", 4, [](Run& r)
    {
      auto s         = NewState(r);
      s->extraWindow = Window::New(PositionSize(40, 40, 320, 240), "TC138 resize");
      s->window      = s->extraWindow;
      s->root.SetRequestedWidth(MATCH_PARENT);
      s->root.SetRequestedHeight(MATCH_PARENT);
      s->extraWindow.Add(s->root);
      LayoutController::Get(s->window).ProcessLayouts();
      r.Rect("window.initial.root", LayoutValidation::Bounds(s->root), {0, 0, 320, 240});
    }},
                                                                                                         {"resize", 4, [](Run& r)
    {
      auto s = r.State<State>();
      r.AfterLayout(s->window, {s->root}, [s](Run& v)
      { v.Rect("window.resized.root", v.Snapshot(s->root), {0, 0, 400, 300}); });
      s->extraWindow.SetPositionSize(PositionSize(40, 40, 400, 300));
    }}}});
    // A MATCH_PARENT root with maximum 200x150 in a window of realized size Rw x Rh: each MATCH_PARENT
    // axis is measured EXACTLY at (window - margin) clamped by the maximum, (min(Rw-10,200),
    // min(Rh-16,150)) = (200,150), and placed at (11+3, 13+5). An application Measure with the pass's
    // pre-clamp numbers derives the same modes (no View parent: MATCH_PARENT axes EXACTLY) and the same
    // clamped key, so it is a cache hit that runs no producer; a smaller constraint is a new exact size.
    cases.push_back({"CT01.root-maximum", "A MATCH_PARENT root is measured exactly at its clamped slot; an application Measure with the pass's numbers is a cache hit", {{"pass", 10, [](Run& r)
    {
      auto s         = NewState(r);
      s->extraWindow = Window::New(PositionSize(30, 30, 320, 240), "TC138 root maximum");
      s->window      = s->extraWindow;
      s->root        = View::New();
      s->root.SetBackgroundColor(FixtureColor(3));
      s->root.SetMeasureCallback(MeasureCallback::New(s.get(), &State::Measure));
      s->root.SetRequestedWidth(MATCH_PARENT);
      s->root.SetRequestedHeight(MATCH_PARENT);
      s->root.SetMaximumWidth(200);
      s->root.SetMaximumHeight(150);
      s->root.SetRequestedX(11);
      s->root.SetRequestedY(13);
      s->root.SetMargin(Insets(3, 7, 5, 11));
      s->extraWindow.Add(s->root);
      auto& controller = LayoutController::Get(s->window);
      controller.OnWindowResize(320, 240);
      controller.RequestLayout(&GetImpl(s->root));
      controller.ProcessLayouts();
      const auto realized = s->extraWindow.GetPositionSize();
      r.Truth("precondition.window-above-maximum", realized.width - 10 >= 200 && realized.height - 16 >= 150);
      r.Size("root.producer.constraint", s->measuredConstraint, {std::min(static_cast<float>(realized.width) - 10.f, 200.f), std::min(static_cast<float>(realized.height) - 16.f, 150.f)});
      r.Size("root.measured", s->root.GetMeasuredSize(), MeasuredSize(200, 150));
      r.Rect("root.bounds", LayoutValidation::Bounds(s->root), {14, 18, 200, 150});
      r.Truth("root.producer.called", s->measureCalls > 0);
    }},
                                                                                                                                                                       {"application-measure", 3, [](Run& r)
    {
      auto               s        = r.State<State>();
      const auto         realized = s->extraWindow.GetPositionSize();
      const uint32_t     before   = s->measureCalls;
      const MeasuredSize result   = s->root.Measure(static_cast<float>(realized.width) - 10.f, static_cast<float>(realized.height) - 16.f);
      r.Size("app.measure.result", result, MeasuredSize(200, 150));
      r.Equal("app.measure.producer-runs", s->measureCalls - before, 0);
    }},
                                                                                                                                                                       {"below-maximum", 5, [](Run& r)
    {
      auto           s      = r.State<State>();
      const uint32_t before = s->measureCalls;
      r.Size("below.result", s->root.Measure(150, 100), MeasuredSize(150, 100));
      r.Equal("below.producer-runs", s->measureCalls - before, 1);
      r.Size("below.producer.constraint", s->measuredConstraint, MeasuredSize(150, 100));
    }}}});
    // A WRAP_CONTENT standalone view (margin (3,7,5,11), requested position (11,13)) under the Absolute
    // root in the main window: measured against the window less its margin, (W-10, H-16), placed at
    // (14,18) at the 80x40 its producer returns. Reconnecting the tree re-measures it exactly once, and
    // a same-size window resize reaches it as a tracked root: its producer runs once more at the same
    // window bound. Dirty flags and the controller's root registry are internal and are not observed.
    cases.push_back({"CT01.standalone-window-root", "A WRAP_CONTENT standalone view is measured against its window, re-measured after reconnection and reached by a window resize", {{"mount", 8, [](Run& r)
    {
      auto s    = NewState(r);
      s->nested = View::New();
      s->nested.SetBackgroundColor(FixtureColor(3));
      s->nested.SetMeasureCallback(MeasureCallback::New(s.get(), &State::Measure));
      s->nested.SetLayoutMode(LayoutMode::STANDALONE);
      s->nested.SetRequestedX(11);
      s->nested.SetRequestedY(13);
      s->nested.SetMargin(Insets(3, 7, 5, 11));
      s->root.Add(s->nested);
      r.Attach(s->root);
      r.AfterLayout({s->nested}, [s](Run& v)
      {
        const auto window = v.GetWindow().GetPositionSize();
        v.Size("standalone.window-bound", s->measuredConstraint, {static_cast<float>(window.width) - 10.f, static_cast<float>(window.height) - 16.f});
        v.Size("standalone.measured", s->nested.GetMeasuredSize(), MeasuredSize(80, 40));
        v.Rect("standalone.target", v.Snapshot(s->nested), {14, 18, 80, 40});
      });
    }},
                                                                                                                                                                                                {"reconnect", 1, [](Run& r)
    {
      auto           s      = r.State<State>();
      const uint32_t before = s->measureCalls;
      s->root.Unparent();
      r.Host().Add(s->root);
      LayoutController::Get(s->window).ProcessLayouts();
      r.Equal("reconnect.producer-runs", s->measureCalls - before, 1);
    }},
                                                                                                                                                                                                {"window-resize", 7, [](Run& r)
    {
      auto           s      = r.State<State>();
      const uint32_t before = s->measureCalls;
      r.AfterLayout({s->nested}, [s, before](Run& v)
      {
        const auto window = v.GetWindow().GetPositionSize();
        v.Equal("resize.producer-runs", s->measureCalls - before, 1);
        v.Rect("resize.target", v.Snapshot(s->nested), {14, 18, 80, 40});
        v.Size("resize.window-bound", s->measuredConstraint, {static_cast<float>(window.width) - 10.f, static_cast<float>(window.height) - 16.f});
      });
      const auto size = s->window.GetPositionSize();
      LayoutController::Get(s->window).OnWindowResize(size.width, size.height);
    }}}});
    return cases;
  }
};
REGISTER_MANUAL_TEST(Tc138)
