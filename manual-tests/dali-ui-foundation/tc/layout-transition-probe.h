#ifndef MANUAL_TEST_LAYOUT_TRANSITION_PROBE_H
#define MANUAL_TEST_LAYOUT_TRANSITION_PROBE_H

/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include <dali-ui-foundation/dali-ui-foundation.h>

#include <chrono>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

/**
 * Prints what a transition does to one actor, one line per tick, on stdout
 * (dlog tag STDOUT on Tizen):
 *
 *   [TC78][EXIT] t=0.016 h=79.2 opacity=0.991 scene=1
 *   ...
 *   [TC78][EXIT][END] samples=50
 *
 * t is seconds since the touch; h and opacity are the rendered (current)
 * values, scene is CONNECTED_TO_SCENE. The probe only reads — it changes
 * nothing about the transition.
 *
 * The phase is the slot that was triggered: ENTER (the added child), EXIT (the
 * leaving child), CHANGE (a child whose requested size changed) or TOGGLE (a
 * reflow). With inner actors the line also carries their rendered heights in
 * the same tick, so a reflow of nested items can be compared with its parent:
 *
 *   [TC81][TOGGLE] t=0.112 h=77.0 opacity=1.000 scene=1 inner=72.0,72.0,72.0
 *
 * Why: an automated check reading the accessibility tree cannot see a 0.4 s
 * transition on a slow target — the first tree read after the change takes
 * ~0.5-2 s there — and opacity is not in the tree at all. The child is held by
 * this probe for the sampling window only.
 */
class LayoutTransitionProbe : public Dali::ConnectionTracker
{
public:
  explicit LayoutTransitionProbe(std::string tag)
  : mTag(std::move(tag))
  {
  }

  /// EXIT: call right BEFORE the remove, so the first line is the pre-EXIT state.
  /// ENTER: call right AFTER the add. CHANGE / TOGGLE: call right AFTER the size
  /// change (the first line is the pre-layout state).
  void Start(Dali::Actor actor, std::string phase, std::vector<Dali::Actor> inner = {})
  {
    Stop();
    mActor   = actor;
    mInner   = std::move(inner);
    mPhase   = std::move(phase);
    mStart   = Clock::now();
    mSamples = 0u;
    Sample();
    mTimer = Dali::Timer::New(TICK_MS);
    mTimer.TickSignal().Connect(this, &LayoutTransitionProbe::OnTick);
    mTimer.Start();
  }

  void Stop()
  {
    if(mTimer)
    {
      mTimer.Stop();
      mTimer.Reset();
    }
    mActor.Reset();
    mInner.clear();
  }

private:
  using Clock = std::chrono::steady_clock;

  static constexpr unsigned int TICK_MS  = 16u;
  static constexpr double       WINDOW_S = 0.8; // 2x the 0.4 s transition

  double Elapsed() const
  {
    return std::chrono::duration<double>(Clock::now() - mStart).count();
  }

  void Sample()
  {
    const Dali::Vector3 size    = mActor.GetCurrentProperty<Dali::Vector3>(Dali::Actor::Property::SIZE);
    const float         opacity = mActor.GetCurrentProperty<float>(Dali::Actor::Property::OPACITY);
    const bool          scene   = mActor.GetProperty<bool>(Dali::Actor::Property::CONNECTED_TO_SCENE);
    char                line[128];
    std::snprintf(line, sizeof(line), "[%s][%s] t=%.3f h=%.1f opacity=%.3f scene=%d",
                  mTag.c_str(), mPhase.c_str(), Elapsed(), size.height, opacity, scene ? 1 : 0);
    std::string out(line);
    for(std::size_t i = 0; i < mInner.size(); ++i)
    {
      std::snprintf(line, sizeof(line), "%s%.1f", i == 0u ? " inner=" : ",",
                    mInner[i].GetCurrentProperty<Dali::Vector3>(Dali::Actor::Property::SIZE).height);
      out += line;
    }
    std::cout << out << std::endl;
    ++mSamples;
  }

  bool OnTick()
  {
    if(!mActor)
    {
      return false;
    }
    Sample();
    if(Elapsed() < WINDOW_S)
    {
      return true;
    }
    std::cout << "[" << mTag << "][" << mPhase << "][END] samples=" << mSamples << std::endl;
    mActor.Reset();
    mInner.clear();
    return false;
  }

  std::string       mTag;
  std::string       mPhase;
  Dali::Actor       mActor;
  std::vector<Dali::Actor> mInner;
  Dali::Timer       mTimer;
  Clock::time_point mStart{};
  unsigned int      mSamples{0u};
};

#endif // MANUAL_TEST_LAYOUT_TRANSITION_PROBE_H
