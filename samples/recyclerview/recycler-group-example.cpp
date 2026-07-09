/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>
#include <dali-ui-foundation/public-api/views/effects/overlay-effect.h>
#include <dali/dali.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------
namespace
{
constexpr float WINDOW_W    = 600.0f;
constexpr float WINDOW_H    = 900.0f;
constexpr float HEADER_H    = 100.0f;
constexpr float ITEM_H      = 56.0f;   // body item height
constexpr float SECTION_H   = 40.0f;   // group header height
constexpr float BODY_MARGIN = 14.0f;   // horizontal card inset (each side)
constexpr float CARD_RADIUS = 10.0f;   // group body card corner radius
constexpr float ITEM_RADIUS = 10.0f;   // body item corner radius (same as card)
constexpr float GAP_H       = 20.0f;   // spacing between groups

// Body item view types (inner adapter):
constexpr uint32_t BODY_TYPE_A = 0u;
constexpr uint32_t BODY_TYPE_B = 1u;

// Colour palette
const UiColor COLOR_BG(0xEEF0F5);        // window background
const UiColor COLOR_HEADER_BG(0x1C2A42); // app header bar
const UiColor COLOR_TEXT_DARK(0x1A2332); // primary text
const UiColor COLOR_TEXT_MUTED(0x8899AA);// secondary / subtext
const UiColor COLOR_CARD(0xFFFFFF);      // group card background
const UiColor COLOR_STAT(0x7EC8E3);      // stats label in header

// ---- Group data ------------------------------------------------------------

struct GroupInfo
{
  const char* name;
  uint32_t    itemCount;
  bool        hasHeader;
};

static const GroupInfo GROUPS[] = {
  {"Accounts",          4u, true},
  {"Notifications",     3u, true},
  {"Display",           5u, true},
  {"Sound & Vibration", 2u, true},
  {"Privacy",           4u, false}, // no header — just card
  {"Storage",           3u, true},
  {"Advanced",          2u, true},
  {"About",             1u, false},
};
constexpr uint32_t GROUP_COUNT = static_cast<uint32_t>(std::size(GROUPS));

static const char* ITEM_NAMES[][5] = {
  // Accounts (4)
  {"Google Account", "Samsung Account", "Add Account", "Backup & Restore"},
  // Notifications (3)
  {"App Notifications", "Do Not Disturb", "Notification History"},
  // Display (5)
  {"Brightness", "Dark Mode", "Font Size", "Screen Timeout", "Adaptive Brightness"},
  // Sound & Vibration (2)
  {"Volume", "Ringtone"},
  // Privacy (4)
  {"Permissions", "Location", "Microphone", "Camera"},
  // Storage (3)
  {"Internal Storage", "SD Card", "USB Storage"},
  // Advanced (2)
  {"Developer Options", "OEM Unlocking"},
  // About (1)
  {"Software Information"},
};

// Map innerPosition → (groupIndex, localIndex)
struct InnerPos
{
  uint32_t group;
  uint32_t local;
};
InnerPos ResolveInner(uint32_t innerPos)
{
  uint32_t rem = innerPos;
  for(uint32_t g = 0u; g < GROUP_COUNT; ++g)
  {
    if(rem < GROUPS[g].itemCount) return {g, rem};
    rem -= GROUPS[g].itemCount;
  }
  return {GROUP_COUNT - 1u, 0u};
}

} // namespace

// ---------------------------------------------------------------------------
// GroupItemDataSource
// ---------------------------------------------------------------------------
class GroupItemDataSource : public GroupDataSource
{
public:
  uint32_t GetGroupCount() const override
  {
    return GROUP_COUNT;
  }
  uint32_t GetGroupItemCount(uint32_t groupIndex) const override
  {
    return (groupIndex < GROUP_COUNT) ? GROUPS[groupIndex].itemCount : 0u;
  }
  bool HasGroupHeader(uint32_t groupIndex) const override
  {
    return (groupIndex < GROUP_COUNT) && GROUPS[groupIndex].hasHeader;
  }
};

