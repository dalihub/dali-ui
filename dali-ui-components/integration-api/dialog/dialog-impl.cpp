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

// EXTERNAL INCLUDES
#include <dali-ui-foundation/public-api/layouts/stack-layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/render-effects/background-blur-effect.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/public-api/common/unique-ptr.h>
#include <cmath>

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
: ViewImpl()
{
}

DialogImpl::~DialogImpl()
{
}

void DialogImpl::ApplyInitialStyle(Ui::DialogStyle style)
{
  DALI_ASSERT_ALWAYS(style && "Dialog style must be initialized");
  Ui::View self = Ui::View::DownCast(Self());
  self.SetRequestedWidth(style.GetRequestedWidth());
  self.SetRequestedHeight(style.GetRequestedHeight());
  self.SetPadding(style.GetPadding());
  // Keep an empty background on the legacy transparent default path.
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
