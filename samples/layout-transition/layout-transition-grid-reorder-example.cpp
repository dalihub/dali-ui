/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

/**
 * LayoutTransition grid reorder sample.
 *
 * A white root fills the window. Its child is a vertical StackLayout
 * (400 x 600, translucent gray, 40px corner radius) placed at
 * (80, 80) with 20px padding and 20px spacing between its three children:
 *
 *   1. A transparent row that right-aligns a notification and an edit
 *      icon (each 50 x 50) with 20px between them.
 *   2. A horizontal StackLayout with a Wi-Fi icon button on the left and
 *      a Bluetooth icon button on the right (each icon 100 x 100, with
 *      the file-name label to the right of the icon).
 *   3. A scrollable 3-column GridLayout that fills the remaining area.
 *      Each cell shows a 100 x 100 SVG icon with its file-name label
 *      below it.
 *
 * The GridLayout owns a LayoutTransition whose CHANGE slot shares the
 * 0.4s EASE_IN_OUT_SINE timing used by the other layout-transition
 * samples. The reorder interaction follows layout-transition-reorder:
 * long-pressing a cell floats it under the window at its press-time world
 * bounds and inserts a same-sized invisible (OPACITY 0) proxy into its
 * logical slot to reserve the grid space. Dragging moves the floating
 * cell by RequestedX/Y; once the floating cell's world position moves
 * past another cell's position by more than half a cell, the proxy is
 * moved to that cell's linear index and every cell's GridLayoutParams
 * Row/Column is reassigned, so the CHANGE slot animates the reflow.
 * Dragging near the top / bottom edge of the
 * ScrollView viewport auto-scrolls so reorder keeps tracking the finger
 * as content scrolls underneath. On release the proxy is removed and the
 * cell returns to the grid at the proxy's index, with its arranged bounds
 * pre-baked so the slide-into-slot animation starts from the on-screen
 * drop position.
 *
 *   - Long-press a grid cell, then drag to reorder.
 *       Drag near the top / bottom of the grid to auto-scroll.
 *   - Esc / Back: quit.
 */
class LayoutTransitionGridReorderController : public ConnectionTracker
{
public:
  explicit LayoutTransitionGridReorderController(Application& application)
  : mApplication(application),
    mDragging(false),
    mDraggedIndex(0u),
    mDragBounds{},
    mDragGrabOffset(0.0f, 0.0f),
    mDraggedOriginalReqW(0.0f),
    mDraggedOriginalReqH(0.0f),
    mLastTouchRootPosition(0.0f, 0.0f),
    mLongPressArmed(false),
    mPressIndex(0u),
    mPressRootPosition(0.0f, 0.0f)
  {
    mApplication.InitSignal().Connect(this, &LayoutTransitionGridReorderController::Create);
  }

