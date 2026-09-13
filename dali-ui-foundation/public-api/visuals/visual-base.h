#ifndef DALI_UI_VISUAL_BASE_H
#define DALI_UI_VISUAL_BASE_H

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
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/object/base-handle.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/view-types.h>
#include <dali-ui-foundation/public-api/visuals/visual-types.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal DALI_INTERNAL
{
class VisualBaseImpl;
} //namespace Internal DALI_INTERNAL

/**
 * @addtogroup dali_ui_visuals
 * @{
 */

class View;

/**
 * @brief The base class of every visual, which can be attached to a Dali::Ui::View.
 *
 * VisualBase manages attachment/detachment, sibling order, and property updates.
 * A VisualBase can belong to only one depth layer of one View; adding it to another
 * View, or to another depth layer, automatically removes it from the previous one.
 *
 * @code
 * Dali::Ui::View view = Dali::Ui::View::New();
 * Dali::Ui::ColorVisual visual = Dali::Ui::ColorVisual::New();
 * visual.SetColor(UiColor("Primary"));
 * visual.SetOffsetX(0.5f);
 * visual.SetWidth(0.5f);
 * view.AddVisual(visual, Dali::Ui::Visual::DepthLayer::BACKGROUND);
 *
 * // view.GetVisualCount(Dali::Ui::Visual::DepthLayer::BACKGROUND) == 1u.
 * // visual.GetOwner() == view.
 *
 * anotherView.AddVisual(visual, Dali::Ui::Visual::DepthLayer::CONTENT)
 *
 * // view.GetVisualCount(Dali::Ui::Visual::DepthLayer::BACKGROUND) == 0u.
 * // visual.GetOwner() == anotherView.
 * @endcode
 *
 * VisualBase could change sibling order. It will change the rendering order at view.
 * Sibling order only reorders visuals sharing the same DepthLayer. A VisualBase can
 * never be drawn outside of the layer it is attached to.
 *
 * For example, a visual attached to DepthLayer::BACKGROUND can not be rendered
 * under the View's background, nor over its content.
 *
 * @code
 * view.AddVisual(visual1, Dali::Ui::Visual::DepthLayer::BACKGROUND);
 * view.AddVisual(visual2, Dali::Ui::Visual::DepthLayer::BACKGROUND);
 *
 * // visual1.GetSiblingOrder() == 0u, visual2.GetSiblingOrder() == 1u.
 * // view.GetVisualAt(Dali::Ui::Visual::DepthLayer::BACKGROUND, 0u) == visual1;
 * // view.GetVisualAt(Dali::Ui::Visual::DepthLayer::BACKGROUND, 1u) == visual2;
 * // Rendering order = view's Background -> visual1 -> visual2 -> Content (e.g. Text for Label, Image for ImageView)
 *
 * visual1.RaiseToTop();
 *
 * // visual1.GetSiblingOrder() == 1u, visual2.GetSiblingOrder() == 0u.
 * // view.GetVisualAt(Dali::Ui::Visual::DepthLayer::BACKGROUND, 0u) == visual2;
 * // view.GetVisualAt(Dali::Ui::Visual::DepthLayer::BACKGROUND, 1u) == visual1;
 * // Rendering order = view's Background -> visual2 -> visual1 -> Content
 *
 * visual2.Detach();
 *
 * // visual1.GetSiblingOrder() == 0u.
 * // view.GetVisualAt(Dali::Ui::Visual::DepthLayer::BACKGROUND, 0u) == visual1;
 * // Rendering order = view's Background -> visual1 -> Content
 *
 * view.AddVisual(visual2, Dali::Ui::Visual::DepthLayer::BACKGROUND_EFFECT);
 *
 * // visual1.GetSiblingOrder() == 0u.
 * // visual2.GetSiblingOrder() == 0u.
 * // view.GetVisualAt(Dali::Ui::Visual::DepthLayer::BACKGROUND_EFFECT, 0u) == visual2;
 * // view.GetVisualAt(Dali::Ui::Visual::DepthLayer::BACKGROUND, 0u) == visual1;
 * // Rendering order = visual2 -> view's Background -> visual1 -> Content
 * @endcode
 *
 * VisualBase origin and pivot define the reference point of the visual relative to its attached view.
 * The origin selects a point on the attached view; the pivot selects the point on the visual that
 * is placed there.
 *
 * Offset properties (X/Y) are expressed as a proportion of the pivot from the origin.
 * Width/Height define the size of the visual.
 * By default, Offset and Size parameters are proportions of the attached view size.
 * You can set Dali::Ui::Visual::Transform::ProportionFlags to make each value absolute or relative.
 *
 * ExtraWidth/ExtraHeight add an absolute size to the visual, and may be negative.
 * The extra size expands from the pivot.
 *
 * VisualBase itself has no constructor. Construct one through a subclass,
 * e.g. ColorVisual::New().
 *
 * @note VisualBase properties are applied before the LayoutController runs. A visual created or
 * changed during layout (e.g. ColorVisual::New() inside OnMeasure()) therefore has its properties
 * applied too late for that pass, which flickers.
 * DO NOT create or change VisualBase properties during layout calculation.
 */
