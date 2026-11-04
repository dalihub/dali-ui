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
#include <dali/public-api/math/rect.h>
#include <dali/public-api/object/any.h>
// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/views/view.h>
namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Integration DALI_INTERNAL
{
class MediaViewImpl;
}
/**
 * @brief Displays a borrowed native frame source without controlling its producer.
 * @note Use on the DALi event thread. One MediaView per source. Gapless is unsupported.
 * @addtogroup dali_ui_view
 * @{
 */
class DALI_UI_API MediaView : public View
{
public:
  /**
   * @brief Constructs an empty handle.
   */
  MediaView();

  /**
   * @brief Creates a view without a source.
   * @return An initialized view
   */
  static MediaView New();

  /**
   * @brief Creates and connects a view.
   * @param[in] nativeSource A borrowed native frame source wrapped in Any
   * @return An initialized view, or an empty handle if connection fails
   */
  static MediaView New(Dali::Any nativeSource);

  /**
   * @brief Downcasts a compatible handle.
   * @param[in] handle A handle to an object
   * @return A MediaView handle, or an empty handle for an incompatible object
   */
  static MediaView DownCast(BaseHandle handle);

  /**
   * @brief Releases this handle.
   */
  ~MediaView();

  /**
   * @brief Copies a handle, sharing its view.
   */
  MediaView(const MediaView&);

  /**
   * @brief Moves a handle.
   */
  MediaView(MediaView&&) noexcept;

  /**
   * @brief Copies a handle.
   */
  MediaView& operator=(const MediaView&);

  /**
   * @brief Moves a handle.
   */
  MediaView& operator=(MediaView&&) noexcept;

  /**
   * @brief Connects an opaque native frame source; failure leaves this view disconnected.
   * @param[in] nativeSource A borrowed native frame source wrapped in Any
   * @return True if connected; false if unsupported, unavailable, or already used
   * @note The producer owns the source. ClearSource() before destroying the producer.
   */
  bool SetSource(Dali::Any nativeSource);

  /**
   * @brief Returns the connected native source, or an empty Any.
   * @return The borrowed native source, or an empty Any
   */
  Dali::Any GetSource() const;

  /**
   * @brief Disconnects and returns all retained producer packets.
   */
  void ClearSource();

  /**
   * @brief Sets a normalized crop in unrotated source coordinates; invalid values leave it unchanged.
   * @param[in] crop A finite rectangle within [0,1], with positive width and height
   * @return True for a valid crop; false if the previous crop is kept
   */
  bool SetSourceCrop(Dali::Rect<float> crop);

  /**
   * @brief Returns the source crop; the default is the full frame.
   * @return The crop rectangle; initially (0,0,1,1)
   */
  Dali::Rect<float> GetSourceCrop() const;

  /**
   * @brief Sets horizontal reflection in the rotated display coordinate system.
   * @param[in] enabled True to reflect the image; initially false
   */
  void SetHorizontalFlipEnabled(bool enabled);

  /**
   * @brief Returns whether horizontal reflection is enabled.
   * @return True if reflection is enabled
   */
  bool IsHorizontalFlipEnabled() const;

  /**
   * @brief Sets vertical reflection in the rotated display coordinate system.
   * @param[in] enabled True to reflect the image; initially false
   */
  void SetVerticalFlipEnabled(bool enabled);

  /**
   * @brief Returns whether vertical reflection is enabled.
   * @return True if reflection is enabled
   */
  bool IsVerticalFlipEnabled() const;

  /**
   * @brief Internal handle constructor.
   */
  explicit DALI_INTERNAL MediaView(Integration::MediaViewImpl& implementation);

  /**
   * @brief Internal actor constructor used by DownCast.
   */
  explicit DALI_INTERNAL MediaView(Dali::Internal::CustomActor* actor);
};
/** @} */
} // namespace Ui
} // namespace DALI_NAMESPACE
