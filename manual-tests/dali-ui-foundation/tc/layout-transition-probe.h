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
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// Read current values without changing the transition. Sampling is requested
// every 16ms for 0.8s; neither the timer interval nor frame delivery is guaranteed.
// The first sample may precede layout. END means the observation window ended;
// STOP means it was superseded or the TC exited, not normal animation completion.
class LayoutTransitionProbe : public Dali::ConnectionTracker
{
public:
  explicit LayoutTransitionProbe(std::string tag)
  : mTag(std::move(tag))
  {
  }

  // EXIT: call before Remove to retain the pre-EXIT state. Other actions: call
  // after Add/request changes. The first sample is labelled pre-layout.
  void Start(Dali::Actor actor, std::string phase, std::vector<Dali::Actor> peers = {})
  {
    Stop();
    mActor   = actor;
    mPeers   = std::move(peers);
    mPhase   = std::move(phase);
    mStart   = Clock::now();
    mSamples = 0;
    ++mRun;
    Sample("pre-layout");
    mTimer = Dali::Timer::New(16);
    mTimer.TickSignal().Connect(this, &LayoutTransitionProbe::OnTick);
    mTimer.Start();
  }

  void Stop()
  {
    if(mTimer) mTimer.Stop();
    mTimer.Reset();
    if(mActor)
    {
      Sample("stop");
      std::cout << "[" << mTag << "][" << mPhase << "][STOP] run=" << mRun << " samples=" << mSamples << std::endl;
    }
    mActor.Reset();
    mPeers.clear();
  }

  // Log each actual callback independently of current-property timer samples.
  // The event-thread context is not the progress of the last rendered frame.
  static void LogAnimator(const char* tag, const Dali::Ui::LayoutAnimatorContext& context)
  {
    auto view = context.GetView();
    if(!view) return;
    const auto         from = context.GetFromBounds(), to = context.GetToBounds();
    std::ostringstream out;
    out << std::fixed << std::setprecision(6) << '[' << tag << "][CONTEXT] clock=" << Timestamp()
        << " id=" << view.GetProperty<int>(Dali::Actor::Property::ID)
        << " slot=" << unsigned(context.GetSlot()) << " raw=" << context.GetRawProgress()
        << " eased=" << context.GetProgress()
        << " from=" << from.x << ',' << from.y << ',' << from.width << ',' << from.height
        << " to=" << to.x << ',' << to.y << ',' << to.width << ',' << to.height;
    std::cout << out.str() << std::endl;
  }

private:
  using Clock = std::chrono::steady_clock;

  static double Timestamp()
  {
    return std::chrono::duration<double>(Clock::now().time_since_epoch()).count();
  }
  double Elapsed() const
  {
    return std::chrono::duration<double>(Clock::now() - mStart).count();
  }
  static void Bounds(std::ostream& out, Dali::Actor actor)
  {
    const auto p      = actor.GetCurrentProperty<Dali::Vector3>(Dali::Actor::Property::POSITION);
    const auto s      = actor.GetCurrentProperty<Dali::Vector3>(Dali::Actor::Property::SIZE);
    const auto parent = actor.GetParent();
    out << " id=" << actor.GetProperty<int>(Dali::Actor::Property::ID)
        << " name=" << std::quoted(actor.GetProperty<Dali::String>(Dali::Actor::Property::NAME).CStr())
        << " parent=" << (parent ? parent.GetProperty<int>(Dali::Actor::Property::ID) : 0)
        << " x=" << p.x << " y=" << p.y << " w=" << s.width << " h=" << s.height
        << " opacity=" << actor.GetCurrentProperty<float>(Dali::Actor::Property::OPACITY)
        << " scene=" << actor.GetProperty<bool>(Dali::Actor::Property::CONNECTED_TO_SCENE);
  }
  void Sample(const char* kind)
  {
    ++mSamples;
    std::ostringstream out;
    out << std::fixed << std::setprecision(6)
        << '[' << mTag << "][" << mPhase << "] t=" << Elapsed() << " clock=" << Timestamp()
        << " run=" << mRun << " sample=" << mSamples << " kind=" << kind << " peers=" << mPeers.size();
    Bounds(out, mActor);
    out << '\n';
    for(auto peer : mPeers)
    {
      out << '[' << mTag << "][" << mPhase << "][PEER] run=" << mRun << " sample=" << mSamples;
      Bounds(out, peer);
      out << '\n';
    }
    std::cout << out.str() << std::flush;
  }
  bool OnTick()
  {
    if(!mActor) return false;
    Sample("timer");
    if(Elapsed() < .8) return true;
    std::cout << "[" << mTag << "][" << mPhase << "][END] run=" << mRun << " samples=" << mSamples << std::endl;
    mActor.Reset();
    mPeers.clear();
    return false;
  }

  std::string              mTag, mPhase;
  Dali::Actor              mActor;
  std::vector<Dali::Actor> mPeers;
  Dali::Timer              mTimer;
  Clock::time_point        mStart{};
  unsigned                 mRun{0}, mSamples{0};
};

#endif // MANUAL_TEST_LAYOUT_TRANSITION_PROBE_H
