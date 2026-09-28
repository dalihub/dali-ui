/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include <dali/devel-api/events/touch-point.h>
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include "layout-validation-fixtures.h"
struct HitState
{
  Dali::Window             window;
  Dali::Ui::AbsoluteLayout root;
  Dali::Ui::View           child;
  Dali::Actor              lastHit;
  Dali::Vector2            local{};
  uint32_t                 downs{0};
  int                      timestamp{1000};
  void                     Feed(float x, float y)
  {
    auto             holder = Dali::Integration::SceneHolder::DownCast(window);
    Dali::TouchPoint down(56, Dali::PointState::DOWN, x, y), up(56, Dali::PointState::UP, x, y);
    holder.FeedTouchPoint(down, timestamp);
    holder.FeedTouchPoint(up, timestamp + 16);
    timestamp += 100;
  }
};

using namespace LayoutValidation;
using namespace LayoutValidation::Fixtures;

class Tc149 : public Case
{
public:
  Tc149()
  : Case("TC149", "Hit and clip", "Check touch hits respect layout clipping")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    Scenario              scenario;
    scenario.id          = "I09.hit-clipped-layout";
    scenario.description = "Actual touch dispatch follows arranged bounds and clipping";
    scenario.steps.push_back({"mount", 8, [](Run& r)
    {auto s=std::make_shared<HitState>();s->window=Window::New(PositionSize(60,60,240,160),"TC149 hit fixture");s->root=AbsoluteLayout::New();Fixed(s->root,100,80);s->root.SetRequestedX(20);s->root.SetRequestedY(20);s->root.SetClippingMode(ClippingMode::CLIP_TO_BOUNDING_BOX);s->root.SetBackgroundColor(UiColor(0xDDDDDDu));s->child=Leaf(60,40);s->child.SetBackgroundColor(UiColor(0x3377CCu));s->child.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(80,20,60,40)));s->root.Add(s->child);s->child.TouchEventSignal().Connect(&r,[s](Actor,TouchEvent event){if(event.GetState(0)==PointState::DOWN){++s->downs;s->lastHit=event.GetHitActor(0);s->local=event.GetLocalPosition(0);}return true;});r.SetState(s);r.OnCleanup([s](){if(s->window){s->window.Remove(s->root);LayoutController::Remove(s->window);s->window.Hide();s->window.Reset();}});r.AfterLayout(s->window,{s->root,s->child},[s](Run&done){done.Rect("clip-root",done.Snapshot(s->root),LayoutRect(20,20,100,80));done.Rect("overflow-child",done.Snapshot(s->child),LayoutRect(80,20,60,40));});s->window.Add(s->root); }});
    scenario.steps.push_back({"inside-hit", 4, [](Run& r)
    {auto s=r.State<HitState>();uint32_t before=s->downs;s->Feed(110,50);r.Delay(100,[s,before](Run&done){done.Equal("one-down",s->downs,before+1);done.Truth("actual-hit-child",s->lastHit==s->child);done.Near("local-x",s->local.x,10,0.05);done.Near("local-y",s->local.y,10,0.05);}); }});
    scenario.steps.push_back({"outside-clipped", 1, [](Run& r)
    {auto s=r.State<HitState>();uint32_t before=s->downs;s->Feed(140,50);r.Delay(100,[s,before](Run&done){done.Equal("clipped-no-down",s->downs,before);}); }});
    scenario.steps.push_back({"unclipped-hit", 4, [](Run& r)
    {auto s=r.State<HitState>();s->root.SetClippingMode(ClippingMode::DISABLED);r.Delay(100,[s](Run&ready){uint32_t before=s->downs;s->Feed(140,50);ready.Delay(100,[s,before](Run&done){done.Equal("unclipped-one-down",s->downs,before+1);done.Truth("unclipped-hit-child",s->lastHit==s->child);done.Near("unclipped-local-x",s->local.x,40,0.05);done.Near("unclipped-local-y",s->local.y,10,0.05);});}); }});
    scenario.steps.push_back({"enlarge-clip", 8, [](Run& r)
    {auto s=r.State<HitState>();r.AfterLayout(s->window,{s->root,s->child},[s](Run&done){done.Rect("enlarged-root",done.Snapshot(s->root),LayoutRect(20,20,140,80));done.Rect("retained-child",done.Snapshot(s->child),LayoutRect(80,20,60,40));});s->root.SetClippingMode(ClippingMode::CLIP_TO_BOUNDING_BOX);s->root.SetRequestedWidth(140); }});
    scenario.steps.push_back({"newly-visible-hit", 4, [](Run& r)
    {auto s=r.State<HitState>();uint32_t before=s->downs;s->Feed(140,50);r.Delay(100,[s,before](Run&done){done.Equal("resized-one-down",s->downs,before+1);done.Truth("resized-hit-child",s->lastHit==s->child);done.Near("resized-local-x",s->local.x,40,0.05);done.Near("resized-local-y",s->local.y,10,0.05);}); }});
    out.push_back(std::move(scenario));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc149)
