/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include "layout-validation-fixtures.h"
#include <dali-ui-foundation/public-api/views/view-impl.h>

using namespace LayoutValidation;
using namespace LayoutValidation::Fixtures;

namespace
{
struct Dispatch
{
  std::shared_ptr<Probe> probe;
  View                   stage;
};
// Application-defined LayoutManager (the public extension point): pure Measure/Arrange
// overrides that place each child at the previous child's far corner scaled by `step`, plus
// manager-owned state whose setter must invalidate the owner because no layout cache key
// can see it. Producer calls are counted to observe the measure cache and the default
// IF_CHANGED arrange policy of a custom manager.
class DiagonalManager : public LayoutManager
{
public:
  MeasuredSize Measure(ViewImpl* view, float widthConstraint, float heightConstraint) override
  {
    ++measures;
    float          totalWidth  = 0.0f;
    float          totalHeight = 0.0f;
    const uint32_t count       = GetChildViewCount(view);
    for(uint32_t i = 0; i < count; ++i)
    {
      View      child     = GetChildViewAt(view, i);
      ViewImpl& childImpl = GetImpl(child);
      if(IsStandalone(&childImpl)) continue;
      const MeasuredSize size = childImpl.Measure(widthConstraint - totalWidth, heightConstraint - totalHeight);
      totalWidth += size.width;
      totalHeight += size.height;
    }
    return {totalWidth, totalHeight};
  }
  void Arrange(ViewImpl* view, const LayoutRect& bounds) override
  {
    ++arranges;
    float          x     = bounds.x;
    float          y     = bounds.y;
    const uint32_t count = GetChildViewCount(view);
    for(uint32_t i = 0; i < count; ++i)
    {
      View      child     = GetChildViewAt(view, i);
      ViewImpl& childImpl = GetImpl(child);
      if(IsStandalone(&childImpl)) continue;
      const MeasuredSize size = childImpl.GetMeasuredSize();
      childImpl.Arrange({x, y, size.width, size.height});
      x += size.width * mStep;
      y += size.height * mStep;
    }
  }
  // The equality guard matters as much as the invalidation: writing the held value must
  // not schedule work.
  void SetStep(float step)
  {
    if(mStep == step) return;
    mStep = step;
    InvalidateOwnerMeasure();
  }
  uint32_t measures{0};
  uint32_t arranges{0};

private:
  float mStep{1.0f};
};
struct CustomManaged
{
  View             stage, sibling, root, a, b, c;
  DiagonalManager* manager{nullptr}; // owned by root through AttachLayoutManager
  uint32_t         measures{0}, arranges{0};
  void             Snapshot()
  {
    measures = manager->measures;
    arranges = manager->arranges;
  }
};
} // namespace

