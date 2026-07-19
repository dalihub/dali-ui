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
 */

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/math/vector2.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{
namespace Text
{

/**
 * @brief Provides the authored properties of an ImageSpan.
 *
 * ImageAttributes is a mutable value type used to configure an inline image
 * and its reserved image box. ImageSpan::New() snapshots the supplied values
 * and their explicitly set state. Changing this object afterward does not
 * affect an existing ImageSpan.
 *
 * A default-constructed object has every attribute unset. Its getters still
 * return their documented default values. Use Has() to distinguish an unset
 * attribute from an explicitly set value that equals the default.
 *
 * A non-empty source and a reserved size with finite, positive components are
 * required for an ImageSpan to replace its underlying text. The vertical
 * offset must also be finite. ImageAttributes does not load or own a rendered
 * image resource.
 */
class DALI_UI_API ImageAttributes
{
public:
  /**
   * @brief Specifies the vertical alignment of the reserved image box relative
   *        to surrounding text metrics.
   *
   * Alignment is not recalculated from the final line box expanded by the
   * reserved image box. A replacement-only line uses the consuming control's
   * default font metrics. The vertical offset is applied after alignment.
   */
  enum class InlineAlignment : uint8_t
  {
    TEXT_BOTTOM = 0, ///< Aligns the box bottom with the bottom of the surrounding text.
    TEXT_BASELINE,   ///< Aligns the box bottom with the surrounding text baseline.
    TEXT_CENTER      ///< Aligns the box center with the center of the surrounding text.
  };

  /**
   * @brief Identifies an attribute for Has() and Unset().
   */
  enum class Attribute : uint32_t
  {
    SOURCE,         ///< The image resource identifier.
    RESERVED_SIZE,  ///< The reserved image box size in logical pixels.
    ALIGNMENT,      ///< The vertical alignment relative to the surrounding text metrics.
    VERTICAL_OFFSET ///< The vertical offset applied after alignment, in logical pixels.
  };

  /**
   * @brief Creates image attributes with every attribute unset.
   */
  ImageAttributes();

  /**
   * @brief Creates image attributes with a source and reserved size.
   *
   * SOURCE and RESERVED_SIZE are marked as explicitly set. Other attributes
   * remain unset and return their documented default values. The supplied
   * values are stored without validation.
   *
   * @param[in] source The image resource identifier
   * @param[in] reservedSize The width and height of the reserved image box in logical pixels
   */
  ImageAttributes(const Dali::String& source, const Vector2& reservedSize);

  /**
   * @brief Creates a copy of another ImageAttributes object.
   *
   * All values and their explicitly set or unset state are copied.
   *
   * @param[in] rhs The image attributes to copy
   */
  ImageAttributes(const ImageAttributes& rhs);

  /**
   * @brief Creates image attributes by moving another ImageAttributes object.
   *
   * @param[in] rhs The image attributes to move from
   */
  ImageAttributes(ImageAttributes&& rhs) noexcept;

  /**
   * @brief Copies another ImageAttributes object to this object.
   *
   * All values and their explicitly set or unset state are copied.
   *
   * @param[in] rhs The image attributes to copy
   * @return A reference to this object
   */
  ImageAttributes& operator=(const ImageAttributes& rhs);

  /**
   * @brief Moves another ImageAttributes object to this object.
   *
   * @param[in] rhs The image attributes to move from
   * @return A reference to this object
   */
  ImageAttributes& operator=(ImageAttributes&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~ImageAttributes();

  /**
   * @brief Sets the image resource identifier.
   *
   * The identifier is stored without loading or validating the resource. An
   * empty identifier makes an ImageSpan replacement invalid. Failure to load a
   * non-empty identifier does not change the reserved image box or text
   * layout.
   *
   * @param[in] source The image resource identifier
   */
  void SetSource(const Dali::String& source);

  /**
   * @brief Retrieves the image resource identifier.
   *
   * A default-constructed object returns an empty string. Use Has(SOURCE) to
   * determine whether the identifier is explicitly set.
   *
   * @return The image resource identifier
   */
  Dali::String GetSource() const;

  /**
   * @brief Sets the size of the reserved image box.
   *
   * The width and height are measured in logical pixels. Both components must
   * be finite and greater than zero for an ImageSpan replacement to be
   * applied. Otherwise, the underlying text remains visible.
   *
   * The reserved image box participates in natural size, line height,
   * wrapping, and ellipsis independently of the resource's natural size.
   * Resource loading success or failure does not change this reservation.
   * A text control applies its effective text scale during layout without
   * changing this authored value.
   *
   * @param[in] size The width and height of the reserved image box in logical pixels
   */
  void SetReservedSize(const Vector2& size);

  /**
   * @brief Retrieves the size of the reserved image box.
   *
   * A default-constructed object returns Vector2::ZERO. Use Has(RESERVED_SIZE)
   * to determine whether the size is explicitly set.
   *
   * @return The width and height of the reserved image box in logical pixels
   */
  Vector2 GetReservedSize() const;

  /**
   * @brief Sets the vertical alignment of the reserved image box.
   *
   * The vertical offset is applied after this alignment.
   *
   * @param[in] alignment The vertical alignment relative to the surrounding text metrics
   */
  void SetAlignment(InlineAlignment alignment);

  /**
   * @brief Retrieves the vertical alignment of the reserved image box.
   *
   * A default-constructed object returns TEXT_BOTTOM. Use Has(ALIGNMENT) to
   * distinguish that default from an explicitly set TEXT_BOTTOM value.
   *
   * @return The vertical alignment
   */
  InlineAlignment GetAlignment() const;

  /**
   * @brief Sets the vertical offset of the reserved image box.
   *
   * The offset is applied after alignment for every InlineAlignment value. It
   * is measured in logical pixels. Positive values move the box downward and
   * negative values move it upward in control-local coordinates. A non-finite
   * offset makes an ImageSpan replacement invalid.
   *
   * @param[in] offset The vertical offset in logical pixels
   */
  void SetVerticalOffset(float offset);

  /**
   * @brief Retrieves the vertical offset of the reserved image box.
   *
   * A default-constructed object returns 0.0f. Use Has(VERTICAL_OFFSET) to
   * distinguish that default from an explicitly set zero value.
   *
   * @return The vertical offset in logical pixels
   */
  float GetVerticalOffset() const;

  /**
   * @brief Checks whether an attribute is explicitly set.
   *
   * This check distinguishes an unset attribute from an explicitly set value
   * that equals its documented default.
   *
   * @param[in] attribute The attribute to check
   * @return true if the attribute is explicitly set
   */
  bool Has(Attribute attribute) const;

  /**
   * @brief Checks whether any attribute is explicitly set.
   *
   * @return true if one or more attributes are explicitly set
   */
  bool HasAttributes() const;

  /**
   * @brief Marks an attribute as unset.
   *
   * Unsetting an attribute does not need to change the value returned by its
   * getter. Consumers use Has() to determine whether the value was authored
   * and ignore a value whose attribute is unset.
   *
   * @param[in] attribute The attribute to unset
   */
  void Unset(Attribute attribute);

  /**
   * @brief Compares two ImageAttributes objects for equality.
   *
   * The explicitly set attribute sets must match. Values are compared only for
   * attributes that are explicitly set. Therefore, an unset default value is
   * not equal to the same value when it is explicitly set.
   *
   * @param[in] rhs The image attributes to compare with
   * @return true if both image attribute sets are equal
   */
  bool operator==(const ImageAttributes& rhs) const;

  /**
   * @brief Compares two ImageAttributes objects for inequality.
   *
   * @param[in] rhs The image attributes to compare with
   * @return true if the image attribute sets are not equal
   */
  bool operator!=(const ImageAttributes& rhs) const;

private:
  class Impl;
  Impl* mImpl{nullptr};
};

} // namespace Text
} // namespace Ui
} // namespace Dali
