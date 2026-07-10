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
#include <dali-ui-foundation/public-api/views/interactive-view.h>
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

void AlertDialogImpl::SetActionButtons(const std::vector<std::pair<Dali::String, std::function<void()>>>& buttons)
{
  mActionHandlers.clear();

  if(buttons.empty())
  {
    SetFooterView(Ui::View());
    return;
  }

  StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
  row.SetRequestedWidth(MATCH_PARENT);
  row.SetRequestedHeight(64.0f);
  row.SetSpacing(8.0f);

  for(const auto& entry : buttons)
  {
    InteractiveView button = InteractiveView::New();
    button.SetBackgroundColor(UiColor(0x3367D6u));
    button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));

    Ui::Label label = Ui::Label::New(entry.first);
    label.SetFontSize(16.0f);
    label.SetTextColor(UiColor(0xFFFFFFu));
    label.SetRequestedPositionX(16.0f);
    label.SetRequestedPositionY(18.0f);
    button.Add(label);

    button.ConnectClickedSignal(this, &AlertDialogImpl::OnActionClicked);
    mActionHandlers.emplace_back(button, entry.second);
    row.Add(button);
  }

  SetFooterView(row);
}

void AlertDialogImpl::OnActionClicked(Ui::View view, Ui::InputEvent /*event*/)
{
  for(auto& entry : mActionHandlers)
  {
    if(entry.first == view)
    {
      if(entry.second)
      {
        entry.second();
      }
      return;
    }
  }
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