class DALI_UI_API VisualBase : public Dali::BaseHandle
{
public:
  /**
   * @brief Downcasts a handle to VisualBase handle.
   *
   * If handle points to a VisualBase, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a VisualBase or an uninitialized handle
   */
  static VisualBase DownCast(BaseHandle handle);

public: // Attachment and type
  /**
   * @brief Get attached view. Empty handle if this visual is not be attached.
   *
   * @return The attached view, or empty handle if not be attached.
   */
  Dali::Ui::View GetOwner() const;

  /**
   * @brief Get the attached depth layer. NONE if this visual is not attached.
   *
   * @return The attached depth layer, or NONE if not attached.
   */
  Dali::Ui::Visual::DepthLayer GetDepthLayer() const;

  /**
   * @brief Detach from the attached view.
   */
  void Detach();

  /**
   * @brief Get the type of this VisualBase.
   *
   * @return The type of this VisualBase
   */
  Dali::Ui::VisualType GetVisualType() const;

public: // Accessors
  /**
   * @brief Gets the name of the VisualBase.
   *
   * @return The name of the VisualBase
   */
  const Dali::String& GetName() const;

  /**
   * @brief Sets the name of this VisualBase.
   *
   * @param[in] name The name to set
   */
  void SetName(const Dali::String& name);

  /**
   * @brief Gets the color of this VisualBase.
   *
   * @return The color of this VisualBase
   */
  UiColor GetColor() const;

  /**
   * @brief Sets the color of this VisualBase.
   *
   * For a VisualBase that draws no content of its own, such as ColorVisual or BorderVisual,
   * this is the rendered color. For a VisualBase that draws content, such as ImageVisual or
   * TextVisual, the red, green, blue and alpha components are multiplied with the
   * corresponding components of that content, tinting it.
   *
   * @param[in] color The UiColor to apply
   * @note The default color is white, which leaves drawn content unchanged.
   */
  void SetColor(const UiColor& color);

  // Transform (Offset / Size / Proportion flags / Extra size / Origin / Pivot)
  /**
   * @brief Gets the X offset of the VisualBase.
   *
   * @return The X offset of the VisualBase
   */
  float GetOffsetX() const;

  /**
   * @brief Sets the X offset of the VisualBase.
   *
   * @param[in] x The X offset to set
   */
  void SetOffsetX(float x);

  /**
   * @brief Gets the Y offset of the VisualBase.
   *
   * @return The Y offset of the VisualBase
   */
  float GetOffsetY() const;

  /**
   * @brief Sets the Y offset of the VisualBase.
   *
   * @param[in] y The Y offset to set
   */
  void SetOffsetY(float y);

  /**
   * @brief Gets the width of the VisualBase.
   *
   * @return The width of the VisualBase
   */
  float GetWidth() const;

  /**
   * @brief Sets the width of the VisualBase.
   *
   * @param[in] width The width to set
   */
  void SetWidth(float width);

  /**
   * @brief Gets the height of the VisualBase.
   *
   * @return The height of the VisualBase
   */
  float GetHeight() const;

