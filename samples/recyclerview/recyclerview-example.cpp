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
 *
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>
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
constexpr float    WINDOW_W    = 600.0f;
constexpr float    WINDOW_H    = 900.0f;
constexpr float    HEADER_H    = 148.0f;
constexpr float    ROW_H       = 82.0f;
constexpr float    ROW_SPACING = 8.0f;
constexpr uint32_t ITEM_COUNT  = 200u;
constexpr uint32_t INVALID_POS = std::numeric_limits<uint32_t>::max();

// Colour palette
const UiColor COLOR_BACKGROUND(0xF5F7FA);
const UiColor COLOR_HEADER(0x172033);
const UiColor COLOR_ROW_A(0xE7EEF8);        // focusable — blue tint
const UiColor COLOR_ROW_B(0xEAF6EC);        // focusable — green tint
const UiColor COLOR_ROW_C(0xF7EFE3);        // focusable — orange tint
const UiColor COLOR_ROW_DISABLED(0xDDE3EC);  // unfocusable — grey
const UiColor COLOR_TEXT(0x101828);
const UiColor COLOR_TEXT_DISABLED(0x94A3B8); // muted grey for unfocusable
const UiColor COLOR_WHITE(0xFFFFFF);
const UiColor COLOR_GREEN_BRIGHT(0xA3E635);
const UiColor COLOR_SLATE(0xCBD5E1);

// ---------------------------------------------------------------------------
// Focusability rule:
//   positions 0-7  in each block of 10 → FOCUSABLE   (8 items)
//   positions 8-9  in each block of 10 → UNFOCUSABLE (2 items, consecutive gap)
// This tests: single-gap skip, double-gap skip, long runs, boundary handling.
// ---------------------------------------------------------------------------
bool IsFocusable(uint32_t position)
{
  return (position % 10u) < 8u;
}

// View types: 0-2 for focusable (different colours), 3 for unfocusable.
uint32_t ViewTypeFor(uint32_t position)
{
  return IsFocusable(position) ? (position % 3u) : 3u;
}

UiColor NormalBgForType(uint32_t vtype)
{
  switch(vtype)
  {
    case 1u: return COLOR_ROW_B;
    case 2u: return COLOR_ROW_C;
    case 3u: return COLOR_ROW_DISABLED;
    default: return COLOR_ROW_A;
  }
}

float GetItemHeight(uint32_t position)
{
  switch(position % 5u)
  {
    case 1u: return 58.0f;
    case 2u: return 92.0f;
    case 3u: return 118.0f;
    case 4u: return 76.0f;
    default: return 68.0f;
  }
}

Dali::String BuildRowText(uint32_t viewSlot, uint32_t position, float height)
{
  std::ostringstream oss;
  if(!IsFocusable(position))
  {
    oss << "[NO FOCUS]  ";
  }
  oss << "slot #" << viewSlot
      << "   item #" << position
      << "   h:" << static_cast<int>(height)
      << "   " << (IsFocusable(position) ? "focusable" : "unfocusable");
  return Dali::String(oss.str().c_str());
}

} // namespace

// ---------------------------------------------------------------------------
// RecyclingTextAdapter
//
// Maintains a per-created-view record so that focus highlight changes can be
// applied to the right Label even while the RecyclerView is rebinding views.
// ---------------------------------------------------------------------------
class RecyclingTextAdapter : public ConnectionTracker
{
public:
  RecyclingTextAdapter()
  : mCreateCount(0u),
    mBindCount(0u),
    mRecycleCount(0u),
    mFirstVisible(0u),
    mLastVisible(0u),
    mFocusedPosition(INVALID_POS)
  {
  }

  void SetStatsLabel(Label label) { mStatsLabel = label; }

  void Attach(ItemAdapter& adapter)
  {
    adapter.GetItemCountSignal().Connect(this, &RecyclingTextAdapter::GetItemCount);
    adapter.GetItemViewTypeSignal().Connect(this, &RecyclingTextAdapter::GetItemViewType);
    adapter.CreateItemViewSignal().Connect(this, &RecyclingTextAdapter::CreateItemView);
    adapter.BindItemViewSignal().Connect(this, &RecyclingTextAdapter::BindItemView);
    adapter.ItemViewRecycledSignal().Connect(this, &RecyclingTextAdapter::ItemViewRecycled);
  }

  void SetRange(uint32_t first, uint32_t last)
  {
    mFirstVisible = first;
    mLastVisible  = last;
    UpdateStatsText();
  }

