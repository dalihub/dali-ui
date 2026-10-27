/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#pragma once
#include <dali-ui-foundation/integration-api/recycler/group-adapter.h>
#include <dali-ui-foundation/integration-api/recycler/group-data-source.h>
#include <dali-ui-foundation/integration-api/recycler/group-linear-items-layouter.h>
#include <dali-ui-foundation/integration-api/recycler/item-adapter.h>
#include <dali-ui-foundation/integration-api/recycler/item-decoration.h>
#include <dali-ui-foundation/integration-api/recycler/item-view-holder.h>
#include <dali-ui-foundation/integration-api/recycler/items-layouter-impl.h>
#include <dali-ui-foundation/integration-api/recycler/linear-items-layouter.h>
#include <dali-ui-foundation/integration-api/recycler/recycler-view.h>
#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include "layout-validation-support.h"

// The Recycler API family lives under Dali::Ui::Integration, so the unqualified names
// the fixtures and their test cases use are not reached by the support header's
// `using namespace Dali::Ui`.
using Dali::Ui::Integration::GroupAdapter;
using Dali::Ui::Integration::GroupDataSource;
using Dali::Ui::Integration::GroupLinearItemsLayouter;
using Dali::Ui::Integration::GroupRowType;
using Dali::Ui::Integration::ItemAdapter;
using Dali::Ui::Integration::ItemDecoration;
using Dali::Ui::Integration::ItemsLayouter;
using Dali::Ui::Integration::ItemViewHolder;
using Dali::Ui::Integration::LinearItemsLayouter;
using Dali::Ui::Integration::RecyclerView;

namespace LayoutValidation
{
struct FixedRecycler : Dali::Ui::Integration::Recycler
{
  std::vector<View> items;
  std::vector<bool> active;
  float viewport{50};
  float cross{120};
  float before{0};
  float after{0};
  Insets offsets{};
  uint32_t gets{0};
  uint32_t recycled{0};
  explicit FixedRecycler(uint32_t count, float extent = 20, bool horizontal = false)
  {
    for(uint32_t i = 0; i < count; ++i) items.push_back(Leaf(horizontal ? extent : MATCH_PARENT, horizontal ? MATCH_PARENT : extent));
    active.resize(count, false);
  }
  View GetViewForPosition(uint32_t position) override { ++gets; if(position >= items.size()) return {}; active[position] = true; return items[position]; }
  void RecycleViewForPosition(uint32_t position) override { if(position < active.size() && active[position]) { active[position] = false; ++recycled; } }
  void RecycleAllViews() override { for(uint32_t i = 0; i < active.size(); ++i) RecycleViewForPosition(i); }
  uint32_t GetItemCount() const override { return static_cast<uint32_t>(items.size()); }
  float GetViewportExtent() const override { return viewport; }
  float GetCrossExtent() const override { return cross; }
  float GetCacheBefore() const override { return before; }
  float GetCacheAfter() const override { return after; }
  Insets GetDecorationOffsets(uint32_t) const override { return offsets; }
};
struct Groups : GroupDataSource
{
  std::vector<uint32_t> counts{1, 3, 0, 2};
  std::vector<bool> headers{true, false, true, false};
  uint32_t GetGroupCount() const override { return static_cast<uint32_t>(counts.size()); }
  uint32_t GetGroupItemCount(uint32_t group) const override { return counts.at(group); }
  bool HasGroupHeader(uint32_t group) const override { return headers.at(group); }
};
struct AdapterFixture
{
  ItemAdapter adapter;
  RecyclerView view;
  LinearItemsLayouter layouter;
  std::vector<uint32_t> ids;
  std::vector<float> heights;
  std::map<uint32_t, View> bound;
  uint32_t creates{0}, binds{0}, recycles{0};
  View stage; ///< On-screen stage holding the recycler for rendered verification.
};
inline std::shared_ptr<AdapterFixture> MakeAdapterFixture(Run& r, uint32_t count = 12)
{
  auto s = std::make_shared<AdapterFixture>();
  for(uint32_t i = 0; i < count; ++i) { s->ids.push_back(1000 + i); s->heights.push_back(20); }
  s->adapter = ItemAdapter::New();
  s->adapter.GetItemCountSignal().Connect(&r, [s]() { return static_cast<uint32_t>(s->ids.size()); });
  s->adapter.CreateViewHolderSignal().Connect(&r, [s](ItemViewHolder& holder) { ++s->creates; holder.view = Leaf(MATCH_PARENT, 20); });
  s->adapter.BindViewHolderSignal().Connect(&r, [s](ItemViewHolder& holder)
  {
    ++s->binds;
    holder.view.SetProperty(Actor::Property::NAME, Dali::String(Id("stable", s->ids.at(holder.position)).c_str()));
    holder.view.SetRequestedHeight(s->heights.at(holder.position));
    s->bound[holder.position] = holder.view;
  });
  s->adapter.RecycleViewHolderSignal().Connect(&r, [s](ItemViewHolder& holder)
  {
    ++s->recycles;
    auto found = s->bound.find(holder.position);
    if(found != s->bound.end() && found->second == holder.view) s->bound.erase(found);
  });
  s->layouter = LinearItemsLayouter::New(); s->layouter.SetItemExtent(20); s->layouter.SetItemSpacing(2);
  s->view = RecyclerView::New(); s->view.SetRequestedWidth(120); s->view.SetRequestedHeight(50);
  s->view.SetCacheExtent(0, 0); s->view.SetItemsLayouter(s->layouter); s->view.SetAdapter(s->adapter);
  return s;
}
inline void CheckBoundIds(Run& r, const AdapterFixture& s, const char* prefix)
{
  r.Truth((std::string(prefix) + ".nonempty").c_str(), !s.bound.empty());
  for(const auto& entry : s.bound)
  {
    r.Truth(Id("bound.valid_position", entry.first).c_str(), entry.first < s.ids.size());
    if(entry.first < s.ids.size())
    {
      const auto name = entry.second.GetProperty<Dali::String>(Actor::Property::NAME);
      r.Text(Id("bound.stable_id", entry.first).c_str(), std::string(name.CStr()), Id("stable", s.ids[entry.first]));
    }
  }
}

// A real RecyclerView fixture with an independent catalog of slot geometry. It keeps a
// bounded viewport even when a preparation action materializes every item through cache.
struct WindowRecyclerFixture
{
  struct Decoration : ItemDecoration
  {
    Insets offsets{};
    Insets GetItemOffsets(const ItemViewHolder&) const override
    {
      return offsets;
    }
  };
  ItemAdapter                 adapter;
  RecyclerView                view;
  LinearItemsLayouter         layouter;
  View                        stage, scroller;
  std::unique_ptr<Decoration> decoration;
  std::vector<float>          extents;
  std::map<uint32_t, View>    bound;
  bool                        horizontal{false};
  bool                        wrapCross{false}; ///< Items request WRAP_CONTENT instead of MATCH_PARENT on the cross axis
  float                       viewport{50}, cross{120}, spacing{2}, before{0}, after{0};
  uint32_t                    creates{0}, recycles{0};

