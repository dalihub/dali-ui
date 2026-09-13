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

#include <dali/public-api/animation/animation.h>
#include <dali/public-api/signals/callback.h>

#include <array>
#include <cstdint>
#include <vector>

#include <dali-ui-components/public-api/progress-bar.h>
#include <dali-ui-components/public-api/styles/progress-bar-style.h>
#include <dali-ui-foundation/extension-api/progress-view-impl.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{

class ProgressBarImpl : public Extension::ProgressViewImpl
{
public:
  static Ui::ProgressBar New(float minimumValue, float maximumValue, ProgressBarStyle style);

  void    SetTrackThickness(float thickness);
  float   GetTrackThickness() const;
  void    SetTrackColor(const UiColor& color);
  UiColor GetTrackColor() const;
  void    SetTrailColor(const UiColor& color);
  UiColor GetTrailColor() const;
  void    SetDividerStepCount(int32_t count);
  int32_t GetDividerStepCount() const;
  void    SetReversed(bool reversed);
  bool    IsReversed() const;

  ProgressBar::TrailEndPositionChangedSignalType& TrailEndPositionChangedSignal();
  ProgressBar::DividersChangedSignalType&         DividersChangedSignal();
  Vector2                                         GetTrailEndPosition() const;
  uint32_t                                        GetDividerCount() const;
  Vector2                                         GetDividerPosition(uint32_t index) const;

protected:
  void         OnInitialize() override;
  void         OnDestroy() override;
  void         OnSceneConnection(int depth) override;
  void         OnSceneDisconnection() override;
  MeasuredSize OnMeasure(float widthConstraint, float heightConstraint) override;
  LayoutRect   OnArrange(const LayoutRect& bounds) override;
  void         OnValueRatioChanged(float valueRatio) override;
  void         OnDeterminateChanged(bool determinate) override;

  ProgressBarImpl(float minimumValue, float maximumValue);
  ~ProgressBarImpl() override;

private:
  static MeasuredSize MeasureTrackSurface(View view, float widthConstraint, float heightConstraint);
  static LayoutRect   ArrangeTrackSurface(View view, const LayoutRect& bounds);

  void ApplyInitialStyle(ProgressBarStyle style);
  void EnsureDividerCount(uint32_t count);
  void UpdateLocalGeometry();
  void CommitTrailSnapshot(const Vector2& position);
  void CommitDividerSnapshot(std::vector<Vector2> positions);
  void QueueGeometryNotification();
  void EmitGeometryNotifications();
  void SetIndeterminateOffscreenRendering(bool enabled);
  void StartIndeterminateAnimation();
  void ClearIndeterminateAnimation();
  bool IsRightToLeft() const;

private:
  View                 mTrack;
  View                 mTrail;
  std::vector<View>    mDividers;
  std::array<View, 4u> mIndeterminateBars;

  UiColor mTrackColor;
  UiColor mTrailColor;
  UiColor mDividerColor;
  float   mTrackThickness{10.0f};
  int32_t mDividerStepCount{0};
  bool    mReversed{false};

  LayoutRect           mTrackBounds;
  float                mArrangedWidth{0.0f};
  Vector2              mTrailEndPosition{Vector2::ZERO};
  std::vector<Vector2> mDividerPositions;

  ProgressBar::TrailEndPositionChangedSignalType mTrailEndPositionChangedSignal;
  ProgressBar::DividersChangedSignalType         mDividersChangedSignal;
  CallbackBase*                                  mGeometryIdleCallback{nullptr};
  bool                                           mTrailGeometryChangedPending{false};
  bool                                           mDividersChangedPending{false};

  Animation mIndeterminateAnimation;
  bool      mOnScene{false};
};

} // namespace Internal

inline Internal::ProgressBarImpl& GetImpl(Ui::ProgressBar& progressBar)
{
  DALI_ASSERT_ALWAYS(progressBar);
  return static_cast<Internal::ProgressBarImpl&>(progressBar.GetImplementation());
}

inline const Internal::ProgressBarImpl& GetImpl(const Ui::ProgressBar& progressBar)
{
  DALI_ASSERT_ALWAYS(progressBar);
  return static_cast<const Internal::ProgressBarImpl&>(progressBar.GetImplementation());
}

} // namespace Ui
} //namespace DALI_NAMESPACE
