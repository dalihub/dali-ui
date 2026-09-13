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
#include <dali-ui-components/internal/styles/alert-dialog-style-impl.h>

// EXTERNAL INCLUDES
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>

namespace DALI_NAMESPACE
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
  return New(Ui::AlertDialogStyle::Default());
}

Ui::AlertDialog AlertDialogImpl::New(Ui::AlertDialogStyle style)
{
  DALI_ASSERT_ALWAYS(style && "AlertDialog style must be initialized");
  IntrusivePtr<AlertDialogImpl> impl = new AlertDialogImpl();

  Ui::AlertDialog handle = Ui::AlertDialog(*impl);

  impl->mStyle = style;
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

void AlertDialogImpl::OnInitialize()
{
  DialogImpl::OnInitialize();
  // Preserve protected-constructor factories used by integration subclasses.
  if(!mStyle)
  {
    mStyle = Ui::AlertDialogStyle::Default();
  }
  auto dialogStyle = mStyle.GetDialogStyle();
  ApplyInitialStyle(dialogStyle ? dialogStyle : DialogStyle::Default());
  mActionStyle = Internal::ResolveAlertActionButtonStyle(mStyle);
}

void AlertDialogImpl::SetTitle(const Dali::String& title)
{
  mTitle = title;
  if(title.Empty())
  {
    SetHeaderView(Ui::View());
    mTitleLabel.Reset();
    return;
  }
  if(mTitleLabel && GetHeaderView() == mTitleLabel)
  {
    Ui::Label::DownCast(mTitleLabel).SetText(title);
    return;
  }
  Ui::Label label = Ui::Label::New(title);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetFontSize(mStyle.GetTitleFontSize());
  label.SetTextColor(mStyle.GetTitleTextColor());
  if(!mStyle.GetTitleFontFamily().Empty())
  {
    label.SetFontFamily(mStyle.GetTitleFontFamily());
  }
  mTitleLabel = label;
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
    mMessageLabel.Reset();
    return;
  }
  if(mMessageLabel && GetBodyView() == mMessageLabel)
  {
    Ui::Label::DownCast(mMessageLabel).SetText(message);
    return;
  }
  Ui::Label label = Ui::Label::New(message);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetFontSize(mStyle.GetMessageFontSize());
  label.SetTextColor(mStyle.GetMessageTextColor());
  if(!mStyle.GetMessageFontFamily().Empty())
  {
    label.SetFontFamily(mStyle.GetMessageFontFamily());
  }
  mMessageLabel = label;
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
    row.SetRequestedHeight(mStyle.GetActionRowHeight());
    row.SetSpacing(mStyle.GetActionButtonSpacing());
    mActionButtonRow = row;
    SetFooterView(row);
  }

  Ui::TextButton button = Ui::TextButton::New(text, mActionStyle);
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
} //namespace DALI_NAMESPACE
