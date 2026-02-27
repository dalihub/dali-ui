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

namespace UI
{

namespace Integration
{

UIConfigImpl::UIConfigImpl()
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

UIConfigImpl::~UIConfigImpl() = default;

UIConfig UIConfigImpl::New()
{
  IntrusivePtr<UIConfigImpl> impl = new UIConfigImpl();
  UIConfig handle(impl.Get());
  return handle;
}

void UIConfigImpl::Freeze()
{
  mFrozen = true;
}

bool UIConfigImpl::IsFrozen() const
{
  return mFrozen;
}

void UIConfigImpl::SetScalingFactor(float scalingFactor)
{
  DALI_ASSERT_ALWAYS(!mFrozen && "UIConfig is frozen after  UIConfig::Apply()");
  mScalingFactor = scalingFactor;
}

float UIConfigImpl::GetScalingFactor() const
{
  return mScalingFactor;
}

void UIConfigImpl::SetDpi(int dpi)
{
  DALI_ASSERT_ALWAYS(!mFrozen && "UIConfig is frozen after  UIConfig::Apply()");
  mDpi = dpi;
}

int UIConfigImpl::GetDpi() const
{
  return mDpi;
}

void UIConfigImpl::SetBaselineDpi(int baselineDpi)
{
  DALI_ASSERT_ALWAYS(!mFrozen && "UIConfig is frozen after  UIConfig::Apply()");
  DALI_ASSERT_ALWAYS(mBaselineDpi != 0 && "Baseline dpi could not be 0");
  mBaselineDpi = baselineDpi;
}

int UIConfigImpl::GetBaselineDpi() const
{
  return mBaselineDpi;
}

float UIConfigImpl::GetDpiFactor() const
{
  return static_cast<float>(mDpi) / static_cast<float>(mBaselineDpi);
}

float UIConfigImpl::GetScaledDpiFactor() const
{
  return GetDpiFactor() * mScalingFactor;
}

void UIConfigImpl::SetKeyClickPolicy(KeyClickPolicy policy)
{
  DALI_ASSERT_ALWAYS(!mFrozen && "UIConfig is frozen after  UIConfig::Apply()");
  mKeyClickPolicy = policy;
}

KeyClickPolicy UIConfigImpl::GetKeyClickPolicy() const
{
  return mKeyClickPolicy;
}

void UIConfigImpl::SetExecutionKeyPredicate(ExecutionKeyPredicate predicate)
{
  DALI_ASSERT_ALWAYS(!mFrozen && "UIConfig is frozen after  UIConfig::Apply()");
  DALI_ASSERT_ALWAYS(predicate && "ExecutionKeyPredicate must not be null");
  mExecutionKeyPredicate = predicate;
}

ExecutionKeyPredicate UIConfigImpl::GetExecutionKeyPredicate() const
{
  return mExecutionKeyPredicate;
}

void UIConfigImpl::SetMinLongPressKeyCount(uint32_t count)
{
  DALI_ASSERT_ALWAYS(!mFrozen && "UIConfig is frozen after  UIConfig::Apply()");
  mMinLongPressKeyCount = count;
}

uint32_t UIConfigImpl::GetMinLongPressKeyCount() const
{
  return mMinLongPressKeyCount;
}

void UIConfigImpl::SetTapRecognizerTime(uint32_t timeMs)
{
  DALI_ASSERT_ALWAYS(!mFrozen && "UIConfig is frozen after  UIConfig::Apply()");
  mTapRecognizerTime = timeMs;
}

uint32_t UIConfigImpl::GetTapRecognizerTime() const
{
  return mTapRecognizerTime;
}

void UIConfigImpl::OnInitialized()
{
}

} // namespace Integration

} // namespace UI

} // namespace Dali
