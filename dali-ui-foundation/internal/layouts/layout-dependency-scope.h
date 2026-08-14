#pragma once

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

// EXTERNAL INCLUDES
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{
class ViewImpl;

namespace Internal
{
/**
 * @brief Records which View owns the nested Measure() calls currently in flight.
 *
 * A "layout dependency owner" is the View that deliberately issued a Measure() on a
 * descendant while it was running its own pass, and that therefore already accounts
 * for whatever result that Measure() produces. Two producers do this:
 *
 * - an arranging View re-measuring a MATCH_PARENT child with its final slot
 *   (ViewDataImpl::ArrangeDefault and every LayoutManager::Arrange), and
 * - RecyclerView driving its ItemsLayouter, whose own measured/arranged geometry is a
 *   pure function of its constraints and never reads an item's stored slot.
 *
 * The owner is recorded on an intrusive stack of stack-resident frames so that a
 * consumer can ask "is this Measure() owned, and by whom?" instead of inferring it.
 *
 * @note The consumer is ViewDataImpl::InvalidateAncestorLayoutCachesForMeasureMiss():
 * it stops its ascent at Top()->owner instead of clearing that owner's cache chain.
 *
 * @note Event thread only. The stack is thread-local because a layout pass and every
 * nested Measure() it issues run synchronously on the event thread.
 */
namespace LayoutDependency
{
/**
 * @brief Which kind of producer owns the frame.
 */
enum class OwnerKind : uint8_t
{
  ARRANGE, ///< An arranging View re-measuring one of its own children.
  RECYCLER ///< A RecyclerView driving its ItemsLayouter over the item views.
};

/**
 * @brief One entry of the owner stack.
 *
 * POD and trivially destructible: every frame is a member of a stack-resident RAII
 * scope object, so the stack costs no allocation and unwinds with the C++ stack.
 */
struct Frame
{
  ViewImpl*   owner;               ///< The owning View. nullptr means the scope is inert (not pushed).
  const void* secondary;           ///< RECYCLER: the ItemsLayouterImpl identity. ARRANGE: nullptr.
  Frame*      previous;            ///< The enclosing frame, restored when this one is popped.
  OwnerKind   kind;                ///< Which producer pushed this frame.
  bool        poisonedDuringScope; ///< Reserved: armed by the increment that introduces the arrange cache-hit path.
};

/**
 * @brief Returns the innermost active owner frame, or nullptr when none is active.
 *
 * Only the innermost frame is ever meaningful: a producer owns only the measurements
 * it issues directly, so an enclosing frame can never describe a measurement issued
 * under an inner one.
 */
DALI_UI_API const Frame* Top();

/**
 * @brief Marks the enclosed Measure() calls as owned by an arranging View.
 *
 * The scope must span the owned child Measure() call(s) and nothing else -- in
 * particular it must NOT span the following child Arrange(), because a Measure()
 * issued out-of-band from a child's arrange callback is not owned by this View.
 */
class DALI_UI_API ArrangeOwnedMeasureScope
{
public:
  /**
   * @brief Pushes an ARRANGE frame owned by @p owner.
   * @param[in] owner The arranging View. nullptr makes the scope inert (nothing is pushed).
   */
  explicit ArrangeOwnedMeasureScope(ViewImpl* owner);

  /**
   * @brief Pops the frame, restoring the enclosing one.
   */
  ~ArrangeOwnedMeasureScope();

  ArrangeOwnedMeasureScope(const ArrangeOwnedMeasureScope&)            = delete;
  ArrangeOwnedMeasureScope& operator=(const ArrangeOwnedMeasureScope&) = delete;

private:
  Frame mFrame; ///< mFrame.owner == nullptr encodes "inert".
};

/**
 * @brief Marks the enclosed ItemsLayouter work as owned by a RecyclerView.
 *
 * Installed on the recycler side of the layouter boundary rather than inside the
 * layouter, so that third-party ItemsLayouterImpl subclasses are covered too.
 */
class DALI_UI_API RecyclerLayoutOwnerScope
{
public:
  /**
   * @brief Pushes a RECYCLER frame owned by @p recycler.
   * @param[in] recycler         The recycler driving the layouter. nullptr makes the scope inert.
   * @param[in] layouterIdentity The ItemsLayouterImpl address, recorded as the secondary identity.
   */
  RecyclerLayoutOwnerScope(ViewImpl* recycler, const void* layouterIdentity);

  /**
   * @brief Pops the frame, restoring the enclosing one.
   */
  ~RecyclerLayoutOwnerScope();

  RecyclerLayoutOwnerScope(const RecyclerLayoutOwnerScope&)            = delete;
  RecyclerLayoutOwnerScope& operator=(const RecyclerLayoutOwnerScope&) = delete;

private:
  Frame mFrame; ///< mFrame.owner == nullptr encodes "inert".
};

} // namespace LayoutDependency
} // namespace Internal
} // namespace Ui
} // namespace Dali
