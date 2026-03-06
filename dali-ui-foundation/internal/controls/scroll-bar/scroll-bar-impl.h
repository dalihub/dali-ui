#ifndef DALI_UI_INTERNAL_SCROLL_BAR_H
#define DALI_UI_INTERNAL_SCROLL_BAR_H

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
#include <dali/devel-api/atspi-interfaces/value.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/property-notification.h>
#include <dali/public-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/controls/control-devel.h>
#include <dali-ui-foundation/devel-api/controls/scroll-bar/scroll-bar.h>
#include <dali-ui-foundation/internal/controls/control/control-data-impl.h>
#include <dali-ui-foundation/public-api/controls/control-impl.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
class ScrollBar;

typedef IntrusivePtr<ScrollBar> ScrollBarPtr;

/**
 * ScrollBar is a UI component that can be added to the scrollable controls
 * indicating the current scroll position of the scrollable content.
 */
class ScrollBar : public Control
{
public:
  // Signals
  typedef Ui::ScrollBar::PanFinishedSignalType PanFinishedSignalType;
  typedef Ui::ScrollBar::ScrollPositionIntervalReachedSignalType ScrollPositionIntervalReachedSignalType;

public:
  /**
   * @copydoc Ui::ScrollBar::New()
   */
  static Ui::ScrollBar New(Ui::ScrollBar::Direction direction);

  /**
   * @copydoc Ui::ScrollBar::SetScrollPropertySource()
   */
  void SetScrollPropertySource(Handle handle, Property::Index propertyScrollPosition,
                               Property::Index propertyMinScrollPosition, Property::Index propertyMaxScrollPosition,
                               Property::Index propertyScrollContentSize);

  /**
   * @copydoc Ui::ScrollBar::SetScrollIndicator()
   */
  void SetScrollIndicator(Actor indicator);

  /**
   * @copydoc Ui::ScrollBar::GetScrollIndicator()
   */
  Actor GetScrollIndicator();

  /**
   * @copydoc Ui::ScrollBar::SetScrollPositionIntervals()
   */
  void SetScrollPositionIntervals(const Dali::Vector<float>& positions);

  /**
   * @copydoc Ui::ScrollBar::GetScrollPositionIntervals()
   */
  Dali::Vector<float> GetScrollPositionIntervals() const;

  /**
   * @copydoc Ui::ScrollBar::SetScrollDirection()
   */
  void SetScrollDirection(Ui::ScrollBar::Direction direction);

  /**
   * @copydoc Ui::ScrollBar::GetScrollDirection()
   */
  Ui::ScrollBar::Direction GetScrollDirection() const;

  /**
   * @copydoc Ui::ScrollBar::SetIndicatorHeightPolicy()
   */
  void SetIndicatorHeightPolicy(Ui::ScrollBar::IndicatorHeightPolicy policy);

  /**
   * @copydoc Ui::ScrollBar::GetIndicatorHeightPolicy()
   */
  Ui::ScrollBar::IndicatorHeightPolicy GetIndicatorHeightPolicy() const;

  /**
   * @copydoc Ui::ScrollBar::SetIndicatorFixedHeight()
   */
  void SetIndicatorFixedHeight(float height);

  /**
   * @copydoc Ui::ScrollBar::GetIndicatorFixedHeight()
   */
  float GetIndicatorFixedHeight() const;

  /**
   * @copydoc Ui::ScrollBar::SetIndicatorShowDuration()
   */
  void SetIndicatorShowDuration(float durationSeconds);

  /**
   * @copydoc Ui::ScrollBar::GetIndicatorShowDuration()
   */
  float GetIndicatorShowDuration() const;

  /**
   * @copydoc Ui::ScrollBar::SetIndicatorHideDuration()
   */
  void SetIndicatorHideDuration(float durationSeconds);

  /**
   * @copydoc Ui::ScrollBar::GetIndicatorHideDuration()
   */
  float GetIndicatorHideDuration() const;

