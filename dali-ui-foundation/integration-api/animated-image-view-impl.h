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
#include <dali-ui-foundation/public-api/animated-image-view-properties.h>
#include <dali-ui-foundation/public-api/image/animated-image-enumerations.h>
#include <dali-ui-foundation/public-api/image/image-enumerations.h>
#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali-ui-foundation/public-api/view-impl.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

class AnimatedImageViewImpl;
using AnimatedImageViewImplPtr = IntrusivePtr<AnimatedImageViewImpl>;

/**
 * @brief This is the internal implementation class for AnimatedImageView.
 *
 * @see Dali::Ui::AnimatedImageView
 */
class DALI_UI_API AnimatedImageViewImpl : public ViewImpl
{
public: // Properties
  /**
   * @brief Property indices — aliased from the shared AnimatedImageViewPropertyIndex.
   *
   * Keeping a local Property struct means existing impl.cpp code (e.g.
   * AnimatedImageViewImpl::Property::IMAGE) continues to compile unchanged,
   * while the public header (animated-image-view.h) exposes the same values
   * through AnimatedImageView::Property.
   */
  struct Property
  {
    enum
    {
      IMAGE                     = Ui::AnimatedImageViewPropertyIndex::IMAGE,
      IMAGE_URLS                = Ui::AnimatedImageViewPropertyIndex::IMAGE_URLS,
      LOOP_COUNT                = Ui::AnimatedImageViewPropertyIndex::LOOP_COUNT,
      STOP_BEHAVIOR             = Ui::AnimatedImageViewPropertyIndex::STOP_BEHAVIOR,
      FRAME_SPEED_FACTOR        = Ui::AnimatedImageViewPropertyIndex::FRAME_SPEED_FACTOR,
      BATCH_SIZE                = Ui::AnimatedImageViewPropertyIndex::BATCH_SIZE,
      CACHE_SIZE                = Ui::AnimatedImageViewPropertyIndex::CACHE_SIZE,
      FRAME_DELAY               = Ui::AnimatedImageViewPropertyIndex::FRAME_DELAY,
      IMAGE_COLOR               = Ui::AnimatedImageViewPropertyIndex::IMAGE_COLOR,
      PRE_MULTIPLIED_ALPHA      = Ui::AnimatedImageViewPropertyIndex::PRE_MULTIPLIED_ALPHA,
      FITTING_MODE              = Ui::AnimatedImageViewPropertyIndex::FITTING_MODE,
      SAMPLING_MODE             = Ui::AnimatedImageViewPropertyIndex::SAMPLING_MODE,
      DESIRED_WIDTH             = Ui::AnimatedImageViewPropertyIndex::DESIRED_WIDTH,
      DESIRED_HEIGHT            = Ui::AnimatedImageViewPropertyIndex::DESIRED_HEIGHT,
      LOAD_POLICY               = Ui::AnimatedImageViewPropertyIndex::LOAD_POLICY,
      RELEASE_POLICY            = Ui::AnimatedImageViewPropertyIndex::RELEASE_POLICY,
      SYNCHRONOUS_LOADING       = Ui::AnimatedImageViewPropertyIndex::SYNCHRONOUS_LOADING,
      IMAGE_LOAD_WITH_VIEW_SIZE = Ui::AnimatedImageViewPropertyIndex::IMAGE_LOAD_WITH_VIEW_SIZE,
      ALPHA_MASK_URL            = Ui::AnimatedImageViewPropertyIndex::ALPHA_MASK_URL,
      CROP_TO_MASK              = Ui::AnimatedImageViewPropertyIndex::CROP_TO_MASK,
      MASKING_MODE              = Ui::AnimatedImageViewPropertyIndex::MASKING_MODE,
      PLACEHOLDER_IMAGE         = Ui::AnimatedImageViewPropertyIndex::PLACEHOLDER_IMAGE,
      PIXEL_AREA                = Ui::AnimatedImageViewPropertyIndex::PIXEL_AREA,
    };
  };

protected: // Construction & Destruction
  /**
   * @brief AnimatedImageViewImpl constructor.
   */
  AnimatedImageViewImpl();

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~AnimatedImageViewImpl();

public: // Creation
  /**
   * @brief Creates a new AnimatedImageViewImpl.
   *
   * @return An intrusive pointer to the newly allocated AnimatedImageViewImpl
   */
  static AnimatedImageViewImplPtr New();

public: // Properties (required by DALI_PROPERTY_REGISTRATION)
  /// @cond internal
  static void                  SetProperty(Dali::BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value);
  static Dali::Property::Value GetProperty(Dali::BaseObject* object, Dali::Property::Index index);
  /// @endcond

public: // API
  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetResourceUrl
   */
  void SetResourceUrl(const Dali::String& url);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetResourceUrl
   */
  Dali::String GetResourceUrl() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetResourceUrls
   */
  void SetResourceUrls(const Dali::Vector<Dali::String>& urls);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetResourceUrls
   */
  const Dali::Vector<Dali::String>& GetResourceUrls() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::Play
   */
  void Play();

