#ifndef DALI_UI_TEXT_TYPESETTER_IMPL_H
#define DALI_UI_TEXT_TYPESETTER_IMPL_H

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
#include <dali/devel-api/text-abstraction/text-abstraction-definitions.h>
#include <dali/public-api/adaptor-framework/pixel-buffer.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/images/pixel.h>
#include <dali/public-api/object/ref-object.h>
#include <memory> ///< for std::unique_ptr

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/rendering/text-typesetter.h>
#include <dali-ui-foundation/internal/text/text-enumerations.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
class ModelInterface;
class ViewModel;
struct FinalElisionResult;
struct RevealRasterContext;

namespace Internal
{
namespace Reveal
{
/**
 * @brief Expands discrete Reveal ownership by one texel for LINEAR text filtering.
 *
 * Existing raster-owned texels remain unchanged. Empty destinations receive
 * ownership from their original 8-neighborhood only, keep zero coverage in
 * the alpha channel, and conservatively select the latest start timing when
 * different units meet.
 *
 * @param[in,out] metadata A tightly packed RGBA8888 Reveal metadata buffer.
 * @param[in] width The buffer width in pixels.
 * @param[in] height The buffer height in pixels.
 */
void ExpandMetadataOwnership(uint8_t* metadata, uint32_t width, uint32_t height);
} // namespace Reveal
} // namespace Internal

/**
 * @brief This class is seperated logics for TypeSetter.
 * It will reduce the complexicy of typesetter logic.
 */
struct Typesetter::Impl
{
public:
  /**
   * @brief Create an initialized image buffer filled with transparent color.
   *
   * Creates the pixel data used to generate the final image with the given size.
   *
   * @param[in] bufferWidth The width of the image buffer.
   * @param[in] bufferHeight The height of the image buffer.
   * @param[in] pixelFormat The format of the pixel in the image that the text is rendered as (i.e. either
   * Pixel::BGRA8888 or Pixel::L8).
   *
   * @return An image buffer.
   */
  static PixelBuffer CreateTransparentImageBuffer(const uint32_t bufferWidth, const uint32_t bufferHeight,
                                                  const Pixel::Format pixelFormat);

public: // Constructor & Destructor
  /**
   * @brief Creates a Typesetter impl instance.
   */
  Impl(const ModelInterface* const model);

  ~Impl();

public:
  /**
   * @brief Retrieves the pointer to the view model.
   *
   * @return A pointer to the view model.
   */
  ViewModel* GetViewModel();

  /**
   * @brief Sets the model used by subsequent render calls.
   *
   * @param[in] model Pointer to the text's data model.
   */
  void SetModel(const ModelInterface* model);

  /**
   * @brief Sets the authoritative resolved glyph sequence.
   *
   * @param[in] result The replacement or non-replacement END final result.
   */
  void SetFinalElisionResult(const FinalElisionResult* result);

  /**
   * @brief Set the font client.
   *
   * Set the font client used in the update/render process of the text model.
   *
   * @param[in] fontClient The font client used by the Typesetter.
   */
  void SetFontClient(TextAbstraction::FontClient& fontClient);

  /**
   * @brief Get the font client.
   *
   * @return The font client used by the Typesetter.
   */
  TextAbstraction::FontClient& GetFontClient();

  void BeginRevealMetadata(uint32_t width, uint32_t height, const Internal::Reveal::Plan& plan);

  PixelData EndRevealMetadata();

public: // Image buffer creation
  void DrawGlyphsBackground(PixelBuffer& buffer, const uint32_t bufferWidth, const uint32_t bufferHeight,
                            const bool ignoreHorizontalAlignment, const int32_t horizontalOffset,
                            const int32_t verticalOffset);