  void Create(Application application)
  {
    mWindow = application.GetWindow();

    // CHANGE timing shared with the other layout-transition samples.
    LayoutTransitionTiming timing{Duration(0.4f),
                                  AlphaFunction(AlphaFunction::EASE_IN_OUT_SINE),
                                  Duration()};

    // ── Child 1: transparent row, right-aligned notification + edit ──────────
    StackLayout topRow = StackLayout::New(StackOrientation::HORIZONTAL);
    topRow.SetRequestedWidth(MATCH_PARENT);
    topRow.SetRequestedHeight(WRAP_CONTENT);
    topRow.SetSpacing(20.0f);

    View topSpacer = View::New();
    topSpacer.SetRequestedHeight(1.0f);
    topSpacer.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    topRow.Add(topSpacer);
    topRow.Add(MakeFixedIcon("notification", TOP_ICON));
    topRow.Add(MakeFixedIcon("edit", TOP_ICON));

    // ── Child 2: Wi-Fi button (left) and Bluetooth button (right) ────────────
    StackLayout midRow = StackLayout::New(StackOrientation::HORIZONTAL);
    midRow.SetRequestedWidth(MATCH_PARENT);
    midRow.SetRequestedHeight(WRAP_CONTENT);

    View midSpacer = View::New();
    midSpacer.SetRequestedHeight(1.0f);
    midSpacer.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    midRow.Add(MakeIconButton("wifi"));
    midRow.Add(midSpacer);
    midRow.Add(MakeIconButton("bluetooth"));

    // ── Child 3: scrollable 3-column reorderable grid ────────────────────────
    static const char* const kGridIcons[] = {
      "brightness", "color", "energy", "game",
      "multi view", "output display", "picture", "settings",
      "share", "sound", "support", "timer"};
    const uint32_t iconCount = static_cast<uint32_t>(sizeof(kGridIcons) / sizeof(kGridIcons[0]));
    const uint32_t rowCount  = (iconCount + COLUMNS - 1u) / COLUMNS;

    mGrid = GridLayout::New();
    mGrid.SetRequestedWidth(MATCH_PARENT);
    // WRAP_CONTENT so the grid can grow taller than the ScrollView viewport
    // and the content becomes scrollable.
    mGrid.SetRequestedHeight(WRAP_CONTENT);
    for(uint32_t c = 0u; c < COLUMNS; ++c)
    {
      mGrid.AddColumnDefinition(GridLength::Absolute(CELL_W));
    }
    for(uint32_t r = 0u; r < rowCount; ++r)
    {
      mGrid.AddRowDefinition(GridLength::Absolute(CELL_H));
    }

    LayoutTransition transition = LayoutTransition::New();
    transition.SetChangeTiming(timing);
    mGrid.SetLayoutTransition(transition);

    for(uint32_t i = 0u; i < iconCount; ++i)
    {
      View cell = MakeGridCell(kGridIcons[i]);
      mGridItems.push_back(cell);
      mGrid.Add(cell);
    }
    ApplyGridOrder();

    // The ScrollView's height comes from the StackLayout weight (it fills the
    // space left by the two rows above), so its main-axis RequestedHeight is
    // left unset; only the cross-axis width is requested.
    mScrollView = ScrollView::New();
    mScrollView.SetScrollDirection(ScrollDirection::Vertical);
    mScrollView.SetOverScrollMode(OverScrollMode::ContentScrolls);
    mScrollView.SetRequestedWidth(MATCH_PARENT);
    mScrollView.SetContent(mGrid);
    mScrollView.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));

    // ── Outer translucent panel ──────────────────────────────────────────────
    mOuterStack = StackLayout::New(StackOrientation::VERTICAL);
    // A definite main-axis size is required for the weighted ScrollView child to
    // fill the remaining space; without it the stack wraps to its content and
    // the ScrollView grows to the grid's full (scrollable) height instead.
    mOuterStack.SetRequestedWidth(400.0f);
    mOuterStack.SetRequestedHeight(600.0f);
    mOuterStack.SetSpacing(20.0f);
    mOuterStack.SetPadding(Insets(20.0f, 20.0f, 20.0f, 20.0f));
    mOuterStack.SetBackgroundColor(Vector4(0.5f, 0.5f, 0.5f, 0.5f));
    mOuterStack.SetCornerRadius(40.0f);
    mOuterStack.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(80.0f, 80.0f, 400.0f, 600.0f)));
    mOuterStack.Add(topRow);
    mOuterStack.Add(midRow);
    mOuterStack.Add(mScrollView);

    // ── White root filling the window ────────────────────────────────────────
    mRoot = AbsoluteLayout::New();
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(MATCH_PARENT);
    mRoot.SetBackgroundColor(Color::WHITE);
    mRoot.Add(mOuterStack);

    mWindow.Add(mRoot);
    mWindow.KeyEventSignal().Connect(this, &LayoutTransitionGridReorderController::OnKeyEvent);
  }

  // ── Builders ───────────────────────────────────────────────────────────────

  ImageView MakeFixedIcon(const Dali::String& name, float size)
  {
    ImageView icon = ImageView::New(Dali::String(RESOURCES_DIR) + name + ".svg");
    icon.SetRequestedWidth(size);
    icon.SetRequestedHeight(size);
    icon.SetDesiredWidth(static_cast<int>(size));
    icon.SetDesiredHeight(static_cast<int>(size));
    return icon;
  }

  // Wi-Fi / Bluetooth button: 100x100 icon with the file-name label to its right.
  View MakeIconButton(const Dali::String& name)
  {
    StackLayout button = StackLayout::New(StackOrientation::HORIZONTAL);
    button.SetRequestedWidth(WRAP_CONTENT);
    button.SetRequestedHeight(WRAP_CONTENT);
    button.SetSpacing(8.0f);
    button.Add(MakeFixedIcon(name, BTN_ICON));

    Label label = Label::New(name);
    label.SetTextColor(Color::WHITE);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    label.SetFontSize(16.0f);
    label.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::CENTER));
    button.Add(label);
    return button;
  }

  // Grid cell: 100x100 icon with the file-name label below it. The icon and
  // label are insensitive so the cell itself is the touch / capture target.
  View MakeGridCell(const Dali::String& name)
  {
    StackLayout cell = StackLayout::New(StackOrientation::VERTICAL);
    cell.SetRequestedWidth(WRAP_CONTENT);
    cell.SetRequestedHeight(WRAP_CONTENT);
    cell.SetSpacing(4.0f);

    ImageView icon = MakeFixedIcon(name, GRID_ICON);
    icon.SetProperty(Actor::Property::SENSITIVE, false);
    icon.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::CENTER));
    cell.Add(icon);

    Label label = Label::New(name);
    label.SetTextColor(Color::WHITE);
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetFontSize(14.0f);
    label.SetProperty(Actor::Property::SENSITIVE, false);
    label.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::CENTER));
    cell.Add(label);

    // Capture all touch on the cell after the initial DOWN so the drag stream
    // stays on the cell once it is reparented under the window and bypasses
    // the ScrollView's gesture intercept during a reorder.
    cell.SetProperty(DevelActor::Property::CAPTURE_ALL_TOUCH_AFTER_START, true);
    cell.TouchEventSignal().Connect(this, &LayoutTransitionGridReorderController::OnItemTouched);
    return cell;
  }

  // Reassigns every logical item's grid cell from its index in mGridItems.
  // SetLayoutParams invalidates measure, so the next layout pass reflows and
  // the CHANGE slot animates the items that moved.
  void ApplyGridOrder()
  {
    for(uint32_t i = 0u; i < mGridItems.size(); ++i)
    {
      mGridItems[i].SetLayoutParams(GridLayoutParams::New()
                                      .SetRow(i / COLUMNS)
                                      .SetColumn(i % COLUMNS)
                                      .SetHorizontalAlignment(LayoutAlignment::CENTER)
                                      .SetVerticalAlignment(LayoutAlignment::CENTER));
    }
  }

  // ── Touch handling ───────────────────────────────────────────────────────

  bool OnItemTouched(Actor actor, TouchEvent touch)
  {
    if(touch.GetPointCount() < 1u)
    {
      return false;
    }

    Vector2 rootPosition;
    if(!GetRootLocalPosition(touch, rootPosition))
    {
      return false;
    }

    const PointState::Type state = touch.GetState(0u);

    if(!mDragging)
    {
      // DOWN on a tracked cell arms a long press; the drag only begins when
      // the long-press timer fires with the finger still down and still.
      if(IsDownState(state))
      {
        View          cell      = View::DownCast(actor);
        const int32_t itemIndex = cell ? FindItemIndex(cell) : -1;
        if(itemIndex < 0)
        {
          return false;
        }
        mPressedItem           = cell;
        mPressIndex            = static_cast<uint32_t>(itemIndex);
        mPressRootPosition     = rootPosition;
        mLastTouchRootPosition = rootPosition;
        mLongPressArmed        = true;
        StartLongPressTimer();
        return true;
      }

      if(mLongPressArmed && state == PointState::MOTION)
      {
        mLastTouchRootPosition = rootPosition;
        // Moving too far before the long press fires cancels it (treated as a
        // scroll / slip rather than a reorder intent).
        if((rootPosition - mPressRootPosition).Length() > MOVE_THRESHOLD)
        {
          CancelLongPress();
        }
        return true;
      }

      if(IsUpState(state))
      {
        CancelLongPress();
        return true;
      }
      return false;
    }

    // Drag in progress.
    if(state == PointState::MOTION)
    {
      UpdateDrag(rootPosition);
      return true;
    }
    if(IsUpState(state))
    {
      UpdateDrag(rootPosition);
      FinishDrag();
      return true;
    }
    return true;
  }

  void OnKeyEvent(Window /*window*/, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::DOWN)
    {
      return;
    }
    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
    }
  }

