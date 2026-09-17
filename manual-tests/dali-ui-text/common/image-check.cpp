/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
 */

#include "image-check.h"

#if TEXT_PIXEL_CHECKS
#include <png.h>
#endif
#include <algorithm>
#include <cmath>
#include <limits>
#include <sstream>

namespace TextManualTest::ImageCheck
{
namespace
{
// Bound allocations and arithmetic on untrusted readback dimensions.
constexpr uint64_t MAX_PIXELS = 16u * 1024u * 1024u;
int                Distance(Color a, Color b)
{
  return std::max({std::abs(int(a.r) - b.r), std::abs(int(a.g) - b.g),
                   std::abs(int(a.b) - b.b), std::abs(int(a.a) - b.a)});
}
bool Ink(Color pixel, Color background = {}, uint8_t threshold = 32)
{
  return Distance(pixel, background) > threshold;
}
} // namespace

bool Image::IsValid() const
{
  const uint64_t pixels = uint64_t(width) * height;
  return width && height && pixels <= MAX_PIXELS && pixels * 4 == rgba.size();
}

Color Image::At(uint32_t x, uint32_t y) const
{
  const auto i = (std::size_t(y) * width + x) * 4u;
  return {rgba[i], rgba[i + 1], rgba[i + 2], rgba[i + 3]};
}

bool Decode(const uint8_t* bytes, std::size_t byteCount, uint32_t width, uint32_t height,
            uint32_t channels, Image& output, std::string& error, bool blueFirst)
{
  output = {};
  error.clear();
  const uint64_t pixels = uint64_t(width) * height;
  if(!bytes || !width || !height || pixels > MAX_PIXELS ||
     (channels != 3 && channels != 4) || pixels * channels != byteCount)
  {
    error = "invalid dimensions, RGB/RGBA format or tightly packed buffer size";
    return false;
  }
  output.width  = width;
  output.height = height;
  output.rgba.resize(pixels * 4u);
  for(std::size_t i = 0; i < pixels; ++i)
  {
    std::copy_n(bytes + i * channels, 3, output.rgba.data() + i * 4u);
    if(blueFirst) std::swap(output.rgba[i * 4u], output.rgba[i * 4u + 2]);
    output.rgba[i * 4u + 3] = channels == 4 ? bytes[i * channels + 3] : 255;
  }
  return true;
}

bool Contains(const Image& image, Rect r)
{
  return image.IsValid() && r.x >= 0 && r.y >= 0 && r.width > 0 && r.height > 0 &&
         uint64_t(r.x) + r.width <= image.width && uint64_t(r.y) + r.height <= image.height;
}

Image Crop(const Image& image, Rect r)
{
  if(!Contains(image, r)) return {};
  Image result{uint32_t(r.width), uint32_t(r.height), std::vector<uint8_t>(std::size_t(r.width) * r.height * 4)};
  for(int y = 0; y < r.height; ++y)
    std::copy_n(image.rgba.data() + (std::size_t(r.y + y) * image.width + r.x) * 4,
                r.width * 4, result.rgba.data() + std::size_t(y) * r.width * 4);
  return result;
}

std::size_t CountColor(const Image& image, Rect r, Color color, uint8_t tolerance)
{
  if(!Contains(image, r)) return 0;
  std::size_t count = 0;
  for(int y = r.y; y < r.y + r.height; ++y)
    for(int x = r.x; x < r.x + r.width; ++x)
      count += Distance(image.At(x, y), color) <= tolerance;
  return count;
}

Rect ColorBounds(const Image& image, Rect r, Color color, uint8_t tolerance)
{
  if(!Contains(image, r)) return {};
  int left = r.x + r.width, right = -1, top = r.y + r.height, bottom = -1;
  for(int y = r.y; y < r.y + r.height; ++y)
    for(int x = r.x; x < r.x + r.width; ++x)
      if(Distance(image.At(x, y), color) <= tolerance)
      {
        left   = std::min(left, x);
        right  = std::max(right, x);
        top    = std::min(top, y);
        bottom = std::max(bottom, y);
      }
  return right < 0 ? Rect{} : Rect{left, top, right - left + 1, bottom - top + 1};
}

std::size_t CountInk(const Image& image, Rect r, Color background, uint8_t threshold)
{
  if(!Contains(image, r)) return 0;
  std::size_t count = 0;
  for(int y = r.y; y < r.y + r.height; ++y)
    for(int x = r.x; x < r.x + r.width; ++x)
      count += Ink(image.At(x, y), background, threshold);
  return count;
}

Comparison Compare(const Image& expected, const Image& actual, Rect r, CompareOptions options)
{
  Comparison result;
  if(expected.width != actual.width || expected.height != actual.height ||
     !Contains(expected, r) || !Contains(actual, r) ||
     !std::isfinite(options.maximumChangedFraction) || options.maximumChangedFraction < 0 || options.maximumChangedFraction > 1)
  {
    result.error = "invalid image, ROI, dimensions or comparison options";
    return result;
  }
  for(int y = r.y; y < r.y + r.height; ++y)
    for(int x = r.x; x < r.x + r.width; ++x)
    {
      const auto a = expected.At(x, y), b = actual.At(x, y);
      result.inkPixels += Ink(a, options.background, options.inkThreshold) || Ink(b, options.background, options.inkThreshold);
      result.changedPixels += Distance(a, b) > options.channelTolerance;
    }
  if(result.inkPixels < options.minimumInkPixels)
  {
    result.error = "insufficient foreground; blank captures cannot pass";
    return result;
  }
  result.valid           = true;
  result.changedFraction = double(result.changedPixels) / std::max(std::size_t(1), result.inkPixels);
  result.equal           = result.changedFraction <= options.maximumChangedFraction && result.changedPixels <= options.maximumChangedPixels;
  return result;
}

std::string Comparison::Describe() const
{
  std::ostringstream out;
  out << "valid=" << valid << " changed=" << changedPixels << " ink=" << inkPixels
      << " fraction=" << changedFraction;
  if(!error.empty()) out << " error=" << error;
  return out.str();
}

Image Difference(const Image& expected, const Image& actual, uint8_t tolerance)
{
  if(!expected.IsValid() || !actual.IsValid() || expected.width != actual.width || expected.height != actual.height) return {};
  Image output = actual;
  for(uint32_t y = 0; y < output.height; ++y)
    for(uint32_t x = 0; x < output.width; ++x)
    {
      const bool changed = Distance(expected.At(x, y), actual.At(x, y)) > tolerance;
      auto*      p       = output.rgba.data() + (std::size_t(y) * output.width + x) * 4;
      p[0]               = 255;
      p[1]               = changed ? 0 : 255;
      p[2]               = 255;
      p[3]               = 255;
    }
  return output;
}

Translation EstimateTranslation(const Image& a, const Image& b, Rect r, int maximumX, int maximumY, std::size_t minimumInkPixels)
{
  Translation best;
  if(a.width != b.width || a.height != b.height || !Contains(a, r) || !Contains(b, r) ||
     maximumX < 0 || maximumY < 0 || maximumX > r.width / 3 || maximumY > r.height / 3) return best;
  const Rect overlap{r.x + maximumX, r.y + maximumY, r.width - 2 * maximumX, r.height - 2 * maximumY};
  // Classify once, not for every candidate displacement. This keeps capture
  // analysis affordable on a small target CPU without reducing pixel coverage.
  std::vector<uint8_t> maskA(std::size_t(a.width) * a.height), maskB(maskA.size());
  for(int y = r.y; y < r.y + r.height; ++y)
    for(int x = r.x; x < r.x + r.width; ++x)
    {
      const auto offset = std::size_t(y) * a.width + x;
      maskA[offset]     = Ink(a.At(x, y));
      maskB[offset]     = Ink(b.At(x, y));
    }
  for(int dy = -maximumY; dy <= maximumY; ++dy)
    for(int dx = -maximumX; dx <= maximumX; ++dx)
    {
      std::size_t ink = 0, different = 0;
      for(int y = overlap.y; y < overlap.y + overlap.height; ++y)
        for(int x = overlap.x; x < overlap.x + overlap.width; ++x)
        {
          const bool pa = maskA[std::size_t(y) * a.width + x];
          const bool pb = maskB[std::size_t(y + dy) * b.width + x + dx];
          ink += pa || pb;
          different += pa != pb;
        }
      const double score = double(different) / std::max(std::size_t(1), ink);
      if(ink >= minimumInkPixels && (score < best.mismatch ||
                                     (score == best.mismatch && std::abs(dx) + std::abs(dy) < std::abs(best.dx) + std::abs(best.dy))))
        best = {score < 0.25, dx, dy, score};
    }
  return best;
}

bool SavePng(const Image& image, const std::string& path, std::string& error)
{
#if TEXT_PIXEL_CHECKS
  error.clear();
  if(!image.IsValid())
  {
    error = "invalid PNG source";
    return false;
  }
  png_image png{};
  png.version   = PNG_IMAGE_VERSION;
  png.width     = image.width;
  png.height    = image.height;
  png.format    = PNG_FORMAT_RGBA;
  const bool ok = png_image_write_to_file(&png, path.c_str(), 0, image.rgba.data(), 0, nullptr) != 0;
  if(!ok) error = png.message;
  png_image_free(&png);
  return ok;
#else
  (void)image;
  (void)path;
  error = "pixel validation disabled at build time (TEXT_PIXEL_CHECKS=OFF)";
  return false;
#endif
}
bool LoadPng(const std::string& path, Image& image, std::string& error)
{
#if TEXT_PIXEL_CHECKS
  image = {};
  error.clear();
  png_image png{};
  png.version = PNG_IMAGE_VERSION;
  if(!png_image_begin_read_from_file(&png, path.c_str()))
  {
    error = png.message;
    png_image_free(&png);
    return false;
  }
  const uint64_t pixels = uint64_t(png.width) * png.height;
  if(!pixels || pixels > MAX_PIXELS)
  {
    error = "PNG dimensions exceed checker limits";
    png_image_free(&png);
    return false;
  }
  png.format    = PNG_FORMAT_RGBA;
  image         = {png.width, png.height, std::vector<uint8_t>(pixels * 4)};
  const bool ok = png_image_finish_read(&png, nullptr, image.rgba.data(), 0, nullptr) != 0;
  if(!ok)
  {
    error = png.message;
    image = {};
  }
  png_image_free(&png);
  return ok;
#else
  (void)path;
  image = {};
  error = "pixel validation disabled at build time (TEXT_PIXEL_CHECKS=OFF)";
  return false;
#endif
}
} // namespace TextManualTest::ImageCheck
