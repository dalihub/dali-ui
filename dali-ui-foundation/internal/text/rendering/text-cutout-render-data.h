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

#ifndef DALI_UI_TEXT_CUTOUT_RENDER_DATA_H
#define DALI_UI_TEXT_CUTOUT_RENDER_DATA_H

// EXTERNAL INCLUDES
#include <dali/integration-api/pixel-data-integ.h>
#include <dali/public-api/images/pixel-data.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <algorithm>
#include <cmath>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/rendering/text-raster-coordinate.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Text
{
/**
 * @brief Checks the supported shader cutout combinations without changing the request.
 * @param[in] hasWinningGradientSpan Whether a canonical gradient span owns any characters.
 * @param[in] marquee Whether scrolling is active.
 * @param[in] textGradient Whether the global foreground gradient is renderable.
 * @param[in] overlayGradient Whether the global overlay gradient is renderable.
 * @param[in] reveal Whether text reveal is enabled.
 * @param[in] replacement Whether valid inline replacements are present.
 * @param[in] emboss Whether the bevel effect is enabled.
 * @param[in] nonColorBackground Whether the owner has a non-COLOR background visual.
 * @return True if the request can use cropped shader cutout planes.
 */
inline bool IsShaderCutoutSupported(bool hasWinningGradientSpan,
                                    bool marquee, bool textGradient, bool overlayGradient,
                                    bool reveal, bool replacement, bool emboss, bool nonColorBackground = false)
{
  return !(marquee || textGradient || overlayGradient || reveal || replacement || emboss || nonColorBackground ||
           hasWinningGradientSpan);
}

/**
 * @brief CPU raster inputs from one layout, published together as an immutable snapshot.
 *
 * The style plane excludes the control background, which is composed from a
 * renderer uniform. PixelData handles own the buffers through async publication.
 */
struct CutoutRenderData
{
  PixelData coverage;    ///< L8 coverage, or premultiplied RGBA colored glyphs.
  PixelData style;       ///< Background decorations, without the control background.
  PixelData overlay;     ///< Underline/strikethrough, composited after cutout.
  Vector2   controlSize; ///< Full raster size before downsampling, including padding.
  Vector4   rect;        ///< Integer crop (x, y, width, height) in full-control raster coordinates.

  /**
   * @brief Validates formats, buffer extents and the crop before texture upload.
   * @param[in] maxTextureSize Device limit, or zero for CPU-only validation.
   * @return True if every required plane belongs to a complete, valid snapshot.
   */
  bool IsValid(uint32_t maxTextureSize = 0u) const
  {
    const auto validPlane = [maxTextureSize](const PixelData& plane, Pixel::Format format)
    {
      if(!plane || plane.GetPixelFormat() != format || !plane.GetWidth() || !plane.GetHeight())
      {
        return false;
      }
      if(maxTextureSize && (plane.GetWidth() > maxTextureSize || plane.GetHeight() > maxTextureSize))
      {
        return false;
      }
      const auto bpp = Pixel::GetBytesPerPixel(format);
      if(!Raster::BufferFits(plane.GetWidth(), plane.GetHeight(), bpp))
      {
        return false;
      }
      const auto     bytes  = Dali::Integration::GetPixelDataBuffer(plane);
      const uint64_t stride = plane.GetStrideBytes() ? plane.GetStrideBytes() : static_cast<uint64_t>(plane.GetWidth()) * bpp;
      return bytes.buffer && stride >= static_cast<uint64_t>(plane.GetWidth()) * bpp &&
             stride * plane.GetHeight() <= bytes.bufferSize;
    };
    const auto sameSize = [&](const PixelData& plane)
    {
      return plane.GetWidth() == coverage.GetWidth() && plane.GetHeight() == coverage.GetHeight();
    };
    if(!coverage || (coverage.GetPixelFormat() != Pixel::L8 && coverage.GetPixelFormat() != Pixel::RGBA8888) ||
       !validPlane(coverage, coverage.GetPixelFormat()) || !validPlane(style, Pixel::RGBA8888) ||
       !(sameSize(style) || (style.GetWidth() == 1u && style.GetHeight() == 1u)) ||
       (overlay && (!validPlane(overlay, Pixel::RGBA8888) || !sameSize(overlay))))
    {
      return false;
    }
    return std::isfinite(controlSize.x) && std::isfinite(controlSize.y) && controlSize.x >= 0.f && controlSize.y >= 0.f &&
           std::isfinite(rect.x) && std::isfinite(rect.y) && rect.x >= 0.f && rect.y >= 0.f &&
           rect.x == std::floor(rect.x) && rect.y == std::floor(rect.y) &&
           static_cast<double>(rect.z) == coverage.GetWidth() && static_cast<double>(rect.w) == coverage.GetHeight() &&
           // Empty requests use a transparent 1x1 plane for a zero logical extent.
           static_cast<double>(rect.x) + rect.z <= std::max(1.f, controlSize.x) &&
           static_cast<double>(rect.y) + rect.w <= std::max(1.f, controlSize.y);
  }
};
} // namespace Text
} // namespace Ui
} // namespace DALI_NAMESPACE
#endif // DALI_UI_TEXT_CUTOUT_RENDER_DATA_H
