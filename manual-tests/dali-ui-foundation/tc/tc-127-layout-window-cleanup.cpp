/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include "layout-validation-fixtures.h"
struct WindowState
{
  Dali::Window          window;
  Dali::Ui::StackLayout root;
  Dali::Ui::View        child;
};

using namespace LayoutValidation;
using namespace LayoutValidation::Fixtures;

class Tc127 : public Case
{
public:
  Tc127()
  : Case("TC127", "Window cleanup", "Check controller re-create and Window close")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    Scenario              scenario;
    scenario.id          = "L04.window-recreate";
    scenario.description = "Secondary Window controller teardown and recreation";
    scenario.steps.push_back({"secondary-window", 5, [](Run& r)
    {auto s=std::make_shared<WindowState>();s->window=Window::New(PositionSize(40,40,220,140),"TC127 isolated window");s->root=StackLayout::New(StackOrientation::HORIZONTAL);Fixed(s->root,100,60);s->child=Leaf(37,19);s->root.Add(s->child);ColorizeTree(s->root);r.SetState(s);r.OnCleanup([s](){if(s->window){s->window.Remove(s->root);LayoutController::Remove(s->window);s->window.Hide();s->window.Reset();}});r.AfterLayout(s->window,{s->child},[s](Run&done){done.Truth("secondary-distinct",s->window!=done.GetWindow());done.Rect("secondary-child",done.Snapshot(s->child),LayoutRect(0,0,37,19));});s->window.Add(s->root); }});
    scenario.steps.push_back({"remove-and-recreate-controller", 5, [](Run& r)
    {auto s=r.State<WindowState>();s->window.Remove(s->root);LayoutController::Remove(s->window);LayoutController::Get(s->window);r.AfterLayout(s->window,{s->child},[s](Run&done){done.Rect("recreated-child",done.Snapshot(s->child),LayoutRect(0,0,53,19));done.Truth("main-window-alive",static_cast<bool>(done.GetWindow()));});s->child.SetRequestedWidth(53);s->window.Add(s->root); }});
    scenario.steps.push_back({"detach-close", 3, [](Run& r)
    {auto s=r.State<WindowState>();s->window.Remove(s->root);r.Truth("root-unparented",!s->root.GetParent());LayoutController::Remove(s->window);s->window.Hide();s->window.Reset();r.Truth("window-handle-released",!s->window);r.Truth("main-window-retained",static_cast<bool>(r.GetWindow())); }});
    out.push_back(std::move(scenario));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc127)
