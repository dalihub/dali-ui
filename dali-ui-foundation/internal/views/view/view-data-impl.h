#ifndef DALI_UI_VIEW_DATA_IMPL_H
#define DALI_UI_VIEW_DATA_IMPL_H

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
#include <dali/devel-api/adaptor-framework/accessibility.h>
#include <dali/devel-api/adaptor-framework/input-method-context.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/processor-interface.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/object/property-notification.h>
#include <string>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-accessible.h>
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/internal/render-effects/offscreen-rendering-impl.h>
#include <dali-ui-foundation/internal/render-effects/render-effect-impl.h>
#include <dali/integration-api/debug.h>
#include <map>
#include <memory>
#include <set>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace Visual
{
class Base;
}

enum class TriStateProperty
{
  AUTO = 0,
  TRUE,
  FALSE
};

/**
 * @brief Holds the Implementation for the internal view class
 */
class ViewDataImpl : public ConnectionTracker, public Dali::Integration::Processor
{
private:
  class AccessibilityData;
  class VisualData;

public:
  /**
   * @brief Retrieves the implementation of the internal view class.
   * @param[in] viewImpl A ref to the view whose internal implementation is required
   * @return The internal implementation
   */
  static ViewDataImpl& Get(Integration::ViewImpl& viewImpl);

  /**
   * @copydoc Get( Integration::ViewImpl& )
   */
  static const ViewDataImpl& Get(const Integration::ViewImpl& viewImpl);

  /**
   * @brief Constructor.
   * @param[in] viewImpl The view which owns this implementation
   */
  ViewDataImpl(Integration::ViewImpl& viewImpl);

  /**
   * @brief Destructor.
   */
  ~ViewDataImpl();

  /**
   * @brief Initialize private VisualData context for this impl.
   */
  void InitializeVisualData();

  /**
   * @brief Called when a pinch is detected.
   * @param[in] actor The actor the pinch occurred on
   * @param[in] pinch The pinch gesture details
   */
  void PinchDetected(Actor actor, const PinchGesture& pinch);

  /**
   * @brief Called when a pan is detected.
   * @param[in] actor The actor the pan occurred on
   * @param[in] pinch The pan gesture details
   */
  void PanDetected(Actor actor, const PanGesture& pan);

  /**
   * @brief Called when a tap is detected.
   * @param[in] actor The actor the tap occurred on
   * @param[in] pinch The tap gesture details
   */
  void TapDetected(Actor actor, const TapGesture& tap);

  /**
   * @brief Called when a long-press is detected.
   * @param[in] actor The actor the long-press occurred on
   * @param[in] pinch The long-press gesture details
   */
  void LongPressDetected(Actor actor, const LongPressGesture& longPress);

  /**
   * @brief Called when resources of view are ready.
   */
  void ResourceReady();

  /**
   * @copydoc Dali::Ui::View::RegisterVisual()
   */
  void RegisterVisual(Property::Index index, Ui::Visual::Base& visual);

  /**
   * @copydoc Dali::Ui::View::RegisterVisual()
   */
  void RegisterVisual(Property::Index index, Ui::Visual::Base& visual, int depthIndex);

  /**
   * @copydoc Dali::Ui::View::RegisterVisual()
   */
  void RegisterVisual(Property::Index index, Ui::Visual::Base& visual, bool enabled);

  /**
   * @copydoc Dali::Ui::View::RegisterVisual()
   */
  void RegisterVisual(Property::Index index, Ui::Visual::Base& visual, bool enabled, int depthIndex);

  /**
   * @copydoc Dali::Ui::View::UnregisterVisual()
   */
  void UnregisterVisual(Property::Index index);

  /**
   * @copydoc Dali::Ui::View::GetVisual()
   */
  Ui::Visual::Base GetVisual(Property::Index index) const;

  /**
   * @brief Get the raw pointer of visual impl.
   * It will be used when we want to get visual infomations without increase reference counts.
   * @note Only for internal usage.
   *
   * @param[in] index Index of parameter
   * @return Raw pointer of visual base implements. nullptr if not exist.
   */
  Ui::Internal::Visual::Base* GetVisualImplPtr(Property::Index index) const;

  /**
   * @brief Enables or disables overriding the given visual's corner properties to its view's
   * @param[in] visual A registered visual
   * @param[in] enable flat to set enabled or disabled.
   * @param[in] cornerRadiusConstraint Optional constraint to link the view's corner properties to the visual's.
   */
  void EnableCornerPropertiesOverridden(Ui::Visual::Base& visual, bool enable,
                                        Dali::Constraint cornerRadiusConstraint = Dali::Constraint());

