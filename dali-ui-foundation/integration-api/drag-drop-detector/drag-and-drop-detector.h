#ifndef DALI_DRAG_AND_DROP_DETECTOR_H
#define DALI_DRAG_AND_DROP_DETECTOR_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{
namespace Ui
{
namespace Internal DALI_INTERNAL
{
class DragAndDropDetector;
}

namespace Integration
{
/**
 * @brief The DragAndDropDetector%s provides signals when draggable objects are dragged into other object.
 *
 * It provides signals for when the draggable object start drag, draggable object enters other object, moves around in
 * other object, leaves other object, dropped into other object and finally when the drag ended.
 *
 * The basic usage is shown below:
 *
 * @code
 *
 *  void Example()
 *  {
 *    DragAndDropDetector detector = DragAndDropDetector::New();
 *
 *    // Get notifications when the draggable item start drag
 *    detector.StartedSignal().Connect( &OnStarted );
 *
 *    // Get notifications when the draggable item enters other item
 *    detector.EnteredSignal().Connect( &OnEntered );
 *
 *    // Get notifications when the draggable item leaves other item
 *    detector.ExitedSignal().Connect( &OnExited );
 *
 *    // Get notifications when the draggable item is moved within other item
 *    detector.MovedSignal().Connect( &OnMoved );
 *
 *    // Get notifications when the draggable item is dropped
 *    detector.DroppedSignal().Connect( &OnDropped );
 *
 *    // Get notifications when the draggable object drag ended
 *    detector.EndedSignal().Connect( &OnEnded );
 *  }
 *
 *  void OnStarted( View view, DragAndDropDetector detector )
 *  {
 *    // Query the new values
 *    std::cout << "Position = " << detector.GetCurrentScreenPosition() << std::endl;
 *  }
 *
 *  void OnEntered( View view, DragAndDropDetector detector )
 *  {
 *    // Change mode as required
 *  }
 *
 *  void OnExited( View view, DragAndDropDetector detector )
 *  {
 *    // Change mode as required
 *  }
 *
 *  void OnMoved( View view, DragAndDropDetector detector )
 *  {
 *    // Query the new values
 *    std::cout << "Position = " << detector.GetCurrentScreenPosition() << std::endl;
 *  }
 *
 *  void OnDropped( View view, DragAndDropDetector detector )
 *  {
 *    // Query the new values
 *    std::cout << "Position = " << detector.GetCurrentScreenPosition() << ", Content = " << detector.GetContent() <<
 * std::endl;
 *  }
 *
 *  void OnEnded( View view, DragAndDropDetector detector )
 *  {
 *    // Change mode as required
 *  }
 *
 * @endcode
 */
class DALI_UI_API DragAndDropDetector : public BaseHandle
{
public:
  // Typedefs

  using DragAndDropSignal = Signal<void(Dali::Ui::View, DragAndDropDetector)>; ///< Drag & Drop signal

  /**
   * @brief Create an initialized DragAndDropDetector.
   *
   * @return A handle to a newly allocated Dali DragAndDropDetector
   */
  static DragAndDropDetector New();

  /**
   * @brief Create an uninitialized handle.
   *
   * This can be initialized with DragAndDropDetector::New().
   */
  DragAndDropDetector();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~DragAndDropDetector();

  /**
   * @brief Returns the dropped content.
   *
   * @return A reference to the string representing the dropped content.
   */
  const std::string& GetContent() const;

  /**
   * @brief Attaches a View to the detector.
   *
   * @note You can attach several views to a DragAndDropDetector.
   * DragAndDropDetector will keep a handle to the view and keep it alive as long as
   * DragAndDropDetector is deleted or Detach is called.
   */
  void Attach(Dali::Ui::View view);

  /**
   * @brief Detaches the attached view from the detector.
   *
   * @pre The specified view has been attached to the DragAndDropDetector.
   */
  void Detach(Dali::Ui::View view);

  /**
   * @brief Detaches all attached view from the detector.
   *
   * @pre At least one view has been attached to the DragAndDropDetector.
   */
  void DetachAll();

  /**
   * @brief Returns the number of views attached to the DragAndDropDetector.
   *
   * @pre The DragAndDropDetector has been initialized.
   */
  uint32_t GetAttachedViewCount() const;

  /**
   * @brief Returns a view by index. An empty handle if the index is not valid.
   *
   * @pre The DragAndDropDetector has been initialized.
   */
  Dali::Ui::View GetAttachedView(uint32_t index) const;

  /**
   * @brief Returns the current position of the dragged object.
   *
   * This is the dropped position when an object is dropped.
   * @return The current screen position.
   */
  const Vector2& GetCurrentScreenPosition() const;

  // Signals
  /**
   * @brief This is emitted when a dragged object start drag.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallback( View view, DragAndDropDetector detector );
   * @endcode
   * @return The signal to connect to.
   */
  DragAndDropSignal& StartedSignal();

  /**
   * @brief This is emitted when a dragged object enters other object.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallback( View view, DragAndDropDetector detector );
   * @endcode
   * @return The signal to connect to.
   */
  DragAndDropSignal& EnteredSignal();

  /**
   * @brief This is emitted when a dragged object leaves other object.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallback( View view, DragAndDropDetector detector );
   * @endcode
   * @return The signal to connect to.
   */
  DragAndDropSignal& ExitedSignal();

  /**
   * @brief This is emitted when a dragged object is moved within other object.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallback( View view, DragAndDropDetector detector );
   * @endcode
   * @return The signal to connect to.
   */
  DragAndDropSignal& MovedSignal();

  /**
   * @brief This is emitted when a dragged object is dropped within other object.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallback( View view, DragAndDropDetector detector );
   * @endcode
   * @return The signal to connect to.
   */
  DragAndDropSignal& DroppedSignal();

  /**
   * @brief This is emitted when a dragged object drag ended.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallback( View view, DragAndDropDetector detector );
   * @endcode
   * @return The signal to connect to.
   */
  DragAndDropSignal& EndedSignal();

public: // Not intended for application developers
  /**
   * @brief This constructor is used by DragAndDropDetector::Get().
   *
   * @param[in] detector A pointer to the drag and drop detector.
   */
  explicit DALI_INTERNAL DragAndDropDetector(Dali::Ui::Internal::DragAndDropDetector* detector);
};

} // namespace Integration
} // namespace Ui

} // namespace Dali

#endif // DALI_DRAG_AND_DROP_DETECTOR_H
