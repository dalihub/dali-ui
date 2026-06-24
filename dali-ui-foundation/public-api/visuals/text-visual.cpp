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
#include <dali-ui-foundation/public-api/visuals/text-visual.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/property-map.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visuals/visual-base-impl.h>

namespace Dali
{
namespace Ui
{
TextVisual TextVisual::New()
{
  Internal::VisualBaseImplPtr internal = Internal::VisualBaseImpl::New(Dali::Ui::VisualType::TEXT);
  return TextVisual(internal.Get());
}

TextVisual TextVisual::DownCast(BaseHandle handle)
{
  Internal::VisualBaseImpl* visualBaseImpl = dynamic_cast<Internal::VisualBaseImpl*>(handle.GetObjectPtr());
  if(visualBaseImpl && visualBaseImpl->GetVisualType() == Dali::Ui::VisualType::TEXT)
  {
    return TextVisual(visualBaseImpl);
  }
  return TextVisual();
}

// =============================================================================
// Properties
// =============================================================================

Dali::String TextVisual::GetText() const
{
  return VisualBase::GetProperty<Dali::String>(TextVisual::Property::TEXT);
}

void TextVisual::SetText(const Dali::String& text)
{
  VisualBase::SetProperty(TextVisual::Property::TEXT, text);
}

Dali::String TextVisual::GetFontFamily() const
{
  return VisualBase::GetProperty<Dali::String>(TextVisual::Property::FONT_FAMILY);
}

void TextVisual::SetFontFamily(const Dali::String& fontFamily)
{
  VisualBase::SetProperty(TextVisual::Property::FONT_FAMILY, fontFamily);
}

float TextVisual::GetFontSize() const
{
  return VisualBase::GetProperty<float>(TextVisual::Property::FONT_SIZE);
}

void TextVisual::SetFontSize(float fontSize)
{
  VisualBase::SetProperty(TextVisual::Property::FONT_SIZE, fontSize);
}

Dali::Ui::Text::FontWeight TextVisual::GetFontWeight() const
{
  return VisualBase::GetProperty<Dali::Ui::Text::FontWeight>(TextVisual::Property::FONT_WEIGHT);
}

void TextVisual::SetFontWeight(Dali::Ui::Text::FontWeight fontWeight)
{
  VisualBase::SetProperty(TextVisual::Property::FONT_WEIGHT, fontWeight);
}

Dali::Ui::Text::FontWidth TextVisual::GetFontWidth() const
{
  return VisualBase::GetProperty<Dali::Ui::Text::FontWidth>(TextVisual::Property::FONT_WIDTH);
}

void TextVisual::SetFontWidth(Dali::Ui::Text::FontWidth fontWidth)
{
  VisualBase::SetProperty(TextVisual::Property::FONT_WIDTH, fontWidth);
}

Dali::Ui::Text::FontSlant TextVisual::GetFontSlant() const
{
  return VisualBase::GetProperty<Dali::Ui::Text::FontSlant>(TextVisual::Property::FONT_SLANT);
}

void TextVisual::SetFontSlant(Dali::Ui::Text::FontSlant fontSlant)
{
  VisualBase::SetProperty(TextVisual::Property::FONT_SLANT, fontSlant);
}

bool TextVisual::IsMultiLine() const
{
  return VisualBase::GetProperty<bool>(TextVisual::Property::MULTI_LINE);
}

void TextVisual::SetMultiLine(bool multiLine)
{
  VisualBase::SetProperty(TextVisual::Property::MULTI_LINE, multiLine);
}

Dali::Ui::Text::LineWrapMode TextVisual::GetLineWrapMode() const
{
  return VisualBase::GetProperty<Dali::Ui::Text::LineWrapMode>(TextVisual::Property::LINE_WRAP_MODE);
}

void TextVisual::SetLineWrapMode(Dali::Ui::Text::LineWrapMode lineWrapMode)
{
  VisualBase::SetProperty(TextVisual::Property::LINE_WRAP_MODE, lineWrapMode);
}

Dali::Ui::Text::Alignment TextVisual::GetHorizontalAlignment() const
{
  return VisualBase::GetProperty<Dali::Ui::Text::Alignment>(TextVisual::Property::HORIZONTAL_ALIGNMENT);
}

void TextVisual::SetHorizontalAlignment(Dali::Ui::Text::Alignment alignment)
{
  VisualBase::SetProperty(TextVisual::Property::HORIZONTAL_ALIGNMENT, alignment);
}

Dali::Ui::Text::Alignment TextVisual::GetVerticalAlignment() const
{
  return VisualBase::GetProperty<Dali::Ui::Text::Alignment>(TextVisual::Property::VERTICAL_ALIGNMENT);
}

void TextVisual::SetVerticalAlignment(Dali::Ui::Text::Alignment alignment)
{
  VisualBase::SetProperty(TextVisual::Property::VERTICAL_ALIGNMENT, alignment);
}

Dali::Ui::Text::OverflowMode TextVisual::GetTextOverflowMode() const
{
  return VisualBase::GetProperty<Dali::Ui::Text::OverflowMode>(TextVisual::Property::OVERFLOW_MODE);
}

void TextVisual::SetTextOverflowMode(Dali::Ui::Text::OverflowMode overflowMode)
{
  VisualBase::SetProperty(TextVisual::Property::OVERFLOW_MODE, overflowMode);
}

float TextVisual::GetLineHeight() const
{
  return VisualBase::GetProperty<float>(TextVisual::Property::LINE_HEIGHT);
}

void TextVisual::SetLineHeight(float lineHeight)
{
  VisualBase::SetProperty(TextVisual::Property::LINE_HEIGHT, lineHeight);
}

Dali::Ui::Text::LineHeightMode TextVisual::GetLineHeightMode() const
{
  return VisualBase::GetProperty<Dali::Ui::Text::LineHeightMode>(TextVisual::Property::LINE_HEIGHT_MODE);
}

void TextVisual::SetLineHeightMode(Dali::Ui::Text::LineHeightMode lineHeightMode)
{
  VisualBase::SetProperty(TextVisual::Property::LINE_HEIGHT_MODE, lineHeightMode);
}

UiColor TextVisual::GetTextColor() const
{
  // Note: Property system doesn't support UiColor with string token, so we use Vector4 internally.
  return UiColor(VisualBase::GetProperty<Dali::Vector4>(TextVisual::Property::TEXT_COLOR));
}

void TextVisual::SetTextColor(const UiColor& textColor)
{
  // Note: Property system doesn't support UiColor with string token, so we use GetRgba() directly.
  VisualBase::SetProperty(TextVisual::Property::TEXT_COLOR, textColor.GetRgba());
}

bool TextVisual::IsMarkupEnabled() const
{
  return VisualBase::GetProperty<bool>(TextVisual::Property::MARKUP_ENABLED);
}

void TextVisual::SetMarkupEnabled(bool enabled)
{
  VisualBase::SetProperty(TextVisual::Property::MARKUP_ENABLED, enabled);
}

// =============================================================================
// Internal API
// =============================================================================

TextVisual::TextVisual(Dali::Ui::Internal::VisualBaseImpl* object)
: VisualBase(object)
{
}
} // namespace Ui
} // namespace Dali
