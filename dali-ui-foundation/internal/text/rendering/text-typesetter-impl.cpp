/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/math/math-utils.h>
#include <memory.h>
#include <algorithm>
#include <cmath>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/character-spacing-glyph-run.h>
#include <dali-ui-foundation/internal/text/color-glyph-helper.h>
#include <dali-ui-foundation/internal/text/final-glyph-index-domain.h>
#include <dali-ui-foundation/internal/text/glyph-metrics-helper.h>
#include <dali-ui-foundation/internal/text/gradient-glyph-classification.h>
#include <dali-ui-foundation/internal/text/line-helper-functions.h>
#include <dali-ui-foundation/internal/text/line-run.h>
#include <dali-ui-foundation/internal/text/rendering/styles/character-spacing-helper-functions.h>
#include <dali-ui-foundation/internal/text/rendering/styles/strikethrough-helper-functions.h>
#include <dali-ui-foundation/internal/text/rendering/styles/underline-helper-functions.h>
#include <dali-ui-foundation/internal/text/rendering/text-typesetter-impl.h>
#include <dali-ui-foundation/internal/text/rendering/view-model.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-run-snapshot.h>
#include <dali-ui-foundation/internal/text/reveal/text-reveal.h>
#include <dali-ui-foundation/internal/text/strikethrough-glyph-run.h>
#include <dali-ui-foundation/internal/text/text-definitions.h>
#include <dali-ui-foundation/internal/text/underlined-glyph-run.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
struct RevealRasterContext
{
  PixelBuffer                   metadata;
  const Internal::Reveal::Plan* plan{nullptr};
  GlyphIndex                    currentGlyph{0u};
};

namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_TEXT_PERFORMANCE_MARKER, false);

const float HALF(0.5f);
const float ONE_AND_A_HALF(1.5f);

void RecordRevealPixel(RevealRasterContext* context, uint32_t pixelIndex, uint8_t coverage, bool overwrite)
{
  if(!context || coverage == 0u || !context->plan || context->currentGlyph >= context->plan->glyphToUnit.size() ||
     pixelIndex >= static_cast<uint64_t>(context->metadata.GetWidth()) * context->metadata.GetHeight())
  {
    return;
  }

  const uint32_t unit = context->plan->glyphToUnit[context->currentGlyph];
  if(unit == Internal::Reveal::NO_UNIT || unit >= context->plan->unitStart.size())
  {
    return;
  }

  uint8_t* pixel = context->metadata.GetBuffer() + pixelIndex * 4u;
  if(overwrite || pixel[2] == 0u || coverage >= pixel[3])
  {
    const float    normalizedStart = context->plan->unitStart[unit];
    const uint32_t encoded         = static_cast<uint32_t>(std::round(std::max(0.0f, std::min(1.0f, normalizedStart)) * 65535.0f));
    pixel[0]                       = static_cast<uint8_t>((encoded >> 8u) & 0xffu);
    pixel[1]                       = static_cast<uint8_t>(encoded & 0xffu);
    pixel[2]                       = 255u;
    pixel[3]                       = coverage;
  }
}

/**
 * @brief Fast multiply & divide by 255. It wiil be useful when we applying alpha value in color
 *
 * @param x The value between [0..255]
 * @param y The value between [0..255]
 * @return (x*y)/255
 */
inline uint8_t MultiplyAndNormalizeColor(const uint8_t x, const uint8_t y) noexcept
{
  const uint32_t xy = static_cast<const uint32_t>(x) * y;
  return ((xy << 15) + (xy << 7) + xy) >> 23;
}

/// Helper macro define for glyph typesetter. It will reduce some duplicated code line.
// clang-format off
/**
 * @brief Prepare decode glyph bitmap data. It must be call END_GLYPH_BITMAP end of same scope.
 */
#define BEGIN_GLYPH_BITMAP(data)                                                                                                                \
{                                                                                                                                               \
  uint32_t   glyphOffet               = 0u;                                                                                                     \
  const bool useLocalScanline         = data.glyphBitmap.compressionType != TextAbstraction::GlyphBufferData::CompressionType::NO_COMPRESSION;  \
  uint8_t* __restrict__ glyphScanline = useLocalScanline ? (uint8_t*)malloc(data.glyphBitmap.width * glyphPixelSize) : data.glyphBitmap.buffer; \
  DALI_ASSERT_ALWAYS(glyphScanline && "Glyph scanline for buffer is nullptr!");

/**
 * @brief Macro to skip useless line fast.
 */
#define SKIP_GLYPH_SCANLINE(skipLine)                                                                  \
if(useLocalScanline)                                                                                   \
{                                                                                                      \
  for(int32_t lineIndex = 0; lineIndex < skipLine; ++lineIndex)                                        \
  {                                                                                                    \
    TextAbstraction::GlyphBufferData::DecompressScanline(data.glyphBitmap, glyphScanline, glyphOffet); \
  }                                                                                                    \
}                                                                                                      \
else                                                                                                   \
{                                                                                                      \
  glyphScanline += skipLine * static_cast<int32_t>(data.glyphBitmap.width * glyphPixelSize);           \
}

/**
 * @brief Prepare scanline of glyph bitmap data per each lines. It must be call END_GLYPH_SCANLINE_DECODE end of same scope.
 */
#define BEGIN_GLYPH_SCANLINE_DECODE(data)                                                              \
{                                                                                                      \
  if(useLocalScanline)                                                                                 \
  {                                                                                                    \
    TextAbstraction::GlyphBufferData::DecompressScanline(data.glyphBitmap, glyphScanline, glyphOffet); \
  }

/**
 * @brief Finalize scanline of glyph bitmap data per each lines.
 */
#define END_GLYPH_SCANLINE_DECODE(data)                       \
  if(!useLocalScanline)                                       \
  {                                                           \
    glyphScanline += data.glyphBitmap.width * glyphPixelSize; \
  }                                                           \
} // For ensure that we call BEGIN_GLYPH_SCANLINE_DECODE before

/**
 * @brief Finalize decode glyph bitmap data.
 */
#define END_GLYPH_BITMAP() \
  if(useLocalScanline)     \
  {                        \
    free(glyphScanline);   \
  }                        \
} // For ensure that we call BEGIN_GLYPH_BITMAP before

// clang-format on
/// Helper macro define end.

/**
 * @brief Data struct used to set the buffer of the glyph's bitmap into the final bitmap's buffer.
 */
struct GlyphData
{
  PixelBuffer                      bitmapBuffer;     ///< The buffer of the whole bitmap. The format is RGBA8888.
  Vector2*                         position;         ///< The position of the glyph.
  TextAbstraction::GlyphBufferData glyphBitmap;      ///< The glyph's bitmap.
  uint32_t                         width;            ///< The bitmap's width.
  uint32_t                         height;           ///< The bitmap's height.
  int32_t                          horizontalOffset; ///< The horizontal offset to be added to the 'x' glyph's position.
  int32_t                          verticalOffset;   ///< The vertical offset to be added to the 'y' glyph's position.
  RevealRasterContext*             revealContext;    ///< Optional reveal metadata target for this traversal.
};

/**
 * @brief Sets the glyph's buffer into the bitmap's buffer.
 *
 * @param[in, out] data Struct which contains the glyph's data and the bitmap's data.
 * @param[in] position The position of the glyph.
 * @param[in] color The color of the glyph.
 * @param[in] style The style of the text.
 * @param[in] pixelFormat The format of the pixel in the image that the text is rendered as (i.e. either Pixel::BGRA8888
 * or Pixel::L8).
 */