  /**
   * @copydoc Dali::Ui::AnimatedImageView::Pause
   */
  void Pause();

  /**
   * @copydoc Dali::Ui::AnimatedImageView::Stop
   */
  void Stop();

  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetLoopCount
   */
  void SetLoopCount(int count);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetLoopCount
   */
  int GetLoopCount() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetImageColor
   */
  void SetImageColor(const UiColor& color);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetImageColor
   */
  UiColor GetImageColor() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetFittingMode
   */
  void SetFittingMode(Ui::Image::FittingMode fittingMode);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetFittingMode
   */
  Ui::Image::FittingMode GetFittingMode() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetSamplingMode
   */
  void SetSamplingMode(Ui::Image::SamplingMode samplingMode);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetSamplingMode
   */
  Ui::Image::SamplingMode GetSamplingMode() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetImageLoadWithViewSize
   */
  void SetImageLoadWithViewSize(bool enabled);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::IsImageLoadWithViewSizeEnabled
   */
  bool IsImageLoadWithViewSizeEnabled() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetAlphaMaskUrl
   */
  void SetAlphaMaskUrl(const Dali::String& maskUrl);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetAlphaMaskUrl
   */
  Dali::String GetAlphaMaskUrl() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetCropToMask
   */
  void SetCropToMask(bool cropToMask);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::IsCropToMask
   */
  bool IsCropToMask() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetMaskingMode
   */
  void SetMaskingMode(Ui::Image::MaskingType maskingMode);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetMaskingMode
   */
  Ui::Image::MaskingType GetMaskingMode() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetPlaceholderUrl
   */
  void SetPlaceholderUrl(const Dali::String& url);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetPlaceholderUrl
   */
  Dali::String GetPlaceholderUrl() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::JumpToFrame
   */
  void JumpToFrame(int frame);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetStopBehavior
   */
  void SetStopBehavior(Ui::AnimatedImage::StopBehavior behavior);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetStopBehavior
   */
  Ui::AnimatedImage::StopBehavior GetStopBehavior() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetFrameSpeedFactor
   */
  void SetFrameSpeedFactor(float factor);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetFrameSpeedFactor
   */
  float GetFrameSpeedFactor() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetBatchSize
   */
  void SetBatchSize(int size);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetBatchSize
   */
  int GetBatchSize() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetCacheSize
   */
  void SetCacheSize(int size);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetCacheSize
   */
  int GetCacheSize() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetFrameDelay
   */
  void SetFrameDelay(int milliseconds);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetFrameDelay
   */
  int GetFrameDelay() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetPixelArea
   */
  void SetPixelArea(const Vector4& pixelArea);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetPixelArea
   */
  Vector4 GetPixelArea() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetPlayState
   */
  Ui::AnimatedImage::PlayState GetPlayState() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetCurrentFrame
   */
  int GetCurrentFrame() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetTotalFrame
   */
  int GetTotalFrame() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetDesiredWidth
   */
  void SetDesiredWidth(int width);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetDesiredWidth
   */
  int GetDesiredWidth() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetDesiredHeight
   */
  void SetDesiredHeight(int height);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetDesiredHeight
   */
  int GetDesiredHeight() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetLoadPolicy
   */
  void SetLoadPolicy(Ui::Image::LoadPolicy loadPolicy);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetLoadPolicy
   */
  Ui::Image::LoadPolicy GetLoadPolicy() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetReleasePolicy
   */
  void SetReleasePolicy(Ui::Image::ReleasePolicy releasePolicy);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetReleasePolicy
   */
  Ui::Image::ReleasePolicy GetReleasePolicy() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetSynchronousLoading
   */
  void SetSynchronousLoading(bool synchronous);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::IsSynchronousLoading
   */
  bool IsSynchronousLoading() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetPreMultipliedAlpha
   */
  void SetPreMultipliedAlpha(bool preMultiplied);

