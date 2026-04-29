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

#include <dali-ui-foundation/devel-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/devel-api/visuals/animated-vector-image-visual-actions-devel.h>
#include <dali-ui-foundation/public-api/image/animated-image-enumerations.h>
#include <dali-ui-foundation/public-api/image/image-enumerations.h>
#include <dali-ui-foundation/public-api/image/lottie-animation-enumerations.h>
#include <dali-ui-foundation/public-api/image/lottie-animation-types.h>
#include <dali-ui-foundation/public-api/lottie-animation-view-properties.h>
#include <dali-ui-foundation/public-api/view-impl.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

class LottieAnimationViewImpl;
using LottieAnimationViewImplPtr = IntrusivePtr<LottieAnimationViewImpl>;

/**
 * @brief This is the internal implementation class for LottieAnimationView.
 *
 * @see Dali::Ui::LottieAnimationView
 */
class DALI_UI_API LottieAnimationViewImpl : public ViewImpl
{
public: // Properties
  /**
   * @brief Property indices — aliased from the shared LottieAnimationViewPropertyIndex.
   */
  struct Property
  {
    enum
    {
      IMAGE                      = Ui::LottieAnimationViewPropertyIndex::IMAGE,
      LOOP_COUNT                 = Ui::LottieAnimationViewPropertyIndex::LOOP_COUNT,
      LOOPING_MODE               = Ui::LottieAnimationViewPropertyIndex::LOOPING_MODE,
      STOP_BEHAVIOR              = Ui::LottieAnimationViewPropertyIndex::STOP_BEHAVIOR,
      FRAME_SPEED_FACTOR         = Ui::LottieAnimationViewPropertyIndex::FRAME_SPEED_FACTOR,
      IMAGE_COLOR                = Ui::LottieAnimationViewPropertyIndex::IMAGE_COLOR,
      DESIRED_WIDTH              = Ui::LottieAnimationViewPropertyIndex::DESIRED_WIDTH,
      DESIRED_HEIGHT             = Ui::LottieAnimationViewPropertyIndex::DESIRED_HEIGHT,
      RELEASE_POLICY             = Ui::LottieAnimationViewPropertyIndex::RELEASE_POLICY,
      SYNCHRONOUS_LOADING        = Ui::LottieAnimationViewPropertyIndex::SYNCHRONOUS_LOADING,
      PRE_MULTIPLIED_ALPHA       = Ui::LottieAnimationViewPropertyIndex::PRE_MULTIPLIED_ALPHA,
      REDRAW_IN_SCALING_DOWN     = Ui::LottieAnimationViewPropertyIndex::REDRAW_IN_SCALING_DOWN,
      REDRAW_IN_SCALING_UP       = Ui::LottieAnimationViewPropertyIndex::REDRAW_IN_SCALING_UP,
      ENABLE_FRAME_CACHE         = Ui::LottieAnimationViewPropertyIndex::ENABLE_FRAME_CACHE,
      NOTIFY_AFTER_RASTERIZATION = Ui::LottieAnimationViewPropertyIndex::NOTIFY_AFTER_RASTERIZATION,
      RENDER_SCALE               = Ui::LottieAnimationViewPropertyIndex::RENDER_SCALE,
      PLACEHOLDER_IMAGE          = Ui::LottieAnimationViewPropertyIndex::PLACEHOLDER_IMAGE,
    };
  };

  /**
   * @brief Internal enum for how the play range was set.
   */
  enum class PlayRangeType
  {
    NONE,  ///< No play range override
    FRAME, ///< Range set via SetMinMaxFrame
    MARKER ///< Range set via SetMinMaxFrameByMarker
  };

protected: // Construction & Destruction
  /**
   * @brief LottieAnimationViewImpl constructor.
   */
  LottieAnimationViewImpl();

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~LottieAnimationViewImpl();

public: // Creation
  /**
   * @brief Creates a new LottieAnimationViewImpl.
   *
   * @return An intrusive pointer to the newly allocated LottieAnimationViewImpl
   */
  static LottieAnimationViewImplPtr New();

public: // Properties (required by DALI_PROPERTY_REGISTRATION)
  /// @cond internal
  static void                  SetProperty(Dali::BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value);
  static Dali::Property::Value GetProperty(Dali::BaseObject* object, Dali::Property::Index index);
  /// @endcond

public: // API
  /**
   * @copydoc Dali::Ui::LottieAnimationView::SetResourceUrl
   */
  void SetResourceUrl(const Dali::String& url);

  /**
   * @copydoc Dali::Ui::LottieAnimationView::GetResourceUrl
   */
  Dali::String GetResourceUrl() const;

  /**
   * @copydoc Dali::Ui::LottieAnimationView::Play
   */
  void Play();

  /**
   * @copydoc Dali::Ui::LottieAnimationView::Pause
   */
  void Pause();

  /**
   * @copydoc Dali::Ui::LottieAnimationView::Stop
   */
  void Stop();

  /**
   * @copydoc Dali::Ui::LottieAnimationView::SetLoopCount
   */
  void SetLoopCount(int count);