  Insets Offsets() const
  {
    return decoration ? decoration->offsets : Insets{};
  }
  float SlotExtent(uint32_t index) const
  {
    const auto offsets = Offsets();
    return extents[index] + (horizontal ? offsets.start + offsets.end : offsets.top + offsets.bottom);
  }
  float SlotStart(uint32_t index) const
  {
    float result = 0;
    for(uint32_t i = 0; i < index; ++i) result += SlotExtent(i) + spacing;
    return result;
  }
  float Range() const
  {
    return extents.empty() ? 0 : SlotStart(extents.size() - 1) + SlotExtent(extents.size() - 1);
  }
  LayoutRect ItemRect(uint32_t index) const
  {
    const auto offsets = Offsets();
    return horizontal ? LayoutRect(SlotStart(index) + offsets.start, offsets.top, extents[index], cross - offsets.top - offsets.bottom)
                      : LayoutRect(offsets.start, SlotStart(index) + offsets.top, cross - offsets.start - offsets.end, extents[index]);
  }
  std::vector<uint32_t> Active(float offset) const
  {
    std::vector<uint32_t> result;
    if(viewport <= 0) return result;
    for(uint32_t i = 0; i < extents.size(); ++i)
      if(SlotStart(i) + SlotExtent(i) > std::max(0.f, offset - before) && SlotStart(i) < offset + viewport + after) result.push_back(i);
    return result;
  }
  void SetCache(float leading, float trailing)
  {
    before = leading;
    after  = trailing;
    view.SetCacheExtent(before, after);
  }
  void SetCross(float value)
  {
    cross = value;
    if(horizontal)
      view.SetRequestedHeight(value);
    else
      view.SetRequestedWidth(value);
    view.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds({0, 0, horizontal ? viewport : cross, horizontal ? cross : viewport}));
  }
};

