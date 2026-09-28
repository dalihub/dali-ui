/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include <dali/public-api/animation/spring-data.h>
#include "layout-validation-transition-fixtures.h"
using namespace LayoutValidation;
using namespace LayoutValidation::TransitionFixtures;
namespace
{
float CustomCurve(float t)
{
  return 2.0f - .5f * t;
}
// Closed forms of the builtin curves. The quarter-progress tables of the earlier revision are
// their values at t = 0, .25, .5, .75 and 1.
float Linear(float t)
{
  return t;
}
float SquareIn(float t)
{
  return t * t;
}
float SquareOut(float t)
{
  return 1.0f - (1.0f - t) * (1.0f - t);
}
float CubicIn(float t)
{
  return t * t * t;
}
float CubicOut(float t)
{
  return 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);
}
float CubicInOut(float t)
{
  return t < .5f ? 4.0f * t * t * t : 1.0f - 4.0f * (1.0f - t) * (1.0f - t) * (1.0f - t);
}
float SineIn(float t)
{
  return 1.0f - std::cos(float(M_PI) * t / 2);
}
float SineOut(float t)
{
  return std::sin(float(M_PI) * t / 2);
}
float SineInOut(float t)
{
  return .5f * (1.0f - std::cos(float(M_PI) * t));
}
bool SameRect(const LayoutRect& a, const LayoutRect& b)
{
  return std::abs(a.x - b.x) <= GEOMETRY_TOLERANCE && std::abs(a.y - b.y) <= GEOMETRY_TOLERANCE &&
         std::abs(a.width - b.width) <= GEOMETRY_TOLERANCE && std::abs(a.height - b.height) <= GEOMETRY_TOLERANCE;
}
struct Curve
{
  const char*   name;
  AlphaFunction alpha;
  float (*expected)(float);
};
} //namespace
class Tc134 : public Case
{
public:
  Tc134()
  : Case("TC134", "Transition animator clock and easing", "Check animator progress, easing and delay")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> cases;
    const std::vector<Curve> curves{
      {"default", AlphaFunction(AlphaFunction::DEFAULT), &Linear},
      {"linear", AlphaFunction(AlphaFunction::LINEAR), &Linear},
      {"square-in", AlphaFunction(AlphaFunction::EASE_IN_SQUARE), &SquareIn},
      {"square-out", AlphaFunction(AlphaFunction::EASE_OUT_SQUARE), &SquareOut},
      {"cubic-in", AlphaFunction(AlphaFunction::EASE_IN), &CubicIn},
      {"cubic-out", AlphaFunction(AlphaFunction::EASE_OUT), &CubicOut},
      {"cubic-inout", AlphaFunction(AlphaFunction::EASE_IN_OUT), &CubicInOut},
      {"sine-in", AlphaFunction(AlphaFunction::EASE_IN_SINE), &SineIn},
      {"sine-out", AlphaFunction(AlphaFunction::EASE_OUT_SINE), &SineOut},
      {"sine-inout", AlphaFunction(AlphaFunction::EASE_IN_OUT_SINE), &SineInOut},
      {"bounce-fallback", AlphaFunction(AlphaFunction::BOUNCE), &Linear},
      {"sin-fallback", AlphaFunction(AlphaFunction::SIN), &Linear},
      {"back-fallback", AlphaFunction(AlphaFunction::EASE_OUT_BACK), &Linear},
      {"bezier-fallback", AlphaFunction(Vector2(.1f, .9f), Vector2(.8f, .2f)), &Linear},
      {"spring-fallback", AlphaFunction(AlphaFunction::GENTLE), &Linear},
      {"custom-spring-fallback", AlphaFunction(SpringData(100.f, 10.f, 1.f)), &Linear},
      {"custom-nonterminal", AlphaFunction(&CustomCurve), &CustomCurve}};
    for(const auto& curve : curves)
    {
      cases.push_back({"TR07.curve-" + std::string(curve.name), "Actual animator ticks: raw 0 first context, every real-time sample on the curve, natural finish at raw 1", {{"mount", 4, [](Run& r)
      { Mount(r, NewState(r)); }},
                                                                                                                                  {"first-tick", 13, [curve](Run& r)
      {
        auto s = r.State<State>();
        s->Bind(LayoutTransition::New());
        auto timing = AnimatorTiming();
        timing.SetAlpha(curve.alpha);
        s->transition.SetChangeAnimator(LayoutAnimatorCallback::New(s.get(), &State::OnAnimator), timing);
        s->root.SetLayoutTransition(s->transition);
        SetFixtureBounds(s->child, {140, 50, 120, 60});
        // The manual pass ends with the animator tick, which dispatches the fresh animator once at raw 0.
        LayoutController::Get(s->window).ProcessLayouts();
        const Observation& first = s->observationCount ? s->observations[0] : NoObservation();
        r.Equal("first.count", s->observationCount, 1);
        r.Near("first.raw", first.raw, 0, .00001);
        r.Near("first.alpha", first.progress, curve.expected(0), .00001);
        CheckObservation(r, first, LayoutTransitionSlot::CHANGE, LayoutChangeCause::OTHER, {40, 30, 80, 40}, {140, 50, 120, 60}, "first");
      }},
                                                                                                                                  {"natural-finish", 9, [curve](Run& r)
      {
        // Waits for the natural finish (an event) on the real animator clock; the frame deadline is
        // only a liveness bound. Every sample the clock produced must lie on the curve; how many
        // samples there are and when they arrive is not judged.
        auto s = r.State<State>();
        r.AfterFrame([s, curve](Run& v)
        {
          bool     monotonic         = true;
          uint32_t alphaMismatches   = 0;
          uint32_t contextMismatches = 0;
          for(std::size_t i = 0; i < s->observationCount; ++i)
          {
            const Observation& sample = s->observations[i];
            if(i > 0 && sample.raw < s->observations[i - 1].raw) monotonic = false;
            if(!(std::abs(sample.progress - curve.expected(sample.raw)) <= .00001f)) ++alphaMismatches;
            if(sample.slot != LayoutTransitionSlot::CHANGE || sample.cause != LayoutChangeCause::OTHER ||
               !SameRect(sample.from, {40, 30, 80, 40}) || !SameRect(sample.to, {140, 50, 120, 60}))
              ++contextMismatches;
          }
          const Observation& last = LastObservation(*s);
          v.Truth("samples.count", s->observationCount >= 2);
          v.Truth("samples.raw-monotonic", monotonic);
          v.Equal("samples.alpha-mismatches", alphaMismatches, 0);
          v.Equal("samples.context-mismatches", contextMismatches, 0);
          v.Near("last.raw", last.raw, 1, .00001);
          v.Near("last.alpha", last.progress, curve.expected(1), .00001);
          v.Equal("lifecycle.start", s->starts[2], 1);
          v.Equal("lifecycle.finish", s->finishes[2], 1);
          v.Truth("capture.complete", !s->observationOverflow);
        }, [s]
        { return s->finishes[2] >= 1; }, s->root);
      }}}});
    }
    cases.push_back({"TR07.delay-cap-restart", "Delay, raw 0 first dispatch, per-tick delta cap and a drained restart on the real tick driver", {{"mount", 4, [](Run& r)
    { Mount(r, NewState(r)); }},
                                                                                                                                      {"delay-and-cap", 6, [](Run& r)
    {
      auto s = r.State<State>();
      CaptureLifecycle(s);
      s->transition.SetChangeAnimator(LayoutAnimatorCallback::New(s.get(), &State::OnAnimator), AnimatorTiming(.2f, .2f));
      SetFixtureBounds(s->child, {140, 30, 80, 40});
      LayoutController::Get(s->window).ProcessLayouts();
      // Real clock, 0.2 s delay then 0.2 s duration. The 0.1 s per-tick delta cap bounds every raw
      // step by 0.1 / 0.2 whatever the frame timing; no elapsed time is judged.
      r.AfterFrame([s](Run& v)
      {
        bool monotonic = true;
        bool bounded   = true;
        for(std::size_t i = 1; i < s->observationCount; ++i)
        {
          const float step = s->observations[i].raw - s->observations[i - 1].raw;
          if(step < 0) monotonic = false;
          if(!(step <= .1f / .2f + .00001f)) bounded = false;
        }
        const Observation& first = s->observationCount ? s->observations[0] : NoObservation();
        v.Near("first.raw", first.raw, 0, .00001);
        v.Truth("raw-monotonic", monotonic);
        v.Truth("raw-step-bound", bounded);
        v.Near("last.raw", LastObservation(*s).raw, 1, .00001);
        v.Equal("finish.once", s->finishes[2], 1);
        // The finalized animator is never dispatched again.
        v.Equal("drained.no-late-context", s->observationCount, s->observationsAtFinish[2]);
      }, [s]
      { return s->finishes[2] >= 1; }, s->root);
    }},
                                                                                                                                      {"restore-natural-timer", 3, [](Run& r)
    {
      auto s = r.State<State>();
      s->ResetObservations();
      s->transition.SetChangeAnimator(LayoutAnimatorCallback::New(s.get(), &State::OnAnimator), AnimatorTiming(.15f));
      SetFixtureBounds(s->child, {40, 30, 80, 40});
      LayoutController::Get(s->window).ProcessLayouts();
      const Observation& first = s->observationCount ? s->observations[0] : NoObservation();
      r.Near("restart.first.raw", first.raw, 0, .00001);
      // The new animator has to reach its natural finish on the real clock. The frame wait itself
      // drives ticks, so this does not tell whether the dispatcher restarted its own timer.
      r.AfterFrame([s](Run& v)
      {
        v.Equal("restart.natural.finish", s->finishes[2], 1);
        v.Truth("restart.natural.raw-one", s->observationCount > 1 && LastObservation(*s).raw == 1.f);
      }, [s]
      { return s->finishes[2] >= 1; }, s->root);
    }}}});
    for(float duration : {0.f, -.1f})
      cases.push_back({"TR07.nonpositive-" + std::to_string(duration), "Non-positive duration ignores delay and completes in its first tick", {{"mount", 4, [](Run& r)
      { Mount(r, NewState(r)); }},
                                                                                                                                               {"instant-tick", 4, [duration](Run& r)
      {
        auto s = r.State<State>();
        CaptureLifecycle(s);
        s->transition.SetChangeAnimator(LayoutAnimatorCallback::New(s.get(), &State::OnAnimator), AnimatorTiming(duration, 10));
        SetFixtureBounds(s->child, {140, 30, 80, 40});
        // The first animator tick at the end of this manual pass dispatches raw 1 and finalizes.
        LayoutController::Get(s->window).ProcessLayouts();
        r.Equal("instant.callbacks", s->observationCount, 1);
        r.Near("instant.raw", s->observations[0].raw, 1);
        r.Equal("instant.start", s->starts[2], 1);
        r.Equal("instant.finish", s->finishes[2], 1);
      }}}});
    return cases;
  }
};
REGISTER_MANUAL_TEST(Tc134)
