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
#include <dali-ui-foundation/internal/text/text-visualizer/text-visualizer-view-interface.h>

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/text-visualizer/atlas-view-adapter.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>

namespace Dali::Ui::Internal::TextVisualizer
{
namespace
{
const Vector2& ZeroVector2()
{
  return Vector2::ZERO;
}

const Vector4& ZeroVector4()
{
  return Vector4::ZERO;
}

const Vector4& DefaultTextColor()
{
  static const Vector4 defaultTextColor(0.0f, 0.0f, 0.0f, 1.0f);
  return defaultTextColor;
}

const Vector<Text::BoundedParagraphRun>& EmptyBoundedParagraphRuns()
{
  static const Vector<Text::BoundedParagraphRun> emptyRuns;
  return emptyRuns;
}

const Vector<Text::CharacterIndex>& EmptyGlyphToCharacterMap()
{
  static const Vector<Text::CharacterIndex> emptyMap;
  return emptyMap;
}
} // unnamed namespace

TextVisualizerViewInterface::TextVisualizerViewInterface()
: mAdapter(nullptr)
{
}

TextVisualizerViewInterface::~TextVisualizerViewInterface() = default;

void TextVisualizerViewInterface::SetAdapter(const AtlasViewAdapter* adapter)
{
  mAdapter = adapter;
  ResetDiagnostics();
}

void TextVisualizerViewInterface::Clear()
{
  mAdapter = nullptr;
  ResetDiagnostics();
}

bool TextVisualizerViewInterface::HasAdapter() const
{
  return (nullptr != mAdapter);
}

void TextVisualizerViewInterface::ResetDiagnostics() const
{
  mDiagnostics = Diagnostics{};
}

const TextVisualizerViewInterface::Diagnostics& TextVisualizerViewInterface::GetDiagnostics() const
{
  return mDiagnostics;
}

const Vector2& TextVisualizerViewInterface::GetControlSize() const
{
  return HasAdapter() ? mAdapter->GetControlSize() : ZeroVector2();
}

const Vector2& TextVisualizerViewInterface::GetLayoutSize() const
{
  return HasAdapter() ? mAdapter->GetLayoutSize() : ZeroVector2();
}

Text::Length TextVisualizerViewInterface::GetNumberOfGlyphs() const
{
  return HasAdapter() ? mAdapter->GetRenderableGlyphCount() : 0u;
}

Text::Length TextVisualizerViewInterface::GetGlyphs(Text::GlyphInfo* glyphs, Vector2* glyphPositions, float& minLineOffset,
                                                    Text::GlyphIndex glyphIndex, Text::Length numberOfGlyphs) const
{
  minLineOffset = 0.0f;
  ++mDiagnostics.getGlyphsCallCount;
  mDiagnostics.lastGlyphStartIndex     = glyphIndex;
  mDiagnostics.lastRequestedGlyphCount = numberOfGlyphs;
  mDiagnostics.lastReturnedGlyphCount  = 0u;

  if(!HasAdapter() || !glyphs || !glyphPositions)
  {
    return 0u;
  }

  const Text::Length renderableGlyphCount = mAdapter->GetRenderableGlyphCount();
  if(glyphIndex >= renderableGlyphCount)
  {
    return 0u;
  }

  const Text::Length count = std::min(numberOfGlyphs, renderableGlyphCount - glyphIndex);
  for(Text::Length index = 0u; index < count; ++index)
  {
    const uint32_t  placementIndex = glyphIndex + index;
    GlyphPlacement  placement;
    Text::GlyphInfo glyphInfo;

    if(!mAdapter->GetGlyphPlacement(placementIndex, placement) ||
       !mAdapter->GetGlyphInfo(placement.glyphIndex, glyphInfo))
    {
      mDiagnostics.lastReturnedGlyphCount = index;
      return index;
    }

    glyphs[index] = glyphInfo;
    if(!mAdapter->GetRendererGlyphPosition(placementIndex, glyphPositions[index]))
    {
      mDiagnostics.lastReturnedGlyphCount = index;
      return index;
    }
  }

  mDiagnostics.lastReturnedGlyphCount = count;
  return count;
}

const Vector4* TextVisualizerViewInterface::GetColors() const
{
  return nullptr;
}

const Text::ColorIndex* TextVisualizerViewInterface::GetColorIndices() const
{
  return nullptr;
}

const Vector4* TextVisualizerViewInterface::GetBackgroundColors() const
{
  return nullptr;
}

const Text::ColorIndex* TextVisualizerViewInterface::GetBackgroundColorIndices() const
{
  return nullptr;
}

bool TextVisualizerViewInterface::IsMarkupBackgroundColorSet() const
{
  return false;
}

const Vector4& TextVisualizerViewInterface::GetTextColor() const
{
  return HasAdapter() ? mAdapter->GetTextColor() : DefaultTextColor();
}

const Vector2& TextVisualizerViewInterface::GetShadowOffset() const
{
  return ZeroVector2();
}

const Vector4& TextVisualizerViewInterface::GetShadowColor() const
{
  return ZeroVector4();
}

const Vector4& TextVisualizerViewInterface::GetUnderlineColor() const
{
  return ZeroVector4();
}

bool TextVisualizerViewInterface::IsUnderlineEnabled() const
{
  return false;
}

bool TextVisualizerViewInterface::IsMarkupUnderlineSet() const
{
  return false;
}

const Text::GlyphInfo* TextVisualizerViewInterface::GetHyphens() const
{
  return nullptr;
}

const Text::Length* TextVisualizerViewInterface::GetHyphenIndices() const
{
  return nullptr;
}

Text::Length TextVisualizerViewInterface::GetHyphensCount() const
{
  return 0u;
}

float TextVisualizerViewInterface::GetUnderlineHeight() const
{
  return 0.0f;
}

Text::Underline::Type TextVisualizerViewInterface::GetUnderlineType() const
{
  return Text::Underline::Type::SOLID;
}

float TextVisualizerViewInterface::GetDashedUnderlineWidth() const
{
  return 0.0f;
}

float TextVisualizerViewInterface::GetDashedUnderlineGap() const
{
  return 0.0f;
}

Text::Length TextVisualizerViewInterface::GetNumberOfUnderlineRuns() const
{
  return 0u;
}

void TextVisualizerViewInterface::GetUnderlineRuns(Text::UnderlinedGlyphRun* underlineRuns, Text::UnderlineRunIndex index,
                                                   Text::Length numberOfRuns) const
{
  (void)underlineRuns;
  (void)index;
  (void)numberOfRuns;
}

const Vector2& TextVisualizerViewInterface::GetOutlineOffset() const
{
  return ZeroVector2();
}

const Vector4& TextVisualizerViewInterface::GetOutlineColor() const
{
  return ZeroVector4();
}

uint16_t TextVisualizerViewInterface::GetOutlineWidth() const
{
  return 0u;
}

Text::EllipsisPosition::Type TextVisualizerViewInterface::GetEllipsisPosition() const
{
  return Text::EllipsisPosition::END;
}

bool TextVisualizerViewInterface::IsTextElideEnabled() const
{
  return false;
}

Text::GlyphIndex TextVisualizerViewInterface::GetStartIndexOfElidedGlyphs() const
{
  return 0u;
}

Text::GlyphIndex TextVisualizerViewInterface::GetEndIndexOfElidedGlyphs() const
{
  const Text::Length glyphCount = GetNumberOfGlyphs();
  return (glyphCount > 0u) ? (glyphCount - 1u) : 0u;
}

Text::GlyphIndex TextVisualizerViewInterface::GetFirstMiddleIndexOfElidedGlyphs() const
{
  return 0u;
}

Text::GlyphIndex TextVisualizerViewInterface::GetSecondMiddleIndexOfElidedGlyphs() const
{
  return 0u;
}

const Vector4& TextVisualizerViewInterface::GetStrikethroughColor() const
{
  return ZeroVector4();
}

bool TextVisualizerViewInterface::IsStrikethroughEnabled() const
{
  return false;
}

bool TextVisualizerViewInterface::IsMarkupStrikethroughSet() const
{
  return false;
}

float TextVisualizerViewInterface::GetStrikethroughHeight() const
{
  return 0.0f;
}

Text::Length TextVisualizerViewInterface::GetNumberOfStrikethroughRuns() const
{
  return 0u;
}

Text::Length TextVisualizerViewInterface::GetNumberOfBoundedParagraphRuns() const
{
  return 0u;
}

const Vector<Text::BoundedParagraphRun>& TextVisualizerViewInterface::GetBoundedParagraphRuns() const
{
  return EmptyBoundedParagraphRuns();
}

void TextVisualizerViewInterface::GetStrikethroughRuns(Text::StrikethroughGlyphRun* strikethroughRuns,
                                                       Text::StrikethroughRunIndex index, Text::Length numberOfRuns) const
{
  (void)strikethroughRuns;
  (void)index;
  (void)numberOfRuns;
}

float TextVisualizerViewInterface::GetCharacterSpacing() const
{
  return 0.0f;
}

const Text::Character* TextVisualizerViewInterface::GetTextBuffer() const
{
  return HasAdapter() ? mAdapter->GetTextBuffer() : nullptr;
}

const Vector<Text::CharacterIndex>& TextVisualizerViewInterface::GetGlyphsToCharacters() const
{
  return HasAdapter() ? mAdapter->GetGlyphToCharacterMap() : EmptyGlyphToCharacterMap();
}

bool TextVisualizerViewInterface::IsCutoutEnabled() const
{
  return false;
}

} // namespace Dali::Ui::Internal::TextVisualizer
