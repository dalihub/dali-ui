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
#include <dali/public-api/common/dali-string.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-foundation/public-api/views/widget/widget-view-properties.h>

namespace DALI_NAMESPACE
{

namespace Ui
{

namespace Text
{
class FontAttributes;
}

namespace Internal DALI_INTERNAL
{
class WidgetViewImpl;
}

/**
 * @addtogroup dali_widget_view
 * @{
 */

/**
 * @brief WidgetView is a class for displaying the widget image and controlling the widget.
 * Input events that WidgetView gets are delivered to the widget.
 * Create widget views using New(). The application and actor hierarchy own the
 * view; releasing their last reference also removes the widget instance.
 * A view disconnected from the scene is paused and can be attached again.
 *
 * @SINCE_3_0.0
 */
class DALI_UI_API WidgetView : public View
{
public:
  /**
   * @brief Properties for generic GetProperty() and SetProperty() access.
   *
   * Applications can use the corresponding typed methods below. These are
   * event-side properties; they are not registered as animatable properties.
   *
   * @privlevel public
   * @privilege %http://tizen.org/privilege/widget.viewer
   * @SINCE_3_0.0
   */
  struct Property
  {
    /**
     * @brief An enumeration of properties belonging to the WidgetView class.
     * @privlevel public
     * @privilege %http://tizen.org/privilege/widget.viewer
     * @SINCE_3_0.0
     */
    enum
    {
      // Event side properties

      /**
       * @brief name "widgetId", Get the id of the widget, type string, read-only
       * @SINCE_3_0.0
       * @see GetWidgetId()
       */
      WIDGET_ID = WidgetViewPropertyIndex::WIDGET_ID,

      /**
       * @brief name "instanceId", type string, read-only
       * @SINCE_3_0.0
       * @see GetInstanceId()
       */
      INSTANCE_ID = WidgetViewPropertyIndex::INSTANCE_ID,

      /**
       * @brief name "contentInfo", type string, read-only
       * @SINCE_3_0.0
       * @see GetContentInfo()
       */
      CONTENT_INFO = WidgetViewPropertyIndex::CONTENT_INFO,

      /**
       * @brief name "title", type string, read-only
       * @SINCE_3_0.0
       * @see GetTitle()
       */
      TITLE = WidgetViewPropertyIndex::TITLE,

      /**
       * @brief name "updatePeriod", type float, read-only
       * @SINCE_3_0.0
       * @see GetUpdatePeriod()
       */
      UPDATE_PERIOD = WidgetViewPropertyIndex::UPDATE_PERIOD,

      /**
       * @brief name "preview", type boolean, enables the preview during loading.
       * Reading returns the setting, not the preview's effective visibility.
       * @SINCE_3_0.0
       * @see SetPreviewEnabled(), IsPreviewEnabled()
       */
      PREVIEW = WidgetViewPropertyIndex::PREVIEW,

      /**
       * @brief name "widgetStateFaulted", type boolean, read-only
       * @SINCE_3_0.0
       * @see IsWidgetFaulted()
       */
      WIDGET_STATE_FAULTED = WidgetViewPropertyIndex::WIDGET_STATE_FAULTED,

      /**
       * @brief name "keepWidgetSize", type boolean
       * @SINCE_3_0.0
       * @see SetKeepWidgetSizeEnabled(), IsKeepWidgetSizeEnabled()
       */
      KEEP_WIDGET_SIZE = WidgetViewPropertyIndex::KEEP_WIDGET_SIZE

    };
  };

