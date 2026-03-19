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
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/callback.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{
namespace Ui
{

namespace Internal
{
class UiColorManagerImpl;
}

/**
 * @brief Function pointer type for overriding theme color lookups.
 *
 * When registered via SetColorOverride(), this function is called before
 * querying the theme loader for every color lookup. If the function
 * returns @c true, the output color is used and the theme loader is
 * skipped. If it returns @c false, the normal theme lookup proceeds.
 *
 * @param[in] colorId The color identifier being looked up
 * @param[out] outColor The overridden RGBA value (only used when returning true)
 * @return @c true if the color is overridden, @c false to fall through to the theme
 */
using ColorOverrideFunc = bool (*)(StringView colorId, Vector4& outColor);

/**
 * @brief Provides public access to the global color table.
 *
 * UiColorManager is a Handle-based singleton that exposes color table
 * operations (query, update) to application developers. The underlying
 * color table and View binding management is handled internally.
 *
 * @code
 *   // Query a color
 *   Vector4 bg = UiColorManager::Get().GetColor("bg_primary");
 * @endcode
 */
class DALI_UI_API UiColorManager : public BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized UiColorManager handle.
   */
  UiColorManager();

  /**
   * @brief Destructor.
   */
  ~UiColorManager() = default;

  /**
   * @brief Copy constructor.
   *
   * @param[in] handle Handle to copy
   */
  UiColorManager(const UiColorManager& handle) = default;

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  UiColorManager(UiColorManager&& rhs) noexcept = default;

  /**
   * @brief Copy assignment operator.
   *
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  UiColorManager& operator=(const UiColorManager& handle) = default;

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  UiColorManager& operator=(UiColorManager&& rhs) noexcept = default;

  /**
   * @brief Returns the singleton UiColorManager instance.
   *
   * Creates the instance on first call.
   *
   * @return A handle to the UiColorManager singleton
   */
  static UiColorManager Get();

  /**
   * @brief Downcasts a handle to a UiColorManager handle.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a UiColorManager or an uninitialized handle
   */
  static UiColorManager DownCast(BaseHandle handle);

  /**
   * @brief Looks up a single color by its identifier.
   *
   * @param[in] colorId The color identifier
   * @return The resolved RGBA value, or Vector4::ZERO if not found
   */
  Vector4 GetColor(StringView colorId) const;

  /**
   * @brief Looks up a color by its identifier.
   *
   * @param[in] colorId The color identifier
   * @param[out] outColor The resolved RGBA value if found
   * @return True if the color was found
   */
  bool GetColor(StringView colorId, Vector4& outColor) const;

  /**
   * @brief Updates color binding for a View.
   *
   * If the UiColor has a color ID, registers or updates the binding so the
   * View is refreshed when the theme changes. If the UiColor has direct RGBA
   * values, removes any existing binding for this View+applyFunc pair.
   * Does not execute the callback immediately.
   *
   * @param[in] color The UiColor to bind
   * @param[in] view The target View
   * @param[in] func A free function with signature @c void(const @c Vector4&),
   *                  invoked on color table change to apply the resolved color
   *
   * @code
   *   void SetColor(const Vector4& color) { ... }
   *   manager.UpdateBinding(color, view, SetColor);
   * @endcode
   */
  void UpdateBinding(const UiColor& color, View view, void (*func)(const Vector4&))
  {
    UpdateBinding(color, view, MakeCallback(func));
  }

  /**
   * @brief Updates color binding for a View.
   *
   * @param[in] color The UiColor to bind
   * @param[in] view The target View
   * @param[in] obj The object instance
   * @param[in] func A member function with signature @c void(const @c Vector4&),
   *                  invoked on color table change to apply the resolved color
   *
   * @code
   *   manager.UpdateBinding(color, view, this, &MyImpl::SetColor);
   * @endcode
   */
  template<class X>
  void UpdateBinding(const UiColor& color, View view, X* obj, void (X::*func)(const Vector4&))
  {
    UpdateBinding(color, view, MakeCallback(obj, func));
  }

  /**
   * @brief Retrieves the UiColor associated with a specific View+applyFunc binding.
   *
   * @param[in] view The target View
   * @param[in] func Free function matching the one used in UpdateBinding
   * @param[out] outColor The bound UiColor if found
   * @return @c true if a binding was found, @c false otherwise
   */
  bool GetBindingColor(View view, void (*func)(const Vector4&), UiColor& outColor) const
  {
    return GetBindingColor(view, MakeCallback(func), outColor);
  }

  /**
   * @brief Retrieves the UiColor associated with a specific View+applyFunc binding.
   *
   * @param[in] view The target View
   * @param[in] obj The object instance
   * @param[in] func Member function matching the one used in UpdateBinding
   * @param[out] outColor The bound UiColor if found
   * @return @c true if a binding was found, @c false otherwise
   */
  template<class X>
  bool GetBindingColor(View view, X* obj, void (X::*func)(const Vector4&), UiColor& outColor) const
  {
    return GetBindingColor(view, MakeCallback(obj, func), outColor);
  }

  /**
   * @brief Removes a specific binding for a View+applyFunc pair.
   *
   * @param[in] view The View to unbind
   * @param[in] func Free function matching the one used in UpdateBinding
   */
  void RemoveBinding(View view, void (*func)(const Vector4&))
  {
    RemoveBinding(view, MakeCallback(func));
  }

  /**
   * @brief Removes a specific binding for a View+applyFunc pair.
   *
   * @param[in] view The View to unbind
   * @param[in] obj The object instance
   * @param[in] func Member function matching the one used in UpdateBinding
   */
  template<class X>
  void RemoveBinding(View view, X* obj, void (X::*func)(const Vector4&))
  {
    RemoveBinding(view, MakeCallback(obj, func));
  }

  /**
   * @brief Removes all bindings associated with a given View.
   *
   * @param[in] view The View to unbind completely
   */
  void RemoveBindings(View view);

  /**
   * @brief Sets a function that overrides theme color lookups.
   *
   * When set, this function is invoked before querying the theme loader
   * for every color lookup. If the function returns @c true, the output
   * color is used and the theme loader is skipped. If it returns @c false,
   * the normal theme lookup proceeds.
   *
   * Setting the override immediately refreshes all existing View bindings
   * so that the overridden colors are applied without delay.
   *
   * @note This parameter is a plain function pointer (ColorOverrideFunc).
   * Lambdas with captures and member function pointers are not accepted.
   * Only free functions or stateless lambdas may be used.
   *
   * @note Passing @c nullptr clears the override, equivalent to calling
   * ClearColorOverride(). All View bindings are refreshed to use theme values.
   *
   * @note The function may be called on any thread that invokes GetColor()
   * or triggers a theme change. The implementation must be thread-safe if
   * colors are queried from multiple threads.
   *
   * @param[in] func A function pointer matching the ColorOverrideFunc signature,
   *                  or @c nullptr to clear the override
   *
   * @code
   *   bool MyOverride(StringView id, Vector4& out) {
   *     if(id == "Primary") { out = Vector4(1,0,0,1); return true; }
   *     return false;
   *   }
   *   UiColorManager::Get().SetColorOverride(MyOverride);
   * @endcode
   */
  void SetColorOverride(ColorOverrideFunc func);

  /**
   * @brief Clears the color override callback.
   *
   * All View bindings are immediately refreshed to use the theme values.
   */
  void ClearColorOverride();

public: // Not intended for Application developers
  /// @cond internal
  explicit UiColorManager(Internal::UiColorManagerImpl* impl);
  /// @endcond

private:
  void UpdateBinding(const UiColor& color, View view, CallbackBase* applyFunc);
  bool GetBindingColor(View view, CallbackBase* applyFunc, UiColor& outColor) const;
  void RemoveBinding(View view, CallbackBase* applyFunc);
};

} // namespace Ui
} // namespace Dali
