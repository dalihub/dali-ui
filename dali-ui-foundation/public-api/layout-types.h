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
#include <dali/public-api/common/extents.h>
#include <dali/public-api/math/vector2.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{

/**
 * @brief Enumeration for layout alignment options.
 */
enum class LayoutAlignment : uint8_t
{
  /**
   * @brief Fill the available space
   */
  FILL = 0,
  /**
   * @brief Align to the start (left/top)
   */
  START = 1,
  /**
   * @brief Align to the center
   */
  CENTER = 2,
  /**
   * @brief Align to the end (right/bottom)
   */
  END = 3
};

/**
 * @brief Namespace containing LayoutDimension constants.
 *
 * Use these constants for LayoutWidth and LayoutHeight.
 */
namespace LayoutDimension
{
constexpr float WrapContent = -1.0f; ///< Size determined by content
constexpr float MatchParent = -2.0f; ///< Size matches parent container
} // namespace LayoutDimension

/**
 * @brief Class representing measured LayoutDimensions.
 *
 * This class stores the measured width and height of a view.
 */
class DALI_UI_API MeasuredSize
{
public:
  /**
   * @brief Default constructor.
   */
  MeasuredSize()
  : width(0.0f),
    height(0.0f)
  {
  }

  /**
   * @brief Constructor with width and height.
   *
   * @param[in] w The width value
   * @param[in] h The height value
   */
  MeasuredSize(float w, float h)
  : width(w),
    height(h)
  {
  }

  /**
   * @brief Gets the width.
   * @return The width value
   */
  float GetWidth() const
  {
    return width;
  }

  /**
   * @brief Sets the width.
   * @param[in] w The width value
   */
  void SetWidth(float w)
  {
    width = w;
  }

  /**
   * @brief Gets the height.
   * @return The height value
   */
  float GetHeight() const
  {
    return height;
  }

  /**
   * @brief Sets the height.
   * @param[in] h The height value
   */
  void SetHeight(float h)
  {
    height = h;
  }

  /**
   * @brief Converts to Vector2.
   * @return Vector2 representation
   */
  Vector2 ToVector2() const
  {
    return Vector2(width, height);
  }

  float width;  ///< Measured width
  float height; ///< Measured height
};

/**
 * @brief Class representing a rectangle for layout positioning and sizing.
 */
class DALI_UI_API LayoutRect
{
public:
  /**
   * @brief Default constructor.
   */
  LayoutRect()
  : x(0.0f),
    y(0.0f),
    width(0.0f),
    height(0.0f)
  {
  }

  /**
   * @brief Constructor with position and size.
   *
   * @param[in] posX X position
   * @param[in] posY Y position
   * @param[in] w Width
   * @param[in] h Height
   */
  LayoutRect(float posX, float posY, float w, float h)
  : x(posX),
    y(posY),
    width(w),
    height(h)
  {
  }

  /**
   * @brief Gets the X position.
   */
  float GetX() const
  {
    return x;
  }

  /**
   * @brief Sets the X position.
   */
  void SetX(float posX)
  {
    x = posX;
  }

  /**
   * @brief Gets the Y position.
   */
  float GetY() const
  {
    return y;
  }

  /**
   * @brief Sets the Y position.
   */
  void SetY(float posY)
  {
    y = posY;
  }

  /**
   * @brief Gets the width.
   */
  float GetWidth() const
  {
    return width;
  }

  /**
   * @brief Sets the width.
   */
  void SetWidth(float w)
  {
    width = w;
  }

  /**
   * @brief Gets the height.
   */
  float GetHeight() const
  {
    return height;
  }

  /**
   * @brief Sets the height.
   */
  void SetHeight(float h)
  {
    height = h;
  }

  /**
   * @brief Gets the position as Vector2.
   */
  Vector2 GetPosition() const
  {
    return Vector2(x, y);
  }

  /**
   * @brief Gets the size as Vector2.
   */
  Vector2 GetSize() const
  {
    return Vector2(width, height);
  }

  float x;      ///< X position
  float y;      ///< Y position
  float width;  ///< Width
  float height; ///< Height
};

/**
 * @brief Enumeration for stack layout orientation.
 */
enum class StackOrientation : uint8_t
{
  /**
   * @brief Vertical stacking (top to bottom)
   */
  VERTICAL = 0,
  /**
   * @brief Horizontal stacking (left to right)
   */
  HORIZONTAL = 1
};

/**
 * @brief Enumeration for flex layout direction.
 */
enum class FlexDirection : uint8_t
{
  /**
   * @brief Left to right
   */
  ROW = 0,
  /**
   * @brief Right to left
   */
  ROW_REVERSE = 1,
  /**
   * @brief Top to bottom
   */
  COLUMN = 2,
  /**
   * @brief Bottom to top
   */
  COLUMN_REVERSE = 3
};