  /**
   * @brief Sets the height of the VisualBase.
   *
   * @param[in] height The height to set
   */
  void SetHeight(float height);

  /**
   * @brief Gets the transform proportion flags of the VisualBase.
   *
   * @return The transform proportion flags of the VisualBase
   */
  Dali::Ui::Visual::Transform::ProportionFlags GetTransformProportionFlags() const;

  /**
   * @brief Sets the transform proportion flags of the VisualBase.
   *
   * A flag selects one of the offset and size values. A value whose flag is set is a fraction of
   * the attached View's size, where 1.0f is the full size; a value whose flag is left unset is an
   * absolute length in pixels.
   *
   * @note ExtraWidth and ExtraHeight are always absolute and are unaffected by these flags.
   *
   * @param[in] flags The transform proportion flags to set
   */
  void SetTransformProportionFlags(Dali::Ui::Visual::Transform::ProportionFlags flags);

  /**
   * @brief Gets the extra width of the VisualBase.
   *
   * @return The extra width of the VisualBase
   */
  float GetExtraWidth() const;

  /**
   * @brief Sets the extra width of the VisualBase.
   * This width is added to the final width absolutely, independent of the transform proportion flags.
   *
   * @param[in] extraWidth The extra width to set
   */
  void SetExtraWidth(float extraWidth);

  /**
   * @brief Gets the extra height of the VisualBase.
   *
   * @return The extra height of the VisualBase
   */
  float GetExtraHeight() const;

  /**
   * @brief Sets the extra height of the VisualBase.
   * This height is added to the final height absolutely, independent of the transform proportion flags.
   *
   * @param[in] extraHeight The extra height to set
   */
  void SetExtraHeight(float extraHeight);

  /**
   * @brief Gets the origin of the VisualBase.
   *
   * @return The origin of the VisualBase
   */
  VisualOrigin GetOrigin() const;

  /**
   * @brief Sets the origin of the VisualBase.
   *
   * The origin is the point on the attached View that this visual is positioned from.
   *
   * @note The origin is absolute and is never mirrored: a LEFT origin stays on the left whatever
   * the effective layout direction.
   *
   * @param[in] origin The origin to set
   */
  void SetOrigin(VisualOrigin origin);

  /**
   * @brief Gets the pivot of the VisualBase.
   *
   * @return The pivot of the VisualBase
   */
  VisualPivot GetPivot() const;

  /**
   * @brief Sets the pivot of the VisualBase.
   *
   * The pivot is the point on the visual itself that is placed at the origin.
   *
   * @note Like the origin, the pivot is absolute and is never mirrored.
   * @note This takes a Ui::VisualPivot, unlike Actor::SetPivot() which takes a Vector3.
   *
   * @param[in] pivot The pivot to set
   */
  void SetPivot(VisualPivot pivot);

  // Decoration (CornerRadius / CornerSquareness / Borderline)
  /**
   * @brief Gets the corner radius of the VisualBase.
   *
   * @return The corner radius as Vector4 (topLeft, topRight, bottomRight, bottomLeft)
   */
  Vector4 GetCornerRadius() const;

  /**
   * @brief Sets a uniform corner radius for all four corners.
   * @note Supported only by ColorVisual, GradientVisual, ImageVisual, AnimatedImageVisual and
   * LottieAnimationVisual. It has no effect on other visuals, including an ImageVisual that is
   * showing an n-patch image.
   *
   * @param[in] radius The corner radius to apply to all corners
   */
  void SetCornerRadius(float radius);

  /**
   * @brief Sets individual corner radii for all four corners.
   * @note Supported only by ColorVisual, GradientVisual, ImageVisual, AnimatedImageVisual and
   * LottieAnimationVisual. It has no effect on other visuals, including an ImageVisual that is
   * showing an n-patch image.
   *
   * @param[in] topLeft     The radius for the top-left corner
   * @param[in] topRight    The radius for the top-right corner
   * @param[in] bottomRight The radius for the bottom-right corner
   * @param[in] bottomLeft  The radius for the bottom-left corner
   */
  void SetCornerRadius(float topLeft, float topRight, float bottomRight, float bottomLeft);

