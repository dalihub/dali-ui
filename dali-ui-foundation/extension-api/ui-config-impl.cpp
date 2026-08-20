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

// CLASS HEADER
#include <dali-ui-foundation/extension-api/ui-config-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/window-system-devel.h>
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/input-options.h>
#include <dali/public-api/common/dali-common.h>
#include <array>
#include <optional>

// INTERNAL INCLUDES
namespace
{

bool DefaultExecutionKeyPredicate(const Dali::String& keyName)
{
  return keyName == "Return";
}

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_UI_CONFIG");
#endif

} // unnamed namespace

namespace Dali
{

namespace Ui
{

namespace Extension
{

class UiConfigImpl::Impl
{
public:
  Impl()
  : mDefaultStateEffectForInteractive(StateEffect::None()),
    mExecutionKeyPredicate(DefaultExecutionKeyPredicate),
    mViewInitializer(UiConfig::DefaultViewInitializer),
    mDefaultTextColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f)),
    mDefaultPlaceholderTextColor(Vector4(0.8f, 0.8f, 0.8f, 0.8f)),
    mScalingFactor(1.0f),
    mDefaultFontSize(16.0f),
    mDefaultSystemFontSizeScaleEnabled(true),
    mSystemFontSizeScales{{0.87f, 1.0f, 1.13f, 1.26f, 1.4f}}, // SMALL, NORMAL, LARGE, EXTRA_LARGE, GIANT
    mDefaultMinimumFontSizeScale(0.01f),
    mDefaultMaximumFontSizeScale(10.0f),
    mDpi(160),
    mBaselineDpi(160),
    mMarqueeSpeed(80),
    mMarqueeLoopCount(2),
    mMarqueeLoopDelay(1.0f),
    mMarqueeGap(50.0f),
    mMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE),
    mMarqueeOrientation(Text::MarqueeOrientation::HORIZONTAL),
    mTextLayoutDirectionMode(Text::LayoutDirectionMode::INHERIT),
    mKeyClickPolicy(KeyClickPolicy::ON_RELEASE),
    mLongPressKeyEventMinimumCount(3),
    mAmbiguousPressDelay(100u),
    mAmbiguousPressDuration(64u),
    mWebEngineType(WebEngineType::CHROMIUM),
    mFocusIndicationPolicy(&FocusIndicationPolicy::Default),
    mClearFocusOnEscapeEnabled(true),
    mClearFocusIndicationOnTouch(true),
    mClearFocusIndicationOnHover(false),
    mDefaultFocusIndicatorEnabled(true),
    mShowPlaceholderTextOnFocus(true),
    mLabelAsyncRendering(false),
    mFrozen(false)
  {
  }

public:
  Dali::String mBrokenImageUrls[3]{}; ///< Broken image URLs for SMALL, NORMAL, LARGE

  StateEffect                     mDefaultStateEffectForInteractive; ///< Default effect for interactive views
  mutable UiStyleSheet            mStyleSheet;
  ExecutionKeyPredicate           mExecutionKeyPredicate;
  ViewInitializer                 mViewInitializer;
  Vector4                         mDefaultTextColor;
  Vector4                         mDefaultPlaceholderTextColor;
  float                           mScalingFactor;
  float                           mDefaultFontSize;
  bool                            mDefaultSystemFontSizeScaleEnabled;
  std::array<float, 5>            mSystemFontSizeScales;
  float                           mDefaultMinimumFontSizeScale;
  float                           mDefaultMaximumFontSizeScale;
  int                             mDpi;
  int                             mBaselineDpi;
  int                             mMarqueeSpeed;
  int                             mMarqueeLoopCount;
  float                           mMarqueeLoopDelay;
  float                           mMarqueeGap;
  Text::MarqueeStopMode           mMarqueeStopMode;
  Text::MarqueeOrientation        mMarqueeOrientation;
  Text::LayoutDirectionMode       mTextLayoutDirectionMode;
  KeyClickPolicy                  mKeyClickPolicy;
  uint32_t                        mLongPressKeyEventMinimumCount;
  std::optional<uint32_t>         mLongPressGestureMinimumHoldingTime;
  std::optional<uint32_t>         mTapGestureMaximumMultiTapInterval;
  std::optional<uint32_t>         mTapGestureMaximumHoldingTime;
  std::optional<float>            mTapGestureMaximumMotionDistance;
  uint32_t                        mAmbiguousPressDelay;
  uint32_t                        mAmbiguousPressDuration;
  WebEngineType                   mWebEngineType;
  float                           mCachedDpiFactor{1.0f};
  float                           mCachedScaledDpiFactor{1.0f};
  FocusIndicationPolicy::Function mFocusIndicationPolicy;
  bool                            mClearFocusOnEscapeEnabled;
  bool                            mClearFocusIndicationOnTouch;
  bool                            mClearFocusIndicationOnHover;
  bool                            mDefaultFocusIndicatorEnabled;
  bool                            mShowPlaceholderTextOnFocus;
  bool                            mLabelAsyncRendering;
  bool                            mFrozen;
};

