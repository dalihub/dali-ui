#ifndef DALI_UI_INTERNAL_POPUP_H
#define DALI_UI_INTERNAL_POPUP_H

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
#include <dali/devel-api/animation/animation-data.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/animation/animation.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/controls/control-devel.h>
#include <dali-ui-foundation/devel-api/controls/popup/popup.h>
#include <dali-ui-foundation/devel-api/controls/table-view/table-view.h>
#include <dali-ui-foundation/internal/controls/control/control-data-impl.h>
#include <dali-ui-foundation/public-api/controls/control-impl.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
class Popup;

typedef IntrusivePtr<Popup> PopupPtr;

/**
 * @copydoc Ui::Popup
 */
class Popup : public Control
{
public:
  /**
   * Create a new Popup.
   * @return A public handle to the newly allocated Popup.
   */
  static Dali::Ui::Popup New();

public:
  /**
   * @copydoc Ui::Popup::SetPopupBackgroundImage
   */
  void SetPopupBackgroundImage(Actor image);

  /**
   * @copydoc Ui::Popup::GetPopupBackgroundImage
   */
  Actor GetPopupBackgroundImage() const;

  /**
   * @copydoc Ui::Popup::SetTitle( Actor titleActor )
   */
  void SetTitle(Actor titleActor);

  /**
   * @copydoc Ui::Popup::GetTitle
   */
  Actor GetTitle() const;

  /**
   * @copydoc Ui::Popup::SetContent
   */
  void SetContent(Actor content);

  /**
   * @copydoc Ui::Popup::GetContent
   */
  Actor GetContent() const;

  /**
   * @copydoc Ui::Popup::SetFooter
   */
  void SetFooter(Actor control);

  /**
   * @copydoc Ui::Popup::GetFooter
   */
  Actor GetFooter() const;

  /**
   * @copydoc Ui::Popup::SetDisplayState
   */
  void SetDisplayState(Ui::Popup::DisplayState displayState);

  /**
   * @copydoc Ui::Popup::GetDisplayState
   */
  Ui::Popup::DisplayState GetDisplayState() const;

  /**
   * @copydoc Ui::Popup::SetTailVisibility
   */
  void SetTailVisibility(bool visible);

  /**
   * @copydoc Ui::Popup::IsTailVisible
   */
  bool IsTailVisible() const;

  /**
   * @copydoc Ui::Popup::SetTailPosition
   */
  void SetTailPosition(Vector3 position);

  /**
   * @copydoc Ui::Popup::GetTailPosition
   */
  const Vector3& GetTailPosition() const;

  /**
   * @copydoc Ui::Popup::SetContextualMode
   */
  void SetContextualMode(Ui::Popup::ContextualMode mode);

  /**
   * @copydoc Ui::Popup::GetContextualMode
   */
  Ui::Popup::ContextualMode GetContextualMode() const;

  /**
   * @copydoc Ui::Popup::SetAnimationDuration
   */
  void SetAnimationDuration(float duration);

  /**
   * @copydoc Ui::Popup::GetAnimationDuration
   */
  float GetAnimationDuration() const;

  /**
   * @copydoc Ui::Popup::SetAnimationMode
   */
  void SetAnimationMode(Ui::Popup::AnimationMode animationMode);

  /**
   * @copydoc Ui::Popup::GetAnimationMode
   */
  Ui::Popup::AnimationMode GetAnimationMode() const;

  /**
   * @copydoc Ui::Popup::SetAutoHideDelay
   */
  void SetAutoHideDelay(int delay);

  /**
   * @copydoc Ui::Popup::GetAutoHideDelay
   */
  int GetAutoHideDelay() const;

  /**
   * @copydoc Ui::Popup::SetBackingEnabled
   */
  void SetBackingEnabled(bool enabled);

  /**
   * @copydoc Ui::Popup::IsBackingEnabled
   */
  bool IsBackingEnabled() const;

  /**
   * @copydoc Ui::Popup::SetBackingColor
   */
  void SetBackingColor(Vector4 color);

  /**
   * @copydoc Ui::Popup::GetBackingColor
   */
  const Vector4& GetBackingColor() const;

  /**
   * @copydoc Ui::Popup::SetTailUpImage
   */
  void SetTailUpImage(std::string image);

  /**
   * @copydoc Ui::Popup::GetTailUpImage
   */
  const std::string& GetTailUpImage() const;

