#ifndef DALI_UI_VISUAL_BASE_H
#define DALI_UI_VISUAL_BASE_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali-ui-foundation/public-api/align-enumerations.h>
#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali-ui-foundation/public-api/view-types.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

namespace Dali
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

#include "visual-base.autogen.h"
/**
 * @brief VisualBase is a owner of Visual::Base.
 *
 * It represents the base visual object that can be attached to a Dali::Ui::View.
 * VisualBase manages attachment/detachment, sibling order, and property updates.
 * A VisualBase can belong to only one View's ContainerRangeType; adding it to
 * another View or ContainerRangeType will automatically remove it from the
 * previous one.
 *
 * @code
 * Dali::Ui::View view = Dali::Ui::View::New();
 * Dali::Ui::ColorVisual visual = Dali::Ui::ColorVisual::New()
 *                                  .SetColor(UiColor("Primary"))
 *                                  .SetOffsetX(0.5f)
 *                                  .SetWidth(0.5f);
 * view.AddVisual(visual, Dali::Ui::Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);
 *
 * // view.GetVisualCount(Dali::Ui::Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT) == 1u.
 * // visual.GetOwner() == view.
 *
 * anotherView.AddVisual(visual, Dali::Ui::Visual::ContainerRangeType::BETWEEN_CONTENT_AND_DECORATION)
 *
 * // view.GetVisualCount(Dali::Ui::Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT) == 0u.
 * // visual.GetOwner() == anotherView.
 * @endcode
 *
 * VisualBase could change sibling order. It will change the rendering order at view.
 * Sibling order only works at same type of ContainerRangeType. VisualBase could not
 * over the container.
 *
 * For example, if ContainerRangeType added at BETWEEN_BACKGROUND_AND_CONTENT,
 * this visual could not be rendered under the background, or over the content.
 *
 * @code
 * view.AddVisual(visual1, Dali::Ui::Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);
 * view.AddVisual(visual2, Dali::Ui::Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);
 *
 * // visual1.GetSiblingOrder() == 0u, visual2.GetSiblingOrder() == 1u.
 * // view.GetVisualAt(Dali::Ui::Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT, 0u) == visual1;
 * // view.GetVisualAt(Dali::Ui::Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT, 1u) == visual2;
 * // Rendering order = view's Background -> visual1 -> visual2 -> Content (e.g. Text for Label, Image for ImageView)
 *
 * visual1.RaiseToTop();
 *
 * // visual1.GetSiblingOrder() == 1u, visual2.GetSiblingOrder() == 0u.
 * // view.GetVisualAt(Dali::Ui::Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT, 0u) == visual2;
 * // view.GetVisualAt(Dali::Ui::Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT, 1u) == visual1;
 * // Rendering order = view's Background -> visual2 -> visual1 -> Content
 *
 * visual2.Detach();
 *
 * // visual1.GetSiblingOrder() == 0u.
 * // view.GetVisualAt(Dali::Ui::Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT, 0u) == visual1;
 * // Rendering order = view's Background -> visual1 -> Content
 *
 * view.AddVisual(visual2, Dali::Ui::Visual::ContainerRangeType::BETWEEN_BACKGROUND_EFFECT_AND_BACKGROUND);
 *
 * // visual1.GetSiblingOrder() == 0u.
 * // visual2.GetSiblingOrder() == 0u.
 * // view.GetVisualAt(Dali::Ui::Visual::ContainerRangeType::BETWEEN_BACKGROUND_EFFECT_AND_BACKGROUND, 0u) == visual2;
 * // view.GetVisualAt(Dali::Ui::Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT, 0u) == visual1;
 * // Rendering order = visual2 -> view's Background -> visual1 -> Content
 * @endcode
 *
 * VisualBase origin and pivot define the reference point of the visual relative to its attached view.
 * Origin define where should the Pivot positions from attached view.
 * (Similar with ParentOrigin and Pivot property of View class.)
 *
 * Offset properties (X/Y) are expressed as a proportion of the pivot from the origin.
 * Width/Height
 * By default, Offset and Size parameters are proportions by attached view size.
 * You can set Dali::Ui::Visual::Transform::ProportionFlags to change each varaibles be absolute or relative.
 *
 * ExtraWidth/ExtraHeight is additional absolute size of the visual. (Could be negative value)
 * The extra size expand from the Pivot.
 *
 * VisualBase itself don't have constructor. We should construct class by subclass of
 * VisualBase, e.g. ColorVisual::New().
 *
 * @note VisualBase properties applied before LayoutController process.
 * So if you try to create, or change properties during Layout, (e.g. ColorVisual::New() at OnMeasure())
 * that VisualBase properties didn't applied at this phase, so flickering will be occured.
 * DO NOT create or change VisualBase properties during Layout calculation.
 */