template<bool RECORD_REVEAL>
void TypesetGlyph(GlyphData& __restrict__ data, const Vector2* const __restrict__ position,
                  const Vector4* const __restrict__ color, const Typesetter::Style style,
                  const Pixel::Format pixelFormat)
{
  if((0u == data.glyphBitmap.width) || (0u == data.glyphBitmap.height))
  {
    // Nothing to do if the width or height of the buffer is zero.
    return;
  }

  // Initial vertical / horizontal offset.
  const int32_t yOffset = data.verticalOffset + static_cast<int32_t>(position->y);
  const int32_t xOffset = data.horizontalOffset + static_cast<int32_t>(position->x);

  // Whether the given glyph is a color one.
  const bool     isColorGlyph    = Internal::IsColorGlyphBuffer(data.glyphBitmap);
  const uint32_t glyphPixelSize  = Pixel::GetBytesPerPixel(data.glyphBitmap.format);
  const uint32_t glyphAlphaIndex = (glyphPixelSize > 0u) ? glyphPixelSize - 1u : 0u;

  // Determinate iterator range.
  const int32_t lineIndexRangeMin = std::max(0, -yOffset);
  const int32_t lineIndexRangeMax =
    std::min(static_cast<int32_t>(data.glyphBitmap.height), static_cast<int32_t>(data.height) - yOffset);
  const int32_t indexRangeMin = std::max(0, -xOffset);
  const int32_t indexRangeMax =
    std::min(static_cast<int32_t>(data.glyphBitmap.width), static_cast<int32_t>(data.width) - xOffset);

  // If current glyph don't need to be rendered, just ignore.
  if(lineIndexRangeMax <= lineIndexRangeMin || indexRangeMax <= indexRangeMin)
  {
    return;
  }

  if(Pixel::RGBA8888 == pixelFormat)
  {
    uint32_t* __restrict__ bitmapBuffer = reinterpret_cast<uint32_t*>(data.bitmapBuffer.GetBuffer());
    // Skip basic line.
    bitmapBuffer += (lineIndexRangeMin + yOffset) * static_cast<int32_t>(data.width);

    // Fast-cut if style is MASK or OUTLINE. Outline not shown for color glyph.
    // Just overwrite transparent color and return.
    if(isColorGlyph && (Typesetter::STYLE_MASK == style || Typesetter::STYLE_OUTLINE == style))
    {
      for(int32_t lineIndex = lineIndexRangeMin; lineIndex < lineIndexRangeMax; ++lineIndex)
      {
        // We can use memset here.
        memset(bitmapBuffer + xOffset + indexRangeMin, 0, (indexRangeMax - indexRangeMin) * sizeof(uint32_t));
        bitmapBuffer += data.width;
      }
      return;
    }

    const bool swapChannelsBR = Pixel::BGRA8888 == data.glyphBitmap.format;

    // Precalculate input color's packed result.
    uint32_t packedInputColor                    = 0u;
    uint8_t* __restrict__ packedInputColorBuffer = reinterpret_cast<uint8_t*>(&packedInputColor);

    *(packedInputColorBuffer + 3u) = static_cast<uint8_t>(color->a * 255);
    *(packedInputColorBuffer + 2u) = static_cast<uint8_t>(color->b * 255);
    *(packedInputColorBuffer + 1u) = static_cast<uint8_t>(color->g * 255);
    *(packedInputColorBuffer)      = static_cast<uint8_t>(color->r * 255);

    // Prepare glyph bitmap
    BEGIN_GLYPH_BITMAP(data);

    // Skip basic line of glyph.
    SKIP_GLYPH_SCANLINE(lineIndexRangeMin);

    // Traverse the pixels of the glyph line per line.
    if(isColorGlyph)
    {
      for(int32_t lineIndex = lineIndexRangeMin; lineIndex < lineIndexRangeMax; ++lineIndex)
      {
        BEGIN_GLYPH_SCANLINE_DECODE(data);

        for(int32_t index = indexRangeMin; index < indexRangeMax; ++index)
        {
          const int32_t xOffsetIndex = xOffset + index;

          // Retrieves the color from the color glyph.
          uint32_t packedColorGlyph                    = *(reinterpret_cast<const uint32_t*>(glyphScanline + (index << 2)));
          uint8_t* __restrict__ packedColorGlyphBuffer = reinterpret_cast<uint8_t*>(&packedColorGlyph);

          // Update the alpha channel.
          const uint8_t colorAlpha =
            MultiplyAndNormalizeColor(*(packedInputColorBuffer + 3u), *(packedColorGlyphBuffer + 3u));
          *(packedColorGlyphBuffer + 3u) = colorAlpha;

          if constexpr(RECORD_REVEAL)
          {
            RecordRevealPixel(data.revealContext, static_cast<uint32_t>(lineIndex + yOffset) * data.width + static_cast<uint32_t>(xOffsetIndex),
                              colorAlpha, true);
          }

          if(Typesetter::STYLE_SHADOW == style)
          {
            // The shadow of color glyph needs to have the shadow color.
            *(packedColorGlyphBuffer + 2u) = MultiplyAndNormalizeColor(*(packedInputColorBuffer + 2u), colorAlpha);
            *(packedColorGlyphBuffer + 1u) = MultiplyAndNormalizeColor(*(packedInputColorBuffer + 1u), colorAlpha);
            *packedColorGlyphBuffer        = MultiplyAndNormalizeColor(*packedInputColorBuffer, colorAlpha);
          }
          else
          {
            if(swapChannelsBR)
            {
              std::swap(*packedColorGlyphBuffer, *(packedColorGlyphBuffer + 2u)); // Swap B and R.
            }

            *(packedColorGlyphBuffer + 2u) = MultiplyAndNormalizeColor(*(packedColorGlyphBuffer + 2u), colorAlpha);
            *(packedColorGlyphBuffer + 1u) = MultiplyAndNormalizeColor(*(packedColorGlyphBuffer + 1u), colorAlpha);
            *packedColorGlyphBuffer        = MultiplyAndNormalizeColor(*packedColorGlyphBuffer, colorAlpha);

            if(data.glyphBitmap.isColorBitmap)
            {
              *(packedColorGlyphBuffer + 2u) =
                MultiplyAndNormalizeColor(*(packedInputColorBuffer + 2u), *(packedColorGlyphBuffer + 2u));
              *(packedColorGlyphBuffer + 1u) =
                MultiplyAndNormalizeColor(*(packedInputColorBuffer + 1u), *(packedColorGlyphBuffer + 1u));
              *packedColorGlyphBuffer = MultiplyAndNormalizeColor(*packedInputColorBuffer, *packedColorGlyphBuffer);
            }
          }

          // Set the color into the final pixel buffer.
          *(bitmapBuffer + xOffsetIndex) = packedColorGlyph;
        }

        bitmapBuffer += data.width;

        END_GLYPH_SCANLINE_DECODE(data);
      }
    }
    else
    {
      for(int32_t lineIndex = lineIndexRangeMin; lineIndex < lineIndexRangeMax; ++lineIndex)
      {
        BEGIN_GLYPH_SCANLINE_DECODE(data);

        for(int32_t index = indexRangeMin; index < indexRangeMax; ++index)
        {
          // Update the alpha channel.
          const uint8_t alpha = *(glyphScanline + index * glyphPixelSize + glyphAlphaIndex);

          // Copy non-transparent pixels only
          if(alpha > 0u)
          {
            const int32_t xOffsetIndex = xOffset + index;

            if constexpr(RECORD_REVEAL)
            {
              RecordRevealPixel(data.revealContext, static_cast<uint32_t>(lineIndex + yOffset) * data.width + static_cast<uint32_t>(xOffsetIndex),
                                alpha, false);
            }

            // Check alpha of overlapped pixels
            uint32_t& currentColor             = *(bitmapBuffer + xOffsetIndex);
            uint8_t*  packedCurrentColorBuffer = reinterpret_cast<uint8_t*>(&currentColor);

            // For any pixel overlapped with the pixel in previous glyphs, make sure we don't
            // overwrite a previous bigger alpha with a smaller alpha (in order to avoid
            // semi-transparent gaps between joint glyphs with overlapped pixels, which could
            // happen, for example, in the RTL text when we copy glyphs from right to left).
            uint8_t currentAlpha = *(packedCurrentColorBuffer + 3u);
            currentAlpha         = std::max(currentAlpha, alpha);
            if(currentAlpha == 255)
            {
              // Fast-cut to avoid float type operation.
              currentColor = packedInputColor;
            }
            else
            {
              // Pack the given color into a 32bit buffer. The alpha channel will be updated later for each pixel.
              // The format is RGBA8888.
              uint32_t packedColor                    = 0u;
              uint8_t* __restrict__ packedColorBuffer = reinterpret_cast<uint8_t*>(&packedColor);

              // Color is pre-muliplied with its alpha.
              *(packedColorBuffer + 3u) = MultiplyAndNormalizeColor(*(packedInputColorBuffer + 3u), currentAlpha);
              *(packedColorBuffer + 2u) = MultiplyAndNormalizeColor(*(packedInputColorBuffer + 2u), currentAlpha);
              *(packedColorBuffer + 1u) = MultiplyAndNormalizeColor(*(packedInputColorBuffer + 1u), currentAlpha);
              *(packedColorBuffer)      = MultiplyAndNormalizeColor(*packedInputColorBuffer, currentAlpha);

              // Set the color into the final pixel buffer.
              currentColor = packedColor;
            }
          }
        }

        bitmapBuffer += data.width;

        END_GLYPH_SCANLINE_DECODE(data);
      }
    }

    END_GLYPH_BITMAP();
  }
  else // Pixel::L8
  {
    // Below codes required only if not color glyph.
    if(!isColorGlyph)
    {
      uint8_t* __restrict__ bitmapBuffer = data.bitmapBuffer.GetBuffer();
      // Skip basic line.
      bitmapBuffer += (lineIndexRangeMin + yOffset) * static_cast<int32_t>(data.width);

      // Prepare glyph bitmap
      BEGIN_GLYPH_BITMAP(data);

      // Skip basic line of glyph.
      SKIP_GLYPH_SCANLINE(lineIndexRangeMin);

      // Traverse the pixels of the glyph line per line.
      for(int32_t lineIndex = lineIndexRangeMin; lineIndex < lineIndexRangeMax; ++lineIndex)
      {
        BEGIN_GLYPH_SCANLINE_DECODE(data);

        for(int32_t index = indexRangeMin; index < indexRangeMax; ++index)
        {
          const int32_t xOffsetIndex = xOffset + index;

          // Update the alpha channel.
          const uint8_t alpha = *(glyphScanline + index * glyphPixelSize + glyphAlphaIndex);

          // Copy non-transparent pixels only
          if(alpha > 0u)
          {
            // Check alpha of overlapped pixels
            uint8_t& currentAlpha = *(bitmapBuffer + xOffsetIndex);

            // For any pixel overlapped with the pixel in previous glyphs, make sure we don't
            // overwrite a previous bigger alpha with a smaller alpha (in order to avoid
            // semi-transparent gaps between joint glyphs with overlapped pixels, which could
            // happen, for example, in the RTL text when we copy glyphs from right to left).
            currentAlpha = std::max(currentAlpha, alpha);
          }
        }

        bitmapBuffer += data.width;

        END_GLYPH_SCANLINE_DECODE(data);
      }

      END_GLYPH_BITMAP();
    }
  }
}

/// Draws the background color to the buffer
void DrawBackgroundColor(Vector4 backgroundColor, const uint32_t bufferWidth, const uint32_t bufferHeight,
                         GlyphData& glyphData, const float baseline, const LineRun& line, const float lineExtentLeft,
                         const float lineExtentRight)
{
  const int32_t yRangeMin = std::max(0, static_cast<int32_t>(glyphData.verticalOffset + baseline - line.ascender));
  const int32_t yRangeMax = std::min(static_cast<int32_t>(bufferHeight),
                                     static_cast<int32_t>(glyphData.verticalOffset + baseline - line.descender));
  const int32_t xRangeMin = std::max(0, static_cast<int32_t>(glyphData.horizontalOffset + lineExtentLeft));
  const int32_t xRangeMax =
    std::min(static_cast<int32_t>(bufferWidth), static_cast<int32_t>(glyphData.horizontalOffset + lineExtentRight +
                                                                     1)); // Due to include last point, we add 1 here

  // If current glyph don't need to be rendered, just ignore.
  if(yRangeMax <= yRangeMin || xRangeMax <= xRangeMin)
  {
    return;
  }

  // We can optimize by memset when backgroundColor.a is near zero
  uint8_t backgroundColorAlpha = static_cast<uint8_t>(backgroundColor.a * 255.f);

  uint32_t* bitmapBuffer = reinterpret_cast<uint32_t*>(glyphData.bitmapBuffer.GetBuffer());

  // Skip yRangeMin line.
  bitmapBuffer += yRangeMin * glyphData.width;

  if(backgroundColorAlpha == 0)
  {
    for(int32_t y = yRangeMin; y < yRangeMax; y++)
    {
      // We can use memset.
      memset(bitmapBuffer + xRangeMin, 0, (xRangeMax - xRangeMin) * sizeof(uint32_t));
      bitmapBuffer += glyphData.width;
    }
  }
  else
  {
    uint32_t packedBackgroundColor       = 0u;
    uint8_t* packedBackgroundColorBuffer = reinterpret_cast<uint8_t*>(&packedBackgroundColor);

    // Write the color to the pixel buffer
    *(packedBackgroundColorBuffer + 3u) = backgroundColorAlpha;
    *(packedBackgroundColorBuffer + 2u) = static_cast<uint8_t>(backgroundColor.b * backgroundColorAlpha);
    *(packedBackgroundColorBuffer + 1u) = static_cast<uint8_t>(backgroundColor.g * backgroundColorAlpha);
    *(packedBackgroundColorBuffer)      = static_cast<uint8_t>(backgroundColor.r * backgroundColorAlpha);

    for(int32_t y = yRangeMin; y < yRangeMax; y++)
    {
      for(int32_t x = xRangeMin; x < xRangeMax; x++)
      {
        // Note : this is same logic as bitmap[y][x] = backgroundColor;
        *(bitmapBuffer + x) = packedBackgroundColor;
      }
      bitmapBuffer += glyphData.width;
    }
  }
}

