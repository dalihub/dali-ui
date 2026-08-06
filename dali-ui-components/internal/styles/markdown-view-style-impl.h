#pragma once

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

// INTERNAL INCLUDES
#include <dali-ui-components/internal/markdown/markdown-view-defaults.h>
#include <dali-ui-components/public-api/styles/markdown-view-style.h>
#include <dali-ui-foundation/extension-api/styles/ui-style-impl.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

class MarkdownViewStyleImpl : public Extension::UiStyleImpl
{
public:
  MarkdownViewStyleImpl() = default;
  MarkdownViewStyleImpl(const MarkdownViewStyleImpl& rhs)
  : mTextFontFamily(rhs.mTextFontFamily),
    mHeadingFontFamily(rhs.mHeadingFontFamily),
    mCodeFontFamily(rhs.mCodeFontFamily),
    mTextFontSize(rhs.mTextFontSize),
    mHeading1FontSize(rhs.mHeading1FontSize),
    mHeading2FontSize(rhs.mHeading2FontSize),
    mHeading3FontSize(rhs.mHeading3FontSize),
    mHeading4FontSize(rhs.mHeading4FontSize),
    mHeading5FontSize(rhs.mHeading5FontSize),
    mHeading6FontSize(rhs.mHeading6FontSize),
    mCodeBlockFontSize(rhs.mCodeBlockFontSize),
    mCodeBlockTitleFontSize(rhs.mCodeBlockTitleFontSize),
    mTextColor(rhs.mTextColor),
    mHeadingTextColor(rhs.mHeadingTextColor),
    mQuoteTextColor(rhs.mQuoteTextColor),
    mCodeTextColor(rhs.mCodeTextColor),
    mCodeBlockTitleTextColor(rhs.mCodeBlockTitleTextColor),
    mInlineCodeBackgroundColor(rhs.mInlineCodeBackgroundColor),
    mCodeBlockBackgroundColor(rhs.mCodeBlockBackgroundColor),
    mCodeBlockTitleBackgroundColor(rhs.mCodeBlockTitleBackgroundColor),
    mQuoteBarColor(rhs.mQuoteBarColor),
    mThematicBreakColor(rhs.mThematicBreakColor),
    mTableRuleColor(rhs.mTableRuleColor)
  {
  }

#define DALI_MARKDOWN_STYLE_STRING_PROPERTY(Name) \
  void Set##Name(const Dali::String& value)       \
  {                                               \
    m##Name = value;                              \
  }                                               \
  Dali::String Get##Name() const                  \
  {                                               \
    return m##Name;                               \
  }

  DALI_MARKDOWN_STYLE_STRING_PROPERTY(TextFontFamily)
  DALI_MARKDOWN_STYLE_STRING_PROPERTY(HeadingFontFamily)
  DALI_MARKDOWN_STYLE_STRING_PROPERTY(CodeFontFamily)
#undef DALI_MARKDOWN_STYLE_STRING_PROPERTY

#define DALI_MARKDOWN_STYLE_FLOAT_PROPERTY(Name) \
  void Set##Name(float value)                    \
  {                                              \
    m##Name = value;                             \
  }                                              \
  float Get##Name() const                        \
  {                                              \
    return m##Name;                              \
  }

  DALI_MARKDOWN_STYLE_FLOAT_PROPERTY(TextFontSize)
  DALI_MARKDOWN_STYLE_FLOAT_PROPERTY(Heading1FontSize)
  DALI_MARKDOWN_STYLE_FLOAT_PROPERTY(Heading2FontSize)
  DALI_MARKDOWN_STYLE_FLOAT_PROPERTY(Heading3FontSize)
  DALI_MARKDOWN_STYLE_FLOAT_PROPERTY(Heading4FontSize)
  DALI_MARKDOWN_STYLE_FLOAT_PROPERTY(Heading5FontSize)
  DALI_MARKDOWN_STYLE_FLOAT_PROPERTY(Heading6FontSize)
  DALI_MARKDOWN_STYLE_FLOAT_PROPERTY(CodeBlockFontSize)
  DALI_MARKDOWN_STYLE_FLOAT_PROPERTY(CodeBlockTitleFontSize)
#undef DALI_MARKDOWN_STYLE_FLOAT_PROPERTY

#define DALI_MARKDOWN_STYLE_COLOR_PROPERTY(Name) \
  void Set##Name(const UiColor& value)           \
  {                                              \
    m##Name = value;                             \
  }                                              \
  UiColor Get##Name() const                      \
  {                                              \
    return m##Name;                              \
  }

