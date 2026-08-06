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
#include <dali-ui-components/public-api/styles/markdown-view-style.h>

// INTERNAL INCLUDES
#include <dali-ui-components/internal/styles/markdown-view-style-impl.h>
#include <dali-ui-foundation/extension-api/styles/ui-style-debug.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>

// EXTERNAL INCLUDES
#include <cmath>
#include <utility>

namespace Dali
{
namespace Ui
{
namespace
{

void AssertValidFontSize(float value)
{
  DALI_ASSERT_ALWAYS(std::isfinite(value) && value > 0.0f && "MarkdownViewStyle font sizes must be finite and greater than zero");
}

} // namespace

UiStyleKey<MarkdownViewStyle> MarkdownViewStyle::DefaultKey()
{
  static UiStyleKey<MarkdownViewStyle> key = UiStyleKey<MarkdownViewStyle>::Alloc();
  return key;
}

MarkdownViewStyle MarkdownViewStyle::DefaultPreset()
{
  DebugAssertStyleConfigApplied();
  static MarkdownViewStyle style = MarkdownViewStyle::Builder().Build();
  return style;
}

MarkdownViewStyle MarkdownViewStyle::Default()
{
  DebugAssertStyleConfigApplied();
  MarkdownViewStyle style = UiConfig::GetCurrent().GetStyle(DefaultKey());
  return style ? style : DefaultPreset();
}

MarkdownViewStyle MarkdownViewStyle::DownCast(BaseHandle handle)
{
  return MarkdownViewStyle(dynamic_cast<Internal::MarkdownViewStyleImpl*>(handle.GetObjectPtr()));
}

MarkdownViewStyle MarkdownViewStyle::StaticDownCast(UiStyle style)
{
  return MarkdownViewStyle(static_cast<Internal::MarkdownViewStyleImpl*>(style.GetObjectPtr()));
}

MarkdownViewStyle::Builder MarkdownViewStyle::Configure() const
{
  IntrusivePtr<Internal::MarkdownViewStyleImpl> impl(new Internal::MarkdownViewStyleImpl(GetImpl(*this)));
  return Builder(impl.Get());
}

#define DALI_MARKDOWN_STYLE_GETTER(Type, Name) \
  Type MarkdownViewStyle::Get##Name() const    \
  {                                            \
    return GetImpl(*this).Get##Name();         \
  }

DALI_MARKDOWN_STYLE_GETTER(Dali::String, TextFontFamily)
DALI_MARKDOWN_STYLE_GETTER(Dali::String, HeadingFontFamily)
DALI_MARKDOWN_STYLE_GETTER(Dali::String, CodeFontFamily)
DALI_MARKDOWN_STYLE_GETTER(float, TextFontSize)
DALI_MARKDOWN_STYLE_GETTER(float, Heading1FontSize)
DALI_MARKDOWN_STYLE_GETTER(float, Heading2FontSize)
DALI_MARKDOWN_STYLE_GETTER(float, Heading3FontSize)
DALI_MARKDOWN_STYLE_GETTER(float, Heading4FontSize)
DALI_MARKDOWN_STYLE_GETTER(float, Heading5FontSize)
DALI_MARKDOWN_STYLE_GETTER(float, Heading6FontSize)
DALI_MARKDOWN_STYLE_GETTER(float, CodeBlockFontSize)
DALI_MARKDOWN_STYLE_GETTER(float, CodeBlockTitleFontSize)
DALI_MARKDOWN_STYLE_GETTER(UiColor, TextColor)
DALI_MARKDOWN_STYLE_GETTER(UiColor, HeadingTextColor)
DALI_MARKDOWN_STYLE_GETTER(UiColor, QuoteTextColor)
DALI_MARKDOWN_STYLE_GETTER(UiColor, CodeTextColor)
DALI_MARKDOWN_STYLE_GETTER(UiColor, CodeBlockTitleTextColor)
DALI_MARKDOWN_STYLE_GETTER(UiColor, InlineCodeBackgroundColor)
DALI_MARKDOWN_STYLE_GETTER(UiColor, CodeBlockBackgroundColor)
DALI_MARKDOWN_STYLE_GETTER(UiColor, CodeBlockTitleBackgroundColor)
DALI_MARKDOWN_STYLE_GETTER(UiColor, QuoteBarColor)
DALI_MARKDOWN_STYLE_GETTER(UiColor, ThematicBreakColor)
DALI_MARKDOWN_STYLE_GETTER(UiColor, TableRuleColor)
#undef DALI_MARKDOWN_STYLE_GETTER

MarkdownViewStyle::MarkdownViewStyle(Internal::MarkdownViewStyleImpl* impl)
: UiStyle(impl)
{
}

MarkdownViewStyle::Builder::Builder()
: mImpl(new Internal::MarkdownViewStyleImpl())
{
}

MarkdownViewStyle::Builder::Builder(Builder&& rhs) noexcept                               = default;
MarkdownViewStyle::Builder& MarkdownViewStyle::Builder::operator=(Builder&& rhs) noexcept = default;
MarkdownViewStyle::Builder::~Builder()                                                    = default;

#define DALI_MARKDOWN_STYLE_STRING_SETTER(Name)                                                    \
  MarkdownViewStyle::Builder& MarkdownViewStyle::Builder::Set##Name(const Dali::String& value) &   \
  {                                                                                                \
    mImpl->Set##Name(value);                                                                       \
    return *this;                                                                                  \
  }                                                                                                \
  MarkdownViewStyle::Builder&& MarkdownViewStyle::Builder::Set##Name(const Dali::String& value) && \
  {                                                                                                \
    Set##Name(value);                                                                              \
    return std::move(*this);                                                                       \
  }

DALI_MARKDOWN_STYLE_STRING_SETTER(TextFontFamily)
DALI_MARKDOWN_STYLE_STRING_SETTER(HeadingFontFamily)
DALI_MARKDOWN_STYLE_STRING_SETTER(CodeFontFamily)
#undef DALI_MARKDOWN_STYLE_STRING_SETTER

#define DALI_MARKDOWN_STYLE_FLOAT_SETTER(Name)                                       \
  MarkdownViewStyle::Builder& MarkdownViewStyle::Builder::Set##Name(float value) &   \
  {                                                                                  \
    AssertValidFontSize(value);                                                      \
    mImpl->Set##Name(value);                                                         \
    return *this;                                                                    \
  }                                                                                  \
  MarkdownViewStyle::Builder&& MarkdownViewStyle::Builder::Set##Name(float value) && \
  {                                                                                  \
    Set##Name(value);                                                                \
    return std::move(*this);                                                         \
  }

DALI_MARKDOWN_STYLE_FLOAT_SETTER(TextFontSize)
DALI_MARKDOWN_STYLE_FLOAT_SETTER(Heading1FontSize)
DALI_MARKDOWN_STYLE_FLOAT_SETTER(Heading2FontSize)
DALI_MARKDOWN_STYLE_FLOAT_SETTER(Heading3FontSize)
DALI_MARKDOWN_STYLE_FLOAT_SETTER(Heading4FontSize)
DALI_MARKDOWN_STYLE_FLOAT_SETTER(Heading5FontSize)
DALI_MARKDOWN_STYLE_FLOAT_SETTER(Heading6FontSize)
DALI_MARKDOWN_STYLE_FLOAT_SETTER(CodeBlockFontSize)
DALI_MARKDOWN_STYLE_FLOAT_SETTER(CodeBlockTitleFontSize)
#undef DALI_MARKDOWN_STYLE_FLOAT_SETTER

#define DALI_MARKDOWN_STYLE_COLOR_SETTER(Name)                                                \
  MarkdownViewStyle::Builder& MarkdownViewStyle::Builder::Set##Name(const UiColor& value) &   \
  {                                                                                           \
    mImpl->Set##Name(value);                                                                  \
    return *this;                                                                             \
  }                                                                                           \
  MarkdownViewStyle::Builder&& MarkdownViewStyle::Builder::Set##Name(const UiColor& value) && \
  {                                                                                           \
    Set##Name(value);                                                                         \
    return std::move(*this);                                                                  \
  }

DALI_MARKDOWN_STYLE_COLOR_SETTER(TextColor)
DALI_MARKDOWN_STYLE_COLOR_SETTER(HeadingTextColor)
DALI_MARKDOWN_STYLE_COLOR_SETTER(QuoteTextColor)
DALI_MARKDOWN_STYLE_COLOR_SETTER(CodeTextColor)
DALI_MARKDOWN_STYLE_COLOR_SETTER(CodeBlockTitleTextColor)
DALI_MARKDOWN_STYLE_COLOR_SETTER(InlineCodeBackgroundColor)
DALI_MARKDOWN_STYLE_COLOR_SETTER(CodeBlockBackgroundColor)
DALI_MARKDOWN_STYLE_COLOR_SETTER(CodeBlockTitleBackgroundColor)
DALI_MARKDOWN_STYLE_COLOR_SETTER(QuoteBarColor)
DALI_MARKDOWN_STYLE_COLOR_SETTER(ThematicBreakColor)
DALI_MARKDOWN_STYLE_COLOR_SETTER(TableRuleColor)
#undef DALI_MARKDOWN_STYLE_COLOR_SETTER

MarkdownViewStyle MarkdownViewStyle::Builder::Build() &&
{
  DALI_ASSERT_ALWAYS(mImpl && "MarkdownViewStyle::Builder has already been consumed");
  MarkdownViewStyle style(mImpl.Get());
  mImpl.Reset();
  return style;
}

MarkdownViewStyle::Builder::Builder(Internal::MarkdownViewStyleImpl* impl)
: mImpl(impl)
{
}

} // namespace Ui
} // namespace Dali
