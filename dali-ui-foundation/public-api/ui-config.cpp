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
#include <dali-ui-foundation/public-api/ui-config.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/ui-config-impl.h>
#include <dali-ui-foundation/integration-api/ui-config-manager.h>

namespace Dali
{

namespace Ui
{

UiConfig::UiConfig(Integration::UiConfigImpl* impl)
: BaseHandle(impl)
{
}

UiConfig UiConfig::New()
{
  Integration::UiConfigImplPtr impl = Integration::UiConfigImpl::New();

  // Pass ownership to handle
  UiConfig handle(impl.Get());

  return handle;
}

UiConfig UiConfig::DownCast(BaseHandle handle)
{
  return UiConfig(dynamic_cast<Integration::UiConfigImpl*>(handle.GetObjectPtr()));
}

void UiConfig::Apply()
{
  // Do not self-move: Apply() freezes the underlying implementation, but the handle
  // should remain usable (e.g. for getters) to match user expectations.
  Integration::UiConfigManager::Get().Initialize(*this);
}

UiConfig& UiConfig::SetScalingFactor(float scalingFactor)
{
  GetImpl(*this).SetScalingFactor(scalingFactor);
  return *this;
}

float UiConfig::GetScalingFactor() const
{
  return GetImpl(*this).GetScalingFactor();
}

float UiConfig::GetDpiFactor() const
{
  return GetImpl(*this).GetDpiFactor();
}

float UiConfig::GetScaledDpiFactor() const
{
  return GetImpl(*this).GetScaledDpiFactor();
}

UiConfig& UiConfig::SetDpi(int dpi)
{
  GetImpl(*this).SetDpi(dpi);
  return *this;
}

int UiConfig::GetDpi() const
{
  return GetImpl(*this).GetDpi();
}

UiConfig& UiConfig::SetBaselineDpi(int baselineDpi)
{
  GetImpl(*this).SetBaselineDpi(baselineDpi);
  return *this;
}

int UiConfig::GetBaselineDpi() const
{
  return GetImpl(*this).GetBaselineDpi();
}

UiConfig& UiConfig::SetKeyClickPolicy(KeyClickPolicy policy)
{
  GetImpl(*this).SetKeyClickPolicy(policy);
  return *this;
}

KeyClickPolicy UiConfig::GetKeyClickPolicy() const
{
  return GetImpl(*this).GetKeyClickPolicy();
}

UiConfig& UiConfig::SetExecutionKeyPredicate(ExecutionKeyPredicate predicate)
{
  GetImpl(*this).SetExecutionKeyPredicate(predicate);
  return *this;
}

ExecutionKeyPredicate UiConfig::GetExecutionKeyPredicate() const
{
  return GetImpl(*this).GetExecutionKeyPredicate();
}

UiConfig& UiConfig::SetKeyLongPressThreshold(uint32_t count)
{
  GetImpl(*this).SetKeyLongPressThreshold(count);
  return *this;
}

uint32_t UiConfig::GetKeyLongPressThreshold() const
{
  return GetImpl(*this).GetKeyLongPressThreshold();
}

UiConfig& UiConfig::SetTapRecognizerTime(uint32_t timeMs)
{
  GetImpl(*this).SetTapRecognizerTime(timeMs);
  return *this;
}

uint32_t UiConfig::GetTapRecognizerTime() const
{
  return GetImpl(*this).GetTapRecognizerTime();
}

UiConfig& UiConfig::SetBrokenImageUrl(BrokenImageType brokenImageType, const Dali::String& brokenImageUrl)
{
  GetImpl(*this).SetBrokenImageUrl(brokenImageType, brokenImageUrl);
  return *this;
}

const Dali::String& UiConfig::GetBrokenImageUrl(BrokenImageType brokenImageType) const
{
  return GetImpl(*this).GetBrokenImageUrl(brokenImageType);
}

UiConfig& UiConfig::EnableFocusClearOnEscape(bool enable)
{
  GetImpl(*this).EnableFocusClearOnEscape(enable);
  return *this;
}

bool UiConfig::IsFocusClearOnEscapeEnabled() const
{
  return GetImpl(*this).IsFocusClearOnEscapeEnabled();
}

UiConfig& UiConfig::SetAlwaysShowFocus(bool alwaysShow)
{
  GetImpl(*this).SetAlwaysShowFocus(alwaysShow);
  return *this;
}

bool UiConfig::IsFocusIndicatorAlwaysShown() const
{
  return GetImpl(*this).IsFocusIndicatorAlwaysShown();
}

UiConfig& UiConfig::SetDefaultFontSize(float fontSize)
{
  GetImpl(*this).SetDefaultFontSize(fontSize);
  return *this;
}

float UiConfig::GetDefaultFontSize() const
{
  return GetImpl(*this).GetDefaultFontSize();
}

UiConfig& UiConfig::SetDefaultTextColor(const Vector4& color)
{
  GetImpl(*this).SetDefaultTextColor(color);
  return *this;
}

Vector4 UiConfig::GetDefaultTextColor() const
{
  return GetImpl(*this).GetDefaultTextColor();
}

UiConfig& UiConfig::SetMarqueeSpeed(int speed)
{
  GetImpl(*this).SetMarqueeSpeed(speed);
  return *this;
}

int UiConfig::GetMarqueeSpeed() const
{
  return GetImpl(*this).GetMarqueeSpeed();
}

UiConfig& UiConfig::SetMarqueeLoopCount(int loopCount)
{
  GetImpl(*this).SetMarqueeLoopCount(loopCount);
  return *this;
}

int UiConfig::GetMarqueeLoopCount() const
{
  return GetImpl(*this).GetMarqueeLoopCount();
}

UiConfig& UiConfig::SetMarqueeLoopDelay(float delay)
{
  GetImpl(*this).SetMarqueeLoopDelay(delay);
  return *this;
}

float UiConfig::GetMarqueeLoopDelay() const
{
  return GetImpl(*this).GetMarqueeLoopDelay();
}

UiConfig& UiConfig::SetMarqueeGap(float gap)
{
  GetImpl(*this).SetMarqueeGap(gap);
  return *this;
}

float UiConfig::GetMarqueeGap() const
{
  return GetImpl(*this).GetMarqueeGap();
}

UiConfig& UiConfig::SetMarqueeStopMode(Text::MarqueeStopMode stopMode)
{
  GetImpl(*this).SetMarqueeStopMode(stopMode);
  return *this;
}

Text::MarqueeStopMode UiConfig::GetMarqueeStopMode() const
{
  return GetImpl(*this).GetMarqueeStopMode();
}

UiConfig& UiConfig::SetMarqueeOrientation(Text::MarqueeOrientation orientation)
{
  GetImpl(*this).SetMarqueeOrientation(orientation);
  return *this;
}

Text::MarqueeOrientation UiConfig::GetMarqueeOrientation() const
{
  return GetImpl(*this).GetMarqueeOrientation();
}

UiConfig& UiConfig::SetLabelAsyncRendering(bool asyncRendering)
{
  GetImpl(*this).SetLabelAsyncRendering(asyncRendering);
  return *this;
}

bool UiConfig::IsLabelAsyncRendering() const
{
  return GetImpl(*this).IsLabelAsyncRendering();
}

UiConfig& UiConfig::SetDefaultInteractionEffect(Trait effect)
{
  GetImpl(*this).SetDefaultInteractionEffect(effect);
  return *this;
}

Trait UiConfig::GetDefaultInteractionEffect() const
{
  return GetImpl(*this).GetDefaultInteractionEffect();
}

} // namespace Ui
} // namespace Dali
