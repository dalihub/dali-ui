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
#include <dali/public-api/signals/dali-signal.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/types/callback.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>

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
 * @brief Typed callback for applying a resolved color to a View.
 *
 * Pass to UpdateBinding() to register a binding between a UiColor and a View.
 *
 * @code
 *   manager.UpdateBinding(color, view, ColorCallback::New(this, &MyImpl::SetColor));
 * @endcode
 */
using ColorCallback = Callback<void(const Vector4&)>;

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
  using ColorTableChangedSignalType = Signal<void()>;

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
   * @brief Registers a named color binding for an object.
   *
   * Associates @a callback with the given @a bindingId. The callback is invoked
   * whenever the theme changes and a color has been set via UpdateBinding().
   * If a binding with the same @a bindingId already exists, it is replaced.
   * Does not invoke the callback immediately.
   *
   * Ownership of @a callback is transferred to the manager.
   *
   * @param[in] view      The target object
   * @param[in] bindingId A caller-defined identifier for this binding (e.g. "BackgroundColor")
   * @param[in] callback  A ColorCallback created via ColorCallback::New(),
   *                      invoked on color table change to apply the resolved color
   *
   * @code
   *   manager.RegisterBinding(Self(), "BackgroundColor", ColorCallback::New(this, &MyImpl::SetBg));
   * @endcode
   */
  void RegisterBinding(BaseHandle view, StringView bindingId, ColorCallback callback);

  /**
   * @brief Retrieves the UiColor associated with a named binding.
   *
   * @param[in]  view      The target object
   * @param[in]  bindingId The binding identifier used in UpdateBinding
   * @param[out] outColor  The bound UiColor if found
   * @return @c true if a binding was found, @c false otherwise
   */
  [[nodiscard]] bool GetBindingColor(BaseHandle view, StringView bindingId, UiColor& outColor) const;

  /**
   * @brief Returns whether a named binding exists for an object.
   *
   * @param[in] view      The target object
   * @param[in] bindingId The binding identifier
   * @return @c true if a binding with the given id is registered
   */
  [[nodiscard]] bool HasBinding(BaseHandle view, StringView bindingId) const;

  /**
   * @brief Sets the color for an existing named binding.
   *
   * Stores @a color so the callback is invoked with the resolved value on theme change.
   * If no binding with @a bindingId exists, this call is a no-op.
   *
   * @param[in] view      The target object
   * @param[in] bindingId The binding identifier used in RegisterBinding
   * @param[in] color     The new UiColor to store (must have a color ID)
   */
  void SetBindingColor(BaseHandle view, StringView bindingId, const UiColor& color);

  /**
   * @brief Removes a named binding.
   *
   * @param[in] view      The target object
   * @param[in] bindingId The binding identifier used in UpdateBinding
   */
  void ClearBinding(BaseHandle view, StringView bindingId);

  /**
   * @brief Removes all bindings associated with a given object.
   *
   * @param[in] view The target object to unbind completely
   */
  void ClearBindings(BaseHandle view);

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

  /**
   * @brief Invalidates all cached resolved colors.
   *
   * Call this when a color override function changes its internal state
   * so that subsequent UiColor::GetRgba() calls re-resolve from the
   * theme/override.
   */
  void InvalidateCache();

  /**
   * @brief Invalidates the cached resolved color for a specific UiColor.
   *
   * If the given UiColor holds a token, its cached value is cleared so
   * the next GetRgba() call re-resolves from the theme/override.
   * No-op if the UiColor holds a direct RGBA value.
   *
   * @param[in] color The UiColor whose cache entry should be invalidated
   */
  void InvalidateCache(const UiColor& color);

  /**
   * @brief Returns a signal that is emitted when the resolved color table changes.
   *
   * This signal is emitted after cached colors are invalidated and existing
   * color bindings are refreshed. It can be used by objects that store
   * compound color data, such as gradients, and need to re-resolve their own
   * UiColor values.
   *
   * @return A reference to the color-table-changed signal
   */
  ColorTableChangedSignalType& ColorTableChangedSignal();

public: // Not intended for Application developers
  /// @cond internal
  explicit UiColorManager(Internal::UiColorManagerImpl* impl);
  /// @endcond
};

} // namespace Ui
} // namespace Dali