  // Called by the controller when the global focus changes.
  void OnFocusChanged(View from, View to)
  {
    Label toLabel = FindTrackedLabel(to);
    if(toLabel)
    {
      mFocusedPosition = PositionOf(toLabel);
      if(mFocusedPosition != INVALID_POS)
      {
        DALI_LOG_RELEASE_INFO("RecyclerView focus → item #%u (%s)\n",
                              mFocusedPosition,
                              IsFocusable(mFocusedPosition) ? "focusable" : "unfocusable");
      }
    }
    else
    {
      mFocusedPosition = INVALID_POS;
    }

    UpdateStatsText();
  }

  uint32_t GetFocusedPosition() const { return mFocusedPosition; }

private:
  // Per-view tracking: associates each created Label with its current data position.
  struct ViewRecord
  {
    Label    label;
    uint32_t position{INVALID_POS};
  };

  uint32_t GetItemCount() { return ITEM_COUNT; }

  uint32_t GetItemViewType(uint32_t position) { return ViewTypeFor(position); }

  View CreateItemView(uint32_t viewType)
  {
    ++mCreateCount;

    Label row = Label::New();
    row.SetRequestedWidth(WINDOW_W);
    row.SetRequestedHeight(ROW_H);
    row.SetBackgroundColor(NormalBgForType(viewType));
    row.SetTextColor(viewType == 3u ? COLOR_TEXT_DISABLED : COLOR_TEXT);
    row.SetProperty(View::Property::CORNER_RADIUS, Vector4(6.0f, 6.0f, 6.0f, 6.0f));

    // Types 0/1/2 are focusable: AsInteractive() attaches press/click StateEffect,
    // SetFocusable enables keyboard navigation. Type 3 is unfocusable — no trait needed.
    if(viewType != 3u)
    {
      row.AsInteractive();
      row.SetFocusable(true);
    }

    mViewRecords.push_back({row, INVALID_POS});
    UpdateStatsText();
    return row;
  }

  void BindItemView(View itemView, uint32_t position)
  {
    ++mBindCount;

    Label row = Label::DownCast(itemView);
    if(!row) return;

    // Update position record.
    auto it = std::find_if(mViewRecords.begin(), mViewRecords.end(),
                           [&](const ViewRecord& r) { return r.label == row; });
    if(it != mViewRecords.end()) it->position = position;

    const float    height   = GetItemHeight(position);
    const uint32_t viewSlot = static_cast<uint32_t>(it - mViewRecords.begin());

    row.SetRequestedHeight(height);
    row.SetText(BuildRowText(viewSlot, position, height));
    ApplyRowStyle(row, position);

    UpdateStatsText();
  }

  void ItemViewRecycled(View itemView, uint32_t /*viewType*/)
  {
    ++mRecycleCount;
    Label row = Label::DownCast(itemView);
    auto  it  = std::find_if(mViewRecords.begin(), mViewRecords.end(),
                              [&](const ViewRecord& r) { return r.label == row; });
    if(it != mViewRecords.end()) it->position = INVALID_POS;
    UpdateStatsText();
  }

  // Apply visual style: unfocusable (grey) vs normal (type colour).
  void ApplyRowStyle(Label row, uint32_t position)
  {
    if(!row) return;
    if(!IsFocusable(position))
    {
      row.SetBackgroundColor(COLOR_ROW_DISABLED);
      row.SetTextColor(COLOR_TEXT_DISABLED);
    }
    else
    {
      row.SetBackgroundColor(NormalBgForType(ViewTypeFor(position)));
      row.SetTextColor(COLOR_TEXT);
    }
  }

  Label FindTrackedLabel(View view) const
  {
    if(!view) return Label();
    Label label = Label::DownCast(view);
    if(!label) return Label();
    auto it = std::find_if(mViewRecords.begin(), mViewRecords.end(),
                           [&](const ViewRecord& r) { return r.label == label; });
    return (it != mViewRecords.end()) ? it->label : Label();
  }

  uint32_t PositionOf(Label label) const
  {
    auto it = std::find_if(mViewRecords.begin(), mViewRecords.end(),
                           [&](const ViewRecord& r) { return r.label == label; });
    return (it != mViewRecords.end()) ? it->position : INVALID_POS;
  }

  void UpdateStatsText()
  {
    if(!mStatsLabel) return;
    std::ostringstream oss;
    oss << "Created:" << mCreateCount
        << "  Bind:" << mBindCount
        << "  Recycled:" << mRecycleCount
        << "  Range:" << mFirstVisible << "-" << mLastVisible;
    mStatsLabel.SetText(Dali::String(oss.str().c_str()));
  }

