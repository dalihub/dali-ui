/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#pragma once
#include <dali-ui-foundation/public-api/layouts/layout-controller.h>
#include <array>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <utility>
#include "layout-validation-support.h"

namespace LayoutValidation
{
enum class WorkloadKind
{
  STACK,
  FLEX,
  GRID,
  ABSOLUTE
};
inline const char* KindName(WorkloadKind kind)
{
  switch(kind)
  {
    case WorkloadKind::STACK:
      return "stack";
    case WorkloadKind::FLEX:
      return "flex";
    case WorkloadKind::GRID:
      return "grid";
    case WorkloadKind::ABSOLUTE:
      return "absolute";
  }
  return "invalid";
}
struct Workload
{
  View                    root;
  std::vector<View>       leaves;
  std::vector<LayoutRect> expected;
  float                   width{0};
  float                   height{16};
};
inline Workload FlatWorkload(WorkloadKind kind, uint32_t count)
{
  Workload w;
  w.width = count ? count * 10.f - 2.f : 0;
  switch(kind)
  {
    case WorkloadKind::STACK:
    {
      auto root = StackLayout::New(StackOrientation::HORIZONTAL);
      root.SetSpacing(2);
      w.root = root;
      break;
    }
    case WorkloadKind::FLEX:
    {
      auto root = FlexLayout::New();
      root.SetDirection(FlexDirection::ROW);
      root.SetWrap(FlexWrap::NO_WRAP);
      root.SetJustifyContent(FlexJustify::FLEX_START);
      root.SetAlignItems(FlexAlign::FLEX_START);
      w.root = root;
      break;
    }
    case WorkloadKind::GRID:
    {
      auto root = GridLayout::New();
      root.SetColumnSpacing(2);
      root.AddRowDefinition(GridLength::Absolute(16));
      for(uint32_t i = 0; i < count; ++i) root.AddColumnDefinition(GridLength::Absolute(8));
      w.root = root;
      break;
    }
    case WorkloadKind::ABSOLUTE:
      w.root = AbsoluteLayout::New();
      break;
  }
  w.root.SetRequestedWidth(w.width);
  w.root.SetRequestedHeight(w.height);
  w.leaves.reserve(count);
  w.expected.reserve(count);
  for(uint32_t i = 0; i < count; ++i)
  {
    auto leaf = Leaf(8, 16, Id("workload.leaf", i).c_str());
    if(kind == WorkloadKind::FLEX)
    {
      leaf.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(8).SetFlexGrow(0).SetFlexShrink(0));
      if(i + 1 < count) leaf.SetMargin(Insets(0, 2, 0, 0));
    }
    if(kind == WorkloadKind::GRID) leaf.SetLayoutParams(GridLayoutParams::New().SetColumn(i));
    if(kind == WorkloadKind::ABSOLUTE) leaf.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(i * 10.f, 0, 8, 16)));
    w.root.Add(leaf);
    w.leaves.push_back(leaf);
    w.expected.emplace_back(i * 10.f, 0, 8, 16);
  }
  return w;
}
/// On-screen host for a workload: an opaque stage holding the workload root plus a sibling
/// leaf whose width can be nudged to force a controller pass that does not touch the root.
struct WorkloadStage
{
  View  stage, sibling, root;
  float nudge{10};
  void  Nudge()
  {
    sibling.SetRequestedWidth(nudge += 1);
  }
};
inline WorkloadStage MountWorkload(Run& r, const Workload& w, bool isolateRoot = false)
{
  WorkloadStage host;
  host.root    = w.root;
  host.sibling = Leaf(10, 10, "workload.sibling");
  host.sibling.SetRequestedY(w.height + 10);
  const float stageW = std::max(60.0f, std::min(w.width, 460.0f)), stageH = w.height + 30;
  // isolateRoot: attach as an independent Window layout root so the leaf producer counts of a work
  // step see a single drain of the workload (see StageOnWindow). Otherwise mount under the fixture host.
  host.stage = isolateRoot ? r.StageOnWindow(stageW, stageH) : r.Stage(stageW, stageH);
  ColorizeTree(w.root);
  host.stage.Add(w.root);
  host.stage.Add(host.sibling);
  return host;
}
/// Replaces the workload root inside an existing stage (used by the repeated TC152 samples).
inline void ReplaceWorkload(WorkloadStage& host, const Workload& w)
{
  if(host.root && host.root.GetParent()) host.root.Unparent();
  host.root = w.root;
  ColorizeTree(w.root);
  host.stage.Add(w.root);
  host.stage.SetRequestedHeight(w.height + 30);
  host.sibling.SetRequestedY(w.height + 10);
}
/// Runs one synchronous LayoutController pass over the window (the controller path used by
/// applications); pending roots are drained and completion events are queued as usual.
inline void ProcessNow(Run& r)
{
  LayoutController::Get(r.GetWindow()).ProcessLayouts();
}
/// Compares the rendered rectangle (last update-thread state, parent relative) of every leaf
/// with the independent expectation. Emits 3 aggregate rows plus, when `rows`, 4 rows per leaf.
inline void VerifyRendered(Run& run, const Workload& w, const char* id, bool rows = false)
{
  uint32_t mismatches   = 0;
  double   maximumError = 0;
  for(std::size_t i = 0; i < w.leaves.size(); ++i)
  {
    const auto  actual   = Run::RenderedBounds(w.leaves[i]);
    const auto  expected = w.expected[i];
    const float a[]      = {actual.x, actual.y, actual.width, actual.height};
    const float e[]      = {expected.x, expected.y, expected.width, expected.height};
    for(unsigned j = 0; j < 4; ++j)
    {
      const double error = std::abs(static_cast<double>(a[j]) - e[j]);
      if(!std::isfinite(a[j]) || error > GEOMETRY_TOLERANCE) ++mismatches;
      if(!std::isfinite(error))
        maximumError = std::numeric_limits<double>::infinity();
      else
        maximumError = std::max(maximumError, error);
    }
    if(rows) run.Rect(Id(id, static_cast<uint32_t>(i)).c_str(), actual, expected);
  }
  run.Equal((std::string(id) + ".visited").c_str(), w.leaves.size(), w.expected.size());
  run.Equal((std::string(id) + ".mismatches").c_str(), mismatches, 0);
  run.Near((std::string(id) + ".maximum_error").c_str(), maximumError, 0);
}
inline void VerifyWorkload(Run& run, const Workload& w, const char* id, bool rows = false)
{
  uint32_t mismatches   = 0;
  double   maximumError = 0;
  for(std::size_t i = 0; i < w.leaves.size(); ++i)
  {
    const auto  actual   = Bounds(w.leaves[i]);
    const auto  expected = w.expected[i];
    const float a[]      = {actual.x, actual.y, actual.width, actual.height};
    const float e[]      = {expected.x, expected.y, expected.width, expected.height};
    for(unsigned j = 0; j < 4; ++j)
    {
      const double error = std::abs(static_cast<double>(a[j]) - e[j]);
      if(!std::isfinite(a[j]) || error > GEOMETRY_TOLERANCE) ++mismatches;
      if(!std::isfinite(error))
        maximumError = std::numeric_limits<double>::infinity();
      else
        maximumError = std::max(maximumError, error);
    }
    if(rows) run.Rect(Id(id, static_cast<uint32_t>(i)).c_str(), actual, expected);
  }
  run.Equal((std::string(id) + ".visited").c_str(), w.leaves.size(), w.expected.size());
  run.Equal((std::string(id) + ".mismatches").c_str(), mismatches, 0);
  run.Near((std::string(id) + ".maximum_error").c_str(), maximumError, 0);
}
/// Checks the final mounted workload after a fresh render task.
/// Copy the handles and independent expected values because the caller's workload is local.
inline void VerifyFinalGeometry(Run& r, const WorkloadStage& host, const Workload& w, const char* id, bool rows)
{
  const std::string prefix(id);
  r.AfterFrame([w, prefix, rows](Run& done)
  { VerifyRendered(done, w, prefix.c_str(), rows); }, {}, host.stage);
}

