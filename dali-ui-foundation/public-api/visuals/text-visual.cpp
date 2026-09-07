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
#include <dali-ui-foundation/integration-api/visuals/text-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-base-impl.h>

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
  return GetImplementation(*this).GetProperty<Dali::String>(Dali::Ui::Integration::TextVisual::Property::TEXT);
}

void TextVisual::SetText(const Dali::String& text)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::TextVisual::Property::TEXT, text);
}

Dali::String TextVisual::GetFontFamily() const
{
  return GetImplementation(*this).GetProperty<Dali::String>(Dali::Ui::Integration::TextVisual::Property::FONT_FAMILY);
}

void TextVisual::SetFontFamily(const Dali::String& fontFamily)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::TextVisual::Property::FONT_FAMILY, fontFamily);
}

float TextVisual::GetFontSize() const
{
  return GetImplementation(*this).GetProperty<float>(Dali::Ui::Integration::TextVisual::Property::FONT_SIZE);
}

void TextVisual::SetFontSize(float fontSize)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::TextVisual::Property::FONT_SIZE, fontSize);
}

Dali::Ui::Text::FontWeight TextVisual::GetFontWeight() const
{
  return GetImplementation(*this).GetProperty<Dali::Ui::Text::FontWeight>(Dali::Ui::Integration::TextVisual::Property::FONT_WEIGHT);
}

void TextVisual::SetFontWeight(Dali::Ui::Text::FontWeight fontWeight)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::TextVisual::Property::FONT_WEIGHT, fontWeight);
}

Dali::Ui::Text::FontWidth TextVisual::GetFontWidth() const
{
  return GetImplementation(*this).GetProperty<Dali::Ui::Text::FontWidth>(Dali::Ui::Integration::TextVisual::Property::FONT_WIDTH);
}

void TextVisual::SetFontWidth(Dali::Ui::Text::FontWidth fontWidth)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::TextVisual::Property::FONT_WIDTH, fontWidth);
}

Dali::Ui::Text::FontSlant TextVisual::GetFontSlant() const
{
  return GetImplementation(*this).GetProperty<Dali::Ui::Text::FontSlant>(Dali::Ui::Integration::TextVisual::Property::FONT_SLANT);
}

void TextVisual::SetFontSlant(Dali::Ui::Text::FontSlant fontSlant)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::TextVisual::Property::FONT_SLANT, fontSlant);
}

bool TextVisual::IsMultiLine() const
{
  return GetImplementation(*this).GetProperty<bool>(Dali::Ui::Integration::TextVisual::Property::MULTI_LINE);
}

void TextVisual::SetMultiLine(bool multiLine)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::TextVisual::Property::MULTI_LINE, multiLine);
}

Dali::Ui::Text::LineWrapMode TextVisual::GetLineWrapMode() const
{
  return GetImplementation(*this).GetProperty<Dali::Ui::Text::LineWrapMode>(Dali::Ui::Integration::TextVisual::Property::LINE_WRAP_MODE);
}

void TextVisual::SetLineWrapMode(Dali::Ui::Text::LineWrapMode lineWrapMode)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::TextVisual::Property::LINE_WRAP_MODE, lineWrapMode);
}

Dali::Ui::Text::Alignment TextVisual::GetHorizontalAlignment() const
{
  return GetImplementation(*this).GetProperty<Dali::Ui::Text::Alignment>(Dali::Ui::Integration::TextVisual::Property::HORIZONTAL_ALIGNMENT);
}

void TextVisual::SetHorizontalAlignment(Dali::Ui::Text::Alignment alignment)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::TextVisual::Property::HORIZONTAL_ALIGNMENT, alignment);
}

Dali::Ui::Text::Alignment TextVisual::GetVerticalAlignment() const
{
  return GetImplementation(*this).GetProperty<Dali::Ui::Text::Alignment>(Dali::Ui::Integration::TextVisual::Property::VERTICAL_ALIGNMENT);
}

void TextVisual::SetVerticalAlignment(Dali::Ui::Text::Alignment alignment)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::TextVisual::Property::VERTICAL_ALIGNMENT, alignment);
}

Dali::Ui::Text::OverflowMode TextVisual::GetTextOverflowMode() const
{
  return GetImplementation(*this).GetProperty<Dali::Ui::Text::OverflowMode>(Dali::Ui::Integration::TextVisual::Property::OVERFLOW_MODE);
}

void TextVisual::SetTextOverflowMode(Dali::Ui::Text::OverflowMode overflowMode)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::TextVisual::Property::OVERFLOW_MODE, overflowMode);
}

float TextVisual::GetLineHeight() const
{
  return GetImplementation(*this).GetProperty<float>(Dali::Ui::Integration::TextVisual::Property::LINE_HEIGHT);
}

void TextVisual::SetLineHeight(float lineHeight)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::TextVisual::Property::LINE_HEIGHT, lineHeight);
}

Dali::Ui::Text::LineHeightMode TextVisual::GetLineHeightMode() const
{
  return GetImplementation(*this).GetProperty<Dali::Ui::Text::LineHeightMode>(Dali::Ui::Integration::TextVisual::Property::LINE_HEIGHT_MODE);
}

void TextVisual::SetLineHeightMode(Dali::Ui::Text::LineHeightMode lineHeightMode)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::TextVisual::Property::LINE_HEIGHT_MODE, lineHeightMode);
}

UiColor TextVisual::GetTextColor() const
{
  // Note: Property system doesn't support UiColor with string token, so we use Vector4 internally.
  return UiColor(GetImplementation(*this).GetProperty<Dali::Vector4>(Dali::Ui::Integration::TextVisual::Property::TEXT_COLOR));
}

void TextVisual::SetTextColor(const UiColor& textColor)
{
  // Note: Property system doesn't support UiColor with string token, so we use GetRgba() directly.
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::TextVisual::Property::TEXT_COLOR, textColor.GetRgba());
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
