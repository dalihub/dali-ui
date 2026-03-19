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
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/signals/callback.h>
#include <dali/public-api/signals/slot-delegate.h>
#include <memory>
#include <string>
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
   * @copydoc Dali::Ui::UiColorManager::GetColor(StringView colorId)
   */
  Vector4 GetColor(StringView colorId) const;

  /**
   * @copydoc Dali::Ui::UiColorManager::GetColor(StringView colorId, Vector4& outColor)
   */
  bool GetColor(StringView colorId, Vector4& outColor) const;

  /**
   * @copydoc Dali::Ui::UiColorManager::UpdateBinding
   */
  void UpdateBinding(const UiColor& color, View view, CallbackBase* applyFunc);

  /**
   * @copydoc Dali::Ui::UiColorManager::GetBindingColor
   */
  bool GetBindingColor(View view, CallbackBase* applyFunc, UiColor& outColor) const;

  /**
   * @copydoc Dali::Ui::UiColorManager::RemoveBinding
   */
  void RemoveBinding(View view, CallbackBase* applyFunc);

  /**
   * @copydoc Dali::Ui::UiColorManager::RemoveBindings
   */
  void RemoveBindings(View view);

  /**
   * @copydoc Dali::Ui::UiColorManager::SetColorOverride
   */
  void SetColorOverride(ColorOverrideFunc func);

  /**
   * @copydoc Dali::Ui::UiColorManager::ClearColorOverride
   */
  void ClearColorOverride();

protected:
  UiColorManagerImpl();
  ~UiColorManagerImpl() override;

private:
  UiColorManagerImpl(const UiColorManagerImpl&)            = delete;
  UiColorManagerImpl(UiColorManagerImpl&&)                 = delete;
  UiColorManagerImpl& operator=(const UiColorManagerImpl&) = delete;
  UiColorManagerImpl& operator=(UiColorManagerImpl&&)      = delete;

private:
  struct BindingInfo
  {
    std::unique_ptr<CallbackBase> applyFunc;
    UiColor                       color;
  };

  struct ViewBinding
  {
    WeakHandle<View>         weakView;
    std::vector<BindingInfo> bindings;
  };

  void               OnThemeChanged();
  void               RefreshBindings();
  void               EraseBinding(View view, const CallbackBase& callback);
  const BindingInfo* FindBinding(View view, const CallbackBase& callback) const;

  std::unordered_map<void*, ViewBinding> mBindings;
  ColorOverrideFunc                      mColorOverride{nullptr};
  SlotDelegate<UiColorManagerImpl>       mSlotDelegate{this};
  bool                                   mIsApplying{false};
  bool                                   mConnected{false};
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
