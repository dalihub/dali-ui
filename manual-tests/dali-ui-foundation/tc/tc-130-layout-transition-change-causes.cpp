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
namespace
{
// A MATCH_PARENT child at bounds position (40,30) of the 300x200 Absolute root with maximum 120x80:
// its slot (40,30,300,200) is laid out clamped at (40,30,120,80), under RTL from the slot's right
// edge (300 - 40 - 120 = 140). Raising the maximum width to 150 is a CHANGE (cause OTHER) to the newly
// clamped rect (40,30,150,80), under RTL (300 - 40 - 150 = 110). A slot-only change that clamps to the
// same rect makes no CHANGE.
Scenario ClampedTarget(bool rtl)
{
  const float before = rtl ? 140.0f : 40.0f, after = rtl ? 110.0f : 40.0f;
  Scenario    scenario{rtl ? "TR03.clamped-target-rtl" : "TR03.clamped-target-ltr", "A CHANGE transition targets the clamped rect; a slot change that clamps to the same rect starts none", {}};
  scenario.steps.push_back({"mount", 6, [rtl, before](Run& r)
  {
    auto s = NewState(r);
    if(rtl) s->root.SetProperty(Actor::Property::LAYOUT_DIRECTION, LayoutDirection::RIGHT_TO_LEFT);
    s->child.SetRequestedWidth(MATCH_PARENT);
    s->child.SetRequestedHeight(MATCH_PARENT);
    SetFixtureBounds(s->child, {40, 30, -1, -1});
    s->child.SetMaximumWidth(120);
    s->child.SetMaximumHeight(80);
    s->root.Add(s->child);
    r.Attach(s->root);
    r.AfterLayout({s->child}, [s, before](Run& v)
    {
      v.Rect("clamp.mount.target", v.Snapshot(s->child), {before, 30, 120, 80});
      v.Size("clamp.mount.measured", s->child.GetMeasuredSize(), MeasuredSize(120, 80));
    });
  }});
  scenario.steps.push_back({"change", 17, [before, after](Run& r)
  {
    auto s = r.State<State>();
    CaptureLifecycle(s);
    s->child.SetMaximumWidth(150);
    // The manual pass ends with the animator tick, which dispatches every fresh animator once at
    // raw progress 0, so the first context of the child is available right after the call.
    LayoutController::Get(s->window).ProcessLayouts();
    Observation found;
    bool        seen = false;
    for(std::size_t i = 0; i < s->observationCount; ++i)
      if(s->observations[i].viewId == 2)
      {
        found = s->observations[i];
        seen  = true;
        break;
      }
    r.Truth("clamp.context.present", seen);
    CheckObservation(r, found, LayoutTransitionSlot::CHANGE, LayoutChangeCause::OTHER, {before, 30, 120, 80}, {after, 30, 150, 80}, "clamp");
    r.Rect("clamp.logical", ArrangedBounds(s->child), {40, 30, 150, 80});
    r.Size("clamp.measured", s->child.GetMeasuredSize(), MeasuredSize(150, 80));
  }});
  if(!rtl)
  {
    scenario.steps.push_back({"slot-only", 7, [](Run& r)
    {
      auto s = r.State<State>();
      s->root.SetRequestedWidth(280);
      LayoutController::Get(s->window).ProcessLayouts();
      r.Equal("slot-only.no-new-start", s->starts[2], 1);
      r.Rect("slot-only.logical", ArrangedBounds(s->child), {40, 30, 150, 80});
      r.Size("slot-only.measured", s->child.GetMeasuredSize(), MeasuredSize(150, 80));
    }});
  }
  return scenario;
}
} // namespace
class Tc130 : public Case
{
public:
  Tc130()
  : Case("TC130", "Transition CHANGE causes and precedence", "Check CHANGE transition causes and precedence")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> cases;
    for(unsigned kind = 0; kind < 7; ++kind)
      cases.push_back({"TR03.cause-" + std::to_string(kind), "All five causes, resize opt-out and simultaneous cause precedence", {{"mount", 4, [](Run& r)
      {
        auto s = NewState(r, true);
        s->root.Add(s->sibling);
        Mount(r, s, {0, 20, 80, 40});
      }},
                                                                                                                                   {"change", kind == 5 ? 12u : 13u, [kind](Run& r)
      {
        auto s = r.State<State>();
        CaptureLifecycle(s);
        s->transition.SetChangeOnWindowResize(kind != 5);
        auto       cause = LayoutChangeCause::OTHER;
        LayoutRect target{0, 20, 120, 40};
        if(kind == 0 || kind == 6)
        {
          s->root.Add(s->other);
          cause = LayoutChangeCause::SIBLING_ADDED;
        }
        if(kind == 1 || kind == 6)
        {
          s->root.Remove(s->sibling);
          cause    = kind == 6 ? cause : LayoutChangeCause::SIBLING_REMOVED;
          target.y = 0;
        }
        if(kind == 2 || kind == 6)
        {
          if(kind == 2)
            s->child.LowerToBottom();
          else
            s->child.RaiseToTop();
          cause    = LayoutChangeCause::REORDERED;
          target.y = kind == 6 ? 120.f : 0.f;
        }
        s->child.SetRequestedWidth(120);
        if(kind == 3 || kind == 5)
        {
          const auto size   = s->window.GetPositionSize();
          const auto window = s->window;
          r.OnCleanup([window, size]()
          { LayoutController::Get(window).OnWindowResize(size.width, size.height); });
          LayoutController::Get(s->window).OnWindowResize(640, 480);
          cause = LayoutChangeCause::WINDOW_RESIZED;
        }
        // The manual pass ends with the animator tick, which dispatches every fresh animator once at
        // raw progress 0, so the first context of the child is available right after the call.
        LayoutController::Get(s->window).ProcessLayouts();
        Observation found;
        bool        seen = false;
        for(std::size_t i = 0; i < s->observationCount; ++i)
          if(s->observations[i].viewId == 2)
          {
            found = s->observations[i];
            seen  = true;
            break;
          }
        if(kind == 5)
        {
          r.Truth("resize.opt-out.no-context", !seen);
          r.Rect("resize.opt-out.target", LayoutValidation::Bounds(s->child), target);
          r.Rect("resize.opt-out.logical", ArrangedBounds(s->child), target);
          r.Equal("resize.opt-out.start", s->starts[2], 0);
          r.Equal("resize.opt-out.finish", s->finishes[2], 0);
          r.Truth("resize.opt-out.capture", !s->observationOverflow);
        }
        else
        {
          r.Truth("cause.context.present", seen);
          CheckObservation(r, found, LayoutTransitionSlot::CHANGE, cause, {0, 20, 80, 40}, target);
          r.Near("cause.first.raw", found.raw, 0);
          r.Truth("cause.capture.complete", !s->observationOverflow);
        }
      }}}});
    for(float delta : {.49f, .5f, .51f})
      cases.push_back({"TR14.threshold-" + std::to_string(delta), "Half-pixel CHANGE threshold", {{"mount", 4, [](Run& r)
      { Mount(r, NewState(r)); }},
                                                                                                  {"threshold", 5, [delta](Run& r)
      {
        auto s = r.State<State>();
        CaptureLifecycle(s);
        SetFixtureBounds(s->child, {40 + delta, 30, 80, 40});
        LayoutController::Get(s->window).ProcessLayouts();
        r.Rect("threshold.layout", ArrangedBounds(s->child), {40 + delta, 30, 80, 40});
        r.Equal("threshold.start", s->starts[2], delta > .5f ? 1 : 0);
      }}}});
    cases.push_back({"TR01.timing-enable-override", "Default disabled, cause override independent, clear and zero-duration settle", {{"mount", 4, [](Run& r)
    { Mount(r, NewState(r)); }},
                                                                                                                                     {"default-disabled", 5, [](Run& r)
    {
      auto s = r.State<State>();
      s->Bind(LayoutTransition::New());
      s->transition.ClearChangeTiming();
      s->root.SetLayoutTransition(s->transition);
      SetFixtureBounds(s->child, {80, 30, 80, 40});
      LayoutController::Get(s->window).ProcessLayouts();
      r.Rect("disabled.target", LayoutValidation::Bounds(s->child), {80, 30, 80, 40});
      r.Equal("disabled.start", s->starts[2], 0);
    }},
                                                                                                                                     {"override-and-clear", 7, [](Run& r)
    {
      // One synchronous action: the 0.6 s override transition is still in flight when the override
      // is cleared, so the clear provably cancels a running transition.
      auto s = r.State<State>();
      s->transition.SetChangeTiming(LayoutChangeCause::OTHER, Timing(.6f));
      SetFixtureBounds(s->child, {120, 30, 80, 40});
      LayoutController::Get(s->window).ProcessLayouts();
      r.Equal("override.start", s->starts[2], 1);
      s->transition.ClearChangeTiming(LayoutChangeCause::OTHER);
      SetFixtureBounds(s->child, {160, 30, 80, 40});
      LayoutController::Get(s->window).ProcessLayouts();
      r.Rect("cleared.target", LayoutValidation::Bounds(s->child), {160, 30, 80, 40});
      r.Equal("cancel.no-finish", s->finishes[2], 0);
      s->transition.SetChangeTiming(Timing(0, 10));
      SetFixtureBounds(s->child, {200, 30, 80, 40});
      LayoutController::Get(s->window).ProcessLayouts();
      r.Near("zero-duration.ignore-delay", LayoutValidation::Bounds(s->child).x, 200);
    }}}});
    cases.push_back(ClampedTarget(false));
    cases.push_back(ClampedTarget(true));
    return cases;
  }
};
REGISTER_MANUAL_TEST(Tc130)