  DALI_MARKDOWN_STYLE_COLOR_PROPERTY(TextColor)
  DALI_MARKDOWN_STYLE_COLOR_PROPERTY(HeadingTextColor)
  DALI_MARKDOWN_STYLE_COLOR_PROPERTY(QuoteTextColor)
  DALI_MARKDOWN_STYLE_COLOR_PROPERTY(CodeTextColor)
  DALI_MARKDOWN_STYLE_COLOR_PROPERTY(CodeBlockTitleTextColor)
  DALI_MARKDOWN_STYLE_COLOR_PROPERTY(InlineCodeBackgroundColor)
  DALI_MARKDOWN_STYLE_COLOR_PROPERTY(CodeBlockBackgroundColor)
  DALI_MARKDOWN_STYLE_COLOR_PROPERTY(CodeBlockTitleBackgroundColor)
  DALI_MARKDOWN_STYLE_COLOR_PROPERTY(QuoteBarColor)
  DALI_MARKDOWN_STYLE_COLOR_PROPERTY(ThematicBreakColor)
  DALI_MARKDOWN_STYLE_COLOR_PROPERTY(TableRuleColor)
#undef DALI_MARKDOWN_STYLE_COLOR_PROPERTY

protected:
  ~MarkdownViewStyleImpl() override = default;

private:
  Dali::String mTextFontFamily{MarkdownViewDefaults::TEXT_FONT_FAMILY};
  Dali::String mHeadingFontFamily{MarkdownViewDefaults::HEADING_FONT_FAMILY};
  Dali::String mCodeFontFamily{MarkdownViewDefaults::CODE_FONT_FAMILY};

  float mTextFontSize{MarkdownViewDefaults::TEXT_FONT_SIZE};
  float mHeading1FontSize{MarkdownViewDefaults::HEADING1_FONT_SIZE};
  float mHeading2FontSize{MarkdownViewDefaults::HEADING2_FONT_SIZE};
  float mHeading3FontSize{MarkdownViewDefaults::HEADING3_FONT_SIZE};
  float mHeading4FontSize{MarkdownViewDefaults::HEADING4_FONT_SIZE};
  float mHeading5FontSize{MarkdownViewDefaults::HEADING5_FONT_SIZE};
  float mHeading6FontSize{MarkdownViewDefaults::HEADING6_FONT_SIZE};
  float mCodeBlockFontSize{MarkdownViewDefaults::CODE_BLOCK_FONT_SIZE};
  float mCodeBlockTitleFontSize{MarkdownViewDefaults::CODE_BLOCK_TITLE_FONT_SIZE};

  UiColor mTextColor{MarkdownViewDefaults::TEXT_COLOR};
  UiColor mHeadingTextColor{MarkdownViewDefaults::HEADING_TEXT_COLOR};
  UiColor mQuoteTextColor{MarkdownViewDefaults::QUOTE_TEXT_COLOR};
  UiColor mCodeTextColor{MarkdownViewDefaults::CODE_TEXT_COLOR};
  UiColor mCodeBlockTitleTextColor{MarkdownViewDefaults::CODE_BLOCK_TITLE_TEXT_COLOR};
  UiColor mInlineCodeBackgroundColor{MarkdownViewDefaults::INLINE_CODE_BACKGROUND_COLOR,
                                     MarkdownViewDefaults::INLINE_CODE_BACKGROUND_ALPHA};
  UiColor mCodeBlockBackgroundColor{MarkdownViewDefaults::CODE_BLOCK_BACKGROUND_COLOR,
                                    MarkdownViewDefaults::CODE_BLOCK_BACKGROUND_ALPHA};
  UiColor mCodeBlockTitleBackgroundColor{MarkdownViewDefaults::CODE_BLOCK_TITLE_BACKGROUND_COLOR,
                                         MarkdownViewDefaults::CODE_BLOCK_TITLE_BACKGROUND_ALPHA};
  UiColor mQuoteBarColor{MarkdownViewDefaults::QUOTE_BAR_COLOR};
  UiColor mThematicBreakColor{MarkdownViewDefaults::THEMATIC_BREAK_COLOR};
  UiColor mTableRuleColor{MarkdownViewDefaults::TABLE_RULE_COLOR};
};

} // namespace Internal

inline Internal::MarkdownViewStyleImpl& GetImpl(Ui::MarkdownViewStyle& style)
{
  BaseObject& handle = style.GetBaseObject();
  return static_cast<Internal::MarkdownViewStyleImpl&>(handle);
}

inline const Internal::MarkdownViewStyleImpl& GetImpl(const Ui::MarkdownViewStyle& style)
{
  const BaseObject& handle = style.GetBaseObject();
  return static_cast<const Internal::MarkdownViewStyleImpl&>(handle);
}

} // namespace Ui
} // namespace Dali
