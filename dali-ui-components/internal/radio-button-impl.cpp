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
#include <dali-ui-components/internal/radio-button-impl.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/configuration/ui-theme-manager.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>

#include <algorithm>
#include <cmath>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{
float NormalizeIconDimension(float value)
{
  return std::isfinite(value) && value > 0.0f ? value : 0.0f;
}

BaseHandle Create()
{
  return BaseHandle();
}

DALI_TYPE_REGISTRATION_BEGIN(RadioButtonImpl, Extension::GroupSelectableViewImpl, Create)
DALI_TYPE_REGISTRATION_END()
} // unnamed namespace

Ui::RadioButton RadioButtonImpl::New(RadioButtonStyle style)
{
  DALI_ASSERT_ALWAYS(style && "RadioButtonStyle must be initialized");
  IntrusivePtr<RadioButtonImpl> impl(new RadioButtonImpl());
  Ui::RadioButton               handle(*impl);
  impl->Initialize();
  impl->ApplyInitialStyle(style);
  return handle;
}

void RadioButtonImpl::SetSelectionAnimationMode(SelectionAnimationMode mode)
{
  mSelectionAnimationMode = mode;
}

SelectionAnimationMode RadioButtonImpl::GetSelectionAnimationMode() const
{
  return mSelectionAnimationMode;
}

void RadioButtonImpl::SetIconWidth(float width)
{
  mIconWidth = NormalizeIconDimension(width);
  InvalidateMeasure();
}

float RadioButtonImpl::GetIconWidth() const
{
  return mIconWidth;
}

void RadioButtonImpl::SetIconHeight(float height)
{
  mIconHeight = NormalizeIconDimension(height);
  InvalidateMeasure();
}

float RadioButtonImpl::GetIconHeight() const
{
  return mIconHeight;
}

void RadioButtonImpl::OnInitialize()
{
  Ui::Extension::GroupSelectableViewImpl::OnInitialize();

  Ui::View self = Ui::View::DownCast(Self());
  self.SetAccessibilityRole(Accessibility::Role::RADIO_BUTTON);
  Self().SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);

  SelectionChangedSignal().Connect(this, &RadioButtonImpl::OnSelectionChanged);
  self.StateChangedSignal().Connect(this, &RadioButtonImpl::OnViewStateChanged);
  UiThemeManager::Get().ThemeChangedSignal().Connect(this, &RadioButtonImpl::OnThemeChanged);
}

void RadioButtonImpl::ApplyInitialStyle(RadioButtonStyle style)
{
  Ui::View self = Ui::View::DownCast(Self());
  self.SetMinimumWidth(style.GetMinimumWidth());
  self.SetMinimumHeight(style.GetMinimumHeight());
  self.SetPadding(style.GetPadding());
  self.SetStateEffect(style.GetStateEffect());

  mIconWidth           = style.GetIconWidth();
  mIconHeight          = style.GetIconHeight();
  mDeselectedIconColor = style.GetIconColor();
  mSelectedIconColor   = style.GetSelectedIconColor();

  mIcon = style.CreateIcon();
  DALI_ASSERT_ALWAYS(mIcon && "RadioButton icon generator returned an empty SelectableImageInterface");
  Ui::View iconView = mIcon.GetView();
  DALI_ASSERT_ALWAYS(iconView && "RadioButton icon generator returned a selectable image without a view");
  iconView.SetAccessibilityHidden(true);
  Self().Add(iconView);
  mIcon.TransitionFinishedSignal().Connect(this, &RadioButtonImpl::OnAnimationFinished);

  PushStateColors();
  RefreshRestingFrame();
}

void RadioButtonImpl::OnSelectionChanged(View /*view*/, bool selected, InputEvent event)
{
  mIcon.SetSelected(selected, IsSelectionAnimationRequired(event));

  Ui::View self = Ui::View::DownCast(Self());
  if(selected)
  {
    self.AddAccessibilityState(Accessibility::State::CHECKED);
  }
  else
  {
    self.RemoveAccessibilityState(Accessibility::State::CHECKED);
  }
}

void RadioButtonImpl::OnViewStateChanged(Ui::View view, StateEvent event)
{
  if(event.Added(ViewState::DISABLED))
  {
    view.RemoveAccessibilityState(Accessibility::State::ENABLED);
  }
  else if(event.Removed(ViewState::DISABLED))
  {
    view.AddAccessibilityState(Accessibility::State::ENABLED);
  }
}

bool RadioButtonImpl::IsSelectionAnimationRequired(const InputEvent& event) const
{
  if(mSelectionAnimationMode == SelectionAnimationMode::DISABLED)
  {
    return false;
  }
  if(!(Self().GetProperty<bool>(Dali::Actor::Property::CONNECTED_TO_SCENE) && Self().IsVisible()))
  {
    return false;
  }
  if(mSelectionAnimationMode == SelectionAnimationMode::ENABLED)
  {
    return true;
  }
  return !event.IsProgrammatic();
}