  Label                   mStatsLabel;
  std::vector<ViewRecord> mViewRecords;
  uint32_t                mCreateCount;
  uint32_t                mBindCount;
  uint32_t                mRecycleCount;
  uint32_t                mFirstVisible;
  uint32_t                mLastVisible;
  uint32_t                mFocusedPosition;
};

// ---------------------------------------------------------------------------
// RecyclerViewSampleController
// ---------------------------------------------------------------------------
class RecyclerViewSampleController : public ConnectionTracker
{
public:
  explicit RecyclerViewSampleController(Application& application)
  : mApplication(application)
  {
    mLayouter = LinearItemsLayouter::New(ItemsLayouter::Orientation::VERTICAL);
    mApplication.InitSignal().Connect(this, &RecyclerViewSampleController::Create);
  }

  void Create(Application application)
  {
    Window window       = application.GetWindow();
    auto   positionSize = window.GetPositionSize();
    window.SetPositionSize(PositionSize(positionSize.x, positionSize.y,
                                       static_cast<uint32_t>(WINDOW_W),
                                       static_cast<uint32_t>(WINDOW_H)));
    window.SetBackgroundColor(COLOR_BACKGROUND);

    BuildHeader(window);
    mTextAdapter.SetStatsLabel(mStatsLabel);
    BuildRecyclerView(window);

    // Connect global focus change to adapter (for highlight) and to controller (for info label).
    FocusManager::Get().FocusChangedSignal().Connect(this, &RecyclerViewSampleController::OnFocusChanged);

    mRangeTimer = Timer::New(100u);
    mRangeTimer.TickSignal().Connect(this, &RecyclerViewSampleController::OnRangeTimerTick);
    mRangeTimer.Start();
  }

private:
  // ---- Header ----------------------------------------------------------------
  void BuildHeader(Window& window)
  {
    View header = View::New();
    header.SetRequestedWidth(WINDOW_W);
    header.SetRequestedHeight(HEADER_H);
    header.SetBackgroundColor(COLOR_HEADER);

    // Row 0 — title
    Label title = Label::New("RecyclerView  |  Key scroll + focusability mix sample");
    title.SetRequestedWidth(WINDOW_W - 32.0f);
    title.SetRequestedHeight(30.0f);
    title.SetRequestedPositionX(16.0f);
    title.SetRequestedPositionY(8.0f);
    title.SetTextColor(COLOR_WHITE);
    header.Add(title);

    // Row 1 — recycler stats
    mStatsLabel = Label::New("--");
    mStatsLabel.SetRequestedWidth(WINDOW_W - 32.0f);
    mStatsLabel.SetRequestedHeight(22.0f);
    mStatsLabel.SetRequestedPositionX(16.0f);
    mStatsLabel.SetRequestedPositionY(42.0f);
    mStatsLabel.SetTextColor(COLOR_SLATE);
    header.Add(mStatsLabel);

    // Row 2 — focus info
    mFocusInfoLabel = Label::New("Focus: none");
    mFocusInfoLabel.SetRequestedWidth(WINDOW_W - 32.0f);
    mFocusInfoLabel.SetRequestedHeight(22.0f);
    mFocusInfoLabel.SetRequestedPositionX(16.0f);
    mFocusInfoLabel.SetRequestedPositionY(68.0f);
    mFocusInfoLabel.SetTextColor(UiColor(0xFDE047));
    header.Add(mFocusInfoLabel);

    // Row 3 — scroll state
    mScrollStateLabel = Label::New("Scroll: IDLE");
    mScrollStateLabel.SetRequestedWidth(WINDOW_W - 32.0f);
    mScrollStateLabel.SetRequestedHeight(22.0f);
    mScrollStateLabel.SetRequestedPositionX(16.0f);
    mScrollStateLabel.SetRequestedPositionY(94.0f);
    mScrollStateLabel.SetTextColor(COLOR_GREEN_BRIGHT);
    header.Add(mScrollStateLabel);

    // Row 4 — hint
    // Colour legend is baked into the hint text.
    Label hint = Label::New(
      "Blue/Green/Orange=focusable  Grey=unfocusable(pos%10>=8)"
      "   Keys: Up/Down/PgUp/PgDn/Home/End");
    hint.SetRequestedWidth(WINDOW_W - 32.0f);
    hint.SetRequestedHeight(28.0f);
    hint.SetRequestedPositionX(16.0f);
    hint.SetRequestedPositionY(118.0f);
    hint.SetTextColor(COLOR_SLATE);
    header.Add(hint);

    window.Add(header);
  }

