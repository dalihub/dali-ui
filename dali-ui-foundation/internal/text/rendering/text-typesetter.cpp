/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/adaptor-framework/pixel-buffer-operations.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/math/math-utils.h>
#include <memory.h>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <utility>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/glyph-metrics-helper.h>
#include <dali-ui-foundation/internal/text/line-helper-functions.h>
#include <dali-ui-foundation/internal/text/marquee/marquee-start-geometry.h>
#include <dali-ui-foundation/internal/text/rendering/styles/character-spacing-helper-functions.h>
#include <dali-ui-foundation/internal/text/rendering/styles/strikethrough-helper-functions.h>
#include <dali-ui-foundation/internal/text/rendering/styles/underline-helper-functions.h>
#include <dali-ui-foundation/internal/text/rendering/text-cutout-render-data.h>
#include <dali-ui-foundation/internal/text/rendering/text-raster-coordinate.h>
#include <dali-ui-foundation/internal/text/rendering/text-typesetter-impl.h>
#include <dali-ui-foundation/internal/text/rendering/text-typesetter.h>
#include <dali-ui-foundation/internal/text/rendering/view-model.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-run-snapshot.h>
#include <dali-ui-foundation/internal/text/styled-text/gradient-span-data.h>
#include <dali-ui-foundation/internal/text/text-trace.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Text
{
namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_TEXT_PERFORMANCE_MARKER, false);

// Validate once per render operation, before allocating or converting dimensions.
// Style/metadata passes use RGBA; plain text and masks may use L8.
bool ResolveRasterSize(const Vector2& size, uint32_t& width, uint32_t& height, uint32_t bytesPerPixel = 4u)
{
  int32_t x, y;
  if(!Raster::Convert(size.width, x) || !Raster::Convert(size.height, y) || x < 0 || y < 0 ||
     !Raster::BufferFits(static_cast<uint32_t>(x), static_cast<uint32_t>(y), bytesPerPixel))
  {
    return false;
  }
  width  = static_cast<uint32_t>(x);
  height = static_cast<uint32_t>(y);
  return true;
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

/**
 * @brief Combine the two RGBA image buffers together.
 *
 * The top layer buffer will blend over the bottom layer buffer:
 * - If the pixel is not fully opaque from either buffer, it will be blended with
 *   the pixel from the other buffer and copied to the combined buffer.
 * - If the pixels from both buffers are fully opaque, the pixels from the top layer
 *   buffer will be copied to the combined buffer.
 *
 * Due to the performance issue, We need to re-use input'ed pixelBuffer memory.
 * We can determine which pixelBuffer's memory is destination
 *
 * @param[in, out] topPixelBuffer The top layer buffer.
 * @param[in, out] bottomPixelBuffer The bottom layer buffer.
 * @param[in] bufferWidth The width of the image buffer.
 * @param[in] bufferHeight The height of the image buffer.
 * @param[in] storeResultIntoTop True if we store the combined image buffer result into topPixelBuffer.
 * False if we store the combined image buffer result into bottomPixelBuffer.
 *
 */
void CombineImageBuffer(PixelBuffer& __restrict__ topPixelBuffer,
                        PixelBuffer& __restrict__ bottomPixelBuffer, const uint32_t bufferWidth,
                        const uint32_t bufferHeight, bool storeResultIntoTop)
{
  // Assume that we always combine two RGBA images
  // Jump with 4bytes for optimize runtime.
  uint32_t* topBuffer    = reinterpret_cast<uint32_t*>(topPixelBuffer.GetBuffer());
  uint32_t* bottomBuffer = reinterpret_cast<uint32_t*>(bottomPixelBuffer.GetBuffer());

  if(topBuffer == nullptr && bottomBuffer == nullptr)
  {
    // Nothing to do if both buffers are empty.
    return;
  }

  if(topBuffer == nullptr)
  {
    // Nothing to do if topBuffer is empty.
    // If we need to store the result into top, change topPixelBuffer as bottomPixelBuffer.
    if(storeResultIntoTop)
    {
      topPixelBuffer = bottomPixelBuffer;
    }
    return;
  }

  if(bottomBuffer == nullptr)
  {
    // Nothing to do if bottomBuffer is empty.
    // If we need to store the result into bottom, change bottomPixelBuffer as topPixelBuffer.
    if(!storeResultIntoTop)
    {
      bottomPixelBuffer = topPixelBuffer;
    }
    return;
  }

  const uint32_t bufferSizeInt = bufferWidth * bufferHeight;

  uint32_t* __restrict__ combinedBuffer       = storeResultIntoTop ? topBuffer : bottomBuffer;
  uint8_t* __restrict__ topAlphaBufferPointer = reinterpret_cast<uint8_t*>(topBuffer) + 3;

  for(uint32_t pixelIndex = 0; pixelIndex < bufferSizeInt; ++pixelIndex)
  {
    // If the alpha of the pixel in either buffer is not fully opaque, blend the two pixels.
    // Otherwise, copy pixel from topBuffer to combinedBuffer.
    // Note : Be careful when we read & write into combinedBuffer. It can be write into same pointer.

    uint8_t topAlpha = *topAlphaBufferPointer;

    if(topAlpha == 0)
    {
      // Copy the pixel from bottomBuffer to combinedBuffer
      if(storeResultIntoTop)
      {
        *(combinedBuffer) = *(bottomBuffer);
      }
    }
    else if(topAlpha == 255)
    {
      // Copy the pixel from topBuffer to combinedBuffer
      if(!storeResultIntoTop)
      {
        *(combinedBuffer) = *(topBuffer);
      }
    }
    else
    {
      // At least one pixel is not fully opaque
      // "Over" blend the the pixel from topBuffer with the pixel in bottomBuffer
      uint32_t blendedBottomBufferColor                    = *(bottomBuffer);
      uint8_t* __restrict__ blendedBottomBufferColorBuffer = reinterpret_cast<uint8_t*>(&blendedBottomBufferColor);

      blendedBottomBufferColorBuffer[0] = MultiplyAndNormalizeColor(blendedBottomBufferColorBuffer[0], 255 - topAlpha);
      blendedBottomBufferColorBuffer[1] = MultiplyAndNormalizeColor(blendedBottomBufferColorBuffer[1], 255 - topAlpha);
      blendedBottomBufferColorBuffer[2] = MultiplyAndNormalizeColor(blendedBottomBufferColorBuffer[2], 255 - topAlpha);
      blendedBottomBufferColorBuffer[3] = MultiplyAndNormalizeColor(blendedBottomBufferColorBuffer[3], 255 - topAlpha);

      *(combinedBuffer) = *(topBuffer) + blendedBottomBufferColor;
    }

    // Increase each buffer's pointer.
    ++combinedBuffer;
    ++topBuffer;
    ++bottomBuffer;
    topAlphaBufferPointer += sizeof(uint32_t) / sizeof(uint8_t);
  }
}

} // namespace

TypesetterPtr Typesetter::New(const ModelInterface* const model)
{
  return TypesetterPtr(new Typesetter(model));
}

