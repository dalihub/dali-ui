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
  using TrailEndPositionChangedSignalType = Signal<void(ProgressBar)>;
  using DividersChangedSignalType         = Signal<void(ProgressBar)>;

  ProgressBar();

  /**
   * @brief Creates a ProgressBar with range 0.0 to 1.0 and the configured default style.
   */
  static ProgressBar New();

  /**
   * @brief Creates a ProgressBar with range 0.0 to 1.0 and an explicit style.
   */
  static ProgressBar New(ProgressBarStyle style);

  /**
   * @brief Creates a ProgressBar with an explicit range and the configured default style.
   */
  static ProgressBar New(float minimumValue, float maximumValue);

  /**
   * @brief Creates a ProgressBar with an explicit range and style.
   */
  static ProgressBar New(float minimumValue, float maximumValue, ProgressBarStyle style);

  static ProgressBar DownCast(BaseHandle handle);

  ProgressBar(const ProgressBar& progressBar);
  ProgressBar(ProgressBar&& rhs) noexcept;
  ~ProgressBar();

  ProgressBar& operator=(const ProgressBar& handle)  = default;
  ProgressBar& operator=(ProgressBar&& rhs) noexcept = default;

  DALI_UI_VIEW_WITH(ProgressBar)

  void    SetTrackThickness(float thickness);
  float   GetTrackThickness() const;
  void    SetTrackColor(const UiColor& color);
  UiColor GetTrackColor() const;
  void    SetTrailColor(const UiColor& color);
  UiColor GetTrailColor() const;

  /**
   * @brief Sets the number of equal visual segments separated by dividers.
   *
   * Zero and one produce no dividers. A value N greater than one produces N - 1 dividers.
   * Negative values are rejected. This setting is independent of ValueStepCount.
   */
  void SetDividerStepCount(int32_t count);

  int32_t GetDividerStepCount() const;

  /**
   * @brief Sets whether determinate trail fill starts from the logical end.
   */
  void SetReversed(bool reversed);

  bool IsReversed() const;

  /**
   * @brief Returns the sender-only signal for changes to GetTrailEndPosition().
   *
   * Snapshot state is committed before notification. Multiple changes in one event turn are
   * coalesced. Determinate transitions, scene connection, world movement and layouts that keep
   * the same local result do not emit this signal. Call GetTrailEndPosition() immediately after
   * connecting when an initial synchronization is required.
   */
  TrailEndPositionChangedSignalType& TrailEndPositionChangedSignal();

  /**
   * @brief Returns the sender-only signal for changes to the complete divider collection.
   *
   * The signal is emitted when divider count or any indexed local position changes. Snapshot
   * state is committed first and event-turn changes are coalesced. The handler should read
   * GetDividerCount() and every GetDividerPosition() again to perform a full resynchronization.
   * Color, determinate state, scene connection and unchanged local geometry do not emit it.
   */
  DividersChangedSignalType& DividersChangedSignal();

  /**
   * @brief Returns the mathematical trail endpoint in ProgressBar-local visual coordinates.
   *
   * The origin is the ProgressBar's top-left. RTL and reversed state are reflected. The value
   * remains valid before first arrange, at zero size, while indeterminate and while off scene.
   */
  Vector2 GetTrailEndPosition() const;

  /**
   * @brief Returns the logical divider count, max(DividerStepCount - 1, 0).
   */
  uint32_t GetDividerCount() const;

  /**
   * @brief Returns one divider position in ProgressBar-local visual coordinates.
   *
   * Indices are ordered from logical start. Returned coordinates reflect RTL. The caller must
   * satisfy @p index < GetDividerCount(); an out-of-range index triggers a DALi assertion.
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
