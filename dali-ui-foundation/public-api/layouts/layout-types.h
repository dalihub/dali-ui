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
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/signals/callback.h>
#include <cfloat>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace DALI_NAMESPACE
{
namespace Ui
{

class View;

/**
 * @brief Enumeration for layout alignment options.
 * @SINCE_3_0.0
 */
enum class LayoutAlignment : uint8_t
{
  /**
   * @brief Fill the available space
   * @SINCE_3_0.0
   */
  FILL = 0,
  /**
   * @brief Align to the start (left/top)
   * @SINCE_3_0.0
   */
  START = 1,
  /**
   * @brief Align to the center
   * @SINCE_3_0.0
   */
  CENTER = 2,
  /**
   * @brief Align to the end (right/bottom)
   * @SINCE_3_0.0
   */
  END = 3
};

/**
 * @brief Constant representing wrap-content sizing.
 *
 * When RequestedWidth or RequestedHeight is set to this value (the default),
 * the View sizes to fit its content (natural size or children bounding box).
 * @SINCE_3_0.0
 */
constexpr float WRAP_CONTENT = -1.0f;

/**
 * @brief Constant representing match-parent sizing.
 *
 * When RequestedWidth or RequestedHeight is set to this value,
 * the View fills the parent container's available space on that axis.
 * @SINCE_3_0.0
 */
constexpr float MATCH_PARENT = -2.0f;

/**
 * @brief Constant representing no minimum size constraint.
 *
 * Use this with minimum width or height APIs to remove the lower size bound.
 * @SINCE_3_0.0
 */
constexpr float UNCONSTRAINED_MIN_SIZE = 0.0f;

/**
 * @brief Constant representing no maximum size constraint.
 *
 * Use this with maximum width or height APIs to remove the upper size bound.
 * @SINCE_3_0.0
 */
constexpr float UNCONSTRAINED_MAX_SIZE = FLT_MAX;

/**
 * @brief Layout mode of a View.
 *
 * Default: the parent container's layout system manages this View's size and position
 * (measure/arrange follow the parent's layout rules and the View participates in
 * the parent's accumulation, spacing and index calculations).
 *
 * Standalone: the parent container excludes this View from its accumulation/spacing/
 * index calculations. The View's size is still measured (MATCH_PARENT, WRAP_CONTENT and
 * explicit RequestedWidth/Height work as usual): a MATCH_PARENT axis against the parent's
 * full size and a WRAP_CONTENT axis against the size of the window the View is shown in
 * (the parent's full size while it is in no window), each minus the View's own margin, and
 * an explicitly sized axis at that size. Its position is taken from the View's own
 * SetRequestedX/SetRequestedY instead of being decided by the parent's layout. Useful for
 * floating overlays, drag previews, tooltips and absolute positioning within an arbitrary
 * LayoutManager.
 * @SINCE_3_0.0
 */
enum class LayoutMode : uint8_t
{
  DEFAULT    = 0, ///< Managed by the parent container's layout (default). @SINCE_3_0.0
  STANDALONE = 1, ///< Excluded from parent's layout accumulation; uses SetRequestedX/SetRequestedY. @SINCE_3_0.0
};

/**
 * @brief Class representing measured LayoutDimensions.
 *
 * This class stores the measured width and height of a view.
 * The two public fields are a fixed ABI representation. Additional measurement
 * metadata belongs in a separate type rather than new fields in this class.
 * @SINCE_3_0.0
 */
class DALI_UI_API MeasuredSize
{
public:
  /**
   * @brief Default constructor.
   * @SINCE_3_0.0
   */
  constexpr MeasuredSize()
  : width(0.0f),
    height(0.0f)
  {
  }

  /**
   * @brief Constructor with width and height.
   *
   * @SINCE_3_0.0
   * @param[in] w The width value
   * @param[in] h The height value
   */
  constexpr MeasuredSize(float w, float h)
  : width(w),
    height(h)
  {
  }

  /**
   * @brief Gets the width.
   * @SINCE_3_0.0
   * @return The width value
   */
  constexpr float GetWidth() const
  {
    return width;
  }

  /**
   * @brief Sets the width.
   * @SINCE_3_0.0
   * @param[in] w The width value
   */
  void SetWidth(float w)
  {
    width = w;
  }

  /**
   * @brief Gets the height.
   * @SINCE_3_0.0
   * @return The height value
   */
  constexpr float GetHeight() const
  {
    return height;
  }

  /**
   * @brief Sets the height.
   * @SINCE_3_0.0
   * @param[in] h The height value
   */
  void SetHeight(float h)
  {
    height = h;
  }

  /**
   * @brief Converts to Vector2.
   * @SINCE_3_0.0
   * @return Vector2 representation
   */
  constexpr Vector2 ToVector2() const
  {
    return Vector2(width, height);
  }

  float width;  ///< Measured width
  float height; ///< Measured height
};

/**
 * @brief Class representing a rectangle for layout positioning and sizing.
 * @SINCE_3_0.0
 */
class DALI_UI_API LayoutRect
{
public:
  /**
   * @brief Default constructor.
   * @SINCE_3_0.0
   */
  constexpr LayoutRect()
  : x(0.0f),
    y(0.0f),
    width(0.0f),
    height(0.0f)
  {
  }

  /**
   * @brief Constructor with position and size.
   *
   * @SINCE_3_0.0
   * @param[in] posX X position
   * @param[in] posY Y position
   * @param[in] w Width
   * @param[in] h Height
   */
  constexpr LayoutRect(float posX, float posY, float w, float h)
  : x(posX),
    y(posY),
    width(w),
    height(h)
  {
  }

  /**
   * @brief Gets the X position.
   * @SINCE_3_0.0
   */
  constexpr float GetX() const
  {
    return x;
  }

  /**
   * @brief Sets the X position.
   * @SINCE_3_0.0
   */
  void SetX(float posX)
  {
    x = posX;
  }

  /**
   * @brief Gets the Y position.
   * @SINCE_3_0.0
   */
  constexpr float GetY() const
  {
    return y;
  }

  /**
   * @brief Sets the Y position.
   * @SINCE_3_0.0
   */
  void SetY(float posY)
  {
    y = posY;
  }

  /**
   * @brief Gets the width.
   * @SINCE_3_0.0
   */
  constexpr float GetWidth() const
  {
    return width;
  }

  /**
   * @brief Sets the width.
   * @SINCE_3_0.0
   */
  void SetWidth(float w)
  {
    width = w;
  }

  /**
   * @brief Gets the height.
   * @SINCE_3_0.0
   */
  constexpr float GetHeight() const
  {
    return height;
  }

  /**
   * @brief Sets the height.
   * @SINCE_3_0.0
   */
  void SetHeight(float h)
  {
    height = h;
  }

  /**
   * @brief Gets the position as Vector2.
   * @SINCE_3_0.0
   */
  constexpr Vector2 GetPosition() const
  {
    return Vector2(x, y);
  }

  /**
   * @brief Gets the size as Vector2.
   * @SINCE_3_0.0
   */
  constexpr Vector2 GetSize() const
  {
    return Vector2(width, height);
  }

  float x;      ///< X position
  float y;      ///< Y position
  float width;  ///< Width
  float height; ///< Height
};

// ============================================================================
// Layout Callback types
// ============================================================================

/**
 * @brief Move-only callback for custom layout measurement.
 *
 * @code
 * view.SetMeasureCallback(MeasureCallback::New(this, &MyClass::OnMeasure));
 * @endcode
 *
 * @SINCE_3_0.0
 * @note Signature: MeasuredSize(View view, float widthConstraint, float heightConstraint)
 *       Both widthConstraint and heightConstraint are visual (scale-applied) sizes,
 *       consistent with ArrangeCallback. The return value must also be visual.
 */
using MeasureCallback = Callback<MeasuredSize(View, float, float)>;

/**
 * @brief Move-only callback for custom layout arrangement.
 *
 * @code
 * view.SetArrangeCallback(ArrangeCallback::New(this, &MyClass::OnArrange));
 * @endcode
 *
 * @SINCE_3_0.0
 * @note Signature: LayoutRect(View view, const LayoutRect& bounds)
 *
 * @note A callback installed through the one-argument SetArrangeCallback() uses
 *       ArrangePolicy::IF_CHANGED. Use the two-argument overload with
 *       ArrangePolicy::ALWAYS when the callback reads state that is not
 *       tracked by layout invalidation or performs externally visible work on
 *       every arrange pass.
 */
using ArrangeCallback = Callback<LayoutRect(View, const LayoutRect&)>;

/**
 * @brief Controls when a view's arrange implementation runs.
 *
 * The framework caches arrange results. When a view is re-arranged with the same
 * bounds, the same effective layout direction and the same effective scale, and
 * nothing has invalidated its layout, the cached result may be served WITHOUT
 * running the view's arrange implementation -- its OnArrange() override, an attached
 * LayoutManager, or an ArrangeCallback set through SetArrangeCallback() -- at all.
 *
 * ArrangePolicy::IF_CHANGED is the default. An arrange implementation must use
 * ArrangePolicy::ALWAYS when it reads state outside the layout invalidation system or
 * performs work that must happen on every arrange pass.
 *
 * @SINCE_3_0.0
 * @see Ui::ViewImpl::SetArrangePolicy()
 * @see Ui::View::SetArrangeCallback(ArrangeCallback, ArrangePolicy)
 */
enum class ArrangePolicy : uint8_t
{
  /**
   * @brief Default. Runs the arrange implementation when no reusable unchanged result
   * exists.
   * @SINCE_3_0.0
   */
  IF_CHANGED = 0,

  /**
   * @brief Runs the arrange implementation on every arrange pass that reaches this view.
   * @SINCE_3_0.0
   */
  ALWAYS = 1
};

/**
 * @brief Enumeration for stack layout orientation.
 * @SINCE_3_0.0
 */
enum class StackOrientation : uint8_t
{
  /**
   * @brief Vertical stacking (top to bottom)
   * @SINCE_3_0.0
   */
  VERTICAL = 0,
  /**
   * @brief Horizontal stacking (left to right)
   * @SINCE_3_0.0
   */
  HORIZONTAL = 1
};

/**
 * @brief Enumeration for flex layout direction.
 * @SINCE_3_0.0
 */
enum class FlexDirection : uint8_t
{
  /**
   * @brief Left to right
   * @SINCE_3_0.0
   */
  ROW = 0,
  /**
   * @brief Right to left
   * @SINCE_3_0.0
   */
  ROW_REVERSE = 1,
  /**
   * @brief Top to bottom
   * @SINCE_3_0.0
   */
  COLUMN = 2,
  /**
   * @brief Bottom to top
   * @SINCE_3_0.0
   */
  COLUMN_REVERSE = 3
};

/**
 * @brief Enumeration for flex wrap behavior.
 * @SINCE_3_0.0
 */
enum class FlexWrap : uint8_t
{
  /**
   * @brief No wrapping
   * @SINCE_3_0.0
   */
  NO_WRAP = 0,
  /**
   * @brief Normal wrapping
   * @SINCE_3_0.0
   */
  WRAP = 1,
  /**
   * @brief Reverse wrapping
   * @SINCE_3_0.0
   */
  WRAP_REVERSE = 2
};

/**
 * @brief Enumeration for flex justify content.
 * @SINCE_3_0.0
 */
enum class FlexJustify : uint8_t
{
  /**
   * @brief Align to start
   * @SINCE_3_0.0
   */
  FLEX_START = 0,
  /**
   * @brief Align to end
   * @SINCE_3_0.0
   */
  FLEX_END = 1,
  /**
   * @brief Center alignment
   * @SINCE_3_0.0
   */
  CENTER = 2,
  /**
   * @brief Space between items
   * @SINCE_3_0.0
   */
  SPACE_BETWEEN = 3,
  /**
   * @brief Space around items
   * @SINCE_3_0.0
   */
  SPACE_AROUND = 4,
  /**
   * @brief Even spacing
   * @SINCE_3_0.0
   */
  SPACE_EVENLY = 5
};

/**
 * @brief Enumeration for flex alignment.
 * @SINCE_3_0.0
 */
enum class FlexAlign : uint8_t
{
  /**
   * @brief Auto alignment
   * @SINCE_3_0.0
   */
  AUTO = 0,
  /**
   * @brief Align to start
   * @SINCE_3_0.0
   */
  FLEX_START = 1,
  /**
   * @brief Align to end
   * @SINCE_3_0.0
   */
  FLEX_END = 2,
  /**
   * @brief Center alignment
   * @SINCE_3_0.0
   */
  CENTER = 3,
  /**
   * @brief Stretch to fill
   * @SINCE_3_0.0
   */
  STRETCH = 4,
  /**
   * @brief Baseline alignment
   * @SINCE_3_0.0
   */
  BASELINE = 5
};

/**
 * @brief Enumeration for grid length types.
 * @SINCE_3_0.0
 */
enum class GridLengthType : uint8_t
{
  /**
   * @brief Absolute pixel value
   * @SINCE_3_0.0
   */
  ABSOLUTE = 0,
  /**
   * @brief Proportional (star) value
   * @SINCE_3_0.0
   */
  STAR = 1,
  /**
   * @brief Auto sizing
   * @SINCE_3_0.0
   */
  AUTO = 2
};

/**
 * @brief Class representing grid length specification.
 *
 * The type and scalar value form a fixed ABI representation. Their types,
 * order and count, and the underlying type of GridLengthType, must remain
 * unchanged. Compound sizing rules or additional metadata require a separate
 * type or API. Copies store their values directly.
 * @SINCE_3_0.0
 */
class DALI_UI_API GridLength
{
public:
  /**
   * @brief Default constructor (creates absolute 0).
   * @SINCE_3_0.0
   */
  constexpr GridLength()
  : mType(GridLengthType::ABSOLUTE),
    mValue(0.0f)
  {
  }

  /**
   * @brief Creates an absolute grid length.
   *
   * @SINCE_3_0.0
   * @param[in] pixels The pixel value
   * @return GridLength instance
   */
  static constexpr GridLength Absolute(float pixels)
  {
    GridLength length;
    length.mType  = GridLengthType::ABSOLUTE;
    length.mValue = pixels;
    return length;
  }

  /**
   * @brief Creates a star (proportional) grid length.
   *
   * @SINCE_3_0.0
   * @param[in] factor The star factor (default 1.0)
   * @return GridLength instance
   */
  static constexpr GridLength Star(float factor = 1.0f)
  {
    GridLength length;
    length.mType  = GridLengthType::STAR;
    length.mValue = factor;
    return length;
  }

  /**
   * @brief Creates an auto grid length.
   *
   * @SINCE_3_0.0
   * @return GridLength instance
   */
  static constexpr GridLength Auto()
  {
    GridLength length;
    length.mType  = GridLengthType::AUTO;
    length.mValue = 0.0f;
    return length;
  }

  /**
   * @brief Gets the type.
   * @SINCE_3_0.0
   */
  constexpr GridLengthType GetType() const
  {
    return mType;
  }

  /**
   * @brief Gets the value.
   * @SINCE_3_0.0
   */
  constexpr float GetValue() const
  {
    return mValue;
  }

private:
  GridLengthType mType;
  float          mValue;
};

/**
 * @brief Enumeration for absolute layout flags.
 *
 * Flags can be combined per-axis, allowing X/Y positions and Width/Height sizes
 * to be independently specified as either absolute values or proportions of the
 * parent's content area. The composite values POSITION_PROPORTIONAL and
 * SIZE_PROPORTIONAL are provided for convenience.
 *
 * @SINCE_3_0.0
 * @note RTL mirror: these flags control only proportional resolution of the
 * bounds. They do not affect right-to-left mirroring. Under a RIGHT_TO_LEFT
 * effective layout direction a child's X is mirrored about the parent width
 * regardless of these flags; the only opt-out is LayoutMode STANDALONE.
 */
enum class AbsoluteLayoutFlags : uint8_t
{
  /**
   * @brief No flags set
   * @SINCE_3_0.0
   */
  NONE = 0x0,
  /**
   * @brief X position is proportional to parent size
   * @SINCE_3_0.0
   */
  X_PROPORTIONAL = 1 << 0,
  /**
   * @brief Y position is proportional to parent size
   * @SINCE_3_0.0
   */
  Y_PROPORTIONAL = 1 << 1,
  /**
   * @brief Width is proportional to parent size
   * @SINCE_3_0.0
   */
  WIDTH_PROPORTIONAL = 1 << 2,
  /**
   * @brief Height is proportional to parent size
   * @SINCE_3_0.0
   */
  HEIGHT_PROPORTIONAL = 1 << 3,
  /**
   * @brief Both X and Y positions are proportional to parent size
   * @SINCE_3_0.0
   */
  POSITION_PROPORTIONAL = X_PROPORTIONAL | Y_PROPORTIONAL,
  /**
   * @brief Both width and height are proportional to parent size
   * @SINCE_3_0.0
   */
  SIZE_PROPORTIONAL = WIDTH_PROPORTIONAL | HEIGHT_PROPORTIONAL,
  /**
   * @brief Every position and size value is proportional to parent size.
   * @SINCE_3_0.0
   */
  ALL = POSITION_PROPORTIONAL | SIZE_PROPORTIONAL
};

/**
 * @brief Bitwise OR operator for AbsoluteLayoutFlags.
 * @SINCE_3_0.0
 */
inline constexpr AbsoluteLayoutFlags operator|(AbsoluteLayoutFlags lhs, AbsoluteLayoutFlags rhs)
{
  return static_cast<AbsoluteLayoutFlags>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}

/**
 * @brief Bitwise AND operator for AbsoluteLayoutFlags.
 * @SINCE_3_0.0
 */
inline constexpr AbsoluteLayoutFlags operator&(AbsoluteLayoutFlags lhs, AbsoluteLayoutFlags rhs)
{
  return static_cast<AbsoluteLayoutFlags>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}

} // namespace Ui
} //namespace DALI_NAMESPACE
