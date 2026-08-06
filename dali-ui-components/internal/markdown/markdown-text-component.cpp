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
#include <dali-ui-components/internal/markdown/markdown-text-component.h>

// EXTERNAL INCLUDES
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/text/style/anchor-attributes.h>
#include <dali-ui-foundation/public-api/text/style/font-attributes.h>
#include <dali-ui-foundation/public-api/text/style/line-through.h>
#include <dali-ui-foundation/public-api/text/style/underline.h>
#include <dali-ui-foundation/public-api/text/styled-text/anchor-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/background-color-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/font-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/foreground-color-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/line-through-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text-builder.h>
#include <dali-ui-foundation/public-api/text/styled-text/underline-span.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <algorithm>
#include <utility>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-components/internal/markdown/markdown-view-defaults.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{

constexpr uint32_t LINK_COLOR               = 0x1A73E8;
constexpr uint32_t LINK_CLICKED_COLOR       = 0x174EA6;
constexpr float    INVALID_FONT_SIZE        = -1.0f;
constexpr float    NATURAL_LINE_HEIGHT      = -1.0f;
constexpr uint32_t MARKDOWN_TEXT_FONT_FLAGS = MARKDOWN_TEXT_STYLE_STRONG |
                                              MARKDOWN_TEXT_STYLE_EMPHASIS |
                                              MARKDOWN_TEXT_STYLE_INLINE_CODE;

enum class MarkdownLinkPresentation : uint8_t
{
  TEXT_WITH_URL,
  INTERACTIVE_ANCHOR
};

constexpr MarkdownLinkPresentation DEFAULT_LINK_PRESENTATION = MarkdownLinkPresentation::TEXT_WITH_URL;

UiColor TextColorForRole(const MarkdownRenderNode& node, const MarkdownViewStyle& style)
{
  if(node.role == MarkdownRenderRole::QUOTE_PARAGRAPH)
  {
    return style.GetQuoteTextColor();
  }
  if(node.role == MarkdownRenderRole::CODE_BLOCK)
  {
    return style.GetCodeTextColor();
  }
  if(node.role == MarkdownRenderRole::HEADING)
  {
    return style.GetHeadingTextColor();
  }
  return style.GetTextColor();
}

Dali::String FontFamilyForRole(const MarkdownRenderNode& node, const MarkdownViewStyle& style)
{
  if(node.role == MarkdownRenderRole::CODE_BLOCK)
  {
    return style.GetCodeFontFamily();
  }
  if(node.role == MarkdownRenderRole::HEADING)
  {
    return style.GetHeadingFontFamily();
  }
  return style.GetTextFontFamily();
}

float ResolveMarkdownTextFontSize(const MarkdownRenderNode& node, const MarkdownViewStyle& style)
{
  if(node.role == MarkdownRenderRole::HEADING)
  {
    switch(node.headingLevel)
    {
      case 1u:
        return style.GetHeading1FontSize();
      case 2u:
        return style.GetHeading2FontSize();
      case 3u:
        return style.GetHeading3FontSize();
      case 4u:
        return style.GetHeading4FontSize();
      case 5u:
        return style.GetHeading5FontSize();
      default:
        return style.GetHeading6FontSize();
    }
  }
  if(node.role == MarkdownRenderRole::CODE_BLOCK)
  {
    return style.GetCodeBlockFontSize();
  }
  return style.GetTextFontSize();
}

float ResolveMarkdownTextLineHeight(const MarkdownRenderNode& node, const MarkdownViewStyle& style)
{
  if(node.role == MarkdownRenderRole::HEADING)
  {
    return std::max(MarkdownViewDefaults::DEFAULT_BODY_LINE_HEIGHT,
                    ResolveMarkdownTextFontSize(node, style) *
                      MarkdownViewDefaults::DEFAULT_BODY_LINE_HEIGHT /
                      MarkdownViewDefaults::HEADING1_FONT_SIZE);
  }
  if(node.role == MarkdownRenderRole::CODE_BLOCK)
  {
    return NATURAL_LINE_HEIGHT;
  }
  return MarkdownViewDefaults::BODY_LINE_HEIGHT_RATIO;
}

Text::LineHeightMode ResolveMarkdownTextLineHeightMode(const MarkdownRenderNode& node)
{
  return node.role == MarkdownRenderRole::HEADING || node.role == MarkdownRenderRole::CODE_BLOCK
           ? Text::LineHeightMode::ABSOLUTE
           : Text::LineHeightMode::RELATIVE;
}

