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
#include <dali-ui-components/integration-api/dialog/alert-dialog-impl.h>

// EXTERNAL INCLUDES
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{
namespace
{
// Register the type with DialogImpl as the base so the inheritance chain
// (AlertDialog -> Dialog -> View) carries View's (animatable) properties such
// as viewEffectiveScale, which ViewImpl::Measure reads for every view.
BaseHandle Create()
{
  return BaseHandle();
}

DALI_TYPE_REGISTRATION_BEGIN(AlertDialogImpl, DialogImpl, Create)
DALI_TYPE_REGISTRATION_END()
} // anonymous namespace

Ui::AlertDialog AlertDialogImpl::New()
{
  IntrusivePtr<AlertDialogImpl> impl = new AlertDialogImpl();

  Ui::AlertDialog handle = Ui::AlertDialog(*impl);

  impl->Initialize();

  return handle;
}

AlertDialogImpl::AlertDialogImpl()
: DialogImpl()
{
}

AlertDialogImpl::~AlertDialogImpl()
{
}

void AlertDialogImpl::SetTitle(const Dali::String& title)
{
  mTitle = title;
  if(title.Empty())
  {
    SetHeaderView(Ui::View());
    return;
  }
  Ui::Label label = Ui::Label::New(title);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetFontSize(22.0f);
  label.SetTextColor(UiColor(0x202124u));
  SetHeaderView(label);
}

Dali::String AlertDialogImpl::GetTitle() const
{
  return mTitle;
}

void AlertDialogImpl::SetMessage(const Dali::String& message)
{
  mMessage = message;
  if(message.Empty())
  {
    SetBodyView(Ui::View());
    return;
  }
  Ui::Label label = Ui::Label::New(message);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetFontSize(16.0f);
  label.SetTextColor(UiColor(0x5F6368u));
  SetBodyView(label);
}

Dali::String AlertDialogImpl::GetMessage() const
{
  return mMessage;
}

Ui::TextButton AlertDialogImpl::AddActionButton(const Dali::String& text)
{
  StackLayout row = StackLayout::DownCast(mActionButtonRow);
  if(!row || GetFooterView() != row)
  {
    row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(64.0f);
    row.SetSpacing(8.0f);
    mActionButtonRow = row;
    SetFooterView(row);
  }

  Ui::TextButton button = Ui::TextButton::New(text);
  button.SetBackgroundColor(UiColor(0x3367D6u));
  button.SetTextColor(UiColor(0xFFFFFFu));
  button.SetFontSize(16.0f);
  button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
  row.Add(button);

  return button;
}

void AlertDialogImpl::ClearActionButtons()
{
  SetFooterView(Ui::View());
  mActionButtonRow = Ui::View();
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
