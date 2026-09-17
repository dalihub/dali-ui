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

#include <dali-ui-foundation/internal/text/rendering/text-typesetter.h>
#include <dali-ui-foundation/internal/text/reveal/text-reveal.h>
#include <dali-ui-foundation/internal/text/styled-text/gradient-span-data.h>
#include <dali-ui-foundation/internal/text/text-model-interface.h>
#include <dali-ui-test-suite-utils.h>
#include <dali-ui/test-font-glyph-constants.h>
#include <dali.h>
#include <dali/integration-api/pixel-data-integ.h>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <cmath>
#include <limits>
#include <dali-ui-foundation/internal/text/rendering/text-raster-coordinate.h>

using namespace Dali;

namespace
{

namespace UiText = Dali::Ui::Text;

const Vector2 MASK_SIZE(240.0f, 96.0f);

class MaskModel : public UiText::ModelInterface
{
public:
  MaskModel(UiText::Length glyphCount,
            std::initializer_list<UiText::ColorIndex> colorIndices = {},
            bool styleEnabled = false)
  : mStyleEnabled(styleEnabled)
  {
    mGlyphs.Resize(glyphCount);
    mPositions.Resize(glyphCount);
    mCharacters.Resize(glyphCount);
    mGlyphToCharacters.Resize(glyphCount);

    for(UiText::GlyphIndex index = 0u; index < glyphCount; ++index)
    {
      UiText::GlyphInfo& glyph = mGlyphs[index];
      glyph.fontId             = 1u;
      glyph.index              = index + 1u;
      glyph.xBearing           = 0.0f;
      glyph.yBearing           = 4.0f;
      glyph.width              = 2.0f;
      glyph.height             = 4.0f;
      glyph.advance            = 8.0f;

      mPositions[index]         = Vector2(static_cast<float>(index) * 8.0f, 0.0f);
      mCharacters[index]        = 'A' + index;
      mGlyphToCharacters[index] = index;
    }

    if(glyphCount > 0u)
    {
      UiText::LineRun line{};
      line.glyphRun.glyphIndex        = 0u;
      line.glyphRun.numberOfGlyphs    = glyphCount;
      line.characterRun.characterIndex = 0u;
      line.characterRun.numberOfCharacters = glyphCount;
      line.width                      = static_cast<float>(glyphCount) * 8.0f;
      line.ascender                   = 0.0f;
      line.descender                  = 0.0f;
      line.alignmentOffset            = 0.0f;
      line.direction                  = false;
      line.ellipsis                   = false;
      line.isSplitToTwoHalves         = false;
      mLines.PushBack(line);
    }

    if(colorIndices.size() != 0u)
    {
      mColors.PushBack(Color::RED);
      mColorIndices.Reserve(static_cast<uint32_t>(colorIndices.size()));
      for(auto colorIndex : colorIndices)
      {
        mColorIndices.PushBack(colorIndex);
      }
    }
  }

  const Size& GetControlSize() const override { return mControlSize; }
  const Size& GetLayoutSize() const override { return mLayoutSize; }
  const Vector2& GetScrollPosition() const override { return mScrollPosition; }
  UiText::Alignment GetHorizontalAlignment() const override { return UiText::Alignment::START; }
  UiText::Alignment GetVerticalAlignment() const override { return mVerticalAlignment; }
  UiText::Alignment GetVerticalLineAlignment() const override { return UiText::Alignment::START; }
  UiText::EllipsisPosition::Type GetEllipsisPosition() const override { return UiText::EllipsisPosition::END; }
  bool IsTextElideEnabled() const override { return false; }
  UiText::Length GetNumberOfLines() const override { return static_cast<Dali::Ui::Text::Length>(mLines.Count()); }
  const UiText::LineRun* GetLines() const override { return mLines.Begin(); }
  UiText::Length GetNumberOfScripts() const override { return 0u; }
  const UiText::ScriptRun* GetScriptRuns() const override { return nullptr; }
  UiText::Length GetNumberOfCharacters() const override { return static_cast<Dali::Ui::Text::Length>(mCharacters.Count()); }
  UiText::Length GetNumberOfGlyphs() const override { return static_cast<Dali::Ui::Text::Length>(mGlyphs.Count()); }
  UiText::GlyphIndex GetStartIndexOfElidedGlyphs() const override { return 0u; }
  UiText::GlyphIndex GetEndIndexOfElidedGlyphs() const override { return static_cast<Dali::Ui::Text::GlyphIndex>(mGlyphs.Count() > 0u ? mGlyphs.Count() - 1u : 0u); }
  UiText::GlyphIndex GetFirstMiddleIndexOfElidedGlyphs() const override { return 0u; }
  UiText::GlyphIndex GetSecondMiddleIndexOfElidedGlyphs() const override { return 0u; }
  const UiText::GlyphInfo* GetGlyphs() const override { return mGlyphs.Begin(); }
  const Vector2* GetLayout() const override { return mPositions.Begin(); }
  const Vector4* GetColors() const override { return mColors.Count() > 0u ? mColors.Begin() : nullptr; }
  const UiText::ColorIndex* GetColorIndices() const override { return mColorIndices.Count() > 0u ? mColorIndices.Begin() : nullptr; }
  const UiText::Internal::GradientSpanModelData* GetGradientSpanModelData() const override
  {
    return mHasGradientSpan ? &mGradientSpanData : nullptr;
  }
  const Vector4* GetBackgroundColors() const override { return mBackgroundIndices.Empty() ? nullptr : &mShadowColor; }
  const UiText::ColorIndex* GetBackgroundColorIndices() const override { return mBackgroundIndices.Empty() ? nullptr : mBackgroundIndices.Begin(); }
  bool IsMarkupBackgroundColorSet() const override { return !mBackgroundIndices.Empty(); }
  const Vector4& GetDefaultColor() const override { return mDefaultColor; }
  const Vector2& GetShadowOffset() const override { return mStyleEnabled ? mShadowOffset : mZeroVector; }
  bool IsShadowEnabled() const override { return mStyleEnabled; }
  const Vector4& GetShadowColor() const override { return mStyleEnabled ? mShadowColor : mTransparentColor; }
  const float& GetShadowBlurRadius() const override { return mZeroFloat; }
  const Vector4& GetUnderlineColor() const override { return mStyleEnabled ? mUnderlineColor : mTransparentColor; }
  bool IsUnderlineEnabled() const override { return mStyleEnabled; }
  bool IsMarkupUnderlineSet() const override { return !mUnderlineRuns.Empty(); }
  float GetUnderlineHeight() const override { return mUnderlineHeight; }
  UiText::Underline::Type GetUnderlineType() const override { return mUnderlineType; }
  float GetDashedUnderlineWidth() const override { return 2.0f; }
  float GetDashedUnderlineGap() const override { return 2.0f; }
  UiText::Length GetNumberOfUnderlineRuns() const override { return static_cast<UiText::Length>(mUnderlineRuns.Count()); }
  void GetUnderlineRuns(UiText::UnderlinedGlyphRun* runs, UiText::UnderlineRunIndex index, UiText::Length count) const override
  {
    for(UiText::Length i = 0u; i < count; ++i) { runs[i] = mUnderlineRuns[index + i]; }
  }
  const Vector2& GetOutlineOffset() const override { return mZeroVector; }
  const Vector4& GetOutlineColor() const override { return mShadowColor; }
  uint16_t GetOutlineWidth() const override { return mOutlineWidth; }
  bool IsOutlineEnabled() const override { return mOutlineWidth != 0u; }
  const float& GetOutlineBlurRadius() const override { return mZeroFloat; }
  const Vector4& GetBackgroundColor() const override { return mTransparentColor; }
  bool IsBackgroundEnabled() const override { return false; }
  const UiText::GlyphInfo* GetHyphens() const override { return nullptr; }
  const UiText::Length* GetHyphenIndices() const override { return nullptr; }
  UiText::Length GetHyphensCount() const override { return 0u; }
  const Vector4& GetStrikethroughColor() const override { return mUnderlineColor; }
  bool IsStrikethroughEnabled() const override { return mStrikethroughHeight > 0.0f; }
  bool IsMarkupStrikethroughSet() const override { return !mStrikethroughRuns.Empty(); }
  float GetStrikethroughHeight() const override { return mStrikethroughHeight; }
  UiText::Length GetNumberOfStrikethroughRuns() const override { return static_cast<UiText::Length>(mStrikethroughRuns.Count()); }
  UiText::Length GetNumberOfBoundedParagraphRuns() const override { return 0u; }
  const Vector<UiText::BoundedParagraphRun>& GetBoundedParagraphRuns() const override { return mBoundedParagraphRuns; }
  UiText::Length GetNumberOfCharacterSpacingGlyphRuns() const override { return 0u; }
  const Vector<UiText::CharacterSpacingGlyphRun>& GetCharacterSpacingGlyphRuns() const override { return mCharacterSpacingGlyphRuns; }
  void GetStrikethroughRuns(UiText::StrikethroughGlyphRun* runs, UiText::StrikethroughRunIndex index, UiText::Length count) const override
  {
    for(UiText::Length i = 0u; i < count; ++i) { runs[i] = mStrikethroughRuns[index + i]; }
  }
  float GetCharacterSpacing() const override { return 0.0f; }
  const UiText::Character* GetTextBuffer() const override { return mCharacters.Begin(); }
  const Vector<UiText::CharacterIndex>& GetGlyphsToCharacters() const override { return mGlyphToCharacters; }
  const Vector<UiText::FontRun>& GetFontRuns() const override { return mFontRuns; }
  const Vector<UiText::FontDescriptionRun>& GetFontDescriptionRuns() const override { return mFontDescriptionRuns; }
  bool IsRemoveFrontInset() const override { return false; }
  bool IsRemoveBackInset() const override { return false; }
  bool IsCutoutEnabled() const override { return false; }
  const bool IsBackgroundWithCutoutEnabled() const override { return false; }
  const Vector4& GetBackgroundColorWithCutout() const override { return mTransparentColor; }
  const Vector2& GetOffsetWithCutout() const override { return mZeroVector; }
  const Vector<UiText::CharacterDirection>& GetCharacterDirections() const override { return mCharacterDirections; }

