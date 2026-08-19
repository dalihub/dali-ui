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
#include <dali-ui-components/internal/text-button-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <algorithm>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/layouts/layout-dependency-scope.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

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

DALI_TYPE_REGISTRATION_BEGIN(TextButtonImpl, Extension::InteractiveViewImpl, Create)
DALI_TYPE_REGISTRATION_END()

Text::Alignment ToTextAlignment(LayoutAlignment alignment)
{
  switch(alignment)
  {
    case LayoutAlignment::START:
      return Text::Alignment::START;
    case LayoutAlignment::END:
      return Text::Alignment::END;
    case LayoutAlignment::FILL:
    case LayoutAlignment::CENTER:
    default:
      return Text::Alignment::CENTER;
  }
}

} // namespace

Ui::TextButton TextButtonImpl::New(TextButtonStyle style)
{
  DALI_ASSERT_ALWAYS(style && "TextButtonStyle must be initialized");
  IntrusivePtr<TextButtonImpl> impl(new TextButtonImpl());
  Ui::TextButton               handle(*impl);
  impl->Initialize();
  impl->ApplyInitialStyle(style);
  return handle;
}

void TextButtonImpl::SetText(const Dali::String& text)
{
  mLabel.SetText(text);
}

Dali::String TextButtonImpl::GetText() const
{
  return mLabel.GetText();
}

void TextButtonImpl::SetHorizontalAlignment(LayoutAlignment alignment)
{
  mHorizontalAlignment = alignment;
  ApplyAlignment();
}

LayoutAlignment TextButtonImpl::GetHorizontalAlignment() const
{
  return mHorizontalAlignment;
}

void TextButtonImpl::SetVerticalAlignment(LayoutAlignment alignment)
{
  mVerticalAlignment = alignment;
  ApplyAlignment();
}

LayoutAlignment TextButtonImpl::GetVerticalAlignment() const
{
  return mVerticalAlignment;
}

void TextButtonImpl::SetTextColor(const UiColor& color)
{
  mLabel.SetTextColor(color);
}

UiColor TextButtonImpl::GetTextColor() const
{
  return const_cast<Ui::Label&>(mLabel).GetTextColor();
}

void TextButtonImpl::SetFontSize(float fontSize)
{
  mLabel.SetFontSize(fontSize);
}

float TextButtonImpl::GetFontSize() const
{
  return mLabel.GetFontSize();
}

void TextButtonImpl::SetFontFamily(const Dali::String& fontFamily)
{
  mLabel.SetFontFamily(fontFamily);
}

Dali::String TextButtonImpl::GetFontFamily() const
{
  return mLabel.GetFontFamily();
}

void TextButtonImpl::SetTextUnderline(const Text::Underline& underline)
{
  mUnderline = underline;
  mLabel.SetTextUnderline(underline);
}

Text::Underline TextButtonImpl::GetTextUnderline() const
{
  return mUnderline;
}

void TextButtonImpl::OnInitialize()
{
  Dali::Ui::Extension::InteractiveViewImpl::OnInitialize();

  Ui::View self = Ui::View::DownCast(Self());

  // A TextButton must be exposed as an actual button to Screen Reader, not merely look
  // like one. Since a View with role NONE is not accessibility-highlightable by default,
  // the component provides BUTTON on creation. Applications can still explicitly set a
  // different role later when needed.
  self.SetAccessibilityRole(Accessibility::Role::BUTTON);

  mLabel = Ui::Label::New();

  // The internal Label is a visual implementation detail of TextButton. Because the root
  // TextButton represents the name, role, and action, retaining the Label in the accessibility
  // tree would announce the same text twice, as a button and as text. Hide only the subtree
  // from accessibility, without affecting rendering.
  mLabel.SetAccessibilityHidden(true);
  self.Add(mLabel);

  // The visual DISABLED state produced by View::SetEnabled() and the accessibility ENABLED
  // state use separate storage. Update the accessibility state in the same transition to
  // prevent a mismatch where the control looks disabled but Screen Reader announces it as enabled.
  self.StateChangedSignal().Connect(this, &TextButtonImpl::OnViewStateChanged);

  ApplyAlignment();
}