  /**
   * @copydoc Dali::Ui::LottieAnimationView::GetLoopCount
   */
  int GetLoopCount() const;

  /**
   * @copydoc Dali::Ui::LottieAnimationView::JumpToFrame
   */
  void JumpToFrame(int frame);

  /**
   * @copydoc Dali::Ui::LottieAnimationView::SetMinMaxFrame
   */
  void SetMinMaxFrame(int minFrame, int maxFrame);

  /**
   * @copydoc Dali::Ui::LottieAnimationView::SetMinMaxFrameByMarker
   */
  void SetMinMaxFrameByMarker(const Dali::String& minMarker, const Dali::String& maxMarker);

  /**
   * @copydoc Dali::Ui::LottieAnimationView::SetStopBehavior
   */
  void SetStopBehavior(Ui::AnimatedImage::StopBehavior behavior);

  /**
   * @copydoc Dali::Ui::LottieAnimationView::GetStopBehavior
   */
  Ui::AnimatedImage::StopBehavior GetStopBehavior() const;

  /**
   * @copydoc Dali::Ui::LottieAnimationView::SetLoopingMode
   */
  void SetLoopingMode(Ui::LottieAnimation::LoopingMode mode);

  /**
   * @copydoc Dali::Ui::LottieAnimationView::GetLoopingMode
   */
  Ui::LottieAnimation::LoopingMode GetLoopingMode() const;

  /**
   * @copydoc Dali::Ui::LottieAnimationView::SetFrameSpeedFactor
   */
  void SetFrameSpeedFactor(float factor);

  /**
   * @copydoc Dali::Ui::LottieAnimationView::GetFrameSpeedFactor
   */
  float GetFrameSpeedFactor() const;

  /**
   * @copydoc Dali::Ui::LottieAnimationView::GetPlayState
   */
  Ui::AnimatedImage::PlayState GetPlayState() const;

  /**
   * @copydoc Dali::Ui::LottieAnimationView::GetCurrentFrame
   */
  int GetCurrentFrame() const;

  /**
   * @copydoc Dali::Ui::LottieAnimationView::GetTotalFrame
   */
  int GetTotalFrame() const;

  /**
   * @copydoc Dali::Ui::LottieAnimationView::SetRedrawOnScaleDown
   */
  void SetRedrawOnScaleDown(bool redraw);

  /**
   * @copydoc Dali::Ui::LottieAnimationView::IsRedrawOnScaleDown
   */
  bool IsRedrawOnScaleDown() const;

  /**
   * @copydoc Dali::Ui::LottieAnimationView::SetRedrawOnScaleUp
   */
  void SetRedrawOnScaleUp(bool redraw);

  /**
   * @copydoc Dali::Ui::LottieAnimationView::IsRedrawOnScaleUp
   */
  bool IsRedrawOnScaleUp() const;

  /**
   * @copydoc Dali::Ui::LottieAnimationView::SetEnableFrameCache
   */
  void SetEnableFrameCache(bool enable);

  /**
   * @copydoc Dali::Ui::LottieAnimationView::IsFrameCacheEnabled
   */
  bool IsFrameCacheEnabled() const;

  /**
   * @copydoc Dali::Ui::LottieAnimationView::SetNotifyAfterRasterization
   */
  void SetNotifyAfterRasterization(bool notify);

  /**
   * @copydoc Dali::Ui::LottieAnimationView::IsNotifyAfterRasterizationEnabled
   */
  bool IsNotifyAfterRasterizationEnabled() const;

  /**
   * @copydoc Dali::Ui::LottieAnimationView::SetRenderScale
   */
  void SetRenderScale(float scale);

  /**
   * @copydoc Dali::Ui::LottieAnimationView::GetRenderScale
   */
  float GetRenderScale() const;

  /**
   * @copydoc Dali::Ui::LottieAnimationView::GetContentInfo
   */
  Dali::Property::Map GetContentInfo();

  /**
   * @copydoc Dali::Ui::LottieAnimationView::GetMarkerInfo
   */
  Dali::Property::Map GetMarkerInfo();

  /**
   * @copydoc Dali::Ui::LottieAnimationView::SetDynamicProperty
   */
  void SetDynamicProperty(const Ui::LottieAnimation::DynamicPropertyInfo& info);

  /**
   * @copydoc Dali::Ui::LottieAnimationView::SetDesiredWidth
   */
  void SetDesiredWidth(int width);

  /**
   * @copydoc Dali::Ui::LottieAnimationView::GetDesiredWidth
   */
  int GetDesiredWidth() const;

  /**
   * @copydoc Dali::Ui::LottieAnimationView::SetDesiredHeight
   */
  void SetDesiredHeight(int height);

  /**
   * @copydoc Dali::Ui::LottieAnimationView::GetDesiredHeight
   */
  int GetDesiredHeight() const;

  /**
   * @copydoc Dali::Ui::LottieAnimationView::SetReleasePolicy
   */
  void SetReleasePolicy(Ui::Image::ReleasePolicy releasePolicy);