private:
  static bool IsDownState(PointState::Type state)
  {
    return state == PointState::DOWN || state == PointState::STARTED;
  }

  // INTERRUPTED is deliberately NOT treated as a release. Reparenting the
  // pressed cell from the grid to the window in BeginDrag (driven by the
  // long-press timer, i.e. outside touch-event processing) makes the touch
  // framework deliver a spurious INTERRUPTED on the captured stream. Treating
  // it as a release would finish the drag the instant it starts. The capture
  // keeps delivering MOTION / UP, so a real release still arrives as UP.
  static bool IsUpState(PointState::Type state)
  {
    return state == PointState::UP ||
           state == PointState::FINISHED ||
           state == PointState::LEAVE;
  }

  bool GetRootLocalPosition(TouchEvent touch, Vector2& localPosition)
  {
    const Vector2 screenPosition = touch.GetScreenPosition(0u);
    float         localX         = 0.0f;
    float         localY         = 0.0f;
    if(!mRoot.ScreenToLocal(localX, localY, screenPosition.x, screenPosition.y))
    {
      return false;
    }
    localPosition = Vector2(localX, localY);
    return true;
  }

  int32_t FindItemIndex(View cell) const
  {
    for(uint32_t i = 0u; i < mGridItems.size(); ++i)
    {
      if(mGridItems[i] == cell)
      {
        return static_cast<int32_t>(i);
      }
    }
    return -1;
  }

  // World X / Y of mGrid's top-left in mRoot-local (== window) space. mGrid is
  // the ScrollView content, so the ScrollView's offset inside mOuterStack and
  // mOuterStack's offset inside mRoot are added; mGrid.POSITION already folds
  // in the scroll offset (ScrollView scrolls by writing its content POSITION).
  // This mirrors layout-transition-reorder's GetStackWorldX/Y.
  float GetGridWorldX() const
  {
    return mOuterStack.GetCurrentProperty<float>(Actor::Property::POSITION_X) +
           mScrollView.GetCurrentProperty<float>(Actor::Property::POSITION_X) +
           mGrid.GetCurrentProperty<float>(Actor::Property::POSITION_X);
  }

  float GetGridWorldY() const
  {
    return mOuterStack.GetCurrentProperty<float>(Actor::Property::POSITION_Y) +
           mScrollView.GetCurrentProperty<float>(Actor::Property::POSITION_Y) +
           mGrid.GetCurrentProperty<float>(Actor::Property::POSITION_Y);
  }

  // Cell bounds in mRoot-local (== window) space.
  LayoutRect GetItemRootBounds(View cell) const
  {
    LayoutRect bounds;
    bounds.x      = GetGridWorldX() + cell.GetCurrentProperty<float>(Actor::Property::POSITION_X);
    bounds.y      = GetGridWorldY() + cell.GetCurrentProperty<float>(Actor::Property::POSITION_Y);
    bounds.width  = cell.GetCurrentProperty<float>(Actor::Property::SIZE_WIDTH);
    bounds.height = cell.GetCurrentProperty<float>(Actor::Property::SIZE_HEIGHT);
    return bounds;
  }

  // Row-major target index for the dragged cell. The floating cell's world
  // centre is mapped into mGrid-local space (by subtracting the grid origin)
  // and quantised to the grid cell it sits over, so a cell only changes slot
  // once the dragged centre has crossed more than half a cell past a neighbour
  // (i.e. exceeded the neighbour's position by half the cell size).
  uint32_t ComputeTargetIndex(const LayoutRect& bounds) const
  {
    const uint32_t count = static_cast<uint32_t>(mGridItems.size());
    if(count == 0u)
    {
      return 0u;
    }
    const uint32_t rows = (count + COLUMNS - 1u) / COLUMNS;

    const float centerX = bounds.x + bounds.width * 0.5f - GetGridWorldX();
    const float centerY = bounds.y + bounds.height * 0.5f - GetGridWorldY();

    int col = static_cast<int>(std::floor(centerX / CELL_W));
    int row = static_cast<int>(std::floor(centerY / CELL_H));
    col     = std::clamp(col, 0, static_cast<int>(COLUMNS) - 1);
    row     = std::clamp(row, 0, static_cast<int>(rows) - 1);

    const uint32_t index = static_cast<uint32_t>(row) * COLUMNS + static_cast<uint32_t>(col);
    return std::min(index, count - 1u);
  }

  // Precondition: no drag is active (only called from OnLongPressTick, which
  // guards with !mDragging).
  void BeginDrag(View cell, uint32_t itemIndex, const Vector2& rootPosition)
  {
    // World bounds of the pressed cell in mRoot-local (== window) space. The
    // floating cell uses these directly as its position and size, and the grab
    // offset is the finger relative to the cell's top-left in the same space —
    // exactly how layout-transition-reorder floats its dragged child.
    const LayoutRect bounds = GetItemRootBounds(cell);

    mDraggedChild          = cell;
    mDraggedIndex          = itemIndex;
    mDragBounds            = bounds;
    mDragGrabOffset        = rootPosition - Vector2(bounds.x, bounds.y);
    mDraggedOriginalReqW   = cell.GetRequestedWidth();
    mDraggedOriginalReqH   = cell.GetRequestedHeight();
    mLastTouchRootPosition = rootPosition;
    mDragging              = true;

    // Swap the pressed cell for a same-sized invisible proxy with no
    // transition so the swap is visually silent; re-attach right after so the
    // in-drag reorders animate through the CHANGE slot.
    LayoutTransition savedTransition = mGrid.GetLayoutTransition();
    mGrid.SetLayoutTransition(LayoutTransition());
    mGrid.Remove(cell, RemovePolicy::IMMEDIATE);

    mDragProxy = View::New();
    mDragProxy.SetRequestedWidth(bounds.width);
    mDragProxy.SetRequestedHeight(bounds.height);
    mDragProxy.SetProperty(Actor::Property::OPACITY, 0.0f);
    mDragProxy.SetProperty(Actor::Property::SENSITIVE, false);
    mGridItems[itemIndex] = mDragProxy;
    mGrid.Add(mDragProxy);
    ApplyGridOrder();

    mGrid.SetLayoutTransition(savedTransition);

    // Float the cell under the window at its press-time position.
    cell.SetRequestedWidth(mDragBounds.width);
    cell.SetRequestedHeight(mDragBounds.height);
    cell.SetRequestedX(mDragBounds.x);
    cell.SetRequestedY(mDragBounds.y);
    mWindow.Add(cell);
    cell.RaiseToTop(LayoutOrderPolicy::PRESERVE);
    // Force the floating cell's layout pass so it lands at its requested world
    // position immediately rather than after the next frame's pass.
    LayoutController::Get(mWindow).ProcessLayouts();

    StartAutoScrollTimer();
  }

  void UpdateDrag(const Vector2& rootPosition)
  {
    if(!mDragging || !mDraggedChild)
    {
      return;
    }

    mLastTouchRootPosition = rootPosition;

    // Clamp the floating cell to the visible ScrollView viewport (Y) and to the
    // grid's column band (X) so it stays on screen.
    const float viewportTop    = mOuterStack.GetCurrentProperty<float>(Actor::Property::POSITION_Y) +
                                 mScrollView.GetCurrentProperty<float>(Actor::Property::POSITION_Y);
    const float viewportHeight = mScrollView.GetCurrentProperty<float>(Actor::Property::SIZE_HEIGHT);
    const float minY           = viewportTop;
    const float maxY           = std::max(minY, viewportTop + viewportHeight - mDragBounds.height);

    const float gridLeft  = GetGridWorldX();
    const float gridRight = gridLeft + static_cast<float>(COLUMNS) * CELL_W;
    const float minX      = gridLeft;
    const float maxX      = std::max(minX, gridRight - mDragBounds.width);

    mDragBounds.x = std::clamp(rootPosition.x - mDragGrabOffset.x, minX, maxX);
    mDragBounds.y = std::clamp(rootPosition.y - mDragGrabOffset.y, minY, maxY);

    mDraggedChild.SetRequestedX(mDragBounds.x);
    mDraggedChild.SetRequestedY(mDragBounds.y);

    const uint32_t targetIndex = ComputeTargetIndex(mDragBounds);
    if(targetIndex != mDraggedIndex)
    {
      MoveProxy(mDraggedIndex, targetIndex);
      mDraggedIndex = targetIndex;
      // Reassign every cell's Row/Column for the new order. SetLayoutParams
      // marks the grid dirty, but its InvalidateMeasure early-exits if the grid
      // was already dirty, so the reflow can be skipped. Drive a synchronous
      // layout pass right here to guarantee the cells move to their new cells;
      // the transition stays attached so the CHANGE slot animates the reflow.
      ApplyGridOrder();
      LayoutController::Get(mWindow).ProcessLayouts();
    }
  }

  void FinishDrag()
  {
    if(!mDragging)
    {
      return;
    }

    StopAutoScrollTimer();

    View             droppedChild = mDraggedChild;
    View             proxyToRemove = mDragProxy;
    const uint32_t   targetIndex   = mDraggedIndex;
    const float      originalReqW  = mDraggedOriginalReqW;
    const float      originalReqH  = mDraggedOriginalReqH;
    const LayoutRect dragBounds    = mDragBounds;

    mDragging            = false;
    mDraggedChild        = View();
    mDragProxy           = View();
    mDraggedIndex        = 0u;
    mDraggedOriginalReqW = 0.0f;
    mDraggedOriginalReqH = 0.0f;
    mDragGrabOffset      = Vector2(0.0f, 0.0f);
    mDragBounds          = {};

    if(!droppedChild || !proxyToRemove)
    {
      return;
    }

    // Pre-bake the dropped cell's arranged bounds to the drop position
    // expressed in mGrid-local coordinates while it is still a layout root
    // under the window, so the CHANGE animation starts from the on-screen drop
    // position rather than teleporting before sliding into the slot.
    const float dropGridX = dragBounds.x - GetGridWorldX();
    const float dropGridY = dragBounds.y - GetGridWorldY();
    droppedChild.SetRequestedX(dropGridX);
    droppedChild.SetRequestedY(dropGridY);
    LayoutController::Get(mWindow).ProcessLayouts();

    // Mirror BeginDrag's swap: detach the transition so removing the proxy and
    // re-inserting the dropped cell is silent, then re-attach so the next
    // layout pass dispatches CHANGE from the pre-baked snapshot.
    LayoutTransition savedTransition = mGrid.GetLayoutTransition();
    mGrid.SetLayoutTransition(LayoutTransition());

    mGrid.Remove(proxyToRemove, RemovePolicy::IMMEDIATE);
    if(droppedChild.GetParent())
    {
      droppedChild.Unparent();
    }
    droppedChild.SetRequestedWidth(originalReqW);
    droppedChild.SetRequestedHeight(originalReqH);
    droppedChild.SetRequestedX(0.0f);
    droppedChild.SetRequestedY(0.0f);
    mGridItems[targetIndex] = droppedChild;
    mGrid.Add(droppedChild);
    ApplyGridOrder();

    mGrid.SetLayoutTransition(savedTransition);
  }

  void MoveProxy(uint32_t from, uint32_t to)
  {
    if(from == to || from >= mGridItems.size() || to >= mGridItems.size())
    {
      return;
    }
    View proxy = mGridItems[from];
    mGridItems.erase(mGridItems.begin() + from);
    mGridItems.insert(mGridItems.begin() + to, proxy);
  }

  // ── Long press ─────────────────────────────────────────────────────────────

  void StartLongPressTimer()
  {
    if(!mLongPressTimer)
    {
      mLongPressTimer = Timer::New(LONG_PRESS_MS);
      mLongPressTimer.TickSignal().Connect(this, &LayoutTransitionGridReorderController::OnLongPressTick);
    }
    mLongPressTimer.SetInterval(LONG_PRESS_MS);
    mLongPressTimer.Start();
  }

  void CancelLongPress()
  {
    mLongPressArmed = false;
    mPressedItem    = View();
    if(mLongPressTimer && mLongPressTimer.IsRunning())
    {
      mLongPressTimer.Stop();
    }
  }

  bool OnLongPressTick()
  {
    if(mLongPressArmed && mPressedItem && !mDragging)
    {
      View     cell  = mPressedItem;
      uint32_t index = mPressIndex;
      mLongPressArmed = false;
      mPressedItem    = View();
      BeginDrag(cell, index, mLastTouchRootPosition);
    }
    return false; // one-shot
  }

  // ── Auto-scroll while dragging near ScrollView edges ────────────────────────

  void StartAutoScrollTimer()
  {
    if(!mAutoScrollTimer)
    {
      mAutoScrollTimer = Timer::New(AUTO_SCROLL_TICK_MS);
      mAutoScrollTimer.TickSignal().Connect(this, &LayoutTransitionGridReorderController::OnAutoScrollTick);
    }
    if(!mAutoScrollTimer.IsRunning())
    {
      mAutoScrollTimer.Start();
    }
  }

  void StopAutoScrollTimer()
  {
    if(mAutoScrollTimer && mAutoScrollTimer.IsRunning())
    {
      mAutoScrollTimer.Stop();
    }
  }

  bool OnAutoScrollTick()
  {
    if(!mDragging || !mScrollView)
    {
      return false;
    }

    const float viewportTop    = mOuterStack.GetCurrentProperty<float>(Actor::Property::POSITION_Y) +
                                 mScrollView.GetCurrentProperty<float>(Actor::Property::POSITION_Y);
    const float viewportHeight = mScrollView.GetCurrentProperty<float>(Actor::Property::SIZE_HEIGHT);
    const float viewportBottom = viewportTop + viewportHeight;
    const float touchY         = mLastTouchRootPosition.y;

    float scrollStepY = 0.0f;
    if(touchY < viewportTop + AUTO_SCROLL_EDGE_ZONE)
    {
      const float intensity = std::clamp((viewportTop + AUTO_SCROLL_EDGE_ZONE - touchY) / AUTO_SCROLL_EDGE_ZONE, 0.0f, 1.0f);
      scrollStepY           = -AUTO_SCROLL_MAX_STEP * intensity;
    }
    else if(touchY > viewportBottom - AUTO_SCROLL_EDGE_ZONE)
    {
      const float intensity = std::clamp((touchY - (viewportBottom - AUTO_SCROLL_EDGE_ZONE)) / AUTO_SCROLL_EDGE_ZONE, 0.0f, 1.0f);
      scrollStepY           = AUTO_SCROLL_MAX_STEP * intensity;
    }

    if(scrollStepY != 0.0f)
    {
      const Vector2 current = mScrollView.GetScrollPosition();
      mScrollView.ScrollTo(Vector2(current.x, current.y + scrollStepY), false);
      // Finger has not moved but content has, so re-evaluate the proxy slot.
      UpdateDrag(mLastTouchRootPosition);
    }

    return true;
  }

