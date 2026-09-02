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
#include <utility>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/configuration/ui-color-manager.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>

namespace Dali
{
namespace Ui
{
namespace Internal
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
   * Creates the implementation on first call. Returns an uninitialized
   * handle if called after the SingletonService has been destroyed.
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
  void RegisterBinding(BaseHandle view, StringView bindingId, ColorCallback callback);

  /**
   * @copydoc Dali::Ui::UiColorManager::GetBindingColor
   */
  [[nodiscard]] bool GetBindingColor(BaseHandle view, StringView bindingId, UiColor& outColor) const;

  /**
   * @copydoc Dali::Ui::UiColorManager::HasBinding
   */
  [[nodiscard]] bool HasBinding(BaseHandle view, StringView bindingId) const;

  /**
   * @copydoc Dali::Ui::UiColorManager::SetBindingColor
   */
  void SetBindingColor(BaseHandle view, StringView bindingId, const UiColor& color);

  /**
   * @copydoc Dali::Ui::UiColorManager::ClearBinding
   */
  void ClearBinding(BaseHandle view, StringView bindingId);

  /**
   * @copydoc Dali::Ui::UiColorManager::ClearBindings(BaseHandle)
   */
  void ClearBindings(BaseHandle view);

  /**
   * @brief Removes all bindings associated with a given object.
   *
   * This overload accepts a raw pointer, avoiding the ref-count increment
   * that occurs when constructing a BaseHandle.
   * Intended for use in impl-class destructors where Self() is unavailable.
   * The pointer must be the same value as BaseHandle::GetObjectPtr() used at registration time.
   *
   * @param[in] objectPtr The target object pointer to unbind, or @c nullptr (no-op)
   */
  void ClearBindings(void* objectPtr);

  /**
   * @copydoc Dali::Ui::UiColorManager::SetColorOverride
   */
  void SetColorOverride(ColorOverrideFunc func);

  /**
   * @copydoc Dali::Ui::UiColorManager::ClearColorOverride
   */
  void ClearColorOverride();

  /**
   * @copydoc Dali::Ui::UiColorManager::InvalidateCache()
   */
  void InvalidateCache();

  /**
   * @copydoc Dali::Ui::UiColorManager::InvalidateCache(const UiColor&)
   */
  void InvalidateCache(const UiColor& color);

  /**
   * @copydoc Dali::Ui::UiColorManager::ColorTableChangedSignal
   */
  UiColorManager::ColorTableChangedSignalType& ColorTableChangedSignal();

protected:
  /**
   * @brief Constructs a new UiColorManagerImpl.
   */
  UiColorManagerImpl();

  /**
   * @brief Destructor.
   */
  ~UiColorManagerImpl() override;

private:
  UiColorManagerImpl(const UiColorManagerImpl&)            = delete;
  UiColorManagerImpl(UiColorManagerImpl&&)                 = delete;
  UiColorManagerImpl& operator=(const UiColorManagerImpl&) = delete;
  UiColorManagerImpl& operator=(UiColorManagerImpl&&)      = delete;

private:
  struct BindingInfo
  {
    ColorCallback applyFunc;
    UiColor       color;
  };

  struct ViewBinding
  {
    WeakHandle<BaseHandle>                           weakView;
    std::vector<std::pair<std::string, BindingInfo>> bindings;
  };

  void OnThemeChanged();
  void HandleColorTableUpdate();
  void RefreshBindings();

  std::unordered_map<RefObject*, ViewBinding> mBindings;
  UiColorManager::ColorTableChangedSignalType mColorTableChangedSignal;
  ColorOverrideFunc                           mColorOverride{nullptr};
  SlotDelegate<UiColorManagerImpl>            mSlotDelegate{this};
  bool                                        mIsApplying{false};
  bool                                        mConnected{false};
};

} // namespace Internal

/**
 * @brief Retrieves the UiColorManagerImpl from a UiColorManager handle.
 *
 * @param[in] obj The UiColorManager handle
 * @return A reference to the internal implementation
 */
inline Internal::UiColorManagerImpl& GetImpl(UiColorManager& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Internal::UiColorManagerImpl&>(handle);
}

/**
 * @brief Retrieves the UiColorManagerImpl from a const UiColorManager handle.
 *
 * @param[in] obj The UiColorManager handle
 * @return A const reference to the internal implementation
 */
inline const Internal::UiColorManagerImpl& GetImpl(const UiColorManager& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Internal::UiColorManagerImpl&>(handle);
}

} // namespace Ui
} // namespace Dali
