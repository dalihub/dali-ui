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
#include <dali/public-api/common/dali-string.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>

namespace Dali
{
namespace Ui
{
namespace Text
{

/**
 * @brief Partial font attribute overrides used by FontSpan.
 *
 * A default-constructed FontAttributes object is an empty override set, not a
 * complete default font style. Only attributes explicitly set with SetXXX()
 * are applied by FontSpan. Use Has() to distinguish an explicitly set value
 * from a stored value that simply looks empty or default.
 */
class DALI_UI_API FontAttributes
{
public:
  /**
   * @brief Font attribute identifiers used by Has() and Unset().
   */
  enum class Attribute : uint32_t
  {
    FAMILY,
    SIZE,
    WEIGHT,
    WIDTH,
    SLANT,
  };

public:
  /**
   * @brief Creates an empty font override set.
   */
  FontAttributes();

  /**
   * @brief Creates a copy of another font override set.
   *
   * @param[in] rhs The font attributes to copy
   */
  FontAttributes(const FontAttributes& rhs);

  /**
   * @brief Creates font attributes by moving another font override set.
   *
   * @param[in] rhs The font attributes to move
   */
  FontAttributes(FontAttributes&& rhs) noexcept;

  /**
   * @brief Copies another font override set to this object.
   *
   * @param[in] rhs The font attributes to copy
   * @return This object
   */
  FontAttributes& operator=(const FontAttributes& rhs);

  /**
   * @brief Moves another font override set to this object.
   *
   * @param[in] rhs The font attributes to move
   * @return This object
   */
  FontAttributes& operator=(FontAttributes&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~FontAttributes();

  /**
   * @brief Sets an explicit font family override.
   *
   * Passing an empty string still defines the FAMILY attribute. Use Unset() to
   * remove the override.
   *
   * @param[in] family The font family name
   */
  void SetFamily(const Dali::String& family);

  /**
   * @brief Gets the stored font family value.
   *
   * Use Has(Attribute::FAMILY) to check whether this value is applied as an
   * override.
   *
   * @return The stored font family value
   */
  Dali::String GetFamily() const;

  /**
   * @brief Sets an explicit font size override in pixels.
   *
   * @param[in] size The font size in pixels
   */
  void SetSize(float size);

  /**
   * @brief Gets the stored font size value in pixels.
   *
   * Use Has(Attribute::SIZE) to check whether this value is applied as an
   * override.
   *
   * @return The stored font size value
   */
  float GetSize() const;

  /**
   * @brief Sets an explicit font weight override.
   *
   * @param[in] weight The font weight
   */
  void SetWeight(FontWeight weight);

  /**
   * @brief Gets the stored font weight value.
   *
   * @return The stored font weight value
   */
  FontWeight GetWeight() const;

  /**
   * @brief Sets an explicit font width override.
   *
   * @param[in] width The font width
   */
  void SetWidth(FontWidth width);

  /**
   * @brief Gets the stored font width value.
   *
   * @return The stored font width value
   */
  FontWidth GetWidth() const;

  /**
   * @brief Sets an explicit font slant override.
   *
   * @param[in] slant The font slant
   */
  void SetSlant(FontSlant slant);

  /**
   * @brief Gets the stored font slant value.
   *
   * @return The stored font slant value
   */
  FontSlant GetSlant() const;

  /**
   * @brief Checks whether a specific attribute is explicitly set.
   *
   * @param[in] attribute The attribute to check
   * @return true if the attribute is explicitly set
   */
  bool Has(Attribute attribute) const;

  /**
   * @brief Checks whether any font attribute is explicitly set.
   *
   * This does not inspect whether stored values look empty or default. For
   * example, SetFamily("") makes FAMILY explicitly set.
   *
   * @return true if one or more attributes are explicitly set
   */
  bool HasAttributes() const;

  /**
   * @brief Removes one explicit attribute override.
   *
   * This does not reset the stored value to a default font value; it only
   * removes the attribute from the override set.
   *
   * @param[in] attribute The attribute to unset
   */
  void Unset(Attribute attribute);

  /**
   * @brief Compares two font override sets.
   *
   * The explicitly defined attribute set must match. Values are compared only
   * for attributes that are explicitly defined.
   *
   * @param[in] rhs The font attributes to compare with
   * @return true if both font override sets are equal
   */
  bool operator==(const FontAttributes& rhs) const;

  /**
   * @brief Compares two font override sets.
   *
   * @param[in] rhs The font attributes to compare with
   * @return true if both font override sets are not equal
   */
  bool operator!=(const FontAttributes& rhs) const;

private:
  class Impl;
  Impl* mImpl{nullptr};
};

} // namespace Text
} // namespace Ui
} // namespace Dali