ViewModel* Typesetter::GetViewModel()
{
  return mImpl->GetViewModel();
}

void Typesetter::SetModel(const ModelInterface* model)
{
  mImpl->SetModel(model);
}

void Typesetter::SetFinalElisionResult(const FinalElisionResult* result)
{
  mImpl->SetFinalElisionResult(result);
}

void Typesetter::SetFontClient(TextAbstraction::FontClient& fontClient)
{
  mImpl->SetFontClient(fontClient);
}

TextAbstraction::FontClient Typesetter::GetFontClient()
{
  return mImpl->GetFontClient();
}

PixelData Typesetter::Render(const Vector2& size, Direction textDirection,
                             RenderBehaviour behaviour, bool ignoreHorizontalAlignment, Pixel::Format pixelFormat,
                             const Vector2&               originSize,
                             const GradientRasterContext* gradientContext)
{
  PixelBuffer result =
    RenderWithPixelBuffer(size, textDirection, behaviour, ignoreHorizontalAlignment, pixelFormat, originSize, gradientContext);
  PixelData pixelData = result ? PixelBuffer::Convert(result) : PixelData{};

  return pixelData;
}

MarqueeTextureAnchor Typesetter::ResolveMarqueeTextureAnchor(const MarqueeStartAnchor& anchor) const
{
  if(!anchor.valid)
  {
    return {};
  }

  // The caller resolves immediately after the primary marquee Render(). The
  // ViewModel therefore describes the glyphs and positions actually used to
  // generate that texture, not a source-layout estimate.
  const ViewModel&              viewModel          = *mImpl->GetViewModel();
  const Length                  numberOfGlyphs     = viewModel.GetNumberOfGlyphs();
  const Vector<CharacterIndex>& glyphsToCharacters = viewModel.GetGlyphsToCharacters();
  const GlyphInfo*              glyphs             = viewModel.GetGlyphs();
  const Vector2*                positions          = viewModel.GetLayout();
  if(numberOfGlyphs == 0u || glyphsToCharacters.Count() != numberOfGlyphs || !glyphs || !positions)
  {
    return {};
  }

  Length occurrence = 0u;
  for(GlyphIndex glyphIndex = 0u; glyphIndex < numberOfGlyphs; ++glyphIndex)
  {
    if(glyphsToCharacters[glyphIndex] != anchor.characterIndex)
    {
      continue;
    }

    if(occurrence++ != anchor.glyphOccurrence)
    {
      continue;
    }

    const GlyphInfo& glyph    = glyphs[glyphIndex];
    const float      textureX = positions[glyphIndex].x;
    if(IsSyntheticReplacementGlyph(glyph) ||
       glyph.width <= 0.01f || glyph.height <= 0.01f || !std::isfinite(textureX))
    {
      return {};
    }
    return {textureX, true};
  }

  return {};
}

PixelData Typesetter::RenderTextGradientMask(const Vector2& size, Direction textDirection,
                                             bool ignoreHorizontalAlignment, Pixel::Format pixelFormat,
                                             const Vector2& originSize)
{
  uint32_t bufferWidth, bufferHeight;
  if(!ResolveRasterSize(size, bufferWidth, bufferHeight, Pixel::GetBytesPerPixel(pixelFormat)))
  {
    return {};
  }

  DALI_TEXT_TRACE_SCOPE(gTraceFilter, "DALI_TEXT_RASTERIZE");

  auto& viewModel = *(mImpl->GetViewModel());

  viewModel.ElideGlyphs(mImpl->GetFontClient());

  const Size&   layoutSize   = viewModel.GetLayoutSize();
  const int32_t outlineWidth = static_cast<int32_t>(viewModel.GetOutlineWidth());

  int32_t penX = 0;
  switch(viewModel.GetHorizontalAlignment())
  {
    case Alignment::START:
    {
      break;
    }
    case Alignment::CENTER:
    {
      penX += (textDirection == Direction::LEFT_TO_RIGHT) ? -outlineWidth : outlineWidth;
      break;
    }
    case Alignment::END:
    {
      penX += (textDirection == Direction::LEFT_TO_RIGHT) ? -outlineWidth * 2 : outlineWidth * 2;
      break;
    }
  }

  const bool  isOriginHeight = originSize.height > 0.0f ? true : false;
  const float controlHeight  = isOriginHeight ? viewModel.GetControlSize().height : size.height;
  const float layoutHeight   = isOriginHeight ? originSize.height : layoutSize.height;

  int32_t penY = 0u;
  switch(viewModel.GetVerticalAlignment())
  {
    case Alignment::START:
    {
      break;
    }
    case Alignment::CENTER:
    {
      if(!Raster::Convert(std::round(0.5f * (controlHeight - layoutHeight)), penY))
      {
        return {};
      }
      break;
    }
    case Alignment::END:
    {
      if(!Raster::Convert(controlHeight - layoutHeight, penY))
      {
        return {};
      }
      break;
    }
  }

  if(viewModel.IsCutoutEnabled())
  {
    Vector2 offset = viewModel.GetOffsetWithCutout();
    if(!Raster::Convert(offset.x, penX) || !Raster::Convert(offset.y, penY))
    {
      return {};
    }
  }

  auto endIndexOfGlyphs = viewModel.GetEndIndexOfElidedGlyphs();

  PixelBuffer result =
    mImpl->CreateTextGradientMaskImageBuffer(bufferWidth, bufferHeight, ignoreHorizontalAlignment, pixelFormat, penX,
                                             penY, 0u, endIndexOfGlyphs);

  return result ? PixelBuffer::Convert(result) : PixelData{};
}

PixelData Typesetter::RenderTextGradientPreserved(const Vector2& size, Direction textDirection,
                                                  bool ignoreHorizontalAlignment, Pixel::Format pixelFormat,
                                                  const Vector2&               originSize,
                                                  const GradientRasterContext* gradientContext)
{
  uint32_t bufferWidth, bufferHeight;
  if(!ResolveRasterSize(size, bufferWidth, bufferHeight, Pixel::GetBytesPerPixel(pixelFormat)))
  {
    return {};
  }

  DALI_TEXT_TRACE_SCOPE(gTraceFilter, "DALI_TEXT_RASTERIZE");

  auto& viewModel = *(mImpl->GetViewModel());

  viewModel.ElideGlyphs(mImpl->GetFontClient());

  const Size&   layoutSize   = viewModel.GetLayoutSize();
  const int32_t outlineWidth = static_cast<int32_t>(viewModel.GetOutlineWidth());

  int32_t penX = 0;
  switch(viewModel.GetHorizontalAlignment())
  {
    case Alignment::START:
    {
      break;
    }
    case Alignment::CENTER:
    {
      penX += (textDirection == Direction::LEFT_TO_RIGHT) ? -outlineWidth : outlineWidth;
      break;
    }
    case Alignment::END:
    {
      penX += (textDirection == Direction::LEFT_TO_RIGHT) ? -outlineWidth * 2 : outlineWidth * 2;
      break;
    }
  }

  const bool  isOriginHeight = originSize.height > 0.0f ? true : false;
  const float controlHeight  = isOriginHeight ? viewModel.GetControlSize().height : size.height;
  const float layoutHeight   = isOriginHeight ? originSize.height : layoutSize.height;

  int32_t penY = 0u;
  switch(viewModel.GetVerticalAlignment())
  {
    case Alignment::START:
    {
      break;
    }
    case Alignment::CENTER:
    {
      if(!Raster::Convert(std::round(0.5f * (controlHeight - layoutHeight)), penY))
      {
        return {};
      }
      break;
    }
    case Alignment::END:
    {
      if(!Raster::Convert(controlHeight - layoutHeight, penY))
      {
        return {};
      }
      break;
    }
  }

  if(viewModel.IsCutoutEnabled())
  {
    Vector2 offset = viewModel.GetOffsetWithCutout();
    if(!Raster::Convert(offset.x, penX) || !Raster::Convert(offset.y, penY))
    {
      return {};
    }
  }

  auto endIndexOfGlyphs = viewModel.GetEndIndexOfElidedGlyphs();

  PixelBuffer result =
    mImpl->CreateTextGradientPreservedImageBuffer(bufferWidth, bufferHeight, ignoreHorizontalAlignment, pixelFormat,
                                                  penX, penY, 0u, endIndexOfGlyphs, gradientContext);

  return result ? PixelBuffer::Convert(result) : PixelData{};
}