class DALI_UI_API VisualBase : public Dali::BaseHandle
{
public: ///< Public API
  /**
   * @brief Get attached view. Empty handle if this visual is not be attached.
   *
   * @return The attached view, or empty handle if not be attached.
   */
  Dali::Ui::View GetOwner() const;

  /**
   * @brief Get the attached container range type. INVALID if this visual is not be attached.
   *
   * @return The attached container range type, or INVALID if not be attached.
   */
  Dali::Ui::Visual::ContainerRangeType GetContainerRangeType() const;

  /**
   * @brief Detach from the attached view.
   *
   */
  void Detach();

  /**
   * @brief Perform an action on a visual registered to this view.
   * Visuals will have actions, this API is used to perform one of these actions with the given attributes.
   * @note If visual is not been registered to the view, action should be ignored.
   *
   * @param[in] actionId The action to perform.  See Visual to find supported actions.
   * @param[in] attributes Optional attributes for the action.
   */
  void DoAction(Dali::Property::Index actionId, const Dali::Property::Value& attributes);

  /**
   * @brief Get the type of this VisualBase.
   *
   * @return The type of this VisualBase
   */
  Dali::Ui::Visual::Type GetVisualType() const;

public: // GetProperty / SetProperty
  /**
   * @brief Retrieves a property value.
   * @note BaseHandle is not subclass of Handle. So this API is not use Handle.SetProperty
   *
   * @param[in] index The index of the property
   * @return The property value
   * @note This returns the value set by SetProperty() or the animation target value if it is being animated.
   */
  Dali::Property::Value GetProperty(Dali::Property::Index index) const;

  /**
   * @brief Convenience function for obtaining a property of a known type.
   *
   * @param[in] index The index of the property
   * @return The property value
   * @pre The property types match i.e. PropertyTypes::Get<T>() is equal to GetPropertyType(index).
   * @see GetProperty()
   */
  template<typename T>
  T GetProperty(Dali::Property::Index index) const
  {
    Dali::Property::Value value = GetProperty(index);

    return T(value.Get<T>());
  }

  /**
   * @brief Sets the value of an existing property.
   * @note BaseHandle is not subclass of Handle. So this API is not use Handle.SetProperty
   *
   * @param[in] index The index of the property
   * @param[in] propertyValue The new value of the property
   */
  void SetProperty(Dali::Property::Index index, Dali::Property::Value propertyValue);

public: // Setters for chaining
  // @CHAIN_START(VisualBase)
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
  VisualBase& SetName(const Dali::String& name);

  /**
   * @brief Gets the color of the VisualBase.
   *
   * @return The color of the VisualBase
   */
  UiColor GetColor() const;

  /**
   * @brief Sets the color of this VisualBase.
   *
   * @param[in] color The UiColor to apply
   */
  VisualBase& SetColor(const UiColor& color);

  /**
   * @brief Gets the X offset of the VisualBase.
   *
   * @return The X offset of the VisualBase
   */
  float GetOffsetX() const;

  // Transform relative properties
  /**
   * @brief Sets the X offset of the VisualBase.
   *
   * @param[in] x The X offset to set
   */
  VisualBase& SetOffsetX(float x);

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
  VisualBase& SetOffsetY(float y);

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
  VisualBase& SetWidth(float width);

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
  VisualBase& SetHeight(float height);

  /**
   * @brief Gets the proportion flags of the VisualBase.
   *
   * @return The proportion flags of the VisualBase
   */
  Dali::Ui::Visual::Transform::ProportionFlags GetProportionFlags() const;

  /**
   * @brief Sets the proportion flags of the VisualBase.
   *
   * @param[in] flags The proportion flags to set
   */
  VisualBase& SetProportionFlags(Dali::Ui::Visual::Transform::ProportionFlags flags);

