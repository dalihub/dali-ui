#ifndef DALI_UI_VISUAL_BASE_IMPL_H
#define DALI_UI_VISUAL_BASE_IMPL_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/processor-interface.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/object/weak-handle.h>
#include <limits> ///< for std::numeric_limits
#include <memory> ///< for std::unique_ptr

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/devel-api/visuals/visual-properties-devel.h>
#include <dali-ui-foundation/devel-api/visuals/visual-transform.h>
#include <dali-ui-foundation/devel-api/visuals/visuals-container.h>
#include <dali-ui-foundation/public-api/align-enumerations.h>
#include <dali-ui-foundation/public-api/callback.h>
#include <dali-ui-foundation/public-api/ui-color-manager.h>
#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali-ui-foundation/public-api/view-types.h>
#include <dali-ui-foundation/public-api/visuals/visual-base.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
class VisualBaseImpl;
using VisualBaseImplPtr = Dali::IntrusivePtr<VisualBaseImpl>;

class VisualBaseImpl : public Dali::BaseObject, public Dali::ConnectionTracker, public Dali::Integration::Processor
{
public:
  using VisualPropertyId                                       = uint32_t;
  static constexpr VisualPropertyId INVALID_VISUAL_PROPERTY_ID = std::numeric_limits<VisualPropertyId>::max();

  /**
   * @brief Creates a VisualBase object.
   *
   * @param[in] type The type of visual base.
   * @return The newly created visual object.
   */
  static VisualBaseImplPtr New(Dali::Ui::VisualType type);

public: ///< Public API
  /**
   * @copydoc Dali::Ui::VisualBase::GetOwner()
   */
  Dali::Ui::View GetOwner() const;

  /**
   * @copydoc Dali::Ui::VisualBase::GetContainerRangeType()
   */
  Dali::Ui::DevelVisual::InternalContainerRangeType GetInternalContainerRangeType() const;

  /**
   * @copydoc Dali::Ui::VisualBase::Detach()
   */
  void DetachFromContainer();

  /**
   * @copydoc Dali::Ui::VisualBase::DoAction()
   */
  void DoAction(Dali::Property::Index actionId, const Dali::Property::Value& attributes);

  /**
   * @copydoc Dali::Ui::VisualBase::GetVisualType()
   */
  Dali::Ui::VisualType GetVisualType() const
  {
    return mVisualType;
  }

public: // SetProperty / GetProperty
  /**
   * @copydoc Dali::Ui::VisualBase::SetProperty()
   */
  void SetProperty(Dali::Property::Index index, Dali::Property::Value propertyValue);

  /**
   * @copydoc Dali::Ui::VisualBase::GetProperty()
   */
  Dali::Property::Value GetProperty(Dali::Property::Index index) const;

public:
  /**
   * @copydoc Dali::Ui::VisualBase::GetName()
   */
  const Dali::String& GetName() const;

  /**
   * @copydoc Dali::Ui::VisualBase::SetName()
   */
  void SetName(const Dali::String& name);

  /**
   * @copydoc Dali::Ui::VisualBase::GetColor()
   */
  UiColor GetColor() const;

  /**
   * @copydoc Dali::Ui::VisualBase::SetColor()
   */
  void SetColor(const UiColor& color);

  // Transform relative properties
  /**
   * @copydoc Dali::Ui::VisualBase::GetOffsetX()
   */
  float GetOffsetX() const;

  /**
   * @copydoc Dali::Ui::VisualBase::SetOffsetX()
   */
  void SetOffsetX(float x);

  /**
   * @copydoc Dali::Ui::VisualBase::GetOffsetY()
   */
  float GetOffsetY() const;

  /**
   * @copydoc Dali::Ui::VisualBase::SetOffsetY()
   */
  void SetOffsetY(float y);

  /**
   * @copydoc Dali::Ui::VisualBase::GetWidth()
   */
  float GetWidth() const;

  /**
   * @copydoc Dali::Ui::VisualBase::SetWidth()
   */
  void SetWidth(float width);

  /**
   * @copydoc Dali::Ui::VisualBase::GetHeight()
   */
  float GetHeight() const;

  /**
   * @copydoc Dali::Ui::VisualBase::SetHeight()
   */
  void SetHeight(float height);

  /**
   * @copydoc Dali::Ui::VisualBase::GetProportionFlags()
   */
  Dali::Ui::Visual::Transform::ProportionFlags GetProportionFlags() const;