  // ---- RecyclerView ----------------------------------------------------------
  void BuildRecyclerView(Window& window)
  {
    mTextAdapter.Attach(mAdapter);

    mLayouter.SetItemExtent(ROW_H);
    mLayouter.SetItemSpacing(ROW_SPACING);

    mRecyclerView = RecyclerView::New();
    mRecyclerView.SetRequestedWidth(WINDOW_W);
    mRecyclerView.SetRequestedHeight(WINDOW_H - HEADER_H);
    mRecyclerView.SetRequestedPositionY(HEADER_H);
    mRecyclerView.SetCacheExtent(ROW_H * 2.0f, ROW_H * 2.0f);
    mRecyclerView.SetItemsLayouter(mLayouter);
    mRecyclerView.SetAdapter(mAdapter);

    // Key-scroll: enabled, step = 1.5 rows (focus jumps when target is that close),
    // peek = half a row so the next item is partially revealed after each jump.
    mRecyclerView.SetKeyScrollEnabled(true);
    mRecyclerView.SetKeyScrollStep(ROW_H * 1.5f);
    mRecyclerView.SetFocusScrollPeek(ROW_H * 0.5f);
    // RecyclerView itself must be KEYBOARD_FOCUSABLE for the "no-item" step-scroll mode.
    mRecyclerView.SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, true);

    mRecyclerView.ScrollStartedSignal().Connect(this, &RecyclerViewSampleController::OnScrollStarted);
    mRecyclerView.ScrollFinishedSignal().Connect(this, &RecyclerViewSampleController::OnScrollFinished);
    mRecyclerView.DragStartedSignal().Connect(this, &RecyclerViewSampleController::OnDragStarted);
    mRecyclerView.DragFinishedSignal().Connect(this, &RecyclerViewSampleController::OnDragFinished);

    window.Add(mRecyclerView);
  }

  // ---- Focus changed ---------------------------------------------------------
  void OnFocusChanged(View from, View to)
  {
    // Forward to adapter so it can update highlight on the affected views.
    mTextAdapter.OnFocusChanged(from, to);

    // Update focus info label.
    const uint32_t pos = mTextAdapter.GetFocusedPosition();
    if(!mFocusInfoLabel) return;

    if(pos == INVALID_POS)
    {
      if(to == mRecyclerView)
        mFocusInfoLabel.SetText(Dali::String("Focus: RecyclerView (step-scroll mode)"));
      else
        mFocusInfoLabel.SetText(Dali::String("Focus: none / outside RecyclerView"));
    }
    else
    {
      std::ostringstream oss;
      oss << "Focus: item #" << pos
          << "   " << (IsFocusable(pos) ? "FOCUSABLE" : "UNFOCUSABLE")
          << "   (pos % 10 = " << (pos % 10u) << ")";
      mFocusInfoLabel.SetText(Dali::String(oss.str().c_str()));
    }
  }

  // ---- Scroll state ----------------------------------------------------------
  void UpdateScrollStateLabel()
  {
    if(!mScrollStateLabel) return;
    const char* state = "IDLE";
    if(mRecyclerView.IsScrolling())
      state = mIsDragging ? "DRAGGING" : "FLINGING";
    std::ostringstream oss;
    oss << "Scroll: " << state
        << "   offset: " << static_cast<int>(mRecyclerView.GetScrollOffset()) << "px";
    mScrollStateLabel.SetText(Dali::String(oss.str().c_str()));
  }

  void OnScrollStarted(RecyclerView) { UpdateScrollStateLabel(); }
  void OnScrollFinished(RecyclerView) { UpdateScrollStateLabel(); }
  void OnDragStarted(RecyclerView) { mIsDragging = true;  UpdateScrollStateLabel(); }
  void OnDragFinished(RecyclerView) { mIsDragging = false; UpdateScrollStateLabel(); }

  // ---- Range timer -----------------------------------------------------------
  bool OnRangeTimerTick()
  {
    if(mRecyclerView)
      mTextAdapter.SetRange(mRecyclerView.GetFirstVisiblePosition(),
                            mRecyclerView.GetLastVisiblePosition());
    return true;
  }

private:
  Application&         mApplication;
  RecyclerView         mRecyclerView;
  LinearItemsLayouter  mLayouter;
  ItemAdapter          mAdapter;
  RecyclingTextAdapter mTextAdapter;

  Label mStatsLabel;
  Label mFocusInfoLabel;
  Label mScrollStateLabel;
  Timer mRangeTimer;
  bool  mIsDragging{false};
};

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig    config      = UiConfig::New();
  config.Apply();

  RecyclerViewSampleController controller(application);
  application.MainLoop();
  return 0;
}