struct ImmediateWorkResult
{
  uint64_t values{0};
  uint64_t mismatches{0};
  double   maximumError{0};
  void     Add(float actual, float expected)
  {
    ++values;
    const double error = std::abs(static_cast<double>(actual) - expected);
    if(!std::isfinite(actual) || error > GEOMETRY_TOLERANCE) ++mismatches;
    maximumError = std::isfinite(error) ? std::max(maximumError, error) : std::numeric_limits<double>::infinity();
  }
  void Add(const LayoutRect& actual, const LayoutRect& expected)
  {
    Add(actual.x, expected.x);
    Add(actual.y, expected.y);
    Add(actual.width, expected.width);
    Add(actual.height, expected.height);
  }
  void Check(Run& run, const char* id, uint64_t requiredValues) const
  {
    run.Equal((std::string(id) + ".values").c_str(), values, requiredValues);
    run.Equal((std::string(id) + ".mismatches").c_str(), mismatches, 0);
    run.Near((std::string(id) + ".maximum_error").c_str(), maximumError, 0);
  }
};

inline void ObserveMeasuredSlots(ImmediateWorkResult& result, const Workload& w)
{
  const auto root = w.root.GetMeasuredSize();
  result.Add(root.width, w.width);
  result.Add(root.height, w.height);
  for(std::size_t i = 0; i < w.leaves.size(); ++i)
  {
    const auto slot = w.leaves[i].GetMeasuredSize();
    result.Add(slot.width, w.expected[i].width);
    result.Add(slot.height, w.expected[i].height);
  }
}

