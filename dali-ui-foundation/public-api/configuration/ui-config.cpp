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
#include <dali-ui-foundation/public-api/configuration/ui-config.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/lifecycle-controller.h>
#include <dali/devel-api/common/singleton-service.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/connection-tracker.h>
#include <utility>

// INTERNAL INCLUDES
#include <dali-ui-foundation/extension-api/ui-config-impl.h>
#include <dali-ui-foundation/integration-api/ui-theme-manager-impl.h>
#include <dali-ui-foundation/public-api/configuration/ui-theme-manager.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{

namespace Ui
{

namespace
{
const char* const UICONFIG_NOT_APPLIED_MESSAGE =
  "UiConfig has not been applied. "
  "Call UiConfig::New().Apply() in main() before the application main loop starts. "
  "UiConfig provides global settings for the entire dali-ui framework. "
  "Do NOT access UiConfig-dependent features in static/global variable initializers.";

class UiConfigRuntime : public BaseObject, public ConnectionTracker
{
public:
  UiConfigRuntime()
  {
    RegisterLifecycleControllerCallback();
  }

  void Apply(const UiConfig& config)
  {
    DALI_ASSERT_ALWAYS(!mApplied && "UiConfig::Apply() must be called only once");

    mConfig = config;
    GetImpl(mConfig).Freeze();
    mApplied = true;

    RegisterLifecycleControllerCallback();

    UiThemeManager themeManager = UiThemeManager::Get();
    GetImpl(themeManager).EnsureThemeLoader();

    GetImpl(mConfig).OnApplied();

    if(mApplicationCreated)
    {
      GetImpl(mConfig).OnApplicationCreated();
    }
  }

  bool HasCurrent() const
  {
    return mApplied;
  }

  UiConfig GetCurrent() const
  {
    DALI_ASSERT_ALWAYS(mApplied && UICONFIG_NOT_APPLIED_MESSAGE);
    return mConfig;
  }

  bool RegisterLifecycleControllerCallback()
  {
    if(!mLifecycleControllerCallbackConnected)
    {
      Dali::LifecycleController lifecycleController = Dali::LifecycleController::Get();
      if(DALI_LIKELY(lifecycleController))
      {
        mLifecycleControllerCallbackConnected = true;
        lifecycleController.PreInitSignal().Connect(this, &UiConfigRuntime::OnApplicationCreated);
      }
    }
    return mLifecycleControllerCallbackConnected;
  }

protected:
  ~UiConfigRuntime() override = default;

private:
  UiConfigRuntime(const UiConfigRuntime&)            = delete;
  UiConfigRuntime(UiConfigRuntime&&)                 = delete;
  UiConfigRuntime& operator=(const UiConfigRuntime&) = delete;
  UiConfigRuntime& operator=(UiConfigRuntime&&)      = delete;