// ---------------------------------------------------------------------------
// BodyItemAdapter — handles BODY items only (inner adapter for GroupAdapter)
// ---------------------------------------------------------------------------
class BodyItemAdapter : public ConnectionTracker
{
public:
  BodyItemAdapter()
  : mCreateCount(0u),
    mBindCount(0u),
    mRecycleCount(0u)
  {
  }

  void Attach(ItemAdapter adapter)
  {
    adapter.GetItemCountSignal().Connect(this, &BodyItemAdapter::OnGetItemCount);
    adapter.GetItemViewTypeSignal().Connect(this, &BodyItemAdapter::OnGetItemViewType);
    adapter.CreateViewHolderSignal().Connect(this, &BodyItemAdapter::OnCreateViewHolder);
    adapter.BindViewHolderSignal().Connect(this, &BodyItemAdapter::OnBindViewHolder);
    adapter.RecycleViewHolderSignal().Connect(this, &BodyItemAdapter::OnRecycleViewHolder);
  }

  uint32_t CreateCount() const { return mCreateCount; }
  uint32_t BindCount() const { return mBindCount; }
  uint32_t RecycleCount() const { return mRecycleCount; }

private:
  // Count of all body items across all groups.
  uint32_t OnGetItemCount()
  {
    uint32_t total = 0u;
    for(uint32_t g = 0u; g < GROUP_COUNT; ++g) total += GROUPS[g].itemCount;
    return total;
  }

  // Alternate between type A and type B based on innerPosition parity.
  uint32_t OnGetItemViewType(uint32_t innerPos)
  {
    return (innerPos % 2u == 0u) ? BODY_TYPE_A : BODY_TYPE_B;
  }

  void OnCreateViewHolder(ItemViewHolder& holder)
  {
    ++mCreateCount;

    StackLayout row = StackLayout::New(StackOrientation::VERTICAL);
    row.SetRequestedWidth(WINDOW_W - 2.0f * BODY_MARGIN);
    row.SetRequestedHeight(ITEM_H);
    row.SetBackgroundColor(COLOR_CARD);
    row.AsInteractive();
    row.SetStateEffect(OverlayEffect::ListItem());
    row.SetFocusable(true);

    Label label = Label::New();
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(MATCH_PARENT);
    label.SetTextColor(COLOR_TEXT_DARK);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    row.Add(label);

    holder.view = row;
    ApplyCornerRadius(row, holder.rowType);
  }

  void OnBindViewHolder(ItemViewHolder& holder)
  {
    ++mBindCount;

    Label label = Label::DownCast(holder.view.GetChildAt(0));
    if(!label) return;

    ApplyCornerRadius(holder.view, holder.rowType);

    const InnerPos ip = ResolveInner(holder.position);
    if(ip.group < GROUP_COUNT && ip.local < GROUPS[ip.group].itemCount)
    {
      std::ostringstream oss;
      oss << "  " << ITEM_NAMES[ip.group][ip.local];
      label.SetText(Dali::String(oss.str().c_str()));
    }
  }

  void OnRecycleViewHolder(ItemViewHolder& holder)
  {
    ++mRecycleCount;
  }

  void ApplyCornerRadius(View row, GroupRowType rowType)
  {
    float tl = 0.0f, tr = 0.0f, bl = 0.0f, br = 0.0f;
    switch(rowType)
    {
      case GroupRowType::BODY_SINGLE:
        tl = tr = bl = br = ITEM_RADIUS;
        break;
      case GroupRowType::BODY_TOP:
        tl = tr = ITEM_RADIUS;
        break;
      case GroupRowType::BODY_BOTTOM:
        bl = br = ITEM_RADIUS;
        break;
      default:
        break;
    }
    row.SetCornerRadius(tl, tr, br, bl);
  }

  uint32_t mCreateCount;
  uint32_t mBindCount;
  uint32_t mRecycleCount;
};

