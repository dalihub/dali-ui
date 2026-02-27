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
#include <dali-ui-elements/integration-api/ui-element-config-impl.h>
#include <dali-ui-elements/integration-api/ui-element-config-manager.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

namespace UI
{

namespace Integration
{

UIElementConfigImpl::UIElementConfigImpl()
  : UIConfigImpl(),
    mDefaultFontSize(16.0f),
    mDefaultTextColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f))
{
}

UIElementConfigImpl::~UIElementConfigImpl() = default;

UIElementConfig UIElementConfigImpl::New()
{
  IntrusivePtr<UIElementConfigImpl> impl = new UIElementConfigImpl();
  UIElementConfig handle(impl.Get());
  return handle;
}

void UIElementConfigImpl::SetDefaultFontSize(float fontSize)
{
  DALI_ASSERT_ALWAYS(!IsFrozen() && "UIElementConfig is frozen after UIConfig::Apply()");
  mDefaultFontSize = fontSize;
}

float UIElementConfigImpl::GetDefaultFontSize() const
{
  return mDefaultFontSize;
}

void UIElementConfigImpl::SetDefaultTextColor(const Vector4& color)
{
  DALI_ASSERT_ALWAYS(!IsFrozen() && "UIElementConfig is frozen after UIConfig::Apply()");
  mDefaultTextColor = color;
}

Vector4 UIElementConfigImpl::GetDefaultTextColor() const
{
  return mDefaultTextColor;
}

void UIElementConfigImpl::OnInitialized()
{
  UIElementConfigManager::Get().Init(*this);
}

} // namespace Integration

} // namespace UI

} // namespace Dali
