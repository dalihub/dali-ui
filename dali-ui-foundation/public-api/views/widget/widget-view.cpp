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
#include <dali-ui-foundation/public-api/views/widget/widget-view.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/string-utils.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/views/widget/widget-view-impl.h>

namespace DALI_NAMESPACE
{

namespace Ui
{

WidgetView WidgetView::New(const Dali::String& appId, const Dali::String& widgetId, const Dali::String& contentInfo, int width, int height, float updatePeriod)
{
  return Internal::WidgetViewImpl::New(Dali::Integration::ToStdString(appId), Dali::Integration::ToStdString(widgetId), Dali::Integration::ToStdString(contentInfo), width, height, updatePeriod);
}

WidgetView WidgetView::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<WidgetView, Internal::WidgetViewImpl>(handle);
}

WidgetView::WidgetView() = default;

WidgetView::WidgetView(const WidgetView& handle) = default;

WidgetView& WidgetView::operator=(const WidgetView& handle) = default;

WidgetView::~WidgetView() = default;

Dali::String WidgetView::GetWidgetId() const
{
  return Dali::Ui::Internal::GetImplementation(*this).GetWidgetId();
}

Dali::String WidgetView::GetInstanceId() const
{
  return Dali::Ui::Internal::GetImplementation(*this).GetInstanceId();
}

Dali::String WidgetView::GetContentInfo() const
{
  return Dali::Ui::Internal::GetImplementation(*this).GetContentInfo();
}

Dali::String WidgetView::GetTitle() const
{
  return Dali::Ui::Internal::GetImplementation(*this).GetTitle();
}

float WidgetView::GetUpdatePeriod() const
{
  return Dali::Ui::Internal::GetImplementation(*this).GetUpdatePeriod();
}

bool WidgetView::IsWidgetFaulted() const
{
  return Dali::Ui::Internal::GetImplementation(*this).IsWidgetFaulted();
}

void WidgetView::SetPreviewEnabled(bool enabled)
{
  Dali::Ui::Internal::GetImplementation(*this).SetPreviewEnabled(enabled);
}

bool WidgetView::IsPreviewEnabled() const
{
  return Dali::Ui::Internal::GetImplementation(*this).IsPreviewEnabled();
}

void WidgetView::SetLoadingTextVisible(bool visible)
{
  Dali::Ui::Internal::GetImplementation(*this).SetLoadingTextVisible(visible);
}

bool WidgetView::IsLoadingTextVisible() const
{
  return Dali::Ui::Internal::GetImplementation(*this).IsLoadingTextVisible();
}

void WidgetView::SetRetryTextVisible(bool visible)
{
  Dali::Ui::Internal::GetImplementation(*this).SetRetryTextVisible(visible);
}

bool WidgetView::IsRetryTextVisible() const
{
  return Dali::Ui::Internal::GetImplementation(*this).IsRetryTextVisible();
}

void WidgetView::SetKeepWidgetSizeEnabled(bool enabled)
{
  Dali::Ui::Internal::GetImplementation(*this).SetKeepWidgetSizeEnabled(enabled);
}

bool WidgetView::IsKeepWidgetSizeEnabled() const
{
  return Dali::Ui::Internal::GetImplementation(*this).IsKeepWidgetSizeEnabled();
}

void WidgetView::SetLoadingText(const Dali::String& text)
{
  Dali::Ui::Internal::GetImplementation(*this).SetLoadingText(text);
}

void WidgetView::SetRetryText(const Dali::String& text)
{
  Dali::Ui::Internal::GetImplementation(*this).SetRetryText(text);
}

void WidgetView::SetLoadingTextFont(const Text::FontAttributes& font)
{
  Dali::Ui::Internal::GetImplementation(*this).SetLoadingTextFont(font);
}

void WidgetView::SetRetryTextFont(const Text::FontAttributes& font)
{
  Dali::Ui::Internal::GetImplementation(*this).SetRetryTextFont(font);
}

void WidgetView::SetLoadingTextColor(const UiColor& color)
{
  Dali::Ui::Internal::GetImplementation(*this).SetLoadingTextColor(color);
}

void WidgetView::SetRetryTextColor(const UiColor& color)
{
  Dali::Ui::Internal::GetImplementation(*this).SetRetryTextColor(color);
}

void WidgetView::SetCustomShader(const Dali::String& vertexSource, const Dali::String& fragmentSource)
{
  Dali::Ui::Internal::GetImplementation(*this).SetCustomShader(vertexSource, fragmentSource);
}

void WidgetView::ClearCustomShader()
{
  Dali::Ui::Internal::GetImplementation(*this).ClearCustomShader();
}

bool WidgetView::PauseWidget()
{
  return Dali::Ui::Internal::GetImplementation(*this).PauseWidget();
}

bool WidgetView::ResumeWidget()
{
  return Dali::Ui::Internal::GetImplementation(*this).ResumeWidget();
}

bool WidgetView::CancelTouchEvent()
{
  return Dali::Ui::Internal::GetImplementation(*this).CancelTouchEvent();
}

void WidgetView::ActivateFaultedWidget()
{
  return Dali::Ui::Internal::GetImplementation(*this).ActivateFaultedWidget();
}

WidgetView::WidgetViewSignalType& WidgetView::WidgetAddedSignal()
{
  return Dali::Ui::Internal::GetImplementation(*this).WidgetAddedSignal();
}

WidgetView::WidgetViewSignalType& WidgetView::WidgetDeletedSignal()
{
  return Dali::Ui::Internal::GetImplementation(*this).WidgetDeletedSignal();
}

WidgetView::WidgetViewSignalType& WidgetView::WidgetCreationAbortedSignal()
{
  return Dali::Ui::Internal::GetImplementation(*this).WidgetCreationAbortedSignal();
}

WidgetView::WidgetViewSignalType& WidgetView::WidgetContentUpdatedSignal()
{
  return Dali::Ui::Internal::GetImplementation(*this).WidgetContentUpdatedSignal();
}

WidgetView::WidgetViewSignalType& WidgetView::WidgetUpdatePeriodChangedSignal()
{
  return Dali::Ui::Internal::GetImplementation(*this).WidgetUpdatePeriodChangedSignal();
}

WidgetView::WidgetViewSignalType& WidgetView::WidgetFaultedSignal()
{
  return Dali::Ui::Internal::GetImplementation(*this).WidgetFaultedSignal();
}

WidgetView::WidgetView(Internal::WidgetViewImpl& implementation)
: View(implementation)
{
}

WidgetView::WidgetView(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Internal::WidgetViewImpl>(internal);
}

} // namespace Ui

} // namespace DALI_NAMESPACE
