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
struct ReturnedRect
{
  std::shared_ptr<Probe> probe;
  View                   stage;
};
} // namespace

class Tc96 : public Case
{
public:
  Tc96()
  : Case("TC96", "Core invalid inputs", "Check handling of invalid sizes and Arrange rects")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario>      out;
    const std::array<float, 4> values{{-3.0f, std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()}};
    for(unsigned i = 0; i < values.size(); ++i)
      out.push_back(Single(Id("C09.reject", i).c_str(), "Rejected size properties preserve prior state and the rendered 37x19 leaf", 12, [v = values[i]](Run& r) {
        View a = Leaf(37, 19);
        a.SetMinimumWidth(3);
        a.SetMaximumWidth(100);
        a.SetMinimumHeight(2);
        a.SetMaximumHeight(80);
        a.SetRequestedWidth(v);
        a.SetRequestedHeight(v);
        a.SetMinimumWidth(v);
        a.SetMaximumWidth(v);
        a.SetMinimumHeight(v);
        a.SetMaximumHeight(v);
        r.Near("requested-width", a.GetRequestedWidth(), 37, 0);
        r.Near("requested-height", a.GetRequestedHeight(), 19, 0);
        r.Near("minimum-width", a.GetMinimumWidth(), 3, 0);
        r.Near("maximum-width", a.GetMaximumWidth(), 100, 0);
        r.Near("minimum-height", a.GetMinimumHeight(), 2, 0);
        r.Near("maximum-height", a.GetMaximumHeight(), 80, 0);
        View stage = Mount(r, a, 200, 100);
        r.AfterRender({stage, a}, [=](Run& r) {
          r.Size("preserved-measure", a.GetMeasuredSize(), MeasuredSize(37, 19));
          r.Rendered("preserved-rendered", a, LayoutRect(0, 0, 37, 19));
        });
      }));
    for(float requested : {20.0f, 70.0f, 150.0f})
      out.push_back(Single(("C09.max-wins." + FloatId(requested)).c_str(), "Conflicting bounds have a documented maximum winner on screen", 6, [requested](Run& r) {
        View a = Leaf(requested, requested);
        a.SetMinimumWidth(100);
        a.SetMaximumWidth(40);
        a.SetMinimumHeight(90);
        a.SetMaximumHeight(30);
        View stage = Mount(r, a, 300, 300);
        r.AfterRender({stage, a}, [=](Run& r) {
          r.Size("maximum-wins", a.GetMeasuredSize(), MeasuredSize(40, 30));
          r.Rendered("maximum-wins.rendered", a, LayoutRect(0, 0, 40, 30));
        });
      }));
    for(float sentinel : {WRAP_CONTENT, MATCH_PARENT})
      out.push_back(Single(("C09.sentinel." + FloatId(sentinel)).c_str(), "Near sentinel is canonicalized and lays out as the sentinel", 6, [sentinel](Run& r) {
        View a = Leaf(37, 19);
        a.SetRequestedWidth(sentinel + 0.0005f);
        a.SetRequestedHeight(sentinel - 0.0005f);
        r.Near("canonical-width", a.GetRequestedWidth(), sentinel, 0);
        r.Near("canonical-height", a.GetRequestedHeight(), sentinel, 0);
        View       stage    = Mount(r, a, 200, 100);
        const bool match    = sentinel == MATCH_PARENT;
        LayoutRect expected = match ? LayoutRect(0, 0, 200, 100) : LayoutRect(0, 0, 0, 0);
        r.AfterRender({stage, a}, [=](Run& r) { r.Rendered("sentinel.rendered", a, expected); });
      }));
    out.push_back(Single("R03.negative-flex-factors", "Negative flex factors normalize to zero", 2, [](Run& r) {
      auto p = FlexLayoutParams::New().SetFlexGrow(-1).SetFlexShrink(-2);
      r.Near("grow", p.GetFlexGrow(), 0, 0);
      r.Near("shrink", p.GetFlexShrink(), 0, 0);
    }));
    for(unsigned invalid = 0; invalid < 3; ++invalid)
      out.push_back(Steps(Id("R03.returned-rect", invalid), "An invalid custom Arrange result is rejected as a whole; the valid slot stays on screen", {
        {"invalid-result", 5, [invalid](Run& r) {
           auto s      = std::make_shared<ReturnedRect>();
           s->probe    = Probed(r, 53, 29);
           auto& probe = *s->probe;
           probe.view.SetRequestedWidth(53);
           probe.view.SetRequestedHeight(29);
           probe.view.SetMargin(Insets(7, 0, 9, 0));
           probe.returned = [invalid](const LayoutRect&) {
             return invalid == 0 ? LayoutRect(0, 0, -1, 19) : invalid == 1 ? LayoutRect(std::numeric_limits<float>::quiet_NaN(), 0, 37, 19) : LayoutRect(0, 0, 37, std::numeric_limits<float>::infinity());
           };
           s->stage = Mount(r, probe.view, 200, 100);
           r.SetState(s);
           // The invalid return is exercised in ONE synchronous pass: a Debug library rejects it
           // with DALI_ASSERT_DEBUG (DaliException, caught here instead of aborting the app from
           // the controller's idle pass) and a Release library falls back to the input rect. The
           // hook is removed before any later pass can see it again.
           bool caught = false;
           try
           {
             LayoutController::Get(r.GetWindow()).ProcessLayouts();
           }
           catch(const Dali::DaliException&)
           {
             caught = true;
           }
           s->probe->returned      = {};
           const LayoutRect actual = LayoutValidation::Bounds(s->probe->view);
           r.Truth("invalid-result-contained", caught || (actual.x == 7 && actual.y == 9 && actual.width == 53 && actual.height == 29));
           s->probe->view.InvalidateArrange();
           r.AfterRender({s->stage, s->probe->view}, [=](Run& r) { r.Rendered("safe-actor", s->probe->view, LayoutRect(7, 9, 53, 29)); });
         }},
        {"recovered", 4, [](Run& r) {
           auto s            = r.State<ReturnedRect>();
           s->probe->returned = {};
           // The probe's own measured size follows the new request; only its arrange callback
           // was invalid.
           s->probe->width  = 61;
           s->probe->height = 31;
           s->probe->view.SetRequestedWidth(61);
           s->probe->view.SetRequestedHeight(31);
           s->probe->view.SetMargin(Insets(11, 0, 13, 0));
           r.AfterRender({s->stage, s->probe->view}, [=](Run& r) { r.Rendered("recovered", s->probe->view, LayoutRect(11, 13, 61, 31)); });
         }},
      }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc96)
