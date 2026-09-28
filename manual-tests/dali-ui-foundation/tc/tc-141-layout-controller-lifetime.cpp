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
class Tc141 : public Case
{
public:
  Tc141()
  : Case("TC141", "Controller lifetime and window isolation", "Check controller lifetime and Window isolation")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> cases;
    cases.push_back({"CT04.remove-in-completion", "Controller removal inside its completion slot detaches safely and permits a fresh controller", {{"mount-extra-window", 4, [](Run& r)
    {
      auto s         = NewState(r);
      s->extraWindow = Window::New(PositionSize(50, 50, 320, 240), "TC141 lifetime");
      s->window      = s->extraWindow;
      s->extraWindow.Add(s->root);
      r.AfterLayout(s->window, {s->root}, [s](Run& v)
      { v.Rect("lifetime.first", v.Snapshot(s->root), {0, 0, 300, 200}); });
    }},
                                                                                                                                     {"remove-during-fence", 2, [](Run& r)
    {
      auto s                          = r.State<State>();
      s->windowCompletions            = 0;
      s->removeControllerOnCompletion = true;
      LayoutController::Get(s->window).LayoutFinishedSignal().Connect(s.get(), &State::OnWindowFinished);
      s->root.InvalidateMeasure();
      // Waits for the completion whose listener removes the controller (an event); the frame
      // deadline is only a liveness bound.
      r.AfterFrame([s](Run& v)
      {
        v.Equal("lifetime.old-fence.once", s->windowCompletions, 1);
        v.Truth("lifetime.window.alive", static_cast<bool>(s->window));
      }, [s]
      { return s->windowCompletions >= 1; });
    }},
                                                                                                                                     {"recreate-and-process", 5, [](Run& r)
    {
      auto       s          = r.State<State>();
      const auto oldCount   = s->windowCompletions;
      auto&      controller = LayoutController::Get(s->window);
      s->root.SetRequestedWidth(280);
      controller.RequestLayout(&GetImpl(s->root));
      r.AfterLayout(s->window, {s->root}, [s, oldCount](Run& v)
      {
        v.Rect("lifetime.new.target", v.Snapshot(s->root), {0, 0, 280, 200});
        v.Equal("lifetime.old.listener.inert", s->windowCompletions, oldCount);
      });
    }}}});
    cases.push_back({"CT04.window-isolation", "Two Window controllers maintain separate roots and completion state", {{"mount-main", 4, [](Run& r)
    { Mount(r, NewState(r)); }},
                                                                                                        {"mount-other", 5, [](Run& r)
    {
      auto s         = r.State<State>();
      s->extraWindow = Window::New(PositionSize(70, 70, 200, 160), "TC141 isolation");
      s->extraWindow.Add(s->other);
      r.AfterLayout(s->extraWindow, {s->other}, [s](Run& v)
      {
        v.Rect("second.window.root", v.Snapshot(s->other), {0, 0, 160, 120});
        v.Truth("windows.distinct", s->extraWindow != s->window);
      });
    }},
                                                                                                        {"close-other-then-main-change", 4, [](Run& r)
    {
      auto s = r.State<State>();
      LayoutController::Remove(s->extraWindow);
      LayoutController::Remove(s->extraWindow);
      s->other.Unparent();
      s->extraWindow.Hide();
      s->extraWindow.Reset();
      SetFixtureBounds(s->child, {55, 30, 80, 40});
      r.AfterLayout({s->child}, [s](Run& v)
      { v.Rect("surviving.window.target", v.Snapshot(s->child), {55, 30, 80, 40}); });
    }}}});
    return cases;
  }
};
REGISTER_MANUAL_TEST(Tc141)
