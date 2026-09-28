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

class Tc129 : public Case
{
public:
  Tc129()
  : Case("TC129", "Transition initial mount", "Check ENTER transition on initial mount")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> cases;
    for(int mode = 0; mode < 4; ++mode)
    {
      Scenario scenario{"TR02.mount-" + std::to_string(mode), "Spec/Animator initial mount suppressed or enabled", {}};
      scenario.steps.push_back({"mount", 4, [mode](Run& r)
      {
        auto s = NewState(r);
        s->Bind(LayoutTransition::New());
        s->child.SetProperty(Actor::Property::OPACITY, .0f);
        s->transition.SetEnterOnInitialMount((mode & 1) != 0);
        if(mode < 2)
        {
          auto spec = ViewAnimationSpec::New();
          spec.Opacity(1, Duration(.2f), AlphaFunction(AlphaFunction::LINEAR));
          s->transition.SetEnterVisualSpec(spec);
        }
        else
          s->transition.SetEnterAnimator(LayoutAnimatorCallback::New(s.get(), &State::OnAnimator), AnimatorTiming(.2f));
        s->root.SetLayoutTransition(s->transition);
        Mount(r, s);
      }});
      scenario.steps.push_back({"settled", 7, [mode](Run& r)
      {
        auto s = r.State<State>();
        r.Delay(700, [s, mode](Run& v)
        {
          v.Rect("initial.rendered", Current(s->child), {40, 30, 80, 40}, .01);
          v.Equal("initial.started", s->starts[0], (mode & 1) ? 1 : 0);
          v.Equal("initial.finished", s->finishes[0], (mode & 1) ? 1 : 0);
          v.Near("initial.opacity", s->child.GetCurrentProperty<float>(Actor::Property::OPACITY), mode < 2 ? 1 : 0, .001);
        });
      }});
      scenario.steps.push_back({"runtime-add", 7, [mode](Run& r)
      {
        auto s = r.State<State>();
        s->sibling.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds({140, 30, 60, 20}));
        s->root.Add(s->sibling);
        r.Delay(700, [s, mode](Run& v)
        {
          v.Rect("runtime.rendered", Current(s->sibling), {140, 30, 60, 20}, .01);
          v.Equal("runtime.started", s->starts[0], (mode & 1) ? 2 : 1);
          v.Equal("runtime.finished", s->finishes[0], (mode & 1) ? 2 : 1);
          v.Truth("runtime.capture.overflow", !s->observationOverflow);
        });
      }});
      cases.push_back(std::move(scenario));
    }
    cases.push_back({"TR02.attach-after-add", "Transition attachment after an already settled child does not invent ENTER", {{"mount", 4, [](Run& r)
    { Mount(r, NewState(r)); }},
                                                                                                                             {"attach-and-invalidate", 6, [](Run& r)
    {
      auto s = r.State<State>();
      CaptureLifecycle(s, .15f);
      s->root.InvalidateArrange();
      r.Delay(500, [s](Run& v)
      {
        v.Equal("late.attach.enter", s->starts[0], 0);
        v.Equal("late.attach.finish", s->finishes[0], 0);
        v.Rect("late.attach.geometry", Current(s->child), {40, 30, 80, 40}, .01);
      });
    }}}});
    return cases;
  }
};
REGISTER_MANUAL_TEST(Tc129)