inline std::shared_ptr<WindowRecyclerFixture> MakeWindowRecycler(Run& r, const std::vector<float>& extents, bool horizontal = false, bool decorated = false, float viewport = 50, bool wrapCross = false)
{
  auto s        = std::make_shared<WindowRecyclerFixture>();
  s->extents    = extents;
  s->horizontal = horizontal;
  s->wrapCross  = wrapCross;
  s->viewport   = viewport;
  s->spacing    = decorated ? 5 : 2;
  if(decorated)
  {
    s->decoration                 = std::make_unique<WindowRecyclerFixture::Decoration>();
    s->decoration->offsets.start  = 3;
    s->decoration->offsets.end    = 7;
    s->decoration->offsets.top    = 2;
    s->decoration->offsets.bottom = 4;
  }
  const std::weak_ptr<WindowRecyclerFixture> weak = s;
  s->adapter                                      = ItemAdapter::New();
  s->adapter.GetItemCountSignal().Connect(&r, [weak]()
  { auto state = weak.lock(); return state ? static_cast<uint32_t>(state->extents.size()) : 0u; });
  s->adapter.CreateViewHolderSignal().Connect(&r, [weak](ItemViewHolder& holder)
  {
    if(auto state = weak.lock())
    {
      ++state->creates;
      holder.view = Leaf(20, 20);
    }
  });
  s->adapter.BindViewHolderSignal().Connect(&r, [weak](ItemViewHolder& holder)
  {
    if(auto state = weak.lock())
    {
      const float cross = state->wrapCross ? WRAP_CONTENT : MATCH_PARENT;
      holder.view.SetRequestedWidth(state->horizontal ? state->extents.at(holder.position) : cross);
      holder.view.SetRequestedHeight(state->horizontal ? cross : state->extents.at(holder.position));
      holder.view.SetProperty(Actor::Property::NAME, Dali::String(Id("window.item", holder.position).c_str()));
      holder.view.SetBackgroundColor(FixtureColor(holder.position));
      state->bound[holder.position] = holder.view;
    }
  });
  s->adapter.RecycleViewHolderSignal().Connect(&r, [weak](ItemViewHolder& holder)
  {
    if(auto state = weak.lock())
    {
      ++state->recycles;
      auto found = state->bound.find(holder.position);
      if(found != state->bound.end() && found->second == holder.view) state->bound.erase(found);
    }
  });
  s->layouter = LinearItemsLayouter::New(horizontal ? ItemsLayouter::Orientation::HORIZONTAL : ItemsLayouter::Orientation::VERTICAL);
  s->layouter.SetItemExtent(20);
  s->layouter.SetItemSpacing(s->spacing);
  s->view = RecyclerView::New();
  s->view.SetRequestedWidth(horizontal ? viewport : s->cross);
  s->view.SetRequestedHeight(horizontal ? s->cross : viewport);
  s->view.SetScrollOnFocus(false);
  s->view.SetItemsLayouter(s->layouter);
  s->view.SetAdapter(s->adapter);
  s->view.SetCacheExtent(0, 0);
  if(s->decoration) s->view.AddItemDecoration(*s->decoration);
  // RecyclerView creates its scroller first and keeps it below the scrollbar.
  s->scroller = s->view.GetChildCount() ? View::DownCast(s->view.GetChildAt(0)) : View{};
  s->stage    = r.Stage(200, 160);
  // RecyclerView consumes its incoming constraint. Give it an explicit layout slot;
  // requested dimensions alone do not constrain a child of the default View producer.
  auto viewportHost = AbsoluteLayout::New();
  viewportHost.SetRequestedWidth(200);
  viewportHost.SetRequestedHeight(160);
  s->view.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds({0, 0, horizontal ? viewport : s->cross, horizontal ? s->cross : viewport}));
  viewportHost.Add(s->view);
  s->stage.Add(viewportHost);
  r.OnCleanup([s]()
  {
    if(s->decoration) s->view.RemoveItemDecoration(*s->decoration);
    s->view.ClearAdapter();
  });
  r.SetState(s);
  return s;
}