  void SetGradientSpanGlyphRange(UiText::GlyphIndex begin, UiText::Length count)
  {
    mGradientSpanData.paints.Resize(1u);
    auto& style        = mGradientSpanData.paints[0u].style;
    style.enabled      = true;
    style.type         = Dali::Ui::Gradient::Type::LINEAR;
    style.linearStart  = Vector2(0.0f, 0.0f);
    style.linearEnd    = Vector2(1.0f, 0.0f);
    style.stops.PushBack({0.0f, Color::RED});
    style.stops.PushBack({1.0f, Color::BLUE});
    mGradientSpanData.glyphPaintIndices.Resize(mGlyphs.Count());
    const UiText::GlyphIndex end = std::min<UiText::GlyphIndex>(
      begin + count,
      static_cast<UiText::GlyphIndex>(mGlyphs.Count()));
    for(UiText::GlyphIndex index = begin; index < end; ++index)
    {
      mGradientSpanData.glyphPaintIndices[index] = 1u;
    }
    mHasGradientSpan = true;
  }

  void SetSingleGradientSpan(const Vector4& color,
                             const Vector2& position   = Vector2::ZERO,
                             uint32_t       glyphIndex = 1u)
  {
    DALI_ASSERT_ALWAYS(mGlyphs.Count() >= 1u);

    mPositions[0u]    = position;
    mGlyphs[0u].index = glyphIndex;
    mGradientSpanData.paints.Resize(1u);
    auto& paint             = mGradientSpanData.paints[0u];
    paint.style.enabled     = true;
    paint.style.type        = Dali::Ui::Gradient::Type::LINEAR;
    paint.style.linearStart = Vector2(0.0f, 0.0f);
    paint.style.linearEnd   = Vector2(1.0f, 0.0f);
    paint.style.stops.PushBack({0.0f, color});
    paint.style.stops.PushBack({1.0f, color});

    mGradientSpanData.glyphPaintIndices.Resize(mGlyphs.Count());
    mGradientSpanData.glyphPaintIndices[0u] = 1u;
    mHasGradientSpan                        = true;
  }

  void SetOverlappingGradientSpanGlyphs(const Vector4& firstColor,
                                        const Vector4& secondColor,
                                        const Vector2& secondOffset     = Vector2::ZERO,
                                        uint32_t       secondGlyphIndex = 0u)
  {
    DALI_ASSERT_ALWAYS(mGlyphs.Count() >= 2u);

    mPositions[1u]    = mPositions[0u] + secondOffset;
    mGlyphs[1u].index = secondGlyphIndex == 0u ? mGlyphs[0u].index : secondGlyphIndex;

    mGradientSpanData.paints.Resize(2u);
    auto setSolidPaint = [](UiText::Internal::GradientSpanPaint& paint, const Vector4& color)
    {
      paint.style.enabled     = true;
      paint.style.type        = Dali::Ui::Gradient::Type::LINEAR;
      paint.style.linearStart = Vector2(0.0f, 0.0f);
      paint.style.linearEnd   = Vector2(1.0f, 0.0f);
      paint.style.stops.PushBack({0.0f, color});
      paint.style.stops.PushBack({1.0f, color});
    };
    setSolidPaint(mGradientSpanData.paints[0u], firstColor);
    setSolidPaint(mGradientSpanData.paints[1u], secondColor);

    mGradientSpanData.glyphPaintIndices.Resize(mGlyphs.Count());
    mGradientSpanData.glyphPaintIndices[0u] = 1u;
    mGradientSpanData.glyphPaintIndices[1u] = 2u;
    mHasGradientSpan                        = true;
  }

  void SetOverlappingMixedGlyphs(bool           firstUsesGradient,
                                 bool           secondUsesGradient,
                                 const Vector4& gradientColor,
                                 const Vector4& ordinaryColor,
                                 uint32_t       secondGlyphIndex)
  {
    DALI_ASSERT_ALWAYS(mGlyphs.Count() >= 2u);

    mPositions[1u]    = mPositions[0u];
    mGlyphs[1u].index = secondGlyphIndex;
    mDefaultColor     = ordinaryColor;
    DALI_ASSERT_ALWAYS(mColors.Count() > 0u);
    mColors[0u] = ordinaryColor;

    mGradientSpanData.paints.Resize(1u);
    auto& paint             = mGradientSpanData.paints[0u];
    paint.style.enabled     = true;
    paint.style.type        = Dali::Ui::Gradient::Type::LINEAR;
    paint.style.linearStart = Vector2(0.0f, 0.0f);
    paint.style.linearEnd   = Vector2(1.0f, 0.0f);
    paint.style.stops.PushBack({0.0f, gradientColor});
    paint.style.stops.PushBack({1.0f, gradientColor});

    mGradientSpanData.glyphPaintIndices.Resize(mGlyphs.Count());
    for(UiText::GlyphIndex index = 0u; index < mGlyphs.Count(); ++index)
    {
      mGradientSpanData.glyphPaintIndices[index] = 0u;
    }
    mGradientSpanData.glyphPaintIndices[0u] = firstUsesGradient ? 1u : 0u;
    mGradientSpanData.glyphPaintIndices[1u] = secondUsesGradient ? 1u : 0u;
    mHasGradientSpan                        = true;
  }

  void SetGlyphPosition(UiText::GlyphIndex index, const Vector2& position)
  {
    DALI_ASSERT_ALWAYS(index < mPositions.Count());
    mPositions[index] = position;
  }

  void SetGlyphBitmapIndex(UiText::GlyphIndex index, uint32_t bitmapIndex)
  {
    DALI_ASSERT_ALWAYS(index < mGlyphs.Count());
    mGlyphs[index].index = bitmapIndex;
  }

  void SetDefaultColor(const Vector4& color)
  {
    mDefaultColor = color;
  }

  void SetForegroundColor(const Vector4& color)
  {
    DALI_ASSERT_ALWAYS(mColors.Count() > 0u);
    mColors[0u] = color;
  }

