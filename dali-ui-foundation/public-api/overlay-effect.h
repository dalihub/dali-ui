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
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/math/vector4.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/state-effect.h>
#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali-ui-foundation/public-api/view-types.h>

namespace Dali
{
namespace Ui
{

namespace Internal
{
class OverlayEffectImpl;
}

/**
 * @brief Specifies which View receives the overlay effect recoil feedback.
 */
enum class RecoilScope
{
  OVERLAY_TARGET          = 0, ///< Recoil the overlay target itself
  OVERLAY_TARGET_CHILDREN = 1, ///< Recoil the overlay target's direct children when it has one to three direct children
};

/**
 * @brief State effect that shows a dim overlay while a View is pressed or focus-indicated.
 *
 * By default, OverlayEffect is applied to the View that owns the effect.
 * If the owner has a state effect target configured with
 * View::SetStateEffectTarget(), the overlay is applied to that View instead.
 * This is useful when the interactive area and the View that should show
 * feedback are different.
 *
 * If an active target is removed from the owner while feedback is visible, the
 * active overlay is not immediately retargeted. It is cleaned up when the owner
 * state changes or when the effect or owner is cleaned up.
 *
 * OverlayEffect provides focus-indicated-state feedback itself, so it suppresses
 * the FocusManager default focus indicator while attached.
 *
 * Pressed recoil captures the target scale at press time and restores that
 * captured scale when recoil is stopped or released. Application scale changes
 * made directly to the same target while recoil is active are not preserved.
 */
class DALI_UI_API OverlayEffect : public StateEffect
{
public:
  class Builder;

  /**
   * @brief Shared plain overlay effect preset.
   *
   * Uses UiColor(0x000000, 0.1f), follows the target View corner radius, and
   * applies recoil feedback to the overlay target itself.
   *
   * @return A shared plain overlay effect preset
   */
  static const OverlayEffect& Plain();

  /**
   * @brief Shared round overlay effect preset.
   *
   * Uses UiColor(0x000000, 0.1f) and relative 0.5 corner radius.
   *
   * @return A shared round overlay effect preset
   */
  static const OverlayEffect& Round();

  /**
   * @brief Shared list item overlay effect preset.
   *
   * Uses UiColor(0x000000, 0.1f), follows the target View corner radius, and
   * applies recoil feedback to the target View's direct children. Child recoil
   * is skipped when the resolved overlay target has no direct children or more
   * than three direct children.
   *
   * @return A shared list item overlay effect preset
   */
  static const OverlayEffect& ListItem();

  /**
   * @brief Creates an uninitialized OverlayEffect handle.
   */
  OverlayEffect() = default;

  /**
   * @brief Downcasts a handle to OverlayEffect.
   *
   * @param[in] handle The handle to downcast
   * @return An initialized OverlayEffect on success, otherwise empty
   */
  static OverlayEffect DownCast(BaseHandle handle);

  /**
   * @brief Gets the overlay visual color.
   *
   * @return The overlay color
   */
  UiColor GetOverlayColor() const;

  /**
   * @brief Gets the configured corner radius.
   *
   * If this effect follows the target corner radius, this returns the stored
   * fallback radius value. Use IsUsingTargetCornerRadius() to check whether
   * the target View corner radius is used at runtime.
   *
   * @return The configured corner radius
   */
  Vector4 GetCornerRadius() const;

  /**
   * @brief Gets the configured corner radius policy.
   *
   * If this effect follows the target corner radius, this returns the stored
   * fallback policy value. Use IsUsingTargetCornerRadius() to check whether
   * the target View corner radius policy is used at runtime.
   *
   * @return The configured corner radius policy
   */
  CornerRadiusPolicy GetCornerRadiusPolicy() const;

  /**
   * @brief Creates a mutable builder copied from this effect.
   *
   * @return A builder object
   */
  Builder Configure() const;

  /**
   * @brief Gets whether this effect follows the target View corner radius and policy.
   *
   * @return True if the target View corner radius and policy are used at runtime
   */
  bool IsUsingTargetCornerRadius() const;

  /**
   * @brief Gets the recoil feedback scope.
   *
   * @return The recoil feedback scope
   */
  RecoilScope GetRecoilScope() const;

private:
  explicit OverlayEffect(Internal::OverlayEffectImpl* impl);

  friend class Builder;
};

/**
 * @brief Mutable builder used to create OverlayEffect handles.
 */
class DALI_UI_API OverlayEffect::Builder
{
public:
  /**
   * @brief Creates a builder with default overlay effect values.
   */
  Builder();

  Builder(Builder&& rhs) noexcept;
  Builder& operator=(Builder&& rhs) noexcept;

  Builder(const Builder&)            = delete;
  Builder& operator=(const Builder&) = delete;

  ~Builder();

  /**
   * @brief Sets the overlay visual color.
   *
   * @param[in] color The overlay color
   * @return Reference to this builder
   */
  Builder& SetOverlayColor(const UiColor& color) &;

  /**
   * @brief Sets the overlay visual color on a temporary builder.
   *
   * @param[in] color The overlay color
   * @return Rvalue reference to this builder
   */
  Builder&& SetOverlayColor(const UiColor& color) &&;