  /**
   * @brief Sets corner radii from a Vector4.
   * @note Supported only by ColorVisual, GradientVisual, ImageVisual, AnimatedImageVisual and
   * LottieAnimationVisual. It has no effect on other visuals, including an ImageVisual that is
   * showing an n-patch image.
   *
   * @param[in] radius Corner radii as Vector4 (x=topLeft, y=topRight, z=bottomRight, w=bottomLeft)
   */
  void SetCornerRadius(const Vector4& radius);

  /**
   * @brief Gets the corner radius policy.
   *
   * @return The corner radius policy
   */
  CornerRadiusPolicy GetCornerRadiusPolicy() const;

  /**
   * @brief Sets the corner radius policy.
   *
   * @note The default is CornerRadiusPolicy::ABSOLUTE.
   * @note Supported only by ColorVisual, GradientVisual, ImageVisual, AnimatedImageVisual and
   * LottieAnimationVisual. It has no effect on other visuals, including an ImageVisual that is
   * showing an n-patch image.
   *
   * @param[in] policy ABSOLUTE for world-unit values, RELATIVE for a percentage [0.0, 0.5] of the shorter side
   */
  void SetCornerRadiusPolicy(CornerRadiusPolicy policy);

  /**
   * @brief Shortcut to set the corner radius policy to RELATIVE.
   * @note Supported only by ColorVisual, GradientVisual, ImageVisual, AnimatedImageVisual and
   * LottieAnimationVisual. It has no effect on other visuals, including an ImageVisual that is
   * showing an n-patch image.
   *
   * Equivalent to SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE).
   */
  void SetCornerRadiusPolicyRelative();

  /**
   * @brief Returns true if the corner radius policy is RELATIVE.
   *
   * @return True if the policy is RELATIVE, false if ABSOLUTE
   */
  bool IsCornerRadiusPolicyRelative() const;

  /**
   * @brief Gets the corner squareness of the VisualBase.
   *
   * @return The corner squareness as Vector4 (x=topLeft, y=topRight, z=bottomRight, w=bottomLeft)
   */
  Vector4 GetCornerSquareness() const;

  /**
   * @brief Sets a uniform corner squareness for all four corners.
   * @note Supported only by ColorVisual, GradientVisual, ImageVisual, AnimatedImageVisual and
   * LottieAnimationVisual. It has no effect on other visuals, including an ImageVisual that is
   * showing an n-patch image.
   *
   * @param[in] squareness The squareness value to apply to all corners
   */
  void SetCornerSquareness(float squareness);

  /**
   * @brief Sets individual corner squareness values for all four corners.
   * @note Supported only by ColorVisual, GradientVisual, ImageVisual, AnimatedImageVisual and
   * LottieAnimationVisual. It has no effect on other visuals, including an ImageVisual that is
   * showing an n-patch image.
   *
   * @param[in] topLeft     The squareness for the top-left corner
   * @param[in] topRight    The squareness for the top-right corner
   * @param[in] bottomRight The squareness for the bottom-right corner
   * @param[in] bottomLeft  The squareness for the bottom-left corner
   */
  void SetCornerSquareness(float topLeft, float topRight, float bottomRight, float bottomLeft);

  /**
   * @brief Sets corner squareness from a Vector4.
   * @note Supported only by ColorVisual, GradientVisual, ImageVisual, AnimatedImageVisual and
   * LottieAnimationVisual. It has no effect on other visuals, including an ImageVisual that is
   * showing an n-patch image.
   *
   * @param[in] squareness Squareness values as Vector4 (x=topLeft, y=topRight, z=bottomRight, w=bottomLeft)
   */
  void SetCornerSquareness(const Vector4& squareness);

  /**
   * @brief Gets the borderline width of the VisualBase.
   *
   * @return The borderline width
   */
  float GetBorderlineWidth() const;

