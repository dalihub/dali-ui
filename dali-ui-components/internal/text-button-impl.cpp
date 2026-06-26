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
#include <dali-ui-foundation/public-api/text/text-enumerations.h>
#include <dali-ui-foundation/public-api/view-impl.h>

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

DALI_TYPE_REGISTRATION_BEGIN(TextButtonImpl, Provider::InteractiveViewImpl, Create)
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
  impl->ApplyStyle(style);
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
  mUnderline        = underline;
  mUnderlineEnabled = (underline != Text::Underline::None());
  mLabel.SetTextUnderline(underline);
}

bool TextButtonImpl::IsTextUnderlineEnabled() const
{
  return mUnderlineEnabled;
}

Text::Underline TextButtonImpl::GetTextUnderline() const
{
  return mUnderlineEnabled ? mUnderline : Text::Underline::None();
}

void TextButtonImpl::OnInitialize()
{
  Provider::InteractiveViewImpl::OnInitialize();

  mLabel = Ui::Label::New();
  Self().Add(mLabel);

  ApplyAlignment();
}

MeasuredSize TextButtonImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  float s = GetEffectiveScale();

  Extents padding = GetPadding();
  float   visPadW = static_cast<float>(padding.start + padding.end) * s;
  float   visPadH = static_cast<float>(padding.top + padding.bottom) * s;

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

MeasuredSize TextButtonImpl::OnArrange(const LayoutRect& bounds)
{
  Actor self = Self();
  self.SetProperty(Actor::Property::POSITION_X, bounds.x);
  self.SetProperty(Actor::Property::POSITION_Y, bounds.y);
  self.SetProperty(Actor::Property::SIZE_WIDTH, bounds.width);
  self.SetProperty(Actor::Property::SIZE_HEIGHT, bounds.height);

  float   s       = GetEffectiveScale();
  Extents padding = GetPadding();

  LayoutRect contentBounds;
  contentBounds.x      = static_cast<float>(padding.start) * s;
  contentBounds.y      = static_cast<float>(padding.top) * s;
  contentBounds.width  = std::max(0.0f, bounds.width - static_cast<float>(padding.start + padding.end) * s);
  contentBounds.height = std::max(0.0f, bounds.height - static_cast<float>(padding.top + padding.bottom) * s);

  GetImpl(mLabel).Measure(contentBounds.width, contentBounds.height);
  GetImpl(mLabel).Arrange(contentBounds);

  return MeasuredSize(bounds.width, bounds.height);
}

TextButtonImpl::TextButtonImpl() = default;

TextButtonImpl::~TextButtonImpl() = default;

void TextButtonImpl::ApplyStyle(TextButtonStyle style)
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

  if(style.IsTextUnderlineEnabled())
  {
    SetTextUnderline(style.GetTextUnderline());
  }
  else
  {
    SetTextUnderline(Text::Underline::None());
  }
}

void TextButtonImpl::ApplyAlignment()
{
  mLabel.SetHorizontalTextAlignment(ToTextAlignment(mHorizontalAlignment));
  mLabel.SetVerticalTextAlignment(ToTextAlignment(mVerticalAlignment));
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