/**
 * @brief Enumeration for flex wrap behavior.
 */
enum class FlexWrap : uint8_t
{
  /**
   * @brief No wrapping
   */
  NO_WRAP = 0,
  /**
   * @brief Normal wrapping
   */
  WRAP = 1,
  /**
   * @brief Reverse wrapping
   */
  WRAP_REVERSE = 2
};

/**
 * @brief Enumeration for flex justify content.
 */
enum class FlexJustify : uint8_t
{
  /**
   * @brief Align to start
   */
  FLEX_START = 0,
  /**
   * @brief Align to end
   */
  FLEX_END = 1,
  /**
   * @brief Center alignment
   */
  CENTER = 2,
  /**
   * @brief Space between items
   */
  SPACE_BETWEEN = 3,
  /**
   * @brief Space around items
   */
  SPACE_AROUND = 4,
  /**
   * @brief Even spacing
   */
  SPACE_EVENLY = 5
};

/**
 * @brief Enumeration for flex alignment.
 */
enum class FlexAlign : uint8_t
{
  /**
   * @brief Auto alignment
   */
  AUTO = 0,
  /**
   * @brief Align to start
   */
  FLEX_START = 1,
  /**
   * @brief Align to end
   */
  FLEX_END = 2,
  /**
   * @brief Center alignment
   */
  CENTER = 3,
  /**
   * @brief Stretch to fill
   */
  STRETCH = 4,
  /**
   * @brief Baseline alignment
   */
  BASELINE = 5
};

/**
 * @brief Enumeration for grid length types.
 */
enum class GridLengthType : uint8_t
{
  /**
   * @brief Absolute pixel value
   */
  ABSOLUTE = 0,
  /**
   * @brief Proportional (star) value
   */
  STAR = 1,
  /**
   * @brief Auto sizing
   */
  AUTO = 2
};

/**
 * @brief Class representing grid length specification.
 */
class DALI_UI_API GridLength
{
public:
  /**
   * @brief Default constructor (creates absolute 0).
   */
  GridLength()
  : mType(GridLengthType::ABSOLUTE),
    mValue(0.0f)
  {
  }

  /**
   * @brief Creates an absolute grid length.
   *
   * @param[in] pixels The pixel value
   * @return GridLength instance
   */
  static GridLength Absolute(float pixels)
  {
    GridLength length;
    length.mType  = GridLengthType::ABSOLUTE;
    length.mValue = pixels;
    return length;
  }

  /**
   * @brief Creates a star (proportional) grid length.
   *
   * @param[in] factor The star factor (default 1.0)
   * @return GridLength instance
   */
  static GridLength Star(float factor = 1.0f)
  {
    GridLength length;
    length.mType  = GridLengthType::STAR;
    length.mValue = factor;
    return length;
  }

  /**
   * @brief Creates an auto grid length.
   *
   * @return GridLength instance
   */
  static GridLength Auto()
  {
    GridLength length;
    length.mType  = GridLengthType::AUTO;
    length.mValue = 0.0f;
    return length;
  }

  /**
   * @brief Gets the type.
   */
  GridLengthType GetType() const
  {
    return mType;
  }

  /**
   * @brief Gets the value.
   */
  float GetValue() const
  {
    return mValue;
  }

private:
  GridLengthType mType;
  float          mValue;
};

/**
 * @brief Enumeration for absolute layout flags.
 */
enum class AbsoluteLayoutFlags : uint8_t
{
  /**
   * @brief No flags set
   */
  NONE = 0x0,
  /**
   * @brief Position is proportional to parent size
   */
  POSITION_PROPORTIONAL = 0x1,
  /**
   * @brief Size is proportional to parent size
   */
  SIZE_PROPORTIONAL = 0x2,
  /**
   * @brief All flags set
   */
  ALL = POSITION_PROPORTIONAL | SIZE_PROPORTIONAL
};

/**
 * @brief Bitwise OR operator for AbsoluteLayoutFlags.
 */
inline AbsoluteLayoutFlags operator|(AbsoluteLayoutFlags lhs, AbsoluteLayoutFlags rhs)
{
  return static_cast<AbsoluteLayoutFlags>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}

/**
 * @brief Bitwise AND operator for AbsoluteLayoutFlags.
 */
inline AbsoluteLayoutFlags operator&(AbsoluteLayoutFlags lhs, AbsoluteLayoutFlags rhs)
{
  return static_cast<AbsoluteLayoutFlags>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}

/**
 * @brief Enumeration for identifying layout parameter types.
 *
 * Used by View::GetLayoutParams<T>() and View::SetLayoutParams()
 * to look up the correct layout parameters attached to a View.
 */
enum class LayoutParamsType : uint8_t
{
  ABSOLUTE = 0,
  STACK    = 1,
  GRID     = 2,
  FLEX     = 3
};

} // namespace Ui
} // namespace Dali