  /**
   * @brief Gets the extra width of the VisualBase.
   *
   * @return The extra width of the VisualBase
   */
  float GetExtraWidth() const;

  /**
   * @brief Sets the extra width of the VisualBase.
   * This width will be added to final width absolutly, independent as proportion flags.
   *
   * @param[in] extraWidth The extra width to set
   */
  VisualBase& SetExtraWidth(float extraWidth);

  /**
   * @brief Gets the extra height of the VisualBase.
   *
   * @return The extra height of the VisualBase
   */
  float GetExtraHeight() const;

  /**
   * @brief Sets the extra height of the VisualBase.
   * This height will be added to final height absolutly, independent as proportion flags.
   *
   * @param[in] extraHeight The extra height to set
   */
  VisualBase& SetExtraHeight(float extraHeight);

  /**
   * @brief Gets the origin of the VisualBase.
   *
   * @return The origin of the VisualBase
   */
  Align::Type GetOrigin() const;

  /**
   * @brief Sets the origin of the VisualBase.
   *
   * @param[in] origin The origin type to set
   */
  VisualBase& SetOrigin(Align::Type origin);

  /**
   * @brief Gets the pivot of the VisualBase.
   *
   * @return The pivot of the VisualBase
   */
  Align::Type GetPivot() const;

  /**
   * @brief Sets the pivot of the VisualBase.
   *
   * @param[in] pivot The pivot to set
   */
  VisualBase& SetPivot(Align::Type pivot);

  // Decorated properties (CornerRadius / Borderline)
  /**
   * @brief Gets the corner radius of the VisualBase.
   *
   * @return The corner radius as Vector4 (topLeft, topRight, bottomRight, bottomLeft)
   */
  Vector4 GetCornerRadius() const;

  /**
   * @brief Sets a uniform corner radius for all four corners.
   * @note Only supported type of visual has efforts.
   *
   * @param[in] radius The corner radius to apply to all corners
   */
  VisualBase& SetCornerRadius(float radius);

  /**
   * @brief Sets individual corner radii for all four corners.
   * @note Only supported type of visual has efforts.
   *
   * @param[in] topLeft     The radius for the top-left corner
   * @param[in] topRight    The radius for the top-right corner
   * @param[in] bottomRight The radius for the bottom-right corner
   * @param[in] bottomLeft  The radius for the bottom-left corner
   */
  VisualBase& SetCornerRadius(float topLeft, float topRight, float bottomRight, float bottomLeft);

  /**
   * @brief Sets corner radii from a Vector4.
   * @note Only supported type of visual has efforts.
   *
   * @param[in] radius Corner radii as Vector4 (x=topLeft, y=topRight, z=bottomRight, w=bottomLeft)
   */
  VisualBase& SetCornerRadius(const Vector4& radius);

  /**
   * @brief Gets the corner radius policy.
   *
   * @return The corner radius policy
   */
  CornerRadiusPolicy GetCornerRadiusPolicy() const;

  /**
   * @brief Sets the corner radius policy.
   * @note Only supported type of visual has efforts.
   *
   * @param[in] policy ABSOLUTE for world-unit values (default), RELATIVE for percentage [0.0, 0.5] of the shorter side
   */
  VisualBase& SetCornerRadiusPolicy(CornerRadiusPolicy policy);

  /**
   * @brief Shortcut to set the corner radius policy to RELATIVE.
   * @note Only supported type of visual has efforts.
   *
   * Equivalent to SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE).
   */
  VisualBase& SetCornerRadiusPolicyRelative();

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
   * @note Only supported type of visual has efforts.
   *
   * @param[in] squareness The squareness value to apply to all corners
   */
  VisualBase& SetCornerSquareness(float squareness);

  /**
   * @brief Sets individual corner squareness values for all four corners.
   * @note Only supported type of visual has efforts.
   *
   * @param[in] topLeft     The squareness for the top-left corner
   * @param[in] topRight    The squareness for the top-right corner
   * @param[in] bottomRight The squareness for the bottom-right corner
   * @param[in] bottomLeft  The squareness for the bottom-left corner
   */
  VisualBase& SetCornerSquareness(float topLeft, float topRight, float bottomRight, float bottomLeft);

