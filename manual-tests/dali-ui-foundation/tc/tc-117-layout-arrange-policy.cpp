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
struct Policy
{
  std::shared_ptr<Probe> a, b;
  View                   stage, sibling, root;
  uint32_t               arrangesA{0}, arrangesB{0};
  float                  nudge{10};
  void                   Nudge()
  {
    sibling.SetRequestedWidth(nudge += 1);
  }
  std::vector<View> Fence() const
  {
    std::vector<View> views{stage, a->view, sibling};
    if(root) views.push_back(root);
    if(b) views.push_back(b->view);
    return views;
  }
};
std::shared_ptr<Policy> MountProbe(Run& r, bool arrange)
{
  auto s     = std::make_shared<Policy>();
  s->a       = Probed(r, 37, 19, arrange);
  s->sibling = Leaf(10, 10, "sibling");
  s->sibling.SetRequestedY(60);
  s->stage = r.Stage(100, 100);
  s->stage.Add(s->a->view);
  s->stage.Add(s->sibling);
  r.SetState(s);
  return s;
}
} // namespace

class Tc117 : public Case
{
public:
  Tc117()
  : Case("TC117", "Arrange policy", "Check ArrangePolicy ALWAYS/IF_CHANGED call counts")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    for(bool always : {false, true})
      out.push_back(Steps(always ? "K15.always" : "K15.if-changed", "Explicit callback policy across three controller passes", {
        {"mount", 4, [always](Run& r) {
           auto s = MountProbe(r, false);
           s->a->view.SetArrangeCallback(ArrangeCallback::New(s->a.get(), &Probe::Arrange), always ? ArrangePolicy::ALWAYS : ArrangePolicy::IF_CHANGED);
           r.AfterRender(s->Fence(), [=](Run& r) { r.Rendered("bounds", s->a->view, LayoutRect(0, 0, 37, 19)); });
         }},
        {"second-pass", 1, [always](Run& r) {
           auto s = r.State<Policy>();
           s->Nudge();
           r.AfterRender(s->Fence(), [=](Run& r) { r.Equal("second-producer", s->a->arranges, always ? 2 : 1); });
         }},
        {"third-pass", 5, [always](Run& r) {
           auto s = r.State<Policy>();
           s->Nudge();
           r.AfterRender(s->Fence(), [=](Run& r) {
             r.Equal("producer", s->a->arranges, always ? 3 : 1);
             r.Rendered("bounds-after-passes", s->a->view, LayoutRect(0, 0, 37, 19));
           });
         }},
      }));
    out.push_back(Steps("K16.always-descendant", "An ALWAYS descendant defeats the ancestor replay shortcut", {
      {"mount", 8, [](Run& r) {
         auto s  = std::make_shared<Policy>();
         s->root = StackLayout::New(StackOrientation::HORIZONTAL);
         Fixed(s->root, 100, 40);
         s->a = Probed(r, 20, 10);
         s->b = Probed(r, 30, 10);
         s->a->view.SetArrangeCallback(ArrangeCallback::New(s->a.get(), &Probe::Arrange), ArrangePolicy::ALWAYS);
         s->root.Add(s->a->view);
         s->root.Add(s->b->view);
         s->sibling = Leaf(10, 10, "sibling");
         s->sibling.SetRequestedY(60);
         s->stage = r.Stage(100, 100);
         s->stage.Add(s->root);
         s->stage.Add(s->sibling);
         r.SetState(s);
         r.AfterRender(s->Fence(), [=](Run& r) {
           s->arrangesA = s->a->arranges;
           s->arrangesB = s->b->arranges;
           r.Rendered("initial-a", s->a->view, LayoutRect(0, 0, 20, 10));
           r.Rendered("initial-b", s->b->view, LayoutRect(20, 0, 30, 10));
         });
       }},
      {"extra-pass", 10, [](Run& r) {
         auto s = r.State<Policy>();
         s->Nudge();
         r.AfterRender(s->Fence(), [=](Run& r) {
           r.Equal("always-runs", s->a->arranges, s->arrangesA + 1);
           r.Equal("clean-sibling-hit", s->b->arranges, s->arrangesB);
           r.Rendered("a", s->a->view, LayoutRect(0, 0, 20, 10));
           r.Rendered("b", s->b->view, LayoutRect(20, 0, 30, 10));
         });
       }},
    }));
    out.push_back(Steps("K16.policy-change", "Replacing the callback changes the policy and invalidates", {
      {"mount", 1, [](Run& r) {
         auto s = MountProbe(r, true);
         r.AfterRender(s->Fence(), [=](Run& r) { r.Equal("initial-producer", s->a->arranges, 1); });
       }},
      {"set-always", 1, [](Run& r) {
         auto s = r.State<Policy>();
         s->a->view.SetArrangeCallback(ArrangeCallback::New(s->a.get(), &Probe::Arrange), ArrangePolicy::ALWAYS);
         r.AfterRender(s->Fence(), [=](Run& r) { r.Equal("always-after-replace", s->a->arranges, 2); });
       }},
      {"always-pass", 1, [](Run& r) {
         auto s = r.State<Policy>();
         s->Nudge();
         r.AfterRender(s->Fence(), [=](Run& r) { r.Equal("always-total", s->a->arranges, 3); });
       }},
      {"set-if-changed", 1, [](Run& r) {
         auto s = r.State<Policy>();
         s->a->view.SetArrangeCallback(ArrangeCallback::New(s->a.get(), &Probe::Arrange));
         r.AfterRender(s->Fence(), [=](Run& r) { r.Equal("if-changed-after-replace", s->a->arranges, 4); });
       }},
      {"if-changed-pass", 1, [](Run& r) {
         auto s = r.State<Policy>();
         s->Nudge();
         r.AfterRender(s->Fence(), [=](Run& r) { r.Equal("if-changed-total", s->a->arranges, 4); });
       }},
    }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc117)