PixelBuffer Typesetter::RenderWithPixelBuffer(const Vector2&  size,
                                              Direction       textDirection,
                                              RenderBehaviour behaviour, bool ignoreHorizontalAlignment,
                                              Pixel::Format pixelFormat, const Vector2& originSize,
                                              const GradientRasterContext* gradientContext,
                                              const Vector2&               rasterOrigin)
{
  uint32_t       bufferWidth, bufferHeight;
  const uint32_t bytesPerPixel = (behaviour == RENDER_NO_STYLES || behaviour == RENDER_MASK)
                                   ? Pixel::GetBytesPerPixel(pixelFormat)
                                   : 4u;
  if(!ResolveRasterSize(size, bufferWidth, bufferHeight, bytesPerPixel))
  {
    return {};
  }

  // @todo. This initial implementation for a TextLabel has only one visible page.

  DALI_TEXT_TRACE_SCOPE(gTraceFilter, "DALI_TEXT_RASTERIZE");

  // Use l-value to make ensure it is not nullptr, so compiler happy.
  auto& viewModel = *(mImpl->GetViewModel());

  // Elides the text if needed.
  viewModel.ElideGlyphs(mImpl->GetFontClient());

  // Retrieves the layout size.
  const Size&   layoutSize   = viewModel.GetLayoutSize();
  const int32_t outlineWidth = viewModel.IsOutlineEnabled() ? static_cast<int32_t>(viewModel.GetOutlineWidth()) : 0;

  // Set the offset for the horizontal alignment according to the text direction and outline width.
  int32_t penX = 0;
  switch(viewModel.GetHorizontalAlignment())
  {
    case Alignment::START:
    {
      // No offset to add.
      break;
    }
    case Alignment::CENTER:
    {
      penX += (textDirection == Direction::LEFT_TO_RIGHT) ? -outlineWidth : outlineWidth;
      break;
    }
    case Alignment::END:
    {
      penX += (textDirection == Direction::LEFT_TO_RIGHT) ? -outlineWidth * 2 : outlineWidth * 2;
      break;
    }
  }

  // Consider the case where pen y needs to be calculated based on the original size when rendering marquee texture.
  bool  isOriginHeight = originSize.height > 0.0f ? true : false;
  float controlHeight  = isOriginHeight ? viewModel.GetControlSize().height : size.height;
  float layoutHeight   = isOriginHeight ? originSize.height : layoutSize.height;

  // Set the offset for the vertical alignment.
  int32_t penY = 0u;
  switch(viewModel.GetVerticalAlignment())
  {
    case Alignment::START:
    {
      // No offset to add.
      break;
    }
    case Alignment::CENTER:
    {
      if(!Raster::Convert(std::round(0.5f * (controlHeight - layoutHeight)), penY))
      {
        return {};
      }
      break;
    }
    case Alignment::END:
    {
      if(!Raster::Convert(controlHeight - layoutHeight, penY))
      {
        return {};
      }
      break;
    }
  }

  const bool isCutoutEnabled = viewModel.IsCutoutEnabled();
  if(isCutoutEnabled)
  {
    Vector2 offset = viewModel.GetOffsetWithCutout();
    if(!Raster::Convert(offset.x, penX) || !Raster::Convert(offset.y, penY) ||
       !Raster::AddCoordinate(penX, -rasterOrigin.x, penX) ||
       !Raster::AddCoordinate(penY, -rasterOrigin.y, penY))
    {
      return {};
    }
  }

  // Rasterize text and decorations. The owner background is composed by the cutout shader.

  const uint32_t bufferSizeInt  = bufferWidth * bufferHeight;
  const size_t   bufferSizeChar = sizeof(uint32_t) * static_cast<std::size_t>(bufferSizeInt);

  // PixelBuffer keeps the requested empty geometry, but has no backing bytes.
  // Avoid passing its null buffer to memset/memcpy in the style passes.
  if(bufferSizeInt == 0u)
  {
    return PixelBuffer::New(bufferWidth, bufferHeight,
                            (behaviour == RENDER_NO_TEXT || behaviour == RENDER_OVERLAY_STYLE) ? Pixel::RGBA8888 : pixelFormat);
  }

  // END output keeps a zero-based final glyph buffer.
  auto endIndexOfGlyphs = viewModel.GetEndIndexOfElidedGlyphs();

  PixelBuffer imageBuffer;

  if(RENDER_MASK == behaviour)
  {
    // Generate the image buffer as an alpha mask for color glyphs.
    imageBuffer = mImpl->CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_MASK, ignoreHorizontalAlignment,
                                           pixelFormat, penX, penY, 0u, endIndexOfGlyphs);
  }
  else if(RENDER_NO_TEXT == behaviour || RENDER_OVERLAY_STYLE == behaviour)
  {
    // Generate an empty image buffer so that it can been combined with the image buffers for styles
    imageBuffer = PixelBuffer::New(bufferWidth, bufferHeight, Pixel::RGBA8888);
    memset(imageBuffer.GetBuffer(), 0u, bufferSizeChar);
  }
  else
  {
    // Generate the image buffer for the text with no style.
    imageBuffer = mImpl->CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_NONE, ignoreHorizontalAlignment,
                                           pixelFormat, penX, penY, 0u, endIndexOfGlyphs, 0u, 0u, gradientContext);
  }

  if((RENDER_NO_STYLES != behaviour) && (RENDER_MASK != behaviour))
  {
    // Generate the outline if enabled
    const float outlineAlpha = viewModel.GetOutlineColor().a;
    if(viewModel.IsOutlineEnabled() && fabsf(outlineAlpha) > Math::MACHINE_EPSILON_1 && RENDER_OVERLAY_STYLE != behaviour)
    {
      // Create the image buffer for outline
      PixelBuffer outlineImageBuffer =
        mImpl->CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_OUTLINE, ignoreHorizontalAlignment,
                                 pixelFormat, penX, penY, 0u, endIndexOfGlyphs);

      const float& blurRadius = viewModel.GetOutlineBlurRadius();

      if(blurRadius > Math::MACHINE_EPSILON_1)
      {
        PixelBufferOperations::ApplyGaussianBlur(outlineImageBuffer, blurRadius);
      }

      // Combine the two buffers
      CombineImageBuffer(imageBuffer, outlineImageBuffer, bufferWidth, bufferHeight, true);
    }

    // @todo. Support shadow for partial text later on.

    // Generate the shadow if enabled
    const float shadowAlpha = viewModel.GetShadowColor().a;
    if(RENDER_OVERLAY_STYLE != behaviour && viewModel.IsShadowEnabled() &&
       fabsf(shadowAlpha) > Math::MACHINE_EPSILON_1)
    {
      // Create the image buffer for shadow
      PixelBuffer shadowImageBuffer =
        mImpl->CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_SHADOW, ignoreHorizontalAlignment,
                                 pixelFormat, penX, penY, 0u, endIndexOfGlyphs);

      // Check whether it will be a soft shadow
      const float& blurRadius = viewModel.GetShadowBlurRadius();

      if(blurRadius > Math::MACHINE_EPSILON_1)
      {
        PixelBufferOperations::ApplyGaussianBlur(shadowImageBuffer, blurRadius);
      }

      // Combine the two buffers
      CombineImageBuffer(imageBuffer, shadowImageBuffer, bufferWidth, bufferHeight, true);
    }

    // Generate the background if enabled
    const bool backgroundEnabled   = viewModel.IsBackgroundEnabled();
    const bool backgroundMarkupSet = viewModel.IsMarkupBackgroundColorSet();
    if((backgroundEnabled || backgroundMarkupSet) && RENDER_OVERLAY_STYLE != behaviour)
    {
      PixelBuffer backgroundImageBuffer;

      if(backgroundEnabled)
      {
        backgroundImageBuffer =
          mImpl->CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_BACKGROUND, ignoreHorizontalAlignment,
                                   pixelFormat, penX, penY, 0u, endIndexOfGlyphs);
      }
      else
      {
        backgroundImageBuffer = Impl::CreateTransparentImageBuffer(bufferWidth, bufferHeight, pixelFormat);
      }

      if(backgroundMarkupSet)
      {
        mImpl->DrawGlyphsBackground(backgroundImageBuffer, bufferWidth, bufferHeight, ignoreHorizontalAlignment, penX,
                                    penY);
      }

      // Combine the two buffers
      CombineImageBuffer(imageBuffer, backgroundImageBuffer, bufferWidth, bufferHeight, true);
    }

    if(RENDER_OVERLAY_STYLE == behaviour)
    {
      if(viewModel.IsUnderlineEnabled())
      {
        // Create the image buffer for underline
        PixelBuffer underlineImageBuffer =
          mImpl->CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_UNDERLINE, ignoreHorizontalAlignment,
                                   pixelFormat, penX, penY, 0u, endIndexOfGlyphs);

        // Combine the two buffers
        CombineImageBuffer(imageBuffer, underlineImageBuffer, bufferWidth, bufferHeight, true);
      }

      if(viewModel.IsStrikethroughEnabled())
      {
        // Create the image buffer for strikethrough
        PixelBuffer strikethroughImageBuffer =
          mImpl->CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_STRIKETHROUGH,
                                   ignoreHorizontalAlignment, pixelFormat, penX, penY, 0u, endIndexOfGlyphs);

        // Combine the two buffers
        CombineImageBuffer(imageBuffer, strikethroughImageBuffer, bufferWidth, bufferHeight, true);
      }

      // Range underline/strikethrough runs from markup or StyledText spans.
      // The helper names keep the legacy markup terminology.
      if(viewModel.IsMarkupUnderlineSet())
      {
        imageBuffer = ApplyUnderlineMarkupImageBuffer(imageBuffer, bufferWidth, bufferHeight,
                                                      ignoreHorizontalAlignment, pixelFormat, penX, penY);
      }

      if(viewModel.IsMarkupStrikethroughSet())
      {
        imageBuffer = ApplyStrikethroughMarkupImageBuffer(imageBuffer, bufferWidth, bufferHeight,
                                                          ignoreHorizontalAlignment, pixelFormat, penX, penY);
      }
    }
  }

  return imageBuffer;
}