  /**
   * @copydoc Dali::Ui::View::EnableVisual()
   */
  void EnableVisual(Property::Index index, bool enable);

  /**
   * @copydoc Dali::Ui::View::IsVisualEnabled()
   */
  bool IsVisualEnabled(Property::Index index) const;

  /**
   * @copydoc Dali::Ui::View::GetVisualResourceStatus()
   */
  Ui::Visual::ResourceStatus GetVisualResourceStatus(Property::Index index) const;

  /**
   * @copydoc Dali::Ui::View::DoAction()
   */
  void DoAction(Dali::Property::Index visualIndex, Dali::Property::Index actionId,
                const Dali::Property::Value& attributes);

  /**
   * @copydoc Dali::Ui::View::DoActionExtension()
   */
  void DoActionExtension(Dali::Property::Index visualIndex, Dali::Property::Index actionId,
                         const Dali::Any& attributes);

  /**
   * @brief Function used to set view properties.
   * @param[in] object The object whose property to set
   * @param[in] index The index of the property to set
   * @param[in] value The value of the property to set
   */
  static void SetProperty(BaseObject* object, Property::Index index, const Property::Value& value);

  /**
   * @brief Function used to retrieve the value of view properties.
   * @param[in] object The object whose property to get
   * @param[in] index The index of the property to get
   * @return The value of the property
   */
  static Property::Value GetProperty(BaseObject* object, Property::Index index);

  /**
   * @brief Whether the resource is ready
   * @return True if the resource is read.
   */
  bool IsResourceReady() const;

  /**
   * @copydoc CustomActorImpl::OnSceneConnection()
   */
  void OnSceneConnection();

  /**
   * @copydoc CustomActorImpl::OnSceneDisconnection()
   */
  void OnSceneDisconnection();

  /**
   * @brief Set the input method context.
   * @param[in] inputMethodContext The input method context.
   */
  void SetInputMethodContext(InputMethodContext& inputMethodContext);

  /**
   * @brief Filter an key event.
   * @param[in] event The key to be filtered.
   * @return True if the key handled, otherwise false.
   */
  bool FilterKeyEvent(const KeyEvent& event);

  /**
   * @brief Get private AccessibilityData context for this impl. If not created yet, it will create new data.
   * @return The l-value of AccessibilityData context.
   */
  [[nodiscard]] AccessibilityData& GetOrCreateAccessibilityData();

  /**
   * @brief Get private AccessibilityData context for this impl.
   * @return The pointer of AccessibilityData context.
   */
  [[nodiscard]] AccessibilityData* GetAccessibilityData() const;

  /**
   * @brief Adds accessibility attribute
   * @param[in] key Attribute name to set
   * @param[in] value Attribute value to set
   *
   * Attribute is added if not existed previously or updated
   * if existed.
   */
  void AppendAccessibilityAttribute(const Dali::String& key, const Dali::String& value);

  /**
   * @brief Removes accessibility attribute
   * @param[in] key Attribute name to remove
   *
   * Function does nothing if attribute doesn't exist.
   */
  void RemoveAccessibilityAttribute(const Dali::String& key);

  /**
   * @brief Removes all accessibility attributes
   */
  void ClearAccessibilityAttributes();

  /**
   * @brief Sets reading info type attributes
   * @param[in] types info type attributes to set
   *
   * This function sets, which part of object will be read out
   * by screen-reader.
   */
  void SetAccessibilityReadingInfoType(const Dali::Accessibility::ReadingInfoTypes types);

  /**
   * @brief Gets currently active reading info type attributes
   */
  Dali::Accessibility::ReadingInfoTypes GetAccessibilityReadingInfoType() const;

  /**
   * @copydoc View::VisualEventSignal()
   */
  Ui::View::VisualEventSignalType& VisualEventSignal();

  /**
   * @brief Sets the shadow with a property map.
   * @param[in] map The shadow property map
   */
  void SetShadow(const Property::Map& map);

  /**
   * @brief Clear the shadow.
   */
  void ClearShadow();

  /**
   * @brief Sets the inner shadow with a property map.
   * @param[in] map The inner shadow property map
   */
  void SetInnerShadow(const Property::Map& map);

  /**
   * @brief Clear the inner shadow.
   */
  void ClearInnerShadow();

  /**
   * @brief Sets the borderline with a property map.
   * @param[in] map The borderline property map
   * @param[in] forciblyCreate Create new visual forcibly, False if we only need to update properties.
   */
  void SetBorderline(const Property::Map& map, bool forciblyCreate);

  /**
   * @brief Clear the borderline.
   */
  void ClearBorderline();

  /**
   * @copydoc View::GetVisualProperty()
   */
  Dali::Property GetVisualProperty(Dali::Property::Index index, Dali::Property::Key visualPropertyKey);

