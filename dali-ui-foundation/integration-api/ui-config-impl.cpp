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
#include <dali/public-api/common/dali-common.h>
#include <dali/integration-api/debug.h>

namespace
{

bool DefaultExecutionKeyPredicate(const std::string& keyName)
{
  return keyName == "Return";
}

} // unnamed namespace

namespace Dali
{

namespace Ui
{

namespace Integration
{

UiConfigImpl::UiConfigImpl()
  : mExecutionKeyPredicate(DefaultExecutionKeyPredicate),
    mScalingFactor(1.0f),
    mDpi(160),
    mBaselineDpi(160),
    mKeyClickPolicy(KeyClickPolicy::ON_RELEASE),
    mMinLongPressKeyCount(3),
    mTapRecognizerTime(UINT32_MAX),
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

float UiConfigImpl::GetDpiFactor() const
{
  return static_cast<float>(mDpi) / static_cast<float>(mBaselineDpi);
}

float UiConfigImpl::GetScaledDpiFactor() const
{
  return GetDpiFactor() * mScalingFactor;
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

void UiConfigImpl::OnInitialized()
{
}

} // namespace Integration

} // namespace Ui

} // namespace Dali