  void SetAlignmentOffset(float value) { mLines[0u].alignmentOffset = value; }
  void SetLineAscender(float value) { mLines[0u].ascender = value; }
  void SetGlyphSize(float width, float height) { mGlyphs[0u].width = width; mGlyphs[0u].height = height; }
  void SetShadowOffset(const Vector2& offset) { mShadowOffset = offset; }
  void SetVerticalAlignment(UiText::Alignment alignment) { mVerticalAlignment = alignment; }
  void SetDecoration(UiText::Underline::Type type, float height, float strikeHeight = 0.0f, uint16_t outline = 0u)
  {
    mUnderlineType = type;
    mUnderlineHeight = height;
    mStrikethroughHeight = strikeHeight;
    mOutlineWidth = outline;
  }
  void SetBackgroundSpan()
  {
    mBackgroundIndices.Resize(mGlyphs.Count());
    std::fill(mBackgroundIndices.Begin(), mBackgroundIndices.End(), 1u);
  }
  void SetDecorationSpan()
  {
    UiText::UnderlinedGlyphRun underline;
    underline.glyphRun.numberOfGlyphs = 1u;
    underline.properties.height = 9.0f;
    underline.properties.heightDefined = true;
    mUnderlineRuns.PushBack(underline);
    UiText::StrikethroughGlyphRun strike;
    strike.glyphRun.numberOfGlyphs = 1u;
    strike.properties.height = 9.0f;
    strike.properties.heightDefined = true;
    mStrikethroughRuns.PushBack(strike);
  }
  void SetTwoLines()
  {
    DALI_ASSERT_ALWAYS(mGlyphs.Count() == 2u);
    mLines[0u].glyphRun.numberOfGlyphs = 1u;
    mLines[0u].characterRun.numberOfCharacters = 1u;
    mLines[0u].ascender = 6.75f;
    mLines[0u].descender = -3.5f;
    auto second = mLines[0u];
    second.glyphRun.glyphIndex = 1u;
    second.characterRun.characterIndex = 1u;
    mLines.PushBack(second);
  }

private:
  Size    mControlSize{MASK_SIZE};
  Size    mLayoutSize{MASK_SIZE};
  Vector2 mScrollPosition{Vector2::ZERO};
  Vector2 mZeroVector{Vector2::ZERO};
  Vector2 mShadowOffset{8.0f, 0.0f};
  Vector4 mDefaultColor{Color::BLACK};
  Vector4 mTransparentColor{Vector4::ZERO};
  Vector4 mShadowColor{Color::BLUE};
  Vector4 mUnderlineColor{Color::RED};
  float   mZeroFloat{0.0f};
  bool    mStyleEnabled{false};
  UiText::Underline::Type mUnderlineType{UiText::Underline::Type::SOLID};
  UiText::Alignment mVerticalAlignment{UiText::Alignment::START};
  float mUnderlineHeight{0.0f};
  float mStrikethroughHeight{0.0f};
  uint16_t mOutlineWidth{0u};
  Vector<UiText::ColorIndex> mBackgroundIndices;
  Vector<UiText::UnderlinedGlyphRun> mUnderlineRuns;
  Vector<UiText::StrikethroughGlyphRun> mStrikethroughRuns;

  Vector<UiText::LineRun>                  mLines;
  Vector<UiText::GlyphInfo>                mGlyphs;
  Vector<Vector2>                          mPositions;
  Vector<Vector4>                          mColors;
  Vector<UiText::ColorIndex>               mColorIndices;
  Vector<UiText::Character>                mCharacters;
  Vector<UiText::CharacterIndex>           mGlyphToCharacters;
  Vector<UiText::BoundedParagraphRun>      mBoundedParagraphRuns;
  Vector<UiText::CharacterSpacingGlyphRun> mCharacterSpacingGlyphRuns;
  Vector<UiText::FontRun>                  mFontRuns;
  Vector<UiText::FontDescriptionRun>       mFontDescriptionRuns;
  Vector<UiText::CharacterDirection>       mCharacterDirections;
  UiText::Internal::GradientSpanModelData  mGradientSpanData;
  bool                                     mHasGradientSpan{false};
};

PixelData RenderTextGradientMask(const MaskModel& model)
{
  UiText::TypesetterPtr typesetter = UiText::Typesetter::New(&model);
  return typesetter->RenderTextGradientMask(MASK_SIZE, UiText::Direction::LEFT_TO_RIGHT, false, Pixel::L8);
}

PixelData RenderTextGradientPreserved(const MaskModel& model)
{
  UiText::TypesetterPtr typesetter = UiText::Typesetter::New(&model);
  return typesetter->RenderTextGradientPreserved(MASK_SIZE, UiText::Direction::LEFT_TO_RIGHT, false, Pixel::RGBA8888);
}

uint64_t SumMaskPixels(const PixelData& pixelData)
{
  DALI_TEST_EQUALS(pixelData.GetPixelFormat(), Pixel::L8, TEST_LOCATION);

  Dali::Integration::PixelDataBuffer buffer = Dali::Integration::GetPixelDataBuffer(pixelData);
  DALI_TEST_CHECK(nullptr != buffer.buffer);

  const uint32_t width       = pixelData.GetWidth();
  const uint32_t height      = pixelData.GetHeight();
  const uint32_t strideBytes = pixelData.GetStrideBytes();

  uint64_t sum = 0u;
  for(uint32_t y = 0u; y < height; ++y)
  {
    const uint8_t* row = buffer.buffer + (y * strideBytes);
    for(uint32_t x = 0u; x < width; ++x)
    {
      sum += row[x];
    }
  }

  return sum;
}

uint64_t SumRgbaAlphaPixels(const PixelData& pixelData)
{
  DALI_TEST_EQUALS(pixelData.GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION);

  Dali::Integration::PixelDataBuffer buffer = Dali::Integration::GetPixelDataBuffer(pixelData);
  DALI_TEST_CHECK(nullptr != buffer.buffer);

  const uint32_t width       = pixelData.GetWidth();
  const uint32_t height      = pixelData.GetHeight();
  const uint32_t strideBytes = pixelData.GetStrideBytes();

  uint64_t sum = 0u;
  for(uint32_t y = 0u; y < height; ++y)
  {
    const uint8_t* row = buffer.buffer + (y * strideBytes);
    for(uint32_t x = 0u; x < width; ++x)
    {
      sum += row[x * 4u + 3u];
    }
  }

  return sum;
}

uint32_t CountNonZeroPixels(const PixelData& pixelData)
{
  DALI_TEST_EQUALS(pixelData.GetPixelFormat(), Pixel::L8, TEST_LOCATION);

  Dali::Integration::PixelDataBuffer buffer = Dali::Integration::GetPixelDataBuffer(pixelData);
  DALI_TEST_CHECK(nullptr != buffer.buffer);

  const uint32_t width       = pixelData.GetWidth();
  const uint32_t height      = pixelData.GetHeight();
  const uint32_t strideBytes = pixelData.GetStrideBytes();

  uint32_t count = 0u;
  for(uint32_t y = 0u; y < height; ++y)
  {
    const uint8_t* row = buffer.buffer + (y * strideBytes);
    for(uint32_t x = 0u; x < width; ++x)
    {
      count += (row[x] > 0u) ? 1u : 0u;
    }
  }

  return count;
}

} // namespace