// ---------------------------------------------------------------------------
// HeaderAdapter — handles HEADER items via GroupAdapter's header signals
// ---------------------------------------------------------------------------
class HeaderAdapter : public ConnectionTracker
{
public:
  void Attach(GroupAdapter groupAdapter)
  {
    groupAdapter.CreateHeaderViewHolderSignal().Connect(this, &HeaderAdapter::OnCreateHeader);
    groupAdapter.BindHeaderViewHolderSignal().Connect(this, &HeaderAdapter::OnBindHeader);
  }

private:
  void OnCreateHeader(ItemViewHolder& holder)
  {
    Label header = Label::New();
    header.SetRequestedWidth(WINDOW_W);
    header.SetRequestedHeight(SECTION_H);
    header.SetTextColor(COLOR_TEXT_DARK);

    holder.view = header;
    BindGroupName(header, holder.groupIndex);
  }

  void OnBindHeader(ItemViewHolder& holder)
  {
    Label header = Label::DownCast(holder.view);
    if(header) BindGroupName(header, holder.groupIndex);
  }

  void BindGroupName(Label& header, uint32_t groupIndex)
  {
    if(groupIndex >= GROUP_COUNT) return;
    std::string text = std::string("  ") + GROUPS[groupIndex].name;
    header.SetText(Dali::String(text.c_str()));
  }
};

// ---------------------------------------------------------------------------
// RecyclerGroupController
// ---------------------------------------------------------------------------
class RecyclerGroupController : public ConnectionTracker
{
public:
  explicit RecyclerGroupController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &RecyclerGroupController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    auto   ps     = window.GetPositionSize();
    window.SetPositionSize(PositionSize(ps.x, ps.y,
                                       static_cast<uint32_t>(WINDOW_W),
                                       static_cast<uint32_t>(WINDOW_H)));
    window.SetBackgroundColor(COLOR_BG);

    BuildHeader(window);
    BuildRecyclerView(window);

    FocusManager::Get().FocusChangedSignal().Connect(this, &RecyclerGroupController::OnFocusChanged);

    mRangeTimer = Timer::New(100u);
    mRangeTimer.TickSignal().Connect(this, &RecyclerGroupController::OnRangeTimerTick);
    mRangeTimer.Start();
  }

