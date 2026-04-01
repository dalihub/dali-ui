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

#include <dali-ui-foundation/devel-api/ui-foundation-pre-initialize.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/application-devel.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/public-api/image-view/image-view.h>
#include <dali-ui-foundation/public-api/label.h>
#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali-ui-foundation/public-api/view.h>

namespace Dali::Ui
{
namespace
{
bool gPreloaded = false;
void Preload()
{
  // This function called at pre-initialize time, after ApplicationPreInitialize().
  // If adaptor exist now, we could create view at pre-initialize time.
  if(Dali::Adaptor::IsAvailable())
  {
    DALI_LOG_RELEASE_INFO("IsSupportPreInitializedCreation() : true\n");

    [[maybe_unused]] auto view      = View::New().SetBackgroundColor(UiColor(0xFFFFFF));
    [[maybe_unused]] auto imageView = ImageView::New();
    [[maybe_unused]] auto label     = Label::New("text");
  }
  else
  {
    DALI_LOG_RELEASE_INFO("IsSupportPreInitializedCreation() : false\n");
  }
}

} // namespace
} // namespace Dali::Ui

extern "C" void DaliUiFoundationPreInitialize(int* argc, char** argv[], void* /* not in used */)
{
  if(!Dali::Ui::gPreloaded)
  {
    Dali::Ui::gPreloaded = true;
    ApplicationPreInitialize(argc, argv);
    Dali::Ui::Preload();
  }
  else
  {
    DALI_LOG_ERROR("DaliUiFoundationPreInitialize() called multiple. Ignore\n");
  }
}