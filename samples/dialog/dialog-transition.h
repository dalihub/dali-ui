#pragma once

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <dali-ui-components/public-api/navigator/navigation-transition-spec.h>
#include <dali/public-api/animation/key-frames.h>
#include <dali/public-api/signals/connection-tracker.h>

namespace DialogTransitionSample
{
using namespace Dali;
using namespace Dali::Ui;

enum class Mode { FADE, SLIDE, SCALE };

inline float CenteredY(View content, const Vector2& available)
{
  // This sample uses centered, WRAP_CONTENT AbsoluteLayoutParams with no margin.
  // Measure before Post: a new container has not run its first arrange pass yet.
  return (available.y - content.Measure(available.x, available.y).height) * 0.5f;
}

inline NavigationTransitionSpec CreateSpec(ConnectionTrackerInterface* tracker, Mode mode, float duration, float restingY)
{
  auto spec = NavigationTransitionSpec::New();
  spec.SetDuration(duration);
  spec.EnterSignal().Connect(tracker, [mode, restingY](Animation& animation, View view) {
    KeyFrames frames = KeyFrames::New();
    if(mode == Mode::SLIDE)
    {
      // Keyframes own the position during the first layout pass. Never animate
      // towards zero: the dialog's resting position is relative to its parent.
      frames.Add(0.0f, restingY + 80.0f);
      frames.Add(1.0f, restingY);
      animation.AnimateBetween(Property(view, Actor::Property::POSITION_Y), frames, AlphaFunction::EASE_OUT);
    }
    else if(mode == Mode::SCALE)
    {
      frames.Add(0.0f, Vector3(0.8f, 0.8f, 1.0f));
      frames.Add(1.0f, Vector3::ONE);
      animation.AnimateBetween(Property(view, Actor::Property::SCALE), frames, AlphaFunction::EASE_OUT);
    }
    else
    {
      frames.Add(0.0f, 0.0f);
      frames.Add(1.0f, 1.0f);
      animation.AnimateBetween(Property(view, Actor::Property::OPACITY), frames, AlphaFunction::LINEAR);
    }
  });
  spec.PopExitSignal().Connect(tracker, [mode, restingY](Animation& animation, View view) {
    if(mode == Mode::SLIDE)
    {
      animation.AnimateTo(Property(view, Actor::Property::POSITION_Y), restingY + 80.0f, AlphaFunction::EASE_IN);
    }
    else if(mode == Mode::SCALE)
    {
      animation.AnimateTo(Property(view, Actor::Property::SCALE), Vector3(0.8f, 0.8f, 1.0f), AlphaFunction::EASE_IN);
    }
    else
    {
      animation.AnimateTo(Property(view, Actor::Property::OPACITY), 0.0f, AlphaFunction::LINEAR);
    }
  });
  auto snap = [mode, restingY](View view) {
    if(mode == Mode::SLIDE) view.SetProperty(Actor::Property::POSITION_Y, restingY);
    view.SetProperty(Actor::Property::SCALE, Vector3::ONE);
    view.SetProperty(Actor::Property::OPACITY, 1.0f);
  };
  spec.SnapIncomingSignal().Connect(tracker, snap);
  spec.SnapOutgoingSignal().Connect(tracker, snap);
  return spec;
}
}