  /**
   * @brief Sets corner squareness from a Vector4.
   * @note Only supported type of visual has efforts.
   *
   * @param[in] squareness Squareness values as Vector4 (x=topLeft, y=topRight, z=bottomRight, w=bottomLeft)
   */
  VisualBase& SetCornerSquareness(const Vector4& squareness);

  /**
   * @brief Gets the borderline width of the VisualBase.
   *
   * @return The borderline width
   */
  float GetBorderlineWidth() const;

  /**
   * @brief Sets the borderline width of the VisualBase.
   * @note Only supported type of visual has efforts.
   *
   * @param[in] width The borderline width to set
   */
  VisualBase& SetBorderlineWidth(float width);

  /**
   * @brief Gets the borderline color of the VisualBase.
   *
   * @return The borderline color
   */
  UiColor GetBorderlineColor() const;

  /**
   * @brief Sets the borderline color of the VisualBase.
   * @note Only supported type of visual has efforts.
   *
   * If the UiColor has a color ID, it is resolved from the current
   * theme and a binding is registered so the color is automatically
   * refreshed when the theme changes.
   *
   * @param[in] color The UiColor to apply
   */
  VisualBase& SetBorderlineColor(const UiColor& color);

  /**
   * @brief Gets the borderline offset of the VisualBase.
   *
   * @return The borderline offset
   */
  float GetBorderlineOffset() const;

  /**
   * @brief Sets the borderline offset of the VisualBase.
   * @note Only supported type of visual has efforts.
   *
   * @param[in] offset The borderline offset to set
   */
  VisualBase& SetBorderlineOffset(float offset);

  // @CHAIN_MANUAL
  /**
   * @brief Assigns this VisualBase instance to a target variable.
   * This method is useful for capturing a reference to a VisualBase created within
   * a declarative UI tree for later use.
   */
  VisualBase& As(VisualBase& self)
  {
    self = static_cast<VisualBase&>(*this);
    return *this;
  }

  // @CHAIN_MANUAL_SELF
  /**
   * @brief Executes a custom action on this View instance.
   *
   * Useful for performing additional setup (e.g. signal connections,
   * predefined style application) without breaking the method chain.
   *
   * @param[in] action A callable (lambda or free function) invoked with
   *                   a reference to this instance.
   *
   * @return Reference to this View for fluent chaining.
   */
  template<typename F>
  VisualBase& With(F&& action)
  {
    action(*this);
    return *this;
  }

  // @CHAIN_END

public: /// Sibling Order Change API
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
   * that contianer has.
   *
   * @param[in] siblingOrder The sibling order inside of the container.
   */
  void SetSiblingOrder(uint32_t siblingOrder);

  /**
   * @brief Raise the visual object above the next sibling visual object.
   *
   * @pre The VisualBase has been initialized.
   * @pre The VisualBase has been parented.
   */
  void Raise();

  /**
   * @brief Lower the visual object below the previous sibling visual object.
   *
   * @pre The VisualBase has been initialized.
   * @pre The VisualBase has been parented.
   */
  void Lower();

  /**
   * @brief Raise visual object above all other sibling visual objects.
   *
   * @pre The VisualBase has been initialized.
   * @pre The VisualBase has been parented.
   */
  void RaiseToTop();

  /**
   * @brief Lower visual object to the bottom of all other sibling visual objects.
   *
   * @pre The VisualBase has been initialized.
   * @pre The VisualBase has been parented.
   */
  void LowerToBottom();

  /**
   * @brief Raises the visual object above the target visual object.
   *
   * @param[in] target The target visual object
   * @pre The VisualBase has been initialized.
   * @pre The VisualBase has been parented.
   * @pre The target visual object is a sibling.
   */
  void RaiseAbove(VisualBase target);

  /**
   * @brief Lower the visual object to below the target visual object.
   *
   * @param[in] target The target visual object
   * @pre The VisualBase has been initialized.
   * @pre The VisualBase has been parented.
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
  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param[in] object A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL VisualBase(Dali::Ui::Internal::VisualBaseImpl* object);
};

/**
 * @}
 */
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_VISUAL_BASE_H