  void OnApplicationCreated()
  {
    if(mApplicationCreated)
    {
      return;
    }

    mApplicationCreated = true;

    if(mApplied)
    {
      GetImpl(mConfig).OnApplicationCreated();
    }
  }

private:
  UiConfig mConfig;
  bool     mApplied{false};
  bool     mApplicationCreated{false};
  bool     mLifecycleControllerCallbackConnected{false};
};

BaseHandle gPreInitializedUiConfigRuntime;

BaseHandle CreateRuntime()
{
  return BaseHandle(new UiConfigRuntime());
}

UiConfigRuntime& GetRuntimeObject(BaseHandle& handle)
{
  return static_cast<UiConfigRuntime&>(handle.GetBaseObject());
}

BaseHandle GetRuntime()
{
  BaseHandle runtime;

  SingletonService singletonService(SingletonService::Get());
  if(singletonService)
  {
    BaseHandle handle = singletonService.GetSingleton(typeid(UiConfigRuntime));
    if(handle)
    {
      runtime = handle;
    }

    if(!runtime)
    {
      if(gPreInitializedUiConfigRuntime)
      {
        runtime = std::move(gPreInitializedUiConfigRuntime);
        gPreInitializedUiConfigRuntime.Reset();

        GetRuntimeObject(runtime).RegisterLifecycleControllerCallback();
      }
      else
      {
        runtime = CreateRuntime();
      }
      singletonService.Register(typeid(UiConfigRuntime), runtime);
    }
  }
  else
  {
    if(!gPreInitializedUiConfigRuntime)
    {
      gPreInitializedUiConfigRuntime = CreateRuntime();
    }
    runtime = gPreInitializedUiConfigRuntime;
  }

  return runtime;
}

// Move a UiConfigRuntime created before Core initialization into SingletonService
// and connect it to PreInitSignal before the application InitSignal is emitted.
DALI_TYPE_REGISTRATION_BEGIN_CREATE(UiConfigRuntime, Dali::BaseHandle, GetRuntime, true)
DALI_TYPE_REGISTRATION_END()

} // unnamed namespace

UiConfig::UiConfig(Extension::UiConfigImpl* impl)
: BaseHandle(impl)
{
}

UiConfig UiConfig::New()
{
  Extension::UiConfigImplPtr impl = Extension::UiConfigImpl::New();

  // Pass ownership to handle
  UiConfig handle(impl.Get());

  return handle;
}

UiConfig UiConfig::DownCast(BaseHandle handle)
{
  return UiConfig(dynamic_cast<Extension::UiConfigImpl*>(handle.GetObjectPtr()));
}

void UiConfig::Apply()
{
  // Do not self-move: Apply() freezes the underlying implementation, but the handle
  // should remain usable (e.g. for getters) to match user expectations.
  BaseHandle runtime = GetRuntime();
  GetRuntimeObject(runtime).Apply(*this);
}

bool UiConfig::HasCurrent()
{
  BaseHandle runtime = GetRuntime();
  return GetRuntimeObject(runtime).HasCurrent();
}

UiConfig UiConfig::GetCurrent()
{
  BaseHandle runtime = GetRuntime();
  return GetRuntimeObject(runtime).GetCurrent();
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

void UiConfig::SetLongPressKeyEventMinimumCount(uint32_t count)
{
  GetImpl(*this).SetLongPressKeyEventMinimumCount(count);
}

uint32_t UiConfig::GetLongPressKeyEventMinimumCount() const
{
  return GetImpl(*this).GetLongPressKeyEventMinimumCount();
}

void UiConfig::SetLongPressGestureMinimumHoldingTime(uint32_t timeMs)
{
  GetImpl(*this).SetLongPressGestureMinimumHoldingTime(timeMs);
}

uint32_t UiConfig::GetLongPressGestureMinimumHoldingTime() const
{
  return GetImpl(*this).GetLongPressGestureMinimumHoldingTime();
}

void UiConfig::SetTapGestureMaximumMultiTapInterval(uint32_t intervalMs)
{
  GetImpl(*this).SetTapGestureMaximumMultiTapInterval(intervalMs);
}

uint32_t UiConfig::GetTapGestureMaximumMultiTapInterval() const
{
  return GetImpl(*this).GetTapGestureMaximumMultiTapInterval();
}

void UiConfig::SetTapGestureMaximumHoldingTime(uint32_t timeMs)
{
  GetImpl(*this).SetTapGestureMaximumHoldingTime(timeMs);
}

uint32_t UiConfig::GetTapGestureMaximumHoldingTime() const
{
  return GetImpl(*this).GetTapGestureMaximumHoldingTime();
}

void UiConfig::SetTapGestureMaximumMotionDistance(float distance)
{
  GetImpl(*this).SetTapGestureMaximumMotionDistance(distance);
}

float UiConfig::GetTapGestureMaximumMotionDistance() const
{
  return GetImpl(*this).GetTapGestureMaximumMotionDistance();
}

void UiConfig::SetBrokenImageUrl(BrokenImageType brokenImageType, const Dali::String& brokenImageUrl)
{
  GetImpl(*this).SetBrokenImageUrl(brokenImageType, brokenImageUrl);
}

const Dali::String& UiConfig::GetBrokenImageUrl(BrokenImageType brokenImageType) const
{
  return GetImpl(*this).GetBrokenImageUrl(brokenImageType);
}

void UiConfig::SetClearFocusOnEscapeEnabled(bool enable)
{
  GetImpl(*this).SetClearFocusOnEscapeEnabled(enable);
}

bool UiConfig::IsClearFocusOnEscapeEnabled() const
{
  return GetImpl(*this).IsClearFocusOnEscapeEnabled();
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

void UiConfig::SetDefaultSystemFontSizeScaleEnabled(bool enabled)
{
  GetImpl(*this).SetDefaultSystemFontSizeScaleEnabled(enabled);
}

bool UiConfig::IsDefaultSystemFontSizeScaleEnabled() const
{
  return GetImpl(*this).IsDefaultSystemFontSizeScaleEnabled();
}

void UiConfig::SetDefaultMinimumFontSizeScale(float scale)
{
  GetImpl(*this).SetDefaultMinimumFontSizeScale(scale);
}

float UiConfig::GetDefaultMinimumFontSizeScale() const
{
  return GetImpl(*this).GetDefaultMinimumFontSizeScale();
}

void UiConfig::SetDefaultMaximumFontSizeScale(float scale)
{
  GetImpl(*this).SetDefaultMaximumFontSizeScale(scale);
}

float UiConfig::GetDefaultMaximumFontSizeScale() const
{
  return GetImpl(*this).GetDefaultMaximumFontSizeScale();
}

void UiConfig::SetScaleForSystemFontSize(SystemFontSize fontSize, float scale)
{
  GetImpl(*this).SetScaleForSystemFontSize(fontSize, scale);
}

float UiConfig::GetScaleForSystemFontSize(SystemFontSize fontSize) const
{
  return GetImpl(*this).GetScaleForSystemFontSize(fontSize);
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

void UiConfig::SetDefaultStateEffectForInteractive(StateEffect effect)
{
  GetImpl(*this).SetDefaultStateEffectForInteractive(effect);
}

StateEffect UiConfig::GetDefaultStateEffectForInteractive() const
{
  return GetImpl(*this).GetDefaultStateEffectForInteractive();
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

void UiConfig::SetWebEngineType(WebEngineType type)
{
  GetImpl(*this).SetWebEngineType(type);
}

WebEngineType UiConfig::GetWebEngineType() const
{
  return GetImpl(*this).GetWebEngineType();
}

void UiConfig::ResetStyleSheet(UiStyleSheet styleSheet)
{
  GetImpl(*this).ResetStyleSheet(styleSheet);
}

UiStyleSheet UiConfig::StyleSheet() const
{
  return GetImpl(*this).StyleSheet();
}

void UiConfig::DefaultViewInitializer(View view)
{
}

} // namespace Ui
} // namespace Dali