/// Draws the specified underline color to the buffer
void DrawUnderline(const uint32_t bufferWidth, const uint32_t bufferHeight, GlyphData& glyphData, const float baseline,
                   const float currentUnderlinePosition, const float maxUnderlineHeight, const float lineExtentLeft,
                   const float lineExtentRight, const UnderlineStyleProperties& commonUnderlineProperties,
                   const UnderlineStyleProperties& currentUnderlineProperties, const LineRun& line)
{
  const Vector4& underlineColor =
    currentUnderlineProperties.colorDefined ? currentUnderlineProperties.color : commonUnderlineProperties.color;
  const Text::Underline::Type underlineType =
    currentUnderlineProperties.typeDefined ? currentUnderlineProperties.type : commonUnderlineProperties.type;
  const float dashedUnderlineWidth = currentUnderlineProperties.dashWidthDefined ? currentUnderlineProperties.dashWidth
                                                                                 : commonUnderlineProperties.dashWidth;
  const float dashedUnderlineGap   = currentUnderlineProperties.dashGapDefined ? currentUnderlineProperties.dashGap
                                                                               : commonUnderlineProperties.dashGap;

  int32_t underlineYOffset = glyphData.verticalOffset + static_cast<int32_t>(baseline + currentUnderlinePosition);

  const uint32_t yRangeMin = underlineYOffset;
  const uint32_t yRangeMax = std::min(bufferHeight, underlineYOffset + static_cast<uint32_t>(maxUnderlineHeight));
  const uint32_t xRangeMin = static_cast<uint32_t>(glyphData.horizontalOffset + lineExtentLeft);
  const uint32_t xRangeMax =
    std::min(bufferWidth, static_cast<uint32_t>(glyphData.horizontalOffset + lineExtentRight +
                                                1)); // Due to include last point, we add 1 here

  // If current glyph don't need to be rendered, just ignore.
  if((underlineType != Text::Underline::Type::DOUBLE && yRangeMax <= yRangeMin) || xRangeMax <= xRangeMin)
  {
    return;
  }

  // We can optimize by memset when underlineColor.a is near zero
  uint8_t underlineColorAlpha = static_cast<uint8_t>(underlineColor.a * 255.f);

  uint32_t* bitmapBuffer = reinterpret_cast<uint32_t*>(glyphData.bitmapBuffer.GetBuffer());

  // Skip yRangeMin line.
  bitmapBuffer += yRangeMin * glyphData.width;

  // Note if underlineType is DASHED, we cannot setup color by memset.
  if(underlineType != Text::Underline::Type::DASHED && underlineColorAlpha == 0)
  {
    for(uint32_t y = yRangeMin; y < yRangeMax; y++)
    {
      // We can use memset.
      memset(bitmapBuffer + xRangeMin, 0, (xRangeMax - xRangeMin) * sizeof(uint32_t));
      bitmapBuffer += glyphData.width;
    }
    if(underlineType == Text::Underline::Type::DOUBLE)
    {
      int32_t        secondUnderlineYOffset = underlineYOffset - static_cast<int32_t>(ONE_AND_A_HALF * maxUnderlineHeight);
      const uint32_t secondYRangeMin        = static_cast<uint32_t>(std::max(0, secondUnderlineYOffset));
      const uint32_t secondYRangeMax        = static_cast<uint32_t>(
        std::max(0, std::min(static_cast<int32_t>(bufferHeight),
                                    secondUnderlineYOffset + static_cast<int32_t>(maxUnderlineHeight))));

      // Rewind bitmapBuffer pointer, and skip secondYRangeMin line.
      bitmapBuffer =
        reinterpret_cast<uint32_t*>(glyphData.bitmapBuffer.GetBuffer()) + secondYRangeMin * glyphData.width;

      for(uint32_t y = secondYRangeMin; y < secondYRangeMax; y++)
      {
        // We can use memset.
        memset(bitmapBuffer + xRangeMin, 0, (xRangeMax - xRangeMin) * sizeof(uint32_t));
        bitmapBuffer += glyphData.width;
      }
    }
  }
  else
  {
    uint32_t packedUnderlineColor       = 0u;
    uint8_t* packedUnderlineColorBuffer = reinterpret_cast<uint8_t*>(&packedUnderlineColor);

    // Write the color to the pixel buffer
    *(packedUnderlineColorBuffer + 3u) = underlineColorAlpha;
    *(packedUnderlineColorBuffer + 2u) = static_cast<uint8_t>(underlineColor.b * underlineColorAlpha);
    *(packedUnderlineColorBuffer + 1u) = static_cast<uint8_t>(underlineColor.g * underlineColorAlpha);
    *(packedUnderlineColorBuffer)      = static_cast<uint8_t>(underlineColor.r * underlineColorAlpha);

    for(uint32_t y = yRangeMin; y < yRangeMax; y++)
    {
      if(underlineType == Text::Underline::Type::DASHED)
      {
        float dashWidth = dashedUnderlineWidth;
        float dashGap   = 0;

        for(uint32_t x = xRangeMin; x < xRangeMax; x++)
        {
          if(Dali::EqualsZero(dashGap) && dashWidth > 0)
          {
            // Note : this is same logic as bitmap[y][x] = underlineColor;
            *(bitmapBuffer + x) = packedUnderlineColor;
            dashWidth--;
          }
          else if(dashGap < dashedUnderlineGap)
          {
            dashGap++;
          }
          else
          {
            // reset
            dashWidth = dashedUnderlineWidth;
            dashGap   = 0;
          }
        }
      }
      else
      {
        for(uint32_t x = xRangeMin; x < xRangeMax; x++)
        {
          // Note : this is same logic as bitmap[y][x] = underlineColor;
          *(bitmapBuffer + x) = packedUnderlineColor;
        }
      }
      bitmapBuffer += glyphData.width;
    }
    if(underlineType == Text::Underline::Type::DOUBLE)
    {
      int32_t        secondUnderlineYOffset = underlineYOffset - static_cast<int32_t>(ONE_AND_A_HALF * maxUnderlineHeight);
      const uint32_t secondYRangeMin        = static_cast<uint32_t>(std::max(0, secondUnderlineYOffset));
      const uint32_t secondYRangeMax        = static_cast<uint32_t>(
        std::max(0, std::min(static_cast<int32_t>(bufferHeight),
                                    secondUnderlineYOffset + static_cast<int32_t>(maxUnderlineHeight))));

      // Rewind bitmapBuffer pointer, and skip secondYRangeMin line.
      bitmapBuffer =
        reinterpret_cast<uint32_t*>(glyphData.bitmapBuffer.GetBuffer()) + secondYRangeMin * glyphData.width;

      for(uint32_t y = secondYRangeMin; y < secondYRangeMax; y++)
      {
        for(uint32_t x = xRangeMin; x < xRangeMax; x++)
        {
          // Note : this is same logic as bitmap[y][x] = underlineColor;
          *(bitmapBuffer + x) = packedUnderlineColor;
        }
        bitmapBuffer += glyphData.width;
      }
    }
  }
}

/// Draws the specified strikethrough color to the buffer
void DrawStrikethrough(const uint32_t bufferWidth, const uint32_t bufferHeight, GlyphData& glyphData,
                       const float baseline, const float strikethroughStartingYPosition,
                       const float maxStrikethroughHeight, const float lineExtentLeft, const float lineExtentRight,
                       const StrikethroughStyleProperties& commonStrikethroughProperties,
                       const StrikethroughStyleProperties& currentStrikethroughProperties, const LineRun& line)
{
  const Vector4& strikethroughColor = currentStrikethroughProperties.colorDefined ? currentStrikethroughProperties.color
                                                                                  : commonStrikethroughProperties.color;

  const uint32_t yRangeMin = static_cast<uint32_t>(strikethroughStartingYPosition);
  const uint32_t yRangeMax =
    std::min(bufferHeight, static_cast<uint32_t>(strikethroughStartingYPosition + maxStrikethroughHeight));
  const uint32_t xRangeMin = static_cast<uint32_t>(glyphData.horizontalOffset + lineExtentLeft);
  const uint32_t xRangeMax =
    std::min(bufferWidth, static_cast<uint32_t>(glyphData.horizontalOffset + lineExtentRight +
                                                1)); // Due to include last point, we add 1 here

  // If current glyph don't need to be rendered, just ignore.
  if(yRangeMax <= yRangeMin || xRangeMax <= xRangeMin)
  {
    return;
  }

  // We can optimize by memset when strikethroughColor.a is near zero
  uint8_t strikethroughColorAlpha = static_cast<uint8_t>(strikethroughColor.a * 255.f);

  uint32_t* bitmapBuffer = reinterpret_cast<uint32_t*>(glyphData.bitmapBuffer.GetBuffer());

  // Skip yRangeMin line.
  bitmapBuffer += yRangeMin * glyphData.width;

  if(strikethroughColorAlpha == 0)
  {
    for(uint32_t y = yRangeMin; y < yRangeMax; y++)
    {
      // We can use memset.
      memset(bitmapBuffer + xRangeMin, 0, (xRangeMax - xRangeMin) * sizeof(uint32_t));
      bitmapBuffer += glyphData.width;
    }
  }
  else
  {
    uint32_t packedStrikethroughColor       = 0u;
    uint8_t* packedStrikethroughColorBuffer = reinterpret_cast<uint8_t*>(&packedStrikethroughColor);

    // Write the color to the pixel buffer
    *(packedStrikethroughColorBuffer + 3u) = strikethroughColorAlpha;
    *(packedStrikethroughColorBuffer + 2u) = static_cast<uint8_t>(strikethroughColor.b * strikethroughColorAlpha);
    *(packedStrikethroughColorBuffer + 1u) = static_cast<uint8_t>(strikethroughColor.g * strikethroughColorAlpha);
    *(packedStrikethroughColorBuffer)      = static_cast<uint8_t>(strikethroughColor.r * strikethroughColorAlpha);

    for(uint32_t y = yRangeMin; y < yRangeMax; y++)
    {
      for(uint32_t x = xRangeMin; x < xRangeMax; x++)
      {
        // Note : this is same logic as bitmap[y][x] = strikethroughColor;
        *(bitmapBuffer + x) = packedStrikethroughColor;
      }
      bitmapBuffer += glyphData.width;
    }
  }
}

/// Helper functions to create image buffer

struct InputParameterForEachLine
{
  const uint32_t bufferWidth;
  const uint32_t bufferHeight;
  const int32_t  horizontalOffset;

  const Vector2& styleOffset; ///< If style is STYLE_OUTLINE, outline offset. If style is STYLE_SHADOW, shadow offset.
                              ///< Otherwise, zero.

  const GlyphIndex fromGlyphIndex;
  const GlyphIndex toGlyphIndex;

  // Elide text info
  const GlyphIndex startIndexOfGlyphs;
  const GlyphIndex endIndexOfGlyphs;
  const GlyphIndex firstMiddleIndexOfElidedGlyphs;
  const GlyphIndex secondMiddleIndexOfElidedGlyphs;
  const float      elidedOffset;

  const Alignment                    verticalLineAlignType;
  const Text::EllipsisPosition::Type ellipsisPosition;

  const GlyphInfo* __restrict__ hyphens;
  const Length* __restrict__ hyphenIndices;
  const Length hyphensCount;

  const bool ignoreHorizontalAlignment : 1;
};

struct InputParameterForEachGlyph
{
  const Typesetter::Style style;
  const Pixel::Format     pixelFormat;

  const float outlineWidth;

  const float modelCharacterSpacing;

  const Vector4&
    defaultColor; ///< The default color for the text.
                  ///  Or some color which depends on style value. (e.g. ShadowColor if style is STYLE_SHADOW)

  const Vector<UnderlinedGlyphRun>&       underlineRuns;
  const Vector<StrikethroughGlyphRun>&    strikethroughRuns;
  const Vector<CharacterSpacingGlyphRun>& characterSpacingGlyphRuns;

  const GlyphInfo* const __restrict__ glyphsBuffer;
  const Character* __restrict__ textBuffer;
  const CharacterIndex* __restrict__ glyphToCharacterMapBuffer;
  const GlyphIndex* __restrict__ finalGlyphStyleSourceBuffer;

  const Vector2* const __restrict__ positionBuffer;

  const Vector4* const __restrict__ colorsBuffer;
  const TextAbstraction::ColorIndex* const __restrict__ colorIndexBuffer;

  const UnderlineStyleProperties     modelUnderlineProperties;
  const StrikethroughStyleProperties modelStrikethroughProperties;

  const bool underlineEnabled : 1;
  const bool strikethroughEnabled : 1;
  const bool cutoutEnabled : 1;

  const bool removeFrontInset : 1;
  const bool removeBackInset : 1;

  const bool useDefaultColor : 1;
};

struct OutputParameterForEachGlyph
{
  UnderlineStyleProperties& currentUnderlineProperties;

  float& maxUnderlineHeight;
  bool&  thereAreUnderlinedGlyphs;

  StrikethroughStyleProperties& currentStrikethroughProperties;

