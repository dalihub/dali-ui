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
constexpr uint32_t SAMPLES = 31;
// Applies the sparse/dense/same-value mutation and lets the controller lay the tree out.
// A same-value mutation invalidates nothing, so the sibling nudge forces the pass that
// proves the workload is served from its caches.
void Mutate(Run& r, Workload& w, WorkloadStage& host, uint32_t count, bool dense, bool noOp, float height)
{
  if(dense)
    for(auto& leaf : w.leaves) leaf.SetRequestedHeight(height);
  else
    w.leaves[count / 2].SetRequestedHeight(height);
  if(noOp) host.Nudge();
  ProcessNow(r);
}

ImmediateWorkResult ObserveImmediate(const Workload& w)
{
  ImmediateWorkResult immediate;
  immediate.Add(w.root.GetMeasuredSize().width, w.width);
  immediate.Add(w.root.GetMeasuredSize().height, w.height);
  immediate.Add(LayoutValidation::Bounds(w.root), {0, 0, w.width, w.height});
  ObserveMeasuredSlots(immediate, w);
  ObserveArrangedGeometry(immediate, w);
  return immediate;
}
// Repeats the mutation on one mounted workload and checks every sample. Sparse and dense
// heights alternate 15/16 from the mounted 16, so every sample changes them.
void RepeatUpdate(Run& r, uint32_t count, bool dense, bool noOp)
{
  auto w    = FlatWorkload(WorkloadKind::STACK, count);
  auto host = MountWorkload(r, w, true);
  ProcessNow(r);
  for(uint32_t sample = 0; sample < SAMPLES; ++sample)
  {
    const float height = noOp ? 16.f : ((sample % 2) ? 16.f : 15.f);
    Mutate(r, w, host, count, dense, noOp, height);
    for(uint32_t i = 0; i < count; ++i) w.expected[i].height = dense || i == count / 2 ? height : 16.f;
    const auto immediate = ObserveImmediate(w);
    immediate.Check(r, Id("sample.immediate", sample).c_str(), 12ull + 6ull * count);
    VerifyWorkload(r, w, Id("sample.geometry", sample).c_str());
  }
  VerifyFinalGeometry(r, host, w, "rendered", true);
}

void UpdateWork(Run& r, uint32_t count, bool dense, bool noOp)
{
  auto w      = FlatWorkload(WorkloadKind::STACK, count);
  auto leaves = CountLeafProducers(r, w);
  auto host   = MountWorkload(r, w, true);
  ProcessNow(r);
  const uint32_t changed = noOp ? 0u : dense ? count : 1u;
  const float    height  = noOp ? 16.f : 15.f;
  for(uint32_t i = 0; i < count; ++i) w.expected[i].height = dense || i == count / 2 ? height : 16.f;
  const uint64_t measuresBefore = leaves->measures;
  const uint64_t arrangesBefore = leaves->arranges;
  Mutate(r, w, host, count, dense, noOp, height);
  // Only the changed leaves run their producers again; every other leaf is served from its caches.
  CheckLeafProducers(r, *leaves, measuresBefore, arrangesBefore, changed, changed);
  r.Size("output.measured", w.root.GetMeasuredSize(), {w.width, w.height});
  r.Rect("output.arranged", LayoutValidation::Bounds(w.root), {0, 0, w.width, w.height});
  VerifyWorkload(r, w, "geometry");
  VerifyFinalGeometry(r, host, w, "rendered", true);
}

class Tc153 : public Case
{
public:
  Tc153()
  : Case("TC153", "Update performance", "Test sparse, dense and same-value updates")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> result;
    for(uint32_t count : {16u, 128u, 512u})
    {
      Scenario s{"n" + std::to_string(count), "Sparse, dense and same-value mutation through the controller on a rendered stack", {}};
      // The repeated samples first, then the leaf producer budgets; each step mounts its own workload.
      // A sample emits 6 rows (immediate 3, geometry 3); the final rendered check adds 3 + 4N.
      s.steps.push_back({"sparse", 6u * SAMPLES + 3u + 4u * count, [count](Run& r) { RepeatUpdate(r, count, false, false); }});
      s.steps.push_back({"dense", 6u * SAMPLES + 3u + 4u * count, [count](Run& r) { RepeatUpdate(r, count, true, false); }});
      s.steps.push_back({"same-value", 6u * SAMPLES + 3u + 4u * count, [count](Run& r) { RepeatUpdate(r, count, true, true); }});
      s.steps.push_back({"work-sparse", LEAF_BUDGET_CHECKS + 12 + 4u * count, [count](Run& r) { UpdateWork(r, count, false, false); }});
      s.steps.push_back({"work-dense", LEAF_BUDGET_CHECKS + 12 + 4u * count, [count](Run& r) { UpdateWork(r, count, true, false); }});
      s.steps.push_back({"work-same-value", LEAF_BUDGET_CHECKS + 12 + 4u * count, [count](Run& r) { UpdateWork(r, count, true, true); }});
      result.push_back(std::move(s));
    }
    return result;
  }
};
} //namespace
REGISTER_MANUAL_TEST(Tc153)