  /**
   * @brief Creates a view displaying a platform widget.
   *
   * Call this on the event thread after DALi initialization. Widget views share
   * an engine and must use the same viewer application ID for the application
   * lifetime. The engine and service connection are kept until application
   * shutdown, including while no views exist. Destroying a view releases its
   * widget instance and image resources after deferred cleanup.
   * Initial engine notifications are delivered on a subsequent event-loop turn.
   * Connect the view's signals immediately after this function returns to receive
   * them. Destroying the view before that turn discards initial notifications.
   * The widget is paused until the view is connected to the scene and visible.
   *
   * @privlevel public
   * @privilege %http://tizen.org/privilege/widget.viewer
   * @SINCE_3_0.0
   * @note The library initializes its default loading/retry translation catalog
   * once per localization manager without changing the application default domain.
   * This first initialization binds the widget domain to the library catalog path.
   * @param[in] appId Viewer application ID used for platform IPC.
   * @param[in] widgetId The widget ID.
   * @param[in] contentInfo Contents passed to the widget instance.
   * @param[in] width Initial natural and requested width in logical UI units.
   * @param[in] height Initial natural and requested height in logical UI units.
   * @param[in] updatePeriod The period of updating widget contents.
   * @return A WidgetView handle, or an empty handle if engine initialization or
   * resource initialization, instance creation, or deferred initialization fails,
   * or appId differs from the ID used to initialize the
   * shared engine. Instance creation can also fail while a previous widget's
   * deferred resources are being released.
   * @note A valid handle does not mean provider creation has completed. If the
   * instance exists but provider launch or creation fails, the view remains valid
   * and reports the failure through WidgetCreationAbortedSignal(). Connect this
   * signal immediately after New() returns, as described above.
   * @note WRAP_CONTENT uses the initial natural size supplied by width and height.
   * Parent constraints and provider resizes do not rewrite that measurement input.
   * The provider pixel size follows the arranged bounds unless size keeping is enabled.
   */
  static WidgetView New(const Dali::String& appId, const Dali::String& widgetId, const Dali::String& contentInfo, int width, int height, float updatePeriod);

  /**
   * @brief Downcast a handle to WidgetView handle.
   *
   * If the BaseHandle points is a WidgetView the downcast returns a valid handle.
   * If not the returned handle is left empty.
   *
   * @SINCE_3_0.0
   * @param[in] handle Handle to an object
   * @return handle to a WidgetView or an empty handle
   */
  static WidgetView DownCast(BaseHandle handle);

  /**
   * @brief Creates an empty handle.
   * @SINCE_3_0.0
   */
  WidgetView();

  /**
   * @brief Copy constructor.
   *
   * @SINCE_3_0.0
   * @param[in] handle The handle to copy from.
   */
  WidgetView(const WidgetView& handle);

  /**
   * @brief Assignment operator.
   *
   * @SINCE_3_0.0
   * @param[in] handle The handle to copy from.
   * @return A reference to this.
   */
  WidgetView& operator=(const WidgetView& handle);

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_3_0.0
   */
  ~WidgetView();

  /**
   * @brief Gets the ID of the widget being displayed.
   *
   * @SINCE_3_0.0
   * @return The widget ID.
   */
  Dali::String GetWidgetId() const;

  /**
   * @brief Gets the ID of this widget instance.
   *
   * @SINCE_3_0.0
   * @return The instance ID.
   */
  Dali::String GetInstanceId() const;

  /**
   * @brief Gets the latest available content information from the widget.
   *
   * @SINCE_3_0.0
   * @return The content information, or the previously stored value if querying fails.
   */
  Dali::String GetContentInfo() const;

  /**
   * @brief Gets the widget title.
   *
   * @SINCE_3_0.0
   * @return The widget title, or an empty string if unavailable.
   */
  Dali::String GetTitle() const;

  /**
   * @brief Gets the latest known update period for this widget.
   *
   * The value is initialized from New() and refreshed before
   * WidgetUpdatePeriodChangedSignal() is emitted.
   *
   * @SINCE_3_0.0
   * @return The latest successfully queried update period.
   */
  float GetUpdatePeriod() const;

  /**
   * @brief Queries whether the widget provider is in a faulted state.
   *
   * @SINCE_3_0.0
   * @return True if the instance reports a faulted state.
   */
  bool IsWidgetFaulted() const;

  /**
   * @brief Sets whether to show the preview during loading.
   *
   * After a widget frame is received, enabling the preview keeps it hidden
   * until the widget enters the loading state again.
   *
   * @SINCE_3_0.0
   * @param[in] enabled True to show the preview while loading.
   */
  void SetPreviewEnabled(bool enabled);

