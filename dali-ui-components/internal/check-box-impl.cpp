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
#include <dali-ui-components/internal/check-box-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/public-api/actors/actor-enumerations.h> // Dali::LayoutDirection
#include <algorithm>
#include <string>
#include <utility> // std::swap

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/configuration/ui-theme-manager.h>
#include <dali-ui-foundation/public-api/views/view-impl.h> // public GetImpl(Ui::View&)

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{
BaseHandle Create()
{
  return BaseHandle();
}

DALI_TYPE_REGISTRATION_BEGIN(CheckBoxImpl, Extension::SelectableViewImpl, Create)
DALI_TYPE_REGISTRATION_END()

// The DEFAULT asset (checkbox.json) exposes two markers: "on" (frames 0..18) and "off"
// (frames 20..38). The default select plays [0,19] (the +1 lands on the settled checked frame)
// and deselect plays [20,38]. The style's icon generator
// builds the selectable image (a SelectableImageInterface that composes the Lottie glyph); the image
// owns the frame-range playback and the state-driven inner-fill recolour internally.

} // namespace

Ui::CheckBox CheckBoxImpl::New(CheckBoxStyle style)
{
  DALI_ASSERT_ALWAYS(style && "CheckBoxStyle must be initialized");
  IntrusivePtr<CheckBoxImpl> impl(new CheckBoxImpl());
  Ui::CheckBox               handle(*impl);
  impl->Initialize();
  impl->ApplyInitialStyle(style);
  return handle;
}

void CheckBoxImpl::SetText(const Dali::String& text)
{
  mText = text;
  mLabel.SetText(text);

  InvalidateMeasure(); // label presence changes the measured size
}

Dali::String CheckBoxImpl::GetText() const
{
  return mText;
}

void CheckBoxImpl::SetSelectionAnimationMode(SelectionAnimationMode mode)
{
  mSelectionAnimationMode = mode;
}

SelectionAnimationMode CheckBoxImpl::GetSelectionAnimationMode() const
{
  return mSelectionAnimationMode;
}

void CheckBoxImpl::SetIconWidth(float width)
{
  mIconWidth = width;
  InvalidateMeasure(); // the glyph column width changes the measured size
}

float CheckBoxImpl::GetIconWidth() const
{
  return mIconWidth;
}

void CheckBoxImpl::SetIconHeight(float height)
{
  mIconHeight = height;
  InvalidateMeasure(); // the glyph row height changes the measured size
}

float CheckBoxImpl::GetIconHeight() const
{
  return mIconHeight;
}

void CheckBoxImpl::SetTextColor(const UiColor& color)
{
  mLabel.SetTextColor(color);
}

UiColor CheckBoxImpl::GetTextColor() const
{
  return const_cast<Ui::Label&>(mLabel).GetTextColor();
}

void CheckBoxImpl::SetFontSize(float fontSize)
{
  mLabel.SetFontSize(fontSize);
}

float CheckBoxImpl::GetFontSize() const
{
  return mLabel.GetFontSize();
}

void CheckBoxImpl::SetFontFamily(const Dali::String& fontFamily)
{
  mLabel.SetFontFamily(fontFamily);
}

Dali::String CheckBoxImpl::GetFontFamily() const
{
  return mLabel.GetFontFamily();
}

void CheckBoxImpl::SetTextUnderline(const Text::Underline& underline)
{
  mUnderline = underline;
  mLabel.SetTextUnderline(underline);
}

Text::Underline CheckBoxImpl::GetTextUnderline() const
{
  return mUnderline;
}