bool HasStyledRanges(const MarkdownRenderNode& node)
{
  return !node.styleRuns.empty() || !node.linkRanges.empty();
}

struct ResolvedTextRun
{
  uint32_t start{0u};
  uint32_t end{0u};
  uint32_t flags{MARKDOWN_TEXT_STYLE_NONE};
  int32_t  linkIndex{-1};
};

struct MarkdownLinkSuffixEvent
{
  uint32_t    position{0u};
  uint32_t    order{0u};
  uint32_t    utf32Length{0u};
  bool        blocksRunMerge{false};
  std::string suffix;
};

struct MarkdownTextProjection
{
  std::string                  text;
  std::vector<ResolvedTextRun> resolvedRuns;
};

bool SameResolvedStyle(const ResolvedTextRun& lhs, const ResolvedTextRun& rhs)
{
  return lhs.flags == rhs.flags && lhs.linkIndex == rhs.linkIndex;
}

std::vector<uint32_t> CollectStyleBoundaries(const MarkdownRenderNode& node)
{
  std::vector<uint32_t> boundaries;
  boundaries.reserve(2u + node.styleRuns.size() * 2u + node.linkRanges.size() * 2u);
  boundaries.push_back(0u);
  boundaries.push_back(node.utf32Length);

  for(const auto& run : node.styleRuns)
  {
    boundaries.push_back(std::min(run.start, node.utf32Length));
    boundaries.push_back(std::min(run.end, node.utf32Length));
  }
  for(const auto& link : node.linkRanges)
  {
    boundaries.push_back(std::min(link.start, node.utf32Length));
    boundaries.push_back(std::min(link.end, node.utf32Length));
  }

  std::sort(boundaries.begin(), boundaries.end());
  boundaries.erase(std::unique(boundaries.begin(), boundaries.end()), boundaries.end());
  return boundaries;
}

// mergeBarriers contains sorted, unique UTF-32 positions where adjacent runs must remain separate.
std::vector<ResolvedTextRun> ResolveTextRuns(const MarkdownRenderNode& node, const std::vector<uint32_t>& mergeBarriers = {})
{
  std::vector<ResolvedTextRun> resolved;
  if(node.utf32Length == 0u || !HasStyledRanges(node))
  {
    return resolved;
  }

  const std::vector<uint32_t> boundaries        = CollectStyleBoundaries(node);
  std::size_t                 mergeBarrierIndex = 0u;
  for(std::size_t i = 0u; i + 1u < boundaries.size(); ++i)
  {
    const uint32_t start = boundaries[i];
    const uint32_t end   = boundaries[i + 1u];
    if(start >= end)
    {
      continue;
    }

    ResolvedTextRun segment;
    segment.start = start;
    segment.end   = end;

    for(const auto& run : node.styleRuns)
    {
      if(run.start <= start && end <= run.end)
      {
        segment.flags |= run.flags;
      }
    }

    for(std::size_t linkIndex = 0u; linkIndex < node.linkRanges.size(); ++linkIndex)
    {
      const auto& link = node.linkRanges[linkIndex];
      if(link.start <= start && end <= link.end)
      {
        segment.linkIndex = static_cast<int32_t>(linkIndex);
        break;
      }
    }

    while(mergeBarrierIndex < mergeBarriers.size() && mergeBarriers[mergeBarrierIndex] < segment.start)
    {
      ++mergeBarrierIndex;
    }
    const bool preserveBoundary = mergeBarrierIndex < mergeBarriers.size() && mergeBarriers[mergeBarrierIndex] == segment.start;

    if(!resolved.empty() &&
       resolved.back().end == segment.start &&
       SameResolvedStyle(resolved.back(), segment) &&
       !preserveBoundary)
    {
      resolved.back().end = segment.end;
    }
    else
    {
      resolved.push_back(segment);
    }
  }
  return resolved;
}

std::size_t Utf8CharacterByteLength(const std::string& text, std::size_t byteIndex)
{
  if(byteIndex >= text.size())
  {
    return 0u;
  }

  const auto byte = static_cast<unsigned char>(text[byteIndex]);
  if((byte & 0x80u) == 0u)
  {
    return 1u;
  }
  if((byte & 0xE0u) == 0xC0u)
  {
    return std::min<std::size_t>(2u, text.size() - byteIndex);
  }
  if((byte & 0xF0u) == 0xE0u)
  {
    return std::min<std::size_t>(3u, text.size() - byteIndex);
  }
  if((byte & 0xF8u) == 0xF0u)
  {
    return std::min<std::size_t>(4u, text.size() - byteIndex);
  }
  return 1u;
}