  float& maxStrikethroughHeight;
  bool&  thereAreStrikethroughGlyphs;

  float& currentUnderlinePosition;

  float& baseline;
  float& lineExtentLeft;
  float& lineExtentRight;

  FontId& lastFontId;
};

template<bool RECORD_REVEAL>
void CreateImageBufferForEachGlyph(TextAbstraction::FontClient fontClient, GlyphData& glyphData, GlyphIndex& glyphIndex,
                                   const GlyphIndex elidedGlyphIndex, const GlyphInfo* glyphInfo, const bool addHyphen,
                                   const InputParameterForEachGlyph& inputParamsForGlyph,
                                   OutputParameterForEachGlyph&      outputParamsForGlyph)
{
  const GlyphIndex styleGlyphIndex = inputParamsForGlyph.finalGlyphStyleSourceBuffer
                                       ? ResolveFinalStyleSourceGlyph(inputParamsForGlyph.finalGlyphStyleSourceBuffer,
                                                                      elidedGlyphIndex)
                                       : glyphIndex;
  if constexpr(RECORD_REVEAL)
  {
    // Reveal plans are indexed by the canonical final glyph sequence. The
    // source-domain glyphIndex is retained for colors/styles, while the
    // compacted elidedGlyphIndex selects reveal timing.
    glyphData.revealContext->currentGlyph = elidedGlyphIndex;
  }

  // Replacement glyphs reserve layout space only. They never participate in
  // font bitmap lookup, text effects, underline or strikethrough rasterization.
  if(IsSyntheticReplacementGlyph(*glyphInfo))
  {
    return;
  }

  Vector<UnderlinedGlyphRun>::ConstIterator currentUnderlinedGlyphRunIt = inputParamsForGlyph.underlineRuns.End();
  const bool                                underlineGlyph =
    inputParamsForGlyph.underlineEnabled ||
    IsGlyphUnderlined(styleGlyphIndex, inputParamsForGlyph.underlineRuns, currentUnderlinedGlyphRunIt);
  outputParamsForGlyph.currentUnderlineProperties =
    GetCurrentUnderlineProperties(styleGlyphIndex, underlineGlyph, inputParamsForGlyph.underlineRuns,
                                  currentUnderlinedGlyphRunIt, inputParamsForGlyph.modelUnderlineProperties);
  float currentUnderlineHeight = outputParamsForGlyph.currentUnderlineProperties.height;

  outputParamsForGlyph.thereAreUnderlinedGlyphs = outputParamsForGlyph.thereAreUnderlinedGlyphs || underlineGlyph;

  Vector<StrikethroughGlyphRun>::ConstIterator currentStrikethroughGlyphRunIt =
    inputParamsForGlyph.strikethroughRuns.End();
  const bool strikethroughGlyph =
    inputParamsForGlyph.strikethroughEnabled ||
    IsGlyphStrikethrough(styleGlyphIndex, inputParamsForGlyph.strikethroughRuns, currentStrikethroughGlyphRunIt);
  outputParamsForGlyph.currentStrikethroughProperties = GetCurrentStrikethroughProperties(
    styleGlyphIndex, strikethroughGlyph, inputParamsForGlyph.strikethroughRuns, currentStrikethroughGlyphRunIt,
    inputParamsForGlyph.modelStrikethroughProperties);
  float currentStrikethroughHeight = outputParamsForGlyph.currentStrikethroughProperties.height;

  outputParamsForGlyph.thereAreStrikethroughGlyphs =
    outputParamsForGlyph.thereAreStrikethroughGlyphs || strikethroughGlyph;

  // Are we still using the same fontId as previous
  if((glyphInfo->fontId != outputParamsForGlyph.lastFontId) && (strikethroughGlyph || underlineGlyph))
  {
    // We need to fetch fresh font underline metrics
    FontMetrics fontMetrics;
    fontClient.GetFontMetrics(glyphInfo->fontId, fontMetrics);

    // The currentUnderlinePosition will be used for both Underline and/or Strikethrough
    outputParamsForGlyph.currentUnderlinePosition = FetchUnderlinePositionFromFontMetrics(fontMetrics);

    if(underlineGlyph)
    {
      CalcualteUnderlineHeight(fontMetrics, currentUnderlineHeight, outputParamsForGlyph.maxUnderlineHeight);
    }

    if(strikethroughGlyph)
    {
      CalcualteStrikethroughHeight(currentStrikethroughHeight, outputParamsForGlyph.maxStrikethroughHeight);
    }

    // Update lastFontId because fontId is changed
    outputParamsForGlyph.lastFontId =
      glyphInfo->fontId; // Prevents searching for existing blocksizes when string of the same fontId.
  }

  // Retrieves the glyph's position.
  Vector2 position = *(inputParamsForGlyph.positionBuffer + elidedGlyphIndex);

  if(addHyphen)
  {
    GlyphInfo   tempInfo          = *(inputParamsForGlyph.glyphsBuffer + elidedGlyphIndex);
    const float characterSpacing  = GetGlyphCharacterSpacing(styleGlyphIndex, inputParamsForGlyph.characterSpacingGlyphRuns,
                                                             inputParamsForGlyph.modelCharacterSpacing);
    const float calculatedAdvance = GetCalculatedAdvance(
      *(inputParamsForGlyph.textBuffer + (*(inputParamsForGlyph.glyphToCharacterMapBuffer + elidedGlyphIndex))),
      characterSpacing, tempInfo.advance);
    position.x = position.x + calculatedAdvance - tempInfo.xBearing + glyphInfo->xBearing;
    position.y = -glyphInfo->yBearing;
  }

  if(outputParamsForGlyph.baseline < position.y + glyphInfo->yBearing)
  {
    outputParamsForGlyph.baseline = position.y + glyphInfo->yBearing;
  }

  // Calculate the positions of leftmost and rightmost glyphs in the current line
  if(inputParamsForGlyph.removeFrontInset)
  {
    if(position.x < outputParamsForGlyph.lineExtentLeft)
    {
      outputParamsForGlyph.lineExtentLeft = position.x;
    }
  }
  else
  {
    const float originPositionLeft = position.x - glyphInfo->xBearing;
    if(originPositionLeft < outputParamsForGlyph.lineExtentLeft)
    {
      outputParamsForGlyph.lineExtentLeft = originPositionLeft;
    }
  }

  if(inputParamsForGlyph.removeBackInset)
  {
    if(position.x + glyphInfo->width > outputParamsForGlyph.lineExtentRight)
    {
      outputParamsForGlyph.lineExtentRight = position.x + glyphInfo->width;
    }
  }
  else
  {
    const float originPositionRight = position.x - glyphInfo->xBearing + glyphInfo->advance;
    if(originPositionRight > outputParamsForGlyph.lineExtentRight)
    {
      outputParamsForGlyph.lineExtentRight = originPositionRight;
    }
  }

  // Retrieves the glyph's color.
  const ColorIndex colorIndex =
    inputParamsForGlyph.useDefaultColor ? 0u : *(inputParamsForGlyph.colorIndexBuffer + glyphIndex);

  Vector4 color;
  if(inputParamsForGlyph.style == Typesetter::STYLE_SHADOW)
  {
    color = inputParamsForGlyph.defaultColor;
  }
  else if(inputParamsForGlyph.style == Typesetter::STYLE_OUTLINE)
  {
    color = inputParamsForGlyph.defaultColor;
  }
  else
  {
    color = (inputParamsForGlyph.useDefaultColor || (0u == colorIndex))
              ? inputParamsForGlyph.defaultColor
              : *(inputParamsForGlyph.colorsBuffer + (colorIndex - 1u));
  }

  if(inputParamsForGlyph.style == Typesetter::STYLE_NONE && inputParamsForGlyph.cutoutEnabled)
  {
    // Temporarily adjust the transparency to 1.f
    color.a = 1.f;
  }

  // Premultiply alpha
  color.r *= color.a;
  color.g *= color.a;
  color.b *= color.a;

  // Retrieves the glyph's bitmap.
  glyphData.glyphBitmap.buffer = nullptr;
  glyphData.glyphBitmap.width  = static_cast<uint32_t>(glyphInfo->width); // Desired width and height.
  glyphData.glyphBitmap.height = static_cast<uint32_t>(glyphInfo->height);

  float outlineWidth = inputParamsForGlyph.outlineWidth;

  if(inputParamsForGlyph.style != Typesetter::STYLE_OUTLINE && inputParamsForGlyph.style != Typesetter::STYLE_SHADOW)
  {
    // Don't render outline for other styles
    outlineWidth = 0.0f;
  }

  if(inputParamsForGlyph.style != Typesetter::STYLE_UNDERLINE &&
     inputParamsForGlyph.style != Typesetter::STYLE_STRIKETHROUGH)
  {
    fontClient.CreateBitmap(glyphInfo->fontId, glyphInfo->index, glyphInfo->isItalicRequired, glyphInfo->isBoldRequired,
                            glyphData.glyphBitmap, static_cast<int32_t>(outlineWidth));
  }

  // Sets the glyph's bitmap into the bitmap of the whole text.
  if(nullptr != glyphData.glyphBitmap.buffer)
  {
    if(inputParamsForGlyph.style == Typesetter::STYLE_OUTLINE)
    {
      // Set the position offset for the current glyph
      glyphData.horizontalOffset -= glyphData.glyphBitmap.outlineOffsetX;
      glyphData.verticalOffset -= glyphData.glyphBitmap.outlineOffsetY;
    }

    // Set the buffer of the glyph's bitmap into the final bitmap's buffer
    // The caller selects the specialization once per line. The ordinary Label
    // instantiation contains no reveal-specific branch in either its glyph or
    // per-pixel raster loops.
    TypesetGlyph<RECORD_REVEAL>(glyphData, &position, &color, inputParamsForGlyph.style, inputParamsForGlyph.pixelFormat);

    if(inputParamsForGlyph.style == Typesetter::STYLE_OUTLINE)
    {
      // Reset the position offset for the next glyph
      glyphData.horizontalOffset += glyphData.glyphBitmap.outlineOffsetX;
      glyphData.verticalOffset += glyphData.glyphBitmap.outlineOffsetY;
    }

    // free the glyphBitmap.buffer if it is owner of buffer
    if(glyphData.glyphBitmap.isBufferOwned)
    {
      free(glyphData.glyphBitmap.buffer);
      glyphData.glyphBitmap.isBufferOwned = false;
    }
    glyphData.glyphBitmap.buffer = nullptr;
  }
}

