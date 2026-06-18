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
#include <dali-ui-foundation/public-api/view.h>

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

void UiConfig::SetScalingFactor(float scalingFactor)
{
  GetImpl(*this).SetScalingFactor(scalingFactor);
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

void UiConfig::SetDpi(int dpi)
{
  GetImpl(*this).SetDpi(dpi);
}

int UiConfig::GetDpi() const
{
  return GetImpl(*this).GetDpi();
}

void UiConfig::SetBaselineDpi(int baselineDpi)
{
  GetImpl(*this).SetBaselineDpi(baselineDpi);
}

int UiConfig::GetBaselineDpi() const
{
  return GetImpl(*this).GetBaselineDpi();
}

void UiConfig::SetKeyClickPolicy(KeyClickPolicy policy)
{
  GetImpl(*this).SetKeyClickPolicy(policy);
}

KeyClickPolicy UiConfig::GetKeyClickPolicy() const
{
  return GetImpl(*this).GetKeyClickPolicy();
}

void UiConfig::SetExecutionKeyPredicate(ExecutionKeyPredicate predicate)
{
  GetImpl(*this).SetExecutionKeyPredicate(predicate);
}

ExecutionKeyPredicate UiConfig::GetExecutionKeyPredicate() const
{
  return GetImpl(*this).GetExecutionKeyPredicate();
}

void UiConfig::SetKeyLongPressThreshold(uint32_t count)
{
  GetImpl(*this).SetKeyLongPressThreshold(count);
}

uint32_t UiConfig::GetKeyLongPressThreshold() const
{
  return GetImpl(*this).GetKeyLongPressThreshold();
}

void UiConfig::SetTapRecognizerTime(uint32_t timeMs)
{
  GetImpl(*this).SetTapRecognizerTime(timeMs);
}

uint32_t UiConfig::GetTapRecognizerTime() const
{
  return GetImpl(*this).GetTapRecognizerTime();
}

void UiConfig::SetBrokenImageUrl(BrokenImageType brokenImageType, const Dali::String& brokenImageUrl)
{
  GetImpl(*this).SetBrokenImageUrl(brokenImageType, brokenImageUrl);
}

const Dali::String& UiConfig::GetBrokenImageUrl(BrokenImageType brokenImageType) const
{
  return GetImpl(*this).GetBrokenImageUrl(brokenImageType);
}

void UiConfig::EnableFocusClearOnEscape(bool enable)
{
  GetImpl(*this).EnableFocusClearOnEscape(enable);
}

bool UiConfig::IsFocusClearOnEscapeEnabled() const
{
  return GetImpl(*this).IsFocusClearOnEscapeEnabled();
}

void UiConfig::SetClearFocusIndicationOnTouch(bool clear)
{
  GetImpl(*this).SetClearFocusIndicationOnTouch(clear);
}

bool UiConfig::IsClearFocusIndicationOnTouchEnabled() const
{
  return GetImpl(*this).IsClearFocusIndicationOnTouchEnabled();
}

void UiConfig::SetClearFocusIndicationOnHover(bool clear)
{
  GetImpl(*this).SetClearFocusIndicationOnHover(clear);
}

bool UiConfig::IsClearFocusIndicationOnHoverEnabled() const
{
  return GetImpl(*this).IsClearFocusIndicationOnHoverEnabled();
}

void UiConfig::SetDefaultFocusIndicatorEnabled(bool enabled)
{
  GetImpl(*this).SetDefaultFocusIndicatorEnabled(enabled);
}

bool UiConfig::IsDefaultFocusIndicatorEnabled() const
{
  return GetImpl(*this).IsDefaultFocusIndicatorEnabled();
}

void UiConfig::SetDefaultFontSize(float fontSize)
{
  GetImpl(*this).SetDefaultFontSize(fontSize);
}

float UiConfig::GetDefaultFontSize() const
{
  return GetImpl(*this).GetDefaultFontSize();
}

void UiConfig::SetDefaultTextColor(const Vector4& color)
{
  GetImpl(*this).SetDefaultTextColor(color);
}

Vector4 UiConfig::GetDefaultTextColor() const
{
  return GetImpl(*this).GetDefaultTextColor();
}

void UiConfig::SetDefaultPlaceholderTextColor(const Vector4& color)
{
  GetImpl(*this).SetDefaultPlaceholderTextColor(color);
}

Vector4 UiConfig::GetDefaultPlaceholderTextColor() const
{
  return GetImpl(*this).GetDefaultPlaceholderTextColor();
}

void UiConfig::SetShowPlaceholderTextOnFocus(bool enabled)
{
  GetImpl(*this).SetShowPlaceholderTextOnFocus(enabled);
}

bool UiConfig::IsPlaceholderTextShownOnFocus() const
{
  return GetImpl(*this).IsPlaceholderTextShownOnFocus();
}

void UiConfig::SetMarqueeSpeed(int speed)
{
  GetImpl(*this).SetMarqueeSpeed(speed);
}

int UiConfig::GetMarqueeSpeed() const
{
  return GetImpl(*this).GetMarqueeSpeed();
}

void UiConfig::SetMarqueeLoopCount(int loopCount)
{
  GetImpl(*this).SetMarqueeLoopCount(loopCount);
}

int UiConfig::GetMarqueeLoopCount() const
{
  return GetImpl(*this).GetMarqueeLoopCount();
}

void UiConfig::SetMarqueeLoopDelay(float delay)
{
  GetImpl(*this).SetMarqueeLoopDelay(delay);
}

float UiConfig::GetMarqueeLoopDelay() const
{
  return GetImpl(*this).GetMarqueeLoopDelay();
}

void UiConfig::SetMarqueeGap(float gap)
{
  GetImpl(*this).SetMarqueeGap(gap);
}

float UiConfig::GetMarqueeGap() const
{
  return GetImpl(*this).GetMarqueeGap();
}

void UiConfig::SetMarqueeStopMode(Text::MarqueeStopMode stopMode)
{
  GetImpl(*this).SetMarqueeStopMode(stopMode);
}

Text::MarqueeStopMode UiConfig::GetMarqueeStopMode() const
{
  return GetImpl(*this).GetMarqueeStopMode();
}

void UiConfig::SetMarqueeOrientation(Text::MarqueeOrientation orientation)
{
  GetImpl(*this).SetMarqueeOrientation(orientation);
}

Text::MarqueeOrientation UiConfig::GetMarqueeOrientation() const
{
  return GetImpl(*this).GetMarqueeOrientation();
}

void UiConfig::SetTextLayoutDirectionMode(Text::LayoutDirectionMode mode)
{
  GetImpl(*this).SetTextLayoutDirectionMode(mode);
}

Text::LayoutDirectionMode UiConfig::GetTextLayoutDirectionMode() const
{
  return GetImpl(*this).GetTextLayoutDirectionMode();
}

void UiConfig::SetLabelAsyncRendering(bool asyncRendering)
{
  GetImpl(*this).SetLabelAsyncRendering(asyncRendering);
}

bool UiConfig::IsLabelAsyncRendering() const
{
  return GetImpl(*this).IsLabelAsyncRendering();
}

void UiConfig::SetViewInitializer(ViewInitializer initializer)
{
  GetImpl(*this).SetViewInitializer(initializer);
}

ViewInitializer UiConfig::GetViewInitializer() const
{
  return GetImpl(*this).GetViewInitializer();
}

void UiConfig::SetAmbiguousPressDelay(uint32_t timeMs)
{
  GetImpl(*this).SetAmbiguousPressDelay(timeMs);
}

uint32_t UiConfig::GetAmbiguousPressDelay() const
{
  return GetImpl(*this).GetAmbiguousPressDelay();
}

void UiConfig::SetAmbiguousPressDuration(uint32_t timeMs)
{
  GetImpl(*this).SetAmbiguousPressDuration(timeMs);
}

uint32_t UiConfig::GetAmbiguousPressDuration() const
{
  return GetImpl(*this).GetAmbiguousPressDuration();
}

void UiConfig::DefaultViewInitializer(View view)
{
  // NOTE: UI layout coordinates are normally based on the parent's top-left,
  // while scale/rotation transform origins are normally centered. Keep
  // ParentOrigin as TOP_LEFT for placement and leave PIVOT unset here so the
  // DALi default CENTER pivot remains the View transform origin.
  view.SetParentOrigin(ParentOrigin::TOP_LEFT);
  view.SetProperty(Actor::Property::POSITION_USES_PIVOT, false);
}

} // namespace Ui
} // namespace Dali