uint32_t Utf8CharacterCount(const std::string& text)
{
  uint32_t    count     = 0u;
  std::size_t byteIndex = 0u;
  while(byteIndex < text.size())
  {
    byteIndex += Utf8CharacterByteLength(text, byteIndex);
    ++count;
  }
  return count;
}

MarkdownTextProjection ProjectTextWithUrls(const MarkdownRenderNode& node)
{
  MarkdownTextProjection projection;

  std::vector<MarkdownLinkSuffixEvent> suffixEvents;
  suffixEvents.reserve(node.linkRanges.size());

  std::size_t projectedByteLength = node.text.size();
  std::size_t mergeBarrierCount   = 0u;
  uint32_t    suffixOrder         = 0u;
  for(const auto& link : node.linkRanges)
  {
    MarkdownLinkSuffixEvent event;
    event.position       = std::min(link.end, node.utf32Length);
    event.order          = suffixOrder++;
    event.blocksRunMerge = link.start == link.end;
    mergeBarrierCount += event.blocksRunMerge ? 1u : 0u;
    event.suffix = " [";
    event.suffix += link.href;
    event.suffix += "]";
    event.utf32Length = 3u + Utf8CharacterCount(link.href);

    projectedByteLength += event.suffix.size();
    suffixEvents.push_back(std::move(event));
  }

  std::sort(suffixEvents.begin(), suffixEvents.end(), [](const MarkdownLinkSuffixEvent& lhs, const MarkdownLinkSuffixEvent& rhs)
  {
    if(lhs.position != rhs.position)
    {
      return lhs.position < rhs.position;
    }
    return lhs.order < rhs.order;
  });

  std::vector<uint32_t> mergeBarriers;
  mergeBarriers.reserve(mergeBarrierCount);
  for(const auto& event : suffixEvents)
  {
    if(event.blocksRunMerge && (mergeBarriers.empty() || mergeBarriers.back() != event.position))
    {
      mergeBarriers.push_back(event.position);
    }
  }
  projection.resolvedRuns = ResolveTextRuns(node, mergeBarriers);

  projection.text.reserve(projectedByteLength);

  std::size_t suffixEventIndex = 0u;
  auto        appendSuffixesAt = [&](uint32_t position)
  {
    while(suffixEventIndex < suffixEvents.size() && suffixEvents[suffixEventIndex].position == position)
    {
      projection.text += suffixEvents[suffixEventIndex].suffix;
      ++suffixEventIndex;
    }
  };

  uint32_t    characterIndex = 0u;
  std::size_t byteIndex      = 0u;
  while(byteIndex < node.text.size() && characterIndex < node.utf32Length)
  {
    appendSuffixesAt(characterIndex);
    const std::size_t characterBytes = Utf8CharacterByteLength(node.text, byteIndex);
    projection.text.append(node.text, byteIndex, characterBytes);
    byteIndex += characterBytes;
    ++characterIndex;
  }
  appendSuffixesAt(characterIndex);
  while(suffixEventIndex < suffixEvents.size())
  {
    projection.text += suffixEvents[suffixEventIndex].suffix;
    ++suffixEventIndex;
  }

  suffixEventIndex        = 0u;
  uint32_t insertedLength = 0u;
  for(auto& run : projection.resolvedRuns)
  {
    const uint32_t semanticStart = run.start;
    const uint32_t semanticEnd   = run.end;

    while(suffixEventIndex < suffixEvents.size() && suffixEvents[suffixEventIndex].position <= semanticStart)
    {
      insertedLength += suffixEvents[suffixEventIndex].utf32Length;
      ++suffixEventIndex;
    }
    run.start = semanticStart + insertedLength;

    while(suffixEventIndex < suffixEvents.size() && suffixEvents[suffixEventIndex].position < semanticEnd)
    {
      insertedLength += suffixEvents[suffixEventIndex].utf32Length;
      ++suffixEventIndex;
    }
    run.end = semanticEnd + insertedLength;
  }

  return projection;
}

void AddSpanIfValid(Text::StyledTextBuilder& builder, const Text::Span& span, uint32_t start, uint32_t end)
{
  if(span && end > start)
  {
    builder.SetSpan(span, start, end);
  }
}