void CheckBoxImpl::OnInitialize()
{
  Ui::Extension::SelectableViewImpl::OnInitialize(); // base first (attaches the SelectableTrait)

  Ui::View self = Ui::View::DownCast(Self());

  // The CheckBox root represents the checkbox name, state, and actions. Without an
  // explicit role, the default NONE excludes it from Screen Reader highlighting, so the
  // component provides CHECK_BOX by default. When AsGroupSelectable() is applied, that
  // trait changes the role to RADIO_BUTTON and restores CHECK_BOX when the group is removed.
  self.SetAccessibilityRole(Accessibility::Role::CHECK_BOX);

  // Clip children to the control box.
  Self().SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);

  // The Lottie glyph (mIcon) needs the url + frame ranges from the style, so it is created
  // in ApplyInitialStyle(); OnInitialize() only sets up the label and the selection/theme
  // wiring that does not depend on the glyph.

  // Optional trailing label (empty until SetText()), vertically centered against the icon.
  mLabel = Ui::Label::New();
  mLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);

  // The displayed Label is an internal child used to render the root CheckBox. The root
  // supplies its text as a name fallback, so exposing the Label in the tree would cause
  // the same item to be announced twice, as TEXT and CHECK_BOX. Hide it only from accessibility.
  mLabel.SetAccessibilityHidden(true);
  self.Add(mLabel);

  // React to selection changes (no virtual OnSelectedChanged hook exists on the base).
  SelectionChangedSignal().Connect(this, &CheckBoxImpl::OnSelectionChanged);

  // The visual DISABLED state updated by View::SetEnabled() and the accessibility ENABLED
  // state are not connected automatically. Observe state transitions and update both so
  // they share the same source of truth, including through the component's inherited enabled API.
  self.StateChangedSignal().Connect(this, &CheckBoxImpl::OnViewStateChanged);
  UiThemeManager::Get().ThemeChangedSignal().Connect(this, &CheckBoxImpl::OnThemeChanged);
}

void CheckBoxImpl::ApplyInitialStyle(CheckBoxStyle style)
{
  Ui::View self = Ui::View::DownCast(Self());
  self.SetMinimumWidth(style.GetMinimumWidth());
  self.SetMinimumHeight(style.GetMinimumHeight());
  self.SetPadding(style.GetPadding());
  self.SetStateEffect(style.GetStateEffect());

  mIconWidth  = style.GetIconWidth();
  mIconHeight = style.GetIconHeight();
  mGap        = style.GetLabelGap();

  mIconColor         = style.GetIconColor();
  mSelectedIconColor = style.GetSelectedIconColor();

  // The style's icon generator builds the selectable image (url + explicit integer frame
  // ranges are baked into it); the image owns the frame-range playback and the per-frame
  // inner-fill recolour. GetView() is the scene view the image draws into.
  mIcon = style.CreateIcon();
  DALI_ASSERT_ALWAYS(mIcon && "CheckBox icon generator returned an empty SelectableImageInterface");
  Ui::View iconView = mIcon.GetView();

  // The Lottie icon is also an internal implementation detail that represents the checkbox's
  // visual state. Do not expose it as a separate accessibility target; convey its meaning
  // through the root's CHECKED state.
  iconView.SetAccessibilityHidden(true);
  Self().Add(iconView);
  mIcon.TransitionFinishedSignal().Connect(this, &CheckBoxImpl::OnAnimationFinished);

  // Confine press/focus feedback to the glyph only: the interactive area stays the whole
  // CheckBox (icon + label), but the overlay effect targets the icon so the label area
  // (and any other region) does not animate on touch press/release.
  self.SetStateEffectTarget(iconView);

  // Push the resolved state colours into the glyph, then seat the initial resting frame.
  PushStateColors();

  // Push the label text style through the runtime setters (mirrors TextButtonImpl::ApplyInitialStyle).
  SetTextColor(style.GetTextColor());
  SetFontSize(style.GetFontSize());
  SetFontFamily(style.GetFontFamily());
  SetTextUnderline(style.GetTextUnderline());

  RefreshRestingFrame();
}

void CheckBoxImpl::OnSelectionChanged(View /*view*/, bool selected, InputEvent event)
{
  // Selection authority stays here; the glyph view only renders the requested state.
  mIcon.SetSelected(selected, IsSelectionAnimationRequired(event));

  Ui::View self = Ui::View::DownCast(Self());

  // Update the logical selection and accessibility CHECKED state in the same commit. If
  // only the visual icon changes, Screen Reader continues to announce the item as unchecked.
  // The Add/Remove APIs also deliver state-change events through ViewAccessible and avoid
  // duplicate changes when a radio group has already applied the same value.
  if(selected)
  {
    self.AddAccessibilityState(Accessibility::State::CHECKED);
  }
  else
  {
    self.RemoveAccessibilityState(Accessibility::State::CHECKED);
  }
}

void CheckBoxImpl::OnViewStateChanged(Ui::View view, StateEvent event)
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

bool CheckBoxImpl::OnAccessibilityRequestDefaultName(Dali::String& value)
{
  // The default-name hook is invoked only when the application has not set an explicit
  // value with SetAccessibilityName(). This preserves the component contract: the displayed
  // checkbox text is the default name, but a more specific application-provided name wins.
  // After SetText(), the next Screen Reader query uses the current mText. For an unlabeled
  // checkbox, return false to allow the framework's next fallback or an application-supplied name.
  value = mText;
  return !value.Empty();
}