  /**
   * @copydoc Dali::Ui::VisualBase::SetProportionFlags()
   */
  void SetProportionFlags(Dali::Ui::Visual::Transform::ProportionFlags flags);

  /**
   * @copydoc Dali::Ui::VisualBase::GetExtraWidth()
   */
  float GetExtraWidth() const;

  /**
   * @copydoc Dali::Ui::VisualBase::SetExtraWidth()
   */
  void SetExtraWidth(float extraWidth);

  /**
   * @copydoc Dali::Ui::VisualBase::GetExtraHeight()
   */
  float GetExtraHeight() const;

  /**
   * @copydoc Dali::Ui::VisualBase::SetExtraHeight()
   */
  void SetExtraHeight(float extraHeight);

  /**
   * @copydoc Dali::Ui::VisualBase::GetOrigin()
   */
  Dali::Ui::Align::Type GetOrigin() const;

  /**
   * @copydoc Dali::Ui::VisualBase::SetOrigin()
   */
  void SetOrigin(Dali::Ui::Align::Type origin);

  /**
   * @copydoc Dali::Ui::VisualBase::GetPivot()
   */
  Dali::Ui::Align::Type GetPivot() const;

  /**
   * @copydoc Dali::Ui::VisualBase::SetPivot()
   */
  void SetPivot(Dali::Ui::Align::Type pivot);

  // Decorated properties (CornerRadius / Borderline)
  /**
   * @copydoc Dali::Ui::VisualBase::GetBorderlineColor()
   */
  UiColor GetBorderlineColor() const;

  /**
   * @copydoc Dali::Ui::VisualBase::SetBorderlineColor()
   */
  void SetBorderlineColor(const UiColor& color);

public: /// Sibling Order Change API
  /**
   * @copydoc Dali::Ui::VisualBase::GetSiblingOrder()
   */
  uint32_t GetSiblingOrder() const;

  /**
   * @copydoc Dali::Ui::VisualBase::SetSiblingOrder()
   */
  void SetSiblingOrder(uint32_t siblingOrder);

  /**
   * @copydoc Dali::Ui::VisualBase::Raise()
   */
  void Raise();

  /**
   * @copydoc Dali::Ui::VisualBase::Lower()
   */
  void Lower();

  /**
   * @copydoc Dali::Ui::VisualBase::RaiseToTop()
   */
  void RaiseToTop();

  /**
   * @copydoc Dali::Ui::VisualBase::LowerToBottom()
   */
  void LowerToBottom();

  /**
   * @copydoc Dali::Ui::VisualBase::RaiseAbove()
   */
  void RaiseAbove(Dali::Ui::Internal::VisualBaseImpl& target);

  /**
   * @copydoc Dali::Ui::VisualBase::LowerBelow()
   */
  void LowerBelow(Dali::Ui::Internal::VisualBaseImpl& target);

public: ///< Called from Internal::VisualsContainer
  /**
   * @brief Get the Container who contain this visual object.
   *
   * @return VisualsContainer who contain this visual object. Or empty container if visual object is not in container.
   */
  Dali::Ui::VisualsContainer GetContainer() const;

  /**
   * @brief Set the container who contain this visual object.
   * @note It should be called only from Internal::VisualBaseImpl or Internal::VisualsContainer.
   *
   * @param[in] container VisualsContainer who contain this visual object. Or empty container if visual object is not in container.
   */
  void AttachToContainerInternal(Dali::Ui::VisualsContainer container);

  /**
   * @brief Detach from container without change the container.
   * @note It should be called only from Internal::VisualBaseImpl or Internal::VisualsContainer.
   */
  void DetachFromContainerInternal();

  /**
   * @brief Set the sibling order without change the container.
   * @note It should be called only from Internal::VisualBaseImpl or Internal::VisualsContainer.
   *
   * @param[in] siblingOrder The sibling order inside of the container.
   */
  void SetSiblingOrderInternal(uint32_t siblingOrder);

  /**
   * @brief Get the depth index of visual, depend by sibling order.
   * @note It should be called only from Internal::VisualBaseImpl or Internal::VisualsContainer.
   *
   * @return The depth index of visual.
   */
  int32_t GetDepthIndex() const;