template<bool RECORD_REVEAL>
void CreateImageBufferForEachLine(TextAbstraction::FontClient fontClient, GlyphData& glyphData, Length& hyphenIndex,
                                  const LineRun& line, const bool isFirstLine,
                                  const InputParameterForEachLine&  inputParamsForLine,
                                  const InputParameterForEachGlyph& inputParamsForGlyph)
{
  // Sets the horizontal offset of the line.
  if(inputParamsForLine.ignoreHorizontalAlignment)
  {
    glyphData.horizontalOffset = 0;
  }
  else
  {
    glyphData.horizontalOffset = line.ellipsis ? static_cast<int32_t>(inputParamsForLine.elidedOffset)
                                               : static_cast<int32_t>(line.alignmentOffset);
  }
  glyphData.horizontalOffset += inputParamsForLine.horizontalOffset;

  // Increases the vertical offset with the line's ascender.
  glyphData.verticalOffset += static_cast<int32_t>(
    line.ascender + GetPreOffsetVerticalLineAlignment(line, inputParamsForLine.verticalLineAlignType));

  if(inputParamsForGlyph.style == Typesetter::STYLE_OUTLINE)
  {
    glyphData.horizontalOffset -= static_cast<int32_t>(inputParamsForGlyph.outlineWidth);
    glyphData.horizontalOffset += static_cast<int32_t>(inputParamsForLine.styleOffset.x);
    if(isFirstLine)
    {
      // Only need to add the vertical outline offset for the first line
      glyphData.verticalOffset -= static_cast<int32_t>(inputParamsForGlyph.outlineWidth);
      glyphData.verticalOffset += static_cast<int32_t>(inputParamsForLine.styleOffset.y);
    }
  }
  else if(inputParamsForGlyph.style == Typesetter::STYLE_SHADOW)
  {
    glyphData.horizontalOffset +=
      static_cast<int32_t>(inputParamsForLine.styleOffset.x -
                           inputParamsForGlyph.outlineWidth); // if outline enabled then shadow should offset from outline

    if(isFirstLine)
    {
      // Only need to add the vertical shadow offset for first line
      glyphData.verticalOffset += static_cast<int32_t>(inputParamsForLine.styleOffset.y - inputParamsForGlyph.outlineWidth);
    }
  }

  bool thereAreUnderlinedGlyphs    = false;
  bool thereAreStrikethroughGlyphs = false;

  float currentUnderlinePosition   = 0.0f;
  auto  currentUnderlineProperties = inputParamsForGlyph.modelUnderlineProperties;
  float maxUnderlineHeight         = currentUnderlineProperties.height;

  auto  currentStrikethroughProperties = inputParamsForGlyph.modelStrikethroughProperties;
  float maxStrikethroughHeight         = currentStrikethroughProperties.height;

  FontId lastFontId = 0;

  float lineExtentLeft  = static_cast<float>(inputParamsForLine.bufferWidth);
  float lineExtentRight = 0.0f;
  float baseline        = 0.0f;
  bool  addHyphen       = false;

  // Traverses the glyphs of the line.
  const GlyphIndex startGlyphIndex = std::max(std::max(line.glyphRun.glyphIndex, inputParamsForLine.startIndexOfGlyphs),
                                              inputParamsForLine.fromGlyphIndex);
  GlyphIndex       endGlyphIndex =
    (line.isSplitToTwoHalves ? line.glyphRunSecondHalf.glyphIndex + line.glyphRunSecondHalf.numberOfGlyphs
                             : line.glyphRun.glyphIndex + line.glyphRun.numberOfGlyphs) -
    1u;
  endGlyphIndex =
    std::min(std::min(endGlyphIndex, inputParamsForLine.endIndexOfGlyphs), inputParamsForLine.toGlyphIndex);

  for(GlyphIndex glyphIndex = startGlyphIndex; glyphIndex <= endGlyphIndex; ++glyphIndex)
  {
    // To handle START case of ellipsis, the first glyph has been shifted
    // glyphIndex represent indices in whole glyphs but elidedGlyphIndex represents indices in elided Glyphs
    GlyphIndex elidedGlyphIndex = glyphIndex - inputParamsForLine.startIndexOfGlyphs;

    // To handle MIDDLE case of ellipsis, the first glyph in the second half of line has been shifted and skip the
    // removed glyph from middle.
    if(inputParamsForLine.ellipsisPosition == Text::EllipsisPosition::MIDDLE)
    {
      if(glyphIndex > inputParamsForLine.firstMiddleIndexOfElidedGlyphs &&
         glyphIndex < inputParamsForLine.secondMiddleIndexOfElidedGlyphs)
      {
        // Ignore any glyph that removed for MIDDLE ellipsis
        continue;
      }
      if(glyphIndex >= inputParamsForLine.secondMiddleIndexOfElidedGlyphs)
      {
        elidedGlyphIndex -= (inputParamsForLine.secondMiddleIndexOfElidedGlyphs -
                             inputParamsForLine.firstMiddleIndexOfElidedGlyphs - 1u);
      }
    }

    // Retrieve the glyph's info.
    const GlyphInfo* glyphInfo;

    if(addHyphen && inputParamsForLine.hyphens)
    {
      glyphInfo = inputParamsForLine.hyphens + hyphenIndex;
      hyphenIndex++;
    }
    else
    {
      glyphInfo = inputParamsForGlyph.glyphsBuffer + elidedGlyphIndex;
    }

    if(IsSyntheticReplacementGlyph(*glyphInfo) ||
       (glyphInfo->width < Math::MACHINE_EPSILON_1000) || (glyphInfo->height < Math::MACHINE_EPSILON_1000))
    {
      // Nothing to do if the glyph's width or height is zero.
      continue;
    }

    // Collect output l-values
    // clang-format off
    OutputParameterForEachGlyph outputParamsForGlyph{currentUnderlineProperties,

                                                     maxUnderlineHeight,
                                                     thereAreUnderlinedGlyphs,

                                                     currentStrikethroughProperties,

                                                     maxStrikethroughHeight,
                                                     thereAreStrikethroughGlyphs,

                                                     currentUnderlinePosition,

                                                     baseline,
                                                     lineExtentLeft,
                                                     lineExtentRight,

                                                     lastFontId};
    // clang-format on

    CreateImageBufferForEachGlyph<RECORD_REVEAL>(fontClient, glyphData, glyphIndex, elidedGlyphIndex, glyphInfo,
                                                 addHyphen, inputParamsForGlyph, outputParamsForGlyph);

    if(inputParamsForLine.hyphenIndices)
    {
      while((hyphenIndex < inputParamsForLine.hyphensCount) &&
            (glyphIndex > inputParamsForLine.hyphenIndices[hyphenIndex]))
      {
        hyphenIndex++;
      }

      addHyphen = ((hyphenIndex < inputParamsForLine.hyphensCount) &&
                   ((glyphIndex + 1) == inputParamsForLine.hyphenIndices[hyphenIndex]));
      if(addHyphen)
      {
        glyphIndex--;
      }
    }
  }

  // Draw the underline from the leftmost glyph to the rightmost glyph
  if(thereAreUnderlinedGlyphs && inputParamsForGlyph.style == Typesetter::STYLE_UNDERLINE)
  {
    DrawUnderline(inputParamsForLine.bufferWidth, inputParamsForLine.bufferHeight, glyphData, baseline,
                  currentUnderlinePosition, maxUnderlineHeight, lineExtentLeft, lineExtentRight,
                  inputParamsForGlyph.modelUnderlineProperties, currentUnderlineProperties, line);
  }

  // Draw the background color from the leftmost glyph to the rightmost glyph
  if(inputParamsForGlyph.style == Typesetter::STYLE_BACKGROUND)
  {
    DrawBackgroundColor(inputParamsForGlyph.defaultColor, inputParamsForLine.bufferWidth,
                        inputParamsForLine.bufferHeight, glyphData, baseline, line, lineExtentLeft, lineExtentRight);
  }

  // Draw the strikethrough from the leftmost glyph to the rightmost glyph
  if(thereAreStrikethroughGlyphs && inputParamsForGlyph.style == Typesetter::STYLE_STRIKETHROUGH)
  {
    // TODO : The currently implemented strikethrough creates a strikethrough on the line level. We need to create
    // different strikethroughs the case of glyphs with different sizes.
    const float strikethroughStartingYPosition =
      (glyphData.verticalOffset + baseline + currentUnderlinePosition) -
      ((line.ascender) *
       HALF); // Since Free Type font doesn't contain the strikethrough-position property, strikethrough position will
              // be calculated by moving the underline position upwards by half the value of the line height.
    DrawStrikethrough(inputParamsForLine.bufferWidth, inputParamsForLine.bufferHeight, glyphData, baseline,
                      strikethroughStartingYPosition, maxStrikethroughHeight, lineExtentLeft, lineExtentRight,
                      inputParamsForGlyph.modelStrikethroughProperties, currentStrikethroughProperties, line);
  }

  // Increases the vertical offset with the line's descender & line spacing.
  glyphData.verticalOffset += static_cast<int32_t>(
    -line.descender + GetPostOffsetVerticalLineAlignment(line, inputParamsForLine.verticalLineAlignType));
}

void CreateTextGradientMaskImageBufferForEachLine(TextAbstraction::FontClient       fontClient,
                                                  GlyphData&                        glyphData,
                                                  Length&                           hyphenIndex,
                                                  const LineRun&                    line,
                                                  const InputParameterForEachLine&  inputParamsForLine,
                                                  const InputParameterForEachGlyph& inputParamsForGlyph,
                                                  const ColorIndex*                 gradientColorIndexBuffer,
                                                  const bool                        renderGradientTargets)
{
  if(inputParamsForLine.ignoreHorizontalAlignment)
  {
    glyphData.horizontalOffset = 0;
  }
  else
  {
    glyphData.horizontalOffset = line.ellipsis ? static_cast<int32_t>(inputParamsForLine.elidedOffset)
                                               : static_cast<int32_t>(line.alignmentOffset);
  }
  glyphData.horizontalOffset += inputParamsForLine.horizontalOffset;

  glyphData.verticalOffset += static_cast<int32_t>(
    line.ascender + GetPreOffsetVerticalLineAlignment(line, inputParamsForLine.verticalLineAlignType));

  UnderlineStyleProperties currentUnderlineProperties = inputParamsForGlyph.modelUnderlineProperties;
  float                    maxUnderlineHeight         = currentUnderlineProperties.height;
  bool                     thereAreUnderlinedGlyphs   = false;

  StrikethroughStyleProperties currentStrikethroughProperties = inputParamsForGlyph.modelStrikethroughProperties;
  float                        maxStrikethroughHeight         = currentStrikethroughProperties.height;
  bool                         thereAreStrikethroughGlyphs    = false;

  float  currentUnderlinePosition = 0.0f;
  float  lineExtentLeft           = static_cast<float>(inputParamsForLine.bufferWidth);
  float  lineExtentRight          = 0.0f;
  float  baseline                 = 0.0f;
  FontId lastFontId               = 0;
  bool   addHyphen                = false;

  const GlyphIndex startGlyphIndex = std::max(std::max(line.glyphRun.glyphIndex, inputParamsForLine.startIndexOfGlyphs),
                                              inputParamsForLine.fromGlyphIndex);
  GlyphIndex       endGlyphIndex =
    (line.isSplitToTwoHalves ? line.glyphRunSecondHalf.glyphIndex + line.glyphRunSecondHalf.numberOfGlyphs
                             : line.glyphRun.glyphIndex + line.glyphRun.numberOfGlyphs) -
    1u;
  endGlyphIndex =
    std::min(std::min(endGlyphIndex, inputParamsForLine.endIndexOfGlyphs), inputParamsForLine.toGlyphIndex);

  for(GlyphIndex glyphIndex = startGlyphIndex; glyphIndex <= endGlyphIndex; ++glyphIndex)
  {
    GlyphIndex elidedGlyphIndex = glyphIndex - inputParamsForLine.startIndexOfGlyphs;

    if(inputParamsForLine.ellipsisPosition == Text::EllipsisPosition::MIDDLE)
    {
      if(glyphIndex > inputParamsForLine.firstMiddleIndexOfElidedGlyphs &&
         glyphIndex < inputParamsForLine.secondMiddleIndexOfElidedGlyphs)
      {
        continue;
      }
      if(glyphIndex >= inputParamsForLine.secondMiddleIndexOfElidedGlyphs)
      {
        elidedGlyphIndex -= (inputParamsForLine.secondMiddleIndexOfElidedGlyphs -
                             inputParamsForLine.firstMiddleIndexOfElidedGlyphs - 1u);
      }
    }

    const GlyphInfo* glyphInfo = nullptr;
    if(addHyphen && inputParamsForLine.hyphens)
    {
      glyphInfo = inputParamsForLine.hyphens + hyphenIndex;
      hyphenIndex++;
    }
    else
    {
      glyphInfo = inputParamsForGlyph.glyphsBuffer + elidedGlyphIndex;
    }

    if(IsSyntheticReplacementGlyph(*glyphInfo) ||
       (glyphInfo->width < Math::MACHINE_EPSILON_1000) || (glyphInfo->height < Math::MACHINE_EPSILON_1000))
    {
      continue;
    }

    const Internal::GradientGlyphInfo classification =
      Internal::ClassifyGradientGlyph(fontClient, *glyphInfo, gradientColorIndexBuffer, glyphIndex);
    const bool shouldRenderGlyph =
      renderGradientTargets ? classification.usesGradientFill : !classification.usesGradientFill;
    if(!shouldRenderGlyph)
    {
      if(inputParamsForLine.hyphenIndices)
      {
        while((hyphenIndex < inputParamsForLine.hyphensCount) &&
              (glyphIndex > inputParamsForLine.hyphenIndices[hyphenIndex]))
        {
          hyphenIndex++;
        }

        addHyphen = ((hyphenIndex < inputParamsForLine.hyphensCount) &&
                     ((glyphIndex + 1) == inputParamsForLine.hyphenIndices[hyphenIndex]));
        if(addHyphen)
        {
          glyphIndex--;
        }
      }
      continue;
    }

    OutputParameterForEachGlyph outputParamsForGlyph{currentUnderlineProperties,

                                                     maxUnderlineHeight,
                                                     thereAreUnderlinedGlyphs,

                                                     currentStrikethroughProperties,

                                                     maxStrikethroughHeight,
                                                     thereAreStrikethroughGlyphs,

                                                     currentUnderlinePosition,

                                                     baseline,
                                                     lineExtentLeft,
                                                     lineExtentRight,

                                                     lastFontId};

    CreateImageBufferForEachGlyph<false>(fontClient, glyphData, glyphIndex, elidedGlyphIndex, glyphInfo, addHyphen,
                                         inputParamsForGlyph, outputParamsForGlyph);

    if(inputParamsForLine.hyphenIndices)
    {
      while((hyphenIndex < inputParamsForLine.hyphensCount) &&
            (glyphIndex > inputParamsForLine.hyphenIndices[hyphenIndex]))
      {
        hyphenIndex++;
      }

      addHyphen = ((hyphenIndex < inputParamsForLine.hyphensCount) &&
                   ((glyphIndex + 1) == inputParamsForLine.hyphenIndices[hyphenIndex]));
      if(addHyphen)
      {
        glyphIndex--;
      }
    }
  }

  glyphData.verticalOffset += static_cast<int32_t>(
    -line.descender + GetPostOffsetVerticalLineAlignment(line, inputParamsForLine.verticalLineAlignType));
}

