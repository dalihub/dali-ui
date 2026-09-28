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
#include <dali-ui-components/integration-api/dialog/dialog-impl.h>
#include <dali-ui-components/internal/dialog/dialog-presentation-session.h>

// EXTERNAL INCLUDES
#include <dali-ui-foundation/public-api/layouts/stack-layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/render-effects/background-blur-effect.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/public-api/common/unique-ptr.h>
#include <cmath>
#include <limits>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Integration
{
namespace
{
// Register the type with ViewImpl as the base so instances inherit View's
// (animatable) properties such as viewEffectiveScale, which ViewImpl::Measure
// reads for every view. Without this, measuring a Dialog throws.
BaseHandle Create()
{
  return BaseHandle();
}

DALI_TYPE_REGISTRATION_BEGIN(DialogImpl, ViewImpl, Create)
DALI_TYPE_REGISTRATION_END()
} // anonymous namespace

Ui::Dialog DialogImpl::New()
{
  return New(Ui::DialogStyle::Default());
}

Ui::Dialog DialogImpl::New(Ui::DialogStyle style)
{
  DALI_ASSERT_ALWAYS(style && "Dialog style must be initialized");
  IntrusivePtr<DialogImpl> impl = new DialogImpl();

  Ui::Dialog handle = Ui::Dialog(*impl);

  impl->Initialize();
  impl->ApplyInitialStyle(style);

  return handle;
}

DialogImpl::DialogImpl()
: ViewImpl(), mPresentation(new Internal::DialogPresentationData())
{
}

DialogImpl::~DialogImpl()
{
}

bool DialogImpl::Post(Ui::Navigator navigator, const DialogPostOptions& options)
{
  DALI_ASSERT_ALWAYS(navigator && "Dialog Post requires an initialized Navigator");
  auto current = mPresentation->current;
  if(current)
  {
    return current->state == Internal::DialogPresentationSession::State::REGISTERED &&
           current->navigator.GetHandle() == navigator;
  }
  Ui::Dialog dialog = Ui::Dialog::DownCast(Self());
  DALI_ASSERT_ALWAYS(!dialog.GetParent() && "Dialog::Post cannot be mixed with direct Add/Remove");
  DALI_ASSERT_ALWAYS(mPresentation->nextId != std::numeric_limits<uint64_t>::max());
  // Reserve the preparing session before style resolution can invoke app code.
  // A nested Post must not register another container for this same Dialog.
  auto session = std::make_shared<Internal::DialogPresentationSession>(++mPresentation->nextId, dialog, Ui::DialogContainer(), navigator);
  mPresentation->current = session;
  auto container = Ui::DialogContainer::New(options.containerStyle ? options.containerStyle : Ui::DialogContainerStyle::Default());
  session->container = WeakHandle<Ui::DialogContainer>(container);
  if(!session->Register(options))
  {
    session->Complete(DialogDismissReason::PROGRAMMATIC);
    return false;
  }
  return true;
}

void DialogImpl::Dismiss(bool animated)
{
  auto session = mPresentation->current;
  if(session)
  {
    session->Dismiss(animated, DialogDismissReason::PROGRAMMATIC);
  }
  else
  {
    DALI_ASSERT_ALWAYS(!Self().GetParent() && "Dialog::Dismiss cannot be mixed with direct Add/Remove");
  }
}

bool DialogImpl::IsPosted() const
{
  auto session = mPresentation->current;
  return session && (session->state == Internal::DialogPresentationSession::State::REGISTERED ||
                     session->state == Internal::DialogPresentationSession::State::CLOSING);
}

void DialogImpl::SetDismissPolicy(DialogDismissPolicy policy)
{
  DALI_ASSERT_ALWAYS(static_cast<uint32_t>(policy) <= 3u && "Invalid dialog dismiss policy");
  mPresentation->policy = policy;
}

DialogDismissPolicy DialogImpl::GetDismissPolicy() const
{
  return mPresentation->policy;
}

Ui::Dialog::DismissRequestedSignalType& DialogImpl::DismissRequestedSignal()
{
  return mPresentation->requested;
}

Ui::Dialog::ShownSignalType& DialogImpl::ShownSignal()
{
  return mPresentation->shown;
}

Ui::Dialog::HiddenSignalType& DialogImpl::HiddenSignal()
{
  return mPresentation->hidden;
}

void DialogImpl::ApplyInitialStyle(Ui::DialogStyle style)
{
  DALI_ASSERT_ALWAYS(style && "Dialog style must be initialized");
  Ui::View self = Ui::View::DownCast(Self());
  self.SetRequestedWidth(style.GetRequestedWidth());
  self.SetRequestedHeight(style.GetRequestedHeight());
  self.SetPadding(style.GetPadding());
  // Preserve an explicitly requested empty background.
  if(style.GetBackgroundColor() != UiColor(0x000000u, 0.0f))
  {
    self.SetBackgroundColor(style.GetBackgroundColor());
  }
  self.SetCornerRadius(style.GetCornerRadius());
  self.SetCornerRadiusPolicy(style.GetCornerRadiusPolicy());
  self.SetBorderlineWidth(style.GetBorderlineWidth());
  self.SetBorderlineOffset(style.GetBorderlineOffset());
  self.SetBorderlineColor(style.GetBorderlineColor());
  if(style.GetShadow().GetShadowCount() > 0u)
  {
    self.SetShadow(style.GetShadow());
  }
  if(style.GetBackgroundBlurRadius() > 0.0f)
  {
    self.SetRenderEffect(BackgroundBlurEffect::New(static_cast<uint32_t>(style.GetBackgroundBlurRadius())));
  }
  SetSpacing(style.GetSpacing());
}

void DialogImpl::OnInitialize()
{
  ViewImpl::OnInitialize();
  Ui::View::DownCast(Self()).SetAccessibilityRole(Ui::Accessibility::Role::DIALOG);

  // The dialog stacks its sections vertically.
  AttachLayoutManager(Dali::MakeUnique<StackLayoutManager>(StackOrientation::VERTICAL, 0.0f));
}

void DialogImpl::SetHeaderView(Ui::View headerView)
{
  if(mHeaderView == headerView)
  {
    return;
  }
  DetachIfParented(mHeaderView);
  mHeaderView = headerView;
  RebuildOrder();
}

Ui::View DialogImpl::GetHeaderView() const
{
  return mHeaderView;
}

void DialogImpl::SetBodyView(Ui::View bodyView)
{
  if(mBodyView == bodyView)
  {
    return;
  }
  DetachIfParented(mBodyView);
  mBodyView = bodyView;
  RebuildOrder();
}

Ui::View DialogImpl::GetBodyView() const
{
  return mBodyView;
}

void DialogImpl::SetFooterView(Ui::View footerView)
{
  if(mFooterView == footerView)
  {
    return;
  }
  DetachIfParented(mFooterView);
  mFooterView = footerView;
  RebuildOrder();
}

Ui::View DialogImpl::GetFooterView() const
{
  return mFooterView;
}

void DialogImpl::SetSpacing(float spacing)
{
  DALI_ASSERT_ALWAYS(std::isfinite(spacing) && spacing >= 0.0f && "Dialog spacing must be finite and non-negative");
  auto* manager = static_cast<StackLayoutManager*>(GetLayoutManager());
  if(manager && manager->GetSpacing() != spacing)
  {
    manager->SetSpacing(spacing);
    InvalidateMeasure();
  }
}

float DialogImpl::GetSpacing() const
{
  auto* manager = static_cast<StackLayoutManager*>(GetLayoutManager());
  return manager ? manager->GetSpacing() : 0.0f;
}

void DialogImpl::SetLayoutAlignment(LayoutAlignment alignment)
{
  if(mAlignment == alignment)
  {
    return;
  }
  mAlignment = alignment;
  ApplyAlignment(mHeaderView);
  ApplyAlignment(mBodyView);
  ApplyAlignment(mFooterView);
  InvalidateMeasure();
}

LayoutAlignment DialogImpl::GetLayoutAlignment() const
{
  return mAlignment;
}

void DialogImpl::RebuildOrder()
{
  // Detach all current sections, then re-add them in a fixed order so the
  // visual order is always header -> body -> footer regardless of set order.
  DetachIfParented(mHeaderView);
  DetachIfParented(mBodyView);
  DetachIfParented(mFooterView);

  AddSection(mHeaderView);
  AddSection(mBodyView);
  AddSection(mFooterView);
}

void DialogImpl::AddSection(Ui::View view)
{
  if(!view)
  {
    return;
  }
  ApplyAlignment(view);
  Self().Add(view);
}

void DialogImpl::DetachIfParented(Ui::View view)
{
  if(view && view.GetParent() == Self())
  {
    Self().Remove(view);
  }
}

void DialogImpl::ApplyAlignment(Ui::View view)
{
  if(!view)
  {
    return;
  }
  StackLayoutParams params;
  view.TryGetLayoutParams(params);
  params.SetAlignment(mAlignment);
  view.SetLayoutParams(params);
}

} // namespace Integration
} // namespace Ui
} //namespace DALI_NAMESPACE