  /**
   * @copydoc Dali::Ui::LottieAnimationView::GetReleasePolicy
   */
  Ui::Image::ReleasePolicy GetReleasePolicy() const;

  /**
   * @copydoc Dali::Ui::LottieAnimationView::SetSynchronousLoading
   */
  void SetSynchronousLoading(bool synchronous);

  /**
   * @copydoc Dali::Ui::LottieAnimationView::IsSynchronousLoading
   */
  bool IsSynchronousLoading() const;

  /**
   * @copydoc Dali::Ui::LottieAnimationView::SetPreMultipliedAlpha
   */
  void SetPreMultipliedAlpha(bool preMultiplied);

  /**
   * @copydoc Dali::Ui::LottieAnimationView::IsPreMultipliedAlpha
   */
  bool IsPreMultipliedAlpha() const;

  /**
   * @copydoc Dali::Ui::LottieAnimationView::SetImageColor
   */
  void SetImageColor(const UiColor& color);

  /**
   * @copydoc Dali::Ui::LottieAnimationView::GetImageColor
   */
  UiColor GetImageColor() const;

  /**
   * @copydoc Dali::Ui::LottieAnimationView::SetDepthIndex
   */
  void SetDepthIndex(int depthIndex);

  /**
   * @copydoc Dali::Ui::LottieAnimationView::SetPlaceholderUrl
   */
  void SetPlaceholderUrl(const Dali::String& url);

  /**
   * @copydoc Dali::Ui::LottieAnimationView::GetPlaceholderUrl
   */
  Dali::String GetPlaceholderUrl() const;

  /**
   * @copydoc Dali::Ui::LottieAnimationView::AnimationFinishedSignal
   */
  Dali::Signal<void(Dali::Ui::View)>& AnimationFinishedSignal();

  /**
   * @copydoc Dali::Ui::LottieAnimationView::GetLoadingStatus
   */
  Ui::Visual::ResourceStatus GetLoadingStatus() const;

private: // From ViewImpl
  /**
   * @copydoc ViewImpl::OnInitialize
   */
  void OnInitialize() override;

  /**
   * @copydoc ViewImpl::OnMeasure
   */
  MeasuredSize OnMeasure(float widthConstraint, float heightConstraint) override;

  /**
   * @copydoc ViewImpl::OnArrange
   */
  MeasuredSize OnArrange(const LayoutRect& bounds) override;

private: // Internal methods
  /**
   * @brief Rebuilds and re-registers the Lottie visual from current property values.
   */
  void UpdateVisual();

  /**
   * @brief Applies padding-aware transform to the visual for the given size.
   *
   * When padding is set, the visual is confined to the inner content area.
   * This is the Lottie equivalent of ImageViewImpl::ApplyFittingMode, but
   * without FittingMode support (always FILL within the padded area).
   *
   * @param[in] size The allocated size of the view
   */
  void ApplyLayout(const Vector2& size);

  /**
   * @brief Called when a visual event signal is emitted.
   */
  void OnVisualEvent(View view, Dali::Property::Index visualIndex, Dali::Property::Index signalId);

  /**
   * @brief Rebuilds and re-registers the placeholder visual.
   */
  void UpdatePlaceholderVisual();

  /**
   * @brief Called when the view's ResourceReady signal is emitted.
   */
  void OnViewResourceReady(Ui::View view);

private:
  // Not copyable or movable
  LottieAnimationViewImpl(const LottieAnimationViewImpl&)            = delete;
  LottieAnimationViewImpl(LottieAnimationViewImpl&&)                 = delete;
  LottieAnimationViewImpl& operator=(const LottieAnimationViewImpl&) = delete;
  LottieAnimationViewImpl& operator=(LottieAnimationViewImpl&&)      = delete;

private:                    // Data
  Ui::Visual::Base mVisual; ///< The registered Lottie visual; null when no URL is set

  Dali::String mUrl;
  Dali::String mMinFrameMarker;
  Dali::String mMaxFrameMarker;
  Dali::String mPlaceholderUrl;

  UiColor mImageColor;

  Ui::AnimatedImage::StopBehavior  mStopBehavior;
  Ui::LottieAnimation::LoopingMode mLoopingMode;
  PlayRangeType                    mPlayRangeType;
  Ui::Image::ReleasePolicy         mReleasePolicy;
  int                              mLoopCount;
  int                              mMinFrame;
  int                              mMaxFrame;
  int                              mDesiredWidth;
  int                              mDesiredHeight;
  int                              mDepthIndex;
  float                            mFrameSpeedFactor;
  float                            mRenderScale;

  bool mRedrawInScalingDown;
  bool mRedrawInScalingUp;
  bool mEnableFrameCache;
  bool mNotifyAfterRasterization;
  bool mSynchronousLoading;
  bool mPreMultipliedAlpha;
  bool mVisualDirty; ///< True when a property changed and the visual needs rebuilding on the next measure pass

  Dali::Signal<void(Dali::Ui::View)> mAnimationFinishedSignal;
};

} // namespace Integration
} // namespace Ui
} // namespace Dali
