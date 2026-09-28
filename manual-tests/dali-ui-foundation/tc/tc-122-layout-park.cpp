/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include "layout-validation-fixtures.h"
struct ParkState
{
  std::shared_ptr<LayoutValidation::Fixtures::Probe> probe;
  bool                                               active{false}, saturated{false}, quiet{false};
  uint32_t                                           parkPasses{0}, quietLayoutPasses{0};
};
// The passive quiet interval starts after a settle delay that lets work already queued by the
// arming step retire, and lasts QUIET_MS without any input.
constexpr uint32_t QUIET_SETTLE_MS = 1000;
constexpr uint32_t QUIET_MS        = 10000;

using namespace LayoutValidation;
using namespace LayoutValidation::Fixtures;

class Tc122 : public Case
{
public:
  Tc122()
  : Case("TC122", "Parked invalidation", "Check in-pass self-invalidation stays parked")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    Scenario scenario;
    scenario.id          = "K17.passive-park";
    scenario.description = "No self-wake while in-processing work remains pending";
    scenario.steps.push_back({"prepare", 4, [](Run& r)
    {auto s=std::make_shared<ParkState>();s->probe=Probed(r,37,19);r.SetState(s);r.AfterLayout({s->probe->view},[s](Run&done){done.Rect("settled",done.Snapshot(s->probe->view),LayoutRect(0,0,37,19));});r.Attach(s->probe->view); }});
    scenario.steps.push_back({"arm-passive-observation", 5, [](Run& r)
    {
      auto s              = r.State<ParkState>();
      s->active           = true;
      s->probe->onMeasure = [s]()
      {if(!s->active)return;++s->parkPasses;if(s->parkPasses>4096){s->saturated=true;s->active=false;return;}std::printf("TC122_PARK producer=%u\n",s->parkPasses);std::fflush(stdout);s->probe->view.InvalidateMeasure(); };
      r.OnCleanup([s]()
      {s->active=false;s->quiet=false;s->probe->onMeasure={}; });
      s->probe->view.InvalidateMeasure();
      LayoutController::Get(r.GetWindow()).ProcessLayouts();
      r.Truth("producer-entered", s->parkPasses > 0);
      r.Truth("not-saturated", !s->saturated);
      std::printf("TC122_PARK_ARMED producer=%u\n", s->parkPasses);
      std::fflush(stdout);
      // Passive quiet interval. The settle delay lets the completion of the manual drain above (it
      // is delivered by the next automatic cycle) and the input that started this step be
      // consumed; the parked request must then stay pending with no layout pass and no producer
      // run until the quiet timer. A timer tick only emits its signal, and these callbacks read
      // state and append rows only, so neither releases the parked request.
      r.Delay(QUIET_SETTLE_MS, [s](Run& settled)
      {
        const uint32_t startPasses = s->parkPasses;
        s->quiet                   = true;
        LayoutController::Get(settled.GetWindow()).LayoutFinishedSignal().Connect(&settled, [s](Dali::Window)
        { if(s->quiet) ++s->quietLayoutPasses; });
        settled.Delay(QUIET_MS, [s, startPasses](Run& done)
        {
          s->quiet = false;
          done.Equal("quiet.layout-passes", s->quietLayoutPasses, 0);
          done.Equal("quiet.producer-unchanged", s->parkPasses, startPasses);
          done.Truth("quiet.not-saturated", !s->saturated);
          std::printf("TC122_PARK_QUIET before=%u after=%u\n", startPasses, s->parkPasses);
          std::fflush(stdout);
        });
      });
    }});
    scenario.steps.push_back({"external-event-and-stop", 5, [](Run& r)
    {auto s=r.State<ParkState>();uint32_t before=s->parkPasses;s->probe->view.InvalidateMeasure();LayoutController::Get(r.GetWindow()).ProcessLayouts();uint32_t after=s->parkPasses;s->active=false;s->probe->onMeasure={};r.Truth("external-progress",after>before);r.Truth("not-saturated",!s->saturated);r.Truth("disarmed",!s->active);r.Equal("captured-before-positive",before>0,1);r.Equal("captured-after-positive",after>0,1);std::printf("TC122_PARK_STOP before=%u after=%u\n",before,after);std::fflush(stdout);s->probe->view.InvalidateMeasure(); }});
    scenario.steps.push_back({"recover-window-and-drain", 10, [](Run& r)
    {
      r.SuppressAutomaticPresentation();
      auto           s             = r.State<ParkState>();
      const uint32_t stoppedPasses = s->parkPasses;
      r.Truth("recovery.disarmed", !s->active);
      r.Truth("recovery.callback-cleared", !s->probe->onMeasure);
      r.AfterLayout({s->probe->view}, [s, stoppedPasses](Run& done)
      {
        // This fence must arrive from the natural post-process episode, without a manual drain.
        done.Rect("recovery.geometry", done.Snapshot(s->probe->view), LayoutRect(0, 0, 37, 19));
        done.Equal("recovery.park-producer-stopped", s->parkPasses, stoppedPasses);
        // One observation after the fence lets an already queued idle wake retire; the settled
        // probe must not be measured again. This timer is created only after PARK is disarmed
        // and naturally settled.
        const uint32_t fenceMeasures = s->probe->measures;
        done.Delay(100, [s, stoppedPasses, fenceMeasures](Run& drained)
        {
          drained.Equal("drained.no-producer", s->probe->measures, fenceMeasures);
          drained.Equal("drained.park-producer-stopped", s->parkPasses, stoppedPasses);
          drained.Truth("drained.not-saturated", !s->saturated);
        });
      });
      s->probe->view.InvalidateMeasure();
    }});
    out.push_back(std::move(scenario));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc122)