bool TextButtonImpl::OnAccessibilityRequestDefaultName(Dali::String& value)
{
  // The framework invokes the default-name hook only when the value explicitly set with
  // SetAccessibilityName() is empty. The component therefore need not check the explicit
  // name itself, and its displayed text does not override an application value. After SetText(),
  // the next query returns the current Label text. Return false for empty text so the framework
  // can continue to use its next fallback, such as the Actor name.
  value = mLabel.GetText();
  return !value.Empty();
}

void TextButtonImpl::OnViewStateChanged(Ui::View view, StateEvent event)
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

void TextButtonImpl::ApplyInitialStyle(TextButtonStyle style)
{
  Ui::View self = Ui::View::DownCast(Self());
  self.SetMinimumWidth(style.GetMinimumWidth());
  self.SetMinimumHeight(style.GetMinimumHeight());
  self.SetMaximumWidth(style.GetMaximumWidth());
  self.SetMaximumHeight(style.GetMaximumHeight());
  self.SetCornerRadius(style.GetCornerRadius());
  self.SetCornerRadiusPolicy(style.GetCornerRadiusPolicy());
  self.SetPadding(style.GetPadding());
  self.SetBackgroundColor(style.GetBackgroundColor());

  SetHorizontalAlignment(style.GetHorizontalAlignment());
  SetVerticalAlignment(style.GetVerticalAlignment());
  SetTextColor(style.GetTextColor());
  SetFontSize(style.GetFontSize());
  SetFontFamily(style.GetFontFamily());
  self.SetStateEffect(style.GetStateEffect());

  SetTextUnderline(style.GetTextUnderline());
}

MeasuredSize TextButtonImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  float s = GetEffectiveScale();

  Insets padding = GetPadding();
  float  visPadW = static_cast<float>(padding.start + padding.end) * s;
  float  visPadH = static_cast<float>(padding.top + padding.bottom) * s;

  float requestedWidth  = GetRequestedWidth();
  float requestedHeight = GetRequestedHeight();
  float requestedVisW   = (requestedWidth >= 0.0f) ? requestedWidth * s : requestedWidth;
  float requestedVisH   = (requestedHeight >= 0.0f) ? requestedHeight * s : requestedHeight;

  float effectiveVisW = (requestedVisW >= 0.0f) ? requestedVisW : widthConstraint;
  float effectiveVisH = (requestedVisH >= 0.0f) ? requestedVisH : heightConstraint;
  float contentVisW   = (effectiveVisW >= 0.0f) ? std::max(0.0f, effectiveVisW - visPadW) : effectiveVisW;
  float contentVisH   = (effectiveVisH >= 0.0f) ? std::max(0.0f, effectiveVisH - visPadH) : effectiveVisH;

  MeasuredSize labelSize = GetImpl(mLabel).Measure(contentVisW, contentVisH);

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
    resultVisW = labelSize.width + visPadW;
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
    resultVisH = labelSize.height + visPadH;
  }

  return MeasuredSize(resultVisW, resultVisH);
}

LayoutRect TextButtonImpl::OnArrange(const LayoutRect& bounds)
{
  float  s       = GetEffectiveScale();
  Insets padding = GetPadding();

  LayoutRect contentBounds;
  contentBounds.x      = static_cast<float>(padding.start) * s;
  contentBounds.y      = static_cast<float>(padding.top) * s;
  contentBounds.width  = std::max(0.0f, bounds.width - static_cast<float>(padding.start + padding.end) * s);
  contentBounds.height = std::max(0.0f, bounds.height - static_cast<float>(padding.top + padding.bottom) * s);

  {
    LayoutDependency::ArrangeOwnedMeasureScope ownerScope(this);
    GetImpl(mLabel).Measure(contentBounds.width, contentBounds.height);
  }
  GetImpl(mLabel).Arrange(contentBounds);

  return bounds;
}

TextButtonImpl::TextButtonImpl() = default;

TextButtonImpl::~TextButtonImpl() = default;

void TextButtonImpl::ApplyAlignment()
{
  mLabel.SetHorizontalTextAlignment(ToTextAlignment(mHorizontalAlignment));
  mLabel.SetVerticalTextAlignment(ToTextAlignment(mVerticalAlignment));
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