Text::FontSpan NewFontSpan(uint32_t flags, const MarkdownViewStyle& style)
{
  Text::FontAttributes attributes;
  if(flags & MARKDOWN_TEXT_STYLE_INLINE_CODE)
  {
    attributes.SetFamily(style.GetCodeFontFamily());
  }
  else if(flags & MARKDOWN_TEXT_STYLE_STRONG)
  {
    attributes.SetWeight(Text::FontWeight::BOLD);
  }
  if(flags & MARKDOWN_TEXT_STYLE_EMPHASIS)
  {
    attributes.SetSlant(Text::FontSlant::ITALIC);
  }
  return Text::FontSpan::New(attributes);
}

Text::LineThroughSpan NewLineThroughSpan(const UiColor& color)
{
  Text::LineThrough lineThrough;
  lineThrough.SetColor(color);
  lineThrough.SetThickness(MarkdownViewDefaults::STRIKETHROUGH_THICKNESS);
  return Text::LineThroughSpan::New(lineThrough);
}

Text::UnderlineSpan NewUnderlineSpan(const UiColor& color)
{
  Text::Underline underline;
  underline.SetColor(color);
  underline.SetThickness(1.0f);
  underline.SetType(Text::Underline::Type::SOLID);
  return Text::UnderlineSpan::New(underline);
}

Text::AnchorSpan NewAnchorSpan(const MarkdownLinkRange& link)
{
  Text::AnchorAttributes attributes;
  attributes.SetHref(Dali::String(link.href.c_str()));
  attributes.SetColor(UiColor(LINK_COLOR));
  attributes.SetClickedColor(UiColor(LINK_CLICKED_COLOR));
  return Text::AnchorSpan::New(attributes);
}

Text::StyledText BuildStyledText(const MarkdownRenderNode& node, MarkdownLinkPresentation linkPresentation, const MarkdownViewStyle& style)
{
  MarkdownTextProjection       projection;
  const std::string*           displayText = &node.text;
  std::vector<ResolvedTextRun> resolvedRuns;
  const bool                   projectLinks = linkPresentation == MarkdownLinkPresentation::TEXT_WITH_URL && !node.linkRanges.empty();

  if(projectLinks)
  {
    projection  = ProjectTextWithUrls(node);
    displayText = &projection.text;
  }
  else
  {
    resolvedRuns = ResolveTextRuns(node);
  }

  const std::vector<ResolvedTextRun>& runs    = projectLinks ? projection.resolvedRuns : resolvedRuns;
  Text::StyledTextBuilder             builder = Text::StyledTextBuilder::New(Dali::String(displayText->c_str()));

  for(const auto& run : runs)
  {
    const UiColor  runTextColor = (run.flags & MARKDOWN_TEXT_STYLE_INLINE_CODE) ? style.GetCodeTextColor() : TextColorForRole(node, style);
    const uint32_t fontFlags    = run.flags & MARKDOWN_TEXT_FONT_FLAGS;
    if(fontFlags != MARKDOWN_TEXT_STYLE_NONE)
    {
      AddSpanIfValid(builder, NewFontSpan(fontFlags, style), run.start, run.end);
    }
    if(run.flags & MARKDOWN_TEXT_STYLE_INLINE_CODE)
    {
      AddSpanIfValid(builder, Text::ForegroundColorSpan::New(style.GetCodeTextColor()), run.start, run.end);
      AddSpanIfValid(builder, Text::BackgroundColorSpan::New(style.GetInlineCodeBackgroundColor()), run.start, run.end);
    }
    if(run.flags & MARKDOWN_TEXT_STYLE_STRIKETHROUGH)
    {
      AddSpanIfValid(builder, NewLineThroughSpan(runTextColor), run.start, run.end);
    }
    if(run.flags & MARKDOWN_TEXT_STYLE_UNDERLINE)
    {
      AddSpanIfValid(builder, NewUnderlineSpan(runTextColor), run.start, run.end);
    }
    if(linkPresentation == MarkdownLinkPresentation::INTERACTIVE_ANCHOR && run.linkIndex >= 0)
    {
      const auto& link = node.linkRanges[static_cast<std::size_t>(run.linkIndex)];
      AddSpanIfValid(builder, NewAnchorSpan(link), run.start, run.end);
      AddSpanIfValid(builder, NewUnderlineSpan(UiColor(LINK_COLOR)), run.start, run.end);
    }
  }

  return builder.Build();
}

std::string CodeBlockDisplayText(const std::string& source)
{
  if(source.empty())
  {
    return source;
  }

  std::string text = source;
  if(text.back() == '\n')
  {
    text.pop_back();
    if(!text.empty() && text.back() == '\r')
    {
      text.pop_back();
    }
  }
  else if(text.back() == '\r')
  {
    text.pop_back();
  }
  return text;
}