/// Helper functions to create image buffer end

/**
 * @brief Create an initialized image buffer filled with transparent color.
 *
 * Creates the pixel data used to generate the final image with the given size.
 *
 * @param[in] bufferWidth The width of the image buffer.
 * @param[in] bufferHeight The height of the image buffer.
 * @param[in] pixelFormat The format of the pixel in the image that the text is rendered as (i.e. either Pixel::BGRA8888
 * or Pixel::L8).
 *
 * @return An image buffer.
 */
inline PixelBuffer CreateTransparentImageBuffer(const uint32_t bufferWidth, const uint32_t bufferHeight,
                                                const Pixel::Format pixelFormat)
{
  PixelBuffer imageBuffer = PixelBuffer::New(bufferWidth, bufferHeight, pixelFormat);

  if(Pixel::RGBA8888 == pixelFormat)
  {
    const uint32_t bufferSizeInt  = bufferWidth * bufferHeight;
    const size_t   bufferSizeChar = sizeof(uint32_t) * static_cast<std::size_t>(bufferSizeInt);
    memset(imageBuffer.GetBuffer(), 0, bufferSizeChar);
  }
  else
  {
    memset(imageBuffer.GetBuffer(), 0, static_cast<std::size_t>(bufferWidth * bufferHeight));
  }

  return imageBuffer;
}

} // namespace

ViewModel* Typesetter::Impl::GetViewModel()
{
  return mModel.get();
}

void Typesetter::Impl::SetFontClient(TextAbstraction::FontClient& fontClient)
{
  mFontClient = fontClient;
}

TextAbstraction::FontClient& Typesetter::Impl::GetFontClient()
{
  if(!mFontClient)
  {
    mFontClient = TextAbstraction::FontClient::Get();
  }
  return mFontClient;
}

void Internal::Reveal::ExpandMetadataOwnership(uint8_t* metadata, uint32_t width, uint32_t height)
{
  if(!metadata || width == 0u || height == 0u)
  {
    return;
  }

  constexpr uint32_t PIXEL_SIZE = 4u;
  const size_t       rowBytes   = static_cast<size_t>(width) * PIXEL_SIZE;

  struct OwnershipSource
  {
    uint32_t x;
    uint16_t start;
    uint8_t  coverage;
  };

  auto collectSources = [metadata, width, rowBytes, PIXEL_SIZE](uint32_t y, std::vector<OwnershipSource>& sources)
  {
    sources.clear();
    const uint8_t* row = metadata + static_cast<size_t>(y) * rowBytes;
    for(uint32_t x = 0u; x < width; ++x)
    {
      const uint8_t* pixel = row + static_cast<size_t>(x) * PIXEL_SIZE;
      if(pixel[2u] != 0u)
      {
        sources.push_back({x,
                           static_cast<uint16_t>(static_cast<uint16_t>(pixel[0u]) * 256u + pixel[1u]),
                           pixel[3u]});
      }
    }
  };

  // Source lists are captured before their rows can receive halo writes. The
  // three rolling lists therefore prevent recursive expansion while avoiding
  // a second full RGBA buffer. Temporary storage remains proportional to width.
  std::vector<OwnershipSource> previousSources;
  std::vector<OwnershipSource> currentSources;
  std::vector<OwnershipSource> nextSources;
  std::vector<uint32_t>        haloDestinations;
  std::vector<uint8_t>         originalOwnership(width, 0u);
  previousSources.reserve(width);
  currentSources.reserve(width);
  nextSources.reserve(width);
  haloDestinations.reserve(width);

  collectSources(0u, currentSources);
  if(height > 1u)
  {
    collectSources(1u, nextSources);
  }

  for(uint32_t y = 0u; y < height; ++y)
  {
    const std::vector<OwnershipSource>* sourceRows[] = {&previousSources, &currentSources, &nextSources};
    uint8_t*                            destination  = metadata + static_cast<size_t>(y) * rowBytes;
    haloDestinations.clear();
    for(const OwnershipSource& source : currentSources)
    {
      originalOwnership[source.x] = 1u;
    }

    for(const std::vector<OwnershipSource>* sourceRow : sourceRows)
    {
      for(const OwnershipSource& source : *sourceRow)
      {
        const uint32_t destinationBegin = source.x == 0u ? 0u : source.x - 1u;
        const uint32_t destinationEnd   = std::min(width - 1u, source.x + 1u);
        for(uint32_t destinationX = destinationBegin; destinationX <= destinationEnd; ++destinationX)
        {
          if(originalOwnership[destinationX] != 0u)
          {
            continue;
          }

          uint8_t*       destinationPixel = destination + static_cast<size_t>(destinationX) * PIXEL_SIZE;
          const uint16_t selectedStart =
            static_cast<uint16_t>(static_cast<uint16_t>(destinationPixel[0u]) * 256u + destinationPixel[1u]);
          if(destinationPixel[2u] == 0u || source.start > selectedStart ||
             (source.start == selectedStart && source.coverage > destinationPixel[3u]))
          {
            if(destinationPixel[2u] == 0u)
            {
              haloDestinations.push_back(destinationX);
            }
            const uint32_t encodedStart = static_cast<uint32_t>(source.start);
            destinationPixel[0u]        = static_cast<uint8_t>((encodedStart >> 8u) & 0xffu);
            destinationPixel[1u]        = static_cast<uint8_t>(encodedStart & 0xffu);
            destinationPixel[2u]        = 255u;
            // Source coverage is retained only while resolving conflicts in
            // this destination row and cleared once all candidates are known.
            destinationPixel[3u] = source.coverage;
          }
        }
      }
    }

    for(uint32_t destinationX : haloDestinations)
    {
      destination[static_cast<size_t>(destinationX) * PIXEL_SIZE + 3u] = 0u;
    }
    for(const OwnershipSource& source : currentSources)
    {
      originalOwnership[source.x] = 0u;
    }

    previousSources.swap(currentSources);
    currentSources.swap(nextSources);
    if(y + 2u < height)
    {
      collectSources(y + 2u, nextSources);
    }
    else
    {
      nextSources.clear();
    }
  }
}

void Typesetter::Impl::BeginRevealMetadata(uint32_t width, uint32_t height, const Internal::Reveal::Plan& plan)
{
  DALI_ASSERT_ALWAYS(!mRevealRasterContext && "Nested reveal metadata raster is not supported");
  mRevealRasterContext           = std::make_unique<RevealRasterContext>();
  mRevealRasterContext->metadata = PixelBuffer::New(width, height, Pixel::RGBA8888);
  mRevealRasterContext->plan     = &plan;
  memset(mRevealRasterContext->metadata.GetBuffer(), 0u, static_cast<size_t>(width) * height * 4u);
}

PixelData Typesetter::Impl::EndRevealMetadata()
{
  DALI_ASSERT_ALWAYS(mRevealRasterContext && "Reveal metadata raster was not started");
  Internal::Reveal::ExpandMetadataOwnership(mRevealRasterContext->metadata.GetBuffer(),
                                            mRevealRasterContext->metadata.GetWidth(),
                                            mRevealRasterContext->metadata.GetHeight());
  PixelData result = PixelBuffer::Convert(mRevealRasterContext->metadata);
  mRevealRasterContext.reset();
  return result;
}

PixelBuffer Typesetter::Impl::CreateTransparentImageBuffer(const uint32_t      bufferWidth,
                                                           const uint32_t      bufferHeight,
                                                           const Pixel::Format pixelFormat)
{
  return Dali::Ui::Text::CreateTransparentImageBuffer(bufferWidth, bufferHeight, pixelFormat);
}

