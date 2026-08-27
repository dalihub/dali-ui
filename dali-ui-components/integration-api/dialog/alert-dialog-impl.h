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
 *
 */

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-string.h>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/dialog/dialog-impl.h>
#include <dali-ui-components/public-api/dialog/alert-dialog.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

/**
 * @brief Implementation class for Ui::AlertDialog.
 *
 * Derives from DialogImpl and auto-builds the header (title), body (message) and
 * footer (action buttons) sections.
 */
class DALI_UI_COMPONENTS_API AlertDialogImpl : public DialogImpl
{
public:
  static Ui::AlertDialog New();

  void           SetTitle(const Dali::String& title);
  Dali::String   GetTitle() const;
  void           SetMessage(const Dali::String& message);
  Dali::String   GetMessage() const;
  Ui::TextButton AddActionButton(const Dali::String& text);
  void           ClearActionButtons();

protected:
  AlertDialogImpl();
  virtual ~AlertDialogImpl();

private:
  AlertDialogImpl(const AlertDialogImpl&)            = delete;
  AlertDialogImpl(AlertDialogImpl&&)                 = delete;
  AlertDialogImpl& operator=(const AlertDialogImpl&) = delete;
  AlertDialogImpl& operator=(AlertDialogImpl&&)      = delete;

private:
  Dali::String mTitle;
  Dali::String mMessage;
  Ui::View     mActionButtonRow;
};

} // namespace Integration

inline Integration::AlertDialogImpl& GetImpl(Ui::AlertDialog& alertDialog)
{
  DALI_ASSERT_ALWAYS(alertDialog);
  Dali::RefObject& handle = alertDialog.GetImplementation();
  return static_cast<Integration::AlertDialogImpl&>(handle);
}

inline const Integration::AlertDialogImpl& GetImpl(const Ui::AlertDialog& alertDialog)
{
  DALI_ASSERT_ALWAYS(alertDialog);
  const Dali::RefObject& handle = alertDialog.GetImplementation();
  return static_cast<const Integration::AlertDialogImpl&>(handle);
}

} // namespace Ui
} // namespace Dali