void RadioButtonImpl::RefreshRestingFrame()
{
  if(!mIcon.IsTransitioning())
  {
    mIcon.SetSelected(IsSelected(), false);
  }
}

void RadioButtonImpl::PushStateColors()
{
  mIcon.SetStateColors(mDeselectedIconColor.GetRgba(), mSelectedIconColor.GetRgba());
}

void RadioButtonImpl::OnThemeChanged()
{
  if(mIcon.IsTransitioning())
  {
    mThemeRefreshPending = true;
    return;
  }
  PushStateColors();
  RefreshRestingFrame();
}

void RadioButtonImpl::OnAnimationFinished(View /*view*/)
{
  if(mThemeRefreshPending)
  {
    PushStateColors();
    mThemeRefreshPending = false;
  }
  RefreshRestingFrame();
}

void RadioButtonImpl::OnSceneConnection(int depth)
{
  Ui::Extension::GroupSelectableViewImpl::OnSceneConnection(depth);
  if(mThemeRefreshPending)
  {
    PushStateColors();
    mThemeRefreshPending = false;
  }
  RefreshRestingFrame();
}

MeasuredSize RadioButtonImpl::OnMeasure(float /*widthConstraint*/, float heightConstraint)
{
  const float s = GetEffectiveScale();

  const Insets padding   = GetPadding();
  const float  padWidth  = static_cast<float>(padding.start + padding.end) * s;
  const float  padHeight = static_cast<float>(padding.top + padding.bottom) * s;

  const float requestedWidth        = GetRequestedWidth();
  const float requestedHeight       = GetRequestedHeight();
  const float requestedVisualWidth  = requestedWidth >= 0.0f ? requestedWidth * s : requestedWidth;
  const float requestedVisualHeight = requestedHeight >= 0.0f ? requestedHeight * s : requestedHeight;

  const float effectiveVisualHeight = requestedVisualHeight >= 0.0f ? requestedVisualHeight : heightConstraint;
  const float contentVisualHeight   = effectiveVisualHeight >= 0.0f
                                        ? std::max(0.0f, effectiveVisualHeight - padHeight)
                                        : effectiveVisualHeight;

  const bool definiteHeight   = requestedVisualHeight >= 0.0f || requestedHeight == MATCH_PARENT;
  float      iconVisualHeight = mIconHeight * s;
  if(mIconHeight <= 0.0f)
  {
    if(definiteHeight && contentVisualHeight >= 0.0f)
    {
      iconVisualHeight = contentVisualHeight;
    }
    else
    {
      const float minimumVisualHeight = GetMinimumHeight() * s;
      iconVisualHeight                = minimumVisualHeight > 0.0f ? minimumVisualHeight : 0.0f;
    }
  }
  const float iconVisualWidth = mIconWidth > 0.0f ? mIconWidth * s : iconVisualHeight;

  float resultVisualWidth;
  if(requestedVisualWidth >= 0.0f)
  {
    resultVisualWidth = requestedVisualWidth;
  }
  else if(requestedWidth == MATCH_PARENT)
  {
    resultVisualWidth = GetMinimumWidth() * s;
  }
  else
  {
    resultVisualWidth = iconVisualWidth + padWidth;
  }

  float resultVisualHeight;
  if(requestedVisualHeight >= 0.0f)
  {
    resultVisualHeight = requestedVisualHeight;
  }
  else if(requestedHeight == MATCH_PARENT)
  {
    resultVisualHeight = GetMinimumHeight() * s;
  }
  else
  {
    resultVisualHeight = iconVisualHeight + padHeight;
  }

  return MeasuredSize(resultVisualWidth, resultVisualHeight);
}

LayoutRect RadioButtonImpl::OnArrange(const LayoutRect& bounds)
{
  const float  s       = GetEffectiveScale();
  const Insets padding = GetPadding();

  const float contentX      = static_cast<float>(padding.start) * s;
  const float contentY      = static_cast<float>(padding.top) * s;
  const float contentWidth  = std::max(0.0f, bounds.width - static_cast<float>(padding.start + padding.end) * s);
  const float contentHeight = std::max(0.0f, bounds.height - static_cast<float>(padding.top + padding.bottom) * s);

  const float iconHeight = mIconHeight > 0.0f ? mIconHeight * s : contentHeight;
  const float iconWidth  = mIconWidth > 0.0f ? mIconWidth * s : iconHeight;

  LayoutRect iconRect;
  iconRect.width  = iconWidth;
  iconRect.height = iconHeight;
  iconRect.x      = contentX + std::max(0.0f, (contentWidth - iconWidth) * 0.5f);
  iconRect.y      = contentY + std::max(0.0f, (contentHeight - iconHeight) * 0.5f);

  Ui::View iconView = mIcon.GetView();
  GetImpl(iconView).Measure(iconRect.width, iconRect.height);
  GetImpl(iconView).Arrange(iconRect);
  return bounds;
}

RadioButtonImpl::RadioButtonImpl() = default;

RadioButtonImpl::~RadioButtonImpl() = default;

} // namespace Internal
} // namespace Ui
} // namespace Dali
