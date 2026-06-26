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
#include <dali/integration-api/string-utils.h>
#include <dali/public-api/common/dali-string-view.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/math/vector4.h>
#include <unordered_map>

// INTERNAL INCLUDES
#include <dali-ui-foundation/provider-api/theme-loader-interface.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

/**
 * @brief Built-in default ThemeLoaderInterface implementation.
 *
 * Provides a minimal set of hardcoded colors used as fallback
 * when UiConfigImpl::CreateThemeLoader() returns nullptr.
 */
class DALI_UI_API DefaultThemeLoader : public ThemeLoaderInterface
{
public:
  /**
   * @brief Constructs the default loader with built-in primary, background, and outline colors.
   */
  DefaultThemeLoader();

  /**
   * @brief Destructor.
   */
  ~DefaultThemeLoader() override;

  /**
   * @brief Looks up a color by its identifier.
   *
   * @param[in] colorId The color identifier
   * @param[out] outColor The resolved RGBA value if found
   * @return True if the color was found
   */
  bool GetColor(StringView colorId, Vector4& outColor) override;

  /**
   * @brief Returns the identifier of the current theme.
   *
   * @return The theme identifier, or an empty string
   */
  String GetCurrentThemeId() const override;

  /**
   * @brief Returns the signal emitted when the theme changes.
   *
   * @return A reference to the theme-changed signal
   */
  ThemeChangedSignalType& ThemeChangedSignal() override;

private:
  /**
   * @brief Transparent hasher that accepts both String and StringView
   * without requiring a temporary String allocation.
   */
  struct TransparentHash
  {
    using is_transparent = void;
    std::size_t operator()(const String& s) const noexcept
    {
      return std::hash<String>()(s);
    }
    std::size_t operator()(StringView sv) const noexcept
    {
      return std::hash<StringView>()(sv);
    }
  };

  /**
   * @brief Transparent equality that accepts both String and StringView.
   */
  struct TransparentEqual
  {
    using is_transparent = void;
    bool operator()(const String& a, const String& b) const
    {
      return a == b;
    }
    bool operator()(const String& a, StringView b) const
    {
      return a == b;
    }
    bool operator()(StringView a, const String& b) const
    {
      return b == a;
    }
  };

  std::unordered_map<String, Vector4, TransparentHash, TransparentEqual> mColors;
  ThemeChangedSignalType                                                 mThemeChangedSignal;
};

} // namespace Integration
} // namespace Ui
} // namespace Dali