  /**
   * @brief Create constraints to animate animatable properties.
   * @param[in] animationObject BaseObject of Animation or Constraint
   * @param[in] index The animatable property
   */
  void CreateAnimationConstraints(const Dali::BaseObject& animationObject, Property::Index index);

  /**
   * @brief Clear animatable constraints
   * @param[in] animationObject BaseObject of Animation or Constraint
   * @param[in] index The animatable property
   */
  void ClearAnimationConstraints(const Dali::BaseObject& animationObject, Property::Index index);

  /**
   * @brief Update visual properties.
   * @param[in] properties Property list to be used to update visual properties of this View.
   *
   * @note This method triggers View::OnUpdateVisualProperties().
   */
  void UpdateVisualProperties(const std::vector<std::pair<Dali::Property::Index, Dali::Property::Map>>& properties);

  /**
   * @copydoc Dali::Ui::Integration::ViewImpl::GetAccessibleObject()
   */
  std::shared_ptr<Ui::ViewAccessible> GetAccessibleObject();

  /**
   * @copydoc Dali::Ui::View::IsAccessibleCreated()
   */
  bool IsAccessibleCreated() const;

  /**
   * @copydoc Dali::Ui::View::EnableCreateAccessible()
   */
  void EnableCreateAccessible(bool enable);

  /**
   * @copydoc Dali::Ui::View::IsCreateAccessibleEnabled()
   */
  bool IsCreateAccessibleEnabled() const;

  /**
   * @copydoc Dali::Ui::View::EmitAccessibilityStateChanged()
   */
  void EmitAccessibilityStateChanged(Accessibility::State state, int newValue);

  /**
   * @brief Apply fittingMode
   *
   * @param[in] size The size of the view
   */
  void ApplyFittingMode(const Vector2& size);

  /**
   * @brief Register processor
   */
  void RegisterProcessorOnce();

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
    return "ViewDataImpl";
  }

private:
  /**
   * @brief Emits the resource ready signal.
   */
  void EmitResourceReadySignal();
  /**
   * @brief Callbacks called on idle.
   *
   * @return True if we need to call this idle callback one more time.
   */
  bool OnIdleCallback();

  /**
   * Set off-screen rendering.
   * @param[in] offScreenRenderingType enum OffScreenRenderingType
   * @note When offscreen rendering is on, changing visual's depth index may not apply instantaneously. Turn it off and
   * on again.
   */
  void SetOffScreenRendering(int32_t offScreenRenderingType);

  /**
   * Notify to this view's corner radius changed.
   */
  void UpdateCornerRadius();

  /**
   * Notify to this view's borderline changed.
   */
  void UpdateBorderline();

