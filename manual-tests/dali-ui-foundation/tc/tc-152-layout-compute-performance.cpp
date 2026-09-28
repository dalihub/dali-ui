/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include "layout-validation-workloads.h"

using namespace LayoutValidation;
namespace
{
enum class Phase
{
  CONSTRUCT,
  FIRST_PASS,
  WARM_PASS,
  IDLE_PASS
};
const char* PhaseName(Phase phase)
{
  switch(phase)
  {
    case Phase::CONSTRUCT:
      return "construction";
    case Phase::FIRST_PASS:
      return "first_pass";
    case Phase::WARM_PASS:
      return "warm_pass";
    case Phase::IDLE_PASS:
      return "idle_pass";
  }
  return "invalid";
}
constexpr uint32_t WARM_OPERATIONS = 32;
constexpr uint32_t IDLE_OPERATIONS = 4096;
constexpr uint32_t SAMPLES         = 31;

// Immediate observation right after the operations of a phase: requested inputs for construction,
// otherwise the controller-produced measured slots and event-side arranged geometry.
ImmediateWorkResult ObserveImmediate(const Workload& w, Phase phase)
{
  ImmediateWorkResult result;
  if(phase == Phase::CONSTRUCT)
  {
    result.Add(w.root.GetRequestedWidth(), w.width);
    result.Add(w.root.GetRequestedHeight(), w.height);
    result.Add(static_cast<float>(w.root.GetChildViewCount()), static_cast<float>(w.leaves.size()));
    for(const auto& leaf : w.leaves)
    {
      result.Add(leaf.GetRequestedWidth(), 8);
      result.Add(leaf.GetRequestedHeight(), 16);
    }
  }
  else
  {
    result.Add(w.root.GetMeasuredSize().width, w.width);
    result.Add(w.root.GetMeasuredSize().height, w.height);
    result.Add(LayoutValidation::Bounds(w.root), {0, 0, w.width, w.height});
    ObserveMeasuredSlots(result, w);
    ObserveArrangedGeometry(result, w);
  }
  return result;
}
uint64_t ImmediateValues(Phase phase, uint32_t count)
{
  return phase == Phase::CONSTRUCT ? 3 + 2ull * count : 12 + 6ull * count;
}
// Performs the operations of `phase` on a mounted workload.
void RunPhase(Run& r, WorkloadStage& host, Phase phase)
{
  switch(phase)
  {
    case Phase::CONSTRUCT:
      break;
    case Phase::FIRST_PASS:
      ProcessNow(r);
      break;
    case Phase::WARM_PASS:
      for(uint32_t i = 0; i < WARM_OPERATIONS; ++i)
      {
        host.Nudge();
        ProcessNow(r);
      }
      break;
    case Phase::IDLE_PASS:
      for(uint32_t i = 0; i < IDLE_OPERATIONS; ++i) ProcessNow(r);
      break;
  }
}

void CheckWork(Run& r, WorkloadKind kind, uint32_t count, Phase phase)
{
  auto w      = FlatWorkload(kind, count);
  auto leaves = CountLeafProducers(r, w);
  auto host   = MountWorkload(r, w, true);
  if(phase != Phase::FIRST_PASS) ProcessNow(r); // settle before the counted operations
  const uint64_t measuresBefore = leaves->measures;
  const uint64_t arrangesBefore = leaves->arranges;
  RunPhase(r, host, phase);
  const auto immediate = ObserveImmediate(w, phase);
  immediate.Check(r, "immediate", ImmediateValues(phase, count));
  // The first pass measures and arranges every leaf exactly once; a warm or idle pass serves the
  // whole workload from its caches and runs no leaf producer.
  const uint64_t budget = phase == Phase::FIRST_PASS ? count : 0;
  CheckLeafProducers(r, *leaves, measuresBefore, arrangesBefore, budget, budget);
  VerifyWorkload(r, w, "geometry");
  VerifyFinalGeometry(r, host, w, "rendered", true);
}

// Repeats the operations of `phase` on a freshly constructed workload and checks every sample.
void RepeatPhase(Run& r, WorkloadKind kind, uint32_t count, Phase phase)
{
  WorkloadStage host;
  Workload      w;
  for(uint32_t sample = 0; sample < SAMPLES; ++sample)
  {
    w = FlatWorkload(kind, count);
    if(!host.stage)
      host = MountWorkload(r, w, true);
    else
      ReplaceWorkload(host, w);
    if(phase != Phase::FIRST_PASS) ProcessNow(r); // settle before the operations
    RunPhase(r, host, phase);
    const auto immediate = ObserveImmediate(w, phase);
    immediate.Check(r, Id("sample.immediate", sample).c_str(), ImmediateValues(phase, count));
    VerifyWorkload(r, w, Id("sample.geometry", sample).c_str());
  }
  VerifyFinalGeometry(r, host, w, "rendered", true);
}

class Tc152 : public Case
{
public:
  Tc152()
  : Case("TC152", "Compute performance", "Test first/warm/idle passes on flat trees")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> result;
    for(auto kind : {WorkloadKind::STACK, WorkloadKind::FLEX, WorkloadKind::GRID, WorkloadKind::ABSOLUTE})
      for(uint32_t count : {16u, 128u, 512u})
      {
        Scenario s{std::string(KindName(kind)) + "-n" + std::to_string(count), "Controller-driven passes on a rendered workload: repeated immediate results and exact leaf producer budgets", {}};
        // The repeated samples first, then the leaf producer budgets; every step builds its own workloads.
        // A sample emits 6 rows (immediate 3, geometry 3); the final rendered check adds 3 + 4N.
        for(auto phase : {Phase::CONSTRUCT, Phase::FIRST_PASS, Phase::WARM_PASS, Phase::IDLE_PASS})
          s.steps.push_back({PhaseName(phase), 6u * SAMPLES + 3u + 4u * count, [kind, count, phase](Run& r) { RepeatPhase(r, kind, count, phase); }});
        for(auto phase : {Phase::FIRST_PASS, Phase::WARM_PASS, Phase::IDLE_PASS})
          s.steps.push_back({std::string("work-") + PhaseName(phase), LEAF_BUDGET_CHECKS + 9 + 4u * count, [kind, count, phase](Run& r) { CheckWork(r, kind, count, phase); }});
        result.push_back(std::move(s));
      }
    return result;
  }
};
} //namespace
REGISTER_MANUAL_TEST(Tc152)
