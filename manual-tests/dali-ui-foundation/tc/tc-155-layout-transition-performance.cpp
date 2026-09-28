/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <limits>
#include "layout-validation-transition-fixtures.h"
using namespace LayoutValidation;
using namespace LayoutValidation::TransitionFixtures;
namespace
{
constexpr uint32_t SAMPLE_COUNT       = 31;
constexpr uint32_t UPDATE_OPERATIONS  = 9;
constexpr uint32_t DORMANT_OPERATIONS = 128;

void PerformanceAnimator(const LayoutAnimatorContext&)
{
}
struct PerformanceState : State
{
  std::vector<View> leaves;
  uint32_t          toggle{0};
  uint32_t          sample{0};
  uint32_t          ticks{0};
  unsigned          mode{0};
  unsigned          expectedCount{0};
  void              Update()
  {
    const float width = (++toggle & 1u) ? 81.f : 80.f;
    for(auto& leaf : leaves) leaf.SetRequestedWidth(width);
    LayoutController::Get(window).ProcessLayouts();
  }
  void CountTick(const LayoutAnimatorContext&)
  {
    ++ticks;
  }
};

std::size_t GeometryChecks(unsigned count)
{
  return 5u * count + 2u;
}

void CheckAllLeafTargets(Run& r, const PerformanceState& state, const char* prefix, float expectedWidth, bool useSnapshot = false)
{
  char id[128];
  std::snprintf(id, sizeof(id), "%s.handles", prefix);
  r.Equal(id, state.leaves.size(), state.expectedCount);
  std::snprintf(id, sizeof(id), "%s.logical-children", prefix);
  r.Equal(id, state.root.GetChildViewCount(), state.expectedCount);
  for(unsigned i = 0; i < state.expectedCount; ++i)
  {
    const auto leaf = i < state.leaves.size() ? state.leaves[i] : View{};
    std::snprintf(id, sizeof(id), "%s.leaf-%u.identity-order", prefix, i);
    r.Truth(id, leaf && state.root.GetChildViewAt(i) == leaf);
    const float invalid = std::numeric_limits<float>::quiet_NaN();
    LayoutRect  actual(invalid, invalid, invalid, invalid);
    if(leaf)
    {
      // This existing const getter copies the completed logical result. It does
      // not arrange, flush events, or read an intermediate animated Actor value.
      actual = useSnapshot ? r.Snapshot(leaf) : Dali::Ui::GetImpl(leaf).GetArrangedBounds();
    }
    std::snprintf(id, sizeof(id), "%s.leaf-%u.target", prefix, i);
    r.Rect(id, actual, {0, static_cast<float>(i), expectedWidth, 1});
  }
}

void CheckSample(Run& r, std::shared_ptr<PerformanceState> state)
{
  for(uint32_t operation = 0; operation < UPDATE_OPERATIONS; ++operation) state->Update();

  // The mount leaves width 80. An odd operation count makes every sample's
  // final target differ from the preceding sample, including the first sample.
  const float expectedWidth = (state->sample & 1u) ? 80.f : 81.f;
  char        prefix[96];
  std::snprintf(prefix, sizeof(prefix), "performance.sample-%u", state->sample);
  CheckAllLeafTargets(r, *state, prefix, expectedWidth);
  if(++state->sample < SAMPLE_COUNT)
    r.Delay(20, [state](Run& next)
    { CheckSample(next, state); });
}

/// Builds the workload and attaches it. Shared by the sample and the work-budget scenario
/// families so both start from an identical fixture.
void MountFixture(Run& r, unsigned mode, unsigned count)
{
  auto s    = std::make_shared<PerformanceState>();
  s->window = r.GetWindow();
  s->root   = StackLayout::New();
  s->root.SetRequestedWidth(300);
  s->root.SetRequestedHeight(200);
  s->mode          = mode;
  s->expectedCount = count;
  s->leaves.reserve(count);
  for(unsigned i = 0; i < count; ++i)
  {
    auto leaf = Leaf(80, 1);
    s->root.Add(leaf);
    s->leaves.push_back(leaf);
  }
  s->child = s->leaves.front();
  if(mode != 0)
  {
    s->transition = LayoutTransition::New();
    if(mode == 2) s->transition.ClearChangeTiming();
    if(mode == 3) s->transition.SetChangeTiming(Timing(.4f));
    if(mode == 4) s->transition.SetChangeAnimator(LayoutAnimatorCallback::New(&PerformanceAnimator), AnimatorTiming());
    s->root.SetLayoutTransition(s->transition);
  }
  r.SetState(s);
  r.OnCleanup([s]
  { s->Cleanup(); });
  r.Attach(s->root);
  r.AfterLayout(s->leaves, [s](Run& v)
  { CheckAllLeafTargets(v, *s, "performance.fixture", 80, true); });
}

/// Repeated samples. Never shares a scenario with the work-count step: that step rebinds the
/// transition callbacks (only cleared by State::Cleanup at scenario end) and leaves the fixture
/// width toggled, either of which would invalidate the sample expectations here.
void RunSamples(Run& r, unsigned mode)
{
  auto s = r.State<PerformanceState>();
  if(mode == 1)
  {
    for(unsigned sample = 0; sample < SAMPLE_COUNT; ++sample)
    {
      for(unsigned i = 0; i < DORMANT_OPERATIONS; ++i) LayoutController::Get(s->window).ProcessLayouts();
      char prefix[96];
      std::snprintf(prefix, sizeof(prefix), "performance.sample-%u", sample);
      CheckAllLeafTargets(r, *s, prefix, 80);
    }
  }
  else
  {
    CheckSample(r, s);
  }
}

/// Transition work of one update, observed through the public lifecycle and animator callbacks.
void WorkCount(Run& r, unsigned mode, unsigned count)
{
  auto s = r.State<PerformanceState>();
  if(s->transition)
  {
    // Every lifecycle start is counted; in the active Animator mode the counting callback
    // replaces the empty one, so every dispatched context is counted too.
    s->Bind(s->transition);
    if(mode == 4) s->transition.SetChangeAnimator(LayoutAnimatorCallback::New(s.get(), &PerformanceState::CountTick), AnimatorTiming());
  }
  const uint32_t startsBefore = s->starts[0] + s->starts[1] + s->starts[2];
  const uint32_t ticksBefore  = s->ticks;
  if(mode == 1)
    LayoutController::Get(s->window).ProcessLayouts();
  else
    s->Update();
  // The tick that ends the same ProcessLayouts() dispatches every fresh animator once, at raw 0.
  r.Equal("performance.transition.start-count", s->starts[0] + s->starts[1] + s->starts[2] - startsBefore, mode >= 3 ? count : 0);
  r.Equal("performance.first-tick-count", s->ticks - ticksBefore, mode == 4 ? count : 0);
  CheckAllLeafTargets(r, *s, "performance.work", mode == 1 ? 80.f : 81.f);
}
} //namespace
class Tc155 : public Case
{
public:
  Tc155()
  : Case("TC155", "Transition work regression", "Test layout updates under each transition mode")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> cases;
    for(unsigned mode = 0; mode < 5; ++mode)
      for(unsigned count : {1u, 32u, 128u})
      {
        cases.push_back({"TR15.mode-" + std::to_string(mode) + "-n" + std::to_string(count),
                         "No attachment, dormant, disabled, active Spec, active Animator workload",
                         {{"mount", GeometryChecks(count), [mode, count](Run& r)
                           { MountFixture(r, mode, count); }},
                          {"samples", SAMPLE_COUNT * GeometryChecks(count), [mode](Run& r)
                           { RunSamples(r, mode); }}}});
        cases.push_back({"TR15W.mode-" + std::to_string(mode) + "-n" + std::to_string(count),
                         "Transition starts and first animator ticks of one update on the same workload, in its own scenario so the callbacks it binds and the width it toggles cannot leak into a repeated sample",
                         {{"mount", GeometryChecks(count), [mode, count](Run& r)
                           { MountFixture(r, mode, count); }},
                          {"work-count", GeometryChecks(count) + 2u, [mode, count](Run& r)
                           { WorkCount(r, mode, count); }}}});
      }
    return cases;
  }
};
REGISTER_MANUAL_TEST(Tc155)
