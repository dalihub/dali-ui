/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include "layout-validation-transition-fixtures.h"
using namespace LayoutValidation;
using namespace LayoutValidation::TransitionFixtures;
class Tc142 : public Case
{
public:
  Tc142()
  : Case("TC142", "Controller rollback and reentry", "Check controller recovers from throw/re-entry")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> cases;
    for(bool arrange : {false, true})
      cases.push_back({arrange ? "CT05.arrange-throws" : "CT05.measure-throws", "A producer exception leaves no processing guard behind: the next pass runs both producers again", {{"mount", 4, [](Run& r)
      { Mount(r, NewState(r)); }},
                                                                                                                                                  {"throw-and-recover", 4, [arrange](Run& r)
      {
        auto s = r.State<State>();
        // This is an intentionally synthetic producer View, with no built-in manager to bypass.
        s->child.SetMeasureCallback(MeasureCallback::New(s.get(), &State::Measure));
        s->child.SetArrangeCallback(ArrangeCallback::New(s.get(), &State::Arrange));
        s->throwMeasure = !arrange;
        s->throwArrange = arrange;
        s->child.InvalidateMeasure();
        bool        caught = false;
        std::string message;
        try
        {
          LayoutController::Get(s->window).ProcessLayouts();
        }
        catch(const std::runtime_error& error)
        {
          caught  = true;
          message = error.what();
        }
        r.Truth("rollback.exception.caught", caught);
        r.Text("rollback.exception.message", message, arrange ? "LR injected Arrange failure" : "LR injected Measure failure");
        s->throwMeasure = s->throwArrange = false;
        s->child.InvalidateMeasure();
        const uint32_t measuresBefore = s->measureCalls;
        const uint32_t arrangesBefore = s->arrangeCalls;
        LayoutController::Get(s->window).ProcessLayouts();
        // A processing or in-progress guard leaked by the exception would make the recovery pass
        // return before the producers; both must run again.
        r.Truth("rollback.recovered.measure-producer", s->measureCalls > measuresBefore);
        r.Truth("rollback.recovered.arrange-producer", s->arrangeCalls > arrangesBefore);
      }},
                                                                                                                                                  {"recovered-fence", 4, [](Run& r)
      {
        auto s = r.State<State>();
        SetFixtureBounds(s->child, {65, 30, 80, 40});
        r.AfterLayout({s->child}, [s](Run& v)
        { v.Rect("rollback.recovery.target", v.Snapshot(s->child), {65, 30, 80, 40}); });
      }}}});
    cases.push_back({"CT05.nested-process", "Reentrant ProcessLayouts inside a producer runs once and keeps the outer pass result", {{"mount", 4, [](Run& r)
    { Mount(r, NewState(r)); }},
                                                                                                                          {"nested-call", 6, [](Run& r)
    {
      auto s     = r.State<State>();
      s->reenter = true;
      s->child.SetMeasureCallback(MeasureCallback::New(s.get(), &State::Measure));
      s->child.InvalidateMeasure();
      LayoutController::Get(s->window).ProcessLayouts();
      r.Truth("nested.callback.executed", !s->reenter);
      r.Equal("nested.callback.once", s->measureCalls, 1);
      r.Rect("nested.final.bounds", ArrangedBounds(s->child), {40, 30, 80, 40});
    }}}});
    cases.push_back({"CT05.remaining-root-rollback", "An exception in an earlier root's turn leaves every root of the batch recoverable by the next pass", {{"mount-boundary-roots", 8, [](Run& r)
    {
      auto s = NewState(r);
      s->other.SetLayoutMode(LayoutMode::STANDALONE);
      s->other.SetRequestedX(5);
      s->other.SetRequestedY(7);
      s->child.Add(s->other);
      s->root.Add(s->child);
      r.Attach(s->root);
      r.AfterLayout({s->child, s->other}, [s](Run& v)
      {
        v.Rect("remaining.child", v.Snapshot(s->child), {40, 30, 80, 40});
        v.Rect("remaining.boundary", v.Snapshot(s->other), {5, 7, 160, 120});
      });
    }},
                                                                                                                      {"abort-earlier-root", 2, [](Run& r)
    {
      auto s = r.State<State>();
      s->child.SetMeasureCallback(MeasureCallback::New(s.get(), &State::Measure));
      s->throwMeasure  = true;
      auto& controller = LayoutController::Get(s->window);
      controller.RequestLayout(&GetImpl(s->other));
      controller.RequestLayout(&GetImpl(s->root));
      bool        caught = false;
      std::string message;
      try
      {
        controller.ProcessLayouts();
      }
      catch(const std::runtime_error& e)
      {
        caught  = true;
        message = e.what();
      }
      // The injected failure belongs only to the manual call above.
      s->throwMeasure = false;
      r.Truth("remaining.exception", caught);
      r.Text("remaining.exception.message", message, "LR injected Measure failure");
    }},
                                                                                                                      {"drain-restored-batch", 8, [](Run& r)
    {
      auto s          = r.State<State>();
      s->throwMeasure = false;
      s->child.InvalidateMeasure();
      s->other.InvalidateMeasure();
      r.AfterLayout({s->child, s->other}, [s](Run& v)
      {
        v.Rect("remaining.recovered.child", v.Snapshot(s->child), {40, 30, 80, 40});
        v.Rect("remaining.recovered.boundary", v.Snapshot(s->other), {5, 7, 160, 120});
      });
    }}}});
    cases.push_back({"CT05.lifecycle-throws", "Lifecycle exceptions restore controller processing guards", {{"mount", 4, [](Run& r)
    { Mount(r, NewState(r)); }},
                                                                                                            {"throw-lifecycle", 3, [](Run& r)
    {
      auto s = r.State<State>();
      s->Bind(LayoutTransition::New());
      s->root.SetLayoutTransition(s->transition);
      s->throwOnStart = true;
      SetFixtureBounds(s->child, {80, 30, 80, 40});
      bool        caught = false;
      std::string message;
      try
      {
        LayoutController::Get(s->window).ProcessLayouts();
      }
      catch(const std::runtime_error& e)
      {
        caught  = true;
        message = e.what();
      }
      r.Truth("lifecycle.exception", caught);
      r.Text("lifecycle.exception.message", message, "LR injected lifecycle failure");
      r.Equal("lifecycle.failed-start.once", s->starts[2], 1);
      s->throwOnStart = false;
      s->transition.ClearChangeTiming();
      SetFixtureBounds(s->child, {90, 30, 80, 40});
      LayoutController::Get(s->window).ProcessLayouts();
    }},
                                                                                                            {"recover-lifecycle", 5, [](Run& r)
    {
      auto s = r.State<State>();
      SetFixtureBounds(s->child, {100, 30, 80, 40});
      r.AfterLayout({s->child}, [s](Run& v)
      {
        v.Rect("lifecycle.recovered", v.Snapshot(s->child), {100, 30, 80, 40});
        v.Equal("lifecycle.cancel.silent", s->finishes[2], 0);
      });
    }}}});
    cases.push_back({"CT05.resize-flag-abort", "Aborted resize processing cannot leak WINDOW_RESIZED into the next ordinary change", {{"mount", 4, [](Run& r)
    { Mount(r, NewState(r)); }},
                                                                                                                                      {"abort-resize-then-change", 13, [](Run& r)
    {
      auto       s      = r.State<State>();
      const auto window = s->window;
      const auto size   = window.GetPositionSize();
      r.OnCleanup([window, size]()
      { LayoutController::Get(window).OnWindowResize(size.width, size.height); });
      CaptureLifecycle(s);
      s->transition.SetChangeOnWindowResize(true);
      s->child.SetMeasureCallback(MeasureCallback::New(s.get(), &State::Measure));
      s->throwMeasure  = true;
      auto& controller = LayoutController::Get(s->window);
      controller.OnWindowResize(640, 480);
      bool caught = false;
      try
      {
        controller.ProcessLayouts();
      }
      catch(const std::runtime_error&)
      {
        caught = true;
      }
      r.Truth("resize-abort.exception", caught);
      s->throwMeasure = false;
      s->child.InvalidateMeasure();
      SetFixtureBounds(s->child, {90, 30, 80, 40});
      // The pass ends with the animator tick that dispatches the fresh CHANGE once at raw 0.
      controller.ProcessLayouts();
      Observation found;
      bool        seen = false;
      for(std::size_t i = 0; i < s->observationCount; ++i)
        if(s->observations[i].viewId == 2)
        {
          found = s->observations[i];
          seen  = true;
          break;
        }
      r.Truth("resize-abort.context", seen);
      CheckObservation(r, found, LayoutTransitionSlot::CHANGE, LayoutChangeCause::OTHER, {40, 30, 80, 40}, {90, 30, 80, 40});
      r.Equal("resize-abort.no-finish", s->finishes[2], 0);
    }}}});
    return cases;
  }
};
REGISTER_MANUAL_TEST(Tc142)
