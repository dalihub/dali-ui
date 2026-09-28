/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include <dali-ui-foundation/integration-api/view-integ.h>
#include "layout-validation-fixtures.h"

using namespace LayoutValidation;
using namespace LayoutValidation::Fixtures;

namespace
{
struct Ordered
{
  StackLayout stack;
  View        stage, sibling, a, b, c;
  float       nudge{10};
  void        Nudge()
  {
    sibling.SetRequestedWidth(nudge += 1);
  }
  std::vector<View> Fence() const
  {
    return {stage, stack, a, b, c, sibling};
  }
};
std::shared_ptr<Ordered> MountThree(Run& r)
{
  auto s   = std::make_shared<Ordered>();
  s->stack = StackLayout::New(StackOrientation::HORIZONTAL);
  Fixed(s->stack, 200, 60);
  s->a = Leaf(20, 10, "a");
  s->b = Leaf(30, 10, "b");
  s->c = Leaf(40, 10, "c");
  s->stack.Add(s->a);
  s->stack.Add(s->b);
  s->stack.Add(s->c);
  s->sibling = Leaf(10, 10, "sibling");
  s->sibling.SetRequestedY(70);
  s->stage = r.Stage(200, 90);
  s->stage.Add(s->stack);
  s->stage.Add(s->sibling);
  r.SetState(s);
  return s;
}
} // namespace

class Tc125 : public Case
{
public:
  Tc125()
  : Case("TC125", "Tree and order", "Check Raise/Lower effects under LayoutOrderPolicy")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    for(bool preserve : {false, true})
      out.push_back(Steps(preserve ? "ORDER.preserve" : "ORDER.update", "Visual order versus layout order policy, rendered on screen", {
        {"mount", 8, [](Run& r) {
           auto s = MountThree(r);
           r.AfterRender(s->Fence(), [=](Run& r) {
             r.Rendered("initial-a", s->a, LayoutRect(0, 0, 20, 10));
             r.Rendered("initial-b", s->b, LayoutRect(20, 0, 30, 10));
           });
         }},
        {"raise-a-above-c", 10, [preserve](Run& r) {
           auto s = r.State<Ordered>();
           s->a.RaiseAbove(s->c, preserve ? LayoutOrderPolicy::PRESERVE : LayoutOrderPolicy::UPDATE);
           s->Nudge();
           r.AfterRender(s->Fence(), [=](Run& r) {
             Identity(r, "first", s->stack.GetChildViewAt(0), preserve ? s->a : s->b);
             Identity(r, "last", s->stack.GetChildViewAt(2), preserve ? s->c : s->a);
             r.Rendered("a", s->a, LayoutRect(preserve ? 0 : 70, 0, 20, 10));
             r.Rendered("b", s->b, LayoutRect(preserve ? 20 : 0, 0, 30, 10));
           });
         }},
      }));
    out.push_back(Single("ORDER.non-view", "A non-View actor is excluded from the logical layout order", 7, [](Run& r) {
      auto s   = std::make_shared<Ordered>();
      s->stack = StackLayout::New(StackOrientation::HORIZONTAL);
      s->a     = Leaf(20, 10, "a");
      s->b     = Leaf(30, 10, "b");
      s->stack.Add(s->a);
      Actor decoration = Actor::New();
      Dali::Ui::Integration::View::AddActorChild(s->stack, decoration);
      s->stack.Add(s->b);
      r.Equal("logical", s->stack.GetChildViewCount(), 2);
      r.Equal("actor", s->stack.GetChildCount(), 3);
      r.Truth("out-of-range", !s->stack.GetChildViewAt(2));
      s->stage = Mount(r, s->stack, 100, 50);
      r.AfterRender({s->stage, s->stack, s->a, s->b}, [=](Run& r) { r.Rendered("b", s->b, LayoutRect(20, 0, 30, 10)); });
    }));
    for(unsigned operation = 0; operation < 3; ++operation)
      out.push_back(Steps(operation == 0 ? "ORDER.preserve-raise" : operation == 1 ? "ORDER.preserve-lower" : "ORDER.preserve-lower-below", "PRESERVE changes the actual Actor order while retaining all logical slots", {
        {"mount", 12, [](Run& r) {
           auto s = MountThree(r);
           r.AfterRender(s->Fence(), [=](Run& r) {
             r.Rendered("initial-a", s->a, LayoutRect(0, 0, 20, 10));
             r.Rendered("initial-b", s->b, LayoutRect(20, 0, 30, 10));
             r.Rendered("initial-c", s->c, LayoutRect(50, 0, 40, 10));
           });
         }},
        {"reorder", 20, [operation](Run& r) {
           auto s = r.State<Ordered>();
           if(operation == 0)
             s->a.Raise(LayoutOrderPolicy::PRESERVE);
           else if(operation == 1)
             s->c.Lower(LayoutOrderPolicy::PRESERVE);
           else
             s->c.LowerBelow(s->a, LayoutOrderPolicy::PRESERVE);
           s->Nudge();
           r.AfterRender(s->Fence(), [=](Run& r) {
             const View logical[]   = {s->a, s->b, s->c};
             const View visual[][3] = {{s->b, s->a, s->c}, {s->a, s->c, s->b}, {s->c, s->a, s->b}};
             r.Equal("logical.count", s->stack.GetChildViewCount(), 3);
             r.Equal("actor.count", s->stack.GetChildCount(), 3);
             for(unsigned i = 0; i < 3; ++i)
             {
               Identity(r, Id("logical.order", i).c_str(), s->stack.GetChildViewAt(i), logical[i]);
               r.Truth(Id("actor.order", i).c_str(), s->stack.GetChildAt(i) == visual[operation][i]);
             }
             r.Rendered("preserved.a", s->a, LayoutRect(0, 0, 20, 10));
             r.Rendered("preserved.b", s->b, LayoutRect(20, 0, 30, 10));
             r.Rendered("preserved.c", s->c, LayoutRect(50, 0, 40, 10));
           });
         }},
      }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc125)