  /**
   * @brief Sets the shadow type for this visual object.
   *
   * This method defines how the visual object should be treated in terms of shadow rendering
   * and corner radius calculations. The shadow type influences the internal constraints
   * and properties applied to the visual, especially when it's registered with a control
   * that supports corner radius overrides.
   *
   * @param[in] shadowType The type of shadow to be applied to the visual object.
   *                       Can be `NONE`, `BOX_SHADOW`, or `INNER_SHADOW`.
   * @see GetShadowType(), Dali::VisualsContainer::ShadowType
   */
  void SetShadowType(Dali::Ui::VisualsContainer::ShadowType shadowType)
  {
    mShadowType = shadowType;
  }

  /**
   * @brief Retrieves the currently configured shadow type for this visual object.
   *
   * This method returns the shadow type that was previously set via `SetShadowType`.
   * It allows other parts of the system, particularly the VisualsContainer during
   * visual registration or property updates, to query the visual's shadow behavior
   * and apply the corresponding rendering logic and constraints.
   *
   * @return The shadow type of the visual object.
   * @see SetShadowType(), Dali::Ui::VisualsContainer::ShadowType
   */
  Dali::Ui::VisualsContainer::ShadowType GetShadowType() const
  {
    return mShadowType;
  }

  /**
   * @brief Get the Visual::Base object what this object hold.
   * @note It should be called only from Internal::VisualBase or Internal::VisualsContainer.
   *
   * @return Latest created Visual::Base object created by VisualBase::CreateVisual();
   */
  Dali::Ui::Visual::Base GetVisual() const
  {
    return mVisual;
  }

  /**
   * @brief Set the unique id of visual what it registered into control.
   *
   * @param[in] visualPropertyId The id of visual property registered.
   */
  void SetVisualPropertyId(VisualPropertyId visualPropertyId)
  {
    mVisualPropertyId = visualPropertyId;
  }

  /**
   * @brief Get the unique id of visual what it registered into control.
   *
   * @return The id of visual property registered
   */
  VisualPropertyId GetVisualPropertyId() const
  {
    return mVisualPropertyId;
  }

public: ///< Called from other internal class
  /**
   * @brief Retrieve the property object associated with the given visual property key.
   * It will return invalid property if visual object is not be attached to any view.
   * @note Returned Property will be invalidated after we call CreateVisual method.
   *
   * @param[in] visualPropertyKey The key of the visual's property.
   * @return The Property object
   */
  Dali::Property GetPropertyObject(Dali::Property::Key visualPropertyKey);

  /**
   * @brief Remove cached data of given index.
   * @note We can call this function at const case since cache is mutable.
   *
   * @param[in] index The index want to remove cache.
   */
  void RemoveCache(Dali::Property::Index index) const;

  /**
   * @brief Update properties manually
   */
  void UpdateProperty();

  /**
   * @brief Performs an action on the visual with the given action id and attributes.
   *
   * @param[in] actionId The id of the action to perform this API only takes an Index
   * @param[in] attributes The list of attributes for the action. ( optional for this data structure to have content )
   */
  void DoActionExtension(Dali::Property::Index actionId, const Dali::Any& attributes);

protected: // From processor-interface
  /**
   * @copydoc Dali::Integration::Processor::Process()
   */
  void Process(bool postProcessor) override;

  /**
   * @copydoc Dali::Integration::Processor::GetProcessorName()
   */
  std::string_view GetProcessorName() const override
  {
    return "VisualBaseImpl";
  }

private:
  /**
   * @brief Create Visual with Property::Map.
   * @note It will remove previous visual.
   *
   * @param[in] map The Creation information of the visual.
   */
  void CreateVisual(const Dali::Property::Map& map);

  /**
   * @brief Get the visual property map.
   *
   * @param[out] map The visual property map.
   */
  void RetrieveVisualPropertyMap(Dali::Property::Map& map) const;

  /**
   * @brief Set color value internally.
   */
  void SetColorInternal(const Vector4& color);

  /**
   * @brief Set borderline color value internally.
   */
  void SetBorderlineColorInternal(const Vector4& borderlineColor);

  /**
   * @brief Ensure to create Transform class, and return it
   *
   * @return Transform info.
   */
  Internal::Visual::Transform& GetOrCreateTransform()
  {
    if(DALI_UNLIKELY(!mTransform))
    {
      mTransform.reset(new Internal::Visual::Transform());
    }
    DALI_ASSERT_ALWAYS(mTransform && "Transform must not be null here!");
    return *(mTransform.get());
  }

  /**
   * @brief Apply transform infomations to cched visual property map
   */
  void ApplyTransfromToPropertyMap();

