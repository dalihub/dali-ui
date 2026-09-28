/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include <limits>
#include "layout-validation-transition-fixtures.h"
using namespace LayoutValidation;
using namespace LayoutValidation::TransitionFixtures;

class Tc128 : public Case
{
public:
  Tc128()
  : Case("TC128", "Transition configuration and validation", "Check LayoutTransition defaults and validation")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> cases;
    cases.push_back(Single("TR01.defaults-values", "Timing, bounds descriptor, handle roles and idempotent clearing", 20, [](Run& r)
    {
      LayoutTransitionTiming spec;
      LayoutAnimatorTiming   animator;
      r.Near("spec.default.duration", spec.GetDuration().InSeconds(), .3);
      r.Near("spec.default.delay", spec.GetDelay().InSeconds(), 0);
      r.Near("animator.default.duration", animator.GetDuration().InSeconds(), .3);
      r.Near("animator.default.delay", animator.GetDelay().InSeconds(), 0);
      LayoutBoundsEffect effect;
      r.Truth("effect.offset.absent", !effect.HasOffset());
      r.Truth("effect.size.absent", !effect.HasSizeFactor());
      r.Near("effect.size.x", effect.GetSizeFactorX(), 1);
      r.Near("effect.size.y", effect.GetSizeFactorY(), 1);
      r.Near("effect.anchor.x", effect.GetAnchorX(), .5);
      r.Near("effect.anchor.y", effect.GetAnchorY(), .5);
      effect.SetOffset(LayoutBoundsLength::Pixel(-4), LayoutBoundsLength::SelfFraction(.5)).SetSizeFactor(.5, 2).ClearOffset().ClearSizeFactor();
      r.Truth("clear.offset", !effect.HasOffset());
      r.Truth("clear.size", !effect.HasSizeFactor());
      r.Near("clear.offset.x", effect.GetOffset().x.value, 0);
      r.Near("clear.size.x", effect.GetSizeFactorX(), 1);
      auto a          = Leaf(1, 1);
      auto b          = Leaf(1, 1);
      auto transition = LayoutTransition::New();
      a.SetLayoutTransition(transition);
      b.SetSelfLayoutTransition(transition);
      r.Truth("handle.children_role", a.GetLayoutTransition() == transition);
      r.Truth("handle.self_role", b.GetSelfLayoutTransition() == transition);
      a.SetLayoutTransition({});
      b.SetSelfLayoutTransition({});
      r.Truth("clear.children_role", !a.GetLayoutTransition());
      r.Truth("clear.self_role", !b.GetSelfLayoutTransition());
      transition.ClearEnterVisualSpec().ClearExitVisualSpec().ClearEnterBoundsEffect().ClearExitBoundsEffect().ClearEnterAnimator().ClearExitAnimator().ClearChangeAnimator().ClearChangeTiming();
      r.Truth("handle.downcast", static_cast<bool>(LayoutTransition::DownCast(transition)));
      r.Truth("handle.invalid_downcast", !LayoutTransition::DownCast(a));
    }));
    cases.push_back(Single("TR13.registration-rejection", "Rejected values throw the documented exception without poisoning reuse", 35, [](Run& r)
    {
      auto                  transition = LayoutTransition::New();
      constexpr const char* reverse    = "AlphaFunction::REVERSE is not supported by LayoutTransition";
      constexpr const char* terminal   = "AlphaFunction must end at the target value for LayoutTransition bounds animation";
      for(auto alpha : {AlphaFunction::REVERSE, AlphaFunction::BOUNCE, AlphaFunction::SIN})
      {
        const std::string alphaName = alpha == AlphaFunction::REVERSE ? "reverse" : alpha == AlphaFunction::BOUNCE ? "bounce"
                                                                                                                   : "sin";
        auto              timing    = Timing();
        timing.SetAlpha(AlphaFunction(alpha));
        ExpectException(r, ("change.alpha." + alphaName).c_str(), alpha == AlphaFunction::REVERSE ? reverse : terminal,
                        [&]
        { transition.SetChangeTiming(timing); });
        auto effect = LayoutBoundsEffect().SetTiming(timing);
        ExpectException(r, ("bounds.alpha." + alphaName).c_str(), alpha == AlphaFunction::REVERSE ? reverse : terminal,
                        [&]
        { transition.SetEnterBoundsEffect(effect); });
      }
      for(int axis = 0; axis < 2; ++axis)
      {
        auto effect = LayoutBoundsEffect().SetSizeFactor(axis == 0 ? -1.f : 1.f, axis == 1 ? -1.f : 1.f);
        ExpectException(r, axis == 0 ? "bounds.negative-size.x" : "bounds.negative-size.y", "LayoutBoundsEffect sizeFactor must be non-negative",
                        [&]
        { transition.SetExitBoundsEffect(effect); });
      }
      unsigned anchorIndex = 0;
      for(const auto& anchor : {Vector2(-.1f, .5f), Vector2(1.1f, .5f), Vector2(.5f, -.1f), Vector2(.5f, 1.1f)})
      {
        auto effect = LayoutBoundsEffect().SetAnchor(anchor.x, anchor.y);
        ExpectException(r, Id("bounds.anchor", anchorIndex++).c_str(), "LayoutBoundsEffect anchor must be in [0, 1]",
                        [&]
        { transition.SetEnterBoundsEffect(effect); });
      }
      auto spec = ViewAnimationSpec::New();
      spec.Opacity(1, Duration(.2f), AlphaFunction(AlphaFunction::REVERSE));
      ExpectException(r, "visual.reverse", reverse, [&]
      { transition.SetEnterVisualSpec(spec); });
      auto boundsSpec = ViewAnimationSpec::New();
      boundsSpec.PositionX(10, Duration(.2f));
      ExpectException(r, "visual.bounds", "LayoutTransition visual spec entries must not target layout-owned bounds properties (POSITION_X/Y, SIZE_WIDTH/HEIGHT); use the bounds-effect channel",
                      [&]
      { transition.SetExitVisualSpec(boundsSpec); });
      auto state           = NewState(r);
      auto animatorTiming  = AnimatorTiming();
      animatorTiming.SetAlpha(AlphaFunction(AlphaFunction::REVERSE));
      ExpectException(r, "animator.reverse", reverse, [&]
      {
        transition.SetChangeAnimator(LayoutAnimatorCallback::New(state.get(), &State::OnAnimator), animatorTiming);
      });
      for(auto cause : {LayoutChangeCause::SIBLING_ADDED, LayoutChangeCause::SIBLING_REMOVED,
                        LayoutChangeCause::REORDERED, LayoutChangeCause::WINDOW_RESIZED, LayoutChangeCause::OTHER})
        transition.SetChangeTiming(cause, Timing()).ClearChangeTiming(cause);
      transition.SetChangeTiming(Timing()).SetEnterBoundsEffect(LayoutBoundsEffect().SetSizeFactor(2, 0).SetAnchor(0, 1));
      r.Truth("registration.valid-after-rejection", static_cast<bool>(transition));
      Mount(r, state);
    }));
    cases.push_back(Single("TR05.factory-values", "Factory aliases, default distance and anchor endpoints", 64, [](Run& r)
    {
      const float x[]  = {0, 0, -1, 1};
      const float y[]  = {-1, 1, 0, 0};
      const float sx[] = {1, 1, 0, 0};
      const float sy[] = {0, 0, 1, 1};
      const float ax[] = {.5f, .5f, 0, 1};
      const float ay[] = {0, 1, .5f, .5f};
      for(unsigned edge = 0; edge < 4; ++edge)
      {
        unsigned slideAlias = 0;
        for(const auto& effect : {LayoutBoundsEffects::SlideFrom(static_cast<LayoutBoundsEdge>(edge), Timing()),
                           LayoutBoundsEffects::SlideTo(static_cast<LayoutBoundsEdge>(edge), Timing())})
        {
          const std::string prefix = "factory.slide.e" + std::to_string(edge) + (slideAlias++ == 0 ? ".from" : ".to");
          r.Near((prefix + ".x").c_str(), effect.GetOffset().x.value, x[edge]);
          r.Near((prefix + ".y").c_str(), effect.GetOffset().y.value, y[edge]);
          r.Truth((prefix + ".present").c_str(), effect.HasOffset());
          r.Equal((prefix + ".unit").c_str(), static_cast<int>(edge < 2 ? effect.GetOffset().y.unit : effect.GetOffset().x.unit), static_cast<int>(LayoutBoundsUnit::SELF_FRACTION));
        }
        unsigned sizeAlias = 0;
        for(const auto& effect : {LayoutBoundsEffects::ExpandFrom(static_cast<LayoutBoundsEdge>(edge), Timing()),
                           LayoutBoundsEffects::ShrinkTo(static_cast<LayoutBoundsEdge>(edge), Timing())})
        {
          const std::string prefix = "factory.size.e" + std::to_string(edge) + (sizeAlias++ == 0 ? ".expand" : ".shrink");
          r.Near((prefix + ".x").c_str(), effect.GetSizeFactorX(), sx[edge]);
          r.Near((prefix + ".y").c_str(), effect.GetSizeFactorY(), sy[edge]);
          r.Near((prefix + ".anchor.x").c_str(), effect.GetAnchorX(), ax[edge]);
          r.Near((prefix + ".anchor.y").c_str(), effect.GetAnchorY(), ay[edge]);
        }
      }
    }));
    cases.push_back(Single("TR13.nonfinite-robustness", "Non-finite descriptors must not silently become accepted layout geometry", 20, [](Run& r)
    {
      // Every timed channel, not only the bounds descriptor: a non-finite CHANGE duration or
      // delay reaches Animation::New, and a non-finite animator duration or delay is divided
      // into the progress the animator callback is driven with.
      auto        state    = NewState(r);
      const char* fields[] = {"size-x", "size-y", "anchor-x", "anchor-y", "offset-x", "duration",
                              "change-duration", "change-delay", "animator-duration", "animator-delay"};
      for(float invalid : {std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::infinity()})
      {
        for(unsigned field = 0; field < 10; ++field)
        {
          auto transition     = LayoutTransition::New();
          auto effect         = LayoutBoundsEffect().SetTiming(Timing());
          auto timing         = Timing();
          auto animatorTiming = AnimatorTiming();
          if(field == 0) effect.SetSizeFactor(invalid, 1);
          if(field == 1) effect.SetSizeFactor(1, invalid);
          if(field == 2) effect.SetAnchor(invalid, .5f);
          if(field == 3) effect.SetAnchor(.5f, invalid);
          if(field == 4) effect.SetOffset(LayoutBoundsLength::Pixel(invalid), LayoutBoundsLength::Pixel(0));
          if(field == 5) effect.SetTiming(LayoutTransitionTiming(effect.GetTiming()).SetDuration(Duration(invalid)));
          if(field == 6) timing.SetDuration(Duration(invalid));
          if(field == 7) timing.SetDelay(Duration(invalid));
          if(field == 8) animatorTiming.SetDuration(Duration(invalid));
          if(field == 9) animatorTiming.SetDelay(Duration(invalid));
          bool rejected = false;
          try
          {
            if(field < 6)
              transition.SetEnterBoundsEffect(effect);
            else if(field < 8)
              transition.SetChangeTiming(timing);
            else
              transition.SetChangeAnimator(LayoutAnimatorCallback::New(state.get(), &State::OnAnimator), animatorTiming);
          }
          catch(const DaliException&)
          {
            rejected = true;
          }
          const std::string id = std::string("robustness.nonfinite.") + (std::isnan(invalid) ? "nan." : "positive-infinity.") + fields[field] + ".rejected";
          r.Truth(id.c_str(), rejected);
        }
      }
    }));
    return cases;
  }
};
REGISTER_MANUAL_TEST(Tc128)