class Tc99 : public Case
{
public:
  Tc99()
  : Case("TC99", "Layout dispatch", "Check callback/LayoutManager/default dispatch")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> out;
    out.push_back(Steps("C17.callback-manager-default", "Callback, attached manager and default produce distinct rendered sizes", {
      {"default", 6, [](Run& r) {
         auto s   = std::make_shared<Dispatch>();
         s->probe = Probed(r, 7, 9, false);
         View root = s->probe->view;
         root.Add(Leaf(20, 10, "first"));
         root.Add(Leaf(30, 15, "second"));
         root.SetMeasureCallback({});
         s->stage = Mount(r, root, 200, 100);
         r.SetState(s);
         r.AfterRender({s->stage, root}, [=](Run& r) {
           r.Size("default-before-attach", s->probe->view.GetMeasuredSize(), MeasuredSize(30, 15));
           r.Rendered("default.rendered", s->probe->view, LayoutRect(0, 0, 30, 15));
         });
       }},
      {"callback", 6, [](Run& r) {
         auto s    = r.State<Dispatch>();
         View root = s->probe->view;
         root.AttachLayoutManager(Dali::UniquePtr<LayoutManager>(new StackLayoutManager(StackOrientation::HORIZONTAL, 7)));
         root.SetMeasureCallback(MeasureCallback::New(s->probe.get(), &Probe::Measure));
         root.InvalidateMeasure();
         r.AfterRender({s->stage, root}, [=](Run& r) {
           r.Size("callback-priority", s->probe->view.GetMeasuredSize(), MeasuredSize(7, 9));
           r.Rendered("callback.rendered", s->probe->view, LayoutRect(0, 0, 7, 9));
         });
       }},
      {"manager", 6, [](Run& r) {
         auto s    = r.State<Dispatch>();
         View root = s->probe->view;
         root.SetMeasureCallback({});
         root.InvalidateMeasure();
         r.AfterRender({s->stage, root}, [=](Run& r) {
           r.Size("manager-after-remove", s->probe->view.GetMeasuredSize(), MeasuredSize(57, 15));
           r.Rendered("manager.rendered", s->probe->view, LayoutRect(0, 0, 57, 15));
         });
       }},
    }));
    for(unsigned kind = 1; kind < 5; ++kind)
      out.push_back(Single(Id("C18.attached-manager", kind).c_str(), "View plus attached manager renders like the public container", 8, [kind](Run& r) {
        View                           a = Container(kind, 200, 100), b = Leaf(200, 100, "attached");
        Dali::UniquePtr<LayoutManager> manager;
        if(kind == 1) manager = Dali::UniquePtr<LayoutManager>(new StackLayoutManager(StackOrientation::HORIZONTAL, 0));
        if(kind == 2) manager = Dali::UniquePtr<LayoutManager>(new FlexLayoutManager(FlexDirection::ROW, FlexWrap::NO_WRAP, FlexJustify::FLEX_START, FlexAlign::STRETCH, FlexAlign::STRETCH));
        if(kind == 3) manager = Dali::UniquePtr<LayoutManager>(new GridLayoutManager({}, {}, 0, 0));
        if(kind == 4) manager = Dali::UniquePtr<LayoutManager>(new AbsoluteLayoutManager());
        b.AttachLayoutManager(std::move(manager));
        View ca = Leaf(20, 10, "container-child"), cb = Leaf(20, 10, "attached-child");
        a.Add(ca);
        b.Add(cb);
        b.SetRequestedY(110);
        View stage = r.Stage(200, 220);
        stage.Add(a);
        stage.Add(b);
        LayoutRect expected(0, 0, 20, 10);
        if(kind == 2) expected.height = 100;
        if(kind == 3)
        {
          expected.width  = 200;
          expected.height = 100;
        }
        r.AfterRender({stage, a, b, ca, cb}, [=](Run& r) {
          r.Rendered("container", ca, expected);
          r.Rendered("attached", cb, expected);
        });
      }));
    out.push_back(Steps("C19.custom-manager", "Application-defined LayoutManager: dispatch, manager-owned state invalidation and cache behaviour on screen", {
      {"mount", 14, [](Run& r) {
         auto s  = std::make_shared<CustomManaged>();
         s->root = View::New();
         s->root.SetRequestedWidth(WRAP_CONTENT);
         s->root.SetRequestedHeight(WRAP_CONTENT);
         auto manager = Dali::MakeUnique<DiagonalManager>();
         s->manager   = manager.Get();
         s->root.AttachLayoutManager(std::move(manager));
         s->a = Leaf(20, 10, "a");
         s->b = Leaf(30, 15, "b");
         s->c = Leaf(40, 20, "c");
         s->root.Add(s->a);
         s->root.Add(s->b);
         s->root.Add(s->c);
         s->sibling = Leaf(10, 10, "sibling");
         s->sibling.SetRequestedY(80);
         s->stage = r.Stage(200, 100);
         ColorizeTree(s->root);
         s->stage.Add(s->root);
         s->stage.Add(s->sibling);
         r.SetState(s);
         r.AfterRender({s->stage, s->root, s->a, s->b, s->c}, [=](Run& r) {
           r.Size("custom.measured", s->root.GetMeasuredSize(), MeasuredSize(90, 45));
           r.Rendered("custom.a", s->a, LayoutRect(0, 0, 20, 10));
           r.Rendered("custom.b", s->b, LayoutRect(20, 10, 30, 15));
           r.Rendered("custom.c", s->c, LayoutRect(50, 25, 40, 20));
           s->Snapshot();
         });
       }},
      {"step-1.5", 16, [](Run& r) {
         auto s = r.State<CustomManaged>();
         s->manager->SetStep(1.5f); // InvalidateOwnerMeasure() must schedule the pass by itself
         r.AfterRender({s->stage, s->root, s->a, s->b, s->c}, [=](Run& r) {
           r.Equal("step.measure-delta", s->manager->measures - s->measures, 1);
           r.Equal("step.arrange-delta", s->manager->arranges - s->arranges, 1);
           r.Size("step.measured", s->root.GetMeasuredSize(), MeasuredSize(90, 45));
           r.Rendered("step.a", s->a, LayoutRect(0, 0, 20, 10));
           r.Rendered("step.b", s->b, LayoutRect(30, 15, 30, 15));
           r.Rendered("step.c", s->c, LayoutRect(75, 37.5f, 40, 20));
           s->Snapshot();
         });
       }},
      {"same-value", 6, [](Run& r) {
         auto s = r.State<CustomManaged>();
         s->manager->SetStep(1.5f); // unchanged value: no invalidation, no pass
         r.Delay(300, [=](Run& r) {
           r.Equal("same.measure-delta", s->manager->measures - s->measures, 0);
           r.Equal("same.arrange-delta", s->manager->arranges - s->arranges, 0);
           r.Rendered("same.c", s->c, LayoutRect(75, 37.5f, 40, 20));
         });
       }},
      {"sibling-pass", 6, [](Run& r) {
         auto s = r.State<CustomManaged>();
         s->sibling.SetRequestedWidth(11); // a pass over the stage: the custom manager is served from its caches
         r.AfterRender({s->stage, s->sibling}, [=](Run& r) {
           r.Equal("sibling.measure-delta", s->manager->measures - s->measures, 0);
           r.Equal("sibling.arrange-delta", s->manager->arranges - s->arranges, 0);
           r.Rendered("sibling.c", s->c, LayoutRect(75, 37.5f, 40, 20));
         });
       }},
      {"step-1.0", 14, [](Run& r) {
         auto s = r.State<CustomManaged>();
         s->manager->SetStep(1.0f);
         r.AfterRender({s->stage, s->root, s->a, s->b, s->c}, [=](Run& r) {
           r.Size("restore.measured", s->root.GetMeasuredSize(), MeasuredSize(90, 45));
           r.Rendered("restore.a", s->a, LayoutRect(0, 0, 20, 10));
           r.Rendered("restore.b", s->b, LayoutRect(20, 10, 30, 15));
           r.Rendered("restore.c", s->c, LayoutRect(50, 25, 40, 20));
         });
       }},
    }));
    return out;
  }
};

REGISTER_MANUAL_TEST(Tc99)