bool Typesetter::RenderCutoutPlanes(const Vector2& size, CutoutRenderData& planes,
                                    Pixel::Format glyphFormat, uint32_t maxTextureSize)
{
  int32_t widthValue, heightValue;
  if((glyphFormat != Pixel::L8 && glyphFormat != Pixel::RGBA8888) ||
     !Raster::Convert(size.width, widthValue) || !Raster::Convert(size.height, heightValue) ||
     widthValue <= 0 || heightValue <= 0)
  {
    return false;
  }
  const uint32_t controlWidth  = static_cast<uint32_t>(widthValue);
  const uint32_t controlHeight = static_cast<uint32_t>(heightValue);
  auto&          model         = *mImpl->GetViewModel();
  auto&          fontClient    = mImpl->GetFontClient();
  model.ElideGlyphs(fontClient);
  // Unsupported paints must already have selected ordinary rendering.
  const auto* gradientSpans = model.GetGradientSpanModelData();
  if(!model.IsCutoutEnabled() || (gradientSpans && !gradientSpans->characterRuns.Empty()))
  {
    return false;
  }

  int32_t    penX, penY;
  const auto offset = model.GetOffsetWithCutout();
  if(!Raster::Convert(offset.x, penX) || !Raster::Convert(offset.y, penY))
  {
    return false;
  }
  int32_t    baseline = penY;
  int64_t    left     = controlWidth;
  int64_t    top      = controlHeight;
  int64_t    right    = 0;
  int64_t    bottom   = 0;
  const auto end      = model.GetEndIndexOfElidedGlyphs();
  // Elision is complete; bitmap lookup does not mutate the final glyph model.
  const auto  lineCount = model.GetNumberOfLines();
  const auto* lines     = model.GetLines();
  const auto* glyphs    = model.GetGlyphs();
  const auto* positions = model.GetLayout();
  for(LineIndex lineIndex = 0; lineIndex < lineCount; ++lineIndex)
  {
    const auto& line = lines[lineIndex];
    int32_t     alignment;
    if(!Raster::AddCoordinate(baseline, line.ascender + GetPreOffsetVerticalLineAlignment(line, model.GetVerticalLineAlignment()), baseline) ||
       !Raster::Convert(line.ellipsis ? model.GetElidedOffset() : line.alignmentOffset, alignment))
    {
      return false;
    }
    // Background runs, decorations and inserted hyphens can extend beyond ink.
    if(model.GetHyphensCount() || model.IsBackgroundEnabled() || model.IsMarkupBackgroundColorSet() ||
       model.IsUnderlineEnabled() || model.IsStrikethroughEnabled() ||
       model.IsMarkupUnderlineSet() || model.IsMarkupStrikethroughSet())
    {
      int32_t lineWidth, lineTop, lineBottom;
      if(!Raster::Convert(std::ceil(line.width), lineWidth) ||
         !Raster::Convert(static_cast<float>(std::floor(static_cast<double>(baseline) - line.ascender)), lineTop) ||
         !Raster::Convert(static_cast<float>(std::ceil(static_cast<double>(baseline) - line.descender)), lineBottom))
      {
        return false;
      }
      const int64_t px = static_cast<int64_t>(penX) + alignment;
      left             = std::min(left, px);
      right            = std::max(right, px + lineWidth);
      top              = std::min(top, static_cast<int64_t>(lineTop));
      bottom           = std::max(bottom, static_cast<int64_t>(lineBottom));
    }
    for(GlyphIndex index = line.glyphRun.glyphIndex;
        index < line.glyphRun.glyphIndex + line.glyphRun.numberOfGlyphs && index <= end; ++index)
    {
      const auto& glyph = glyphs[index];
      if(IsSyntheticReplacementGlyph(glyph))
      {
        return false;
      }
      TextAbstraction::GlyphBufferData bitmap;
      int32_t                          glyphWidth, glyphHeight;
      if(!Raster::Convert(glyph.width, glyphWidth) || !Raster::Convert(glyph.height, glyphHeight) ||
         glyphWidth < 0 || glyphHeight < 0 ||
         !Raster::GlyphBufferFits(static_cast<uint32_t>(glyphWidth), static_cast<uint32_t>(glyphHeight), 4u))
      {
        return false;
      }
      bitmap.width  = static_cast<uint32_t>(glyphWidth);
      bitmap.height = static_cast<uint32_t>(glyphHeight);
      fontClient.CreateBitmap(glyph.fontId, glyph.index, glyph.isItalicRequired, glyph.isBoldRequired, bitmap, 0);
      if(bitmap.buffer && bitmap.width && bitmap.height)
      {
        int32_t x, y;
        if(!Raster::Convert(positions[index].x, x) || !Raster::Convert(positions[index].y, y))
        {
          return false;
        }
        const int64_t px = static_cast<int64_t>(penX) + alignment + x;
        const int64_t py = static_cast<int64_t>(baseline) + y;
        left             = std::min(left, px);
        top              = std::min(top, py);
        right            = std::max(right, px + bitmap.width);
        bottom           = std::max(bottom, py + bitmap.height);
      }
    }
    if(!Raster::AddCoordinate(baseline, -line.descender + GetPostOffsetVerticalLineAlignment(line, model.GetVerticalLineAlignment()), baseline))
    {
      return false;
    }
  }
  // Include decoration extents, blur support and a transparent filtering guard.
  float decorationHeight = std::max(model.GetUnderlineHeight(), model.GetStrikethroughHeight());
  for(UnderlineRunIndex i = 0; i < model.GetNumberOfUnderlineRuns(); ++i)
  {
    UnderlinedGlyphRun run;
    model.GetUnderlineRuns(&run, i, 1u);
    decorationHeight = std::max(decorationHeight, run.properties.height);
  }
  for(StrikethroughRunIndex i = 0; i < model.GetNumberOfStrikethroughRuns(); ++i)
  {
    StrikethroughGlyphRun run;
    model.GetStrikethroughRuns(&run, i, 1u);
    decorationHeight = std::max(decorationHeight, run.properties.height);
  }
  const auto  shadow      = model.GetShadowOffset();
  const auto  outline     = model.GetOutlineOffset();
  const float marginValue = 2.f + 3.f * std::max(0.f, decorationHeight) + (model.IsOutlineEnabled() ? 2.f * model.GetOutlineWidth() : 0.f) +
                            (model.IsShadowEnabled() ? std::max(std::abs(shadow.x), std::abs(shadow.y)) + 3.f * model.GetShadowBlurRadius() : 0.f) +
                            (model.IsOutlineEnabled() ? std::max(std::abs(outline.x), std::abs(outline.y)) + 3.f * model.GetOutlineBlurRadius() : 0.f);
  int32_t margin;
  if(!Raster::Convert(std::ceil(marginValue), margin) || margin < 0)
  {
    return false;
  }
  // Keep ink/decoration arithmetic signed and wide until it is clipped. Only
  // coordinates inside the validated control extent become buffer indices.
  const auto clipToControl = [](int64_t coordinate, uint32_t extent)
  {
    return static_cast<uint32_t>(std::clamp<int64_t>(coordinate, 0, extent));
  };
  uint32_t cropLeft   = clipToControl(left - margin, controlWidth);
  uint32_t cropTop    = clipToControl(top - margin, controlHeight);
  uint32_t cropRight  = clipToControl(right + margin, controlWidth);
  uint32_t cropBottom = clipToControl(bottom + margin, controlHeight);
  if(right <= left || bottom <= top || cropRight <= cropLeft || cropBottom <= cropTop)
  {
    cropLeft = cropTop = 0u;
    cropRight = cropBottom = 1u;
  }
  const uint32_t width  = cropRight - cropLeft;
  const uint32_t height = cropBottom - cropTop;
  if(!Raster::BufferFits(width, height, 4u) ||
     (maxTextureSize && (width > maxTextureSize || height > maxTextureSize)))
  {
    return false;
  }
  int32_t x, y;
  if(!Raster::Add(penX, -static_cast<int64_t>(cropLeft), x) ||
     !Raster::Add(penY, -static_cast<int64_t>(cropTop), y))
  {
    return false;
  }
  // The L8 rasterizer preserves the RGBA path's maximum-alpha overlap rule
  // without an intermediate RGBA buffer.
  auto coverage = mImpl->CreateImageBuffer(width, height, STYLE_NONE, false, glyphFormat, x, y, 0u, end);
  if(!coverage)
  {
    return false;
  }
  const bool hasStyle = model.IsOutlineEnabled() || model.IsShadowEnabled() ||
                        model.IsBackgroundEnabled() || model.IsMarkupBackgroundColorSet();
  const Vector2 rasterSize(static_cast<float>(width), static_cast<float>(height));
  const Vector2 origin(static_cast<float>(cropLeft), static_cast<float>(cropTop));
  auto          style = hasStyle ? RenderWithPixelBuffer(rasterSize, Direction::LEFT_TO_RIGHT, RENDER_NO_TEXT,
                                                         false, Pixel::RGBA8888, Size::ZERO, nullptr, origin)
                                 : Impl::CreateTransparentImageBuffer(1u, 1u, Pixel::RGBA8888);
  PixelBuffer   overlay;
  const bool    hasOverlay = model.IsUnderlineEnabled() || model.IsStrikethroughEnabled() ||
                          model.IsMarkupUnderlineSet() || model.IsMarkupStrikethroughSet();
  if(hasOverlay)
  {
    overlay = RenderWithPixelBuffer(rasterSize, Direction::LEFT_TO_RIGHT, RENDER_OVERLAY_STYLE,
                                    false, Pixel::RGBA8888, Size::ZERO, nullptr, origin);
  }
  if(!style || (hasOverlay && !overlay))
  {
    return false;
  }
  CutoutRenderData result;
  result.controlSize = size;
  result.rect        = Vector4(origin.x, origin.y, rasterSize.x, rasterSize.y);
  result.coverage    = PixelBuffer::Convert(coverage);
  result.style       = PixelBuffer::Convert(style);
  if(overlay)
  {
    result.overlay = PixelBuffer::Convert(overlay);
  }
  if(!result.IsValid())
  {
    return false;
  }
  planes = std::move(result);
  return true;
}