UiConfigImpl::UiConfigImpl()
: mImpl(new Impl())
{
}

UiConfigImpl::~UiConfigImpl()
{
  delete mImpl;
}

UiConfigImplPtr UiConfigImpl::New()
{
  return UiConfigImplPtr(new UiConfigImpl());
}

void UiConfigImpl::Freeze()
{
  if(!mImpl->mStyleSheet)
  {
    mImpl->mStyleSheet = UiStyleSheet::New();
  }
  mImpl->mStyleSheet.Freeze();

  mImpl->mFrozen                = true;
  mImpl->mCachedDpiFactor       = static_cast<float>(mImpl->mDpi) / static_cast<float>(mImpl->mBaselineDpi);
  mImpl->mCachedScaledDpiFactor = mImpl->mCachedDpiFactor * mImpl->mScalingFactor;
}

bool UiConfigImpl::IsFrozen() const
{
  return mImpl->mFrozen;
}

float UiConfigImpl::GetDpiFactor() const
{
  return mImpl->mCachedDpiFactor;
}

float UiConfigImpl::GetScaledDpiFactor() const
{
  return mImpl->mCachedScaledDpiFactor;
}

void UiConfigImpl::SetScalingFactor(float scalingFactor)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mImpl->mScalingFactor = scalingFactor;
}

float UiConfigImpl::GetScalingFactor() const
{
  return mImpl->mScalingFactor;
}

void UiConfigImpl::SetDpi(int dpi)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mImpl->mDpi = dpi;
}

int UiConfigImpl::GetDpi() const
{
  return mImpl->mDpi;
}

void UiConfigImpl::SetBaselineDpi(int baselineDpi)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  DALI_ASSERT_ALWAYS(baselineDpi != 0 && "Baseline dpi could not be 0");
  mImpl->mBaselineDpi = baselineDpi;
}

int UiConfigImpl::GetBaselineDpi() const
{
  return mImpl->mBaselineDpi;
}

void UiConfigImpl::SetKeyClickPolicy(KeyClickPolicy policy)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mImpl->mKeyClickPolicy = policy;
}

KeyClickPolicy UiConfigImpl::GetKeyClickPolicy() const
{
  return mImpl->mKeyClickPolicy;
}

void UiConfigImpl::SetExecutionKeyPredicate(ExecutionKeyPredicate predicate)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mImpl->mExecutionKeyPredicate = predicate ? predicate : DefaultExecutionKeyPredicate;
}

ExecutionKeyPredicate UiConfigImpl::GetExecutionKeyPredicate() const
{
  return mImpl->mExecutionKeyPredicate;
}

void UiConfigImpl::SetLongPressKeyEventMinimumCount(uint32_t count)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mImpl->mLongPressKeyEventMinimumCount = count;
}

uint32_t UiConfigImpl::GetLongPressKeyEventMinimumCount() const
{
  return mImpl->mLongPressKeyEventMinimumCount;
}

void UiConfigImpl::SetLongPressGestureMinimumHoldingTime(uint32_t timeMs)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mImpl->mLongPressGestureMinimumHoldingTime = timeMs;
}

uint32_t UiConfigImpl::GetLongPressGestureMinimumHoldingTime() const
{
  return mImpl->mLongPressGestureMinimumHoldingTime.value_or(Dali::Integration::DEFAULT_LONG_PRESS_GESTURE_MINIMUM_HOLDING_TIME);
}

void UiConfigImpl::SetTapGestureMaximumMultiTapInterval(uint32_t intervalMs)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mImpl->mTapGestureMaximumMultiTapInterval = intervalMs;
}