  /**
   * @copydoc Ui::Popup::SetTailDownImage
   */
  void SetTailDownImage(std::string image);

  /**
   * @copydoc Ui::Popup::GetTailDownImage
   */
  const std::string& GetTailDownImage() const;

  /**
   * @copydoc Ui::Popup::SetTailLeftImage
   */
  void SetTailLeftImage(std::string image);

  /**
   * @copydoc Ui::Popup::GetTailLeftImage
   */
  const std::string& GetTailLeftImage() const;

  /**
   * @copydoc Ui::Popup::SetTailRightImage
   */
  void SetTailRightImage(std::string image);

  /**
   * @copydoc Ui::Popup::GetTailRightImage
   */
  const std::string& GetTailRightImage() const;

  /**
   * Called when a property of an object of this type is set.
   * @param[in] object The object whose property is set.
   * @param[in] propertyIndex The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty(BaseObject* object, Property::Index propertyIndex, const Property::Value& value);

  /**
   * Called to retrieve a property of an object of this type.
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] propertyIndex The property index.
   * @return The current value of the property.
   */
  static Property::Value GetProperty(BaseObject* object, Property::Index propertyIndex);

protected:
  class PopupAccessible : public DevelControl::ControlAccessible
  {
  public:
    using DevelControl::ControlAccessible::ControlAccessible;

    /**
     * @copydoc Dali::Ui::DevelControl::ControlAccessible::GetNameRaw()
     */
    std::pair<std::string, bool> GetNameRaw() const override;

    /**
     * @copydoc Dali::Ui::DevelControl::ControlAccessible::CalculateStates()
     */
    Dali::Accessibility::States CalculateStates() override;
  };

  /**
   * Construct a new Popup.
   */
  Popup();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Popup();

private:
  /**
   * @brief Creates the layout of the popup, to be done just before showing for the first time.
   * Also calls OnLayoutSetup() to allow derived classes to perform layout at this stage.
   */
  void LayoutPopup();

  /**
   * @brief Creates or destroys the popup tail based on the current TAIL_DISPLAYED property.
   * Also uses the TAIL_POSITION property to position it.
   */
  void LayoutTail();

  /**
   * @brief Performs any relative positioning required based on the current contextual mode, if set.
   * If contextual mode is not enabled, this method has no effect.
   * @param[in] size The Popups current size (can be accessed from within the OnRelayout() method).
   */
  void LayoutContext(const Vector2& size);

  /**
   * @brief All transition-in animation setup and layout is done here.
   * Different types of animation mode require different layouts to work,
   * this function encapsulates anything animation-mode specific.
   * This is called once for multiple displays/hides of the pops.
   * It is only re-called when the layout becomes dirty.
   */
  void LayoutAnimation();

  /**
   * @brief Initiates a transition-in or transition-out animation based
   * on the current animation settings.
   * @param[in] transitionIn True to perform a transition-in, false for transition out.
   * @param[in] instantaneous Optional - If set to true will override the duration to provide an instant animation.
   */
  void StartTransitionAnimation(bool transitionIn, bool instantaneous = false);

  /**
   * @brief Invoked once a display state change has completed.
   */
  void DisplayStateChangeComplete();

  /**
   * @brief This is called when the auto-hide timer finishes.
   * It performs a display-state change to HIDDEN.
   * @return True as signal is consumed.
   */
  bool OnAutoHideTimeReached();

  /**
   * @brief Create Dimmed Backing (covers all content behind the dialog).
   *
   * @return The backing control.
   */
  Ui::Control CreateBacking();

  /**
   * @brief Creates the lower area within the popup.
   */
  void CreateFooter();

  /**
   * @brief Sets if the popup allows touch events to pass through or not.
   *
   * @param[in] enabled Set to true to make the popup touch-transparent.
   */
  void SetTouchTransparent(bool enabled);

  /**
   * @brief Returns if the popup allows touch events to pass through or not.
   *
   * @return True if the popup is touch-transparent.
   */
  bool IsTouchTransparent() const;

  /**
   * @brief Allows the popup entry animation to be setup from a Property::Map that could
   * originate, for example, from a JSON file.
   *
   * @param[in] map A Property::Map containing a description of an animation
   */
  void SetEntryAnimationData(const Property::Map& map);

