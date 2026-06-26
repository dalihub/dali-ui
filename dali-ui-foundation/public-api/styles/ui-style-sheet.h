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
#include <dali/public-api/object/base-handle.h>
#include <type_traits>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/styles/ui-style-key.h>
#include <dali-ui-foundation/public-api/styles/ui-style.h>

namespace Dali
{
namespace Ui
{

namespace Internal
{
class UiStyleSheetImpl;

using UiStyleValidator = bool (*)(BaseHandle handle);
} // namespace Internal

/**
 * @brief Collection of style creators keyed by UiStyleKey.
 *
 * A style sheet is mutable until it is frozen by UiConfig::Apply(). Once
 * frozen, calling SetStyle() triggers an assertion failure. Style creators are
 * registered before Apply(), and resolved after Apply() when components request
 * style objects.
 */
class DALI_UI_API UiStyleSheet : public BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized UiStyleSheet handle.
   */
  UiStyleSheet() = default;

  /**
   * @brief Creates an initialized, mutable style sheet.
   *
   * @return A new style sheet
   */
  static UiStyleSheet New();

  /**
   * @brief Registers a style creator for @p key.
   *
   * The creator must return a style compatible with @p key's StyleType.
   * UiStyleCreator itself returns UiStyle, so this is not enforced at
   * registration time. The created style is type-checked when the entry is first
   * resolved through GetStyle().
   *
   * @pre The style sheet must not be frozen.
   *
   * @param[in] key The style key
   * @param[in] creator A function pointer that creates the style
   * @return This style sheet, for chained SetStyle() calls
   */
  template<typename StyleType>
  UiStyleSheet& SetStyle(UiStyleKey<StyleType> key, UiStyleCreator creator)
  {
    static_assert(std::is_base_of<UiStyle, StyleType>::value, "StyleType must inherit UiStyle");
    SetStyle(key.value, creator);
    return *this;
  }

  /**
   * @brief Gets the style registered for @p key.
   *
   * If no creator is found, an uninitialized UiStyle is returned.
   * The creator is called only on the first lookup for the entry. The returned
   * style is cached for later lookups, even if the returned handle is
   * uninitialized.
   *
   * @pre UiConfig::Apply() must have been called before resolving style objects.
   * @pre StyleType must inherit UiStyle and provide static DownCast(BaseHandle)
   *      and StaticDownCast(UiStyle). DownCast() validates the creator result
   *      on first resolution, and StaticDownCast() converts the verified cached
   *      UiStyle without repeated dynamic_cast.
   *
   * @param[in] key The style key
   * @return The resolved style, or an uninitialized handle
   */
  template<typename StyleType>
  StyleType GetStyle(UiStyleKey<StyleType> key) const
  {
    static_assert(std::is_base_of<UiStyle, StyleType>::value, "StyleType must inherit UiStyle");

    UiStyle style = GetStyle(
      key.value,
      [](BaseHandle handle)
    {
      return static_cast<bool>(StyleType::DownCast(handle));
    });
    if(!style)
    {
      return StyleType();
    }
    return StyleType::StaticDownCast(style);
  }

  /**
   * @brief Returns whether this style sheet is frozen.
   *
   * @return True if frozen
   */
  bool IsFrozen() const;

  /**
   * @brief Freezes this style sheet.
   *
   * Setter calls after this point trigger an assertion failure.
   */
  DALI_INTERNAL void Freeze();

private:
  explicit UiStyleSheet(Internal::UiStyleSheetImpl* impl);

  void    SetStyle(uint32_t keyValue, UiStyleCreator creator);
  UiStyle GetStyle(uint32_t keyValue, Internal::UiStyleValidator validator) const;
};

} // namespace Ui
} // namespace Dali
