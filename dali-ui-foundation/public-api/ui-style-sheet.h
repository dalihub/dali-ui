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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/ui-style-key.h>
#include <dali-ui-foundation/public-api/ui-style.h>

namespace Dali
{
namespace Ui
{

namespace Internal
{
class UiStyleSheetImpl;
} // namespace Internal

/**
 * @brief Collection of style creators keyed by UiStyleKey.
 *
 * A style sheet is mutable until it is frozen by UiConfig::Apply(). Once
 * frozen, calling SetStyle() triggers an assertion failure.
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
   * @pre The style sheet must not be frozen.
   *
   * @param[in] key The style key
   * @param[in] creator A function pointer that creates the style
   */
  void SetStyle(UiStyleKey key, UiStyleCreator creator);

  /**
   * @brief Gets the style registered for @p key.
   *
   * If no creator is found, an uninitialized UiStyle is returned.
   * The creator is called only on the first lookup for the entry. The returned
   * style is cached for later lookups, even if the returned handle is
   * uninitialized.
   *
   * @param[in] key The style key
   * @return The resolved style, or an uninitialized handle
   */
  UiStyle GetStyle(UiStyleKey key) const;

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
};

} // namespace Ui
} // namespace Dali