  /**
   * @brief Allows the popup exit animation to be setup from a Property::Map that could
   * originate, for example, from a JSON file.
   *
   * @param[in] map A Property::Map containing a description of an animation
   */
  void SetExitAnimationData(const Property::Map& map);

  /**
   * @briefs Updates the popup background's position and size.
   */
  void UpdateBackgroundPositionAndSize();

public: // Signals
  /**
   * @copydoc Dali::Ui::Popup::OutsideTouchedSignal()
   */
  Ui::Popup::TouchedOutsideSignalType& OutsideTouchedSignal();

  /**
   * @copydoc Dali::Ui::Popup::ShowingSignal()
   */
  Ui::Popup::DisplayStateChangeSignalType& ShowingSignal();

  /**
   * @copydoc Dali::Ui::Popup::ShownSignal()
   */
  Ui::Popup::DisplayStateChangeSignalType& ShownSignal();

  /**
   * @copydoc Dali::Ui::Popup::HidingSignal()
   */
  Ui::Popup::DisplayStateChangeSignalType& HidingSignal();

  /**
   * @copydoc Dali::Ui::Popup::HiddenSignal()
   */
  Ui::Popup::DisplayStateChangeSignalType& HiddenSignal();

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

private:
  /**
   * Signal occurs when the State animation (transition from hide <-> show) finishes.
   * @param[in] source The animation that just finished.
   */
  void OnDisplayChangeAnimationFinished(Animation& source);

  /**
   * Signal occurs when the dimmed backing for the Popup is touched.
   * @param[in] actor The Actor Touched
   * @param[in] touch The Touch Data.
   * @return Whether to consume event or not.
   */
  bool OnBackingTouched(Actor actor, const TouchEvent& touch);

  /**
   * Signal occurs when a mouse wheel event occurs on the dimmed backing.
   * @param[in] actor The Actor that got the wheel event.
   * @param[in] event The Wheel Event.
   * @return Whether to consume event or not.
   */
  bool OnBackingWheelEvent(Actor actor, const WheelEvent& event);

  /**
   * Signal occurs when the dialog has been touched.
   * @param[in] actor The Actor Touched
   * @param[in] touch The Touch Data.
   * @return Whether to consume event or not.
   */
  bool OnDialogTouched(Actor actor, const TouchEvent& touch);

  /**
   * @copydoc Ui::Control::OnInitialize()
   */
  void OnInitialize() override;

  /**
   * @copydoc Ui::Internal::Control::CreateAccessibleObject()
   */
  DevelControl::ControlAccessible* CreateAccessibleObject() override;

  /**
   * Called whenever the popup layout is re-set up.
   * Normally due to a change in contents.
   * Note: This is only done when the popup is shown.
   */
  virtual void OnLayoutSetup()
  {
  }

  /**
   * Called when the popup is directly or indirectly parented to the stage.
   */
  void OnSceneConnection(int depth) override;

  /**
   * From Control; called after a child has been added to the owning actor.
   * @param[in] child The child which has been added.
   */
  void OnChildAdd(Actor& child) override;

  /**
   * @copydoc Control::OnRelayOut()
   */
  void OnRelayout(const Vector2& size, RelayoutContainer& container) override;

  /**
   * @copydoc Control::OnSetResizePolicy()
   */
  void OnSetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension) override;

  /**
   * @copydoc Control::GetNaturalSize()
   */
  Vector3 GetNaturalSize() override;

  /**
   * @copydoc Control::GetHeightForWidth()
   */
  float GetHeightForWidth(float width) override;

  /**
   * @copydoc Control::GetWidthForHeight()
   */
  float GetWidthForHeight(float height) override;

  /**
   * @copydoc Control::OnKeyEvent()
   */
  bool OnKeyEvent(const KeyEvent& event) override;

  /**
   * @copydoc Control::GetNextKeyboardFocusableActor()
   */
  Actor GetNextKeyboardFocusableActor(Actor currentFocusedActor, Ui::Control::KeyboardFocus::Direction direction,
                                      bool loopEnabled);

