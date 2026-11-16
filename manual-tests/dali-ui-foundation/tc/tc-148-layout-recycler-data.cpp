/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>
#include "layout-validation-fixtures.h"
#include "layout-validation-recycler-fixtures.h"
using namespace LayoutValidation;
using namespace LayoutValidation::Fixtures;
namespace
{
// Forces one controller pass over the mounted recycler and runs `check` after the window
// has rendered it.
void AfterRelayout(Run& r, const std::shared_ptr<AdapterFixture>& s, std::function<void(Run&, AdapterFixture&)> check)
{
  s->view.InvalidateMeasure();
  r.AfterRender({s->stage, s->view}, [s, check](Run& done) { check(done, *s); });
}
void CheckInitial(Run& r, AdapterFixture& s)
{
  r.Rendered("recycler.target", s.view, LayoutRect(0, 0, 120, 50));
  r.Near("recycler.range", s.layouter.ComputeScrollRange(), 262);
  r.Near("recycler.viewport", s.layouter.ComputeScrollExtent(), 50);
  r.Equal("bound.count", s.bound.size(), 3);
  CheckBoundIds(r, s, "initial");
  for(uint32_t i = 0; i < 3; ++i)
  {
    auto found = s.bound.find(i);
    if(found == s.bound.end())
    {
      r.Fail("bound.missing", "expected materialized item is absent");
      continue;
    }
    r.Rendered(Id("bound.rect", i).c_str(), found->second, LayoutRect(0, 22.f * i, 120, 20));
  }
}

struct ScrollFixture
{
  std::shared_ptr<AdapterFixture> data;
  uint32_t                        started{0}, finished{0};
};
// Thirteen assertions, using content coordinates and both event/render scroller positions.
void CheckScrollGeometry(Run& r, const ScrollFixture& s, uint32_t position, float offset)
{
  const auto& data = *s.data;
  r.Near("scroll.offset", data.view.GetScrollOffset(), offset);
  r.Near("scroll.range", data.layouter.ComputeScrollRange(), 262);
  r.Near("scroll.viewport", data.layouter.ComputeScrollExtent(), 50);
  const auto found = data.bound.find(position);
  r.Truth("scroll.target.materialized", found != data.bound.end());
  if(found == data.bound.end()) return;
  const View item           = found->second;
  const auto parent         = item.GetParent();
  const auto content        = LayoutValidation::Bounds(item);
  const auto eventPosition  = parent.GetProperty<Vector3>(Actor::Property::POSITION);
  const auto renderPosition = parent.GetCurrentProperty<Vector3>(Actor::Property::POSITION);
  r.Rect("scroll.content.rect", content, LayoutRect(0, 22.f * position, 120, 20));
  r.Near("scroll.scroller.x", eventPosition.x, 0);
  r.Near("scroll.scroller.y", eventPosition.y, -offset);
  r.Near("scroll.viewport.item.y", content.y + eventPosition.y, 22.f * position - offset);
  r.Near("scroll.render.scroller.y", renderPosition.y, -offset);
  r.Truth("scroll.animation.idle", !data.view.IsScrolling());
}
Step PositionStep(const char* name, uint32_t position, float offset)
{
  return {name, 13, [position, offset](Run& r)
  {
    auto s = r.State<ScrollFixture>();
    s->data->view.ScrollToPosition(position, false);
    r.Delay(100, [s, position, offset](Run& done)
    { CheckScrollGeometry(done, *s, position, offset); });
  }};
}
Step OffsetStep(const char* name, float requested, uint32_t position, float offset)
{
  return {name, 13, [requested, position, offset](Run& r)
  {
    auto s = r.State<ScrollFixture>();
    s->data->view.SetScrollOffset(requested);
    r.Delay(100, [s, position, offset](Run& done)
    { CheckScrollGeometry(done, *s, position, offset); });
  }};
}
Step FocusStep(const char* name, float initial, uint32_t position, float peek, float expected, bool enable = true)
{
  return {name, 18, [initial, position, peek, expected, enable](Run& r)
  {
    auto s     = r.State<ScrollFixture>();
    auto focus = FocusManager::Get();
    s->data->view.SetScrollOnFocus(false);
    focus.ClearFocus();
    s->data->view.SetScrollOffset(initial);
    s->data->view.SetFocusScrollPeek(peek);
    s->data->view.SetScrollOnFocus(enable);
    const auto target = s->data->bound.find(position);
    if(target == s->data->bound.end())
    {
      r.Fail("focus.target.missing", "required cached holder is absent");
      return;
    }
    const View view    = target->second;
    const auto started = s->started, finished = s->finished;
    r.Truth("focus.request.accepted", focus.SetCurrentFocusView(view));
    r.Near("focus.peek.normalized", s->data->view.GetFocusScrollPeek(), std::max(0.f, peek));
    // The longest configured native scroll animation is 1200 ms. No clock substitution.
    r.Delay(2000, [s, view, position, expected, initial, started, finished](Run& done)
    {
      CheckScrollGeometry(done, *s, position, expected);
      done.Truth("focus.actual.target", FocusManager::Get().GetCurrentFocusView() == view);
      const uint32_t animations = expected == initial ? 0u : 1u;
      done.Equal("focus.scroll.started.delta", s->started - started, animations);
      done.Equal("focus.scroll.finished.delta", s->finished - finished, animations);
    });
  }};
}
Scenario ScrollScenario()
{
  Scenario scenario{"window-scroll-focus", "Public position scrolling and actual focus/peek/clamp geometry", {}};
  scenario.steps.push_back({"attach", 18, [](Run& r)
  {
    auto s  = std::make_shared<ScrollFixture>();
    s->data = MakeAdapterFixture(r);
    s->data->view.SetScrollOnFocus(false);
    s->data->view.SetCacheExtent(1000, 1000);
    for(auto& entry : s->data->bound) entry.second.SetFocusable(true);
    s->data->adapter.BindViewHolderSignal().Connect(&r, [](ItemViewHolder& holder)
    { holder.view.SetFocusable(true); });
    s->data->view.ScrollStartedSignal().Connect(&r, [s](RecyclerView)
    { ++s->started; });
    s->data->view.ScrollFinishedSignal().Connect(&r, [s](RecyclerView)
    { ++s->finished; });
    auto       focus    = FocusManager::Get();
    const auto previous = focus.GetCurrentFocusView();
    r.OnCleanup([s, focus, previous]() mutable
    {
      s->data->view.SetScrollOnFocus(false);
      s->data->view.SetScrollOffset(0);
      if(previous)
        focus.SetCurrentFocusView(previous);
      else
        focus.ClearFocus();
    });
    r.SetState(s);
    r.AfterLayout({s->data->view}, [s](Run& done)
    {
      done.Rect("scroll.window.target", done.Snapshot(s->data->view), LayoutRect(0, 0, 120, 50));
      done.Equal("scroll.cached.all", s->data->bound.size(), 12);
      CheckScrollGeometry(done, *s, 0, 0);
    });
    s->data->stage = Mount(r, s->data->view, 120, 50); // 120x50 viewport from the stage constraint
  }});
  scenario.steps.push_back(PositionStep("position-middle", 3, 66));
  scenario.steps.push_back(PositionStep("position-end-clamp", 11, 212));
  scenario.steps.push_back(PositionStep("position-start", 0, 0));
  scenario.steps.push_back(OffsetStep("offset-low-clamp", -99, 0, 0));
  scenario.steps.push_back(OffsetStep("offset-high-clamp", 999, 11, 212));
  scenario.steps.push_back(FocusStep("focus-end", 0, 3, 0, 36));
  scenario.steps.push_back(FocusStep("focus-start", 80, 3, 0, 66));
  scenario.steps.push_back(FocusStep("focus-already-visible", 60, 3, 0, 60));
  scenario.steps.push_back(FocusStep("peek-end", 0, 3, 7, 43));
  scenario.steps.push_back(FocusStep("peek-start", 60, 3, 7, 59));
  scenario.steps.push_back(FocusStep("peek-head-clamp", 100, 0, 7, 0));
  scenario.steps.push_back(FocusStep("peek-tail-clamp", 0, 11, 7, 212));
  scenario.steps.push_back(FocusStep("negative-peek-normalize", 0, 3, -7, 36));
  scenario.steps.push_back(FocusStep("focus-scroll-disabled", 0, 3, 7, 0, false));
  return scenario;
}

class Tc148 : public Case
{
public:
  Tc148()
  : Case("TC148", "Recycler data mutations", "Check recycler data changes, scroll and focus")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    return {
      {"adapter-lifecycle", "Real RecyclerView binding, content/size updates, insert/move/remove and replacement", {{"initial", 26, [](Run& r)
    {
      auto s = MakeAdapterFixture(r);
      r.SetState(s);
      s->stage = Mount(r, s->view, 120, 50);
      r.AfterRender({s->stage, s->view}, [s](Run& done) { CheckInitial(done, *s); });
    }},
                                                                                                                    {"content-only", 14, [](Run& r)
    {
      auto       s       = r.State<AdapterFixture>();
      const auto binds   = s->binds;
      const auto creates = s->creates;
      s->ids[1]          = 7777;
      s->adapter.NotifyItemContentChanged(1);
      r.Equal("content.bind.delta", s->binds - binds, 1);
      r.Equal("content.create.delta", s->creates - creates, 0);
      r.Text("content.id", std::string(s->bound.at(1).GetProperty<Dali::String>(Actor::Property::NAME).CStr()), "stable.7777");
      r.Rendered("content.geometry", s->bound.at(1), LayoutRect(0, 22, 120, 20));
      CheckBoundIds(r, *s, "content");
    }},
                                                                                                                    {"insert", 9, [](Run& r)
    {
      auto s = r.State<AdapterFixture>();
      s->ids.insert(s->ids.begin(), 8888);
      s->heights.insert(s->heights.begin(), 20);
      s->adapter.NotifyItemInserted(0);
      AfterRelayout(r, s, [](Run& done, AdapterFixture& s) {
        done.Near("insert.range", s.layouter.ComputeScrollRange(), 284);
        done.Equal("insert.count", s.adapter.GetItemCount(), 13);
        CheckBoundIds(done, s, "insert");
      });
    }},
                                                                                                                    {"move", 9, [](Run& r)
    {
      auto       s      = r.State<AdapterFixture>();
      const auto id     = s->ids[0];
      const auto height = s->heights[0];
      s->ids.erase(s->ids.begin());
      s->heights.erase(s->heights.begin());
      s->ids.insert(s->ids.begin() + 2, id);
      s->heights.insert(s->heights.begin() + 2, height);
      s->adapter.NotifyItemMoved(0, 2);
      AfterRelayout(r, s, [](Run& done, AdapterFixture& s) {
        done.Near("move.range", s.layouter.ComputeScrollRange(), 284);
        done.Equal("move.count", s.adapter.GetItemCount(), 13);
        CheckBoundIds(done, s, "move");
      });
    }},
                                                                                                                    {"remove", 9, [](Run& r)
    {
      auto s = r.State<AdapterFixture>();
      s->ids.erase(s->ids.begin());
      s->heights.erase(s->heights.begin());
      s->adapter.NotifyItemRemoved(0);
      AfterRelayout(r, s, [](Run& done, AdapterFixture& s) {
        done.Near("remove.range", s.layouter.ComputeScrollRange(), 262);
        done.Equal("remove.count", s.adapter.GetItemCount(), 12);
        CheckBoundIds(done, s, "remove");
      });
    }},
                                                                                                                    {"size-change", 9, [](Run& r)
    {
      auto s        = r.State<AdapterFixture>();
      s->heights[0] = 30;
      s->adapter.NotifyItemChanged(0);
      AfterRelayout(r, s, [](Run& done, AdapterFixture& s) {
        done.Rendered("size.changed", s.bound.at(0), LayoutRect(0, 0, 120, 30));
        done.Rendered("size.next.offset", s.bound.at(1), LayoutRect(0, 32, 120, 20));
        done.Near("size.range.estimated", s.layouter.ComputeScrollRange(), 272);
      });
    }},
                                                                                                                    {"full-data-empty", 4, [](Run& r)
    {
      auto s = r.State<AdapterFixture>();
      s->ids.clear();
      s->heights.clear();
      s->adapter.NotifyDataSetChanged();
      AfterRelayout(r, s, [](Run& done, AdapterFixture& s) {
        done.Near("empty.range", s.layouter.ComputeScrollRange(), 0);
        done.Equal("empty.bound", s.bound.size(), 0);
        done.Equal("empty.count", s.adapter.GetItemCount(), 0);
        done.Truth("recycle.used", s.recycles > 0);
      });
    }},
                                                                                                                    {"replace", 2, [](Run& r)
    {
      auto s           = r.State<AdapterFixture>();
      auto replacement = MakeAdapterFixture(r, 2);
      s->view.SetAdapter(replacement->adapter);
      AfterRelayout(r, s, [](Run& done, AdapterFixture& s) {
        done.Equal("replace.count", s.view.GetAdapter().GetItemCount(), 2);
        done.Near("replace.range", s.layouter.ComputeScrollRange(), 42);
      });
    }},
                                                                                                                    {"clear", 2, [](Run& r)
    {
      auto s = r.State<AdapterFixture>();
      s->view.ClearAdapter();
      AfterRelayout(r, s, [](Run& done, AdapterFixture& s) {
        done.Truth("clear.adapter", !s.view.GetAdapter());
        done.Near("clear.range", s.layouter.ComputeScrollRange(), 0);
      });
    }}}},
      {"window-viewport", "Real Window completion fence and viewport resize", {{"attach", 8, [](Run& r)
    {
      auto s = MakeAdapterFixture(r);
      r.SetState(s);
      r.AfterLayout({s->view}, [s](Run& run)
      { run.Rect("window.target", run.Snapshot(s->view), LayoutRect(0,0,120,50)); run.Near("window.range", s->layouter.ComputeScrollRange(),262); run.Near("window.viewport",s->layouter.ComputeScrollExtent(),50); run.Equal("window.first",s->view.GetFirstVisiblePosition(),0); run.Equal("window.last",s->view.GetLastVisiblePosition(),2); });
      // A RecyclerView fills its constraint, so the 120x50 viewport comes from a stage of that size.
      s->stage = Mount(r, s->view, 120, 50);
    }},
                                                                               {"resize", 7, [](Run& r)
    {
      auto s = r.State<AdapterFixture>();
      r.AfterLayout({s->view}, [s](Run& run)
      { run.Rect("resized.target",run.Snapshot(s->view),LayoutRect(0,0,120,94)); run.Near("resized.viewport",s->layouter.ComputeScrollExtent(),94); run.Equal("resized.first",s->view.GetFirstVisiblePosition(),0); run.Equal("resized.last",s->view.GetLastVisiblePosition(),4); });
      s->stage.SetRequestedHeight(94); // the stage constraint is what the recycler fills
      s->view.SetRequestedHeight(94);
    }}}},
      ScrollScenario()};
  }
};
} //namespace
REGISTER_MANUAL_TEST(Tc148)
