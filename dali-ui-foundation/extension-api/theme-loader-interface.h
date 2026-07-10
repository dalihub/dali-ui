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
#include <dali/public-api/common/dali-string-view.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/signals/dali-signal.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{

/**
 * @brief Abstract interface for theme-based color loading.
 *
 * Extension or derived config implementations implement this interface.
 * A concrete instance is created via UiConfigImpl::CreateThemeLoader().
 *
 * The virtual method set is fixed for ABI stability. Future extensions
 * should be added as non-virtual helper methods only.
 */
class DALI_UI_API ThemeLoaderInterface
{
public:
  /**
   * @brief Signal type emitted when the current theme changes.
   */
  using ThemeChangedSignalType = Signal<void()>;

  virtual ~ThemeLoaderInterface() = default;

  /**
   * @brief Looks up a color by its identifier.
   *
   * @param[in] colorId The color identifier (e.g. "Primary", "Background")
   * @param[out] outColor The resolved RGBA value if found
   * @return True if the color was found
   */
  virtual bool GetColor(StringView colorId, Vector4& outColor) = 0;

  /**
   * @brief Returns the identifier of the current theme.
   *
   * @return The theme identifier, or an empty string if none
   */
  virtual String GetCurrentThemeId() const = 0;

  /**
   * @brief Returns the signal emitted when the theme changes.
   *
   * @return A reference to the theme-changed signal
   */
  virtual ThemeChangedSignalType& ThemeChangedSignal() = 0;

protected:
  ThemeLoaderInterface() = default;

private:
  ThemeLoaderInterface(const ThemeLoaderInterface&)            = delete;
  ThemeLoaderInterface& operator=(const ThemeLoaderInterface&) = delete;
};

} // namespace Ui
} // namespace Dali
