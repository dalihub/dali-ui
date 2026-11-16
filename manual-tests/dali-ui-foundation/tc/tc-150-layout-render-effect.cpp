/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include <dali-ui-foundation/public-api/render-effects/gaussian-blur-effect.h>
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/public-api/rendering/frame-buffer.h>
#include "layout-validation-fixtures.h"
struct EffectState
{
  Dali::Window                 window;
  Dali::Ui::View               owner;
  Dali::Ui::GaussianBlurEffect effect;
  uint32_t                     Sources(Dali::Texture* texture = nullptr)
  {
    uint32_t count = 0;
    auto     list  = Dali::Integration::SceneHolder::DownCast(window).GetRenderTaskList();
    for(uint32_t i = 0; i < list.GetTaskCount(); ++i)
    {
      auto task = list.GetTask(i);
      if(task.GetSourceActor() == owner && task.GetFrameBuffer())
      {
        ++count;
        if(texture) *texture = task.GetFrameBuffer().GetColorTexture();
      }
    }
    return count;
  }
  void Check(LayoutValidation::Run& run, uint32_t width, uint32_t height)
  {
    Dali::Texture texture;
    run.Truth("effect-active", effect.IsActivated());
    run.Equal("one-source-task", Sources(&texture), 1);
    run.Equal("source-texture-width", texture ? texture.GetWidth() : 0, width);
    run.Equal("source-texture-height", texture ? texture.GetHeight() : 0, height);
  }
};

using namespace LayoutValidation;
using namespace LayoutValidation::Fixtures;

class Tc150 : public Case
{
public:
  Tc150()
  : Case("TC150", "Render effect layout", "Check blur effect texture follows view size")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    Scenario              scenario;
    scenario.id          = "EFFECT.target-resize";
    scenario.description = "Effect source texture follows arranged owner extent";
    scenario.steps.push_back({"mount", 8, [](Run& r)
    {auto s=std::make_shared<EffectState>();s->window=Window::New(PositionSize(80,80,240,180),"TC150 effect fixture");s->owner=Leaf(120,80);s->owner.SetBackgroundColor(UiColor(0x3399CCu));s->effect=GaussianBlurEffect::New(8);s->effect.SetBlurDownscaleFactor(1);/* blurOnce stays OFF: with one-shot blur, OnRenderFinished removes the blur tasks and clears the source task's framebuffer as soon as the single render completes, so every texture observation below would race that release (and lose it outright on a fast GPU). REFRESH_ALWAYS keeps the source framebuffer live, which is what these steps read. */ s->effect.SetBlurOnce(false);s->owner.SetRenderEffect(s->effect);r.SetState(s);r.OnCleanup([s](){s->owner.ClearRenderEffect();if(s->window){s->window.Remove(s->owner);LayoutController::Remove(s->window);s->window.Hide();s->window.Reset();}});r.AfterLayout(s->window,{s->owner},[s](Run&done){done.Rect("owner",done.Snapshot(s->owner),LayoutRect(0,0,120,80));s->Check(done,120,80);});s->window.Add(s->owner); }});
    scenario.steps.push_back({"resize", 8, [](Run& r)
    {auto s=r.State<EffectState>();r.AfterLayout(s->window,{s->owner},[s](Run&done){done.Rect("resized-owner",done.Snapshot(s->owner),LayoutRect(0,0,160,100));s->Check(done,160,100);});Fixed(s->owner,160,100); }});
    scenario.steps.push_back({"zero-size", 6, [](Run& r)
    {auto s=r.State<EffectState>();r.AfterLayout(s->window,{s->owner},[s](Run&done){done.Rect("zero-owner",done.Snapshot(s->owner),LayoutRect(0,0,0,0));done.Truth("effect-inactive",!s->effect.IsActivated());done.Equal("source-tasks-removed",s->Sources(),0);});Fixed(s->owner,0,0); }});
    scenario.steps.push_back({"restore-odd-size", 8, [](Run& r)
    {auto s=r.State<EffectState>();r.AfterLayout(s->window,{s->owner},[s](Run&done){done.Rect("restored-owner",done.Snapshot(s->owner),LayoutRect(0,0,73,41));s->Check(done,73,41);});Fixed(s->owner,73,41); }});
    scenario.steps.push_back({"downscale-half", 4, [](Run& r)
    {auto s=r.State<EffectState>();s->effect.SetBlurDownscaleFactor(0.5f);r.Delay(100,[s](Run&done){s->Check(done,36,20);}); }});
    scenario.steps.push_back({"clear-effect", 3, [](Run& r)
    {auto s=r.State<EffectState>();s->owner.ClearRenderEffect();r.Truth("owner-effect-empty",!s->owner.GetRenderEffect());r.Truth("effect-deactivated",!s->effect.IsActivated());r.Equal("source-tasks-empty",s->Sources(),0); }});
    out.push_back(std::move(scenario));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc150)