  /**
   * @copydoc Dali::Ui::AnimatedImageView::IsPreMultipliedAlpha
   */
  bool IsPreMultipliedAlpha() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::SetDepthIndex
   */
  void SetDepthIndex(int depthIndex);

public: // Loading Status & Signals
  /**
   * @copydoc Dali::Ui::AnimatedImageView::GetLoadingStatus
   */
  Ui::Visual::ResourceStatus GetLoadingStatus() const;

  /**
   * @copydoc Dali::Ui::AnimatedImageView::AnimationFinishedSignal
   */
  Dali::Signal<void(Dali::Ui::View)>& AnimationFinishedSignal();

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
   * @brief Rebuilds and re-registers the image visual from current property values.
   */
  void UpdateVisual();

  /**
   * @brief Rebuilds and re-registers the placeholder visual.
   */
  void UpdatePlaceholderVisual();

  /**
   * @brief Called when a visual event signal is emitted.
   *
   * Used to intercept ANIMATION_FINISHED and forward it as AnimationFinishedSignal.
   */
  void OnVisualEvent(View view, Dali::Property::Index visualIndex, Dali::Property::Index signalId);

  /**
   * @brief Called when the view's resource becomes ready.
   *
   * Used to remove placeholder when main image is loaded.
   */
  void OnViewResourceReady(Ui::View view);

private:
  // Not copyable or movable
  AnimatedImageViewImpl(const AnimatedImageViewImpl&)            = delete;
  AnimatedImageViewImpl(AnimatedImageViewImpl&&)                 = delete;
  AnimatedImageViewImpl& operator=(const AnimatedImageViewImpl&) = delete;
  AnimatedImageViewImpl& operator=(AnimatedImageViewImpl&&)      = delete;

private:                    // Data
  Ui::Visual::Base mVisual; ///< The main image visual

  Dali::String               mUrl;
  Dali::String               mAlphaMaskUrl;
  Dali::String               mPlaceholderUrl;
  Dali::Vector<Dali::String> mUrls;

  Vector4 mPixelArea;

  UiColor mImageColor;

  Ui::AnimatedImage::StopBehavior mStopBehavior;
  Ui::Image::FittingMode          mFittingMode;
  Ui::Image::SamplingMode         mSamplingMode;
  Ui::Image::MaskingType          mMaskingMode;
  Ui::Image::LoadPolicy           mLoadPolicy;
  Ui::Image::ReleasePolicy        mReleasePolicy;
  int                             mLoopCount;
  int                             mBatchSize;
  int                             mCacheSize;
  int                             mFrameDelay;
  int                             mDesiredWidth;
  int                             mDesiredHeight;
  int                             mDepthIndex;
  float                           mFrameSpeedFactor;

  bool mPreMultipliedAlpha;
  bool mImageLoadWithViewSize;
  bool mCropToMask;
  bool mSynchronousLoading;
  bool mVisualDirty; ///< True when a property changed and the visual needs rebuilding on the next measure pass

  Dali::Signal<void(Dali::Ui::View)> mAnimationFinishedSignal;
};

} // namespace Integration
} // namespace Ui
} // namespace Dali