uint32_t UiConfigImpl::GetTapGestureMaximumMultiTapInterval() const
{
  return mImpl->mTapGestureMaximumMultiTapInterval.value_or(Dali::Integration::DEFAULT_TAP_GESTURE_MAXIMUM_MULTI_TAP_INTERVAL);
}

void UiConfigImpl::SetTapGestureMaximumHoldingTime(uint32_t timeMs)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mImpl->mTapGestureMaximumHoldingTime = timeMs;
}

uint32_t UiConfigImpl::GetTapGestureMaximumHoldingTime() const
{
  return mImpl->mTapGestureMaximumHoldingTime.value_or(Dali::Integration::DEFAULT_TAP_GESTURE_MAXIMUM_HOLDING_TIME);
}

void UiConfigImpl::SetTapGestureMaximumMotionDistance(float distance)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mImpl->mTapGestureMaximumMotionDistance = distance;
}

float UiConfigImpl::GetTapGestureMaximumMotionDistance() const
{
  return mImpl->mTapGestureMaximumMotionDistance.value_or(Dali::Integration::DEFAULT_TAP_GESTURE_MAXIMUM_MOTION_DISTANCE);
}

void UiConfigImpl::SetAmbiguousPressDelay(uint32_t timeMs)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mImpl->mAmbiguousPressDelay = timeMs;
}

uint32_t UiConfigImpl::GetAmbiguousPressDelay() const
{
  return mImpl->mAmbiguousPressDelay;
}

void UiConfigImpl::SetAmbiguousPressDuration(uint32_t timeMs)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mImpl->mAmbiguousPressDuration = timeMs;
}

uint32_t UiConfigImpl::GetAmbiguousPressDuration() const
{
  return mImpl->mAmbiguousPressDuration;
}

void UiConfigImpl::SetWebEngineType(WebEngineType type)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after UiConfig::Apply()");
  mImpl->mWebEngineType = type;
}

WebEngineType UiConfigImpl::GetWebEngineType() const
{
  return mImpl->mWebEngineType;
}

void UiConfigImpl::SetBrokenImageUrl(UiConfig::BrokenImageType brokenImageType, const Dali::String& brokenImageUrl)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");

  uint32_t index = static_cast<uint32_t>(brokenImageType);
  if(index >= 3)
  {
    DALI_LOG_ERROR("Invalid BrokenImageType: [%d]\n", index);
    return;
  }

  mImpl->mBrokenImageUrls[index] = brokenImageUrl;
}

const Dali::String& UiConfigImpl::GetBrokenImageUrl(UiConfig::BrokenImageType brokenImageType) const
{
  uint32_t index = static_cast<uint32_t>(brokenImageType);
  if(index >= 3)
  {
    DALI_LOG_ERROR("Invalid BrokenImageType: [%d]\n", index);
    return mImpl->mBrokenImageUrls[0];
  }

  return mImpl->mBrokenImageUrls[index];
}

void UiConfigImpl::SetClearFocusOnEscapeEnabled(bool enable)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mImpl->mClearFocusOnEscapeEnabled = enable;
}

bool UiConfigImpl::IsClearFocusOnEscapeEnabled() const
{
  return mImpl->mClearFocusOnEscapeEnabled;
}

void UiConfigImpl::SetClearFocusIndicationOnTouch(bool clear)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mImpl->mClearFocusIndicationOnTouch = clear;
}

bool UiConfigImpl::IsClearFocusIndicationOnTouchEnabled() const
{
  return mImpl->mClearFocusIndicationOnTouch;
}

void UiConfigImpl::SetClearFocusIndicationOnHover(bool clear)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mImpl->mClearFocusIndicationOnHover = clear;
}

bool UiConfigImpl::IsClearFocusIndicationOnHoverEnabled() const
{
  return mImpl->mClearFocusIndicationOnHover;
}

void UiConfigImpl::SetDefaultFocusIndicatorEnabled(bool enabled)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mImpl->mDefaultFocusIndicatorEnabled = enabled;
}

bool UiConfigImpl::IsDefaultFocusIndicatorEnabled() const
{
  return mImpl->mDefaultFocusIndicatorEnabled;
}

void UiConfigImpl::SetDefaultFontSize(float fontSize)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mImpl->mDefaultFontSize = fontSize;
}

float UiConfigImpl::GetDefaultFontSize() const
{
  return mImpl->mDefaultFontSize;
}

