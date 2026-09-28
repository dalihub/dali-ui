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
struct Scaled
{
  View stage, view, sibling;
  float nudge{10};
};
struct ScaledProbe
{
  std::shared_ptr<Probe> probe;
  View                   stage, sibling;
  uint32_t               measures{0};
  float                  nudge{10};
  Property::Index        scaleIndex{Property::INVALID_INDEX};
};
// Stages for scale scenarios opt out of scaling so their pixel size stays the declared size.
View UnscaledStage(Run& r, float width, float height)
{
  View stage = r.Stage(width, height);
  Dali::Ui::Extension::View::SetUiScalePolicy(stage, Dali::Ui::Extension::UiScalePolicy::DISABLED);
  return stage;
}
// The effective layout scale of `view` as the layout computes it (extension API).
float EffectiveScale(View view)
{
  return Dali::Ui::Extension::View::GetEffectiveLayoutScale(Dali::Ui::GetImpl(view));
}
// The framework-owned effective-scale property of every View, looked up by its registered name.
Property::Index ScaleProperty(View view)
{
  return view.GetPropertyIndex("viewEffectiveScale");
}
} // namespace

class Tc102 : public Case
{
public:
  Tc102()
  : Case("TC102", "Scale inheritance", "Check UI scale inheritance and scaled clamping")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    for(float scale : {1.0f, 1.25f, 1.5f, 2.0f})
      out.push_back(Single(("C13.scale." + FloatId(scale)).c_str(), "Inherited enabled and disabled scale policies render at their own scale", 14, [scale](Run& r) {
        auto manager = Dali::Ui::Extension::LayoutScaleManager::Get();
        manager.SetScalable(true);
        manager.SetUiScale(scale);
        View p = Leaf(200, 100, "parent"), a = Leaf(20, 10, "inherit"), b = Leaf(30, 15, "enabled");
        Dali::Ui::Extension::View::SetUiScalePolicy(p, Dali::Ui::Extension::UiScalePolicy::DISABLED);
        Dali::Ui::Extension::View::SetUiScalePolicy(a, Dali::Ui::Extension::UiScalePolicy::INHERIT);
        Dali::Ui::Extension::View::SetUiScalePolicy(b, Dali::Ui::Extension::UiScalePolicy::ENABLED);
        p.Add(a);
        p.Add(b);
        View stage = UnscaledStage(r, 400, 200);
        stage.Add(p);
        r.AfterRender({stage, p, a, b}, [=](Run& r) {
          r.Size("parent-disabled", p.GetMeasuredSize(), MeasuredSize(200, 100));
          r.Size("child-inherit", a.GetMeasuredSize(), MeasuredSize(20, 10));
          r.Size("child-enabled", b.GetMeasuredSize(), MeasuredSize(30 * scale, 15 * scale));
          r.Rendered("child-inherit.rendered", a, LayoutRect(0, 0, 20, 10));
          r.Rendered("child-enabled.rendered", b, LayoutRect(0, 0, 30 * scale, 15 * scale));
        });
      }));
    out.push_back(Steps("C14.K12.master-switch", "Stored scale survives disable and re-enable on screen", {
      {"enabled", 6, [](Run& r) {
         auto m = Dali::Ui::Extension::LayoutScaleManager::Get();
         m.SetScalable(true);
         m.SetUiScale(2);
         auto s  = std::make_shared<Scaled>();
         s->view = Leaf(37, 19);
         Dali::Ui::Extension::View::SetUiScalePolicy(s->view, Dali::Ui::Extension::UiScalePolicy::ENABLED);
         s->stage = UnscaledStage(r, 200, 100);
         s->stage.Add(s->view);
         r.SetState(s);
         r.AfterRender({s->stage, s->view}, [=](Run& r) {
           r.Size("enabled", s->view.GetMeasuredSize(), MeasuredSize(74, 38));
           r.Rendered("enabled.rendered", s->view, LayoutRect(0, 0, 74, 38));
         });
       }},
      {"disabled", 6, [](Run& r) {
         auto s = r.State<Scaled>();
         Dali::Ui::Extension::LayoutScaleManager::Get().SetScalable(false);
         s->view.InvalidateMeasure();
         r.AfterRender({s->stage, s->view}, [=](Run& r) {
           r.Size("disabled", s->view.GetMeasuredSize(), MeasuredSize(37, 19));
           r.Rendered("disabled.rendered", s->view, LayoutRect(0, 0, 37, 19));
         });
       }},
      {"restored", 6, [](Run& r) {
         auto s = r.State<Scaled>();
         Dali::Ui::Extension::LayoutScaleManager::Get().SetScalable(true);
         s->view.InvalidateMeasure();
         r.AfterRender({s->stage, s->view}, [=](Run& r) {
           r.Size("restored", s->view.GetMeasuredSize(), MeasuredSize(74, 38));
           r.Rendered("restored.rendered", s->view, LayoutRect(0, 0, 74, 38));
         });
       }},
    }));
    out.push_back(Single("R03.scale-finite", "Scale rejects nonfinite values without propagating them", 4, [](Run& r) {
      auto m = Dali::Ui::Extension::LayoutScaleManager::Get();
      m.SetScalable(false);
      m.SetUiScale(1);
      m.SetUiScale(std::numeric_limits<float>::quiet_NaN());
      r.Near("nan-rejected", m.GetUiScale(), 1, 0);
      m.SetUiScale(-1);
      r.Near("negative-rejected", m.GetUiScale(), 1, 0);
      m.SetUiScale(std::numeric_limits<float>::infinity());
      float observed = m.GetUiScale();
      m.SetUiScale(1);
      r.Near("infinity-rejected", observed, 1, 0);
      // A subnormal is finite and positive but unusable: the layout divides by the effective
      // scale, and 1 / denorm_min overflows to infinity.
      m.SetUiScale(std::numeric_limits<float>::denorm_min());
      r.Near("subnormal-rejected", m.GetUiScale(), 1, 0);
      m.SetUiScale(1);
    }));
    out.push_back(Steps("C20.scale-exact-key", "Sub-epsilon scale changes invalidate a registered root and keep an exact cache key", {
      {"mount", 4, [](Run& r) {
         auto manager = Dali::Ui::Extension::LayoutScaleManager::Get();
         manager.SetScalable(true);
         manager.SetUiScale(1);
         auto s     = std::make_shared<ScaledProbe>();
         s->probe   = Probed(r);
         s->sibling = Leaf(10, 10, "sibling");
         s->sibling.SetRequestedY(60);
         Dali::Ui::Extension::View::SetUiScalePolicy(s->probe->view, Dali::Ui::Extension::UiScalePolicy::ENABLED);
         s->stage = UnscaledStage(r, 128, 100);
         s->stage.Add(s->probe->view);
         s->stage.Add(s->sibling);
         r.SetState(s);
         r.AfterRender({s->stage, s->probe->view, s->sibling}, [=](Run& r) {
           s->measures = s->probe->measures;
           r.Rendered("probe", s->probe->view, LayoutRect(0, 0, 37, 19));
         });
       }},
      {"sub-epsilon-scale", 2, [](Run& r) {
         auto s = r.State<ScaledProbe>();
         Dali::Ui::Extension::LayoutScaleManager::Get().SetUiScale(1.0005f);
         r.AfterRender({s->stage, s->probe->view, s->sibling}, [=](Run& r) {
           r.Equal("producer-recomputed", s->probe->measures, s->measures + 1);
           r.Near("exact-effective", EffectiveScale(s->probe->view), 1.0005f, 0);
           s->measures = s->probe->measures;
         });
       }},
      {"same-scale", 1, [](Run& r) {
         auto s = r.State<ScaledProbe>();
         s->sibling.SetRequestedWidth(s->nudge += 1);
         r.AfterRender({s->stage, s->probe->view, s->sibling}, [=](Run& r) { r.Equal("same-scale-hit", s->probe->measures, s->measures); });
       }},
    }));
    out.push_back(Steps("C13.actor-scale-repair", "A cached measure repairs the framework-owned scale property", {
      {"mount", 5, [](Run& r) {
         auto s     = std::make_shared<ScaledProbe>();
         s->probe   = Probed(r);
         s->sibling = Leaf(10, 10, "sibling");
         s->sibling.SetRequestedY(60);
         s->stage = UnscaledStage(r, 100, 100);
         s->stage.Add(s->probe->view);
         s->stage.Add(s->sibling);
         r.SetState(s);
         r.AfterRender({s->stage, s->probe->view, s->sibling}, [=](Run& r) {
           s->scaleIndex = ScaleProperty(s->probe->view);
           r.Truth("property-index", s->scaleIndex != Property::INVALID_INDEX);
           r.Rendered("probe", s->probe->view, LayoutRect(0, 0, 37, 19));
           s->measures = s->probe->measures;
         });
       }},
      {"corrupt-and-repair", 3, [](Run& r) {
         auto  s   = r.State<ScaledProbe>();
         Actor raw = s->probe->view;
         raw.SetProperty(s->scaleIndex, 9.0f);
         r.Near("corrupted", raw.GetProperty<float>(s->scaleIndex), 9, 0);
         s->sibling.SetRequestedWidth(s->nudge += 1);
         r.AfterRender({s->stage, s->probe->view, s->sibling}, [=](Run& r) {
           // The repair pushes the effective scale back into the property on the cached measure path.
           r.Near("actor-scale", s->probe->view.GetProperty<float>(s->scaleIndex), 1, 0);
           r.Equal("producer-hit", s->probe->measures, s->measures);
         });
       }},
    }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc102)