// 23 rows. The loop examines every expected materialized item, with update-thread local
// properties for cached off-screen items and screen extents for the visible batch only.
inline void CheckWindowRecycler(Run& r, const WindowRecyclerFixture& s, float offset)
{
  const auto       active = s.Active(offset);
  const LayoutRect viewport(0, 0, s.horizontal ? s.viewport : s.cross, s.horizontal ? s.cross : s.viewport);
  r.Truth("window.connected", s.view.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE));
  r.Rendered("window.viewport", s.view, viewport);
  r.Equal("window.catalog-count", s.adapter.GetItemCount(), s.extents.size());
  r.Near("window.range", s.layouter.ComputeScrollRange(), s.Range());
  r.Near("window.offset", s.view.GetScrollOffset(), offset);
  r.Near("window.layouter-offset", s.layouter.ComputeScrollOffset(), offset);
  r.Near("window.extent", s.layouter.ComputeScrollExtent(), s.viewport);
  r.Equal("window.first", s.view.GetFirstVisiblePosition(), active.empty() ? 0u : active.front());
  r.Equal("window.last", s.view.GetLastVisiblePosition(), active.empty() ? 0u : active.back());
  r.Equal("window.materialized-count", s.bound.size(), active.size());
  std::vector<uint32_t> observed;
  for(const auto& entry : s.bound) observed.push_back(entry.first);
  r.Truth("window.materialized-identities", observed == active);
  bool     stableIds       = true;
  uint32_t localMismatches = 0, visibleMismatches = 0, visibleChecked = 0, visibleRequired = 0;
  double   maximumError = 0;
  auto     compare      = [&maximumError](const LayoutRect& actual, const LayoutRect& expected, uint32_t& mismatches)
  {
    const float a[] = {actual.x, actual.y, actual.width, actual.height};
    const float e[] = {expected.x, expected.y, expected.width, expected.height};
    for(unsigned component = 0; component < 4; ++component)
    {
      const double error = std::abs(static_cast<double>(a[component]) - e[component]);
      if(!std::isfinite(error) || error > GEOMETRY_TOLERANCE) ++mismatches;
      maximumError = std::isfinite(error) ? std::max(maximumError, error) : std::numeric_limits<double>::infinity();
    }
  };
  for(uint32_t index : active)
  {
    const auto  expected = s.ItemRect(index);
    const float start    = s.horizontal ? expected.x : expected.y;
    const float extent   = s.horizontal ? expected.width : expected.height;
    const bool  visible  = start + extent > offset && start < offset + s.viewport;
    if(visible) ++visibleRequired;
    const auto found = s.bound.find(index);
    if(found == s.bound.end())
    {
      stableIds = false;
      ++localMismatches;
      if(visible) ++visibleMismatches;
      continue;
    }
    View item = found->second;
    stableIds &= item.GetProperty<Dali::String>(Actor::Property::NAME) == Dali::String(Id("window.item", index).c_str());
    const auto position = item.GetCurrentProperty<Vector3>(Actor::Property::POSITION);
    const auto size     = item.GetCurrentProperty<Vector3>(Actor::Property::SIZE);
    compare({position.x, position.y, size.x, size.y}, expected, localMismatches);
    if(visible)
    {
      auto screenExpected = expected;
      if(s.horizontal)
        screenExpected.x -= offset;
      else
        screenExpected.y -= offset;
      compare(Run::RenderedBounds(item, s.view), screenExpected, visibleMismatches);
      ++visibleChecked;
    }
  }
  r.Truth("window.stable-ids", stableIds);
  r.Equal("window.current-local-mismatches", localMismatches, 0);
  r.Equal("window.visible-render-mismatches", visibleMismatches, 0);
  r.Equal("window.visible-checked", visibleChecked, visibleRequired);
  r.Near("window.maximum-error", maximumError, 0);
  r.Truth("window.scroller-present", static_cast<bool>(s.scroller));
  const float invalid  = std::numeric_limits<float>::quiet_NaN();
  const auto  position = s.scroller ? s.scroller.GetCurrentProperty<Vector3>(Actor::Property::POSITION) : Vector3(invalid, invalid, invalid);
  r.Near("window.scroller-current-x", position.x, s.horizontal ? -offset : 0);
  r.Near("window.scroller-current-y", position.y, s.horizontal ? 0 : -offset);
  r.Truth("window.scroll-idle", !s.view.IsScrolling());
}

inline void ObserveWindowRecycler(Run& r, const std::shared_ptr<WindowRecyclerFixture>& s, float offset, bool processLayout = false)
{
  if(processLayout) LayoutController::Get(r.GetWindow()).ProcessLayouts();
  r.AfterFrame([s, offset](Run& done)
  { CheckWindowRecycler(done, *s, offset); }, {}, s->stage);
}

} // namespace LayoutValidation