void UiConfigImpl::SetDefaultSystemFontSizeScaleEnabled(bool enabled)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after UiConfig::Apply()");
  mImpl->mDefaultSystemFontSizeScaleEnabled = enabled;
}

bool UiConfigImpl::IsDefaultSystemFontSizeScaleEnabled() const
{
  return mImpl->mDefaultSystemFontSizeScaleEnabled;
}

void UiConfigImpl::SetDefaultMinimumFontSizeScale(float scale)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after UiConfig::Apply()");
  mImpl->mDefaultMinimumFontSizeScale = scale;
}

float UiConfigImpl::GetDefaultMinimumFontSizeScale() const
{
  return mImpl->mDefaultMinimumFontSizeScale;
}

void UiConfigImpl::SetDefaultMaximumFontSizeScale(float scale)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after UiConfig::Apply()");
  mImpl->mDefaultMaximumFontSizeScale = scale;
}

float UiConfigImpl::GetDefaultMaximumFontSizeScale() const
{
  return mImpl->mDefaultMaximumFontSizeScale;
}

void UiConfigImpl::SetScaleForSystemFontSize(UiConfig::SystemFontSize fontSize, float scale)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after UiConfig::Apply()");

  const auto index = static_cast<size_t>(fontSize);
  if(index >= mImpl->mSystemFontSizeScales.size())
  {
    DALI_LOG_ERROR("Invalid SystemFontSize: [%zu]\n", index);
    return;
  }

  mImpl->mSystemFontSizeScales[index] = scale;
}

float UiConfigImpl::GetScaleForSystemFontSize(UiConfig::SystemFontSize fontSize) const
{
  const auto index = static_cast<size_t>(fontSize);
  if(index >= mImpl->mSystemFontSizeScales.size())
  {
    DALI_LOG_ERROR("Invalid SystemFontSize: [%zu]\n", index);
    return 1.0f;
  }

  return mImpl->mSystemFontSizeScales[index];
}

void UiConfigImpl::SetDefaultTextColor(const Vector4& color)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mImpl->mDefaultTextColor = color;
}

Vector4 UiConfigImpl::GetDefaultTextColor() const
{
  return mImpl->mDefaultTextColor;
}

void UiConfigImpl::SetDefaultPlaceholderTextColor(const Vector4& color)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mImpl->mDefaultPlaceholderTextColor = color;
}

Vector4 UiConfigImpl::GetDefaultPlaceholderTextColor() const
{
  return mImpl->mDefaultPlaceholderTextColor;
}

void UiConfigImpl::SetShowPlaceholderTextOnFocus(bool enabled)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mImpl->mShowPlaceholderTextOnFocus = enabled;
}

bool UiConfigImpl::IsPlaceholderTextShownOnFocus() const
{
  return mImpl->mShowPlaceholderTextOnFocus;
}

void UiConfigImpl::SetMarqueeSpeed(int speed)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mImpl->mMarqueeSpeed = speed;
}

int UiConfigImpl::GetMarqueeSpeed() const
{
  return mImpl->mMarqueeSpeed;
}

void UiConfigImpl::SetMarqueeLoopCount(int loopCount)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mImpl->mMarqueeLoopCount = loopCount;
}

int UiConfigImpl::GetMarqueeLoopCount() const
{
  return mImpl->mMarqueeLoopCount;
}

void UiConfigImpl::SetMarqueeLoopDelay(float delay)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mImpl->mMarqueeLoopDelay = delay;
}

float UiConfigImpl::GetMarqueeLoopDelay() const
{
  return mImpl->mMarqueeLoopDelay;
}

void UiConfigImpl::SetMarqueeGap(float gap)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mImpl->mMarqueeGap = gap;
}

float UiConfigImpl::GetMarqueeGap() const
{
  return mImpl->mMarqueeGap;
}

void UiConfigImpl::SetMarqueeStopMode(Text::MarqueeStopMode stopMode)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mImpl->mMarqueeStopMode = stopMode;
}

Text::MarqueeStopMode UiConfigImpl::GetMarqueeStopMode() const
{
  return mImpl->mMarqueeStopMode;
}

void UiConfigImpl::SetMarqueeOrientation(Text::MarqueeOrientation orientation)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mImpl->mMarqueeOrientation = orientation;
}

Text::MarqueeOrientation UiConfigImpl::GetMarqueeOrientation() const
{
  return mImpl->mMarqueeOrientation;
}

