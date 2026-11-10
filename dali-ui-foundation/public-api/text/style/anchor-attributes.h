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
#include <dali-ui-foundation/public-api/types/ui-color.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Text
{

/**
 * @brief Partial anchor attribute payload used by AnchorSpan.
 *
 * A default-constructed AnchorAttributes object is an empty attribute set. Use
 * Has() to distinguish an explicitly set empty/default-looking value from an
 * unset attribute.
 * @SINCE_3_0.0
 */
class DALI_UI_API AnchorAttributes
{
public:
  /**
   * @brief Anchor attribute identifiers used by Has() and Unset().
   * @SINCE_3_0.0
   */
  enum class Attribute : uint32_t
  {
    HREF,
    COLOR,
    CLICKED_COLOR,
  };

public:
  /**
   * @brief Creates an empty anchor attribute set.
   * @SINCE_3_0.0
   */
  AnchorAttributes();

  /**
   * @brief Creates a copy of another anchor attribute set.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The anchor attributes to copy
   */
  AnchorAttributes(const AnchorAttributes& rhs);

  /**
   * @brief Creates anchor attributes by moving another anchor attribute set.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The anchor attributes to move
   */
  AnchorAttributes(AnchorAttributes&& rhs) noexcept;

  /**
   * @brief Copies another anchor attribute set to this object.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The anchor attributes to copy
   * @return This object
   */
  AnchorAttributes& operator=(const AnchorAttributes& rhs);

  /**
   * @brief Moves another anchor attribute set to this object.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The anchor attributes to move
   * @return This object
   */
  AnchorAttributes& operator=(AnchorAttributes&& rhs) noexcept;

  /**
   * @brief Destructor.
   * @SINCE_3_0.0
   */
  ~AnchorAttributes();

  /**
   * @brief Sets an explicit anchor href.
   *
   * Passing an empty string still defines the HREF attribute. Use Unset() to
   * remove the attribute.
   *
   * @SINCE_3_0.0
   * @param[in] href The href string
   */
  void SetHref(const Dali::String& href);

  /**
   * @brief Gets the stored href value.
   *
   * Use Has(Attribute::HREF) to check whether this value is applied.
   *
   * @SINCE_3_0.0
   * @return The stored href value
   */
  Dali::String GetHref() const;

  /**
   * @brief Sets an explicit normal anchor color.
   *
   * @SINCE_3_0.0
   * @param[in] color The normal anchor color
   */
  void SetColor(const UiColor& color);

  /**
   * @brief Gets the stored normal anchor color.
   *
   * @SINCE_3_0.0
   * @return The stored normal anchor color
   */
  UiColor GetColor() const;

  /**
   * @brief Sets an explicit clicked anchor color.
   *
   * @SINCE_3_0.0
   * @param[in] color The clicked anchor color
   */
  void SetClickedColor(const UiColor& color);

  /**
   * @brief Gets the stored clicked anchor color.
   *
   * @SINCE_3_0.0
   * @return The stored clicked anchor color
   */
  UiColor GetClickedColor() const;

  /**
   * @brief Checks whether a specific attribute is explicitly set.
   *
   * @SINCE_3_0.0
   * @param[in] attribute The attribute to check
   * @return true if the attribute is explicitly set
   */
  bool Has(Attribute attribute) const;

  /**
   * @brief Checks whether any anchor attribute is explicitly set.
   *
   * @SINCE_3_0.0
   * @return true if one or more attributes are explicitly set
   */
  bool HasAttributes() const;

  /**
   * @brief Removes one explicit attribute.
   *
   * This only removes the attribute from the explicit set; it does not reset the
   * stored value.
   *
   * @SINCE_3_0.0
   * @param[in] attribute The attribute to unset
   */
  void Unset(Attribute attribute);

  /**
   * @brief Compares two anchor attribute sets.
   *
   * The explicitly defined attribute set must match. Values are compared only
   * for attributes that are explicitly defined.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The anchor attributes to compare with
   * @return true if both anchor attribute sets are equal
   */
  bool operator==(const AnchorAttributes& rhs) const;

  /**
   * @brief Compares two anchor attribute sets.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The anchor attributes to compare with
   * @return true if both anchor attribute sets are not equal
   */
  bool operator!=(const AnchorAttributes& rhs) const;

private:
  class Impl;
  Impl* mImpl{nullptr};
};

} // namespace Text
} // namespace Ui
} //namespace DALI_NAMESPACE