/**
 * @brief Renders Markdown text through a Label-backed text component.
 */
class MarkdownLabelTextComponent : public MarkdownTextComponent
{
public:
  explicit MarkdownLabelTextComponent(const MarkdownViewStyle& style)
  : mLabel(Ui::Label::New()),
    mStyle(style)
  {
    mLabel.SetMultiLine(true);
    mLabel.SetTextOverflowMode(Text::OverflowMode::CLIP);
    mLabel.SetSystemFontSizeScaleEnabled(false);
    mLabel.SetFontFamily(mStyle.GetTextFontFamily());
    mLabel.SetAsyncRendering(false);
    mFontFamily    = mStyle.GetTextFontFamily();
    mHasFontFamily = true;
  }

  Ui::View GetView() const override
  {
    return mLabel;
  }

  void SetTextContent(const MarkdownRenderNode& node) override
  {
    ApplyStaticTextProperties(node);

    if(HasStyledRanges(node))
    {
      Text::StyledText styledText = BuildStyledText(node, DEFAULT_LINK_PRESENTATION, mStyle);
      mLabel.SetStyledText(styledText);
    }
    else
    {
      if(node.role == MarkdownRenderRole::CODE_BLOCK)
      {
        const std::string text = CodeBlockDisplayText(node.text);
        mLabel.SetText(Dali::String(text.c_str()));
      }
      else
      {
        mLabel.SetText(Dali::String(node.text.c_str()));
      }
    }

    mContentHash   = node.contentHash;
    mAttributeHash = node.attributeHash;
    mStyleHash     = node.styleHash;
  }

  void UpdateTextContent(const MarkdownRenderNode& node, const MarkdownTextUpdate& update) override
  {
    if(update.type == MarkdownTextUpdate::Type::UNCHANGED &&
       mContentHash == node.contentHash &&
       mAttributeHash == node.attributeHash &&
       mStyleHash == node.styleHash)
    {
      return;
    }
    SetTextContent(node);
  }

private:
  void ApplyStaticTextProperties(const MarkdownRenderNode& node)
  {
    const float fontSize = ResolveMarkdownTextFontSize(node, mStyle);
    if(mFontSize != fontSize)
    {
      mLabel.SetFontSize(fontSize);
      mFontSize = fontSize;
    }

    const Text::LineHeightMode lineHeightMode = ResolveMarkdownTextLineHeightMode(node);
    const float                lineHeight     = ResolveMarkdownTextLineHeight(node, mStyle);
    if(!mHasLineHeightMode || mLineHeightMode != lineHeightMode)
    {
      mLabel.SetLineHeightMode(lineHeightMode);
      mLineHeightMode    = lineHeightMode;
      mHasLineHeightMode = true;
    }
    if(mLineHeight != lineHeight)
    {
      mLabel.SetLineHeight(lineHeight);
      mLineHeight = lineHeight;
    }

    const UiColor textColor = TextColorForRole(node, mStyle);
    if(!mHasTextColor || mTextColor != textColor)
    {
      mLabel.SetTextColor(textColor);
      mTextColor    = textColor;
      mHasTextColor = true;
    }

    const Dali::String fontFamily = FontFamilyForRole(node, mStyle);
    if(!mHasFontFamily || mFontFamily != fontFamily)
    {
      mLabel.SetFontFamily(fontFamily);
      mFontFamily    = fontFamily;
      mHasFontFamily = true;
    }
  }

private:
  Ui::Label            mLabel;
  MarkdownViewStyle    mStyle;
  float                mFontSize{INVALID_FONT_SIZE};
  float                mLineHeight{NATURAL_LINE_HEIGHT};
  UiColor              mTextColor;
  Dali::String         mFontFamily;
  Text::LineHeightMode mLineHeightMode{Text::LineHeightMode::ABSOLUTE};
  bool                 mHasTextColor{false};
  bool                 mHasFontFamily{false};
  bool                 mHasLineHeightMode{false};
  uint64_t             mContentHash{0u};
  uint64_t             mAttributeHash{0u};
  uint64_t             mStyleHash{0u};
};

} // namespace

std::unique_ptr<MarkdownTextComponent> CreateMarkdownLabelTextComponent(const MarkdownViewStyle& style)
{
  return std::unique_ptr<MarkdownTextComponent>(new MarkdownLabelTextComponent(style));
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