public:
  Integration::ViewImpl& mViewImpl;

  std::unique_ptr<AccessibilityData> mAccessibilityData;
  std::unique_ptr<VisualData>        mVisualData;

  int mLeftFocusableActorId;             ///< Actor ID of Left focusable view.
  int mRightFocusableActorId;            ///< Actor ID of Right focusable view.
  int mUpFocusableActorId;               ///< Actor ID of Up focusable view.
  int mDownFocusableActorId;             ///< Actor ID of Down focusable view.
  int mClockwiseFocusableActorId;        ///< Actor ID of Clockwise focusable view.
  int mCounterClockwiseFocusableActorId; ///< Actor ID of Counter clockwise focusable view.

  Vector4                           mBackgroundColor;    ///< The color of the background visual
  Extents                           mMargin;             ///< Layout margin
  Extents                           mPadding;            ///< Layout padding
  float                             mRequestedWidth;     ///< Requested width (WRAP_CONTENT = -1.0f, MATCH_PARENT = -2.0f)
  float                             mRequestedHeight;    ///< Requested height (WRAP_CONTENT = -1.0f, MATCH_PARENT = -2.0f)
  float                             mMinimumWidth;       ///< Minimum width applied during measurement
  float                             mMinimumHeight;      ///< Minimum height applied during measurement
  float                             mMaximumWidth;       ///< Maximum width applied during measurement
  float                             mMaximumHeight;      ///< Maximum height applied during measurement
  RenderEffectImplPtr               mRenderEffect;       ///< The render effect on this view
  Vector3*                          mStartingPinchScale; ///< The scale when a pinch gesture starts, TODO: consider removing this
  Vector2                           mSize;               ///< The size of the view
  Ui::View::KeyEventSignalType      mKeyEventSignal;
  Ui::View::FocusChangedSignalType  mFocusChangedSignal;
  Ui::View::ResourceReadySignalType mResourceReadySignal;

  // Gesture Detection
  PinchGestureDetector     mPinchGestureDetector;
  PanGestureDetector       mPanGestureDetector;
  TapGestureDetector       mTapGestureDetector;
  LongPressGestureDetector mLongPressGestureDetector;

  // Off screen rendering context
  std::unique_ptr<OffScreenRenderingImpl> mOffScreenRenderingImpl;
  Ui::View::OffScreenRenderingType        mOffScreenRenderingType;
  Ui::View::OffScreenRenderingFinishedSignalType
    mOffScreenRenderingFinishedSignal; ///< Emits only when type is REFRESH_ONCE

  InputMethodContext mInputMethodContext;
  CallbackBase*      mIdleCallback; ///< The idle callback to emit the resource ready signal.

  Integration::ViewImpl::ViewBehaviour mFlags : Integration::ViewImpl::VIEW_BEHAVIOUR_FLAG_COUNT; ///< Flags passed in from constructor.

  // Frequencly touched accessibility relative values.
  // Keep it on ViewDataImpl to avoid AccessibilityData creation.
  int32_t mAccessibilityRole : Dali::Log<static_cast<uint32_t>(AccessibilityRole::MAX_COUNT)>::value + 2;

  bool mIsKeyboardNavigationSupported : 1; ///< Stores whether keyboard navigation is supported by the view.
  bool mIsKeyboardFocusGroup : 1;          ///< Stores whether the view is a focus group.
  bool mIsEmittingResourceReadySignal : 1; ///< True during ResourceReady().
  bool mIdleCallbackRegistered : 1;        ///< True if need to emit the resource ready signal again.
  bool mDispatchKeyEvents : 1;             ///< Whether the actor emits key event signals
  bool mAccessibleCreatable : 1;           ///< Whether we can create new accessible or not.
  bool mProcessorRegistered : 1;           ///< Whether the processor is registered.

  // Properties - these need to be members of Internal::ViewDataImpl as they access private methods/data of
  // Integration::ViewImpl and Internal::ViewDataImpl.
  static const PropertyRegistration PROPERTY_1;
  static const PropertyRegistration PROPERTY_2;
  static const PropertyRegistration PROPERTY_3;
  static const PropertyRegistration PROPERTY_4;
  static const PropertyRegistration PROPERTY_5;
  static const PropertyRegistration PROPERTY_6;
  static const PropertyRegistration PROPERTY_7;
  static const PropertyRegistration PROPERTY_8;
  static const PropertyRegistration PROPERTY_9;
  static const PropertyRegistration PROPERTY_10;
  static const PropertyRegistration PROPERTY_11;
  static const PropertyRegistration PROPERTY_12;
  static const PropertyRegistration PROPERTY_13;
  static const PropertyRegistration PROPERTY_14;
  static const PropertyRegistration PROPERTY_15;
  static const PropertyRegistration PROPERTY_16;
  static const PropertyRegistration PROPERTY_17;
  static const PropertyRegistration PROPERTY_18;
  static const PropertyRegistration PROPERTY_19;
  static const PropertyRegistration PROPERTY_20;
  static const PropertyRegistration PROPERTY_21;
  static const PropertyRegistration PROPERTY_22;
  static const PropertyRegistration PROPERTY_23;
  static const PropertyRegistration PROPERTY_24;
  static const PropertyRegistration PROPERTY_25;
  static const PropertyRegistration PROPERTY_26;
  static const PropertyRegistration PROPERTY_27;
  static const PropertyRegistration PROPERTY_28;
  static const PropertyRegistration PROPERTY_29;
  static const PropertyRegistration PROPERTY_30;
  static const PropertyRegistration PROPERTY_31;
  static const PropertyRegistration PROPERTY_32;
  static const PropertyRegistration PROPERTY_33;
  static const PropertyRegistration PROPERTY_34;
  static const PropertyRegistration PROPERTY_35;
  static const PropertyRegistration PROPERTY_36;
  static const PropertyRegistration PROPERTY_37;
  static const PropertyRegistration PROPERTY_38;
  static const PropertyRegistration PROPERTY_39;

  static const AnimatablePropertyRegistration ANIMATABLE_PROPERTY_1;
  static const AnimatablePropertyRegistration ANIMATABLE_PROPERTY_2;
  static const AnimatablePropertyRegistration ANIMATABLE_PROPERTY_3;
  static const AnimatablePropertyRegistration ANIMATABLE_PROPERTY_4;
  static const AnimatablePropertyRegistration ANIMATABLE_PROPERTY_5;
  static const AnimatablePropertyRegistration ANIMATABLE_PROPERTY_6;
};

} // namespace Internal

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_VIEW_DATA_IMPL_H