void UiConfigImpl::SetTextLayoutDirectionMode(Text::LayoutDirectionMode mode)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mImpl->mTextLayoutDirectionMode = mode;
}

Text::LayoutDirectionMode UiConfigImpl::GetTextLayoutDirectionMode() const
{
  return mImpl->mTextLayoutDirectionMode;
}

void UiConfigImpl::SetLabelAsyncRendering(bool asyncRendering)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mImpl->mLabelAsyncRendering = asyncRendering;
}

bool UiConfigImpl::IsLabelAsyncRendering() const
{
  return mImpl->mLabelAsyncRendering;
}

void UiConfigImpl::SetViewInitializer(ViewInitializer initializer)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mImpl->mViewInitializer = initializer ? initializer : UiConfig::DefaultViewInitializer;
}

ViewInitializer UiConfigImpl::GetViewInitializer() const
{
  return mImpl->mViewInitializer;
}

void UiConfigImpl::SetDefaultStateEffectForInteractive(StateEffect effect)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after UiConfig::Apply()");
  if(!effect)
  {
    effect = StateEffect::None();
  }
  mImpl->mDefaultStateEffectForInteractive = effect;
}

StateEffect UiConfigImpl::GetDefaultStateEffectForInteractive() const
{
  return mImpl->mDefaultStateEffectForInteractive;
}

void UiConfigImpl::ResetStyleSheet(UiStyleSheet styleSheet)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after UiConfig::Apply()");
  DALI_ASSERT_ALWAYS(styleSheet && "UiStyleSheet must be initialized");
  mImpl->mStyleSheet = styleSheet;
}

UiStyleSheet UiConfigImpl::StyleSheet() const
{
  if(!mImpl->mStyleSheet)
  {
    mImpl->mStyleSheet = UiStyleSheet::New();
  }
  return mImpl->mStyleSheet;
}

void UiConfigImpl::OnApplied()
{
}

void UiConfigImpl::OnApplicationCreated()
{
  Dali::TextAbstraction::EnableDesignCompatibility();

  Dali::DevelWindowSystem::SetGeometryHittestEnabled(true);

  ApplyGestureOptions();
}

void UiConfigImpl::ApplyGestureOptions()
{
  if(mImpl->mLongPressGestureMinimumHoldingTime.has_value())
  {
    Dali::Integration::SetLongPressGestureMinimumHoldingTime(mImpl->mLongPressGestureMinimumHoldingTime.value());
  }
  else
  {
    mImpl->mLongPressGestureMinimumHoldingTime = Dali::Integration::GetLongPressGestureMinimumHoldingTime();
  }

  if(mImpl->mTapGestureMaximumMultiTapInterval.has_value())
  {
    Dali::Integration::SetTapGestureMaximumMultiTapInterval(mImpl->mTapGestureMaximumMultiTapInterval.value());
  }
  else
  {
    mImpl->mTapGestureMaximumMultiTapInterval = Dali::Integration::GetTapGestureMaximumMultiTapInterval();
  }

  if(mImpl->mTapGestureMaximumHoldingTime.has_value())
  {
    Dali::Integration::SetTapGestureMaximumHoldingTime(mImpl->mTapGestureMaximumHoldingTime.value());
  }
  else
  {
    mImpl->mTapGestureMaximumHoldingTime = Dali::Integration::GetTapGestureMaximumHoldingTime();
  }

  if(mImpl->mTapGestureMaximumMotionDistance.has_value())
  {
    Dali::Integration::SetTapGestureMaximumMotionDistance(mImpl->mTapGestureMaximumMotionDistance.value());
  }
  else
  {
    mImpl->mTapGestureMaximumMotionDistance = Dali::Integration::GetTapGestureMaximumMotionDistance();
  }
}

ThemeLoaderInterface* UiConfigImpl::CreateThemeLoader()
{
  return nullptr;
}

void UiConfigImpl::SetFocusIndicationPolicy(FocusIndicationPolicy::Function policy)
{
  DALI_ASSERT_ALWAYS(!mImpl->mFrozen && "UiConfig is frozen after UiConfig::Apply()");
  mImpl->mFocusIndicationPolicy = policy ? policy : &FocusIndicationPolicy::Default;
}

FocusIndicationPolicy::Function UiConfigImpl::GetFocusIndicationPolicy() const
{
  return mImpl->mFocusIndicationPolicy;
}

} // namespace Extension

} // namespace Ui

} // namespace Dali