Internal::Reveal::Plan Typesetter::CreateFinalRevealPlan(const Internal::Reveal::Plan& sourcePlan,
                                                         Internal::Reveal::Unit        unit,
                                                         Internal::Reveal::Sequence    sequence,
                                                         float                         sequenceStaggerRatio)
{
  auto& viewModel = *mImpl->GetViewModel();
  viewModel.EnableFinalGlyphMapping();
  viewModel.ElideGlyphs(mImpl->GetFontClient());
  Internal::Reveal::Plan finalPlan = Internal::Reveal::ProjectToFinalGlyphs(sourcePlan,
                                                                            viewModel.GetNumberOfGlyphs(),
                                                                            viewModel.GetFinalToSourceGlyphIndices(),
                                                                            viewModel.GetEllipsisFinalGlyphIndex(),
                                                                            unit);
  if(unit == Internal::Reveal::Unit::LINE)
  {
    if(Internal::Reveal::ApplyLineUnitSchedule(finalPlan,
                                               viewModel.GetLines(),
                                               viewModel.GetNumberOfLines()) &&
       sequence == Internal::Reveal::Sequence::PER_LINE)
    {
      Internal::Reveal::ApplyPerLineSequenceSchedule(finalPlan,
                                                     viewModel.GetLines(),
                                                     viewModel.GetNumberOfLines(),
                                                     sequenceStaggerRatio);
    }
  }
  else if(unit == Internal::Reveal::Unit::PIXEL)
  {
    TextAbstraction::FontClient fontClient;
    if(finalPlan.fadeDurationRatio == Text::Reveal::AUTO_FADE_DURATION_RATIO)
    {
      fontClient = mImpl->GetFontClient();
    }
    if(!Internal::Reveal::ApplyPixelSpatialSchedule(finalPlan,
                                                    viewModel,
                                                    fontClient,
                                                    viewModel.GetFinalToSourceGlyphIndices(),
                                                    viewModel.GetEllipsisFinalGlyphIndex(),
                                                    sequence,
                                                    sequenceStaggerRatio))
    {
      // The scheduler commits only complete PIXEL timing, so a failure leaves
      // this projected CHARACTER-compatible plan intact.
    }
  }
  else if(sequence == Internal::Reveal::Sequence::PER_LINE)
  {
    Internal::Reveal::ApplyPerLineSequenceSchedule(finalPlan,
                                                   viewModel.GetLines(),
                                                   viewModel.GetNumberOfLines(),
                                                   sequenceStaggerRatio);
  }
  return finalPlan;
}

