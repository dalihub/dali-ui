/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include "layout-validation-fixtures.h"
#include <stdexcept>

using namespace LayoutValidation;
using namespace LayoutValidation::Fixtures;

namespace
{
struct Faulty
{
  std::shared_ptr<Probe> probe;
  View                   stage, sibling;
  uint32_t               measures{0}, arranges{0};
  float                  nudge{10};
  void                   Nudge()
  {
    sibling.SetRequestedWidth(nudge += 1);
  }
  std::vector<View> Fence() const
  {
    return {stage, probe->view, sibling};
  }
};
std::shared_ptr<Faulty> MountProbe(Run& r)
{
  auto s     = std::make_shared<Faulty>();
  s->probe   = Probed(r);
  s->sibling = Leaf(10, 10, "sibling");
  s->sibling.SetRequestedY(60);
  s->stage = r.Stage(100, 100);
  s->stage.Add(s->probe->view);
  s->stage.Add(s->sibling);
  r.SetState(s);
  return s;
}
// Runs one synchronous controller pass and reports whether a std::runtime_error escaped it.
bool ProcessCatchingRuntimeError(Run& r)
{
  try
  {
    LayoutController::Get(r.GetWindow()).ProcessLayouts();
  }
  catch(const std::runtime_error&)
  {
    return true;
  }
  return false;
}
} // namespace

class Tc123 : public Case
{
public:
  Tc123()
  : Case("TC123", "Reentry and exceptions", "Check recovery from producer throw and re-entry")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    out.push_back(Steps("K18.measure-throw", "A throwing measure producer rolls back publication and permits a retry on screen", {
      {"mount", 4, [](Run& r) {
         auto s = MountProbe(r);
         r.AfterRender(s->Fence(), [=](Run& r) { r.Rendered("initial", s->probe->view, LayoutRect(0, 0, 37, 19)); });
       }},
      {"throw", 7, [](Run& r) {
         auto s              = r.State<Faulty>();
         s->probe->onMeasure = []() { throw std::runtime_error("TC123 expected producer exception"); };
         s->probe->view.InvalidateMeasure();
         r.Truth("exception-propagated", ProcessCatchingRuntimeError(r));
         r.Size("last-completed", s->probe->view.GetMeasuredSize(), MeasuredSize(37, 19));
         r.Rendered("last-rendered", s->probe->view, LayoutRect(0, 0, 37, 19), 0.001);
         // The rolled-back root stays pending; the producer must be sane before any later
         // pass (a HUD refresh is enough to wake one) re-drives it outside this try/catch.
         s->probe->onMeasure = {};
       }},
      {"retry", 6, [](Run& r) {
         auto s              = r.State<Faulty>();
         s->probe->onMeasure = {};
         s->probe->width     = 53;
         s->probe->view.InvalidateMeasure();
         r.AfterRender(s->Fence(), [=](Run& r) {
           r.Size("retry", s->probe->view.GetMeasuredSize(), MeasuredSize(53, 19));
           r.Rendered("retry.rendered", s->probe->view, LayoutRect(0, 0, 53, 19));
         });
       }},
    }));
    out.push_back(Steps("K18.arrange-throw", "A failed arrange publication preserves the completed record and permits a cacheable retry", {
      {"mount", 4, [](Run& r) {
         auto s = MountProbe(r);
         s->probe->view.SetMargin(Insets(1, 0, 2, 0));
         r.AfterRender(s->Fence(), [=](Run& r) {
           s->arranges = s->probe->arranges;
           r.Rendered("initial", s->probe->view, LayoutRect(1, 2, 37, 19));
         });
       }},
      {"throw", 9, [](Run& r) {
         auto s              = r.State<Faulty>();
         s->probe->onArrange = []() { throw std::runtime_error("TC123 expected arrange exception"); };
         s->probe->width     = 53;
         s->probe->height    = 29;
         s->probe->view.SetMargin(Insets(7, 0, 9, 0));
         s->probe->view.InvalidateMeasure();
         r.Truth("exception-propagated", ProcessCatchingRuntimeError(r));
         // The provisional actor geometry is visible before the producer runs; the
         // published arrangement record is the separate completed rect.
         r.Rect("provisional-input", LayoutValidation::Bounds(s->probe->view), LayoutRect(7, 9, 53, 29));
         r.Rect("last-completed", GetImpl(s->probe->view).GetArrangedBounds(), LayoutRect(1, 2, 37, 19));
         s->probe->onArrange = {};
       }},
      {"retry", 5, [](Run& r) {
         auto s      = r.State<Faulty>();
         s->arranges = s->probe->arranges; // producer calls so far, whatever passes ran in between
         s->probe->view.InvalidateArrange();
         r.AfterRender(s->Fence(), [=](Run& r) {
           r.Rendered("retry", s->probe->view, LayoutRect(7, 9, 53, 29));
           r.Equal("retry-producer", s->probe->arranges, s->arranges + 1);
         });
       }},
      {"cache", 5, [](Run& r) {
         auto s      = r.State<Faulty>();
         s->arranges = s->probe->arranges;
         s->Nudge();
         r.AfterRender(s->Fence(), [=](Run& r) {
           r.Rendered("retry-cache", s->probe->view, LayoutRect(7, 9, 53, 29));
           r.Equal("retry-cache-producer", s->probe->arranges, s->arranges);
         });
       }},
    }));
    out.push_back(Steps("K18.measure-reentry", "Reentry terminates and does not publish a reusable poisoned result", {
      {"mount", 4, [](Run& r) {
         auto s = MountProbe(r);
         r.AfterRender(s->Fence(), [=](Run& r) {
           s->measures = s->probe->measures;
           r.Rendered("initial", s->probe->view, LayoutRect(0, 0, 37, 19));
         });
       }},
      {"reenter", 2, [](Run& r) {
         auto s       = r.State<Faulty>();
         auto entered = std::make_shared<bool>(false);
         auto probe   = s->probe;
         probe->onMeasure = [probe, entered]() {
           if(!*entered)
           {
             *entered = true;
             probe->view.Measure(100, 50);
           }
         };
         probe->view.InvalidateMeasure();
         try
         {
           LayoutController::Get(r.GetWindow()).ProcessLayouts();
         }
         catch(const DaliException&)
         {
         }
         probe->onMeasure = {};
         r.Truth("entered", *entered);
         r.Truth("bounded", probe->measures <= s->measures + 2);
       }},
      {"recover", 6, [](Run& r) {
         auto s = r.State<Faulty>();
         s->probe->view.InvalidateMeasure();
         r.AfterRender(s->Fence(), [=](Run& r) {
           r.Size("recovered", s->probe->view.GetMeasuredSize(), MeasuredSize(37, 19));
           r.Rendered("recovered.rendered", s->probe->view, LayoutRect(0, 0, 37, 19));
         });
       }},
    }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc123)