void Typesetter::Impl::DrawGlyphsBackground(PixelBuffer& buffer, const uint32_t bufferWidth,
                                            const uint32_t bufferHeight, const bool ignoreHorizontalAlignment,
                                            const int32_t horizontalOffset, const int32_t verticalOffset)
{
  // Use l-value to make ensure it is not nullptr, so compiler happy.
  auto& viewModel = *(mModel.get());

  // Retrieve lines, glyphs, positions and colors from the view model.
  const Length            modelNumberOfLines           = viewModel.GetNumberOfLines();
  const LineRun* const    modelLinesBuffer             = viewModel.GetLines();
  const Length            numberOfGlyphs               = viewModel.GetNumberOfGlyphs();
  const GlyphInfo* const  glyphsBuffer                 = viewModel.GetGlyphs();
  const Vector2* const    positionBuffer               = viewModel.GetLayout();
  const Vector4* const    backgroundColorsBuffer       = viewModel.GetBackgroundColors();
  const ColorIndex* const backgroundColorIndicesBuffer = viewModel.GetBackgroundColorIndices();
  const bool              removeFrontInset             = viewModel.IsRemoveFrontInset();
  const bool              removeBackInset              = viewModel.IsRemoveBackInset();

  const Alignment verticalLineAlignType = viewModel.GetVerticalLineAlignment();

  // Create and initialize the pixel buffer.
  GlyphData glyphData;
  glyphData.verticalOffset   = verticalOffset;
  glyphData.width            = bufferWidth;
  glyphData.height           = bufferHeight;
  glyphData.bitmapBuffer     = buffer;
  glyphData.horizontalOffset = 0;
  glyphData.revealContext    = mRevealRasterContext.get();

  ColorIndex prevBackgroundColorIndex = 0;
  ColorIndex backgroundColorIndex     = 0;

  // Traverses the lines of the text.
  for(LineIndex lineIndex = 0u; lineIndex < modelNumberOfLines; ++lineIndex)
  {
    const LineRun& line = *(modelLinesBuffer + lineIndex);

    // Sets the horizontal offset of the line.
    glyphData.horizontalOffset = ignoreHorizontalAlignment ? 0 : static_cast<int32_t>(line.alignmentOffset);
    glyphData.horizontalOffset += horizontalOffset;

    // Increases the vertical offset with the line's ascender.
    glyphData.verticalOffset +=
      static_cast<int32_t>(line.ascender + GetPreOffsetVerticalLineAlignment(line, verticalLineAlignType));

    float left     = static_cast<float>(bufferWidth);
    float right    = 0.0f;
    float baseline = 0.0f;

    // Traverses the glyphs of the line.
    const GlyphIndex endGlyphIndex = std::min(numberOfGlyphs, line.glyphRun.glyphIndex + line.glyphRun.numberOfGlyphs);
    for(GlyphIndex glyphIndex = line.glyphRun.glyphIndex; glyphIndex < endGlyphIndex; ++glyphIndex)
    {
      // Retrieve the glyph's info.
      const GlyphInfo* const glyphInfo = glyphsBuffer + glyphIndex;

      if((glyphInfo->width < Math::MACHINE_EPSILON_1000) || (glyphInfo->height < Math::MACHINE_EPSILON_1000))
      {
        // Nothing to do if default background color, the glyph's width or height is zero.
        continue;
      }

      backgroundColorIndex = (nullptr == backgroundColorsBuffer) ? 0u : *(backgroundColorIndicesBuffer + glyphIndex);

      if((backgroundColorIndex != prevBackgroundColorIndex) && (prevBackgroundColorIndex != 0u))
      {
        const Vector4& backgroundColor = *(backgroundColorsBuffer + prevBackgroundColorIndex - 1u);
        DrawBackgroundColor(backgroundColor, bufferWidth, bufferHeight, glyphData, baseline, line, left, right);
      }

      if(backgroundColorIndex == 0u)
      {
        prevBackgroundColorIndex = backgroundColorIndex;
        // if background color is the default do nothing
        continue;
      }

      // Retrieves the glyph's position.
      const Vector2* const position = positionBuffer + glyphIndex;

      if(baseline < position->y + glyphInfo->yBearing)
      {
        baseline = position->y + glyphInfo->yBearing;
      }

      // Calculate the positions of leftmost and rightmost glyphs in the current line
      if(removeFrontInset)
      {
        if((position->x < left) || (backgroundColorIndex != prevBackgroundColorIndex))
        {
          left = position->x;
        }
      }
      else
      {
        const float originPositionLeft = position->x - glyphInfo->xBearing;
        if((originPositionLeft < left) || (backgroundColorIndex != prevBackgroundColorIndex))
        {
          left = originPositionLeft;
        }
      }

      if(removeBackInset)
      {
        if(position->x + glyphInfo->width > right)
        {
          right = position->x + glyphInfo->width;
        }
      }
      else
      {
        const float originPositionRight = position->x - glyphInfo->xBearing + glyphInfo->advance;
        if(originPositionRight > right)
        {
          right = originPositionRight;
        }
      }

      prevBackgroundColorIndex = backgroundColorIndex;
    }

    // draw last background at line end if not default
    if(backgroundColorIndex != 0u)
    {
      const Vector4& backgroundColor = *(backgroundColorsBuffer + backgroundColorIndex - 1u);
      DrawBackgroundColor(backgroundColor, bufferWidth, bufferHeight, glyphData, baseline, line, left, right);
    }

    // Increases the vertical offset with the line's descender.
    glyphData.verticalOffset +=
      static_cast<int32_t>(-line.descender + GetPostOffsetVerticalLineAlignment(line, verticalLineAlignType));
  }
}

PixelBuffer Typesetter::Impl::CreateImageBuffer(const uint32_t bufferWidth, const uint32_t bufferHeight,
                                                const Typesetter::Style style,
                                                const bool              ignoreHorizontalAlignment,
                                                const Pixel::Format pixelFormat, const int32_t horizontalOffset,
                                                const int32_t verticalOffset, const GlyphIndex fromGlyphIndex,
                                                const GlyphIndex toGlyphIndex)
{
  // Use l-value to make ensure it is not nullptr, so compiler happy.
  auto& viewModel = *(mModel.get());

  // Retrieve lines, glyphs, positions and colors from the view model.
  const Length modelNumberOfLines                       = viewModel.GetNumberOfLines();
  const LineRun* const __restrict__ modelLinesBuffer    = viewModel.GetLines();
  const GlyphInfo* const __restrict__ glyphsBuffer      = viewModel.GetGlyphs();
  const Vector2* const __restrict__ positionBuffer      = viewModel.GetLayout();
  const Vector4* const __restrict__ colorsBuffer        = viewModel.GetColors();
  const ColorIndex* const __restrict__ colorIndexBuffer = viewModel.GetColorIndices();
  const GlyphInfo* __restrict__ hyphens                 = viewModel.GetHyphens();
  const Length* __restrict__ hyphenIndices              = viewModel.GetHyphenIndices();
  const Length hyphensCount                             = viewModel.GetHyphensCount();

  // Create and initialize the pixel buffer.
  GlyphData glyphData;
  glyphData.verticalOffset   = verticalOffset;
  glyphData.width            = bufferWidth;
  glyphData.height           = bufferHeight;
  glyphData.bitmapBuffer     = CreateTransparentImageBuffer(bufferWidth, bufferHeight, pixelFormat);
  glyphData.horizontalOffset = 0;
  glyphData.revealContext    = mRevealRasterContext.get();

  Length hyphenIndex = 0;

  const Character* __restrict__ textBuffer                       = viewModel.GetTextBuffer();
  const Vector<CharacterIndex>& __restrict__ glyphToCharacterMap = viewModel.GetGlyphsToCharacters();
  const CharacterIndex* __restrict__ glyphToCharacterMapBuffer   = glyphToCharacterMap.Begin();

  // Get the underline runs.
  const Length               numberOfUnderlineRuns = viewModel.GetNumberOfUnderlineRuns();
  Vector<UnderlinedGlyphRun> underlineRuns;
  underlineRuns.Resize(numberOfUnderlineRuns);
  viewModel.GetUnderlineRuns(underlineRuns.Begin(), 0u, numberOfUnderlineRuns);

  // Get the strikethrough runs.
  const Length                  numberOfStrikethroughRuns = viewModel.GetNumberOfStrikethroughRuns();
  Vector<StrikethroughGlyphRun> strikethroughRuns;
  strikethroughRuns.Resize(numberOfStrikethroughRuns);
  viewModel.GetStrikethroughRuns(strikethroughRuns.Begin(), 0u, numberOfStrikethroughRuns);

  // Get the character-spacing runs.
  const Vector<CharacterSpacingGlyphRun>& __restrict__ characterSpacingGlyphRuns =
    viewModel.GetCharacterSpacingGlyphRuns();

  // clang-format off
  // Aggregate input parameter for each line from mModel
  const InputParameterForEachLine inputParamsForLine{bufferWidth,
                                                     bufferHeight,
                                                     horizontalOffset,

                                                     (style == Typesetter::STYLE_OUTLINE) ? viewModel.GetOutlineOffset() :
                                                     (style == Typesetter::STYLE_SHADOW)  ? viewModel.GetShadowOffset()  :
                                                     Vector2::ZERO,

                                                     fromGlyphIndex,
                                                     toGlyphIndex,

                                                     // Elided text info. Indices according to elided text and Ellipsis position.
                                                     viewModel.GetStartIndexOfElidedGlyphs(),
                                                     viewModel.GetEndIndexOfElidedGlyphs(),
                                                     viewModel.GetFirstMiddleIndexOfElidedGlyphs(),
                                                     viewModel.GetSecondMiddleIndexOfElidedGlyphs(),
                                                     viewModel.GetElidedOffset(),
                                                     viewModel.GetVerticalLineAlignment(),
                                                     viewModel.GetEllipsisPosition(),

                                                     hyphens,
                                                     hyphenIndices,
                                                     hyphensCount,

                                                     ignoreHorizontalAlignment};

  // Aggregate underline-style-properties from mModel
  const UnderlineStyleProperties modelUnderlineProperties{viewModel.GetUnderlineType(),
                                                          viewModel.GetUnderlineColor(),
                                                          viewModel.GetUnderlineHeight(),
                                                          viewModel.GetDashedUnderlineGap(),
                                                          viewModel.GetDashedUnderlineWidth(),
                                                          true,
                                                          true,
                                                          true,
                                                          true,
                                                          true};

  // Aggregate strikethrough-style-properties from mModel
  const StrikethroughStyleProperties modelStrikethroughProperties{viewModel.GetStrikethroughColor(),
                                                                  viewModel.GetStrikethroughHeight(),
                                                                  true,
                                                                  true};


  // Aggregate input parameter for each glyph from mModel
  const InputParameterForEachGlyph inputParamsForGlyph{style,
                                                       pixelFormat,

                                                       // Retrieves the glyph's outline width
                                                       viewModel.IsOutlineEnabled()
                                                         ? static_cast<float>(viewModel.GetOutlineWidth())
                                                         : 0.0f,

                                                       viewModel.GetCharacterSpacing(),

                                                       (style == Typesetter::STYLE_OUTLINE)    ? viewModel.GetOutlineColor()    :
                                                       (style == Typesetter::STYLE_SHADOW)     ? viewModel.GetShadowColor()     :
                                                       (style == Typesetter::STYLE_BACKGROUND) ? viewModel.GetBackgroundColor() :
                                                       viewModel.GetDefaultColor(),

                                                       underlineRuns,
                                                       strikethroughRuns,
                                                       characterSpacingGlyphRuns,

                                                       glyphsBuffer,
                                                       textBuffer,
                                                       glyphToCharacterMapBuffer,
                                                       viewModel.GetFinalGlyphStyleSourceIndices(),

                                                       positionBuffer,

                                                       colorsBuffer,
                                                       colorIndexBuffer,

                                                       modelUnderlineProperties,
                                                       modelStrikethroughProperties,

                                                       viewModel.IsUnderlineEnabled(),
                                                       viewModel.IsStrikethroughEnabled(),
                                                       viewModel.IsCutoutEnabled(),

                                                       viewModel.IsRemoveFrontInset(),
                                                       viewModel.IsRemoveBackInset(),

                                                       // Whether to use the default color.
                                                       (nullptr == colorsBuffer)};
  // clang-format on

  // Traverses the lines of the text.
  for(LineIndex lineIndex = 0u; lineIndex < modelNumberOfLines; ++lineIndex)
  {
    const LineRun& line = *(modelLinesBuffer + lineIndex);

    if(glyphData.revealContext)
    {
      // Metadata tiling only needs glyph bitmaps for lines intersecting this
      // tile. Preserve the exact vertical accumulation for skipped lines so a
      // glyph crossing a tile boundary is rasterized into both adjacent tiles.
      const int32_t lineTop = glyphData.verticalOffset +
                              static_cast<int32_t>(GetPreOffsetVerticalLineAlignment(line, inputParamsForLine.verticalLineAlignType));
      const int32_t lineBottom = lineTop + static_cast<int32_t>(line.ascender - line.descender +
                                                                GetPostOffsetVerticalLineAlignment(line, inputParamsForLine.verticalLineAlignType));
      if(lineBottom <= 0 || lineTop >= static_cast<int32_t>(bufferHeight))
      {
        glyphData.verticalOffset = lineBottom;
        continue;
      }
      CreateImageBufferForEachLine<true>(GetFontClient(), glyphData, hyphenIndex, line, (lineIndex == 0u),
                                         inputParamsForLine, inputParamsForGlyph);
    }
    else
    {
      CreateImageBufferForEachLine<false>(GetFontClient(), glyphData, hyphenIndex, line, (lineIndex == 0u),
                                          inputParamsForLine, inputParamsForGlyph);
    }
  }

  return glyphData.bitmapBuffer;
}