bool Typesetter::ExtractReplacementRevealTimings(const Internal::Reveal::Plan&       finalPlan,
                                                 const ReplacementSourceSnapshot&    source,
                                                 const Vector<ReplacementPlacement>& placements,
                                                 Vector<ReplacementRevealTiming>&    timings)
{
  timings.Clear();
  auto&             viewModel          = *mImpl->GetViewModel();
  const Length      finalGlyphCount    = viewModel.GetNumberOfGlyphs();
  const GlyphInfo*  finalGlyphs        = viewModel.GetGlyphs();
  const GlyphIndex* finalToSourceGlyph = viewModel.GetFinalToSourceGlyphIndices();
  if(finalPlan.glyphToUnit.size() != finalGlyphCount || !finalGlyphs)
  {
    return false;
  }

  std::unordered_map<GlyphIndex, GlyphIndex> finalGlyphBySourceSyntheticGlyph;
  finalGlyphBySourceSyntheticGlyph.reserve(placements.Count());
  for(GlyphIndex finalGlyph = 0u; finalGlyph < finalGlyphCount; ++finalGlyph)
  {
    if(IsSyntheticReplacementGlyph(finalGlyphs[finalGlyph]))
    {
      const GlyphIndex sourceGlyph = finalToSourceGlyph ? finalToSourceGlyph[finalGlyph] : finalGlyph;
      finalGlyphBySourceSyntheticGlyph.emplace(sourceGlyph, finalGlyph);
    }
  }

  uint32_t expectedTimingCount = 0u;
  for(const ReplacementPlacement& placement : placements)
  {
    if(!placement.visible || placement.elided || placement.sourceRunIndex >= source.runs.Count())
    {
      continue;
    }
    const ReplacementRunSnapshot& run = source.runs[placement.sourceRunIndex];
    if(run.type != ReplacementType::IMAGE || run.image.source.empty() ||
       run.occurrenceIdentity != placement.occurrenceIdentity)
    {
      continue;
    }
    ++expectedTimingCount;

    const auto matchingFinalGlyph = finalGlyphBySourceSyntheticGlyph.find(placement.syntheticGlyphIndex);
    if(matchingFinalGlyph == finalGlyphBySourceSyntheticGlyph.end())
    {
      timings.Clear();
      return false;
    }

    const uint32_t unit = finalPlan.glyphToUnit[matchingFinalGlyph->second];
    if(unit == Internal::Reveal::NO_UNIT || unit >= finalPlan.unitStart.size() ||
       unit >= finalPlan.imageReplacementUnitMask.size() || finalPlan.imageReplacementUnitMask[unit] == 0u)
    {
      timings.Clear();
      return false;
    }

    float start           = finalPlan.unitStart[unit];
    float progressionSpan = 0.0f;
    bool  rightToLeft     = false;
    if(unit < finalPlan.pixelUnitTiming.size())
    {
      progressionSpan = finalPlan.pixelUnitTiming[unit].progressionSpan;
      rightToLeft     = finalPlan.pixelUnitTiming[unit].rightToLeft;
      start += 0.5f * progressionSpan;
    }
    timings.PushBack({placement.occurrenceIdentity,
                      start,
                      finalPlan.fadeDuration,
                      progressionSpan,
                      rightToLeft});
  }
  return timings.Count() == expectedTimingCount;
}