  /**
   * @brief Sets a uniform explicit corner radius.
   *
   * @param[in] radius The radius for all corners
   * @return Reference to this builder
   */
  Builder& SetCornerRadius(float radius) &;

  /**
   * @brief Sets a uniform explicit corner radius on a temporary builder.
   *
   * @param[in] radius The radius for all corners
   * @return Rvalue reference to this builder
   */
  Builder&& SetCornerRadius(float radius) &&;

  /**
   * @brief Sets explicit corner radii.
   *
   * @param[in] topLeft The top-left radius
   * @param[in] topRight The top-right radius
   * @param[in] bottomRight The bottom-right radius
   * @param[in] bottomLeft The bottom-left radius
   * @return Reference to this builder
   */
  Builder& SetCornerRadius(float topLeft, float topRight, float bottomRight, float bottomLeft) &;

  /**
   * @brief Sets explicit corner radii on a temporary builder.
   *
   * @param[in] topLeft The top-left radius
   * @param[in] topRight The top-right radius
   * @param[in] bottomRight The bottom-right radius
   * @param[in] bottomLeft The bottom-left radius
   * @return Rvalue reference to this builder
   */
  Builder&& SetCornerRadius(float topLeft, float topRight, float bottomRight, float bottomLeft) &&;

  /**
   * @brief Sets explicit corner radii.
   *
   * @param[in] radius The corner radii
   * @return Reference to this builder
   */
  Builder& SetCornerRadius(const Vector4& radius) &;

  /**
   * @brief Sets explicit corner radii on a temporary builder.
   *
   * @param[in] radius The corner radii
   * @return Rvalue reference to this builder
   */
  Builder&& SetCornerRadius(const Vector4& radius) &&;

  /**
   * @brief Sets an explicit corner radius policy.
   *
   * @param[in] policy The corner radius policy
   * @return Reference to this builder
   */
  Builder& SetCornerRadiusPolicy(CornerRadiusPolicy policy) &;

  /**
   * @brief Sets an explicit corner radius policy on a temporary builder.
   *
   * @param[in] policy The corner radius policy
   * @return Rvalue reference to this builder
   */
  Builder&& SetCornerRadiusPolicy(CornerRadiusPolicy policy) &&;

  /**
   * @brief Sets the corner radius policy to relative.
   *
   * @return Reference to this builder
   */
  Builder& SetCornerRadiusPolicyRelative() &;

  /**
   * @brief Sets the corner radius policy to relative on a temporary builder.
   *
   * @return Rvalue reference to this builder
   */
  Builder&& SetCornerRadiusPolicyRelative() &&;

  /**
   * @brief Creates an OverlayEffect from this builder.
   *
   * This consumes the builder and transfers its implementation without cloning.
   * Call this on a temporary builder, or use std::move(builder).Build().
   * Do not use this builder after calling Build().
   *
   * @return A new OverlayEffect
   */
  OverlayEffect Build() &&;

  /**
   * @brief Sets whether the overlay follows the target View corner radius and policy.
   *
   * When enabled, the stored corner radius and policy remain as configured
   * values, but the runtime overlay uses the resolved target View's corner
   * radius and policy. SetCornerRadius() and SetCornerRadiusPolicy() disable
   * this mode because they configure explicit overlay corner values.
   *
   * @param[in] useTargetCornerRadius True to follow the target View corner radius and policy
   * @return Reference to this builder
   */
  Builder& SetUseTargetCornerRadius(bool useTargetCornerRadius) &;

  /**
   * @brief Sets whether the overlay follows the target View corner radius and policy on a temporary builder.
   *
   * When enabled, the stored corner radius and policy remain as configured
   * values, but the runtime overlay uses the resolved target View's corner
   * radius and policy. SetCornerRadius() and SetCornerRadiusPolicy() disable
   * this mode because they configure explicit overlay corner values.
   *
   * @param[in] useTargetCornerRadius True to follow the target View corner radius and policy
   * @return Rvalue reference to this builder
   */
  Builder&& SetUseTargetCornerRadius(bool useTargetCornerRadius) &&;

  /**
   * @brief Sets which View receives pressed-state recoil feedback.
   *
   * RecoilScope::OVERLAY_TARGET_CHILDREN applies only when the resolved overlay
   * target has one to three direct children. If it has no direct children or
   * more than three direct children, recoil feedback is skipped.
   *
   * @param[in] scope The recoil feedback scope
   * @return Reference to this builder
   */
  Builder& SetRecoilScope(RecoilScope scope) &;

  /**
   * @brief Sets which View receives pressed-state recoil feedback on a temporary builder.
   *
   * RecoilScope::OVERLAY_TARGET_CHILDREN applies only when the resolved overlay
   * target has one to three direct children. If it has no direct children or
   * more than three direct children, recoil feedback is skipped.
   *
   * @param[in] scope The recoil feedback scope
   * @return Rvalue reference to this builder
   */
  Builder&& SetRecoilScope(RecoilScope scope) &&;

private:
  explicit Builder(Internal::OverlayEffectImpl* impl);

  friend class OverlayEffect;

private:
  IntrusivePtr<Internal::OverlayEffectImpl> mImpl;
};

} // namespace Ui
} // namespace Dali