PixelBuffer Typesetter::Impl::CreateTextGradientMaskImageBuffer(const uint32_t      bufferWidth,
                                                                const uint32_t      bufferHeight,
                                                                const bool          ignoreHorizontalAlignment,
                                                                const Pixel::Format pixelFormat,
                                                                const int32_t       horizontalOffset,
                                                                const int32_t       verticalOffset,
                                                                const GlyphIndex    fromGlyphIndex,
                                                                const GlyphIndex    toGlyphIndex)
{
  auto& viewModel = *(mModel.get());

  const Length modelNumberOfLines                       = viewModel.GetNumberOfLines();
  const LineRun* const __restrict__ modelLinesBuffer    = viewModel.GetLines();
  const GlyphInfo* const __restrict__ glyphsBuffer      = viewModel.GetGlyphs();
  const Vector2* const __restrict__ positionBuffer      = viewModel.GetLayout();
  const ColorIndex* const __restrict__ colorIndexBuffer = viewModel.GetColorIndices();
  const GlyphInfo* __restrict__ hyphens                 = viewModel.GetHyphens();
  const Length* __restrict__ hyphenIndices              = viewModel.GetHyphenIndices();
  const Length hyphensCount                             = viewModel.GetHyphensCount();

  GlyphData glyphData;
  glyphData.verticalOffset   = verticalOffset;
  glyphData.width            = bufferWidth;
  glyphData.height           = bufferHeight;
  glyphData.bitmapBuffer     = CreateTransparentImageBuffer(bufferWidth, bufferHeight, pixelFormat);
  glyphData.horizontalOffset = 0;
  glyphData.revealContext    = mRevealRasterContext.get();

  Length hyphenIndex = 0;

  const Character* __restrict__ textBuffer                       = viewModel.GetTextBuffer();
  const Vector<CharacterIndex>& __restrict__ glyphToCharacterMap = viewModel.GetGlyphsToCharacters();
  const CharacterIndex* __restrict__ glyphToCharacterMapBuffer   = glyphToCharacterMap.Begin();

  const Vector<UnderlinedGlyphRun>    emptyUnderlineRuns;
  const Vector<StrikethroughGlyphRun> emptyStrikethroughRuns;

  const Vector<CharacterSpacingGlyphRun>& __restrict__ characterSpacingGlyphRuns =
    viewModel.GetCharacterSpacingGlyphRuns();

  const Vector2 styleOffset = Vector2::ZERO;

  const InputParameterForEachLine inputParamsForLine{bufferWidth,
                                                     bufferHeight,
                                                     horizontalOffset,

                                                     styleOffset,

                                                     fromGlyphIndex,
                                                     toGlyphIndex,

                                                     viewModel.GetStartIndexOfElidedGlyphs(),
                                                     viewModel.GetEndIndexOfElidedGlyphs(),
                                                     viewModel.GetFirstMiddleIndexOfElidedGlyphs(),
                                                     viewModel.GetSecondMiddleIndexOfElidedGlyphs(),
                                                     viewModel.GetElidedOffset(),
                                                     viewModel.GetVerticalLineAlignment(),
                                                     viewModel.GetEllipsisPosition(),

                                                     hyphens,
                                                     hyphenIndices,
                                                     hyphensCount,

                                                     ignoreHorizontalAlignment};

  const UnderlineStyleProperties modelUnderlineProperties{Text::Underline::Type::SOLID,
                                                          Vector4::ZERO,
                                                          0.0f,
                                                          0.0f,
                                                          0.0f,
                                                          false,
                                                          false,
                                                          false,
                                                          false,
                                                          false};

  const StrikethroughStyleProperties modelStrikethroughProperties{Vector4::ZERO, 0.0f, false, false};

  const Vector4 maskColor(1.0f, 1.0f, 1.0f, 1.0f);

  const InputParameterForEachGlyph inputParamsForGlyph{Typesetter::STYLE_NONE,
                                                       pixelFormat,

                                                       0.0f,

                                                       viewModel.GetCharacterSpacing(),

                                                       maskColor,

                                                       emptyUnderlineRuns,
                                                       emptyStrikethroughRuns,
                                                       characterSpacingGlyphRuns,

                                                       glyphsBuffer,
                                                       textBuffer,
                                                       glyphToCharacterMapBuffer,
                                                       viewModel.GetFinalGlyphStyleSourceIndices(),

                                                       positionBuffer,

                                                       nullptr,
                                                       nullptr,

                                                       modelUnderlineProperties,
                                                       modelStrikethroughProperties,

                                                       false,
                                                       false,
                                                       false,

                                                       viewModel.IsRemoveFrontInset(),
                                                       viewModel.IsRemoveBackInset(),

                                                       true};

  for(LineIndex lineIndex = 0u; lineIndex < modelNumberOfLines; ++lineIndex)
  {
    const LineRun& line = *(modelLinesBuffer + lineIndex);
    CreateTextGradientMaskImageBufferForEachLine(GetFontClient(), glyphData, hyphenIndex, line, inputParamsForLine,
                                                 inputParamsForGlyph, colorIndexBuffer, true);
  }

  return glyphData.bitmapBuffer;
}

PixelBuffer Typesetter::Impl::CreateTextGradientPreservedImageBuffer(const uint32_t      bufferWidth,
                                                                     const uint32_t      bufferHeight,
                                                                     const bool          ignoreHorizontalAlignment,
                                                                     const Pixel::Format pixelFormat,
                                                                     const int32_t       horizontalOffset,
                                                                     const int32_t       verticalOffset,
                                                                     const GlyphIndex    fromGlyphIndex,
                                                                     const GlyphIndex    toGlyphIndex)
{
  auto& viewModel = *(mModel.get());

  const Length modelNumberOfLines                       = viewModel.GetNumberOfLines();
  const LineRun* const __restrict__ modelLinesBuffer    = viewModel.GetLines();
  const GlyphInfo* const __restrict__ glyphsBuffer      = viewModel.GetGlyphs();
  const Vector2* const __restrict__ positionBuffer      = viewModel.GetLayout();
  const Vector4* const __restrict__ colorsBuffer        = viewModel.GetColors();
  const ColorIndex* const __restrict__ colorIndexBuffer = viewModel.GetColorIndices();
  const GlyphInfo* __restrict__ hyphens                 = viewModel.GetHyphens();
  const Length* __restrict__ hyphenIndices              = viewModel.GetHyphenIndices();
  const Length hyphensCount                             = viewModel.GetHyphensCount();

  GlyphData glyphData;
  glyphData.verticalOffset   = verticalOffset;
  glyphData.width            = bufferWidth;
  glyphData.height           = bufferHeight;
  glyphData.bitmapBuffer     = CreateTransparentImageBuffer(bufferWidth, bufferHeight, pixelFormat);
  glyphData.horizontalOffset = 0;
  glyphData.revealContext    = mRevealRasterContext.get();

  Length hyphenIndex = 0;

  const Character* __restrict__ textBuffer                       = viewModel.GetTextBuffer();
  const Vector<CharacterIndex>& __restrict__ glyphToCharacterMap = viewModel.GetGlyphsToCharacters();
  const CharacterIndex* __restrict__ glyphToCharacterMapBuffer   = glyphToCharacterMap.Begin();

  const Vector<UnderlinedGlyphRun>    emptyUnderlineRuns;
  const Vector<StrikethroughGlyphRun> emptyStrikethroughRuns;

  const Vector<CharacterSpacingGlyphRun>& __restrict__ characterSpacingGlyphRuns =
    viewModel.GetCharacterSpacingGlyphRuns();

  const Vector2 styleOffset = Vector2::ZERO;

  const InputParameterForEachLine inputParamsForLine{bufferWidth,
                                                     bufferHeight,
                                                     horizontalOffset,

                                                     styleOffset,

                                                     fromGlyphIndex,
                                                     toGlyphIndex,

                                                     viewModel.GetStartIndexOfElidedGlyphs(),
                                                     viewModel.GetEndIndexOfElidedGlyphs(),
                                                     viewModel.GetFirstMiddleIndexOfElidedGlyphs(),
                                                     viewModel.GetSecondMiddleIndexOfElidedGlyphs(),
                                                     viewModel.GetElidedOffset(),
                                                     viewModel.GetVerticalLineAlignment(),
                                                     viewModel.GetEllipsisPosition(),

                                                     hyphens,
                                                     hyphenIndices,
                                                     hyphensCount,

                                                     ignoreHorizontalAlignment};

  const UnderlineStyleProperties modelUnderlineProperties{Text::Underline::Type::SOLID,
                                                          Vector4::ZERO,
                                                          0.0f,
                                                          0.0f,
                                                          0.0f,
                                                          false,
                                                          false,
                                                          false,
                                                          false,
                                                          false};

  const StrikethroughStyleProperties modelStrikethroughProperties{Vector4::ZERO, 0.0f, false, false};

  const InputParameterForEachGlyph inputParamsForGlyph{Typesetter::STYLE_NONE,
                                                       pixelFormat,

                                                       0.0f,

                                                       viewModel.GetCharacterSpacing(),

                                                       viewModel.GetDefaultColor(),

                                                       emptyUnderlineRuns,
                                                       emptyStrikethroughRuns,
                                                       characterSpacingGlyphRuns,

                                                       glyphsBuffer,
                                                       textBuffer,
                                                       glyphToCharacterMapBuffer,
                                                       viewModel.GetFinalGlyphStyleSourceIndices(),

                                                       positionBuffer,

                                                       colorsBuffer,
                                                       colorIndexBuffer,

                                                       modelUnderlineProperties,
                                                       modelStrikethroughProperties,

                                                       false,
                                                       false,
                                                       false,

                                                       viewModel.IsRemoveFrontInset(),
                                                       viewModel.IsRemoveBackInset(),

                                                       nullptr == colorsBuffer || nullptr == colorIndexBuffer};

  for(LineIndex lineIndex = 0u; lineIndex < modelNumberOfLines; ++lineIndex)
  {
    const LineRun& line = *(modelLinesBuffer + lineIndex);
    CreateTextGradientMaskImageBufferForEachLine(GetFontClient(), glyphData, hyphenIndex, line, inputParamsForLine,
                                                 inputParamsForGlyph, colorIndexBuffer, false);
  }

  return glyphData.bitmapBuffer;
}

Typesetter::Impl::Impl(const ModelInterface* const model)
: mModel(std::make_unique<ViewModel>(model))
{
}

Typesetter::Impl::~Impl() = default;

void Typesetter::Impl::SetModel(const ModelInterface* model)
{
  mModel->SetModel(model);
}

void Typesetter::Impl::SetFinalElisionResult(const FinalElisionResult* result)
{
  mModel->SetFinalElisionResult(result);
}

} // namespace Text

} // namespace Ui

} // namespace Dali