bool CheckBoxImpl::IsSelectionAnimationRequired(const InputEvent& event) const
{
  if(mSelectionAnimationMode == SelectionAnimationMode::DISABLED)
  {
    return false;
  }
  if(!(Self().GetProperty<bool>(Dali::Actor::Property::CONNECTED_TO_SCENE) && Self().IsVisible())) // animatable only when on-scene and visible
  {
    return false;
  }
  if(mSelectionAnimationMode == SelectionAnimationMode::ENABLED)
  {
    return true;
  }
  // AUTO: animate only user-initiated changes; programmatic SetSelected snaps.
  return !event.IsProgrammatic();
}

void CheckBoxImpl::RefreshRestingFrame()
{
  if(mIcon.IsTransitioning())
  {
    return; // don't disturb a running segment
  }
  mIcon.SetSelected(IsSelected(), false); // snap to the correct resting frame + range
}

void CheckBoxImpl::PushStateColors()
{
  // Resolve the deselected/selected tokens against the current theme and push the RGBA into
  // the glyph view, which seats them on its inner-fill recolour callback.
  mIcon.SetStateColors(mIconColor.GetRgba(), mSelectedIconColor.GetRgba());
}

void CheckBoxImpl::OnThemeChanged()
{
  // Re-resolve the tokens and push them into the glyph.
  if(mIcon.IsTransitioning())
  {
    mThemeRefreshPending = true; // apply when the current segment finishes
    return;
  }
  PushStateColors();
  RefreshRestingFrame();
}

void CheckBoxImpl::OnAnimationFinished(View /*view*/)
{
  if(mThemeRefreshPending)
  {
    PushStateColors();
    mThemeRefreshPending = false;
  }
  RefreshRestingFrame(); // re-assert the resting frame after playback
}

void CheckBoxImpl::OnSceneConnection(int depth)
{
  Ui::Extension::SelectableViewImpl::OnSceneConnection(depth); // base first
  if(mThemeRefreshPending)
  {
    // A theme change arrived while off-scene (deferred); apply the resolved colours now.
    PushStateColors();
    mThemeRefreshPending = false;
  }
  RefreshRestingFrame(); // show the correct resting frame on (re-)attach
}

MeasuredSize CheckBoxImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  float s = GetEffectiveScale();

  Insets padding = GetPadding();
  float  visPadW = static_cast<float>(padding.start + padding.end) * s;
  float  visPadH = static_cast<float>(padding.top + padding.bottom) * s;

  float gapVis   = mGap * s;
  bool  hasLabel = !mText.Empty();

  float requestedWidth  = GetRequestedWidth();
  float requestedHeight = GetRequestedHeight();
  float requestedVisW   = (requestedWidth >= 0.0f) ? requestedWidth * s : requestedWidth;
  float requestedVisH   = (requestedHeight >= 0.0f) ? requestedHeight * s : requestedHeight;

  float effectiveVisW = (requestedVisW >= 0.0f) ? requestedVisW : widthConstraint;
  float effectiveVisH = (requestedVisH >= 0.0f) ? requestedVisH : heightConstraint;
  float contentVisW   = (effectiveVisW >= 0.0f) ? std::max(0.0f, effectiveVisW - visPadW) : effectiveVisW;
  float contentVisH   = (effectiveVisH >= 0.0f) ? std::max(0.0f, effectiveVisH - visPadH) : effectiveVisH;

  // No fixed icon height: take it from the content height only when the height axis is
  // definite (an explicit request or MATCH_PARENT). On a WRAP height axis the budget is
  // indefinite, so it falls back to the minimum height (or zero) instead of the budget.
  bool  definiteHeight = (requestedVisH >= 0.0f) || (requestedHeight == MATCH_PARENT);
  float iconHVis       = mIconHeight * s;
  if(mIconHeight <= 0.0f)
  {
    if(definiteHeight && contentVisH >= 0.0f)
    {
      iconHVis = contentVisH;
    }
    else
    {
      float m  = GetMinimumHeight() * s;
      iconHVis = (m > 0.0f) ? m : 0.0f;
    }
  }
  // An unset icon width defaults to the resolved icon height (square glyph).
  float iconWVis = (mIconWidth > 0.0f) ? (mIconWidth * s) : iconHVis;

  float labelW = 0.0f;
  float labelH = 0.0f;
  if(hasLabel)
  {
    float        labelAvailW = (contentVisW >= 0.0f) ? std::max(0.0f, contentVisW - iconWVis - gapVis) : contentVisW;
    MeasuredSize labelSize   = GetImpl(mLabel).Measure(labelAvailW, contentVisH);
    labelW                   = labelSize.width;
    labelH                   = labelSize.height;
  }

  float naturalW = iconWVis + (hasLabel ? gapVis + labelW : 0.0f);
  float naturalH = std::max(iconHVis, labelH);

  float resultVisW = 0.0f;
  if(requestedVisW >= 0.0f)
  {
    resultVisW = requestedVisW;
  }
  else if(requestedWidth == MATCH_PARENT)
  {
    resultVisW = GetMinimumWidth() * s;
  }
  else
  {
    resultVisW = naturalW + visPadW;
  }

  float resultVisH = 0.0f;
  if(requestedVisH >= 0.0f)
  {
    resultVisH = requestedVisH;
  }
  else if(requestedHeight == MATCH_PARENT)
  {
    resultVisH = GetMinimumHeight() * s;
  }
  else
  {
    resultVisH = naturalH + visPadH;
  }

  return MeasuredSize(resultVisW, resultVisH);
}

