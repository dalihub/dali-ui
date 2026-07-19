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
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/vector4.h>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <utility>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/anchor.h>
#include <dali-ui-foundation/internal/text/character-set-conversion.h>
#include <dali-ui-foundation/internal/text/font-description-run.h>
#include <dali-ui-foundation/internal/text/logical-model-impl.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-projection.h>
#include <dali-ui-foundation/internal/text/styled-text/styled-text-applier.h>
#include <dali-ui-foundation/internal/text/styled-text/styled-text-impl.h>
#include <dali-ui-foundation/internal/text/text-font-style.h>
#include <dali-ui-foundation/public-api/text/styled-text/anchor-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/annotation-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/background-color-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/font-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/foreground-color-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/image-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/line-through-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/underline-span.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace Text
{

namespace
{
constexpr float PIXEL_FORMAT_64_FACTOR = 64.0f;

const StyledText* GetImplementation(const Dali::Ui::Text::StyledText& styledText)
{
  return static_cast<const StyledText*>(styledText.GetObjectPtr());
}

void ConvertTextToUtf32(const Dali::String& text, Dali::Vector<Dali::Ui::Text::Character>& utf32Text)
{
  utf32Text.Clear();

  if(text.Empty())
  {
    return;
  }

  utf32Text.Resize(text.Size());
  const uint32_t characterCount = Dali::Ui::Text::Utf8ToUtf32(reinterpret_cast<const uint8_t*>(text.CStr()),
                                                              text.Size(),
                                                              utf32Text.Begin());
  utf32Text.Resize(characterCount);
}

void ConvertUtf8TextToUtf32(const std::string& text, Dali::Vector<Dali::Ui::Text::Character>& utf32Text)
{
  utf32Text.Clear();

  if(text.empty())
  {
    return;
  }

  utf32Text.Resize(text.size());
  const uint32_t characterCount = Dali::Ui::Text::Utf8ToUtf32(reinterpret_cast<const uint8_t*>(text.c_str()),
                                                              text.size(),
                                                              utf32Text.Begin());
  utf32Text.Resize(characterCount);
}

void SortAttachmentsByInsertionOrder(std::vector<const SpanAttachment*>& attachments)
{
  std::stable_sort(attachments.begin(),
                   attachments.end(),
                   [](const SpanAttachment* lhs, const SpanAttachment* rhs)
  {
    return lhs->insertionOrder < rhs->insertionOrder;
  });
}

Dali::Ui::Text::UnderlineStyleProperties ToUnderlineStyleProperties(const Dali::Ui::Text::Underline& underline)
{
  return Dali::Ui::Text::UnderlineStyleProperties(underline.GetType(),
                                                  underline.GetColor().GetRgba(),
                                                  underline.GetThickness(),
                                                  underline.GetDashGap(),
                                                  underline.GetDashLength(),
                                                  true,
                                                  true,
                                                  true,
                                                  true,
                                                  true);
}

Dali::Ui::Text::StrikethroughStyleProperties ToStrikethroughStyleProperties(const Dali::Ui::Text::LineThrough& lineThrough)
{
  return Dali::Ui::Text::StrikethroughStyleProperties(lineThrough.GetColor().GetRgba(),
                                                      lineThrough.GetThickness(),
                                                      true,
                                                      true);
}

Dali::Ui::Text::PointSize26Dot6 ToPointSize26Dot6(float pixelSize, float dpi)
{
  DALI_ASSERT_ALWAYS(dpi > 0.0f && "FontSpan size conversion requires a valid DPI");

  const float pointSize = pixelSize * 72.0f / dpi;
  return static_cast<Dali::Ui::Text::PointSize26Dot6>(pointSize * PIXEL_FORMAT_64_FACTOR);
}

void CopyFontFamily(const std::string& family, Dali::Ui::Text::FontDescriptionRun& fontRun)
{
  fontRun.familyDefined = true;
  fontRun.familyLength  = family.size();
  fontRun.familyName    = new char[fontRun.familyLength > 0u ? fontRun.familyLength : 1u];
  if(fontRun.familyLength > 0u)
  {
    std::memcpy(fontRun.familyName, family.c_str(), fontRun.familyLength);
  }
}

std::string ToStdString(const Dali::String& string)
{
  return std::string(string.CStr(), string.Size());
}

Dali::Ui::Text::ReplacementVerticalAlignment ToReplacementAlignment(Dali::Ui::Text::ImageAttributes::InlineAlignment alignment)
{
  switch(alignment)
  {
    case Dali::Ui::Text::ImageAttributes::InlineAlignment::TEXT_BOTTOM:
      return Dali::Ui::Text::ReplacementVerticalAlignment::TEXT_BOTTOM;
    case Dali::Ui::Text::ImageAttributes::InlineAlignment::TEXT_CENTER:
      return Dali::Ui::Text::ReplacementVerticalAlignment::TEXT_CENTER;
    case Dali::Ui::Text::ImageAttributes::InlineAlignment::TEXT_BASELINE:
    default:
      return Dali::Ui::Text::ReplacementVerticalAlignment::TEXT_BASELINE;
  }
}

char* CopyToCString(const std::string& string)
{
  char* copiedString = new char[string.size() + 1u];
  if(!string.empty())
  {
    std::memcpy(copiedString, string.c_str(), string.size());
  }
  copiedString[string.size()] = '\0';
  return copiedString;
}

StyledTextColorRunSnapshot ToColorRunSnapshot(uint32_t startIndex, uint32_t endIndex, const Vector4& color)
{
  StyledTextColorRunSnapshot snapshot;
  snapshot.characterIndex     = startIndex;
  snapshot.numberOfCharacters = endIndex - startIndex;
  snapshot.color              = color;
  return snapshot;
}

Dali::Ui::Text::ColorRun ToColorRun(const StyledTextColorRunSnapshot& snapshot)
{
  Dali::Ui::Text::ColorRun colorRun;
  colorRun.characterRun.characterIndex     = snapshot.characterIndex;
  colorRun.characterRun.numberOfCharacters = snapshot.numberOfCharacters;
  colorRun.color                           = snapshot.color;
  return colorRun;
}

StyledTextFontRunSnapshot ToFontRunSnapshot(const Dali::Ui::Text::FontAttributes& attributes, uint32_t startIndex, uint32_t endIndex, float dpi)
{
  StyledTextFontRunSnapshot fontRun;
  fontRun.characterIndex     = startIndex;
  fontRun.numberOfCharacters = endIndex - startIndex;

  if(attributes.Has(Dali::Ui::Text::FontAttributes::Attribute::FAMILY))
  {
    fontRun.hasFamily = true;
    fontRun.family    = ToStdString(attributes.GetFamily());
  }

  if(attributes.Has(Dali::Ui::Text::FontAttributes::Attribute::SIZE))
  {
    fontRun.size    = ToPointSize26Dot6(attributes.GetSize(), dpi);
    fontRun.hasSize = true;
  }

  if(attributes.Has(Dali::Ui::Text::FontAttributes::Attribute::WEIGHT))
  {
    fontRun.weight    = Dali::Ui::Text::ToTextAbstractionFontWeight(attributes.GetWeight());
    fontRun.hasWeight = true;
  }

  if(attributes.Has(Dali::Ui::Text::FontAttributes::Attribute::WIDTH))
  {
    fontRun.width    = Dali::Ui::Text::ToTextAbstractionFontWidth(attributes.GetWidth());
    fontRun.hasWidth = true;
  }

  if(attributes.Has(Dali::Ui::Text::FontAttributes::Attribute::SLANT))
  {
    fontRun.slant    = Dali::Ui::Text::ToTextAbstractionFontSlant(attributes.GetSlant());
    fontRun.hasSlant = true;
  }

  return fontRun;
}

Dali::Ui::Text::FontDescriptionRun ToFontDescriptionRun(const StyledTextFontRunSnapshot& snapshot)
{
  Dali::Ui::Text::FontDescriptionRun fontRun;
  fontRun.characterRun.characterIndex     = snapshot.characterIndex;
  fontRun.characterRun.numberOfCharacters = snapshot.numberOfCharacters;

  if(snapshot.hasFamily)
  {
    CopyFontFamily(snapshot.family, fontRun);
  }

  if(snapshot.hasSize)
  {
    fontRun.size        = snapshot.size;
    fontRun.sizeDefined = true;
  }

  if(snapshot.hasWeight)
  {
    fontRun.weight        = snapshot.weight;
    fontRun.weightDefined = true;
  }

  if(snapshot.hasWidth)
  {
    fontRun.width        = snapshot.width;
    fontRun.widthDefined = true;
  }

  if(snapshot.hasSlant)
  {
    fontRun.slant        = snapshot.slant;
    fontRun.slantDefined = true;
  }

  return fontRun;
}

StyledTextUnderlineRunSnapshot ToUnderlineRunSnapshot(uint32_t startIndex, uint32_t endIndex, const Dali::Ui::Text::Underline& underline)
{
  StyledTextUnderlineRunSnapshot snapshot;
  snapshot.characterIndex     = startIndex;
  snapshot.numberOfCharacters = endIndex - startIndex;
  snapshot.properties         = ToUnderlineStyleProperties(underline);
  return snapshot;
}

Dali::Ui::Text::UnderlinedCharacterRun ToUnderlinedCharacterRun(const StyledTextUnderlineRunSnapshot& snapshot)
{
  Dali::Ui::Text::UnderlinedCharacterRun underlineRun;
  underlineRun.characterRun.characterIndex     = snapshot.characterIndex;
  underlineRun.characterRun.numberOfCharacters = snapshot.numberOfCharacters;
  underlineRun.properties                      = snapshot.properties;
  return underlineRun;
}

StyledTextLineThroughRunSnapshot ToLineThroughRunSnapshot(uint32_t startIndex, uint32_t endIndex, const Dali::Ui::Text::LineThrough& lineThrough)
{
  StyledTextLineThroughRunSnapshot snapshot;
  snapshot.characterIndex     = startIndex;
  snapshot.numberOfCharacters = endIndex - startIndex;
  snapshot.properties         = ToStrikethroughStyleProperties(lineThrough);
  return snapshot;
}

Dali::Ui::Text::StrikethroughCharacterRun ToStrikethroughCharacterRun(const StyledTextLineThroughRunSnapshot& snapshot)
{
  Dali::Ui::Text::StrikethroughCharacterRun strikethroughRun;
  strikethroughRun.characterRun.characterIndex     = snapshot.characterIndex;
  strikethroughRun.characterRun.numberOfCharacters = snapshot.numberOfCharacters;
  strikethroughRun.properties                      = snapshot.properties;
  return strikethroughRun;
}

StyledTextAnchorRunSnapshot ToAnchorRunSnapshot(const Dali::Ui::Text::AnchorAttributes& attributes,
                                                uint32_t                                startIndex,
                                                uint32_t                                endIndex,
                                                const Vector4&                          anchorColor,
                                                const Vector4&                          anchorClickedColor)
{
  StyledTextAnchorRunSnapshot snapshot;
  snapshot.characterIndex     = startIndex;
  snapshot.numberOfCharacters = endIndex - startIndex;
  snapshot.href               = ToStdString(attributes.GetHref());

  snapshot.hasColor = attributes.Has(Dali::Ui::Text::AnchorAttributes::Attribute::COLOR);
  snapshot.color    = snapshot.hasColor ? attributes.GetColor().GetRgba() : anchorColor;

  snapshot.hasClickedColor = attributes.Has(Dali::Ui::Text::AnchorAttributes::Attribute::CLICKED_COLOR);
  snapshot.clickedColor    = snapshot.hasClickedColor ? attributes.GetClickedColor().GetRgba() : anchorClickedColor;

  return snapshot;
}

Dali::Ui::Text::UnderlinedCharacterRun ToAnchorUnderlineRun(const StyledTextAnchorRunSnapshot& snapshot)
{
  Dali::Ui::Text::UnderlinedCharacterRun underlineRun;
  underlineRun.characterRun.characterIndex     = snapshot.characterIndex;
  underlineRun.characterRun.numberOfCharacters = snapshot.numberOfCharacters;
  underlineRun.properties.color                = snapshot.color;
  underlineRun.properties.colorDefined         = true;
  return underlineRun;
}

Dali::Ui::Text::Anchor ToAnchor(const StyledTextAnchorRunSnapshot& snapshot, uint32_t colorRunIndex, uint32_t underlinedCharacterRunIndex)
{
  Dali::Ui::Text::Anchor anchor{};
  anchor.startIndex                  = snapshot.characterIndex;
  anchor.endIndex                    = snapshot.characterIndex + snapshot.numberOfCharacters;
  anchor.href                        = CopyToCString(snapshot.href);
  anchor.colorRunIndex               = colorRunIndex;
  anchor.underlinedCharacterRunIndex = underlinedCharacterRunIndex;
  anchor.markupClickedColor          = snapshot.clickedColor;
  anchor.isMarkupColorSet            = snapshot.hasColor;
  anchor.isMarkupClickedColorSet     = snapshot.hasClickedColor;
  return anchor;
}

void ReleaseFontDescriptionRunFamilyNames(Dali::Vector<Dali::Ui::Text::FontDescriptionRun>& fontDescriptionRuns)
{
  for(auto it = fontDescriptionRuns.Begin(), endIt = fontDescriptionRuns.End(); it != endIt; ++it)
  {
    delete[](*it).familyName;
    (*it).familyName = nullptr;
  }

  fontDescriptionRuns.Clear();
}

} // unnamed namespace

StyledTextApplyResult::StyledTextApplyResult() = default;

StyledTextApplyResult::~StyledTextApplyResult()
{
  ReleaseFontDescriptionRuns();
}

StyledTextApplyResult::StyledTextApplyResult(StyledTextApplyResult&& rhs) noexcept
: text(std::move(rhs.text)),
  foregroundColorRuns(std::move(rhs.foregroundColorRuns)),
  backgroundColorRuns(std::move(rhs.backgroundColorRuns)),
  underlinedCharacterRuns(std::move(rhs.underlinedCharacterRuns)),
  strikethroughCharacterRuns(std::move(rhs.strikethroughCharacterRuns))
{
  fontDescriptionRuns.Swap(rhs.fontDescriptionRuns);
}

StyledTextApplyResult& StyledTextApplyResult::operator=(StyledTextApplyResult&& rhs) noexcept
{
  if(this != &rhs)
  {
    ReleaseFontDescriptionRuns();

    text                = std::move(rhs.text);
    foregroundColorRuns = std::move(rhs.foregroundColorRuns);
    backgroundColorRuns = std::move(rhs.backgroundColorRuns);
    fontDescriptionRuns.Swap(rhs.fontDescriptionRuns);
    underlinedCharacterRuns    = std::move(rhs.underlinedCharacterRuns);
    strikethroughCharacterRuns = std::move(rhs.strikethroughCharacterRuns);
  }

  return *this;
}

void StyledTextApplyResult::ReleaseFontDescriptionRuns()
{
  ReleaseFontDescriptionRunFamilyNames(fontDescriptionRuns);
}

void StyledTextApplyResult::TransferFontDescriptionRunsTo(Dali::Vector<Dali::Ui::Text::FontDescriptionRun>& target)
{
  DALI_ASSERT_ALWAYS(target.Count() == 0u && "Target font description runs must be cleared before transfer");

  // Swap transfers the internal vector buffer to the cleared target. The result
  // keeps the target's empty buffer, so its destructor cannot release the
  // familyName pointers now owned by the target.
  target.Swap(fontDescriptionRuns);
}

bool StyledTextApplier::HasAnchorSpans(const Dali::Ui::Text::StyledText& styledText)
{
  if(!styledText)
  {
    return false;
  }

  const StyledText* const impl = GetImplementation(styledText);
  if(nullptr == impl)
  {
    return false;
  }

  const auto& attachments = impl->GetAttachments();
  return std::any_of(attachments.begin(),
                     attachments.end(),
                     [](const SpanAttachment& attachment)
  {
    return static_cast<bool>(Dali::Ui::Text::AnchorSpan::DownCast(attachment.span));
  });
}

StyledTextStyleRunSnapshot StyledTextApplier::BuildTextStyleRunSnapshot(const Dali::Ui::Text::StyledText& styledText,
                                                                        float                             dpi,
                                                                        const Vector4&                    anchorColor,
                                                                        const Vector4&                    anchorClickedColor,
                                                                        bool                              includeAnchorSpans)
{
  DALI_ASSERT_ALWAYS(dpi > 0.0f && "StyledTextApplier requires a valid DPI");

  StyledTextStyleRunSnapshot snapshot;

  if(!styledText)
  {
    return snapshot;
  }

  const StyledText* const impl = GetImplementation(styledText);
  if(nullptr == impl)
  {
    return snapshot;
  }

  std::vector<const SpanAttachment*> orderedForegroundColorAttachments;
  std::vector<const SpanAttachment*> orderedBackgroundColorAttachments;
  std::vector<const SpanAttachment*> orderedFontAttachments;
  std::vector<const SpanAttachment*> orderedUnderlineAttachments;
  std::vector<const SpanAttachment*> orderedLineThroughAttachments;
  std::vector<const SpanAttachment*> orderedAnchorAttachments;
  const auto&                        attachments = impl->GetAttachments();
  orderedForegroundColorAttachments.reserve(attachments.size());
  orderedBackgroundColorAttachments.reserve(attachments.size());
  orderedFontAttachments.reserve(attachments.size());
  orderedUnderlineAttachments.reserve(attachments.size());
  orderedLineThroughAttachments.reserve(attachments.size());
  orderedAnchorAttachments.reserve(attachments.size());

  for(const auto& attachment : attachments)
  {
    if(Dali::Ui::Text::AnnotationSpan::DownCast(attachment.span))
    {
      continue;
    }
    if(Dali::Ui::Text::ForegroundColorSpan::DownCast(attachment.span))
    {
      orderedForegroundColorAttachments.push_back(&attachment);
    }
    else if(Dali::Ui::Text::BackgroundColorSpan::DownCast(attachment.span))
    {
      orderedBackgroundColorAttachments.push_back(&attachment);
    }
    else if(Dali::Ui::Text::FontSpan::DownCast(attachment.span))
    {
      orderedFontAttachments.push_back(&attachment);
    }
    else if(Dali::Ui::Text::UnderlineSpan::DownCast(attachment.span))
    {
      orderedUnderlineAttachments.push_back(&attachment);
    }
    else if(Dali::Ui::Text::LineThroughSpan::DownCast(attachment.span))
    {
      orderedLineThroughAttachments.push_back(&attachment);
    }
    else if(includeAnchorSpans && Dali::Ui::Text::AnchorSpan::DownCast(attachment.span))
    {
      orderedAnchorAttachments.push_back(&attachment);
    }
  }

  // Preserve SpanAttachment insertionOrder per style category. Merging and
  // overlap resolution remain in the existing downstream style paths.
  SortAttachmentsByInsertionOrder(orderedForegroundColorAttachments);
  SortAttachmentsByInsertionOrder(orderedBackgroundColorAttachments);
  SortAttachmentsByInsertionOrder(orderedFontAttachments);
  SortAttachmentsByInsertionOrder(orderedUnderlineAttachments);
  SortAttachmentsByInsertionOrder(orderedLineThroughAttachments);
  SortAttachmentsByInsertionOrder(orderedAnchorAttachments);

  snapshot.foregroundColorRuns.reserve(orderedForegroundColorAttachments.size());
  for(const SpanAttachment* attachment : orderedForegroundColorAttachments)
  {
    const Dali::Ui::Text::ForegroundColorSpan foregroundColorSpan = Dali::Ui::Text::ForegroundColorSpan::DownCast(attachment->span);
    if(!foregroundColorSpan || attachment->startIndex >= attachment->endIndex)
    {
      continue;
    }

    snapshot.foregroundColorRuns.push_back(ToColorRunSnapshot(attachment->startIndex,
                                                              attachment->endIndex,
                                                              foregroundColorSpan.GetColor().GetRgba()));
  }

  snapshot.backgroundColorRuns.reserve(orderedBackgroundColorAttachments.size());
  for(const SpanAttachment* attachment : orderedBackgroundColorAttachments)
  {
    const Dali::Ui::Text::BackgroundColorSpan backgroundColorSpan = Dali::Ui::Text::BackgroundColorSpan::DownCast(attachment->span);
    if(!backgroundColorSpan || attachment->startIndex >= attachment->endIndex)
    {
      continue;
    }

    snapshot.backgroundColorRuns.push_back(ToColorRunSnapshot(attachment->startIndex,
                                                              attachment->endIndex,
                                                              backgroundColorSpan.GetColor().GetRgba()));
  }

  snapshot.fontRuns.reserve(orderedFontAttachments.size());
  for(const SpanAttachment* attachment : orderedFontAttachments)
  {
    const Dali::Ui::Text::FontSpan fontSpan = Dali::Ui::Text::FontSpan::DownCast(attachment->span);
    if(!fontSpan || attachment->startIndex >= attachment->endIndex)
    {
      continue;
    }

    const Dali::Ui::Text::FontAttributes attributes = fontSpan.GetFontAttributes();
    if(!attributes.HasAttributes())
    {
      continue;
    }

    snapshot.fontRuns.push_back(ToFontRunSnapshot(attributes, attachment->startIndex, attachment->endIndex, dpi));
  }

  snapshot.underlineRuns.reserve(orderedUnderlineAttachments.size());
  for(const SpanAttachment* attachment : orderedUnderlineAttachments)
  {
    const Dali::Ui::Text::UnderlineSpan underlineSpan = Dali::Ui::Text::UnderlineSpan::DownCast(attachment->span);
    if(!underlineSpan || attachment->startIndex >= attachment->endIndex)
    {
      continue;
    }

    const Dali::Ui::Text::Underline underline = underlineSpan.GetUnderline();
    if(underline == Dali::Ui::Text::Underline::None())
    {
      continue;
    }

    snapshot.underlineRuns.push_back(ToUnderlineRunSnapshot(attachment->startIndex,
                                                            attachment->endIndex,
                                                            underline));
  }

  snapshot.lineThroughRuns.reserve(orderedLineThroughAttachments.size());
  for(const SpanAttachment* attachment : orderedLineThroughAttachments)
  {
    const Dali::Ui::Text::LineThroughSpan lineThroughSpan = Dali::Ui::Text::LineThroughSpan::DownCast(attachment->span);
    if(!lineThroughSpan || attachment->startIndex >= attachment->endIndex)
    {
      continue;
    }

    const Dali::Ui::Text::LineThrough lineThrough = lineThroughSpan.GetLineThrough();
    if(lineThrough == Dali::Ui::Text::LineThrough::None())
    {
      continue;
    }

    snapshot.lineThroughRuns.push_back(ToLineThroughRunSnapshot(attachment->startIndex,
                                                                attachment->endIndex,
                                                                lineThrough));
  }

  snapshot.anchorRuns.reserve(orderedAnchorAttachments.size());
  for(const SpanAttachment* attachment : orderedAnchorAttachments)
  {
    const Dali::Ui::Text::AnchorSpan anchorSpan = Dali::Ui::Text::AnchorSpan::DownCast(attachment->span);
    if(!anchorSpan || attachment->startIndex >= attachment->endIndex)
    {
      continue;
    }

    const Dali::Ui::Text::AnchorAttributes attributes = anchorSpan.GetAnchorAttributes();
    if(!attributes.Has(Dali::Ui::Text::AnchorAttributes::Attribute::HREF))
    {
      continue;
    }

    snapshot.anchorRuns.push_back(ToAnchorRunSnapshot(attributes,
                                                      attachment->startIndex,
                                                      attachment->endIndex,
                                                      anchorColor,
                                                      anchorClickedColor));
  }

  return snapshot;
}

Dali::Ui::Text::ReplacementSourceSnapshot StyledTextApplier::BuildReplacementSourceSnapshot(
  const Dali::Ui::Text::StyledText& styledText,
  uint64_t                          sourceRevision)
{
  Dali::Ui::Text::ReplacementSourceSnapshot snapshot;
  snapshot.sourceRevision = sourceRevision;
  if(!styledText)
  {
    return snapshot;
  }

  const StyledText* const impl = GetImplementation(styledText);
  if(nullptr == impl)
  {
    return snapshot;
  }

  // StyledTextBuilder stores attachments in insertion order. Extract each
  // ImageSpan in one pass so the ordinary no-image path does not allocate a
  // temporary pointer vector and an image is downcast only once.
  for(const SpanAttachment& attachment : impl->GetAttachments())
  {
    const Dali::Ui::Text::ImageSpan imageSpan = Dali::Ui::Text::ImageSpan::DownCast(attachment.span);
    if(!imageSpan)
    {
      continue;
    }

    const Dali::Ui::Text::ImageAttributes  attributes = imageSpan.GetImageAttributes();
    Dali::Ui::Text::ReplacementRunSnapshot run;
    run.logicalCharacterRange.characterIndex     = attachment.startIndex;
    run.logicalCharacterRange.numberOfCharacters = attachment.endIndex - attachment.startIndex;
    run.type                                     = Dali::Ui::Text::ReplacementType::IMAGE;
    run.occurrenceIdentity                       = static_cast<uint64_t>(attachment.insertionOrder) + 1u;

    if(attributes.Has(Dali::Ui::Text::ImageAttributes::Attribute::SOURCE))
    {
      run.image.source = ToStdString(attributes.GetSource());
    }
    if(attributes.Has(Dali::Ui::Text::ImageAttributes::Attribute::RESERVED_SIZE))
    {
      const Vector2 size = attributes.GetReservedSize();
      run.metrics.width  = size.width;
      run.metrics.height = size.height;
    }
    if(attributes.Has(Dali::Ui::Text::ImageAttributes::Attribute::ALIGNMENT))
    {
      run.metrics.verticalAlignment = ToReplacementAlignment(attributes.GetAlignment());
    }
    if(attributes.Has(Dali::Ui::Text::ImageAttributes::Attribute::VERTICAL_OFFSET))
    {
      run.metrics.verticalOffset = attributes.GetVerticalOffset();
    }

    snapshot.runs.PushBack(std::move(run));
  }

  if(!snapshot.runs.Empty())
  {
    Dali::Vector<Dali::Ui::Text::Character> logicalText;
    ConvertTextToUtf32(styledText.GetText(), logicalText);
    snapshot.hasValidReplacementSource =
      Dali::Ui::Text::ReplacementProjection::HasValidSource(logicalText, snapshot.runs);
  }
  return snapshot;
}

void StyledTextApplier::ApplySnapshotToLogicalModel(const StyledTextStyleRunSnapshot& snapshot, const std::string& utf8Text, Dali::Ui::Text::LogicalModel& logicalModel)
{
  ConvertUtf8TextToUtf32(utf8Text, logicalModel.mText);

  logicalModel.mColorRuns.Clear();
  logicalModel.mColorRuns.Reserve(static_cast<uint32_t>(snapshot.foregroundColorRuns.size() + snapshot.anchorRuns.size()));
  for(const auto& colorRunSnapshot : snapshot.foregroundColorRuns)
  {
    logicalModel.mColorRuns.PushBack(ToColorRun(colorRunSnapshot));
  }

  logicalModel.mBackgroundColorRuns.Clear();
  logicalModel.mBackgroundColorRuns.Reserve(static_cast<uint32_t>(snapshot.backgroundColorRuns.size()));
  for(const auto& backgroundRunSnapshot : snapshot.backgroundColorRuns)
  {
    logicalModel.mBackgroundColorRuns.PushBack(ToColorRun(backgroundRunSnapshot));
  }

  logicalModel.ClearFontDescriptionRuns();
  logicalModel.mFontDescriptionRuns.Reserve(static_cast<uint32_t>(snapshot.fontRuns.size()));
  for(const auto& fontRunSnapshot : snapshot.fontRuns)
  {
    logicalModel.mFontDescriptionRuns.PushBack(ToFontDescriptionRun(fontRunSnapshot));
  }

  logicalModel.mUnderlinedCharacterRuns.Clear();
  logicalModel.mUnderlinedCharacterRuns.Reserve(static_cast<uint32_t>(snapshot.underlineRuns.size() + snapshot.anchorRuns.size()));
  for(const auto& underlineRunSnapshot : snapshot.underlineRuns)
  {
    logicalModel.mUnderlinedCharacterRuns.PushBack(ToUnderlinedCharacterRun(underlineRunSnapshot));
  }

  logicalModel.ClearAnchors();
  logicalModel.mAnchors.Reserve(static_cast<uint32_t>(snapshot.anchorRuns.size()));
  for(const auto& anchorRunSnapshot : snapshot.anchorRuns)
  {
    const uint32_t colorRunIndex = logicalModel.mColorRuns.Count();
    logicalModel.mColorRuns.PushBack(ToColorRun(StyledTextColorRunSnapshot{
      anchorRunSnapshot.characterIndex,
      anchorRunSnapshot.numberOfCharacters,
      anchorRunSnapshot.color}));

    const uint32_t underlineRunIndex = logicalModel.mUnderlinedCharacterRuns.Count();
    logicalModel.mUnderlinedCharacterRuns.PushBack(ToAnchorUnderlineRun(anchorRunSnapshot));

    logicalModel.mAnchors.PushBack(ToAnchor(anchorRunSnapshot, colorRunIndex, underlineRunIndex));
  }

  logicalModel.mStrikethroughCharacterRuns.Clear();
  logicalModel.mStrikethroughCharacterRuns.Reserve(static_cast<uint32_t>(snapshot.lineThroughRuns.size()));
  for(const auto& lineThroughRunSnapshot : snapshot.lineThroughRuns)
  {
    logicalModel.mStrikethroughCharacterRuns.PushBack(ToStrikethroughCharacterRun(lineThroughRunSnapshot));
  }
}

StyledTextApplyResult StyledTextApplier::BuildTextStyleRunResult(const Dali::Ui::Text::StyledText& styledText,
                                                                 float                             dpi,
                                                                 const Vector4&                    anchorColor,
                                                                 const Vector4&                    anchorClickedColor,
                                                                 bool                              includeAnchorSpans)
{
  StyledTextApplyResult result;

  if(styledText)
  {
    ConvertTextToUtf32(styledText.GetText(), result.text);
  }

  const StyledTextStyleRunSnapshot snapshot = BuildTextStyleRunSnapshot(styledText, dpi, anchorColor, anchorClickedColor, includeAnchorSpans);

  result.foregroundColorRuns.Reserve(static_cast<uint32_t>(snapshot.foregroundColorRuns.size()));
  for(const auto& colorRunSnapshot : snapshot.foregroundColorRuns)
  {
    result.foregroundColorRuns.PushBack(ToColorRun(colorRunSnapshot));
  }

  result.backgroundColorRuns.Reserve(static_cast<uint32_t>(snapshot.backgroundColorRuns.size()));
  for(const auto& backgroundRunSnapshot : snapshot.backgroundColorRuns)
  {
    result.backgroundColorRuns.PushBack(ToColorRun(backgroundRunSnapshot));
  }

  result.fontDescriptionRuns.Reserve(static_cast<uint32_t>(snapshot.fontRuns.size()));
  for(const auto& fontRunSnapshot : snapshot.fontRuns)
  {
    result.fontDescriptionRuns.PushBack(ToFontDescriptionRun(fontRunSnapshot));
  }

  result.underlinedCharacterRuns.Reserve(static_cast<uint32_t>(snapshot.underlineRuns.size()));
  for(const auto& underlineRunSnapshot : snapshot.underlineRuns)
  {
    result.underlinedCharacterRuns.PushBack(ToUnderlinedCharacterRun(underlineRunSnapshot));
  }

  for(const auto& anchorRunSnapshot : snapshot.anchorRuns)
  {
    result.foregroundColorRuns.PushBack(ToColorRun(StyledTextColorRunSnapshot{
      anchorRunSnapshot.characterIndex,
      anchorRunSnapshot.numberOfCharacters,
      anchorRunSnapshot.color}));
    result.underlinedCharacterRuns.PushBack(ToAnchorUnderlineRun(anchorRunSnapshot));
  }

  result.strikethroughCharacterRuns.Reserve(static_cast<uint32_t>(snapshot.lineThroughRuns.size()));
  for(const auto& lineThroughRunSnapshot : snapshot.lineThroughRuns)
  {
    result.strikethroughCharacterRuns.PushBack(ToStrikethroughCharacterRun(lineThroughRunSnapshot));
  }

  return result;
}

void StyledTextApplier::ApplyTextAndStyleRunsToLogicalModel(const Dali::Ui::Text::StyledText& styledText,
                                                            Dali::Ui::Text::LogicalModel&     logicalModel,
                                                            float                             dpi,
                                                            const Vector4&                    anchorColor,
                                                            const Vector4&                    anchorClickedColor,
                                                            bool                              includeAnchorSpans)
{
  const StyledTextStyleRunSnapshot snapshot = BuildTextStyleRunSnapshot(styledText, dpi, anchorColor, anchorClickedColor, includeAnchorSpans);
  const std::string                utf8Text = styledText ? ToStdString(styledText.GetText()) : std::string();
  ApplySnapshotToLogicalModel(snapshot, utf8Text, logicalModel);
}

} // namespace Text
} // namespace Internal
} // namespace Ui
} // namespace Dali
