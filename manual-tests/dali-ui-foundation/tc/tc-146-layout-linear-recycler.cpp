/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include "layout-validation-recycler-fixtures.h"
using namespace LayoutValidation;
namespace
{
class Tc146 : public Case
{
public:
  Tc146() : Case("TC146", "Linear item layout", "Check LinearItemsLayouter slots and scroll range") {}
  std::vector<Scenario> BuildScenarios() override
  {
    std::vector<Scenario> result;
    for(bool horizontal : {false, true})
      result.push_back(Single(horizontal ? "horizontal" : "vertical", "Fixed list extent, both scroll boundaries and wrong-axis no-op", 35, [horizontal](Run& r)
      {
        FixedRecycler fixture(10, 20, horizontal);
        auto layouter = LinearItemsLayouter::New(horizontal ? ItemsLayouter::Orientation::HORIZONTAL : ItemsLayouter::Orientation::VERTICAL);
        layouter.SetItemExtent(20); layouter.SetItemSpacing(2);
        auto& impl = layouter.GetImpl(); impl.OnLayoutChildren(fixture);
        r.Near("range", layouter.ComputeScrollRange(), 218); r.Near("extent", layouter.ComputeScrollExtent(), 50);
        r.Equal("first", layouter.GetFirstVisiblePosition(), 0); r.Equal("last", layouter.GetLastVisiblePosition(), 2);
        for(uint32_t i = 0; i < 3; ++i) r.Rect(Id("initial", i).c_str(), fixture.items[i], horizontal ? LayoutRect(22.f * i, 0, 20, 120) : LayoutRect(0, 22.f * i, 120, 20));
        auto scroll = [&](float delta) { return horizontal ? impl.ScrollHorizontallyBy(delta, fixture) : impl.ScrollVerticallyBy(delta, fixture); };
        r.Near("forward.consumed", scroll(26), 26); r.Near("forward.offset", layouter.ComputeScrollOffset(), 26);
        r.Equal("forward.first", layouter.GetFirstVisiblePosition(), 1); r.Equal("forward.last", layouter.GetLastVisiblePosition(), 3);
        r.Near("end.consumed", scroll(10000), 142); r.Near("end.offset", layouter.ComputeScrollOffset(), 168);
        r.Equal("end.first", layouter.GetFirstVisiblePosition(), 7); r.Equal("end.last", layouter.GetLastVisiblePosition(), 9);
        r.Near("end.clamp", scroll(1), 0); r.Near("start.consumed", scroll(-10000), -168); r.Near("start.offset", layouter.ComputeScrollOffset(), 0);
        r.Near("wrong.axis", horizontal ? impl.ScrollVerticallyBy(12, fixture) : impl.ScrollHorizontallyBy(12, fixture), 0);
        r.Truth("recycling.used", fixture.recycled > 0);
        r.Rect("item.bounds", layouter.GetItemBounds(4, 120), horizontal ? LayoutRect(88, 0, 20, 120) : LayoutRect(0, 88, 120, 20));
        r.Equal("active.range", layouter.GetLastVisiblePosition(), 2);
        r.Truth("axis.capability", horizontal ? layouter.CanScrollHorizontally() && !layouter.CanScrollVertically() : layouter.CanScrollVertically() && !layouter.CanScrollHorizontally());
      }));
    result.push_back(Single("cache-window", "Cache extents expand the active range independently of the viewport", 8, [](Run& r)
    {
      FixedRecycler fixture(10); fixture.before = 22; fixture.after = 22;
      auto layouter = LinearItemsLayouter::New(); layouter.SetItemExtent(20); layouter.SetItemSpacing(2);
      auto& impl = layouter.GetImpl(); impl.OnLayoutChildren(fixture);
      r.Equal("cache.first", layouter.GetFirstVisiblePosition(), 0); r.Equal("cache.last", layouter.GetLastVisiblePosition(), 3);
      r.Near("cache.scroll", impl.ScrollVerticallyBy(44, fixture), 44);
      r.Equal("cache.scrolled.first", layouter.GetFirstVisiblePosition(), 1); r.Equal("cache.scrolled.last", layouter.GetLastVisiblePosition(), 5);
      r.Near("cache.range", layouter.ComputeScrollRange(), 218); r.Near("cache.viewport", layouter.ComputeScrollExtent(), 50);
      r.Truth("cache.materialization", fixture.active[1] && fixture.active[5]);
    }));
    result.push_back(Single("variable-decoration", "Measured sizes and decoration offsets determine every content-space slot", 22, [](Run& r)
    {
      FixedRecycler fixture(3); fixture.viewport = 200;
      fixture.items[0].SetRequestedHeight(10); fixture.items[1].SetRequestedHeight(30); fixture.items[2].SetRequestedHeight(20);
      fixture.offsets.start = 3; fixture.offsets.end = 7; fixture.offsets.top = 2; fixture.offsets.bottom = 4;
      auto layouter = LinearItemsLayouter::New(); layouter.SetItemExtent(20); layouter.SetItemSpacing(5); layouter.GetImpl().OnLayoutChildren(fixture);
      const float y[] = {2, 23, 64}; const float h[] = {10, 30, 20};
      for(uint32_t i = 0; i < 3; ++i) r.Rect(Id("decorated", i).c_str(), fixture.items[i], LayoutRect(3, y[i], 110, h[i]));
      r.Near("variable.range", layouter.ComputeScrollRange(), 88);
      r.Rect("variable.content_slot", layouter.GetItemBounds(1, 120), LayoutRect(0, 21, 120, 36));
      layouter.SetItemExtent(-5); r.Near("extent.clamp", layouter.GetItemExtent(), 1);
      layouter.SetItemSpacing(-3); r.Near("spacing.clamp", layouter.GetItemSpacing(), 0);
      layouter.SetOrientation(ItemsLayouter::Orientation::HORIZONTAL);
      r.Near("orientation.offset.reset", layouter.ComputeScrollOffset(), 0);
      r.Equal("orientation.first.reset", layouter.GetFirstVisiblePosition(), 0); r.Equal("orientation.last.reset", layouter.GetLastVisiblePosition(), 0);
    }));
    result.push_back(Single("empty-zero-viewport", "Empty data and zero viewport perform no materialization", 6, [](Run& r)
    {
      FixedRecycler empty(0);
      auto          layouter = LinearItemsLayouter::New();
      layouter.GetImpl().OnLayoutChildren(empty);
      r.Near("empty.range", layouter.ComputeScrollRange(), 0);
      r.Equal("empty.gets", empty.gets, 0);
      r.Near("empty.scroll", layouter.GetImpl().ScrollVerticallyBy(10, empty), 0);
      FixedRecycler zero(10);
      zero.viewport = 0;
      layouter.GetImpl().OnLayoutChildren(zero);
      r.Equal("zero.gets", zero.gets, 0);
      r.Near("zero.extent", layouter.ComputeScrollExtent(), 0);
      r.Near("zero.scroll", layouter.GetImpl().ScrollVerticallyBy(10, zero), 0);
    }));
    for(bool horizontal : {false, true})
    {
      Scenario mounted{horizontal ? "window-horizontal" : "window-vertical", "Real RecyclerView controller and rendered scroll boundaries", {}};
      mounted.steps.push_back({"attach", 23, [horizontal](Run& r)
      {
        auto s = MakeWindowRecycler(r, std::vector<float>(10, 20), horizontal);
        ObserveWindowRecycler(r, s, 0, true);
      }});
      mounted.steps.push_back({"middle", 23, [](Run& r)
      {
        auto s = r.State<WindowRecyclerFixture>();
        s->view.SetScrollOffset(26);
        ObserveWindowRecycler(r, s, 26);
      }});
      mounted.steps.push_back({"end", 23, [](Run& r)
      {
        auto s = r.State<WindowRecyclerFixture>();
        s->view.SetScrollOffset(10000);
        ObserveWindowRecycler(r, s, 168);
      }});
      mounted.steps.push_back({"start", 23, [](Run& r)
      {
        auto s = r.State<WindowRecyclerFixture>();
        s->view.SetScrollOffset(-10000);
        ObserveWindowRecycler(r, s, 0);
      }});
      result.push_back(std::move(mounted));
      Scenario cached{horizontal ? "window-cache-horizontal" : "window-cache-vertical", "Actual cache materialization around a bounded viewport", {}};
      cached.steps.push_back({"attach", 23, [horizontal](Run& r)
      {
        auto s = MakeWindowRecycler(r, std::vector<float>(10, 20), horizontal);
        s->SetCache(22, 22);
        ObserveWindowRecycler(r, s, 0, true);
      }});
      cached.steps.push_back({"middle", 23, [](Run& r)
      {
        auto s = r.State<WindowRecyclerFixture>();
        s->view.SetScrollOffset(44);
        ObserveWindowRecycler(r, s, 44);
      }});
      cached.steps.push_back({"end", 23, [](Run& r)
      {
        auto s = r.State<WindowRecyclerFixture>();
        s->view.SetScrollOffset(10000);
        ObserveWindowRecycler(r, s, 168);
      }});
      result.push_back(std::move(cached));
    }
    Scenario decorated{"window-variable-decoration", "Actual adapter sizes, decoration offsets and cross-axis resize", {}};
    decorated.steps.push_back({"attach", 23, [](Run& r)
    {
      auto s = MakeWindowRecycler(r, {10, 30, 20}, false, true);
      // Populate the extent cache through the public RecyclerView before checking a bounded
      // materialization range. Unmeasured variable items otherwise have estimated sizes.
      s->SetCache(0, 1000);
      LayoutController::Get(r.GetWindow()).ProcessLayouts();
      s->SetCache(0, 0);
      ObserveWindowRecycler(r, s, 0);
    }});
    decorated.steps.push_back({"middle", 23, [](Run& r)
    {
      auto s = r.State<WindowRecyclerFixture>();
      s->view.SetScrollOffset(26);
      ObserveWindowRecycler(r, s, 26);
    }});
    decorated.steps.push_back({"end", 23, [](Run& r)
    {
      auto s = r.State<WindowRecyclerFixture>();
      s->view.SetScrollOffset(10000);
      ObserveWindowRecycler(r, s, 38);
    }});
    decorated.steps.push_back({"wider", 23, [](Run& r)
    {
      auto s = r.State<WindowRecyclerFixture>();
      s->SetCross(160);
      ObserveWindowRecycler(r, s, 38, true);
    }});
    decorated.steps.push_back({"start", 23, [](Run& r)
    {
      auto s = r.State<WindowRecyclerFixture>();
      s->view.SetScrollOffset(0);
      ObserveWindowRecycler(r, s, 0);
    }});
    result.push_back(std::move(decorated));
    for(bool empty : {true, false})
      result.push_back(Single(empty ? "window-empty" : "window-zero-viewport", "Actual empty data or zero viewport creates no holders", 24, [empty](Run& r)
      {
        auto s = MakeWindowRecycler(r, std::vector<float>(empty ? 0u : 10u, 20), false, false, empty ? 50.f : 0.f);
        LayoutController::Get(r.GetWindow()).ProcessLayouts();
        r.AfterFrame([s](Run& v)
        { CheckWindowRecycler(v, *s, 0); v.Equal("window.no-holder-created", s->creates, 0); }, {}, s->stage);
    }));
    // Items requesting WRAP_CONTENT on the cross axis (a Leaf's natural size there is 0) are measured
    // EXACTLY at the 120 slot cross extent, so they report it; the scroll axis keeps the explicit 20.
    // Viewport 50, extent 20, spacing 2, no cache: items 0..2 are materialized.
    for(bool horizontal : {false, true})
      result.push_back(Single(horizontal ? "window-wrap-cross-horizontal" : "window-wrap-cross-vertical", "Items are measured exactly at the slot's cross extent", 7, [horizontal](Run& r)
      {
        auto s = MakeWindowRecycler(r, std::vector<float>(10, 20), horizontal, false, 50, true);
        LayoutController::Get(r.GetWindow()).ProcessLayouts();
        r.AfterFrame([s, horizontal](Run& v)
        {
          v.Equal("cross.materialized", s->bound.size(), 3);
          for(uint32_t i = 0; i < 3; ++i)
          {
            const auto         found    = s->bound.find(i);
            const MeasuredSize measured = found != s->bound.end() ? found->second.GetMeasuredSize() : MeasuredSize(-1, -1);
            v.Size(Id("cross.measured", i).c_str(), measured, horizontal ? MeasuredSize(20, 120) : MeasuredSize(120, 20));
          }
        }, {}, s->stage);
      }));
    return result;
  }
};
} //namespace
REGISTER_MANUAL_TEST(Tc146)