LayoutRect CheckBoxImpl::OnArrange(const LayoutRect& bounds)
{
  // Self geometry (x/y/width/height) is applied centrally in ViewImpl::Arrange
  // from this method's returned LayoutRect; no actor-level setters needed here.
  float  s       = GetEffectiveScale();
  Insets padding = GetPadding();

  // Under RTL we mirror the layout ourselves: swap start/end padding, and flip each child's
  // x within the content band (mapX below). The Lottie artwork is direction-independent and is
  // NOT mirrored; only the icon/label placement flips (icon leading, label trailing in both).
  const bool rtl = (Self().GetEffectiveLayoutDirection() == Dali::LayoutDirection::RIGHT_TO_LEFT);
  if(rtl)
  {
    std::swap(padding.start, padding.end);
  }

  float contentX = static_cast<float>(padding.start) * s;
  float contentY = static_cast<float>(padding.top) * s;
  float contentW = std::max(0.0f, bounds.width - static_cast<float>(padding.start + padding.end) * s);
  float contentH = std::max(0.0f, bounds.height - static_cast<float>(padding.top + padding.bottom) * s);

  // No fixed icon height => the icon fills the content height; an unset width defaults to that
  // resolved height (square glyph). The app sizes the control.
  float iconHVis = (mIconHeight > 0.0f) ? (mIconHeight * s) : contentH;
  float iconWVis = (mIconWidth > 0.0f) ? (mIconWidth * s) : iconHVis;
  float gapVis   = mGap * s;

  // Map a left-anchored local x within the content band to the arranged x, mirroring under RTL.
  auto mapX = [rtl, contentX, contentW](float lx, float w)
  {
    return rtl ? contentX + (contentW - lx - w) : contentX + lx;
  };

  // Icon (leading, vertically centered).
  LayoutRect iconRect;
  iconRect.width  = iconWVis;
  iconRect.height = iconHVis;
  iconRect.x      = mapX(0.0f, iconWVis);
  iconRect.y      = contentY + std::max(0.0f, (contentH - iconHVis) * 0.5f);

  Ui::View iconView = mIcon.GetView(); // the composed drawing view; use public GetImpl(Ui::View&)
  GetImpl(iconView).Measure(iconRect.width, iconRect.height);
  GetImpl(iconView).Arrange(iconRect);

  // Optional trailing label.
  LayoutRect labelRect;
  labelRect.width  = mText.Empty() ? 0.0f : std::max(0.0f, contentW - iconWVis - gapVis);
  labelRect.height = contentH;
  labelRect.x      = mapX(iconWVis + gapVis, labelRect.width);
  labelRect.y      = contentY;
  GetImpl(mLabel).Measure(labelRect.width, labelRect.height);
  GetImpl(mLabel).Arrange(labelRect);

  return bounds;
}

CheckBoxImpl::CheckBoxImpl() = default;

CheckBoxImpl::~CheckBoxImpl() = default;

} // namespace Internal
} // namespace Ui
} // namespace Dali