  /**
   * @brief Gets whether the preview is enabled.
   *
   * @SINCE_3_0.0
   * @return True if the preview is enabled. This is the setting, not effective visibility.
   */
  bool IsPreviewEnabled() const;

  /**
   * @brief Sets loading text visibility immediately and for subsequent loading states.
   *
   * @SINCE_3_0.0
   * @param[in] visible True to show the loading text.
   */
  void SetLoadingTextVisible(bool visible);

  /**
   * @brief Gets the loading label's current visibility property.
   *
   * @SINCE_3_0.0
   * @return True if the label is visible locally; ancestor visibility is not included.
   */
  bool IsLoadingTextVisible() const;

  /**
   * @brief Sets retry text visibility immediately and for subsequent retry states.
   *
   * @SINCE_3_0.0
   * @param[in] visible True to show the retry text.
   */
  void SetRetryTextVisible(bool visible);

  /**
   * @brief Gets the retry label's current visibility property.
   *
   * @SINCE_3_0.0
   * @return True if the label is visible locally; ancestor visibility is not included.
   */
  bool IsRetryTextVisible() const;

  /**
   * @brief Sets whether layout changes preserve the provider's current size.
   *
   * @SINCE_3_0.0
   * @param[in] enabled True to keep the provider size.
   * @note Setting false does not resize immediately; a subsequent layout or size
   * change applies the new size.
   */
  void SetKeepWidgetSizeEnabled(bool enabled);

  /**
   * @brief Gets whether the provider size is kept across layout changes.
   *
   * @SINCE_3_0.0
   * @return True if the provider size is kept.
   */
  bool IsKeepWidgetSizeEnabled() const;

  /**
   * @brief Sets loading text. An empty string restores the translated default.
   * @SINCE_3_0.0
   * @param[in] text The loading message.
   */
  void SetLoadingText(const Dali::String& text);

  /**
   * @brief Sets retry text. An empty string restores the translated default.
   * @SINCE_3_0.0
   * @param[in] text The retry message.
   */
  void SetRetryText(const Dali::String& text);

  /**
   * @brief Applies explicitly set font attributes to the loading label.
   * @SINCE_3_0.0
   * @param[in] font Partial font overrides. Omitted attributes are unchanged.
   */
  void SetLoadingTextFont(const Text::FontAttributes& font);

  /**
   * @brief Applies explicitly set font attributes to the retry label.
   * @SINCE_3_0.0
   * @param[in] font Partial font overrides. Omitted attributes are unchanged.
   */
  void SetRetryTextFont(const Text::FontAttributes& font);

  /**
   * @brief Sets the loading text color, including a theme color binding.
   * @SINCE_3_0.0
   * @param[in] color The loading text color.
   */
  void SetLoadingTextColor(const UiColor& color);

  /**
   * @brief Sets the retry text color, including a theme color binding.
   * @SINCE_3_0.0
   * @param[in] color The retry text color.
   */
  void SetRetryTextColor(const UiColor& color);

  /**
   * @brief Overrides the image shader for current and subsequent widget frames.
   *
   * An empty source uses that stage of the standard image shader. Native-image
   * adaptation is still performed by the image visual. Two empty sources clear
   * the override. This is distinct from the View's RenderEffect API.
   * @SINCE_3_0.0
   * @param[in] vertexSource Vertex shader source, or empty to use the default.
   * @param[in] fragmentSource Fragment shader source, or empty to use the default.
   */
  void SetCustomShader(const Dali::String& vertexSource, const Dali::String& fragmentSource);

  /**
   * @brief Restores the standard native-image shader and corner handling.
   * @SINCE_3_0.0
   */
  void ClearCustomShader();

  /**
   * @brief Pauses a given widget.
   *
   * A manual pause is retained when visibility or screen position changes.
   *
   * @privlevel public
   * @privilege %http://tizen.org/privilege/widget.viewer
   * @SINCE_3_0.0
   * @return true on success, false otherwise.
   */
  bool PauseWidget();

