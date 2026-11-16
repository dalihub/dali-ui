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
class Tc154 : public Case
{
public:
  Tc154()
  : Case("TC154", "Workload soak", "Test repeated passes for drift and memory growth")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> result;
    for(auto kind : {WorkloadKind::STACK, WorkloadKind::FLEX, WorkloadKind::GRID, WorkloadKind::ABSOLUTE})
      result.push_back(Single(KindName(kind), "256 controller passes with corruption repair and full geometry comparison on screen", 771, [kind](Run& r) {
        r.RequireExternalVerification("TC154 requires ten real TC entries, cleanup-end evidence, RSS/PSS observations and allocation-stack profiling; see the TC MD");
        auto w    = FlatWorkload(kind, 128);
        auto host = MountWorkload(r, w);
        ProcessNow(r);
        for(uint32_t cycle = 0; cycle < 256; ++cycle)
        {
          const uint32_t index = (cycle * 37u) % 128u;
          w.leaves[index].SetProperty(Actor::Property::POSITION_X, -123.25f);
          w.leaves[index].SetProperty(Actor::Property::SIZE_WIDTH, 1.f);
          host.Nudge();
          ProcessNow(r);
          VerifyWorkload(r, w, Id("cycle", cycle).c_str());
          if(cycle % 16 == 0)
          {
            w.root.InvalidateMeasure();
            w.root.InvalidateArrange();
            ProcessNow(r);
          }
        }
        VerifyFinalGeometry(r, host, w, "rendered", false);
      }));
    for(uint32_t depth : {8u, 64u, 192u})
      result.push_back(Single(("depth-" + std::to_string(depth)).c_str(), "Nested padding: root intrinsic size and every rendered local rectangle", 2u + 4u * depth, [depth](Run& r) {
        r.RequireExternalVerification("TC154 requires ten real TC entries, cleanup-end evidence, RSS/PSS observations and allocation-stack profiling; see the TC MD");
        View                                     inner = Leaf(8, 16, "deep.leaf");
        std::vector<std::pair<View, LayoutRect>> expected;
        std::vector<View>                        fence;
        for(uint32_t i = 0; i < depth; ++i)
        {
          auto parent = StackLayout::New();
          parent.SetPadding(Insets(1, 1, 1, 1));
          parent.SetRequestedWidth(WRAP_CONTENT);
          parent.SetRequestedHeight(WRAP_CONTENT);
          parent.Add(inner);
          expected.push_back({inner, LayoutRect(1, 1, 8.f + 2 * i, 16.f + 2 * i)});
          inner = parent;
        }
        View       root  = inner;
        View       stage = r.Stage(std::min(460.0f, 8.f + 2 * depth), 16.f + 2 * depth);
        stage.Add(root);
        r.AfterRender({stage, root}, [=](Run& done) {
          done.Size("deep.root", root.GetMeasuredSize(), MeasuredSize(8.f + 2 * depth, 16.f + 2 * depth));
          for(uint32_t i = 0; i < depth; ++i) done.Rendered(Id("deep.child", i).c_str(), expected[i].first, expected[i].second);
        });
      }));
    return result;
  }
};
} //namespace
REGISTER_MANUAL_TEST(Tc154)
