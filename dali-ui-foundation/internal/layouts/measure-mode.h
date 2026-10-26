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

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
/*
 * Vocabulary used by the measure-mode code and its documentation:
 *
 *   constraint    the scalar a parent passes per axis -- an INPUT (Measure(w, h), OnMeasure(...))
 *   mode          how the constraint binds: AT_MOST / EXACTLY / UNSPECIFIED -- an INPUT
 *                 (MeasureMode, ViewDataImpl::GetMeasureModes())
 *   determined    the axis size is final before the producer runs: an explicit (FIXED) request,
 *                 MATCH_PARENT, or EXACTLY (ViewDataImpl::IsWidthDetermined())
 *   size          the two-axis measure RESULT (MeasuredSize, GetMeasuredSize())
 *   slot / bounds what a parent hands a child at Arrange -- the OUTPUT of placement (LayoutRect)
 *   own bounds    the view's own minimum/maximum: no measured or arranged size lies outside them
 */

/**
 * @brief How a measure constraint binds the measured view on one axis.
 *
 * The well-known tri-state measure specification: the scalar constraint is either an upper
 * bound the view may size within (AT_MOST), the slot the parent has already decided the view
 * will be laid out in (EXACTLY; the view reports, and is laid out at, that size clamped by its
 * own minimum/maximum), or a budget with no binding meaning (UNSPECIFIED, a scrollable axis; the
 * number stays the huge finite value ScrollView passes). AT_MOST is deliberately the zero value:
 * a zero-initialised mode means exactly what every constraint meant before modes existed.
 *
 * Internal on purpose: the only API that carries a mode is ViewDataImpl's mode-carrying
 * Measure() overload. The public View::Measure() keeps one scalar per axis and derives the
 * mode (ViewDataImpl::DeriveMeasureModes()). The values are stable and the underlying type is
 * uint8_t, so the stored bits would not change if the enum were ever exposed.
 */
enum class MeasureMode : uint8_t
{
  AT_MOST     = 0, ///< The constraint is an upper bound; the view reports the size it wants within it.
  EXACTLY     = 1, ///< The parent has decided the slot; the view reports it clamped by its own minimum/maximum.
  UNSPECIFIED = 2  ///< No binding bound (a scrollable axis). Treated like AT_MOST by every producer for now.
};

/**
 * @brief The largest constraint an EXACTLY mode may carry.
 *
 * An exact constraint at or above this is an unbounded budget (float::max, float::max * 0.25,
 * float::max * p), not a size; it is demoted to AT_MOST so no view reports it. 1e30 lies some
 * twenty-six orders of magnitude above any layout size and still catches every
 * float::max-derived budget down to a factor of about 3e-9.
 */
constexpr float MAX_EXACT_MEASURE_SIZE = 1.0e30f;

/**
 * @brief Below this a slot handed out by a two-step measurement is empty.
 *
 * Float rounding at a UI scale that is not a power-of-two fraction leaves about 1e-6 px where a
 * container's size less its padding and the child's margin should be 0. The measure cache compares
 * its keys with the same 0.001 (ViewDataImpl's FloatEqual), but in natural units; a slot is in
 * visual units.
 */
constexpr float EMPTY_SLOT_TOLERANCE = 0.001f;

/**
 * @brief The slot @p available as is, or exactly 0 when it is empty (below EMPTY_SLOT_TOLERANCE,
 * negative included).
 *
 * @param[in] available A container's size less its padding and the child's margin, visual units
 * @return The slot, exactly 0 when it is empty
 */
inline float SnapEmptySlot(float available)
{
  return (available >= EMPTY_SLOT_TOLERANCE) ? available : 0.0f;
}

/**
 * @brief Whether @p size is narrower than @p slot by at least EMPTY_SLOT_TOLERANCE.
 *
 * @param[in] size A child's measured size on one axis, visual units
 * @param[in] slot The slot it is laid out in on that axis, visual units
 * @return True when the slot is wider than the size beyond float rounding
 */
inline bool IsNarrowerThanSlot(float size, float slot)
{
  return (slot - size) >= EMPTY_SLOT_TOLERANCE;
}

/**
 * @brief The measure modes of both axes.
 */
struct MeasureModes
{
  MeasureMode width{MeasureMode::AT_MOST};
  MeasureMode height{MeasureMode::AT_MOST};

  bool operator==(const MeasureModes& rhs) const
  {
    return width == rhs.width && height == rhs.height;
  }

  bool operator!=(const MeasureModes& rhs) const
  {
    return !(*this == rhs);
  }
};

/**
 * @brief DEBUG aid for the mode-carrying Measure().
 *
 * A first-party call site passes MATCHES_DERIVATION when the modes it passes must equal
 * ViewDataImpl::DeriveMeasureModes() for the child, i.e. when the call is the one an
 * application's mode-less View::Measure() must agree with. Intermediate calls that knowingly
 * differ (a probe before the parent's extent is resolved, a re-measure of a stretched container)
 * pass NONE. The check is compiled only into DEBUG builds.
 */
enum class MeasureModeCheck : uint8_t
{
  NONE               = 0,
  MATCHES_DERIVATION = 1
};

/**
 * @brief Which first-party producer a view is, as a PARENT, for measure-mode purposes.
 *
 * NONE covers every producer that is not one of the first-party containers: a view with a measure
 * or arrange callback, a custom or subclassed LayoutManager and every ViewImpl subclass. Such a
 * parent passes its children the custom-parent modes (ViewDataImpl::DeriveChildModesAsCustomParent()):
 * an explicit child size is used as is, and a MATCH_PARENT child is exact when the parent's own
 * size is determined (explicit, or measured EXACTLY), AT_MOST when the parent was measured with a
 * budget and UNSPECIFIED on a scroll axis.
 */
enum class MeasureModeParent : uint8_t
{
  NONE = 0,
  DEFAULT,   ///< A plain View (exactly ViewImpl) without a LayoutManager: the default measure / arrange.
  STACK,     ///< Exactly StackLayoutManager.
  GRID,      ///< Exactly GridLayoutManager.
  FLEX,      ///< Exactly FlexLayoutManager with FlexWrap::NO_WRAP.
  FLEX_WRAP, ///< Exactly FlexLayoutManager with wrapping lines: every child AT_MOST (its lines are built from the measured sizes).
  ABSOLUTE,  ///< Exactly AbsoluteLayoutManager.
  SCROLL     ///< Exactly ScrollViewLayoutManager.
};

} // namespace Internal
} // namespace Ui
} // namespace DALI_NAMESPACE