  /**
   * @brief Resume a given widget.
   *
   * Clears the manual pause request. The widget remains paused while hidden,
   * disconnected from the scene, or entirely outside the window. It resumes
   * automatically when these conditions no longer apply.
   *
   * @privlevel public
   * @privilege %http://tizen.org/privilege/widget.viewer
   * @SINCE_3_0.0
   * @return true on success, false otherwise.
   */
  bool ResumeWidget();

  /**
   * @brief Cancels touch event procedure.
   * If you call this function after feed the touch down event, the widget will get ON_HOLD events.
   * If a widget gets ON_HOLD event, it will not do anything even if you feed touch up event.
   *
   * @privlevel public
   * @privilege %http://tizen.org/privilege/widget.viewer
   * @SINCE_3_0.0
   * @return true on success, false otherwise.
   */
  bool CancelTouchEvent();

  /**
   * @brief Activate a widget in faulted state.
   * Requests a retry for an existing widget whose provider failed.
   * Calling this on a non-faulted or removed widget has no effect.
   * This also resets the per-instance automatic recovery limit.
   *
   * @privlevel public
   * @privilege %http://tizen.org/privilege/widget.viewer
   * @SINCE_3_0.0
   */
  void ActivateFaultedWidget();

public:                                                  //Signals
  typedef Signal<void(WidgetView)> WidgetViewSignalType; ///< WidgetView signal type

  /**
   * @brief This signal is emitted when the first widget frame becomes available.
   *
   * It is emitted again when frames resume after a widget fault.
   *
   * @privlevel public
   * @privilege %http://tizen.org/privilege/widget.viewer
   * @SINCE_3_0.0
   * @return The signal to connect to.
   */
  WidgetViewSignalType& WidgetAddedSignal();

  /**
   * @brief This signal is emitted when a previously added widget loses its displayed content due to a fault.
   *
   * It is emitted before WidgetFaultedSignal(). The widget can recover and
   * emit WidgetAddedSignal() again.
   * Scene disconnection only pauses the widget and does not emit this signal.
   * Destruction of the view disconnects callbacks and does not emit this signal.
   *
   * @privlevel public
   * @privilege %http://tizen.org/privilege/widget.viewer
   * @SINCE_3_0.0
   * @return The signal to connect to.
   */
  WidgetViewSignalType& WidgetDeletedSignal();

  /**
   * @brief This signal is emitted when the widget creation is aborted.
   *
   * @privlevel public
   * @privilege %http://tizen.org/privilege/widget.viewer
   * @SINCE_3_0.0
   * @return The signal to connect to.
   */
  WidgetViewSignalType& WidgetCreationAbortedSignal();

  /**
   * @brief This signal is emitted when the widget content is updated.
   *
   * @privlevel public
   * @privilege %http://tizen.org/privilege/widget.viewer
   * @SINCE_3_0.0
   * @return The signal to connect to.
   */
  WidgetViewSignalType& WidgetContentUpdatedSignal();

  /**
   * @brief This signal is emitted when the widget update period is changed.
   *
   * @privlevel public
   * @privilege %http://tizen.org/privilege/widget.viewer
   * @SINCE_3_0.0
   * @return The signal to connect to.
   */
  WidgetViewSignalType& WidgetUpdatePeriodChangedSignal();

  /**
   * @brief This signal is emitted when the widget process is not running.
   *
   * If a frame was previously added, WidgetDeletedSignal() is emitted first.
   *
   * @privlevel public
   * @privilege %http://tizen.org/privilege/widget.viewer
   * @SINCE_3_0.0
   * @return The signal to connect to.
   */
  WidgetViewSignalType& WidgetFaultedSignal();

public: // Not intended for application developers
  /**
   * @internal
   * @brief Creates a handle using the Ui::Internal implementation.
   *
   * @param[in] implementation The WidgetView implementation.
   */
  DALI_INTERNAL WidgetView(Internal::WidgetViewImpl& implementation);

  /**
   * @internal
   * @brief Allows the creation of this control from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor.
   */
  DALI_INTERNAL WidgetView(Dali::Internal::CustomActor* internal);
};

/**
 * @}
 */
} // namespace Ui

} // namespace DALI_NAMESPACE
