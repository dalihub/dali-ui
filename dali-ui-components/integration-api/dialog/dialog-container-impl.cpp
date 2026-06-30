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
#include <dali-ui-components/integration-api/dialog/dialog-container-impl.h>

// EXTERNAL INCLUDES
#include <dali-ui-foundation/public-api/layouts/absolute-layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/interactive-view.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/public-api/common/unique-ptr.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{
namespace
{
// Register the type with ViewImpl as the base so instances inherit View's
// (animatable) properties such as viewEffectiveScale, which ViewImpl::Measure
// reads for every view. Without this, measuring a DialogContainer throws.
BaseHandle Create()
{
  return BaseHandle();
}

DALI_TYPE_REGISTRATION_BEGIN(DialogContainerImpl, ViewImpl, Create)
DALI_TYPE_REGISTRATION_END()
} // anonymous namespace

Ui::DialogContainer DialogContainerImpl::New()
{
  IntrusivePtr<DialogContainerImpl> impl = new DialogContainerImpl();

  Ui::DialogContainer handle = Ui::DialogContainer(*impl);

  impl->Initialize();

  return handle;
}

DialogContainerImpl::DialogContainerImpl()
: ViewImpl()
{
}

DialogContainerImpl::~DialogContainerImpl()
{
}

void DialogContainerImpl::OnInitialize()
{
  ViewImpl::OnInitialize();

  // Absolute layout so the scrim (MATCH_PARENT) fills the container and the
  // modal content can be positioned/sized freely above it.
  AttachLayoutManager(Dali::MakeUnique<AbsoluteLayoutManager>());

  CreateDefaultScrim();
}

void DialogContainerImpl::CreateDefaultScrim()
{
  InteractiveView scrim = InteractiveView::New();
  scrim.SetBackgroundColor(UiColor(0x000000u, 0.5f)); // semi-transparent dim
  scrim.SetRequestedWidth(MATCH_PARENT);
  scrim.SetRequestedHeight(MATCH_PARENT);
  scrim.ConnectClickedSignal(this, &DialogContainerImpl::OnScrimClicked);

  mScrim = scrim;
  Self().Add(mScrim);
}

void DialogContainerImpl::SetModalContent(Ui::View modalContent)
{
  if(mModalContent == modalContent)
  {
    return;
  }
  if(mModalContent && mModalContent.GetParent() == Self())
  {
    Self().Remove(mModalContent);
  }
  mModalContent = modalContent;
  if(mModalContent)
  {
    Self().Add(mModalContent);
    mModalContent.RaiseToTop(); // keep content above the scrim
  }
}

Ui::View DialogContainerImpl::GetModalContent() const
{
  return mModalContent;
}

void DialogContainerImpl::SetScrim(Ui::View scrim)
{
  if(mScrim == scrim)
  {
    return;
  }
  if(mScrim && mScrim.GetParent() == Self())
  {
    Self().Remove(mScrim);
  }
  mScrim = scrim;
  if(mScrim)
  {
    mScrim.SetRequestedWidth(MATCH_PARENT);
    mScrim.SetRequestedHeight(MATCH_PARENT);
    Self().Add(mScrim);
    mScrim.LowerToBottom(); // keep scrim below the modal content

    // Wire dismiss if the custom scrim is interactive.
    InteractiveView interactive = InteractiveView::DownCast(mScrim);
    if(interactive)
    {
      interactive.ConnectClickedSignal(this, &DialogContainerImpl::OnScrimClicked);
    }
  }
}

Ui::View DialogContainerImpl::GetScrim() const
{
  return mScrim;
}

void DialogContainerImpl::OnScrimClicked(Ui::View /*view*/, Ui::InputEvent /*event*/)
{
  Ui::DialogContainer handle = Ui::DialogContainer::DownCast(Self());
  if(handle)
  {
    mScrimClickedSignal.Emit(handle);
  }
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
