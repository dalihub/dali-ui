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
#include <dali-ui-foundation/integration-api/ui-config-impl.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/dali-common.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/default-theme-loader.h>

namespace
{

bool DefaultExecutionKeyPredicate(const std::string& keyName)
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

namespace Integration
{

UiConfigImpl::UiConfigImpl()
: mExecutionKeyPredicate(DefaultExecutionKeyPredicate),
  mDefaultTextColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f)),
  mScalingFactor(1.0f),
  mDefaultFontSize(16.0f),
  mDpi(160),
  mBaselineDpi(160),
  mKeyClickPolicy(KeyClickPolicy::ON_RELEASE),
  mMinLongPressKeyCount(3),
  mTapRecognizerTime(UINT32_MAX),
  mClearFocusOnEscape(true),
  mAlwaysShowFocus(true),
  mFrozen(false)
{
}

UiConfigImpl::~UiConfigImpl() = default;

UiConfigImplPtr UiConfigImpl::New()
{
  return UiConfigImplPtr(new UiConfigImpl());
}

void UiConfigImpl::Freeze()
{
  mFrozen = true;
}

bool UiConfigImpl::IsFrozen() const
{
  return mFrozen;
}

void UiConfigImpl::SetScalingFactor(float scalingFactor)
{
  DALI_ASSERT_ALWAYS(!mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mScalingFactor = scalingFactor;
}

float UiConfigImpl::GetScalingFactor() const
{
  return mScalingFactor;
}

void UiConfigImpl::SetDpi(int dpi)
{
  DALI_ASSERT_ALWAYS(!mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mDpi = dpi;
}

int UiConfigImpl::GetDpi() const
{
  return mDpi;
}

void UiConfigImpl::SetBaselineDpi(int baselineDpi)
{
  DALI_ASSERT_ALWAYS(!mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  DALI_ASSERT_ALWAYS(mBaselineDpi != 0 && "Baseline dpi could not be 0");
  mBaselineDpi = baselineDpi;
}

int UiConfigImpl::GetBaselineDpi() const
{
  return mBaselineDpi;
}

void UiConfigImpl::SetKeyClickPolicy(KeyClickPolicy policy)
{
  DALI_ASSERT_ALWAYS(!mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mKeyClickPolicy = policy;
}

KeyClickPolicy UiConfigImpl::GetKeyClickPolicy() const
{
  return mKeyClickPolicy;
}

void UiConfigImpl::SetExecutionKeyPredicate(ExecutionKeyPredicate predicate)
{
  DALI_ASSERT_ALWAYS(!mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  DALI_ASSERT_ALWAYS(predicate && "ExecutionKeyPredicate must not be null");
  mExecutionKeyPredicate = predicate;
}

ExecutionKeyPredicate UiConfigImpl::GetExecutionKeyPredicate() const
{
  return mExecutionKeyPredicate;
}

void UiConfigImpl::SetMinLongPressKeyCount(uint32_t count)
{
  DALI_ASSERT_ALWAYS(!mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mMinLongPressKeyCount = count;
}

uint32_t UiConfigImpl::GetMinLongPressKeyCount() const
{
  return mMinLongPressKeyCount;
}

void UiConfigImpl::SetTapRecognizerTime(uint32_t timeMs)
{
  DALI_ASSERT_ALWAYS(!mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mTapRecognizerTime = timeMs;
}

uint32_t UiConfigImpl::GetTapRecognizerTime() const
{
  return mTapRecognizerTime;
}

void UiConfigImpl::SetBrokenImageUrl(UiConfig::BrokenImageType brokenImageType, const std::string& brokenImageUrl)
{
  DALI_ASSERT_ALWAYS(!mFrozen && "UiConfig is frozen after  UiConfig::Apply()");

  uint32_t index = static_cast<uint32_t>(brokenImageType);
  if(index >= 3)
  {
    DALI_LOG_ERROR("Invalid BrokenImageType: [%d]\n", index);
    return;
  }

  mBrokenImageUrls[index] = brokenImageUrl;
}

const std::string& UiConfigImpl::GetBrokenImageUrl(UiConfig::BrokenImageType brokenImageType) const
{
  uint32_t index = static_cast<uint32_t>(brokenImageType);
  if(index >= 3)
  {
    DALI_LOG_ERROR("Invalid BrokenImageType: [%d]\n", index);
    return mBrokenImageUrls[0];
  }

  return mBrokenImageUrls[index];
}

std::vector<std::string> UiConfigImpl::GetBrokenImageUrlList() const
{
  return {mBrokenImageUrls[0], mBrokenImageUrls[1], mBrokenImageUrls[2]};
}

void UiConfigImpl::EnableFocusClearOnEscape(bool enable)
{
  DALI_ASSERT_ALWAYS(!mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mClearFocusOnEscape = enable;
}

bool UiConfigImpl::IsFocusClearOnEscapeEnabled() const
{
  return mClearFocusOnEscape;
}

void UiConfigImpl::SetAlwaysShowFocus(bool alwaysShow)
{
  DALI_ASSERT_ALWAYS(!mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mAlwaysShowFocus = alwaysShow;
}

bool UiConfigImpl::IsFocusIndicatorAlwaysShown() const
{
  return mAlwaysShowFocus;
}

void UiConfigImpl::SetDefaultFontSize(float fontSize)
{
  DALI_ASSERT_ALWAYS(!mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mDefaultFontSize = fontSize;
}

float UiConfigImpl::GetDefaultFontSize() const
{
  return mDefaultFontSize;
}

void UiConfigImpl::SetDefaultTextColor(const Vector4& color)
{
  DALI_ASSERT_ALWAYS(!mFrozen && "UiConfig is frozen after  UiConfig::Apply()");
  mDefaultTextColor = color;
}

Vector4 UiConfigImpl::GetDefaultTextColor() const
{
  return mDefaultTextColor;
}

void UiConfigImpl::OnInitialized()
{
}

ThemeLoaderInterface* UiConfigImpl::CreateThemeLoader()
{
  return new DefaultThemeLoader();
}

} // namespace Integration

} // namespace Ui

} // namespace Dali