void utc_dali_text_gradient_typesetter_mask_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_text_gradient_typesetter_mask_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliTextGradientTypesetterMaskEmptyTextP(void)
{
  UiTestApplication application;

  MaskModel model(0u);
  PixelData mask = RenderTextGradientMask(model);

  DALI_TEST_EQUALS(mask.GetWidth(), static_cast<uint32_t>(MASK_SIZE.width), TEST_LOCATION);
  DALI_TEST_EQUALS(mask.GetHeight(), static_cast<uint32_t>(MASK_SIZE.height), TEST_LOCATION);
  DALI_TEST_EQUALS(SumMaskPixels(mask), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(CountNonZeroPixels(mask), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientTypesetterMaskDefaultGlyphProducesMaskP(void)
{
  UiTestApplication application;

  MaskModel model(6u);
  PixelData mask = RenderTextGradientMask(model);

  DALI_TEST_CHECK(SumMaskPixels(mask) > 0u);
  DALI_TEST_CHECK(CountNonZeroPixels(mask) > 0u);
  END_TEST;
}

int UtcDaliTextGradientTypesetterMaskExplicitColorExcludedP(void)
{
  UiTestApplication application;

  MaskModel plainModel(6u);
  MaskModel mixedModel(6u, {1u, 1u, 1u, 0u, 0u, 0u});

  const uint64_t plainSum = SumMaskPixels(RenderTextGradientMask(plainModel));
  const uint64_t mixedSum = SumMaskPixels(RenderTextGradientMask(mixedModel));

  DALI_TEST_CHECK(plainSum > 0u);
  DALI_TEST_CHECK(mixedSum > 0u);
  DALI_TEST_CHECK(mixedSum < plainSum);
  END_TEST;
}

int UtcDaliTextGradientTypesetterMaskAllExplicitColorEmptyP(void)
{
  UiTestApplication application;

  MaskModel model(6u, {1u, 1u, 1u, 1u, 1u, 1u});
  PixelData mask = RenderTextGradientMask(model);

  DALI_TEST_EQUALS(SumMaskPixels(mask), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(CountNonZeroPixels(mask), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientTypesetterMaskStyleExcludedP(void)
{
  UiTestApplication application;

  MaskModel plainModel(6u);
  MaskModel styledModel(6u, {}, true);

  const uint64_t plainSum  = SumMaskPixels(RenderTextGradientMask(plainModel));
  const uint64_t styledSum = SumMaskPixels(RenderTextGradientMask(styledModel));

  DALI_TEST_CHECK(plainSum > 0u);
  DALI_TEST_EQUALS(styledSum, plainSum, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientTypesetterPreservedAllDefaultEmptyP(void)
{
  UiTestApplication application;

  MaskModel model(6u);
  PixelData preserved = RenderTextGradientPreserved(model);

  DALI_TEST_EQUALS(preserved.GetWidth(), static_cast<uint32_t>(MASK_SIZE.width), TEST_LOCATION);
  DALI_TEST_EQUALS(preserved.GetHeight(), static_cast<uint32_t>(MASK_SIZE.height), TEST_LOCATION);
  DALI_TEST_EQUALS(SumRgbaAlphaPixels(preserved), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientTypesetterPreservedExplicitColorP(void)
{
  UiTestApplication application;

  MaskModel model(6u, {1u, 1u, 1u, 1u, 1u, 1u});

  DALI_TEST_EQUALS(SumMaskPixels(RenderTextGradientMask(model)), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(SumRgbaAlphaPixels(RenderTextGradientPreserved(model)) > 0u);
  END_TEST;
}

int UtcDaliTextGradientTypesetterPreservedComplementsMaskP(void)
{
  UiTestApplication application;

  MaskModel plainModel(6u);
  MaskModel mixedModel(6u, {1u, 1u, 1u, 0u, 0u, 0u});

  const uint64_t plainMaskSum     = SumMaskPixels(RenderTextGradientMask(plainModel));
  const uint64_t mixedMaskSum     = SumMaskPixels(RenderTextGradientMask(mixedModel));
  const uint64_t preservedAlphaSum = SumRgbaAlphaPixels(RenderTextGradientPreserved(mixedModel));

  DALI_TEST_CHECK(plainMaskSum > 0u);
  DALI_TEST_CHECK(mixedMaskSum > 0u);
  DALI_TEST_CHECK(mixedMaskSum < plainMaskSum);
  DALI_TEST_CHECK(preservedAlphaSum > 0u);
  END_TEST;
}

int UtcDaliTextGradientTypesetterGradientSpanOverridesGlobalGradientP(void)
{
  UiTestApplication application;

  MaskModel plainModel(6u);
  MaskModel gradientSpanModel(6u);
  gradientSpanModel.SetGradientSpanGlyphRange(0u, 3u);

  const uint64_t plainMaskSum        = SumMaskPixels(RenderTextGradientMask(plainModel));
  const uint64_t gradientSpanMaskSum = SumMaskPixels(RenderTextGradientMask(gradientSpanModel));
  const uint64_t preservedAlphaSum   = SumRgbaAlphaPixels(RenderTextGradientPreserved(gradientSpanModel));

  DALI_TEST_CHECK(plainMaskSum > 0u);
  DALI_TEST_CHECK(gradientSpanMaskSum > 0u);
  DALI_TEST_CHECK(gradientSpanMaskSum < plainMaskSum);
  DALI_TEST_CHECK(preservedAlphaSum > 0u);
  END_TEST;
}

int UtcDaliTextGradientTypesetterGeometricOverlapUsesLaterPaintP(void)
{
  UiTestApplication application;

  // Case A: with exactly overlapping bitmaps, the later semi-transparent blue
  // paint owns RGB while alpha remains the glyph coverage times paint alpha.
  MaskModel opaqueReferenceModel(1u);
  opaqueReferenceModel.SetSingleGradientSpan(Color::BLUE);
  const PixelData opaqueReference = RenderTextGradientPreserved(opaqueReferenceModel);

  MaskModel exactOverlapModel(2u);
  exactOverlapModel.SetOverlappingGradientSpanGlyphs(Color::RED, Vector4(0.0f, 0.0f, 1.0f, 0.5f));
  const PixelData exactOverlap = RenderTextGradientPreserved(exactOverlapModel);

  const auto opaqueReferenceBuffer = Dali::Integration::GetPixelDataBuffer(opaqueReference);
  const auto exactOverlapBuffer    = Dali::Integration::GetPixelDataBuffer(exactOverlap);
  DALI_TEST_CHECK(nullptr != opaqueReferenceBuffer.buffer);
  DALI_TEST_CHECK(nullptr != exactOverlapBuffer.buffer);

  uint32_t selectedX   = 0u;
  uint32_t selectedY   = 0u;
  uint8_t  maxCoverage = 0u;
  for(uint32_t y = 0u; y < opaqueReference.GetHeight(); ++y)
  {
    const uint8_t* row = opaqueReferenceBuffer.buffer + y * opaqueReference.GetStrideBytes();
    for(uint32_t x = 0u; x < opaqueReference.GetWidth(); ++x)
    {
      if(row[x * 4u + 3u] > maxCoverage)
      {
        maxCoverage = row[x * 4u + 3u];
        selectedX   = x;
        selectedY   = y;
      }
    }
  }
  DALI_TEST_CHECK(maxCoverage > 0u);

  const uint8_t* exactPixel         = exactOverlapBuffer.buffer + selectedY * exactOverlap.GetStrideBytes() + selectedX * 4u;
  const uint8_t  expectedExactAlpha = static_cast<uint8_t>(static_cast<float>(maxCoverage) * 0.5f);
  DALI_TEST_EQUALS(exactPixel[0u], 0u, TEST_LOCATION);
  DALI_TEST_CHECK(std::abs(static_cast<int>(exactPixel[2u]) - static_cast<int>(expectedExactAlpha)) <= 1);
  DALI_TEST_CHECK(std::abs(static_cast<int>(exactPixel[3u]) - static_cast<int>(expectedExactAlpha)) <= 1);

  // Case B: the reserved test glyph has raw coverage 128. With an earlier
  // alpha-0.25 paint and later alpha-0.5 paint, output alpha must be 255*0.5,
  // not max(255*0.25, 128)*0.5.
  constexpr uint32_t HALF_COVERAGE_GLYPH_INDEX = Dali::TextAbstraction::Test::HALF_COVERAGE_GLYPH_INDEX;
  MaskModel          halfCoverageReferenceModel(1u);
  halfCoverageReferenceModel.SetSingleGradientSpan(Color::BLUE, Vector2::ZERO, HALF_COVERAGE_GLYPH_INDEX);
  const PixelData halfCoverageReference       = RenderTextGradientPreserved(halfCoverageReferenceModel);
  const auto      halfCoverageReferenceBuffer = Dali::Integration::GetPixelDataBuffer(halfCoverageReference);
  DALI_TEST_CHECK(nullptr != halfCoverageReferenceBuffer.buffer);
  const uint8_t* halfCoveragePixel = halfCoverageReferenceBuffer.buffer + selectedY * halfCoverageReference.GetStrideBytes() + selectedX * 4u;
  DALI_TEST_EQUALS(halfCoveragePixel[3u], 128u, TEST_LOCATION);

  MaskModel lowerCoverageOverlapModel(2u);
  lowerCoverageOverlapModel.SetOverlappingGradientSpanGlyphs(Vector4(1.0f, 0.0f, 0.0f, 0.25f),
                                                             Vector4(0.0f, 0.0f, 1.0f, 0.5f),
                                                             Vector2::ZERO,
                                                             HALF_COVERAGE_GLYPH_INDEX);
  const PixelData lowerCoverageOverlap       = RenderTextGradientPreserved(lowerCoverageOverlapModel);
  const auto      lowerCoverageOverlapBuffer = Dali::Integration::GetPixelDataBuffer(lowerCoverageOverlap);
  DALI_TEST_CHECK(nullptr != lowerCoverageOverlapBuffer.buffer);

  const uint8_t* resultPixel   = lowerCoverageOverlapBuffer.buffer + selectedY * lowerCoverageOverlap.GetStrideBytes() + selectedX * 4u;
  const uint8_t  expectedAlpha = static_cast<uint8_t>(static_cast<float>(maxCoverage) * 0.5f);
  DALI_TEST_EQUALS(resultPixel[0u], 0u, TEST_LOCATION);
  DALI_TEST_CHECK(std::abs(static_cast<int>(resultPixel[2u]) - static_cast<int>(expectedAlpha)) <= 1);
  DALI_TEST_CHECK(std::abs(static_cast<int>(resultPixel[3u]) - static_cast<int>(expectedAlpha)) <= 1);

  // Case C: an ordinary opaque glyph contributes raw coverage before a later
  // semi-transparent GradientSpan glyph takes paint ownership.
  MaskModel ordinaryThenGradientModel(2u, {1u, 1u});
  ordinaryThenGradientModel.SetOverlappingMixedGlyphs(false,
                                                      true,
                                                      Vector4(0.0f, 0.0f, 1.0f, 0.5f),
                                                      Color::RED,
                                                      HALF_COVERAGE_GLYPH_INDEX);
  const PixelData ordinaryThenGradient       = RenderTextGradientPreserved(ordinaryThenGradientModel);
  const auto      ordinaryThenGradientBuffer = Dali::Integration::GetPixelDataBuffer(ordinaryThenGradient);
  DALI_TEST_CHECK(nullptr != ordinaryThenGradientBuffer.buffer);
  const uint8_t* ordinaryThenGradientPixel = ordinaryThenGradientBuffer.buffer + selectedY * ordinaryThenGradient.GetStrideBytes() + selectedX * 4u;
  DALI_TEST_EQUALS(ordinaryThenGradientPixel[0u], 0u, TEST_LOCATION);
  DALI_TEST_CHECK(std::abs(static_cast<int>(ordinaryThenGradientPixel[2u]) - static_cast<int>(expectedAlpha)) <= 1);
  DALI_TEST_CHECK(std::abs(static_cast<int>(ordinaryThenGradientPixel[3u]) - static_cast<int>(expectedAlpha)) <= 1);

  // Case D: a later ordinary glyph owns paint while retaining the earlier
  // GradientSpan glyph's greater raw coverage.
  MaskModel gradientThenOrdinaryModel(2u, {1u, 1u});
  gradientThenOrdinaryModel.SetOverlappingMixedGlyphs(true,
                                                      false,
                                                      Color::RED,
                                                      Vector4(0.0f, 0.0f, 1.0f, 0.5f),
                                                      HALF_COVERAGE_GLYPH_INDEX);
  const PixelData gradientThenOrdinary       = RenderTextGradientPreserved(gradientThenOrdinaryModel);
  const auto      gradientThenOrdinaryBuffer = Dali::Integration::GetPixelDataBuffer(gradientThenOrdinary);
  DALI_TEST_CHECK(nullptr != gradientThenOrdinaryBuffer.buffer);
  const uint8_t* gradientThenOrdinaryPixel = gradientThenOrdinaryBuffer.buffer + selectedY * gradientThenOrdinary.GetStrideBytes() + selectedX * 4u;
  DALI_TEST_EQUALS(gradientThenOrdinaryPixel[0u], 0u, TEST_LOCATION);
  DALI_TEST_CHECK(std::abs(static_cast<int>(gradientThenOrdinaryPixel[2u]) - static_cast<int>(expectedAlpha)) <= 1);
  DALI_TEST_CHECK(std::abs(static_cast<int>(gradientThenOrdinaryPixel[3u]) - static_cast<int>(expectedAlpha)) <= 1);

  // Case E: a distant GradientSpan activates the image-wide scratch buffer.
  // Unrelated ordinary overlap still follows the intended maximum-raw-coverage rule.
  MaskModel unrelatedOrdinaryOverlapModel(3u, {1u, 1u, 0u});
  unrelatedOrdinaryOverlapModel.SetDefaultColor(Vector4(1.0f, 0.0f, 0.0f, 0.25f));
  unrelatedOrdinaryOverlapModel.SetForegroundColor(Vector4(1.0f, 0.0f, 0.0f, 0.25f));
  unrelatedOrdinaryOverlapModel.SetGlyphPosition(1u, Vector2::ZERO);
  unrelatedOrdinaryOverlapModel.SetGlyphBitmapIndex(1u, HALF_COVERAGE_GLYPH_INDEX);
  unrelatedOrdinaryOverlapModel.SetGradientSpanGlyphRange(2u, 1u);
  const PixelData unrelatedOrdinaryOverlap       = RenderTextGradientPreserved(unrelatedOrdinaryOverlapModel);
  const auto      unrelatedOrdinaryOverlapBuffer = Dali::Integration::GetPixelDataBuffer(unrelatedOrdinaryOverlap);
  DALI_TEST_CHECK(nullptr != unrelatedOrdinaryOverlapBuffer.buffer);
  const uint8_t* unrelatedOrdinaryOverlapPixel = unrelatedOrdinaryOverlapBuffer.buffer + selectedY * unrelatedOrdinaryOverlap.GetStrideBytes() + selectedX * 4u;
  const uint8_t  expectedOrdinaryAlpha         = static_cast<uint8_t>(static_cast<float>(maxCoverage) * 0.25f);
  DALI_TEST_CHECK(std::abs(static_cast<int>(unrelatedOrdinaryOverlapPixel[0u]) - static_cast<int>(expectedOrdinaryAlpha)) <= 1);
  DALI_TEST_EQUALS(unrelatedOrdinaryOverlapPixel[1u], 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(unrelatedOrdinaryOverlapPixel[2u], 0u, TEST_LOCATION);
  DALI_TEST_CHECK(std::abs(static_cast<int>(unrelatedOrdinaryOverlapPixel[3u]) - static_cast<int>(expectedOrdinaryAlpha)) <= 1);
  END_TEST;
}

int UtcDaliTextGradientTypesetterMixedPreservedMaskSizeContractP(void)
{
  UiTestApplication application;

  MaskModel mixedModel(6u, {1u, 1u, 1u, 0u, 0u, 0u});

  PixelData preserved = RenderTextGradientPreserved(mixedModel);
  PixelData mask      = RenderTextGradientMask(mixedModel);

  DALI_TEST_EQUALS(preserved.GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION);
  DALI_TEST_EQUALS(mask.GetPixelFormat(), Pixel::L8, TEST_LOCATION);
  DALI_TEST_EQUALS(preserved.GetWidth(), static_cast<uint32_t>(MASK_SIZE.width), TEST_LOCATION);
  DALI_TEST_EQUALS(preserved.GetHeight(), static_cast<uint32_t>(MASK_SIZE.height), TEST_LOCATION);
  DALI_TEST_EQUALS(mask.GetWidth(), static_cast<uint32_t>(MASK_SIZE.width), TEST_LOCATION);
  DALI_TEST_EQUALS(mask.GetHeight(), static_cast<uint32_t>(MASK_SIZE.height), TEST_LOCATION);
  DALI_TEST_EQUALS(mask.GetWidth(), preserved.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(mask.GetHeight(), preserved.GetHeight(), TEST_LOCATION);
  DALI_TEST_CHECK(SumRgbaAlphaPixels(preserved) > 0u);
  DALI_TEST_CHECK(SumMaskPixels(mask) > 0u);
  END_TEST;
}

// The second glyph is healthy: rejecting malformed geometry must not hide it.
int UtcDaliTypesetterMalformedCoordinatesP(void)
{
  TestApplication application;
  const float values[] = {std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(),
                          std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::infinity(),
                          -std::numeric_limits<float>::infinity(), 2147483648.0f, -2147483648.0f,
                          std::nextafter(2147483648.0f, 0.0f), std::nextafter(-2147483648.0f, 0.0f)};
  for(bool gradient : {false, true})
  {
    for(float value : values)
    {
      for(bool vertical : {false, true})
      {
        MaskModel model(2u);
        if(gradient)
        {
          model.SetGradientSpanGlyphRange(0u, 2u);
        }
        model.SetGlyphPosition(0u, vertical ? Vector2(0.0f, value) : Vector2(value, 0.0f));
        auto typesetter = UiText::Typesetter::New(&model);
        auto pixels = typesetter->Render(MASK_SIZE, UiText::Direction::LEFT_TO_RIGHT,
                                         UiText::Typesetter::RENDER_NO_STYLES, false, Pixel::RGBA8888);
        DALI_TEST_EQUALS(SumRgbaAlphaPixels(pixels), uint64_t(8u * 255u), TEST_LOCATION);
        DALI_TEST_EQUALS(SumMaskPixels(RenderTextGradientMask(model)), gradient ? uint64_t(0u) : uint64_t(8u * 255u), TEST_LOCATION);
        DALI_TEST_EQUALS(SumRgbaAlphaPixels(RenderTextGradientPreserved(model)), gradient ? uint64_t(8u * 255u) : uint64_t(0u), TEST_LOCATION);
      }
    }
  }
  END_TEST;
}

int UtcDaliTypesetterMalformedLineAndStyleCoordinatesP(void)
{
  TestApplication application;
  for(float value : {std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(),
                     std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::infinity(),
                     -std::numeric_limits<float>::infinity(), 2147483648.0f, -2147483648.0f})
  {
    MaskModel model(1u, {}, true);
    model.SetAlignmentOffset(value);
    auto typesetter = UiText::Typesetter::New(&model);
    for(auto behaviour : {UiText::Typesetter::RENDER_TEXT_AND_STYLES, UiText::Typesetter::RENDER_NO_TEXT,
                           UiText::Typesetter::RENDER_OVERLAY_STYLE, UiText::Typesetter::RENDER_MASK})
    {
      const auto pixels = typesetter->Render(MASK_SIZE, UiText::Direction::LEFT_TO_RIGHT, behaviour, false, Pixel::RGBA8888);
      DALI_TEST_EQUALS(SumRgbaAlphaPixels(pixels), uint64_t(0u), TEST_LOCATION);
    }
    DALI_TEST_EQUALS(SumMaskPixels(RenderTextGradientMask(model)), uint64_t(0u), TEST_LOCATION);
    DALI_TEST_EQUALS(SumRgbaAlphaPixels(RenderTextGradientPreserved(model)), uint64_t(0u), TEST_LOCATION);
    // Ignoring measurement alignment is a render policy; the model stays intact.
    DALI_TEST_EQUALS(SumRgbaAlphaPixels(typesetter->Render(MASK_SIZE, UiText::Direction::LEFT_TO_RIGHT,
                       UiText::Typesetter::RENDER_NO_STYLES, true, Pixel::RGBA8888)), uint64_t(8u * 255u), TEST_LOCATION);
    DALI_TEST_CHECK(std::isnan(value) ? std::isnan(model.GetLines()[0u].alignmentOffset)
                                     : model.GetLines()[0u].alignmentOffset == value);
    model.SetAlignmentOffset(0.0f);
    model.SetShadowOffset(Vector2(value, value));
    DALI_TEST_EQUALS(SumRgbaAlphaPixels(typesetter->Render(MASK_SIZE, UiText::Direction::LEFT_TO_RIGHT,
                       UiText::Typesetter::RENDER_NO_TEXT, false, Pixel::RGBA8888)), uint64_t(0u), TEST_LOCATION);
    model.SetLineAscender(value);
    DALI_TEST_EQUALS(SumRgbaAlphaPixels(typesetter->Render(MASK_SIZE, UiText::Direction::LEFT_TO_RIGHT,
                       UiText::Typesetter::RENDER_NO_STYLES, false, Pixel::RGBA8888)), uint64_t(0u), TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliTypesetterMalformedDimensionsP(void)
{
  TestApplication application;
  for(float value : {std::numeric_limits<float>::max(), -1.0f, std::numeric_limits<float>::quiet_NaN(),
                     std::numeric_limits<float>::infinity(), 2147483648.0f, 1073741824.0f})
  {
    for(bool height : {false, true})
    {
      MaskModel model(2u);
      model.SetGlyphSize(height ? 2.0f : value, height ? value : 4.0f);
      auto typesetter = UiText::Typesetter::New(&model);
      DALI_TEST_EQUALS(SumRgbaAlphaPixels(typesetter->Render(MASK_SIZE, UiText::Direction::LEFT_TO_RIGHT,
                         UiText::Typesetter::RENDER_NO_STYLES, false, Pixel::RGBA8888)), uint64_t(8u * 255u), TEST_LOCATION);
      const Vector2 size = height ? Vector2(240.0f, value) : Vector2(value, 96.0f);
      DALI_TEST_CHECK(!typesetter->Render(size, UiText::Direction::LEFT_TO_RIGHT));
      DALI_TEST_CHECK(!typesetter->RenderTextGradientMask(size, UiText::Direction::LEFT_TO_RIGHT));
      DALI_TEST_CHECK(!typesetter->RenderTextGradientPreserved(size, UiText::Direction::LEFT_TO_RIGHT));
    }
  }
  END_TEST;
}

int UtcDaliTypesetterNegativeFractionalCoordinatesP(void)
{
  TestApplication application;
  for(bool gradient : {false, true})
  {
    MaskModel model(1u);
    if(gradient)
    {
      model.SetGradientSpanGlyphRange(0u, 1u);
    }
    model.SetGlyphPosition(0u, Vector2(1.9f, -1.9f));
    auto typesetter = UiText::Typesetter::New(&model);
    const auto pixels = typesetter->Render(MASK_SIZE, UiText::Direction::LEFT_TO_RIGHT,
                                           UiText::Typesetter::RENDER_NO_STYLES, false, Pixel::RGBA8888);
    DALI_TEST_EQUALS(SumRgbaAlphaPixels(pixels), uint64_t(6u * 255u), TEST_LOCATION);
    const auto buffer = Integration::GetPixelDataBuffer(pixels);
    for(uint32_t y = 0; y < pixels.GetHeight(); ++y)
    {
      for(uint32_t x = 0; x < pixels.GetWidth(); ++x)
      {
        DALI_TEST_EQUALS(buffer.buffer[y * pixels.GetStrideBytes() + x * 4u + 3u],
                          uint8_t(y < 3u && x >= 1u && x < 3u ? 255u : 0u), TEST_LOCATION);
      }
    }
  }
  END_TEST;
}

int UtcDaliTypesetterRasterArithmeticBoundariesP(void)
{
  namespace Raster = UiText::Raster;
  int32_t result;
  Raster::GlyphClip clip;
  DALI_TEST_CHECK(!Raster::Convert(2147483648.0f, result));
  DALI_TEST_CHECK(Raster::Convert(-2147483648.0f, result));
  DALI_TEST_CHECK(!Raster::Add(std::numeric_limits<int32_t>::max(), 1, result));
  DALI_TEST_CHECK(!Raster::Add(std::numeric_limits<int32_t>::min(), -1, result));
  DALI_TEST_CHECK(!Raster::BufferFits(0xffffffffu, 0xffffffffu, 4u));
  DALI_TEST_CHECK(!Raster::BufferFits(65536u, 65536u, 1u));
  DALI_TEST_CHECK(Raster::BufferFits(16384u, 16384u, 4u)); // 1 GiB, within each index domain.
  DALI_TEST_CHECK(Raster::BufferFits(4096u, 131072u, 1u)); // Tall L8 text, 512 MiB.
  DALI_TEST_EQUALS(Raster::BufferFits(16384u, 32768u, 4u),
                    std::numeric_limits<ptrdiff_t>::max() > std::numeric_limits<int32_t>::max(), TEST_LOCATION);
  // Widening the destination byte limit must not widen signed source scanlines.
  DALI_TEST_CHECK(!Raster::ClipGlyph(0.0f, 0.0f, 0, 0, 16384u, 32768u, 4u, 240u, 96u, clip));
  // Coordinate converts, but adding the bitmap width would overflow.
  DALI_TEST_CHECK(!Raster::ClipGlyph(2147483520.0f, 0.0f, -2147483520, 0, 256u, 4u, 1u, 240u, 96u, clip));
  DALI_TEST_CHECK(!Raster::ClipGlyph(0.0f, 0.0f, std::numeric_limits<int32_t>::min(), 0, 2u, 4u, 1u, 240u, 96u, clip));
  DALI_TEST_CHECK(!Raster::ClipGlyph(0.0f, 0.0f, 0, 0, 0xffffffffu, 4u, 1u, 240u, 96u, clip));
  DALI_TEST_CHECK(!Raster::ClipGlyph(0.0f, 0.0f, 0, 0, 2u, 0xffffffffu, 1u, 240u, 96u, clip));
  DALI_TEST_CHECK(Raster::ClipGlyph(-1.9f, -1.9f, 0, 0, 2u, 4u, 1u, 240u, 96u, clip));
  DALI_TEST_EQUALS(clip.left, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(clip.top, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(clip.right, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(clip.bottom, 4, TEST_LOCATION);

  // Validate the unsigned extent before narrowing it to a signed coordinate.
  const uint32_t maxExtent = static_cast<uint32_t>(std::numeric_limits<int32_t>::max());
  int32_t begin, end;
  DALI_TEST_CHECK(Raster::ClipRange(-1.9f, 3.9f, maxExtent, begin, end));
  DALI_TEST_EQUALS(begin, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(end, 3, TEST_LOCATION);
  DALI_TEST_CHECK(!Raster::ClipRange(-1.9f, 3.9f, maxExtent + 1u, begin, end));
  DALI_TEST_CHECK(!Raster::ClipRange(-1.9f, 3.9f, std::numeric_limits<uint32_t>::max(), begin, end));
  DALI_TEST_CHECK(!Raster::ClipRange(-1.9f, 3.9f, 0u, begin, end));
  END_TEST;
}

int UtcDaliTypesetterMalformedFirstLineIsolationP(void)
{
  TestApplication application;
  auto samePixels = [](PixelData actual, PixelData expected)
  {
    DALI_TEST_CHECK(actual && expected);
    const auto a = Integration::GetPixelDataBuffer(actual);
    const auto b = Integration::GetPixelDataBuffer(expected);
    DALI_TEST_CHECK(std::memcmp(a.buffer, b.buffer, expected.GetStrideBytes() * expected.GetHeight()) == 0);
  };
  for(bool gradient : {false, true})
  {
    for(float value : {std::numeric_limits<float>::max(), std::numeric_limits<float>::quiet_NaN(),
                       std::numeric_limits<float>::infinity(), -2147483648.0f})
    {
      MaskModel reference(2u, {}, true);
      MaskModel malformed(2u, {}, true);
      for(auto* model : {&reference, &malformed})
      {
        model->SetTwoLines();
        model->SetBackgroundSpan();
        model->SetDecoration(UiText::Underline::Type::DOUBLE, 2.0f, 2.0f, 1u);
        if(gradient)
        {
          model->SetGradientSpanGlyphRange(0u, 2u);
        }
      }
      // A safely offscreen first line still advances the second line's baseline.
      reference.SetAlignmentOffset(-1024.0f);
      malformed.SetAlignmentOffset(value);
      for(auto behaviour : {UiText::Typesetter::RENDER_NO_STYLES, UiText::Typesetter::RENDER_NO_TEXT,
                             UiText::Typesetter::RENDER_OVERLAY_STYLE, UiText::Typesetter::RENDER_TEXT_AND_STYLES})
      {
        auto expected = UiText::Typesetter::New(&reference)->Render(MASK_SIZE, UiText::Direction::LEFT_TO_RIGHT, behaviour);
        DALI_TEST_CHECK(SumRgbaAlphaPixels(expected) > 0u);
        samePixels(UiText::Typesetter::New(&malformed)->Render(MASK_SIZE, UiText::Direction::LEFT_TO_RIGHT, behaviour), expected);
      }
      samePixels(RenderTextGradientMask(malformed), RenderTextGradientMask(reference));
      samePixels(RenderTextGradientPreserved(malformed), RenderTextGradientPreserved(reference));
      // Horizontal failure must not conceal malformed vertical metrics.
      malformed.SetLineAscender(std::numeric_limits<float>::quiet_NaN());
      DALI_TEST_EQUALS(SumRgbaAlphaPixels(UiText::Typesetter::New(&malformed)->Render(
                         MASK_SIZE, UiText::Direction::LEFT_TO_RIGHT, UiText::Typesetter::RENDER_TEXT_AND_STYLES)), uint64_t(0u), TEST_LOCATION);
      DALI_TEST_EQUALS(SumMaskPixels(RenderTextGradientMask(malformed)), uint64_t(0u), TEST_LOCATION);
      DALI_TEST_EQUALS(SumRgbaAlphaPixels(RenderTextGradientPreserved(malformed)), uint64_t(0u), TEST_LOCATION);
    }
  }
  END_TEST;
}

int UtcDaliTypesetterMalformedGlyphStyleIsolationP(void)
{
  TestApplication application;
  for(bool gradient : {false, true})
  {
    for(bool dimension : {false, true})
    {
      for(float value : {std::numeric_limits<float>::max(), std::numeric_limits<float>::quiet_NaN(),
                         std::numeric_limits<float>::infinity()})
      {
        MaskModel reference(2u, {}, true);
        MaskModel malformed(2u, {}, true);
        for(auto* model : {&reference, &malformed})
        {
          model->SetLineAscender(8.0f);
          model->SetBackgroundSpan();
          model->SetDecorationSpan();
          if(gradient)
          {
            model->SetGradientSpanGlyphRange(0u, 2u);
          }
        }
        reference.SetGlyphSize(0.0f, 0.0f);
        if(dimension)
        {
          malformed.SetGlyphSize(value, 4.0f);
        }
        else
        {
          malformed.SetGlyphPosition(0u, Vector2(0.0f, value));
        }
        for(auto behaviour : {UiText::Typesetter::RENDER_NO_TEXT, UiText::Typesetter::RENDER_OVERLAY_STYLE,
                               UiText::Typesetter::RENDER_TEXT_AND_STYLES})
        {
          auto expected = UiText::Typesetter::New(&reference)->Render(MASK_SIZE, UiText::Direction::LEFT_TO_RIGHT, behaviour);
          auto actual = UiText::Typesetter::New(&malformed)->Render(MASK_SIZE, UiText::Direction::LEFT_TO_RIGHT, behaviour);
          DALI_TEST_CHECK(SumRgbaAlphaPixels(expected) > 0u);
          DALI_TEST_CHECK(std::memcmp(Integration::GetPixelDataBuffer(actual).buffer,
                                      Integration::GetPixelDataBuffer(expected).buffer,
                                      expected.GetStrideBytes() * expected.GetHeight()) == 0);
        }
      }
    }
  }
  END_TEST;
}

int UtcDaliTypesetterDecorationClippingP(void)
{
  TestApplication application;
  // Right/bottom clipping must be the exact crop of an otherwise identical
  // render, including fractional baseline/thickness and both DOUBLE strokes.
  for(auto type : {UiText::Underline::Type::SOLID, UiText::Underline::Type::DOUBLE, UiText::Underline::Type::DASHED})
  {
    for(float height : {1.0f, 2.5f, 6.0f})
    {
      for(bool multiline : {false, true})
      {
        MaskModel model(2u, {}, true);
        model.SetDecoration(type, height, height, 1u);
        model.SetBackgroundSpan();
        if(multiline)
        {
          model.SetTwoLines();
        }
        model.SetGlyphPosition(0u, Vector2(1.9f, -1.9f));
        model.SetGlyphPosition(1u, Vector2(8.9f, 1.9f));
        for(auto behaviour : {UiText::Typesetter::RENDER_NO_TEXT, UiText::Typesetter::RENDER_OVERLAY_STYLE,
                               UiText::Typesetter::RENDER_TEXT_AND_STYLES})
        {
          auto full = UiText::Typesetter::New(&model)->Render(MASK_SIZE, UiText::Direction::LEFT_TO_RIGHT, behaviour);
          for(float cropHeight : {4.0f, 7.0f, 12.0f, 18.0f})
          {
            auto clipped = UiText::Typesetter::New(&model)->Render(Vector2(12.0f, cropHeight), UiText::Direction::LEFT_TO_RIGHT, behaviour);
            for(uint32_t y = 0u; y < clipped.GetHeight(); ++y)
            {
              DALI_TEST_CHECK(std::memcmp(Integration::GetPixelDataBuffer(clipped).buffer + y * clipped.GetStrideBytes(),
                                          Integration::GetPixelDataBuffer(full).buffer + y * full.GetStrideBytes(),
                                          clipped.GetWidth() * 4u) == 0);
            }
          }
        }
      }
    }
  }
  END_TEST;
}

int UtcDaliTypesetterMalformedLineMetadataTilesP(void)
{
  TestApplication application;
  MaskModel model(2u);
  model.SetTwoLines();
  model.SetAlignmentOffset(std::numeric_limits<float>::quiet_NaN());
  auto typesetter = UiText::Typesetter::New(&model);
  auto plan = UiText::Internal::Reveal::BuildCharacterPlan(model, UiText::Reveal::AUTO_FADE_DURATION_RATIO);
  float fadeDuration = 0.0f;
  auto full = typesetter->RenderTextRevealMetadata(MASK_SIZE, UiText::Direction::LEFT_TO_RIGHT, plan, fadeDuration);
  // The first line is skipped by the tile culling path; its fractional metrics
  // must accumulate exactly as in the full traversal (two truncations).
  constexpr uint32_t offset = 12u;
  auto tile = typesetter->RenderTextRevealMetadata(Vector2(MASK_SIZE.width, MASK_SIZE.height - offset),
                 UiText::Direction::LEFT_TO_RIGHT, plan, fadeDuration, offset, MASK_SIZE);
  DALI_TEST_CHECK(full && tile);
  DALI_TEST_CHECK(std::memcmp(Integration::GetPixelDataBuffer(tile).buffer,
                              Integration::GetPixelDataBuffer(full).buffer + offset * full.GetStrideBytes(),
                              tile.GetHeight() * tile.GetStrideBytes()) == 0);
  const auto bytes = Integration::GetPixelDataBuffer(full);
  // Two 2x4 glyphs: only the healthy second one contributes metadata.
  uint32_t covered = 0u;
  for(uint32_t y = 0u; y < full.GetHeight(); ++y)
  {
    for(uint32_t x = 0u; x < full.GetWidth(); ++x)
    {
      if(bytes.buffer[y * full.GetStrideBytes() + x * 4u + 3u] != 0u)
      {
        DALI_TEST_CHECK(y >= 15u && y < 19u);
        ++covered;
      }
    }
  }
  DALI_TEST_EQUALS(covered, 8u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTypesetterEmptyBufferP(void)
{
  TestApplication application;
  MaskModel model(2u, {}, true);
  model.SetBackgroundSpan();
  model.SetDecoration(UiText::Underline::Type::DOUBLE, 2.0f, 2.0f, 1u);
  auto typesetter = UiText::Typesetter::New(&model);
  for(const auto& size : {Vector2(0.0f, 96.0f), Vector2(240.0f, 0.0f), Vector2::ZERO})
  {
    for(auto behaviour : {UiText::Typesetter::RENDER_NO_STYLES, UiText::Typesetter::RENDER_NO_TEXT,
                           UiText::Typesetter::RENDER_OVERLAY_STYLE, UiText::Typesetter::RENDER_TEXT_AND_STYLES})
    {
      auto pixels = typesetter->Render(size, UiText::Direction::LEFT_TO_RIGHT, behaviour);
      DALI_TEST_CHECK(pixels);
      DALI_TEST_EQUALS(pixels.GetWidth(), uint32_t(size.width), TEST_LOCATION);
      DALI_TEST_EQUALS(pixels.GetHeight(), uint32_t(size.height), TEST_LOCATION);
    }
    DALI_TEST_CHECK(typesetter->RenderTextGradientMask(size, UiText::Direction::LEFT_TO_RIGHT));
    DALI_TEST_CHECK(typesetter->RenderTextGradientPreserved(size, UiText::Direction::LEFT_TO_RIGHT));
    DALI_TEST_CHECK(typesetter->CreateFullBackgroundBuffer(uint32_t(size.width), uint32_t(size.height), Color::BLUE));
    const auto plan = UiText::Internal::Reveal::BuildCharacterPlan(model, UiText::Reveal::AUTO_FADE_DURATION_RATIO);
    float fadeDuration = 0.0f;
    DALI_TEST_CHECK(typesetter->RenderTextRevealMetadata(size, UiText::Direction::LEFT_TO_RIGHT, plan, fadeDuration));
  }
  END_TEST;
}

int UtcDaliTypesetterDecorationNegativeEdgesP(void)
{
  TestApplication application;
  for(auto type : {UiText::Underline::Type::SOLID, UiText::Underline::Type::DOUBLE, UiText::Underline::Type::DASHED})
  {
    for(float height : {1.0f, 3.0f, 6.0f})
    {
      for(float baseline : {14.0f, 16.0f, 18.0f})
      {
        MaskModel reference(1u, {}, true);
        MaskModel clippedModel(1u, {}, true);
        for(auto* model : {&reference, &clippedModel})
        {
          model->SetVerticalAlignment(UiText::Alignment::END);
          model->SetDecoration(type, height, height, 1u);
          model->SetBackgroundSpan();
          model->SetLineAscender(4.0f);
        }
        reference.SetGlyphPosition(0u, Vector2(18.0f, baseline - 4.0f));
        clippedModel.SetGlyphPosition(0u, Vector2(-2.0f, baseline - 4.0f));
        for(auto behaviour : {UiText::Typesetter::RENDER_NO_TEXT, UiText::Typesetter::RENDER_OVERLAY_STYLE,
                               UiText::Typesetter::RENDER_TEXT_AND_STYLES})
        {
          auto full = UiText::Typesetter::New(&reference)->Render(MASK_SIZE, UiText::Direction::LEFT_TO_RIGHT, behaviour);
          auto clipped = UiText::Typesetter::New(&clippedModel)->Render(Vector2(220.0f, 76.0f), UiText::Direction::LEFT_TO_RIGHT, behaviour);
          for(uint32_t y = 0u; y < clipped.GetHeight(); ++y)
          {
            DALI_TEST_CHECK(std::memcmp(Integration::GetPixelDataBuffer(clipped).buffer + y * clipped.GetStrideBytes(),
                                        Integration::GetPixelDataBuffer(full).buffer + (y + 20u) * full.GetStrideBytes() + 20u * 4u,
                                        clipped.GetWidth() * 4u) == 0);
          }
        }
      }
    }
  }
  END_TEST;
}