inline void ObserveArrangedGeometry(ImmediateWorkResult& result, const Workload& w)
{
  result.Add(LayoutValidation::Bounds(w.root), {0, 0, w.width, w.height});
  for(std::size_t i = 0; i < w.leaves.size(); ++i)
    result.Add(LayoutValidation::Bounds(w.leaves[i]), w.expected[i]);
}

/// Counts the producer runs of every leaf of a workload. Leaves are fixed-size childless Views, so a
/// callback that reports the requested size and returns the slot unchanged lays them out exactly like
/// the default producer. Containers are not counted: a callback on a container replaces its manager.
struct LeafProducerCount
{
  uint64_t     measures{0};
  uint64_t     arranges{0};
  MeasuredSize Measure(View view, float, float)
  {
    ++measures;
    return MeasuredSize(view.GetRequestedWidth(), view.GetRequestedHeight());
  }
  LayoutRect Arrange(View, const LayoutRect& bounds)
  {
    ++arranges;
    return bounds;
  }
};
constexpr std::size_t LEAF_BUDGET_CHECKS = 2;
/// Installs the counting producers on every leaf of `w` and removes them when the scenario is cleaned up.
inline std::shared_ptr<LeafProducerCount> CountLeafProducers(Run& r, const Workload& w)
{
  auto count = std::make_shared<LeafProducerCount>();
  for(View leaf : w.leaves)
  {
    leaf.SetMeasureCallback(MeasureCallback::New(count.get(), &LeafProducerCount::Measure));
    leaf.SetArrangeCallback(ArrangeCallback::New(count.get(), &LeafProducerCount::Arrange));
  }
  const std::vector<View> leaves = w.leaves;
  r.OnCleanup([count, leaves]() {
    for(View leaf : leaves)
    {
      leaf.SetMeasureCallback({});
      leaf.SetArrangeCallback({});
    }
  });
  return count;
}
/// LEAF_BUDGET_CHECKS rows: the leaf producer runs since the given baselines equal the exact budget.
inline void CheckLeafProducers(Run& r, const LeafProducerCount& count, uint64_t measuresBefore, uint64_t arrangesBefore, uint64_t measures, uint64_t arranges)
{
  r.Equal("work.leaf.measure-producers", static_cast<int64_t>(count.measures - measuresBefore), static_cast<int64_t>(measures));
  r.Equal("work.leaf.arrange-producers", static_cast<int64_t>(count.arranges - arrangesBefore), static_cast<int64_t>(arranges));
}
} // namespace LayoutValidation