  /**
   * @brief Register processor for update property
   */
  void RequestUpdateProperty();

  /**
   * @brief Register processor for apply fitting mode
   */
  void RequestApplyFittingMode();

  /**
   * @brief Update properties for visual
   */
  void UpdatePropertyInternal();

  /**
   * @brief Apply fitting mode for visual
   *
   * @param[in] controlSize The size of the parent control.
   * @param[in] viewPadding The padding of the parent control (start, end, top, bottom).
   *                        For RTL layouts, start/end should already be swapped by the caller.
   * @param[in] effectiveScale The effective scale for the owner. It should be multiplied at viewPadding.
   */
  void ApplyFittingModeInternal(const Vector2& controlSize, const Extents& viewPadding, float effectiveScale);

protected:
  /**
   * @brief Constructor - creates a VisualBaseImpl.
   */
  VisualBaseImpl(Dali::Ui::VisualType type);

  /**
   * @brief Registers a color binding for theme-aware color updates.
   *
   * @tparam T        Type of the instance (ViewImpl or a derived class)
   * @param[in] bindingId  Caller-defined identifier for this binding
   * @param[in] color      The UiColor to apply
   * @param[in] inst       The object whose @a setter will be used as the callback
   * @param[in] setter     Member function called both immediately and on theme change
   */
  template<typename T>
  void SetColorBinding(StringView bindingId, const UiColor& color, T* inst, void (T::*setter)(const Vector4&))
  {
    auto manager = UiColorManager::Get();
    auto self    = Dali::BaseHandle(this);
    if(color.HasColorId())
    {
      if(!manager.HasBinding(self, bindingId))
      {
        manager.RegisterBinding(self, bindingId, ColorCallback::New(inst, setter));
      }
      manager.SetBindingColor(self, bindingId, color);
    }
    else
    {
      manager.ClearBinding(self, bindingId);
    }
    (inst->*setter)(color.GetRgba());
  }

public:
  /**
   * @brief Destructor.
   */
  ~VisualBaseImpl() override;

private:
  Dali::WeakHandle<Dali::Ui::VisualsContainer> mContainer;

  const Dali::Ui::VisualType mVisualType;

  Dali::String          mName;
  mutable Property::Map mCachedVisualPropertyMap;        ///< Whole collected map of properties.
  Property::Map         mUpdatedMutableVisualProperties; ///< Temporal properties that was

  std::unique_ptr<Internal::Visual::Transform> mTransform;

  Dali::Ui::Visual::Base mVisual; ///< Created visual by CreateVisual API

  Dali::Ui::DevelVisual::InternalContainerRangeType mRangeType{Dali::Ui::DevelVisual::InternalContainerRangeType::INVALID};

  Dali::Ui::VisualsContainer::ShadowType mShadowType{Dali::Ui::VisualsContainer::ShadowType::NONE};

  uint32_t         mSiblingOrder{0u};
  VisualPropertyId mVisualPropertyId{INVALID_VISUAL_PROPERTY_ID};

  enum class PropertyUpdatedStatus
  {
    IDLE,                       ///< Nothing be changed after create visual
    MUTABLE_PROPERTY_CHANGED,   ///< Only mutable properties are changed
    IMMUTABLE_PROPERTY_CHANGED, ///< At least one of immutable properties are changed.
  };

  PropertyUpdatedStatus mPropertyUpdatedStatus : 3;

  bool mTransformChanged : 1;           ///< Whether transform properties changed.
  bool mUpdatePropertyRegistered : 1;   ///< Whether the processor for update property is registered.
  bool mApplyFittingModeRegistered : 1; ///< Whether the processor for apply fitting mode is registered.
};
} // namespace Internal

// Helpers for api forwarding methods

inline static Internal::VisualBaseImpl& GetImplementation(Dali::Ui::VisualBase& handle)
{
  DALI_ASSERT_ALWAYS(handle && "VisualBase handle is empty.");

  Dali::BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::VisualBaseImpl&>(object);
}

inline static const Internal::VisualBaseImpl& GetImplementation(const Dali::Ui::VisualBase& handle)
{
  DALI_ASSERT_ALWAYS(handle && "VisualBase handle is empty.");

  const Dali::BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::VisualBaseImpl&>(object);
}

} // namespace Ui
} // namespace Dali

#endif // DALI_UI_VISUAL_BASE_IMPL_H
