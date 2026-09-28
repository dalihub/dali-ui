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
class Tc133 : public Case
{
public:
  Tc133()
  : Case("TC133", "Transition composition and clipping", "Check composite effects and clipping restore")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> cases;
    for(unsigned clip = 0; clip < 3; ++clip)
      for(bool size : {false, true})
      {
        cases.push_back({"TR06.clip-" + std::to_string(clip) + (size ? "-size" : "-offset"), "Composite max duration and transient clipping restoration", {{"mount", 4, [](Run& r)
        { Mount(r, NewState(r)); }},
                                                                                                                                                           {"start-composite", 3, [clip, size](Run& r)
        {
          auto s = r.State<State>();
          s->root.Remove(s->child, RemovePolicy::IMMEDIATE);
          s->child.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::DISABLED);
          s->child.SetProperty(Actor::Property::OPACITY, 0.f);
          s->Bind(LayoutTransition::New());
          auto effect = LayoutBoundsEffect().SetTiming(Timing(.2f, .1f)).SetClipMode(static_cast<LayoutBoundsClipMode>(clip));
          if(size)
            effect.SetSizeFactor(.5f, .5f);
          else
            effect.SetOffset(LayoutBoundsLength::Pixel(20), LayoutBoundsLength::Pixel(0));
          auto visual = ViewAnimationSpec::New();
          visual.Opacity(1, Duration(.5f), AlphaFunction(AlphaFunction::LINEAR));
          s->transition.SetEnterVisualSpec(visual).SetEnterBoundsEffect(effect).ClearChangeTiming();
          s->root.SetLayoutTransition(s->transition);
          s->root.Add(s->child);
          LayoutController::Get(s->window).ProcessLayouts();
          r.Equal("composite.start.once", s->starts[0], 1);
          const bool clipping = clip == 2 || (clip == 0 && size);
          r.Equal("composite.transient.clip", s->child.GetProperty<int>(Actor::Property::CLIPPING_MODE),
                  clipping ? ClippingMode::CLIP_TO_BOUNDING_BOX : ClippingMode::DISABLED);
          r.Equal("composite.finish.pending", s->finishes[0], 0);
        }},
                                                                                                                                                           {"natural-finish", 8, [](Run& r)
        {
          // Waits for the natural ENTER finish (an event); the frame deadline is only a liveness
          // bound. The opacity track (0.5 s) is longer than the delayed bounds track (0.1 + 0.2 s),
          // so an opacity of 1 at the finish shows the composite lasted for the longer track.
          auto s = r.State<State>();
          r.AfterFrame([s](Run& v)
          {
            v.Rect("composite.final.bounds", Current(s->child), {40, 30, 80, 40}, .01);
            v.Near("composite.final.opacity", s->child.GetCurrentProperty<float>(Actor::Property::OPACITY), 1, .001);
            v.Equal("composite.finish.once", s->finishes[0], 1);
            v.Equal("composite.clip.restored", s->child.GetProperty<int>(Actor::Property::CLIPPING_MODE), ClippingMode::DISABLED);
            v.Equal("composite.start.still.once", s->starts[0], 1);
          }, [s]
          { return s->finishes[0] >= 1; }, s->root);
        }}}});
      }
    cases.push_back({"TR06.noop-and-instant", "Identity and duration-zero effects do not allocate a timed transition", {{"mount", 4, [](Run& r)
    { Mount(r, NewState(r)); }},
                                                                                                                        {"instant-enter", 8, [](Run& r)
    {
      auto s = r.State<State>();
      s->root.Remove(s->child, RemovePolicy::IMMEDIATE);
      s->Bind(LayoutTransition::New());
      s->transition.SetEnterBoundsEffect(LayoutBoundsEffect().SetTiming(Timing(0, 10)).SetSizeFactor(0, 0));
      s->root.SetLayoutTransition(s->transition);
      s->root.Add(s->child);
      LayoutController::Get(s->window).ProcessLayouts();
      r.Rect("instant.enter.endpoint", LayoutValidation::Bounds(s->child), {40, 30, 80, 40});
      r.Equal("instant.enter.start", s->starts[0], 0);
      r.Equal("instant.enter.finish", s->finishes[0], 0);
      s->transition.SetExitVisualSpec(ViewAnimationSpec::New()).SetExitBoundsEffect(LayoutBoundsEffect());
      s->root.Remove(s->child, RemovePolicy::ANIMATE_EXIT);
      r.Truth("empty.exit.immediate", !s->child.GetParent());
      r.Equal("empty.exit.start", s->starts[1], 0);
    }}}});
    return cases;
  }
};
REGISTER_MANUAL_TEST(Tc133)
