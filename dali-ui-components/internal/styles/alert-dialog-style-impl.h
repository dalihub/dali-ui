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
 */

#include <dali-ui-components/public-api/styles/alert-dialog-style.h>
#include <dali-ui-foundation/extension-api/styles/ui-style-impl.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
TextButtonStyle ResolveAlertActionButtonStyle(AlertDialogStyle style);

class AlertDialogStyleImpl : public Extension::UiStyleImpl
{
public:
  struct Values
  {
    DialogStyle     mDialogStyle{DialogStyle()};
    UiColor         mTitleTextColor{UiColor(0x202124u)};
    float           mTitleFontSize{22.0f};
    Dali::String    mTitleFontFamily{Dali::String()};
    UiColor         mMessageTextColor{UiColor(0x5F6368u)};
    float           mMessageFontSize{16.0f};
    Dali::String    mMessageFontFamily{Dali::String()};
    TextButtonStyle mActionButtonStyle{TextButtonStyle()};
    float           mActionRowHeight{64.0f};
    float           mActionButtonSpacing{8.0f};
  } values;

  AlertDialogStyleImpl() = default;
  AlertDialogStyleImpl(const AlertDialogStyleImpl& rhs)
  : Extension::UiStyleImpl(),
    values(rhs.values)
  {
  }

protected:
  ~AlertDialogStyleImpl() override = default;
};
} // namespace Internal
} // namespace Ui
} // namespace DALI_NAMESPACE
