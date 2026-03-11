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
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-ui-elements/public-api/label.h>
#include <dali-ui-elements/integration-api/label-impl.h>

namespace Dali
{

namespace Ui
{

inline Integration::LabelImpl& GetImpl(Label& label)
{
  DALI_ASSERT_ALWAYS(label);

  Dali::RefObject& handle = label.GetImplementation();

  return static_cast<Integration::LabelImpl&>(handle);
}

inline const Integration::LabelImpl& GetImpl(const Label& label)
{
  DALI_ASSERT_ALWAYS(label);

  const Dali::RefObject& handle = label.GetImplementation();

  return static_cast<const Integration::LabelImpl&>(handle);
}

Label::Label()
{
}

Label Label::New()
{
  Integration::LabelImplPtr impl = Integration::LabelImpl::New();

  Label label = Label(*impl);

  // Second-phase initialization
  impl->Initialize();
  return label;
}

Label::Label(const Label& label)
  : View(label)
{
}

Label::Label(Label&& rhs) noexcept
  : View(std::move(rhs))
{
}

Label::~Label()
{
}

Label& Label::operator=(const Label& handle)
{
  if (&handle != this)
  {
    View::operator=(handle);
  }
  return *this;
}

Label& Label::operator=(Label&& rhs) noexcept
{
  View::operator=(std::move(rhs));
  return *this;
}

Label Label::DownCast(BaseHandle handle)
{
  // TODO: Use Ui::View::DownCast once the template DownCast is migrated from Ui::Control.
  return Ui::Control::DownCast<Label, Integration::LabelImpl>(handle);
}

Label::Label(Integration::LabelImpl& implementation)
  : View(implementation)
{
}

Label::Label(Dali::Internal::CustomActor* internal)
  : View(internal)
{
  VerifyCustomActorPointer<Integration::LabelImpl>(internal);
}

// =============================================================================
// Properties
// =============================================================================

Label& Label::SetText(std::string text)
{
  GetImpl(*this).SetText(text);
  return *this;
}

std::string Label::GetText() const
{
  return GetImpl(*this).GetText();
}

Label& Label::SetFontFamily(std::string fontFamily)
{
  GetImpl(*this).SetFontFamily(fontFamily);
  return *this;
}
std::string Label::GetFontFamily() const
{
  return GetImpl(*this).GetFontFamily();
}

Label& Label::SetFontSize(float fontSize)
{
  GetImpl(*this).SetFontSize(fontSize);
  return *this;
}

float Label::GetFontSize() const
{
  return GetImpl(*this).GetFontSize();
}

Label& Label::SetMultiLine(bool multiLine)
{
  GetImpl(*this).SetMultiLine(multiLine);
  return *this;
}

bool Label::IsMultiLine() const
{
  return GetImpl(*this).IsMultiLine();
}

Label& Label::SetTextColor(const Vector4& color)
{
  GetImpl(*this).SetTextColor(color);
  return *this;
}

const Vector4& Label::GetTextColor() const
{
  return GetImpl(*this).GetTextColor();
}

Label& Label::SetHorizontalTextAlignment(TextAlignment alignment)
{
  GetImpl(*this).SetHorizontalTextAlignment(alignment);
  return *this;
}

TextAlignment Label::GetHorizontalTextAlignment() const
{
  return GetImpl(*this).GetHorizontalTextAlignment();
}

Label& Label::SetVerticalTextAlignment(TextAlignment alignment)
{
  GetImpl(*this).SetVerticalTextAlignment(alignment);
  return *this;
}

TextAlignment Label::GetVerticalTextAlignment() const
{
  return GetImpl(*this).GetVerticalTextAlignment();
}

} // namespace Ui

} // namespace Dali
