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

#include "common/image-check.h"
#if TEXT_PIXEL_CHECKS
#include <png.h>
#endif
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <limits>

namespace IC = TextManualTest::ImageCheck;
namespace
{
int  checks = 0, failures = 0;
void Check(bool condition, const char* name)
{
  ++checks;
  if(!condition)
  {
    ++failures;
    std::fprintf(stderr, "FAIL %s\n", name);
  }
}
IC::Image White(uint32_t width, uint32_t height)
{
  return {width, height, std::vector<uint8_t>(std::size_t(width) * height * 4, 255)};
}
void Paint(IC::Image& image, IC::Rect r, IC::Color color)
{
  for(int y = r.y; y < r.y + r.height; ++y)
    for(int x = r.x; x < r.x + r.width; ++x)
    {
      auto* p = image.rgba.data() + (std::size_t(y) * image.width + x) * 4;
      p[0]    = color.r;
      p[1]    = color.g;
      p[2]    = color.b;
      p[3]    = color.a;
    }
}
} // namespace

int main()
{
  IC::Image   decoded;
  std::string error;
  uint8_t     rgb[] = {7, 23, 201, 255, 0, 3};
  Check(IC::Decode(rgb, 6, 2, 1, 3, decoded, error) && decoded.At(0, 0).a == 255 && decoded.At(1, 0).b == 3, "RGB normalization");
  uint8_t bgra[] = {201, 23, 7, 101};
  Check(IC::Decode(bgra, 4, 1, 1, 4, decoded, error, true) && decoded.At(0, 0).r == 7 && decoded.At(0, 0).a == 101, "BGRA normalization preserves alpha");
  Check(!IC::Decode(rgb, 5, 2, 1, 3, decoded, error) && !decoded.IsValid(), "truncated buffer rejected and stale output cleared");
  Check(!IC::Decode(rgb, 6, 0, 1, 3, decoded, error), "zero dimension rejected");
  Check(!IC::Decode(rgb, 6, UINT32_MAX, UINT32_MAX, 4, decoded, error), "overflow dimensions rejected");
  Check(!IC::Decode(nullptr, 6, 2, 1, 3, decoded, error), "null buffer rejected");
  Check(!IC::Decode(rgb, 6, 2, 1, 2, decoded, error), "unsupported format rejected");
  auto           white = White(120, 60);
  const IC::Rect full{0, 0, 120, 60};
  Check(!IC::Compare(white, white, full).valid, "blank equality is not PASS");
  Check(!IC::Contains(white, {-1, 0, 1, 1}), "negative ROI rejected");
  Check(!IC::Contains(white, {0, 0, 121, 60}), "oversized ROI rejected");
  Check(!IC::Contains(white, {0, 0, 0, 60}), "empty ROI rejected");
  Check(!IC::Contains(white, {INT32_MAX, 0, INT32_MAX, 1}), "ROI overflow rejected");
  Check(!IC::Crop(white, {-1, 0, 4, 4}).IsValid(), "invalid crop rejected");
  auto a = white;
  Paint(a, {30, 10, 7, 26}, {0, 0, 0, 255});
  Paint(a, {37, 29, 19, 7}, {0, 0, 0, 255});
  Paint(a, {70, 13, 4, 13}, {0, 0, 0, 255});
  Check(IC::Compare(a, a, full).equal, "identical foreground passes");
  Check(IC::CountInk(a, full) == 367, "ink pixel count");
  const auto bounds = IC::ColorBounds(a, full, {0, 0, 0, 255});
  Check(bounds.x == 30 && bounds.y == 10 && bounds.width == 44 && bounds.height == 26, "color bounds");
  Check(IC::ColorBounds(a, full, {255, 0, 0, 255}).width == 0, "missing color has empty bounds");
  auto crop = IC::Crop(a, {30, 10, 7, 26});
  Check(crop.width == 7 && crop.height == 26 && IC::CountInk(crop, {0, 0, 7, 26}) == 182, "crop pixels and dimensions");
  auto b = a;
  Paint(b, {30, 10, 1, 10}, {255, 255, 255, 255});
  auto comparison = IC::Compare(a, b, full);
  Check(comparison.valid && !comparison.equal && comparison.changedPixels == 10 && comparison.changedFraction > 0.02, "small lost glyph not hidden by white background");
  Check(!IC::Compare(a, white, full).equal, "missing foreground fails");
  Check(!IC::Compare(a, White(121, 60), full).valid, "different sizes not resized to pass");
  IC::CompareOptions options;
  options.maximumChangedFraction = std::numeric_limits<double>::quiet_NaN();
  Check(!IC::Compare(a, a, full, options).valid, "NaN tolerance rejected");
  b = a;
  Paint(b, {30, 10, 7, 26}, {10, 10, 10, 255});
  Check(IC::Compare(a, b, full).equal, "bounded channel noise tolerated");
  Paint(b, {30, 10, 7, 26}, {40, 40, 40, 255});
  Check(!IC::Compare(a, b, full).equal, "real color change rejected");
  b = white;
  Paint(b, {35, 13, 7, 26}, {0, 0, 0, 255});
  Paint(b, {42, 32, 19, 7}, {0, 0, 0, 255});
  Paint(b, {75, 16, 4, 13}, {0, 0, 0, 255});
  const auto motion = IC::EstimateTranslation(a, b, full, 10, 6);
  Check(motion.valid && motion.dx == 5 && motion.dy == 3 && motion.mismatch == 0, "known two-axis translation");
  Check(!IC::Compare(a, b, full).equal, "comparison never aligns away offset bugs");
  Check(!IC::EstimateTranslation(white, white, full, 10, 6).valid, "blank motion rejected");
  Check(!IC::EstimateTranslation(a, b, full, 80, 6).valid, "invalid search range rejected");
  auto diff = IC::Difference(a, b);
  Check(diff.IsValid() && IC::CountColor(diff, full, {255, 0, 255, 255}) > 0, "difference highlights changes");
  Check(!IC::Difference(a, White(1, 1)).IsValid(), "invalid diff dimensions rejected");

#if TEXT_PIXEL_CHECKS
  char        directory[] = "/tmp/text-image-check-XXXXXX";
  const auto* created     = mkdtemp(directory);
  Check(created != nullptr, "temporary output directory");
  if(created)
  {
    const std::string path = std::string(created) + "/roundtrip.png";
    Check(IC::SavePng(a, path, error), "PNG write");
    IC::Image loaded;
    Check(IC::LoadPng(path, loaded, error) && loaded.rgba == a.rgba, "checker PNG read exact roundtrip");
    Check(!IC::LoadPng(std::string(created) + "/missing.png", loaded, error) && !loaded.IsValid(), "missing PNG clears stale pixels");
    png_image png{};
    png.version     = PNG_IMAGE_VERSION;
    const bool read = png_image_begin_read_from_file(&png, path.c_str()) != 0;
    Check(read, "PNG header read");
    if(read)
    {
      png.format = PNG_FORMAT_RGBA;
      auto back  = White(png.width, png.height);
      Check(png_image_finish_read(&png, nullptr, back.rgba.data(), 0, nullptr) != 0 && back.rgba == a.rgba, "PNG exact roundtrip");
    }
    png_image_free(&png);
    Check(!IC::SavePng(a, std::string(created) + "/missing/file.png", error), "artifact failure reported");
    std::remove(path.c_str());
    rmdir(created);
  }
#else
  Check(!IC::SavePng(a, "/tmp/disabled.png", error), "disabled PNG write cannot pass");
  Check(!IC::LoadPng("/tmp/disabled.png", decoded, error), "disabled PNG read cannot pass");
#endif
  std::printf("image-check: %d checks, %d failures (PNG=%s)\n", checks, failures, TEXT_PIXEL_CHECKS ? "ON" : "OFF");
  return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
