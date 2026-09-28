/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include "layout-validation-fixtures.h"

using namespace LayoutValidation;
using namespace LayoutValidation::Fixtures;

namespace
{
struct Mutation
{
  StackLayout            stack;
  View                   stage, sibling, a, b, c;
  std::shared_ptr<Probe> pa;
  float                  nudge{10};
  void                   Nudge()
  {
    sibling.SetRequestedWidth(nudge += 1);
  }
};
std::shared_ptr<Mutation> MountStack(Run& r, float width, float height)
{
  auto s   = std::make_shared<Mutation>();
  s->stack = StackLayout::New(StackOrientation::HORIZONTAL);
  Fixed(s->stack, width, height);
  s->sibling = Leaf(10, 10, "sibling");
  s->sibling.SetRequestedY(height + 10);
  s->stage = r.Stage(width, height + 30);
  s->stage.Add(s->stack);
  s->stage.Add(s->sibling);
  r.SetState(s);
  return s;
}
} // namespace

class Tc124 : public Case
{
public:
  Tc124()
  : Case("TC124", "Replay mutation", "Check tree mutations invalidate cached replay")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    out.push_back(Steps("K19.add-remove", "Changing the child set invalidates the cached replay on screen", {
      {"mount", 8, [](Run& r) {
         auto s = MountStack(r, 200, 60);
         s->stack.SetSpacing(7);
         s->a = Leaf(20, 10, "a");
         s->b = Leaf(30, 10, "b");
         s->c = Leaf(40, 10, "c");
         s->stack.Add(s->a);
         s->stack.Add(s->b);
         r.AfterRender({s->stage, s->stack, s->a, s->b, s->sibling}, [=](Run& r) {
           r.Rendered("initial-a", s->a, LayoutRect(0, 0, 20, 10));
           r.Rendered("initial-b", s->b, LayoutRect(27, 0, 30, 10));
         });
       }},
      {"swap", 10, [](Run& r) {
         auto s = r.State<Mutation>();
         s->stack.Remove(s->a);
         s->stack.Add(s->c);
         r.AfterRender({s->stage, s->stack, s->b, s->c, s->sibling}, [=](Run& r) {
           r.Equal("child-count", s->stack.GetChildViewCount(), 2);
           Identity(r, "first", s->stack.GetChildViewAt(0), s->b);
           r.Rendered("b", s->b, LayoutRect(0, 0, 30, 10));
           r.Rendered("c", s->c, LayoutRect(37, 0, 40, 10));
         });
       }},
      {"remove-all", 4, [](Run& r) {
         auto s = r.State<Mutation>();
         s->stack.RemoveAll();
         r.AfterRender({s->stage, s->stack, s->sibling}, [=](Run& r) {
           r.Size("empty", s->stack.GetMeasuredSize(), MeasuredSize(200, 60));
           r.Equal("empty-count", s->stack.GetChildViewCount(), 0);
           r.Truth("out-of-range", !s->stack.GetChildViewAt(0));
         });
       }},
    }));
    out.push_back(Steps("K20.mutate-during-producer", "A tree mutation inside a producer is applied by a safe later pass", {
      {"mount", 2, [](Run& r) {
         auto s = MountStack(r, 200, 60);
         s->pa  = Probed(r, 20, 10, false);
         s->b   = Leaf(30, 10, "b");
         s->stack.Add(s->pa->view);
         auto added       = std::make_shared<bool>(false);
         StackLayout host = s->stack;
         View        late = s->b;
         s->pa->onMeasure = [host, late, added]() mutable {
           if(!*added)
           {
             *added = true;
             host.Add(late);
           }
         };
         r.AfterRender({s->stage, s->stack, s->pa->view, s->sibling}, [=](Run& r) {
           s->pa->onMeasure = {};
           r.Truth("added", *added);
           r.Equal("children", s->stack.GetChildViewCount(), 2);
         });
       }},
      {"later-pass", 4, [](Run& r) {
         auto s = r.State<Mutation>();
         s->Nudge();
         r.AfterRender({s->stage, s->stack, s->pa->view, s->b, s->sibling}, [=](Run& r) { r.Rendered("later-child", s->b, LayoutRect(20, 0, 30, 10)); });
       }},
    }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc124)