PixelData Typesetter::RenderTextRevealMetadata(
  const Vector2&                tileSize,
  Direction                     textDirection,
  const Internal::Reveal::Plan& plan,
  float&                        fadeDuration,
  uint32_t                      tileOffsetY,
  const Vector2&                fullSize,
  bool                          ignoreHorizontalAlignment,
  const Vector2&                originSize)
{
  uint32_t width, height;
  if(!ResolveRasterSize(tileSize, width, height))
  {
    return {};
  }

  // Reuse the normal glyph traversal, but offset it into a tile-local target.
  // The caller projects one canonical final plan before visiting any tile.
  auto& viewModel = *mImpl->GetViewModel();

  const Size&   layoutSize   = viewModel.GetLayoutSize();
  const int32_t outlineWidth = viewModel.IsOutlineEnabled() ? static_cast<int32_t>(viewModel.GetOutlineWidth()) : 0;
  int32_t       penX         = 0;
  switch(viewModel.GetHorizontalAlignment())
  {
    case Alignment::START:
      break;
    case Alignment::CENTER:
      penX += textDirection == Direction::LEFT_TO_RIGHT ? -outlineWidth : outlineWidth;
      break;
    case Alignment::END:
      penX += textDirection == Direction::LEFT_TO_RIGHT ? -outlineWidth * 2 : outlineWidth * 2;
      break;
  }

  const Vector2 renderSize    = fullSize.height > 0.0f ? fullSize : tileSize;
  const bool    useOriginSize = originSize.height > 0.0f;
  const float   controlHeight = useOriginSize ? viewModel.GetControlSize().height : renderSize.height;
  const float   layoutHeight  = useOriginSize ? originSize.height : layoutSize.height;
  int32_t       penY          = 0;
  switch(viewModel.GetVerticalAlignment())
  {
    case Alignment::START:
      break;
    case Alignment::CENTER:
      if(!Raster::Convert(std::round(0.5f * (controlHeight - layoutHeight)), penY))
      {
        return {};
      }
      break;
    case Alignment::END:
      if(!Raster::Convert(controlHeight - layoutHeight, penY))
      {
        return {};
      }
      break;
  }
  if(!Raster::Add(penY, -static_cast<int64_t>(tileOffsetY), penY))
  {
    return {};
  }

  const auto endGlyph = viewModel.GetEndIndexOfElidedGlyphs();
  fadeDuration        = plan.fadeDuration;

  mImpl->BeginRevealMetadata(width, height, plan);
  mImpl->CreateImageBuffer(width, height, Typesetter::STYLE_NONE,
                           ignoreHorizontalAlignment, Pixel::RGBA8888,
                           penX, penY, 0u, endGlyph);
  return mImpl->EndRevealMetadata();
}

Rect<uint32_t> Typesetter::GetRuntimeBlurLineRasterBounds(const Vector2& size, LineIndex lineIndex, const int32_t* lineBaseline)
{
  const auto&          model  = *mImpl->GetViewModel();
  const auto           width  = static_cast<uint32_t>(size.width);
  const auto           height = static_cast<uint32_t>(size.height);
  const Rect<uint32_t> full(0u, 0u, width, height);
  if(lineIndex >= model.GetNumberOfLines() || model.GetGradientSpanModelData() ||
     model.GetHyphensCount() != 0u)
  {
    return full;
  }
  const auto& line = model.GetLines()[lineIndex];
  if(line.glyphRun.numberOfGlyphs == 0u)
  {
    return full;
  }

  int32_t baseline = lineBaseline ? *lineBaseline : 0;
  if(!lineBaseline)
  {
    if(model.GetVerticalAlignment() == Alignment::CENTER)
    {
      baseline = static_cast<int32_t>(std::round(0.5f * (size.height - model.GetLayoutSize().height)));
    }
    else if(model.GetVerticalAlignment() == Alignment::END)
    {
      baseline = static_cast<int32_t>(size.height - model.GetLayoutSize().height);
    }
    // Keep the separate per-line truncations used by CreateImageBufferForEachLine.
    // Summing fractional metrics first would move later lines by whole pixels.
    for(LineIndex index = 0u; index <= lineIndex; ++index)
    {
      const auto& current = model.GetLines()[index];
      baseline += static_cast<int32_t>(current.ascender + GetPreOffsetVerticalLineAlignment(current, model.GetVerticalLineAlignment()));
      if(index != lineIndex)
      {
        baseline += static_cast<int32_t>(-current.descender + GetPostOffsetVerticalLineAlignment(current, model.GetVerticalLineAlignment()));
      }
    }
  }

  const auto first  = line.glyphRun.glyphIndex;
  const auto last   = std::min(line.glyphRun.glyphIndex + line.glyphRun.numberOfGlyphs - 1u, model.GetEndIndexOfElidedGlyphs());
  int64_t    top    = height;
  int64_t    bottom = 0;
  for(GlyphIndex index = first; index <= last; ++index)
  {
    const auto& glyph = model.GetGlyphs()[index];
    if(IsSyntheticReplacementGlyph(glyph) || glyph.width < Math::MACHINE_EPSILON_1000 || glyph.height < Math::MACHINE_EPSILON_1000)
    {
      continue;
    }
    TextAbstraction::GlyphBufferData bitmap;
    bitmap.width  = static_cast<uint32_t>(glyph.width);
    bitmap.height = static_cast<uint32_t>(glyph.height);
    mImpl->GetFontClient().CreateBitmap(glyph.fontId, glyph.index, glyph.isItalicRequired, glyph.isBoldRequired, bitmap, 0);
    if(bitmap.buffer && bitmap.width > 0u && bitmap.height > 0u)
    {
      const int64_t y = static_cast<int64_t>(baseline) + static_cast<int32_t>(model.GetLayout()[index].y);
      top             = std::min(top, y);
      bottom          = std::max(bottom, y + bitmap.height);
    }
    // GlyphBufferData releases owned buffers; cached font bitmaps stay borrowed.
  }
  // Match the final source crop's filtering guard; Gaussian support is added
  // later to the GPU target and does not require a full-height CPU plane.
  top    = std::max<int64_t>(0, top - 2);
  bottom = std::min<int64_t>(height, bottom + 2);
  return bottom > top ? Rect<uint32_t>(0u, static_cast<uint32_t>(top), width, static_cast<uint32_t>(bottom - top)) : full;
}

