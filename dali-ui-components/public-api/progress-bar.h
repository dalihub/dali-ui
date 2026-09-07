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

#include <dali/public-api/math/vector2.h>

#include <cstdint>

#include <dali-ui-components/public-api/styles/progress-bar-style.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/progress-view.h>

namespace Dali
{
namespace Ui
{

/**
 * @brief Displays determinate or indeterminate progress in a horizontal bar.
 *
 * Value, range, step and determinate-state APIs are inherited from ProgressView. The bar
 * exposes local geometry snapshots so applications can position their own tooltip and
 * divider labels without accessing implementation children.
 */
class DALI_UI_COMPONENTS_API ProgressBar : public ProgressView
{
public:
  /**
   * @brief Signal type emitted when the trail end position changes.
   */
  using TrailEndPositionChangedSignalType = Signal<void(ProgressBar)>;

  /**
   * @brief Signal type emitted when the divider collection changes.
   */
  using DividersChangedSignalType = Signal<void(ProgressBar)>;

  /**
   * @brief Creates an uninitialized ProgressBar handle.
   */
  ProgressBar();

  /**
   * @brief Creates a ProgressBar with range 0.0 to 1.0 and the configured default style.
   *
   * @return An initialized ProgressBar
   */
  static ProgressBar New();

  /**
   * @brief Creates a ProgressBar with range 0.0 to 1.0 and an explicit style.
   *
   * @param[in] style The style used to initialize the ProgressBar
   * @return An initialized ProgressBar
   */
  static ProgressBar New(ProgressBarStyle style);

  /**
   * @brief Creates a ProgressBar with an explicit range and the configured default style.
   *
   * @param[in] minimumValue The finite minimum value
   * @param[in] maximumValue The finite maximum value, which must be greater than @p minimumValue
   * @return An initialized ProgressBar
   */
  static ProgressBar New(float minimumValue, float maximumValue);

  /**
   * @brief Creates a ProgressBar with an explicit range and style.
   *
   * @param[in] minimumValue The finite minimum value
   * @param[in] maximumValue The finite maximum value, which must be greater than @p minimumValue
   * @param[in] style The style used to initialize the ProgressBar
   * @return An initialized ProgressBar
   */
  static ProgressBar New(float minimumValue, float maximumValue, ProgressBarStyle style);

  /**
   * @brief Downcasts a handle to ProgressBar.
   *
   * @param[in] handle The handle to downcast
   * @return An initialized ProgressBar on success, otherwise an empty handle
   */
  static ProgressBar DownCast(BaseHandle handle);

  /**
   * @brief Creates a copy of another ProgressBar handle.
   *
   * @param[in] progressBar The handle to copy
   */
  ProgressBar(const ProgressBar& progressBar);

  /**
   * @brief Creates a ProgressBar handle by moving another handle.
   *
   * @param[in] rhs The handle to move
   */
  ProgressBar(ProgressBar&& rhs) noexcept;

  /**
   * @brief Destroys the ProgressBar handle.
   */
  ~ProgressBar();

  /**
   * @brief Assigns another ProgressBar handle to this handle.
   *
   * @param[in] handle The handle to copy
   * @return A reference to this handle
   */
  ProgressBar& operator=(const ProgressBar& handle) = default;

  /**
   * @brief Move-assigns another ProgressBar handle to this handle.
   *
   * @param[in] rhs The handle to move
   * @return A reference to this handle
   */
  ProgressBar& operator=(ProgressBar&& rhs) noexcept = default;

  DALI_UI_VIEW_WITH(ProgressBar)

  /**
   * @brief Sets the track thickness.
   *
   * @param[in] thickness The finite, positive track thickness
   */
  void SetTrackThickness(float thickness);

  /**
   * @brief Returns the track thickness.
   *
   * @return The current track thickness
   */
  float GetTrackThickness() const;

  /**
   * @brief Sets the track color.
   *
   * @param[in] color The track color
   */
  void SetTrackColor(const UiColor& color);

  /**
   * @brief Returns the track color.
   *
   * @return The current track color
   */
  UiColor GetTrackColor() const;

  /**
   * @brief Sets the trail color.
   *
   * @param[in] color The trail color used in determinate and indeterminate modes
   */
  void SetTrailColor(const UiColor& color);

  /**
   * @brief Returns the trail color.
   *
   * @return The current trail color
   */
  UiColor GetTrailColor() const;

  /**
   * @brief Sets the number of equal visual segments separated by dividers.
   *
   * Zero and one produce no dividers. A value N greater than one produces N - 1 dividers.
   * Negative values are rejected. This setting is independent of ValueStepCount.
   *
   * @param[in] count The non-negative number of equal visual segments
   */
  void SetDividerStepCount(int32_t count);

  /**
   * @brief Returns the number of equal visual segments.
   *
   * @return The divider step count
   */
  int32_t GetDividerStepCount() const;

  /**
   * @brief Sets whether determinate trail fill starts from the logical end.
   *
   * @param[in] reversed True to fill from the logical end, otherwise false
   */
  void SetReversed(bool reversed);

  /**
   * @brief Returns whether determinate trail fill starts from the logical end.
   *
   * @return True when reversed, otherwise false
   */
  bool IsReversed() const;

  /**
   * @brief Returns the sender-only signal for changes to GetTrailEndPosition().
   *
   * Snapshot state is committed before notification. Multiple changes in one event turn are
   * coalesced. Determinate transitions, scene connection, world movement and layouts that keep
   * the same local result do not emit this signal. Call GetTrailEndPosition() immediately after
   * connecting when an initial synchronization is required.
   *
   * @return The trail end position changed signal
   */
  TrailEndPositionChangedSignalType& TrailEndPositionChangedSignal();

  /**
   * @brief Returns the sender-only signal for changes to the complete divider collection.
   *
   * The signal is emitted when divider count or any indexed local position changes. Snapshot
   * state is committed first and event-turn changes are coalesced. The handler should read
   * GetDividerCount() and every GetDividerPosition() again to perform a full resynchronization.
   * Color, determinate state, scene connection and unchanged local geometry do not emit it.
   *
   * @return The dividers changed signal
   */
  DividersChangedSignalType& DividersChangedSignal();

  /**
   * @brief Returns the mathematical trail endpoint in ProgressBar-local visual coordinates.
   *
   * The origin is the ProgressBar's top-left. RTL and reversed state are reflected. The value
   * remains valid before first arrange, at zero size, while indeterminate and while off scene.
   *
   * @return The trail end position in ProgressBar-local visual coordinates
   */
  Vector2 GetTrailEndPosition() const;

  /**
   * @brief Returns the logical divider count, max(DividerStepCount - 1, 0).
   *
   * @return The number of dividers
   */
  uint32_t GetDividerCount() const;

  /**
   * @brief Returns one divider position in ProgressBar-local visual coordinates.
   *
   * Indices are ordered from logical start. Returned coordinates reflect RTL. The caller must
   * satisfy @p index < GetDividerCount(); an out-of-range index triggers a DALi assertion.
   *
   * @param[in] index The zero-based logical divider index
   * @return The divider position in ProgressBar-local visual coordinates
   */
  Vector2 GetDividerPosition(uint32_t index) const;

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL ProgressBar(Extension::ProgressViewImpl& implementation);
  explicit DALI_INTERNAL ProgressBar(Dali::Internal::CustomActor* internal);
  /// @endcond
};

} // namespace Ui
} // namespace Dali
