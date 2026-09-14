#ifndef DALI_UI_TEXT_TYPESETTER_H
#define DALI_UI_TEXT_TYPESETTER_H

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
#include <dali/public-api/images/pixel-data.h>
#include <dali/public-api/images/pixel.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/object/ref-object.h>
#include <memory> ///< for std::unique_ptr

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/replacement/replacement-run-snapshot.h>
#include <dali-ui-foundation/internal/text/reveal/text-reveal.h>
#include <dali-ui-foundation/internal/text/text-enumerations.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Text
{
class ModelInterface;
class ViewModel;
class Typesetter;
struct FinalElisionResult;
struct MarqueeStartAnchor;
struct MarqueeTextureAnchor;

typedef IntrusivePtr<Typesetter> TypesetterPtr;

/**
 * @brief This class is responsible of controlling the data flow of the text's rendering process.
 */
class Typesetter : public RefObject
{
public:
  /**
   * @brief Behaviours of how to render the text.
   */
  enum RenderBehaviour
  {
    RENDER_TEXT_AND_STYLES, ///< Render both the text and its styles
    RENDER_NO_TEXT,         ///< Render only the underlay style plane: shadow, outline, and background.
    RENDER_NO_STYLES,       ///< Do not render any styles
    RENDER_MASK,            ///< Render an alpha mask (for color glyphs with no color animation, e.g. emoji)
    RENDER_OVERLAY_STYLE    ///< Render only the overlay decoration plane: strikethrough and underline.
  };

  /**
   * @brief Styles of the text.
   */
  enum Style
  {
    STYLE_NONE,         ///< No style
    STYLE_MASK,         ///< Alpha mask
    STYLE_SHADOW,       ///< Hard shadow
    STYLE_SOFT_SHADOW,  ///< Soft shadow
    STYLE_UNDERLINE,    ///< Underline
    STYLE_OUTLINE,      ///< Outline
    STYLE_BACKGROUND,   ///< Text background
    STYLE_STRIKETHROUGH ///< Strikethrough
  };

public: // Constructor.
  /**
   * @brief Creates a Typesetter instance.
   *
   * The typesetter composes the final text retrieving the glyphs and the
   * styles from the text's model.
   *
   * @param[in] model Pointer to the text's data model.
   */
  static TypesetterPtr New(const ModelInterface* const model);

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
   * @brief Selects the authoritative resolved glyph sequence used by subsequent render calls.
   *
   * The result is not owned by Typesetter and must remain valid while it is
   * installed on the underlying ViewModel.
   *
   * @param[in] result The replacement or non-replacement END result, or nullptr for the source path.
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
   * @brief Gets the font client used by this typesetter.
   *
   * @return The font client used by the text rendering pipeline.
   */
  TextAbstraction::FontClient GetFontClient();

  /**
   * @brief Renders the text.
   *
   * Does the following operations:
   * - Finds the visible pages needed to be rendered.
   * - Elide glyphs if needed.
   * - Creates image buffers for diffrent text styles with the given size.
   * - Combines different image buffers to create the pixel data used to generate the final image
   *
   * @param[in] size The renderer size.
   * @param[in] textDirection The direction of the text.
   * @param[in] behaviour The behaviour of how to render the text (i.e. whether to render the text only or the styles
   * only or both).
   * @param[in] ignoreHorizontalAlignment Whether to ignore the horizontal alignment (i.e. always render as if
   * HORIZONTAL_ALIGN_BEGIN).
   * @param[in] pixelFormat The format of the pixel in the image that the text is rendered as (i.e. either
   * Pixel::BGRA8888 or Pixel::L8).
   * @param[in] originSize The origin size for calculating vertical alignment. (default) If zero, the control and
   * renderer sizes are used.
   *
   * @return A pixel data with the text rendered.
   */
  PixelData Render(const Vector2& size, Direction textDirection,
                   RenderBehaviour behaviour = RENDER_TEXT_AND_STYLES, bool ignoreHorizontalAlignment = false,
                   Pixel::Format pixelFormat = Pixel::RGBA8888, const Vector2& originSize = Size::ZERO);

  /**
   * @brief Resolves an anchor in the exact ViewModel geometry used by the preceding render.
   *
   * This is intended for horizontal marquee setup immediately after Render().
   * It does not shape, lay out, or inspect raster pixels.
   *
   * @param[in] anchor Stable source identity captured from the static END layout.
   * @return The anchor's pre-raster X in the marquee texture, or an invalid fallback.
   */
  MarqueeTextureAnchor ResolveMarqueeTextureAnchor(const MarqueeStartAnchor& anchor) const;

  /**
   * @brief Renders the default-color monochrome glyph fill coverage for TextGradient.
   *
   * This is a separate mask path for future TextGradient composition. It does not reuse RENDER_MASK, which is reserved
   * for the existing color-glyph protection path.
   *
   * @param[in] size The renderer size.
   * @param[in] textDirection The direction of the text.
   * @param[in] ignoreHorizontalAlignment Whether to ignore the horizontal alignment (i.e. always render as if
   * HORIZONTAL_ALIGN_BEGIN).
   * @param[in] pixelFormat The format of the mask. The initial implementation uses Pixel::L8.
   * @param[in] originSize The origin size for calculating vertical alignment. (default) If zero, the control and
   * renderer sizes are used.
   *
   * @return A pixel data containing glyph coverage for TextGradient target glyphs only.
   */
  PixelData RenderTextGradientMask(const Vector2& size, Direction textDirection,
                                   bool ignoreHorizontalAlignment = false, Pixel::Format pixelFormat = Pixel::L8,
                                   const Vector2& originSize = Size::ZERO);

  /**
   * @brief Renders non-gradient glyphs for mixed TextGradient composition.
   *
   * This path renders explicit-color glyphs and renderable color glyphs while skipping default-color monochrome glyphs
   * that will be filled by the TextGradient mask. It does not render text styles.
   *
   * @param[in] size The renderer size.
   * @param[in] textDirection The direction of the text.
   * @param[in] ignoreHorizontalAlignment Whether to ignore the horizontal alignment (i.e. always render as if
   * HORIZONTAL_ALIGN_BEGIN).
   * @param[in] pixelFormat The format of the preserved texture. The initial implementation uses Pixel::RGBA8888.
   * @param[in] originSize The origin size for calculating vertical alignment. (default) If zero, the control and
   * renderer sizes are used.
   *
   * @return A pixel data containing non-gradient text glyphs only.
   */
  PixelData RenderTextGradientPreserved(const Vector2& size, Direction textDirection,
                                        bool           ignoreHorizontalAlignment = false,
                                        Pixel::Format  pixelFormat               = Pixel::RGBA8888,
                                        const Vector2& originSize                = Size::ZERO);

  /**
   * @brief After the Render, use the pixel information of the given cutoutBuffer to make the part where the pixel is
   * drawn transparent.
   *
   * @param[in] size The renderer size.
   * @param[in] textDirection The direction of the text.
   * @param[in] cutoutBuffer The buffer to use pixel information to cutout.
   * @param[in] behaviour The behaviour of how to render the text (i.e. whether to render the text only or the styles
   * only or both).
   * @param[in] ignoreHorizontalAlignment Whether to ignore the horizontal alignment (i.e. always render as if
   * HORIZONTAL_ALIGN_BEGIN).
   * @param[in] pixelFormat The format of the pixel in the image that the text is rendered as (i.e. either
   * Pixel::BGRA8888 or Pixel::L8).
   * @param[in] originAlpha The original alpha of text.
   * @param[in] originSize The origin size for calculating vertical alignment. (default) If zero, the control and
   * renderer sizes are used.
   *
   * @return A pixel data with the text rendered.
   */
  PixelData RenderWithCutout(const Vector2& size, Direction textDirection,
                             PixelBuffer cutoutBuffer, RenderBehaviour behaviour = RENDER_TEXT_AND_STYLES,
                             bool ignoreHorizontalAlignment = false, Pixel::Format pixelFormat = Pixel::RGBA8888,
                             float originAlpha = 1.f, const Vector2& originSize = Size::ZERO);

  /**
   * @brief Renders the text, return as PixelBuffer.
   *
   * This function is used to obtain the PixelBuffer required for cutout.
   *
   * @param[in] size The renderer size.
   * @param[in] textDirection The direction of the text.
   * @param[in] cutoutBuffer The buffer to use pixel information to cutout.
   * @param[in] behaviour The behaviour of how to render the text (i.e. whether to render the text only or the styles
   * only or both).
   * @param[in] ignoreHorizontalAlignment Whether to ignore the horizontal alignment (i.e. always render as if
   * HORIZONTAL_ALIGN_BEGIN).
   * @param[in] pixelFormat The format of the pixel in the image that the text is rendered as (i.e. either
   * Pixel::BGRA8888 or Pixel::L8).
   * @param[in] originSize The origin size for calculating vertical alignment. (default) If zero, the control and
   * renderer sizes are used.
   *
   * @return A pixel data with the text rendered.
   */
  PixelBuffer RenderWithPixelBuffer(const Vector2& size, Direction textDirection,
                                    RenderBehaviour behaviour                 = RENDER_TEXT_AND_STYLES,
                                    bool            ignoreHorizontalAlignment = false,
                                    Pixel::Format   pixelFormat               = Pixel::RGBA8888,
                                    const Vector2&  originSize                = Size::ZERO);

  /**
   * @brief Projects source reveal semantics onto the current final glyph sequence.
   *
   * This resolves the ViewModel elision state and enables final-to-source
   * mapping. Call it once after ordinary text rendering and before rasterizing
   * any reveal metadata tile so every tile shares the same final plan.
   *
   * @param[in] sourcePlan The reveal plan indexed by source glyph.
   * @param[in] unit The reveal unit used to assign synthetic ellipsis ownership.
   * @return A reveal plan indexed by final rendered glyph.
   */
  Internal::Reveal::Plan CreateFinalRevealPlan(const Internal::Reveal::Plan& sourcePlan,
                                               Internal::Reveal::Unit        unit,
                                               Internal::Reveal::Sequence    sequence             = Internal::Reveal::Sequence::WHOLE_TEXT,
                                               float                         sequenceStaggerRatio = 0.0f);

  /**
   * @brief Extracts visible ImageSpan timing from a finalized shared plan.
   *
   * PIXEL uses the reserved box center as the atomic image timing point.
   * Returns false if any visible eligible image lacks complete final timing.
   */
  bool ExtractReplacementRevealTimings(const Internal::Reveal::Plan&       finalPlan,
                                       const ReplacementSourceSnapshot&    source,
                                       const Vector<ReplacementPlacement>& placements,
                                       Vector<ReplacementRevealTiming>&    timings);

  /**
   * @brief Rasterizes reveal metadata for one full texture or height tile.
   *
   * RG stores a 16-bit normalized unit start, B stores validity, and A stores
   * the winning glyph coverage used to resolve overlapping glyph bitmaps.
   * The supplied plan must already be projected onto the final glyph sequence.
   * Tile calls for one layout must all receive the same plan.
   *
   * @param[in] tileSize The dimensions of the metadata texture to create.
   * @param[in] textDirection The resolved text direction.
   * @param[in] plan The final-glyph reveal plan shared by all tiles.
   * @param[out] fadeDuration The normalized fade duration stored in the plan.
   * @param[in] tileOffsetY The vertical tile offset in the full rendered text.
   * @param[in] fullSize The full rendered text size, or Size::ZERO for a non-tiled render.
   * @param[in] ignoreHorizontalAlignment Whether horizontal alignment is ignored.
   * @param[in] originSize The original size used to resolve vertical alignment.
   * @return RGBA8888 reveal metadata for the requested full texture or tile.
   */
  PixelData RenderTextRevealMetadata(
    const Vector2&                tileSize,
    Direction                     textDirection,
    const Internal::Reveal::Plan& plan,
    float&                        fadeDuration,
    uint32_t                      tileOffsetY               = 0u,
    const Vector2&                fullSize                  = Size::ZERO,
    bool                          ignoreHorizontalAlignment = false,
    const Vector2&                originSize                = Size::ZERO);

  /**
   * @brief Selects a foreground or metadata plane for Reveal blur preparation.
   */
  enum class RuntimeBlurPlane
  {
    TEXT,
    COLOR_MASK,
    GRADIENT_PRESERVED,
    GRADIENT_MASK,
    REVEAL_METADATA
  };

  /**
   * @brief Finds a conservative source band for one runtime-blur line.
   *
   * Uses actual font bitmaps and the rasterizer's integer baseline accumulation,
   * not nominal line boxes. Keeps the full width and two transparent guard rows.
   * GradientSpan, hyphenation and non-END elision retain the full target to
   * preserve raster coordinates and coverage. No selection state or bitmap
   * ownership survives the call.
   * A supplied baseline must use the rasterizer's per-line integer truncation.
   */
  Rect<uint32_t> GetRuntimeBlurLineRasterBounds(const Vector2& size, LineIndex lineIndex, const int32_t* lineBaseline = nullptr);

  /**
   * @brief Rasterizes one final layout line without changing its coordinates.
   *
   * This opt-in setup path uses the ordinary glyph-range rasterizer. All line
   * metrics and gradient bounds remain intact, including glyph overhangs;
   * no screen-space line clipping or persistent raster-selection state is used.
   * The caller must resolve the final model and plan before requesting planes.
   * Planes may use a vertical tile (zero height selects the full target).
   * Its origin is subtracted after computing alignment in the original size;
   * metadata may also use a horizontal tile (zero width selects the full width).
   * PIXEL timing keeps the original visual X, independently of tile storage.
   * Include one extra source texel outside each cropped metadata edge for
   * ownership expansion. Non-metadata planes retain the full width.
   * Non-metadata GradientSpan planes require the full target so their paint
   * coordinates and overlap coverage retain the ordinary raster semantics.
   */
  PixelData RenderRuntimeBlurLine(const Vector2& size, LineIndex lineIndex,
                                  RuntimeBlurPlane plane, Pixel::Format format,
                                  const Internal::Reveal::Plan& plan,
                                  uint32_t rasterOffsetY = 0u, uint32_t rasterHeight = 0u,
                                  uint32_t rasterOffsetX = 0u, uint32_t rasterWidth = 0u);

  /**
   * @brief Create & draw the image buffer of single background color.
   *
   * @param[in] bufferWidth The width of the image buffer.
   * @param[in] bufferHeight The height of the image buffer.
   * @param[in] backgroundColor The backgroundColor of image buffer.
   *
   * @return An image buffer with the text.
   */
  PixelBuffer CreateFullBackgroundBuffer(const uint32_t bufferWidth, const uint32_t bufferHeight,
                                         const Vector4& backgroundColor);

  /**
   * @brief Set Mask for two pixel buffer.
   *
   * The alpha value of bottomPixelBuffer is decreased as the alpha value of topPixelBuffer is higher.
   *
   * @param[in, out] topPixelBuffer The top layer buffer.
   * @param[in, out] bottomPixelBuffer The bottom layer buffer.
   * @param[in] bufferWidth The width of the image buffer.
   * @param[in] bufferHeight The height of the image buffer.
   * @param[in] originAlpha The original alpha value of the text.
   */
  void SetMaskForImageBuffer(PixelBuffer& __restrict__ topPixelBuffer,
                             PixelBuffer& __restrict__ bottomPixelBuffer, const uint32_t bufferWidth,
                             const uint32_t bufferHeight, float originAlpha);

private:
  /**
   * @brief Private constructor.
   *
   * @param[in] model Pointer to the text's data model.
   */
  Typesetter(const ModelInterface* const model);

  // Declared private and left undefined to avoid copies.
  Typesetter(const Typesetter& handle);

  // Declared private and left undefined to avoid copies.
  Typesetter& operator=(const Typesetter& handle);

  /**
   * @brief Apply markup underline tags.
   *
   * The properties on TextLabel override the behavior of Markup.
   * Because the markup will be the bottom layer buffer
   *  - i.e: If you set property UNDERLINE to enabled and blue.
   *    And the TEXT is "<color value='green'>Hello</color> <u>World</u> <i>Hello</i> <b>World</b>".
   *    Then the output of the whole text is underlined by blue line.
   *
   * @param[in] topPixelBuffer The top layer buffer.
   * @param[in] bufferWidth The width of the image buffer.
   * @param[in] bufferHeight The height of the image buffer.
   * @param[in] ignoreHorizontalAlignment Whether to ignore the horizontal alignment, not ignored by default.
   * @param[in] pixelFormat The format of the pixel in the image that the text is rendered as (i.e. either
   * Pixel::BGRA8888 or Pixel::L8).
   * @param[in] horizontalOffset The horizontal offset to be added to the glyph's position.
   * @param[in] verticalOffset The vertical offset to be added to the glyph's position.
   *
   * @return The image buffer with the markup.
   */
  PixelBuffer ApplyUnderlineMarkupImageBuffer(PixelBuffer topPixelBuffer, const uint32_t bufferWidth,
                                              const uint32_t bufferHeight, const bool ignoreHorizontalAlignment,
                                              const Pixel::Format pixelFormat, const int32_t horizontalOffset,
                                              const int32_t verticalOffset);

  /**
   * @brief Apply markup strikethrough tags.
   *
   * The properties on TextLabel override the behavior of Markup.
   * Because the markup will be the bottom layer buffer
   *  - i.e: If you set property STRIKETHROUGH to enabled and blue.
   *    And the TEXT is "<color value='green'>Hello</color> <s>World</s> <i>Hello</i> <b>World</b>".
   *    Then the whole text will have a blue line strikethrough.
   *
   * @param[in] topPixelBuffer The top layer buffer.
   * @param[in] bufferWidth The width of the image buffer.
   * @param[in] bufferHeight The height of the image buffer.
   * @param[in] ignoreHorizontalAlignment Whether to ignore the horizontal alignment, not ignored by default.
   * @param[in] pixelFormat The format of the pixel in the image that the text is rendered as (i.e. either
   * Pixel::BGRA8888 or Pixel::L8).
   * @param[in] horizontalOffset The horizontal offset to be added to the glyph's position.
   * @param[in] verticalOffset The vertical offset to be added to the glyph's position.
   *
   * @return The image buffer with the markup.
   */
  PixelBuffer ApplyStrikethroughMarkupImageBuffer(PixelBuffer topPixelBuffer, const uint32_t bufferWidth,
                                                  const uint32_t      bufferHeight,
                                                  const bool          ignoreHorizontalAlignment,
                                                  const Pixel::Format pixelFormat,
                                                  const int32_t horizontalOffset, const int32_t verticalOffset);

protected:
  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   *
   * Destroys the visual model.
   */
  virtual ~Typesetter();

private:
  struct Impl;
  std::unique_ptr<Impl> mImpl;
};

} // namespace Text

} // namespace Ui

} //namespace DALI_NAMESPACE

#endif // DALI_UI_TEXT_TYPESETTER_H