PixelData Typesetter::RenderRuntimeBlurLine(const Vector2& size, LineIndex lineIndex,
                                            RuntimeBlurPlane plane, Pixel::Format format,
                                            const Internal::Reveal::Plan& plan,
                                            uint32_t rasterOffsetY, uint32_t rasterHeight,
                                            uint32_t rasterOffsetX, uint32_t rasterWidth,
                                            const GradientRasterContext* gradientContext)
{
  const auto& model = *mImpl->GetViewModel();
  if(lineIndex >= model.GetNumberOfLines())
  {
    return {};
  }
  const auto& line    = model.GetLines()[lineIndex];
  const auto& lastRun = line.glyphRun;
  if(lastRun.numberOfGlyphs == 0u)
  {
    return {};
  }
  const GlyphIndex first = line.glyphRun.glyphIndex;
  const GlyphIndex last  = lastRun.glyphIndex + lastRun.numberOfGlyphs - 1u;
  int32_t          penY  = 0;
  if(model.GetVerticalAlignment() == Alignment::CENTER)
  {
    penY = static_cast<int32_t>(std::round(0.5f * (size.height - model.GetLayoutSize().height)));
  }
  else if(model.GetVerticalAlignment() == Alignment::END)
  {
    penY = static_cast<int32_t>(size.height - model.GetLayoutSize().height);
  }
  const auto width  = static_cast<uint32_t>(size.width);
  const auto height = static_cast<uint32_t>(size.height);
  if(rasterHeight == 0u)
  {
    rasterHeight = height;
  }
  if(rasterWidth == 0u)
  {
    rasterWidth = width;
  }
  if(rasterOffsetY > height || rasterHeight > height - rasterOffsetY ||
     rasterOffsetX > width || rasterWidth > width - rasterOffsetX ||
     (plane != RuntimeBlurPlane::REVEAL_METADATA && (rasterOffsetX != 0u || rasterWidth != width)) ||
     (plane != RuntimeBlurPlane::REVEAL_METADATA && model.GetGradientSpanModelData() &&
      (rasterOffsetY != 0u || rasterHeight != height)))
  {
    return {};
  }
  penY -= static_cast<int32_t>(rasterOffsetY);
  PixelBuffer buffer;
  switch(plane)
  {
    case RuntimeBlurPlane::GRADIENT_PRESERVED:
      buffer = mImpl->CreateTextGradientPreservedImageBuffer(width, rasterHeight, false, format, 0, penY, first, last, gradientContext);
      break;
    case RuntimeBlurPlane::GRADIENT_MASK:
      buffer = mImpl->CreateTextGradientMaskImageBuffer(width, rasterHeight, false, format, 0, penY, first, last);
      break;
    case RuntimeBlurPlane::REVEAL_METADATA:
      mImpl->BeginRevealMetadata(rasterWidth, rasterHeight, plan);
      mImpl->CreateImageBuffer(rasterWidth, rasterHeight, STYLE_NONE, false, Pixel::RGBA8888,
                               -static_cast<int32_t>(rasterOffsetX), penY, first, last,
                               rasterOffsetY, height);
      return mImpl->EndRevealMetadata();
    default:
      buffer = mImpl->CreateImageBuffer(width, rasterHeight,
                                        plane == RuntimeBlurPlane::COLOR_MASK ? STYLE_MASK : STYLE_NONE,
                                        false, format, 0, penY, first, last, 0u, 0u, gradientContext);
      break;
  }
  return PixelBuffer::Convert(buffer);
}

PixelBuffer Typesetter::ApplyUnderlineMarkupImageBuffer(PixelBuffer    topPixelBuffer,
                                                        const uint32_t bufferWidth, const uint32_t bufferHeight,
                                                        const bool          ignoreHorizontalAlignment,
                                                        const Pixel::Format pixelFormat,
                                                        const int32_t       horizontalOffset,
                                                        const int32_t       verticalOffset)
{
  // Use l-value to make ensure it is not nullptr, so compiler happy.
  auto& viewModel = *(mImpl->GetViewModel());

  // Underline-tags (this is for Markup case)
  // Get the underline runs.
  const Length               numberOfUnderlineRuns = viewModel.GetNumberOfUnderlineRuns();
  Vector<UnderlinedGlyphRun> underlineRuns;
  underlineRuns.Resize(numberOfUnderlineRuns);
  viewModel.GetUnderlineRuns(underlineRuns.Begin(), 0u, numberOfUnderlineRuns);

  // Iterate on the consecutive underlined glyph run and connect them into one chunk of underlined characters.
  Vector<UnderlinedGlyphRun>::ConstIterator itGlyphRun    = underlineRuns.Begin();
  Vector<UnderlinedGlyphRun>::ConstIterator endItGlyphRun = underlineRuns.End();
  GlyphIndex                                startGlyphIndex, endGlyphIndex;

  // The outer loop to iterate on the separated chunks of underlined glyph runs
  while(itGlyphRun != endItGlyphRun)
  {
    startGlyphIndex = itGlyphRun->glyphRun.glyphIndex;
    endGlyphIndex   = startGlyphIndex + itGlyphRun->glyphRun.numberOfGlyphs - 1;

    // Create the image buffer for underline
    PixelBuffer underlineImageBuffer =
      mImpl->CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_UNDERLINE, ignoreHorizontalAlignment,
                               pixelFormat, horizontalOffset, verticalOffset, startGlyphIndex, endGlyphIndex);
    // Combine the two buffers
    // Result pixel buffer will be stored into topPixelBuffer.
    CombineImageBuffer(underlineImageBuffer, topPixelBuffer, bufferWidth, bufferHeight, false);

    itGlyphRun++;
  }

  return topPixelBuffer;
}

PixelBuffer Typesetter::ApplyStrikethroughMarkupImageBuffer(
  PixelBuffer topPixelBuffer, const uint32_t bufferWidth, const uint32_t bufferHeight,
  const bool ignoreHorizontalAlignment, const Pixel::Format pixelFormat, const int32_t horizontalOffset,
  const int32_t verticalOffset)
{
  // Use l-value to make ensure it is not nullptr, so compiler happy.
  auto& viewModel = *(mImpl->GetViewModel());

  // strikethrough-tags (this is for Markup case)
  // Get the strikethrough runs.
  const Length                  numberOfStrikethroughRuns = viewModel.GetNumberOfStrikethroughRuns();
  Vector<StrikethroughGlyphRun> strikethroughRuns;
  strikethroughRuns.Resize(numberOfStrikethroughRuns);
  viewModel.GetStrikethroughRuns(strikethroughRuns.Begin(), 0u, numberOfStrikethroughRuns);

  // Iterate on the consecutive strikethrough glyph run and connect them into one chunk of strikethrough characters.
  Vector<StrikethroughGlyphRun>::ConstIterator itGlyphRun    = strikethroughRuns.Begin();
  Vector<StrikethroughGlyphRun>::ConstIterator endItGlyphRun = strikethroughRuns.End();
  GlyphIndex                                   startGlyphIndex, endGlyphIndex;

  // The outer loop to iterate on the separated chunks of strikethrough glyph runs
  while(itGlyphRun != endItGlyphRun)
  {
    startGlyphIndex = itGlyphRun->glyphRun.glyphIndex;
    endGlyphIndex   = startGlyphIndex + itGlyphRun->glyphRun.numberOfGlyphs - 1;

    // Create the image buffer for strikethrough
    PixelBuffer strikethroughImageBuffer =
      mImpl->CreateImageBuffer(bufferWidth, bufferHeight, Typesetter::STYLE_STRIKETHROUGH, ignoreHorizontalAlignment,
                               pixelFormat, horizontalOffset, verticalOffset, startGlyphIndex, endGlyphIndex);
    // Combine the two buffers
    // Result pixel buffer will be stored into topPixelBuffer.
    CombineImageBuffer(strikethroughImageBuffer, topPixelBuffer, bufferWidth, bufferHeight, false);

    itGlyphRun++;
  }

  return topPixelBuffer;
}

Typesetter::Typesetter(const ModelInterface* const model)
: mImpl{std::make_unique<Impl>(model)}
{
}

Typesetter::~Typesetter() = default;

} // namespace Text

} // namespace Ui

} //namespace DALI_NAMESPACE
