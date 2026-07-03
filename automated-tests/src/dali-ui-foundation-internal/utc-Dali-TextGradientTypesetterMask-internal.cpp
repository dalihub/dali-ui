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
#include <dali-ui-foundation/internal/text/text-model-interface.h>
#include <dali-ui-test-suite-utils.h>
#include <dali/integration-api/pixel-data-integ.h>
#include <dali.h>

#include <cstdint>
#include <initializer_list>

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
  UiText::Alignment GetVerticalAlignment() const override { return UiText::Alignment::START; }
  UiText::Alignment GetVerticalLineAlignment() const override { return UiText::Alignment::START; }
  UiText::EllipsisPosition::Type GetEllipsisPosition() const override { return UiText::EllipsisPosition::END; }
  bool IsTextElideEnabled() const override { return false; }
  UiText::Length GetNumberOfLines() const override { return mLines.Count(); }
  const UiText::LineRun* GetLines() const override { return mLines.Begin(); }
  UiText::Length GetNumberOfScripts() const override { return 0u; }
  const UiText::ScriptRun* GetScriptRuns() const override { return nullptr; }
  UiText::Length GetNumberOfCharacters() const override { return mCharacters.Count(); }
  UiText::Length GetNumberOfGlyphs() const override { return mGlyphs.Count(); }
  UiText::GlyphIndex GetStartIndexOfElidedGlyphs() const override { return 0u; }
  UiText::GlyphIndex GetEndIndexOfElidedGlyphs() const override { return mGlyphs.Count() > 0u ? mGlyphs.Count() - 1u : 0u; }
  UiText::GlyphIndex GetFirstMiddleIndexOfElidedGlyphs() const override { return 0u; }
  UiText::GlyphIndex GetSecondMiddleIndexOfElidedGlyphs() const override { return 0u; }
  const UiText::GlyphInfo* GetGlyphs() const override { return mGlyphs.Begin(); }
  const Vector2* GetLayout() const override { return mPositions.Begin(); }
  const Vector4* GetColors() const override { return mColors.Count() > 0u ? mColors.Begin() : nullptr; }
  const UiText::ColorIndex* GetColorIndices() const override { return mColorIndices.Count() > 0u ? mColorIndices.Begin() : nullptr; }
  const Vector4* GetBackgroundColors() const override { return nullptr; }
  const UiText::ColorIndex* GetBackgroundColorIndices() const override { return nullptr; }
  bool IsMarkupBackgroundColorSet() const override { return false; }
  const Vector4& GetDefaultColor() const override { return mDefaultColor; }
  const Vector2& GetShadowOffset() const override { return mStyleEnabled ? mShadowOffset : mZeroVector; }
  bool IsShadowEnabled() const override { return mStyleEnabled; }
  const Vector4& GetShadowColor() const override { return mStyleEnabled ? mShadowColor : mTransparentColor; }
  const float& GetShadowBlurRadius() const override { return mZeroFloat; }
  const Vector4& GetUnderlineColor() const override { return mStyleEnabled ? mUnderlineColor : mTransparentColor; }
  bool IsUnderlineEnabled() const override { return mStyleEnabled; }
  bool IsMarkupUnderlineSet() const override { return false; }
  float GetUnderlineHeight() const override { return 0.0f; }
  UiText::Underline::Type GetUnderlineType() const override { return UiText::Underline::Type::SOLID; }
  float GetDashedUnderlineWidth() const override { return 0.0f; }
  float GetDashedUnderlineGap() const override { return 0.0f; }
  UiText::Length GetNumberOfUnderlineRuns() const override { return 0u; }
  void GetUnderlineRuns(UiText::UnderlinedGlyphRun*, UiText::UnderlineRunIndex, UiText::Length) const override {}
  const Vector2& GetOutlineOffset() const override { return mZeroVector; }
  const Vector4& GetOutlineColor() const override { return mTransparentColor; }
  uint16_t GetOutlineWidth() const override { return 0u; }
  bool IsOutlineEnabled() const override { return false; }
  const float& GetOutlineBlurRadius() const override { return mZeroFloat; }
  const Vector4& GetBackgroundColor() const override { return mTransparentColor; }
  bool IsBackgroundEnabled() const override { return false; }
  bool IsMarkupProcessorEnabled() const override { return false; }
  const UiText::GlyphInfo* GetHyphens() const override { return nullptr; }
  const UiText::Length* GetHyphenIndices() const override { return nullptr; }
  UiText::Length GetHyphensCount() const override { return 0u; }
  const Vector4& GetStrikethroughColor() const override { return mTransparentColor; }
  bool IsStrikethroughEnabled() const override { return false; }
  bool IsMarkupStrikethroughSet() const override { return false; }
  float GetStrikethroughHeight() const override { return 0.0f; }
  UiText::Length GetNumberOfStrikethroughRuns() const override { return 0u; }
  UiText::Length GetNumberOfBoundedParagraphRuns() const override { return 0u; }
  const Vector<UiText::BoundedParagraphRun>& GetBoundedParagraphRuns() const override { return mBoundedParagraphRuns; }
  UiText::Length GetNumberOfCharacterSpacingGlyphRuns() const override { return 0u; }
  const Vector<UiText::CharacterSpacingGlyphRun>& GetCharacterSpacingGlyphRuns() const override { return mCharacterSpacingGlyphRuns; }
  void GetStrikethroughRuns(UiText::StrikethroughGlyphRun*, UiText::StrikethroughRunIndex, UiText::Length) const override {}
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