private:
  // ---- App header bar --------------------------------------------------------
  void BuildHeader(Window& window)
  {
    View bar = View::New();
    bar.SetRequestedWidth(WINDOW_W);
    bar.SetRequestedHeight(HEADER_H);
    bar.SetBackgroundColor(COLOR_HEADER_BG);

    Label title = Label::New("RecyclerView  |  Group + Decoration sample");
    title.SetRequestedWidth(WINDOW_W - 24.0f);
    title.SetRequestedHeight(28.0f);
    title.SetRequestedX(12.0f);
    title.SetRequestedY(8.0f);
    title.SetTextColor(UiColor(0xFFFFFF));
    bar.Add(title);

    Label hint = Label::New("6 groups with header, 2 without header. Card bg drawn by GroupBodyDecoration.");
    hint.SetRequestedWidth(WINDOW_W - 24.0f);
    hint.SetRequestedHeight(24.0f);
    hint.SetRequestedX(12.0f);
    hint.SetRequestedY(40.0f);
    hint.SetTextColor(COLOR_TEXT_MUTED);
    bar.Add(hint);

    mStatsLabel = Label::New("--");
    mStatsLabel.SetRequestedWidth(WINDOW_W - 24.0f);
    mStatsLabel.SetRequestedHeight(24.0f);
    mStatsLabel.SetRequestedX(12.0f);
    mStatsLabel.SetRequestedY(68.0f);
    mStatsLabel.SetTextColor(COLOR_STAT);
    bar.Add(mStatsLabel);

    window.Add(bar);
  }

  // ---- RecyclerView setup ----------------------------------------------------
  void BuildRecyclerView(Window& window)
  {
    // Data source and adapters.
    mGroupDataSource = std::make_shared<GroupItemDataSource>();
    mGroupAdapter    = GroupAdapter::New();
    mInnerAdapter    = ItemAdapter::New();
    mGroupAdapter.SetDataSource(mGroupDataSource);
    mGroupAdapter.SetGapHeight(GAP_H);

    mBodyAdapter.Attach(mInnerAdapter);
    mGroupAdapter.SetInnerAdapter(mInnerAdapter);

    mHeaderAdapter.Attach(mGroupAdapter);

    // Layouter.
    mLayouter = GroupLinearItemsLayouter::New(ItemsLayouter::Orientation::VERTICAL);
    mLayouter.SetGroupAdapter(mGroupAdapter);
    mLayouter.SetItemExtent(ITEM_H);
    mLayouter.SetItemSpacing(0.0f);

    // GroupBodyDecoration: card behind body items.
    // SetBodyHorizontalMargin drives both the item offset (via GetItemOffsets) and
    // the card width — do NOT also call mLayouter.SetBodyHorizontalMargin.
    mBodyDecoration.SetBodyColor(COLOR_CARD);
    mBodyDecoration.SetBodyHorizontalMargin(BODY_MARGIN, BODY_MARGIN);
    mBodyDecoration.SetBodyCornerRadius(CARD_RADIUS);

    // RecyclerView.
    mRecyclerView = RecyclerView::New();
    mRecyclerView.SetRequestedWidth(WINDOW_W);
    mRecyclerView.SetRequestedHeight(WINDOW_H - HEADER_H);
    mRecyclerView.SetRequestedY(HEADER_H);
    mRecyclerView.SetCacheExtent(ITEM_H * 2.0f, ITEM_H * 2.0f);
    mRecyclerView.SetItemsLayouter(mLayouter);
    mRecyclerView.SetAdapter(mGroupAdapter.GetAdapter());
    mRecyclerView.AddItemDecoration(mBodyDecoration);

    mRecyclerView.SetKeyScrollEnabled(true);
    mRecyclerView.SetKeyScrollStep(ITEM_H * 2.0f);
    mRecyclerView.SetFocusScrollPeek(ITEM_H * 0.5f);
    mRecyclerView.SetFocusable(true);

    mRecyclerView.ScrollStartedSignal().Connect(this, &RecyclerGroupController::OnScrollStateChange);
    mRecyclerView.ScrollFinishedSignal().Connect(this, &RecyclerGroupController::OnScrollStateChange);

    window.Add(mRecyclerView);
  }

  // ---- Focus change ----------------------------------------------------------
  void OnFocusChanged(View /*from*/, View to)
  {
    UpdateStats();
  }

  // ---- Scroll state ----------------------------------------------------------
  void OnScrollStateChange(RecyclerView)
  {
    UpdateStats();
  }

  // ---- Range timer -----------------------------------------------------------
  bool OnRangeTimerTick()
  {
    UpdateStats();
    return true;
  }

  void UpdateStats()
  {
    if(!mStatsLabel || !mRecyclerView) return;

    const uint32_t first = mRecyclerView.GetFirstVisiblePosition();
    const uint32_t last  = mRecyclerView.GetLastVisiblePosition();

    std::ostringstream oss;
    oss << "Visible flat positions: " << first << " – " << last
        << "   Created:" << mBodyAdapter.CreateCount()
        << "  Bind:" << mBodyAdapter.BindCount()
        << "  Recycled:" << mBodyAdapter.RecycleCount()
        << (mRecyclerView.IsScrolling() ? "   [scrolling]" : "");

    mStatsLabel.SetText(Dali::String(oss.str().c_str()));
  }

private:
  Application mApplication;

  std::shared_ptr<GroupItemDataSource> mGroupDataSource;

  GroupAdapter    mGroupAdapter;
  ItemAdapter     mInnerAdapter; // initialized via ItemAdapter::New() in BuildRecyclerView
  BodyItemAdapter mBodyAdapter;
  HeaderAdapter   mHeaderAdapter;

  GroupLinearItemsLayouter mLayouter;
  GroupBodyDecoration      mBodyDecoration;
  RecyclerView             mRecyclerView;

  Label mStatsLabel;
  Timer mRangeTimer;
};

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int DALI_EXPORT_API main(int argc, char** argv)
{
  Application             app    = Application::New(&argc, &argv);
  UiConfig                config = UiConfig::New();
  config.Apply();

  RecyclerGroupController controller(app);
  app.MainLoop();
  return 0;
}
