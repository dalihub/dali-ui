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
struct Roots
{
  View        stage, root, child;
  StackLayout left, right;
  View        a, b, c;
};
} // namespace

class Tc126 : public Case
{
public:
  Tc126()
  : Case("TC126", "Reparent and offscene", "Check layout after reparent, detach and off-scene")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    out.push_back(Steps("L01.reparent", "Reparenting invalidates the old and the new intrinsic roots on screen", {
      {"mount", 8, [](Run& r) {
         auto s   = std::make_shared<Roots>();
         s->left  = StackLayout::New(StackOrientation::HORIZONTAL);
         s->right = StackLayout::New(StackOrientation::HORIZONTAL);
         s->left.SetSpacing(7);
         s->right.SetSpacing(5);
         s->a = Leaf(20, 10, "a");
         s->b = Leaf(30, 15, "b");
         s->c = Leaf(40, 20, "c");
         s->left.Add(s->a);
         s->left.Add(s->b);
         s->right.Add(s->c);
         s->right.SetRequestedY(50);
         s->stage = r.Stage(200, 100);
         s->stage.Add(s->left);
         s->stage.Add(s->right);
         r.SetState(s);
         r.AfterRender({s->stage, s->left, s->right, s->a, s->b, s->c}, [=](Run& r) {
           r.Rendered("left", s->left, LayoutRect(0, 0, 57, 15));
           r.Rendered("right", s->right, LayoutRect(0, 50, 40, 20));
         });
       }},
      {"reparent", 12, [](Run& r) {
         auto s = r.State<Roots>();
         s->right.Add(s->b);
         r.AfterRender({s->stage, s->left, s->right, s->a, s->b, s->c}, [=](Run& r) {
           r.Size("old-root", s->left.GetMeasuredSize(), MeasuredSize(20, 10));
           r.Size("new-root", s->right.GetMeasuredSize(), MeasuredSize(75, 20));
           r.Equal("old-count", s->left.GetChildViewCount(), 1);
           r.Equal("new-count", s->right.GetChildViewCount(), 2);
           r.Rendered("moved", s->b, LayoutRect(45, 0, 30, 15));
           Identity(r, "parent", View::DownCast(s->b.GetParent()), s->right);
           Identity(r, "left-child", s->left.GetChildViewAt(0), s->a);
         });
       }},
    }));
    out.push_back(Steps("L02.detached-measure", "A detached child keeps a deterministic layout and re-attaches to the rendered root", {
      {"mount", 6, [](Run& r) {
         auto s   = std::make_shared<Roots>();
         s->root  = View::New();
         s->child = Leaf(37, 19, "child");
         s->child.SetRequestedX(7);
         s->child.SetRequestedY(9);
         s->root.Add(s->child);
         s->stage = Mount(r, s->root, 100, 60);
         r.SetState(s);
         r.AfterRender({s->stage, s->root, s->child}, [=](Run& r) {
           r.Size("attached-to-root", s->root.GetMeasuredSize(), MeasuredSize(44, 28));
           r.Rendered("attached.rendered", s->root, LayoutRect(0, 0, 44, 28));
         });
       }},
      {"remove", 6, [](Run& r) {
         auto s = r.State<Roots>();
         s->root.Remove(s->child);
         r.AfterRender({s->stage, s->root}, [=](Run& r) {
           r.Size("empty", s->root.GetMeasuredSize(), MeasuredSize(0, 0));
           r.Rendered("empty.rendered", s->root, LayoutRect(0, 0, 0, 0));
         });
       }},
      {"detached", 2, [](Run& r) {
         auto s = r.State<Roots>();
         r.Size("detached-child", s->child.Measure(100, 60), MeasuredSize(37, 19));
       }},
      {"reattach", 10, [](Run& r) {
         auto s = r.State<Roots>();
         s->root.Add(s->child);
         r.AfterRender({s->stage, s->root, s->child}, [=](Run& r) {
           r.Size("reattached", s->root.GetMeasuredSize(), MeasuredSize(44, 28));
           r.Rendered("reattached.rendered", s->root, LayoutRect(0, 0, 44, 28));
           r.Rendered("child", s->child, LayoutRect(7, 9, 37, 19));
         });
       }},
    }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc126)