private:
  static constexpr uint32_t COLUMNS   = 3u;
  static constexpr float    GRID_ICON = 100.0f; ///< Grid icon size
  static constexpr float    TOP_ICON  = 50.0f;  ///< Notification / edit icon size
  static constexpr float    BTN_ICON  = 100.0f; ///< Wi-Fi / Bluetooth icon size
  static constexpr float    CELL_W    = 120.0f; ///< Grid cell width
  static constexpr float    CELL_H    = 150.0f; ///< Grid cell height

  static constexpr uint32_t LONG_PRESS_MS  = 500u;  ///< Hold time that starts a drag
  static constexpr float    MOVE_THRESHOLD = 20.0f; ///< Pre-drag slip that cancels the long press

  static constexpr float    AUTO_SCROLL_EDGE_ZONE = 60.0f; ///< px from viewport edge that triggers auto-scroll
  static constexpr float    AUTO_SCROLL_MAX_STEP  = 12.0f; ///< px scrolled per tick at the very edge
  static constexpr uint32_t AUTO_SCROLL_TICK_MS   = 16u;   ///< ~60 Hz

  Application&      mApplication;
  Window           mWindow;
  AbsoluteLayout   mRoot;
  StackLayout      mOuterStack;
  ScrollView       mScrollView;
  GridLayout       mGrid;
  std::vector<View> mGridItems; ///< Logical order of grid cells (proxy swapped in during a drag)

  bool       mDragging;
  View       mDraggedChild;        ///< The dragged cell, floating under the window during a drag
  View       mDragProxy;           ///< Invisible slot reserving the dragged cell's place in the grid
  uint32_t   mDraggedIndex;        ///< Current index of mDragProxy in mGridItems
  LayoutRect mDragBounds;          ///< Dragged cell's world bounds (driven by the finger)
  Vector2    mDragGrabOffset;
  float      mDraggedOriginalReqW; ///< Restored to mDraggedChild on FinishDrag
  float      mDraggedOriginalReqH; ///< Restored to mDraggedChild on FinishDrag
  Vector2    mLastTouchRootPosition;

  Timer    mAutoScrollTimer; ///< Fires while a drag is in flight to apply edge-zone auto-scroll
  Timer    mLongPressTimer;  ///< Fires once after a hold to start a drag
  bool     mLongPressArmed;
  View     mPressedItem;
  uint32_t mPressIndex;
  Vector2  mPressRootPosition;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application                           application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  LayoutTransitionGridReorderController controller(application);
  application.MainLoop();
  return 0;
}