private:
  /**
   * Recursively add any focusable actors or layout containers to the provided vector.
   * Include the top level actor if it is a layout container.
   *
   * @param[in]     parent          The actor to start from
   * @param[in/out] focusableActors The vector to add focusable actors to
   */
  void AddFocusableChildren(Actor parent, std::vector<Actor>& focusableActors);

  /**
   * Recursively add any focusable actors or layout containers to the provided vector.
   *
   * @param[in]     parent          The actor to start from
   * @param[in/out] focusableActors The vector to add focusable actors to
   */
  void AddFocusableChildrenRecursive(Actor parent, std::vector<Actor>& focusableActors);

  /**
   * Sets up the touch signals connections as required.
   * @note This must be called after all the members have been created.
   */
  void SetupTouch();

private:
  // Undefined.
  Popup(const Popup&);

  // Undefined.
  Popup& operator=(const Popup& rhs);

private:
  Ui::Popup::TouchedOutsideSignalType mTouchedOutsideSignal;
  Ui::Popup::DisplayStateChangeSignalType mShowingSignal;
  Ui::Popup::DisplayStateChangeSignalType mShownSignal;
  Ui::Popup::DisplayStateChangeSignalType mHidingSignal;
  Ui::Popup::DisplayStateChangeSignalType mHiddenSignal;

  Layer mLayer;                ///< Popup Layer (i.e. Dim backing and PopupBg reside in this).
  Ui::TableView mPopupLayout;  ///< Popup Background (i.e. dialog reside in this).
  Ui::Control mBacking;        ///< Backing actor (dim effect).
  Actor mPreviousFocusedActor; ///< Store the previous focused actor to restore the focus when popup hide.
  Actor mTailImage;            ///< Stores the tail image.
  Actor mPopupContainer;       ///< This actor is used to house the background image and the main popup layout.
  Animation mAnimation;        ///< The current animation in use used to manage display state changing.
  bool mAlterAddedChild;       ///< Flag used to control whether children are reparented or not.
  bool mLayoutDirty;           ///< Set to true whenever any property that would require a layout update is modified.
  Timer mAutoHideTimer;        ///< Used to perform an auto-hide of the popup if desired.
  bool mTouchTransparent;      ///< Allows all events to pass through the popup.

  // Main Content related properties:
  Actor mTitle;   ///< Stores the text title.
  Actor mContent; ///< Stores the unselected content.
  Actor mFooter;  ///< Stores the footer content (typically controls).

  // Display related properties.
  Ui::Popup::DisplayState mDisplayState; ///< The current display state of the popup.
  bool mTailVisible;                     ///< True if the popup tail should be visible.
  Vector3 mTailPosition;                 ///< The position of the tail.
  Ui::Popup::ContextualMode
      mContextualMode;      ///< Allows the popup to be layed out adjacent to its parent in different directions.
  float mAnimationDuration; ///< The duration of the transition in and out animations.
  Ui::Popup::AnimationMode mAnimationMode; ///< The animation to use to transition in and out.
  Dali::AnimationData
      mEntryAnimationData; ///< Stores description data that can be used for generating a custom entry animation.
  Dali::AnimationData
      mExitAnimationData;      ///< Stores description data that can be used for generating a custom exit animation.
  unsigned int mAutoHideDelay; ///< If set, will auto-hide the popup after a specified amount of time.

  // Style related properties:
  bool mBackingEnabled;        ///< True if a dimmed backing will be used.
  Vector4 mBackingColor;       ///< The color of the backing.
  Actor mPopupBackgroundImage; ///< Stores the background image.
  Rect<int> mBackgroundBorder; ///< Background border.
  float mMargin;               ///< Internal margin for popup contents.
  std::string mTailUpImage;    ///< Image used for the tail for the up direction.
  std::string mTailDownImage;  ///< Image used for the tail for the down direction.
  std::string mTailLeftImage;  ///< Image used for the tail for the left direction.
  std::string mTailRightImage; ///< Image used for the tail for the right direction.
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Ui::Internal::Popup& GetImpl(Ui::Popup& publicObject)
{
  DALI_ASSERT_ALWAYS(publicObject);

  Dali::RefObject& handle = publicObject.GetImplementation();

  return static_cast<Ui::Internal::Popup&>(handle);
}

inline const Ui::Internal::Popup& GetImpl(const Ui::Popup& publicObject)
{
  DALI_ASSERT_ALWAYS(publicObject);

  const Dali::RefObject& handle = publicObject.GetImplementation();

  return static_cast<const Ui::Internal::Popup&>(handle);
}

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_INTERNAL_POPUP_H