  /**
   * @brief Create & draw the image buffer for the given range of the glyphs in the given style.
   *
   * Does the following operations:
   * - Retrieves the data buffers from the text model.
   * - Creates the pixel data used to generate the final image with the given size.
   * - Traverse the visible glyphs, retrieve their bitmaps and compose the final pixel data.
   *
   * @param[in] bufferWidth The width of the image buffer.
   * @param[in] bufferHeight The height of the image buffer.
   * @param[in] style The style of the text.
   * @param[in] ignoreHorizontalAlignment Whether to ignore the horizontal alignment, not ignored by default.
   * @param[in] pixelFormat The format of the pixel in the image that the text is rendered as (i.e. either
   * Pixel::BGRA8888 or Pixel::L8).
   * @param[in] horizontalOffset The horizontal offset to be added to the glyph's position.
   * @param[in] verticalOffset The vertical offset to be added to the glyph's position.
   * @param[in] fromGlyphIndex The index of the first glyph within the text to be drawn
   * @param[in] toGlyphIndex The index of the last glyph within the text to be drawn
   *
   * @return An image buffer with the text.
   */
  PixelBuffer CreateImageBuffer(const uint32_t bufferWidth, const uint32_t bufferHeight,
                                const Typesetter::Style style, const bool ignoreHorizontalAlignment,
                                const Pixel::Format pixelFormat, const int32_t horizontalOffset,
                                const int32_t verticalOffset, const TextAbstraction::GlyphIndex fromGlyphIndex,
                                const TextAbstraction::GlyphIndex toGlyphIndex);

  /**
   * @brief Create & draw a L8 mask containing TextGradient target glyph coverage only.
   *
   * The mask includes default-color monochrome glyph fill. Explicit-color glyphs, renderable color glyphs and text
   * styles are excluded.
   *
   * @param[in] bufferWidth The width of the image buffer.
   * @param[in] bufferHeight The height of the image buffer.
   * @param[in] ignoreHorizontalAlignment Whether to ignore the horizontal alignment, not ignored by default.
   * @param[in] pixelFormat The format of the mask. The initial implementation uses Pixel::L8.
   * @param[in] horizontalOffset The horizontal offset to be added to the glyph's position.
   * @param[in] verticalOffset The vertical offset to be added to the glyph's position.
   * @param[in] fromGlyphIndex The index of the first glyph within the text to be drawn.
   * @param[in] toGlyphIndex The index of the last glyph within the text to be drawn.
   *
   * @return An image buffer containing TextGradient target glyph coverage only.
   */
  PixelBuffer CreateTextGradientMaskImageBuffer(const uint32_t bufferWidth, const uint32_t bufferHeight,
                                                const bool                        ignoreHorizontalAlignment,
                                                const Pixel::Format               pixelFormat,
                                                const int32_t                     horizontalOffset,
                                                const int32_t                     verticalOffset,
                                                const TextAbstraction::GlyphIndex fromGlyphIndex,
                                                const TextAbstraction::GlyphIndex toGlyphIndex);

  /**
   * @brief Create & draw a RGBA buffer containing TextGradient non-target glyphs only.
   *
   * Explicit-color glyphs and renderable color glyphs are preserved. Default-color monochrome glyphs are excluded.
   *
   * @param[in] bufferWidth The width of the image buffer.
   * @param[in] bufferHeight The height of the image buffer.
   * @param[in] ignoreHorizontalAlignment Whether to ignore the horizontal alignment, not ignored by default.
   * @param[in] pixelFormat The format of the preserved buffer. The initial implementation uses Pixel::RGBA8888.
   * @param[in] horizontalOffset The horizontal offset to be added to the glyph's position.
   * @param[in] verticalOffset The vertical offset to be added to the glyph's position.
   * @param[in] fromGlyphIndex The index of the first glyph within the text to be drawn.
   * @param[in] toGlyphIndex The index of the last glyph within the text to be drawn.
   *
   * @return An image buffer containing TextGradient non-target glyphs only.
   */
  PixelBuffer CreateTextGradientPreservedImageBuffer(const uint32_t bufferWidth, const uint32_t bufferHeight,
                                                     const bool                        ignoreHorizontalAlignment,
                                                     const Pixel::Format               pixelFormat,
                                                     const int32_t                     horizontalOffset,
                                                     const int32_t                     verticalOffset,
                                                     const TextAbstraction::GlyphIndex fromGlyphIndex,
                                                     const TextAbstraction::GlyphIndex toGlyphIndex);

private:
  std::unique_ptr<ViewModel>           mModel;
  std::unique_ptr<RevealRasterContext> mRevealRasterContext;
  TextAbstraction::FontClient          mFontClient;
};

} // namespace Text

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_TEXT_TYPESETTER_IMPL_H
