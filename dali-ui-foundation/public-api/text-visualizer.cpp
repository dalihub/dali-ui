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

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/text-visualizer-impl.h>
#include <dali-ui-foundation/public-api/text-visualizer.h>

namespace Dali
{

namespace Ui
{

inline Integration::TextVisualizerImpl& GetImpl(TextVisualizer& textVisualizer)
{
  DALI_ASSERT_ALWAYS(textVisualizer);

  Dali::RefObject& handle = textVisualizer.GetImplementation();

  return static_cast<Integration::TextVisualizerImpl&>(handle);
}

inline const Integration::TextVisualizerImpl& GetImpl(const TextVisualizer& textVisualizer)
{
  DALI_ASSERT_ALWAYS(textVisualizer);

  const Dali::RefObject& handle = textVisualizer.GetImplementation();

  return static_cast<const Integration::TextVisualizerImpl&>(handle);
}

TextVisualizer::TextVisualizer()
{
}

TextVisualizer TextVisualizer::New()
{
  Integration::TextVisualizerImplPtr impl = Integration::TextVisualizerImpl::New();

  TextVisualizer textVisualizer = TextVisualizer(*impl);

  // Second-phase initialization
  impl->Initialize();
  return textVisualizer;
}

TextVisualizer::TextVisualizer(const TextVisualizer& textVisualizer)
: View(textVisualizer)
{
}

TextVisualizer::TextVisualizer(TextVisualizer&& rhs) noexcept
: View(std::move(rhs))
{
}

TextVisualizer::~TextVisualizer()
{
}

TextVisualizer& TextVisualizer::operator=(const TextVisualizer& handle)
{
  if(&handle != this)
  {
    View::operator=(handle);
  }
  return *this;
}

TextVisualizer& TextVisualizer::operator=(TextVisualizer&& rhs) noexcept
{
  View::operator=(std::move(rhs));
  return *this;
}

TextVisualizer TextVisualizer::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<TextVisualizer, Integration::TextVisualizerImpl>(handle);
}

// =============================================================================
// Properties
// =============================================================================

TextVisualizer& TextVisualizer::SetText(const Dali::String& text)
{
  GetImpl(*this).SetText(text);
  return *this;
}

Dali::String TextVisualizer::GetText() const
{
  return GetImpl(*this).GetText();
}

TextVisualizer& TextVisualizer::SetFontFamily(const Dali::String& fontFamily)
{
  GetImpl(*this).SetFontFamily(fontFamily);
  return *this;
}

Dali::String TextVisualizer::GetFontFamily() const
{
  return GetImpl(*this).GetFontFamily();
}

TextVisualizer& TextVisualizer::SetFontSize(float fontSize)
{
  GetImpl(*this).SetFontSize(fontSize);
  return *this;
}

float TextVisualizer::GetFontSize() const
{
  return GetImpl(*this).GetFontSize();
}

TextVisualizer& TextVisualizer::SetLineHeight(float lineHeight)
{
  GetImpl(*this).SetLineHeight(lineHeight);
  return *this;
}

float TextVisualizer::GetLineHeight() const
{
  return GetImpl(*this).GetLineHeight();
}

void TextVisualizer::ClearLineHeight()
{
  GetImpl(*this).ClearLineHeight();
}

TextVisualizer& TextVisualizer::SetTextColor(const UiColor& color)
{
  GetImpl(*this).SetTextColor(color);
  return *this;
}

UiColor TextVisualizer::GetTextColor()
{
  return GetImpl(*this).GetTextColor();
}

void TextVisualizer::Prepare()
{
  GetImpl(*this).Prepare();
}

void TextVisualizer::SetExclusionRegions(const Dali::Vector<Rect<float>>& regions)
{
  GetImpl(*this).SetExclusionRegions(regions);
}

Dali::Vector<Rect<float>> TextVisualizer::GetExclusionRegions() const
{
  return GetImpl(*this).GetExclusionRegions();
}

void TextVisualizer::ClearExclusionRegions()
{
  GetImpl(*this).ClearExclusionRegions();
}

TextVisualizer::TextVisualizer(Integration::TextVisualizerImpl& implementation)
: View(implementation)
{
}

TextVisualizer::TextVisualizer(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Integration::TextVisualizerImpl>(internal);
}

} // namespace Ui

} // namespace Dali