  /**
   * @copydoc Ui::ScrollBar::ShowIndicator()
   */
  void ShowIndicator();

  /**
   * @copydoc Ui::ScrollBar::HideIndicator()
   */
  void HideIndicator();

  /**
   * @brief Shows indicator until the transient duration has expired
   */
  void ShowTransientIndicator();

  /**
   * @copydoc Ui::ScrollBar::PanFinishedSignal()
   */
  PanFinishedSignalType& PanFinishedSignal()
  {
    return mPanFinishedSignal;
  }

  /**
   * @copydoc Ui::ScrollBar::ScrollPositionIntervalReachedSignal()
   */
  ScrollPositionIntervalReachedSignalType& ScrollPositionIntervalReachedSignal()
  {
    return mScrollPositionIntervalReachedSignal;
  }

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is
   * responsible for deleting the unused functor.
   */
  static bool DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName,
                              FunctorDelegate* functor);

  // Properties

  /**
   * Called when a property of an object of this type is set.
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty(BaseObject* object, Property::Index index, const Property::Value& value);

  /**
   * Called to retrieve a property of an object of this type.
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Property::Value GetProperty(BaseObject* object, Property::Index index);

  /**
   * Performs actions as requested using the action name.
   * @param[in] object The object on which to perform the action.
   * @param[in] actionName The action to perform.
   * @param[in] attributes The attributes with which to perfrom this action.
   * @return true if action has been accepted by this control
   */
  static bool DoAction(BaseObject* object, const std::string& actionName, const Property::Map& attributes);

private: // from Control
  /**
   * @copydoc Ui::Control::OnInitialize
   */
  void OnInitialize() override;

  /**
   * @copydoc Ui::Internal::Control::CreateAccessibleObject()
   */
  DevelControl::ControlAccessible* CreateAccessibleObject() override;

  /**
   * @copydoc Ui::Control::OnPan
   */
  void OnPan(const PanGesture& gesture) override;

  /**
   * @copydoc CustomActorImpl::OnSizeSet( const Vector3& size )
   */
  void OnSizeSet(const Vector3& size) override;

private:
  /**
   * Create the default indicator actor.
   */
  void CreateDefaultIndicatorActor();

  /**
   * Apply constraints for background and indicator.
   * These constraints are based on values from the scroll connector.
   */
  void ApplyConstraints();

  /**
   * Callback when the current scroll position of the scrollable content goes above or
   * below the values specified by SetScrollPositionIntervals().
   * @param[in] source the property notification that triggered this callback
   */
  void OnScrollPositionIntervalReached(PropertyNotification& source);

  /**
   * Process the pan gesture per predefined timeout until the gesture is finished.
   * @return True if the timer should be kept running.
   */
  bool OnPanGestureProcessTick();

  /**
   * Handle SetProperty for scroll direction.
   * @param[in] propertyValue The new property value.
   */
  void OnScrollDirectionPropertySet(Property::Value propertyValue);

  /**
   * Handle SetProperty for scroll indicator height policy.
   * @param[in] propertyValue The new property value.
   */
  void OnIndicatorHeightPolicyPropertySet(Property::Value propertyValue);

private:
  /**
   * Constructor.
   * It initializes ScrollBar members.
   */
  ScrollBar(Ui::ScrollBar::Direction direction);

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ScrollBar();

