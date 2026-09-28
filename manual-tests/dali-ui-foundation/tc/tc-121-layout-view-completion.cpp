/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include "layout-validation-fixtures.h"
struct CompletionState
{
  Dali::Ui::StackLayout root;
  Dali::Ui::View        a, b;
  uint32_t              aSignals{0}, bSignals{0};
  Dali::Ui::LayoutRect  frozenA{}, frozenB{};
};

using namespace LayoutValidation;
using namespace LayoutValidation::Fixtures;

class Tc121 : public Case
{
public:
  Tc121()
  : Case("TC121", "View completion", "Check View LayoutFinishedSignal and its snapshots")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    Scenario              sequence;
    sequence.id          = "L03.fenced-snapshots";
    sequence.description = "View snapshots precede the Window fence";
    sequence.steps.push_back({"mount", 10, [](Run& r)
    {auto state=std::make_shared<CompletionState>();state->root=StackLayout::New(StackOrientation::HORIZONTAL);Fixed(state->root,200,60);state->a=Leaf(20,10);state->b=Leaf(30,15);state->root.Add(state->a);state->root.Add(state->b);r.SetState(state);state->a.LayoutFinishedSignal().Connect(&r,[state](View,const LayoutRect&){++state->aSignals;});state->b.LayoutFinishedSignal().Connect(&r,[state](View,const LayoutRect&){++state->bSignals;});r.AfterLayout({state->a,state->b},[state](Run&done){state->frozenA=done.Snapshot(state->a);state->frozenB=done.Snapshot(state->b);done.Rect("a",state->frozenA,LayoutRect(0,0,20,10));done.Rect("b",state->frozenB,LayoutRect(20,0,30,15));done.Truth("a-notified",state->aSignals>0);done.Truth("b-notified",state->bSignals>0);});r.Attach(state->root); }});
    sequence.steps.push_back({"resize-child", 8, [](Run& r)
    {auto s=r.State<CompletionState>();r.AfterLayout({s->a,s->b},[s](Run&done){done.Rect("changed",done.Snapshot(s->a),LayoutRect(0,0,40,10));done.Rect("shifted",done.Snapshot(s->b),LayoutRect(40,0,30,15));});s->a.SetRequestedWidth(40); }});
    sequence.steps.push_back({"no-op-setter", 2, [](Run& r)
    {auto s=r.State<CompletionState>();uint32_t a=s->aSignals,b=s->bSignals;s->a.SetRequestedWidth(40);r.Equal("a-no-synchronous-signal",s->aSignals,a);r.Equal("b-no-synchronous-signal",s->bSignals,b); }});
    sequence.steps.push_back({"direct-arrange", 5, [](Run& r)
    {auto s=r.State<CompletionState>();uint32_t signals=s->aSignals;s->a.Arrange(LayoutRect(7,9,40,10));r.Equal("no-manual-completion",s->aSignals,signals);r.Rect("manual-target",s->a,LayoutRect(7,9,40,10)); }});
    out.push_back(std::move(sequence));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc121)
