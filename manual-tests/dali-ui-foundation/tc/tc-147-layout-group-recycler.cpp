/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include "layout-validation-fixtures.h"
#include "layout-validation-recycler-fixtures.h"
using namespace LayoutValidation;
namespace
{

struct GroupWindowFixture
{
  std::shared_ptr<Groups>                       source;
  GroupAdapter                                  group;
  ItemAdapter                                   inner;
  RecyclerView                                  view;
  View                                          stage; ///< On-screen 120x160 stage; the recycler fills its constraint.
  GroupLinearItemsLayouter                      layouter;
  std::map<uint32_t, View>                      rows;
  std::vector<std::shared_ptr<Fixtures::Probe>> bodies;
  uint32_t                                      invalidations{0};
  uint32_t                                      BodyMeasures() const
  {
    uint32_t count = 0;
    for(const auto& body : bodies) count += body->measures;
    return count;
  }
};
std::shared_ptr<GroupWindowFixture> MakeGroupWindowFixture(Run& r)
{
  auto s             = std::make_shared<GroupWindowFixture>();
  s->source          = std::make_shared<Groups>();
  s->source->counts  = {1, 1};
  s->source->headers = {true, true};
  s->group           = GroupAdapter::New();
  s->group.SetDataSource(s->source);
  s->group.SetGapHeight(7);
  s->inner = ItemAdapter::New();
  s->inner.GetItemCountSignal().Connect(&r, []()
  { return 2u; });
  s->inner.CreateViewHolderSignal().Connect(&r, [s, &r](ItemViewHolder& holder)
  {
    auto body      = Fixtures::Probed(r, 100, 20, false);
    body->wrapping = true;
    body->view.SetRequestedWidth(MATCH_PARENT);
    holder.view = body->view;
    s->bodies.push_back(body);
  });
  s->group.SetInnerAdapter(s->inner);
  s->group.CreateHeaderViewHolderSignal().Connect(&r, [](ItemViewHolder& holder)
  { holder.view = Leaf(MATCH_PARENT, 11); });
  auto flat = s->group.GetAdapter();
  flat.BindViewHolderSignal().Connect(&r, [s](ItemViewHolder& holder)
  { s->rows[holder.position] = holder.view; });
  flat.RecycleViewHolderSignal().Connect(&r, [s](ItemViewHolder& holder)
  {
    const auto found = s->rows.find(holder.position);
    if(found != s->rows.end() && found->second == holder.view) s->rows.erase(found);
  });
  s->layouter = GroupLinearItemsLayouter::New();
  s->layouter.SetGroupAdapter(s->group);
  s->layouter.SetItemExtent(20);
  s->layouter.SetItemSpacing(3);
  s->layouter.SetBodyHorizontalMargin(5, 9);
  s->view = RecyclerView::New();
  s->view.SetRequestedWidth(120);
  s->view.SetRequestedHeight(160);
  s->view.SetCacheExtent(0, 0);
  s->view.SetItemsLayouter(s->layouter);
  s->view.SetAdapter(flat);
  s->layouter.GetImpl().LayoutInvalidatedSignal().Connect(&r, [s]()
  { ++s->invalidations; });
  return s;
}
// Twenty-six observations. The geometry oracle uses only fixture inputs.
void CheckGroupRows(Run& r, const GroupWindowFixture& s, float left, float right, float bodyHeight)
{
  r.Equal("actual.row.count", s.rows.size(), 5);
  r.Near("actual.range", s.layouter.ComputeScrollRange(), 41 + 2 * bodyHeight);
  r.Near("margin.left", s.layouter.GetBodyMarginLeft(), left);
  r.Near("margin.right", s.layouter.GetBodyMarginRight(), right);
  const float heights[] = {11, bodyHeight, 7, 11, bodyHeight};
  float       y         = 0;
  for(uint32_t i = 0; i < 5; ++i)
  {
    const bool body = i == 1 || i == 4;
    const auto row  = s.rows.find(i);
    if(row == s.rows.end())
    {
      r.Fail(Id("row.missing", i).c_str(), "actual RecyclerView did not materialize the required row");
      continue;
    }
    r.Rect(Id("actual.row", i).c_str(), row->second, LayoutRect(body ? left : 0, y, body ? std::max(0.f, 120 - left - right) : 120, heights[i]));
    if(body)
    {
      auto probe = std::find_if(s.bodies.begin(), s.bodies.end(), [&](const auto& item)
      { return item->view == row->second; });
      if(probe == s.bodies.end())
        r.Fail(Id("body.producer.missing", i).c_str(), "required body producer is absent");
      else
        r.Near(Id("body.measure.width", i).c_str(), (*probe)->lastWidth, std::max(0.f, 120 - left - right));
    }
    y += heights[i] + 3;
  }
}

class Tc147 : public Case
{
public:
  Tc147()
  : Case("TC147", "Grouped item layout", "Check grouped item rows and body re-measure")
  {
  }
  std::vector<Scenario> BuildScenarios() override
  {
    return {Single("vertical-group-matrix", "Header/gap/single/top/middle/bottom rows, inset clamp and data rebuild", 122, [](Run& r)
    {
      auto source = std::make_shared<Groups>();
      auto group  = GroupAdapter::New();
      group.SetDataSource(source);
      group.SetGapHeight(7);
      auto inner = ItemAdapter::New();
      inner.GetItemCountSignal().Connect(&r, []()
      { return 6u; });
      inner.CreateViewHolderSignal().Connect(&r, [](ItemViewHolder& holder)
      { holder.view = Leaf(MATCH_PARENT, 20); });
      inner.BindViewHolderSignal().Connect(&r, [](ItemViewHolder& holder)
      { holder.view.SetRequestedHeight(20); });
      group.SetInnerAdapter(inner);
      group.CreateHeaderViewHolderSignal().Connect(&r, [](ItemViewHolder& holder)
      { holder.view = Leaf(MATCH_PARENT, 11); });
      auto               flat      = group.GetAdapter();
      const GroupRowType types[]   = {GroupRowType::HEADER, GroupRowType::BODY_SINGLE, GroupRowType::GAP, GroupRowType::BODY_TOP, GroupRowType::BODY_MIDDLE, GroupRowType::BODY_BOTTOM, GroupRowType::GAP, GroupRowType::HEADER, GroupRowType::GAP, GroupRowType::BODY_TOP, GroupRowType::BODY_BOTTOM};
      const uint32_t     groups[]  = {0, 0, 1, 1, 1, 1, 2, 2, 3, 3, 3};
      const float        heights[] = {11, 20, 7, 20, 20, 20, 7, 11, 7, 20, 20};
      r.Equal("flat.count", flat.GetItemCount(), 11);
      FixedRecycler fixture(0);
      fixture.viewport = 1000;
      std::vector<ItemViewHolder> holders;
      for(uint32_t i = 0; i < 11; ++i)
      {
        ItemViewHolder holder;
        holder.position = i;
        holder.viewType = flat.GetItemViewType(i);
        flat.CreateViewHolder(holder);
        flat.BindViewHolder(holder);
        r.Equal(Id("holder.position", i).c_str(), holder.position, i);
        r.Equal(Id("holder.type", i).c_str(), static_cast<int>(holder.rowType), static_cast<int>(types[i]));
        r.Equal(Id("holder.group", i).c_str(), holder.groupIndex, groups[i]);
        r.Near(Id("holder.height", i).c_str(), holder.view.GetRequestedHeight(), heights[i]);
        fixture.items.push_back(holder.view);
        holders.push_back(holder);
      }
      fixture.active.resize(11, false);
      auto layouter = GroupLinearItemsLayouter::New();
      layouter.SetGroupAdapter(group);
      layouter.SetBodyHorizontalMargin(5, 9);
      layouter.SetItemExtent(20);
      layouter.SetItemSpacing(3);
      layouter.GetImpl().OnLayoutChildren(fixture);
      float cursor = 0;
      for(uint32_t i = 0; i < 11; ++i)
      {
        const bool body = types[i] != GroupRowType::GAP && types[i] != GroupRowType::HEADER;
        r.Rect(Id("group.slot", i).c_str(), fixture.items[i], LayoutRect(body ? 5 : 0, cursor, body ? 106 : 120, heights[i]));
        cursor += heights[i] + 3;
      }
      r.Near("group.range", layouter.ComputeScrollRange(), 193);
      layouter.SetBodyHorizontalMargin(80, 80);
      for(uint32_t i : {1u, 3u, 4u, 5u, 9u, 10u})
      {
        float y = 0;
        for(uint32_t j = 0; j < i; ++j) y += heights[j] + 3;
        r.Rect(Id("inset.clamp", i).c_str(), layouter.GetItemBounds(i, 120), LayoutRect(80, y, 0, heights[i]));
      }
      layouter.SetBodyHorizontalMargin(-7, -9);
      r.Near("inset.left.clamp", layouter.GetBodyMarginLeft(), 0);
      r.Near("inset.right.clamp", layouter.GetBodyMarginRight(), 0);
      auto plain = GroupLinearItemsLayouter::New();
      plain.SetItemExtent(20);
      plain.SetBodyHorizontalMargin(5, 9);
      r.Rect("without.group", plain.GetItemBounds(2, 120), LayoutRect(0, 40, 120, 20));
      for(auto& holder : holders) flat.RecycleViewHolder(holder);
      source->counts  = {0};
      source->headers = {false};
      group.NotifyDataSetChanged();
      r.Equal("rebuild.empty", flat.GetItemCount(), 0);
      group.ClearDataSource();
      r.Equal("clear.empty", flat.GetItemCount(), 0);
    }),
            {"window-group-remeasure", "Actual GroupAdapter RecyclerView width-dependent body remeasurement and invalidation", {{"attach", 30, [](Run& r)
    {
      auto s = MakeGroupWindowFixture(r);
      r.SetState(s);
      r.AfterLayout({s->view}, [s](Run& done)
      {
        done.Rect("window.recycler", done.Snapshot(s->view), LayoutRect(0, 0, 120, 160));
        CheckGroupRows(done, *s, 5, 9, 20);
      });
      // A RecyclerView fills the constraint it is given, so it is mounted under a stage of
      // the intended 120x160 size instead of directly under the window-sized host.
      s->stage = Fixtures::Mount(r, s->view, 120, 160);
    }},
                                                                                                                                {"narrow-body", 28, [](Run& r)
    {
      auto       s       = r.State<GroupWindowFixture>();
      const auto signals = s->invalidations, measures = s->BodyMeasures();
      s->layouter.SetBodyHorizontalMargin(20, 20);
      r.Equal("margin.invalidation.delta", s->invalidations - signals, 1);
      r.Truth("margin.body.producer.ran", s->BodyMeasures() > measures);
      CheckGroupRows(r, *s, 20, 20, 40);
    }},
                                                                                                                                {"narrow-window-fence", 30, [](Run& r)
    {
      auto s = r.State<GroupWindowFixture>();
      r.AfterLayout({s->view}, [s](Run& done)
      {
        done.Rect("settled.recycler", done.Snapshot(s->view), LayoutRect(0, 0, 120, 160));
        CheckGroupRows(done, *s, 20, 20, 40);
      });
      s->view.InvalidateArrange();
    }},
                                                                                                                                {"restore-body", 28, [](Run& r)
    {
      auto       s       = r.State<GroupWindowFixture>();
      const auto signals = s->invalidations, measures = s->BodyMeasures();
      s->layouter.SetBodyHorizontalMargin(5, 9);
      r.Equal("restore.invalidation.delta", s->invalidations - signals, 1);
      r.Truth("restore.body.producer.ran", s->BodyMeasures() > measures);
      CheckGroupRows(r, *s, 5, 9, 20);
    }}}}};
  }
};
} //namespace
REGISTER_MANUAL_TEST(Tc147)