  /**
   * @brief Sets the borderline width of the VisualBase.
   * @note Supported only by ColorVisual, GradientVisual, ImageVisual, AnimatedImageVisual and
   * LottieAnimationVisual. It has no effect on other visuals, including an ImageVisual that is
   * showing an n-patch image.
   *
   * @param[in] width The borderline width to set
   */
  void SetBorderlineWidth(float width);

  /**
   * @brief Gets the borderline color of the VisualBase.
   *
   * @return The borderline color
   */
  UiColor GetBorderlineColor() const;

  /**
   * @brief Sets the borderline color of the VisualBase.
   * @note Supported only by ColorVisual, GradientVisual, ImageVisual, AnimatedImageVisual and
   * LottieAnimationVisual. It has no effect on other visuals, including an ImageVisual that is
   * showing an n-patch image.
   *
   * If the UiColor has a color ID, it is resolved from the current
   * theme and a binding is registered so the color is automatically
   * refreshed when the theme changes.
   *
   * @param[in] color The UiColor to apply
   */
  void SetBorderlineColor(const UiColor& color);

  /**
   * @brief Gets the borderline offset of the VisualBase.
   *
   * @return The borderline offset
   */
  float GetBorderlineOffset() const;

  /**
   * @brief Sets the borderline offset of the VisualBase.
   * @note Supported only by ColorVisual, GradientVisual, ImageVisual, AnimatedImageVisual and
   * LottieAnimationVisual. It has no effect on other visuals, including an ImageVisual that is
   * showing an n-patch image.
   *
   * @param[in] offset The borderline offset to set
   */
  void SetBorderlineOffset(float offset);

public: // Sibling order
  /**
   * @brief Get the sibling order of the visual object inside of the container.
   *
   * @return The sibling order inside of the container. Or 0 if visual object is not in container.
   */
  uint32_t GetSiblingOrder() const;

  /**
   * @brief Set the sibling order of the visual object inside of the container.
   * @note It will change other VisualBase's sibling order to keep the order.
   * @note It will throw assert if siblingOrder is bigger than the number of visuals
   * that the container has.
   *
   * @param[in] siblingOrder The sibling order inside of the container.
   */
  void SetSiblingOrder(uint32_t siblingOrder);

  /**
   * @brief Raise the visual object above the next sibling visual object.
   *
   * @pre The VisualBase has been initialized.
   * @pre The VisualBase has been attached to a View.
   */
  void Raise();

  /**
   * @brief Lower the visual object below the previous sibling visual object.
   *
   * @pre The VisualBase has been initialized.
   * @pre The VisualBase has been attached to a View.
   */
  void Lower();

  /**
   * @brief Raise visual object above all other sibling visual objects.
   *
   * @pre The VisualBase has been initialized.
   * @pre The VisualBase has been attached to a View.
   */
  void RaiseToTop();

  /**
   * @brief Lower visual object to the bottom of all other sibling visual objects.
   *
   * @pre The VisualBase has been initialized.
   * @pre The VisualBase has been attached to a View.
   */
  void LowerToBottom();

  /**
   * @brief Raises the visual object above the target visual object.
   *
   * @param[in] target The target visual object
   * @pre The VisualBase has been initialized.
   * @pre The VisualBase has been attached to a View.
   * @pre The target visual object is a sibling.
   */
  void RaiseAbove(VisualBase target);

  /**
   * @brief Lower the visual object to below the target visual object.
   *
   * @param[in] target The target visual object
   * @pre The VisualBase has been initialized.
   * @pre The VisualBase has been attached to a View.
   * @pre The target visual object is a sibling.
   */
  void LowerBelow(VisualBase target);

public:
  VisualBase()                                     = default;
  ~VisualBase()                                    = default;
  VisualBase(const VisualBase& rhs)                = default;
  VisualBase& operator=(const VisualBase& rhs)     = default;
  VisualBase(VisualBase&& rhs) noexcept            = default;
  VisualBase& operator=(VisualBase&& rhs) noexcept = default;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param[in] object A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL VisualBase(Dali::Ui::Internal::VisualBaseImpl* object);
  /// @endcond
};

/**
 * @}
 */
} // namespace Ui
} // namespace DALI_NAMESPACE

#endif // DALI_UI_VISUAL_BASE_H
