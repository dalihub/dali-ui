#ifndef DALI_UI_TEXT_RASTER_COORDINATE_H
#define DALI_UI_TEXT_RASTER_COORDINATE_H

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
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Text
{
namespace Raster
{
// Ordered comparisons also reject NaN and infinities. The upper bound is
// exclusive: converting INT_MAX to float rounds UP on both ARM32 and x86.
inline bool Convert(float value, int32_t& result)
{
  if(!(value >= -2147483648.0f && value < 2147483648.0f))
  {
    return false;
  }
  result = static_cast<int32_t>(value); // Preserve truncation toward zero.
  return true;
}

inline bool Add(int32_t value, int64_t offset, int32_t& result)
{
  const int64_t sum = static_cast<int64_t>(value) + offset;
  if(sum < std::numeric_limits<int32_t>::min() || sum > std::numeric_limits<int32_t>::max())
  {
    return false;
  }
  result = static_cast<int32_t>(sum);
  return true;
}

inline bool AddCoordinate(int32_t value, float offset, int32_t& result)
{
  int32_t integerOffset;
  return Convert(offset, integerOffset) && Add(value, integerOffset, result);
}

// Destination raster loops use signed 32-bit pixel indices; PixelBuffer stores
// its byte size in uint32_t. Also respect the target's pointer extent. L8 must
// not inherit RGBA's byte limit, especially for tall textures tiled at upload.
inline bool BufferFits(uint32_t width, uint32_t height, uint32_t bytesPerPixel)
{
  constexpr uint64_t maxBytes  = std::min<uint64_t>(std::numeric_limits<uint32_t>::max(),
                                                    static_cast<uint64_t>(std::numeric_limits<ptrdiff_t>::max()));
  constexpr uint64_t maxPixels = static_cast<uint64_t>(std::numeric_limits<int32_t>::max());
  const uint64_t     pixels    = static_cast<uint64_t>(width) * height;
  return bytesPerPixel != 0u && width <= maxPixels && height <= maxPixels &&
         pixels <= maxPixels && pixels <= maxBytes / bytesPerPixel;
}

inline bool GlyphBufferFits(uint32_t width, uint32_t height, uint32_t bytesPerPixel)
{
  // Source scanline skipping multiplies a signed 32-bit byte stride.
  return BufferFits(width, height, bytesPerPixel) &&
         static_cast<uint64_t>(width) * height * bytesPerPixel <= static_cast<uint64_t>(std::numeric_limits<int32_t>::max());
}

struct GlyphClip
{
  int32_t x, y;
  int32_t left, right, top, bottom; // Half-open indices inside the source bitmap.
};

inline bool ClipGlyph(float positionX, float positionY, int32_t horizontalOffset, int32_t verticalOffset,
                      uint32_t width, uint32_t height, uint32_t glyphBytesPerPixel,
                      uint32_t bufferWidth, uint32_t bufferHeight, GlyphClip& clip,
                      uint32_t bufferBytesPerPixel = 4u)
{
  int32_t x, y, end;
  if(width == 0u || height == 0u || !GlyphBufferFits(width, height, glyphBytesPerPixel) ||
     !BufferFits(bufferWidth, bufferHeight, bufferBytesPerPixel) ||
     !Convert(positionX, x) || !Convert(positionY, y) ||
     !Add(x, width, end) || !Add(y, height, end) ||
     !Add(x, horizontalOffset, clip.x) || !Add(y, verticalOffset, clip.y))
  {
    return false;
  }

  // Widen BEFORE negation/subtraction. In particular, -INT_MIN and
  // bufferWidth - a negative offset must never be evaluated as int32_t.
  const int64_t left   = std::max<int64_t>(0, -static_cast<int64_t>(clip.x));
  const int64_t top    = std::max<int64_t>(0, -static_cast<int64_t>(clip.y));
  const int64_t right  = std::min<int64_t>(width, static_cast<int64_t>(bufferWidth) - clip.x);
  const int64_t bottom = std::min<int64_t>(height, static_cast<int64_t>(bufferHeight) - clip.y);
  if(left >= right || top >= bottom)
  {
    return false;
  }
  clip.left   = static_cast<int32_t>(left);
  clip.right  = static_cast<int32_t>(right);
  clip.top    = static_cast<int32_t>(top);
  clip.bottom = static_cast<int32_t>(bottom);
  return true;
}

// Clipping valid coordinates is distinct from clamping malformed layout data:
// nonrepresentable values are rejected before any destination range is formed.
inline bool ClipRange(float first, float last, uint32_t extent, int32_t& begin, int32_t& end)
{
  if(!Convert(first, begin) || !Convert(last, end) || extent > static_cast<uint32_t>(std::numeric_limits<int32_t>::max()))
  {
    return false;
  }
  begin = std::max(0, std::min(begin, static_cast<int32_t>(extent)));
  end   = std::max(0, std::min(end, static_cast<int32_t>(extent)));
  return end > begin;
}
} // namespace Raster
} // namespace Text
} // namespace Ui
} // namespace DALI_NAMESPACE

#endif // DALI_UI_TEXT_RASTER_COORDINATE_H
