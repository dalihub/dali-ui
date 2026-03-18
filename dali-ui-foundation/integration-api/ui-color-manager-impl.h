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
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/signals/slot-delegate.h>
#include <dali/public-api/math/vector4.h>
#include <unordered_map>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/ui-color-manager.h>
#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

/**
 * @brief Internal implementation of UiColorManager.
 *
 * Manages color lookups via the current ThemeLoaderInterface and
 * maintains View bindings so that ID-based colors are automatically
 * refreshed when the theme changes.
 */
class DALI_UI_API UiColorManagerImpl : public BaseObject
{
public:
  /**
   * @brief Returns the singleton UiColorManager handle.
   *
   * Creates the implementation on first call.
   *
   * @return A UiColorManager handle wrapping the singleton impl
   */
  static UiColorManager Get();

  /**
   * @brief Looks up a single color by ID.
   *
   * @param[in] colorId The color identifier
   * @return The resolved RGBA value, or Vector4::ZERO if not found
   */
  Vector4 GetColor(StringView colorId) const;

  /**
   * @brief Looks up a color by ID.
   *
   * @param[in] colorId The color identifier
   * @param[out] outColor The resolved RGBA value if found
   * @return True if the color was found
   */
  bool GetColor(StringView colorId, Vector4& outColor) const;

  /**
   * @brief Resolves a UiColor and applies it to a View, managing
   * bindings automatically.
   *
   * @param[in] color The UiColor to apply
   * @param[in] view The target View
   * @param[in] applyFunc Function to apply the color to the View
   */
  void ApplyColor(const UiColor& color, View view, ColorApplyFunc applyFunc);

  /**
   * @brief Removes a specific binding for a View+applyFunc pair.
   *
   * @param[in] view The View to unbind
   * @param[in] applyFunc The specific apply function to unbind
   */
  void UnregisterBinding(View view, ColorApplyFunc applyFunc);

  /**
   * @brief Removes all bindings associated with a given View.
   *
   * @param[in] view The View to unbind completely
   */
  void UnregisterBindings(View view);

  /**
   * @brief Sets a function that overrides theme color lookups.
   *
   * @param[in] func A function pointer matching ColorOverrideFunc, or nullptr to clear
   */
  void SetColorOverride(ColorOverrideFunc func);

  /**
   * @brief Clears the color override callback.
   */
  void ClearColorOverride();

protected:
  UiColorManagerImpl();
  ~UiColorManagerImpl() override;

private:
  UiColorManagerImpl(const UiColorManagerImpl&) = delete;
  UiColorManagerImpl(UiColorManagerImpl&&) = delete;
  UiColorManagerImpl& operator=(const UiColorManagerImpl&) = delete;
  UiColorManagerImpl& operator=(UiColorManagerImpl&&) = delete;

  void OnThemeChanged();
  void RefreshBindings();

private:
  struct BindingInfo
  {
    ColorApplyFunc applyFunc;
    UiColor       color;
  };

  struct ViewBinding
  {
    WeakHandle<View> weakView;
    std::vector<BindingInfo> bindings;
  };

  std::unordered_map<void*, ViewBinding> mBindings;
  ColorOverrideFunc                      mColorOverride{nullptr};
  SlotDelegate<UiColorManagerImpl>       mSlotDelegate{this};
  bool mIsApplying{false};
  bool mConnected{false};
};

} // namespace Integration

/**
 * @brief Retrieves the UiColorManagerImpl from a UiColorManager handle.
 *
 * @param[in] obj The UiColorManager handle
 * @return A reference to the internal implementation
 */
inline Integration::UiColorManagerImpl& GetImpl(UiColorManager& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Integration::UiColorManagerImpl&>(handle);
}

/**
 * @brief Retrieves the UiColorManagerImpl from a const UiColorManager handle.
 *
 * @param[in] obj The UiColorManager handle
 * @return A const reference to the internal implementation
 */
inline const Integration::UiColorManagerImpl& GetImpl(const UiColorManager& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Integration::UiColorManagerImpl&>(handle);
}

} // namespace Ui
} // namespace Dali