private:
  Actor mIndicator;          ///< Image of scroll indicator.
  float mIndicatorShowAlpha; ///< The alpha value when the indicator is fully shown
  Animation mAnimation;      ///< Scroll indicator Show/Hide Animation.

  Ui::ScrollBar::Direction mDirection; ///< The direction of scroll bar (vertical or horizontal)

  WeakHandle<Handle> mScrollableObject; ///< Object to be scrolled

  Property::Index mPropertyScrollPosition = 0; ///< Index of scroll position property owned by the object to be scrolled
  Property::Index mPropertyMinScrollPosition =
      0; ///< Index of minimum scroll position property owned by the object to be scrolled
  Property::Index mPropertyMaxScrollPosition =
      1; ///< Index of maximum scroll position property owned by the object to be scrolled
  Property::Index
      mPropertyScrollContentSize; ///< Index of scroll content size property owned by the object to be scrolled

  float mIndicatorShowDuration;      ///< The duration of scroll indicator show animation
  float mIndicatorHideDuration;      ///< The duration of scroll indicator hide animation
  float mTransientIndicatorDuration; ///< The duration before hiding transient indicator

  float mScrollStart;           ///< Scroll Start position (start of drag)
  Vector2 mGestureDisplacement; ///< Gesture Displacement.

  float mCurrentScrollPosition; ///< The current scroll position updated by the pan gesture

  Ui::ScrollBar::IndicatorHeightPolicy
      mIndicatorHeightPolicy;    ///< The height policy of scroll indicator (variable or fixed)
  float mIndicatorFixedHeight;   ///< The fixed height of scroll indicator
  float mIndicatorMinimumHeight; ///< The minimum height for a variable size indicator
  float mIndicatorStartPadding;  ///< The padding at the start of the indicator
  float mIndicatorEndPadding;    ///< The padding at the end of the indicator

  Timer mContractDelayTimer; ///< Timer guarantee contract delay time.
  Timer mPanProcessTimer;    ///< The timer to process the pan gesture after the gesture is started.

  Dali::Vector<float> mScrollPositionIntervals; ///< List of values to receive notification for when the current scroll
                                                ///< position goes above or below them
  PropertyNotification mPositionNotification;   ///< Stores the property notification used for scroll position changes

  PanFinishedSignalType mPanFinishedSignal;
  ScrollPositionIntervalReachedSignalType mScrollPositionIntervalReachedSignal;

  Constraint mIndicatorPositionConstraint;
  Constraint mIndicatorSizeConstraint;

  bool mIsPanning : 1;          ///< Whether the scroll bar is being panned.
  bool mIndicatorFirstShow : 1; ///< True if the indicator has never been shown

protected:
  class ScrollBarAccessible : public DevelControl::ControlAccessible, public Dali::Accessibility::Value
  {
  public:
    using DevelControl::ControlAccessible::ControlAccessible;

    /**
     * @copydoc Dali::Accessibility::Value::GetMinimum()
     */
    double GetMinimum() const override;

    /**
     * @copydoc Dali::Accessibility::Value::GetCurrent()
     */
    double GetCurrent() const override;

    /**
     * @copydoc Dali::Accessibility::Value::GetValueText()
     */
    std::string GetValueText() const override;

    /**
     * @copydoc Dali::Accessibility::Value::GetMaximum()
     */
    double GetMaximum() const override;

    /**
     * @copydoc Dali::Accessibility::Value::SetCurrent()
     */
    bool SetCurrent(double) override;

    /**
     * @copydoc Dali::Accessibility::Value::GetMinimumIncrement()
     */
    double GetMinimumIncrement() const override;

  protected:
    /**
     * @copydoc Dali::Accessibility::Accessible::InitDefaultFeatures()
     */
    void InitDefaultFeatures() override;
  };
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Ui::Internal::ScrollBar& GetImpl(Ui::ScrollBar& scrollBar)
{
  DALI_ASSERT_ALWAYS(scrollBar);

  Dali::RefObject& handle = scrollBar.GetImplementation();

  return static_cast<Ui::Internal::ScrollBar&>(handle);
}

inline const Ui::Internal::ScrollBar& GetImpl(const Ui::ScrollBar& scrollBar)
{
  DALI_ASSERT_ALWAYS(scrollBar);

  const Dali::RefObject& handle = scrollBar.GetImplementation();

  return static_cast<const Ui::Internal::ScrollBar&>(handle);
}

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_INTERNAL_SCROLL_BAR_H
