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

#include <dali-ui-foundation/internal/text/line-helper-functions.h>
#include <dali-ui-foundation/internal/text/line-run.h>
#include <dali-ui-foundation/internal/text/rendering/view-model.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-glyph-helper.h>
#include <dali-ui-foundation/internal/text/reveal/text-reveal-blur-preparation.h>
#include <dali/integration-api/pixel-data-integ.h>
#include <dali/public-api/adaptor-framework/pixel-buffer.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>

namespace DALI_NAMESPACE::Ui::Internal
{
uint32_t ResolveRevealBlurRadius(float radius)
{
  // Authored radii are clamped before UI scaling. Keep the shared Gaussian
  // shader's 200-pixel limit; unsupported scales use ordinary Reveal.
  return radius > 0.0f && radius <= 200.0f
           ? (std::max(4u, static_cast<uint32_t>(std::ceil(radius))) + 1u) & ~1u
           : 0u;
}

void AccumulateRuntimeRevealBlurCoverage(PixelData pixels, Rect<uint32_t>& coverage)
{
  const auto     width    = pixels.GetWidth();
  const auto     height   = pixels.GetHeight();
  const auto     format   = pixels.GetPixelFormat();
  const auto     buffer   = Dali::Integration::GetPixelDataBuffer(pixels);
  const uint32_t bytes    = Pixel::GetBytesPerPixel(format);
  const size_t   stride   = pixels.GetStrideBytes();
  const size_t   rowBytes = static_cast<size_t>(width) * bytes;
  if(!buffer.buffer || (format != Pixel::L8 && format != Pixel::RGBA8888) || stride < rowBytes ||
     (height > 0u && (static_cast<size_t>(height) - 1u) * stride + rowBytes > buffer.bufferSize))
  {
    coverage = Rect<uint32_t>(0u, 0u, width, height);
    return;
  }
  uint32_t left = width, top = height, right = 0u, bottom = 0u;
  uint64_t alphaMask = ~uint64_t{0u};
  if(format == Pixel::RGBA8888)
  {
    const uint8_t maskBytes[sizeof(alphaMask)] = {0u, 0u, 0u, 255u, 0u, 0u, 0u, 255u};
    std::memcpy(&alphaMask, maskBytes, sizeof(alphaMask));
  }
  const uint32_t pixelsPerWord = static_cast<uint32_t>(sizeof(alphaMask)) / bytes;
  for(uint32_t y = 0u; y < height; ++y)
  {
    const uint8_t* row = buffer.buffer + static_cast<size_t>(y) * stride;
    for(uint32_t x = 0u; x < width;)
    {
      const uint32_t count = std::min(pixelsPerWord, width - x);
      if(count == pixelsPerWord)
      {
        // Whole-control planes are mostly empty. memcpy permits unaligned
        // rows without aliasing assumptions; the byte mask is endian-neutral.
        uint64_t word;
        std::memcpy(&word, row + static_cast<size_t>(x) * bytes, sizeof(word));
        if((word & alphaMask) == 0u)
        {
          x += count;
          continue;
        }
      }
      for(uint32_t pixel = x; pixel < x + count; ++pixel)
      {
        if(row[static_cast<size_t>(pixel) * bytes + bytes - 1u] != 0u)
        {
          left   = std::min(left, pixel);
          top    = std::min(top, y);
          right  = std::max(right, pixel + 1u);
          bottom = y + 1u;
        }
      }
      x += count;
    }
  }
  if(right > left && bottom > top)
  {
    if(coverage.width > 0u && coverage.height > 0u)
    {
      left   = std::min(left, coverage.x);
      top    = std::min(top, coverage.y);
      right  = std::max(right, coverage.x + coverage.width);
      bottom = std::max(bottom, coverage.y + coverage.height);
    }
    coverage = Rect<uint32_t>(left, top, right - left, bottom - top);
  }
}

PixelData CropRuntimeRevealBlurPixels(PixelData pixels, const Rect<uint32_t>& rectangle)
{
  if(!pixels)
  {
    return {};
  }
  const auto width  = pixels.GetWidth();
  const auto height = pixels.GetHeight();
  const auto format = pixels.GetPixelFormat();
  if(rectangle.width == 0u || rectangle.height == 0u || rectangle.x >= width || rectangle.y >= height ||
     rectangle.width > width - rectangle.x || rectangle.height > height - rectangle.y ||
     (format != Pixel::L8 && format != Pixel::RGBA8888))
  {
    return {};
  }
  const auto   buffer   = Dali::Integration::GetPixelDataBuffer(pixels);
  const size_t bytes    = Pixel::GetBytesPerPixel(format);
  const size_t rowBytes = static_cast<size_t>(width) * bytes;
  const size_t stride   = pixels.GetStrideBytes() ? pixels.GetStrideBytes() : rowBytes;
  if(!buffer.buffer || stride < rowBytes || (static_cast<size_t>(height) - 1u) * stride + rowBytes > buffer.bufferSize)
  {
    return {};
  }
  if(rectangle == Rect<uint32_t>(0u, 0u, width, height))
  {
    return pixels;
  }
  PixelBuffer  cropped   = PixelBuffer::New(rectangle.width, rectangle.height, format);
  const size_t copyBytes = static_cast<size_t>(rectangle.width) * bytes;
  for(uint32_t row = 0u; row < rectangle.height; ++row)
  {
    std::memcpy(cropped.GetBuffer() + static_cast<size_t>(row) * copyBytes,
                buffer.buffer + static_cast<size_t>(rectangle.y + row) * stride + static_cast<size_t>(rectangle.x) * bytes,
                copyBytes);
  }
  return PixelBuffer::Convert(cropped);
}

bool PrepareRevealBlurSourceAtlases(PreparedRevealBlur& prepared)
{
  const uint32_t maximum = prepared.options.maxTextureSize;
  if(!prepared.options.perLine || maximum < 3u)
  {
    return true;
  }
  auto eligible = [maximum](const RevealBlurLineRaster& line)
  {
    return line.sequence.hasTextForeground && line.foreground && line.metadata &&
           line.atlasIndex == RevealBlurLineRaster::NO_ATLAS &&
           line.foreground.GetWidth() > 0u && line.foreground.GetHeight() > 0u &&
           line.foreground.GetWidth() <= maximum - 2u && line.foreground.GetHeight() <= maximum - 2u &&
           (line.foreground.GetPixelFormat() == Pixel::L8 || line.foreground.GetPixelFormat() == Pixel::RGBA8888);
  };
  for(size_t first = 0u; first < prepared.lines.size();)
  {
    const auto& initial = prepared.lines[first];
    if(!eligible(initial))
    {
      ++first;
      continue;
    }
    const auto format = initial.foreground.GetPixelFormat();
    const bool masked = bool(initial.mask);
    uint32_t   width  = 0u;
    uint32_t   height = 0u;
    size_t     end    = first;
    for(; end < prepared.lines.size(); ++end)
    {
      const auto& line = prepared.lines[end];
      if(!eligible(line) || line.foreground.GetPixelFormat() != format || bool(line.mask) != masked ||
         line.foreground.GetHeight() + 2u > maximum - height)
      {
        break;
      }
      width = std::max(width, line.foreground.GetWidth() + 2u);
      height += line.foreground.GetHeight() + 2u;
    }
    if(end - first < 2u)
    {
      first = end;
      continue;
    }
    RevealBlurSourceAtlas atlas;
    for(uint32_t plane = 0u; plane < (masked ? 3u : 2u); ++plane)
    {
      const auto   planeFormat = plane == 0u ? format : (plane == 1u ? Pixel::RGBA8888 : Pixel::L8);
      const size_t bytes       = Pixel::GetBytesPerPixel(planeFormat);
      if(static_cast<uint64_t>(width) * height > std::numeric_limits<size_t>::max() / bytes)
      {
        return false;
      }
      auto pixels = PixelBuffer::New(width, height, planeFormat);
      if(!pixels.GetBuffer())
      {
        return false;
      }
      std::memset(pixels.GetBuffer(), 0, static_cast<size_t>(width) * height * bytes);
      uint32_t y = 0u;
      for(size_t index = first; index < end; ++index)
      {
        const auto& line  = prepared.lines[index];
        const auto  input = plane == 0u ? line.foreground : (plane == 1u ? line.metadata : line.mask);
        const auto  w     = line.foreground.GetWidth();
        const auto  h     = line.foreground.GetHeight();
        if(!input || input.GetWidth() != w || input.GetHeight() != h || input.GetPixelFormat() != planeFormat)
        {
          return false;
        }
        const auto   buffer   = Dali::Integration::GetPixelDataBuffer(input);
        const size_t rowBytes = static_cast<size_t>(w) * bytes;
        const size_t stride   = input.GetStrideBytes() ? input.GetStrideBytes() : rowBytes;
        if(!buffer.buffer || stride < rowBytes || (static_cast<size_t>(h) - 1u) * stride + rowBytes > buffer.bufferSize)
        {
          return false;
        }
        for(uint32_t row = 0u; row < h + 2u; ++row)
        {
          const auto* source = buffer.buffer + static_cast<size_t>(std::clamp(row, 1u, h) - 1u) * stride;
          auto*       target = pixels.GetBuffer() + (static_cast<size_t>(y + row) * width) * bytes;
          // One replicated texel, including corners, preserves standalone
          // CLAMP_TO_EDGE for local [0,1] UV and non-mipmapped linear filtering.
          std::memcpy(target + bytes, source, rowBytes);
          std::memcpy(target, source, bytes);
          std::memcpy(target + (static_cast<size_t>(w) + 1u) * bytes, source + (static_cast<size_t>(w) - 1u) * bytes, bytes);
        }
        y += h + 2u;
      }
      auto& output = plane == 0u ? atlas.foreground : (plane == 1u ? atlas.metadata : atlas.mask);
      output       = PixelBuffer::Convert(pixels);
    }
    const auto atlasIndex = static_cast<uint32_t>(prepared.sourceAtlases.size());
    prepared.sourceAtlases.push_back(std::move(atlas));
    uint32_t y = 0u;
    for(size_t index = first; index < end; ++index)
    {
      auto& line          = prepared.lines[index];
      line.atlasIndex     = atlasIndex;
      line.atlasRectangle = Rect<uint32_t>(1u, y + 1u, line.foreground.GetWidth(), line.foreground.GetHeight());
      y += line.foreground.GetHeight() + 2u;
      line.foreground.Reset();
      line.metadata.Reset();
      line.mask.Reset();
    }
    first = end;
  }
  return true;
}

std::vector<RevealBlurSequence> BuildRevealBlurSequences(
  Ui::Text::Typesetter& typesetter, const Ui::Text::Internal::Reveal::Plan& plan)
{
  std::vector<RevealBlurSequence> result;
  const auto&                     model = *typesetter.GetViewModel();
  std::vector<bool>               covered(plan.glyphToUnit.size(), false);
  for(uint32_t lineIndex = 0u; lineIndex < model.GetNumberOfLines(); ++lineIndex)
  {
    const auto& line     = model.GetLines()[lineIndex];
    auto        contains = [](const Ui::Text::GlyphRun& run, uint32_t glyph)
    {
      return glyph >= run.glyphIndex && glyph - run.glyphIndex < run.numberOfGlyphs;
    };
    float start             = 1.0f;
    bool  hasUnits          = false;
    bool  hasTextForeground = false;
    // Visit only this line's range, retaining the union semantics of split
    // lines and the covered-glyph validation below. Do not rescan all text.
    const auto glyphCount = static_cast<uint32_t>(plan.glyphToUnit.size());
    auto       endOf      = [glyphCount](const Ui::Text::GlyphRun& run)
    {
      const auto first = std::min(run.glyphIndex, glyphCount);
      return first + std::min(run.numberOfGlyphs, glyphCount - first);
    };
    const auto firstGlyph = line.isSplitToTwoHalves ? std::min(line.glyphRun.glyphIndex, line.glyphRunSecondHalf.glyphIndex) : line.glyphRun.glyphIndex;
    const auto lastGlyph  = line.isSplitToTwoHalves ? std::max(endOf(line.glyphRun), endOf(line.glyphRunSecondHalf)) : endOf(line.glyphRun);
    for(uint32_t glyph = firstGlyph; glyph < lastGlyph; ++glyph)
    {
      const auto unit = plan.glyphToUnit[glyph];
      if(unit == Ui::Text::Internal::Reveal::NO_UNIT ||
         !(contains(line.glyphRun, glyph) || (line.isSplitToTwoHalves && contains(line.glyphRunSecondHalf, glyph))))
      {
        continue;
      }
      if(unit >= plan.unitStart.size() || covered[glyph])
      {
        return {};
      }
      covered[glyph] = true;
      hasUnits       = true;
      start          = std::min(start, plan.unitStart[unit]);
      hasTextForeground |= !Ui::Text::IsSyntheticReplacementGlyph(model.GetGlyphs()[glyph]);
    }
    if(!hasUnits)
    {
      continue;
    }
    result.push_back({lineIndex, start, hasTextForeground});
  }
  for(uint32_t glyph = 0u; glyph < covered.size(); ++glyph)
  {
    if(plan.glyphToUnit[glyph] != Ui::Text::Internal::Reveal::NO_UNIT && !covered[glyph])
    {
      return {};
    }
  }
  return result;
}

float GetRuntimeRevealEndProgress(const Ui::Text::Internal::Reveal::Plan& plan)
{
  float end = 0.0f;
  for(size_t unit = 0u; unit < plan.unitStart.size(); ++unit)
  {
    const float span = unit < plan.pixelUnitTiming.size() ? plan.pixelUnitTiming[unit].progressionSpan : 0.0f;
    end              = std::max(end, plan.unitStart[unit] + span + plan.fadeDuration);
  }
  return std::clamp(end, 0.0f, 1.0f);
}

Vector2 GetRevealBlurPlanTiming(Ui::Text::Typesetter& typesetter, const Ui::Text::Internal::Reveal::Plan& plan,
                                const std::vector<RevealBlurSequence>& sequences)
{
  float lastStart = -1.0f;
  if(sequences.empty())
  {
    const auto* glyphs = typesetter.GetViewModel()->GetGlyphs();
    for(size_t glyph = 0u; glyph < plan.glyphToUnit.size(); ++glyph)
    {
      if(plan.glyphToUnit[glyph] != Ui::Text::Internal::Reveal::NO_UNIT && !Ui::Text::IsSyntheticReplacementGlyph(glyphs[glyph]))
      {
        lastStart = 0.0f;
        break;
      }
    }
  }
  else
  {
    for(const auto& sequence : sequences)
    {
      if(sequence.hasTextForeground)
      {
        lastStart = std::max(lastStart, sequence.start);
      }
    }
  }
  return Vector2(GetRuntimeRevealEndProgress(plan), lastStart);
}

std::shared_ptr<const PreparedRevealBlur> PrepareRevealBlur(
  Ui::Text::Typesetter& typesetter, Ui::Text::Internal::Reveal::Plan plan,
  const Vector<Ui::Text::ReplacementRevealTiming>& timings,
  const RevealBlurPreparationOptions&              options,
  const Vector<Ui::Text::ReplacementPlacement>*    placements)
{
  const auto   size           = options.rasterSize;
  const auto   control        = options.controlSize;
  const double halo           = 2.0 * (static_cast<double>(options.radius) + 2.0);
  const double maxTextureSize = options.maxTextureSize;
  if(options.radius == 0u || options.maxTextureSize == 0u ||
     !(size.x > 0.0f && size.y > 0.0f && control.x > 0.0f && control.y > 0.0f) ||
     !std::isfinite(size.x + size.y + control.x + control.y) ||
     size.x > maxTextureSize || size.y > maxTextureSize ||
     std::ceil(control.x) + halo >= maxTextureSize ||
     std::ceil(control.y) + halo >= maxTextureSize)
  {
    return {};
  }
  const double rasterWidth       = std::ceil(static_cast<double>(size.x));
  const double rasterHeight      = std::ceil(static_cast<double>(size.y));
  const double targetWidth       = std::ceil(control.x) + halo;
  const double targetHeight      = std::ceil(control.y) + halo;
  const double targetPixels      = targetWidth * targetHeight;
  const double scaleY            = std::max(1.0, static_cast<double>(control.y) / rasterHeight);
  double       imageTargetPixels = 0.0;
  auto         result            = std::make_shared<PreparedRevealBlur>();
  result->options                = options;
  result->timings                = timings;
  if(placements)
  {
    for(const auto& placement : *placements)
    {
      // Replacement timings contain only visible, supported ImageSpan units.
      const bool scheduled = std::any_of(timings.Begin(), timings.End(), [&](const auto& timing)
      {
        return timing.occurrenceIdentity == placement.occurrenceIdentity;
      });
      if(!scheduled || !placement.visible || placement.elided)
      {
        continue;
      }
      const float left   = std::clamp(std::floor(placement.position.x), 0.0f, size.x);
      const float top    = std::clamp(std::floor(placement.position.y), 0.0f, size.y);
      const float right  = std::clamp(std::ceil(placement.position.x + placement.size.x), 0.0f, size.x);
      const float bottom = std::clamp(std::ceil(placement.position.y + placement.size.y), 0.0f, size.y);
      if(std::isfinite(left + top + right + bottom) && right > left && bottom > top)
      {
        if(options.perLine)
        {
          // Reuse the existing placement visit; no extra image/line index.
          // Overlap with text is deliberately counted conservatively.
          imageTargetPixels += targetWidth * std::min(targetHeight, (bottom - top) * scaleY + halo + 6.0);
        }
        result->images.push_back({placement.occurrenceIdentity, placement.lineIndex,
                                  Rect<uint32_t>(static_cast<uint32_t>(left), static_cast<uint32_t>(top),
                                                 static_cast<uint32_t>(right - left), static_cast<uint32_t>(bottom - top))});
      }
    }
  }
  auto sequences = options.perLine ? BuildRevealBlurSequences(typesetter, plan)
                                   : std::vector<RevealBlurSequence>{};
  if(options.perLine)
  {
    sequences.erase(std::remove_if(sequences.begin(), sequences.end(), [&](const RevealBlurSequence& sequence)
    {
      return !sequence.hasTextForeground && std::none_of(result->images.begin(), result->images.end(), [&](const auto& image)
      {
        return image.lineIndex == sequence.lineIndex;
      });
    }),
                    sequences.end());
    if(sequences.empty())
    {
      return {};
    }
  }
  // Conservative per-publication admission estimate, not an actual CPU/GPU
  // memory quota or process-wide budget. Keep one full-raster allowance
  // for ordinary/global planes and transient fallback work. Retained line
  // planes use generous metric bands, not N copies of the entire Label.
  // No bitmap bounds, coverage scan or duplicate page packing is needed here.
  constexpr double MAX_PREPARATION_BYTES = 256.0 * 1024.0 * 1024.0;
  const auto&      model                 = *typesetter.GetViewModel();
  double           bytes                 = 12.0 * rasterWidth * rasterHeight + 15.0 * imageTargetPixels +
                 (options.decorations ? 4.0 * targetPixels : 0.0);
  if(!std::isfinite(bytes) || bytes > MAX_PREPARATION_BYTES)
  {
    return {};
  }
  if(options.perLine)
  {
    for(const auto& sequence : sequences)
    {
      const auto&  line         = model.GetLines()[sequence.lineIndex];
      const double metricHeight = std::abs(static_cast<double>(line.ascender)) + std::abs(static_cast<double>(line.descender));
      if(!std::isfinite(metricHeight))
      {
        return {};
      }
      // Allow overhang/overlap even with compressed line spacing, plus the
      // source/metadata filtering guard. Actual bitmap coverage is read later.
      const double bandHeight = std::min(rasterHeight, 2.0 * std::ceil(metricHeight) + 6.0);
      if(sequence.hasTextForeground)
      {
        bytes += 12.0 * rasterWidth * bandHeight;
      }
      // Three RGBA targets plus 25% page occupancy slack. Deliberately take
      // no credit for A8, quarter filtering or shared scratch textures.
      bytes += 15.0 * targetWidth * std::min(targetHeight, bandHeight * scaleY + halo + 6.0);
      if(bytes > MAX_PREPARATION_BYTES)
      {
        return {};
      }
    }
  }
  else if(bytes + 12.0 * targetPixels > MAX_PREPARATION_BYTES)
  {
    return {};
  }
  result->planTiming = GetRevealBlurPlanTiming(typesetter, plan, sequences);
  if(!result->images.empty())
  {
    if(!options.perLine)
    {
      result->planTiming.y = 0.0f;
    }
    else
    {
      for(const auto& sequence : sequences)
      {
        result->planTiming.y = std::max(result->planTiming.y, sequence.start);
      }
    }
  }
  if(result->planTiming.y < 0.0f)
  {
    return {};
  }
  const double blur = static_cast<double>(options.durationRatio) * plan.sequenceDuration;
  const double end  = std::max(static_cast<double>(result->planTiming.x),
                               static_cast<double>(result->planTiming.y) + blur);
  if(!(end > 0.0) || !std::isfinite(end))
  {
    return {};
  }
  result->blurDuration = static_cast<float>(blur / end);
  if(!(result->blurDuration > 0.0f))
  {
    return {};
  }
  auto normalize = [end](float value)
  {
    return static_cast<float>(static_cast<double>(value) / end);
  };
  for(auto& start : plan.unitStart)
  {
    start = normalize(start);
  }
  for(auto& pixel : plan.pixelUnitTiming)
  {
    pixel.progressionSpan = normalize(pixel.progressionSpan);
  }
  plan.fadeDuration     = normalize(plan.fadeDuration);
  plan.sequenceDuration = static_cast<float>(std::min(static_cast<double>(plan.sequenceDuration) / end,
                                                      static_cast<double>(std::numeric_limits<float>::max())));
  for(auto& sequence : sequences)
  {
    sequence.start = normalize(sequence.start);
  }
  for(auto& timing : result->timings)
  {
    timing.start           = normalize(timing.start);
    timing.fadeDuration    = normalize(timing.fadeDuration);
    timing.progressionSpan = normalize(timing.progressionSpan);
  }
  result->planTiming = Vector2(normalize(result->planTiming.x), normalize(result->planTiming.y));

  using Plane                 = Ui::Text::Typesetter::RuntimeBlurPlane;
  const uint32_t       width  = static_cast<uint32_t>(size.x);
  const uint32_t       height = static_cast<uint32_t>(size.y);
  const Rect<uint32_t> fullSource(0u, 0u, width, height);
  result->lines.reserve(sequences.size());
  // Baselines belong to this preparation only. Preserve separate integer
  // truncation of pre/post offsets, including compressed line heights.
  std::vector<int32_t> baselines(options.perLine ? model.GetNumberOfLines() : 0u);
  int32_t              baseline = 0;
  if(model.GetVerticalAlignment() == Ui::Text::Alignment::CENTER)
  {
    baseline = static_cast<int32_t>(std::round(0.5f * (size.height - model.GetLayoutSize().height)));
  }
  else if(model.GetVerticalAlignment() == Ui::Text::Alignment::END)
  {
    baseline = static_cast<int32_t>(size.height - model.GetLayoutSize().height);
  }
  for(size_t index = 0u; index < baselines.size(); ++index)
  {
    const auto& line = model.GetLines()[index];
    baseline += static_cast<int32_t>(line.ascender + Ui::Text::GetPreOffsetVerticalLineAlignment(line, model.GetVerticalLineAlignment()));
    baselines[index] = baseline;
    baseline += static_cast<int32_t>(-line.descender + Ui::Text::GetPostOffsetVerticalLineAlignment(line, model.GetVerticalLineAlignment()));
  }
  for(const auto& geometry : sequences)
  {
    RevealBlurLineRaster line;
    line.sequence  = geometry;
    auto& sequence = line.sequence;
    if(!sequence.hasTextForeground)
    {
      // Image-only sequences need a capture slot, not a full transparent text
      // raster and metadata plane. Runtime adds only the resolved image draws.
      result->lines.push_back(std::move(line));
      continue;
    }
    auto sourceBounds = typesetter.GetRuntimeBlurLineRasterBounds(size, sequence.lineIndex, &baselines[sequence.lineIndex]);
    auto render       = [&](Plane plane, Pixel::Format format, const Rect<uint32_t>& bounds)
    {
      return typesetter.RenderRuntimeBlurLine(size, sequence.lineIndex, plane, format, plan,
                                              bounds.y, bounds.height, bounds.x, bounds.width);
    };
    auto renderForeground = [&]()
    {
      line.foreground = render(options.gradientMixed ? Plane::GRADIENT_PRESERVED : Plane::TEXT,
                               options.foregroundFormat, sourceBounds);
      if(!line.foreground)
      {
        return false;
      }
      AccumulateRuntimeRevealBlurCoverage(line.foreground, sequence.coverage);
      if(options.gradientMixed || options.colorMask)
      {
        line.mask = render(options.gradientMixed ? Plane::GRADIENT_MASK : Plane::COLOR_MASK, Pixel::L8, sourceBounds);
        if(!line.mask)
        {
          return false;
        }
        AccumulateRuntimeRevealBlurCoverage(line.mask, sequence.coverage);
      }
      return true;
    };
    if(!renderForeground())
    {
      return {};
    }
    if(sequence.coverage.width == 0u && sourceBounds.height != fullSource.height)
    {
      line.foreground.Reset();
      line.mask.Reset();
      sourceBounds = fullSource;
      if(!renderForeground())
      {
        return {};
      }
    }
    if(sequence.coverage.height > 0u)
    {
      sequence.coverage.y += sourceBounds.y;
    }
    auto        rectangle = fullSource;
    const auto& coverage  = sequence.coverage;
    if(coverage.width > 0u && coverage.height > 0u)
    {
      const uint32_t left   = coverage.x > 2u ? coverage.x - 2u : 0u;
      const uint32_t top    = coverage.y > 2u ? coverage.y - 2u : 0u;
      const uint32_t right  = std::min(width, coverage.x + coverage.width + 2u);
      const uint32_t bottom = std::min(height, coverage.y + coverage.height + 2u);
      rectangle             = Rect<uint32_t>(left, top, right - left, bottom - top);
    }
    // Preserve the ownership halo of the former full metadata raster.
    const uint32_t top    = rectangle.y > 0u ? rectangle.y - 1u : 0u;
    const uint32_t bottom = std::min(height, rectangle.y + rectangle.height + 1u);
    const uint32_t left   = rectangle.x > 0u ? rectangle.x - 1u : 0u;
    const uint32_t right  = std::min(width, rectangle.x + rectangle.width + 1u);
    line.metadata         = render(Plane::REVEAL_METADATA, Pixel::RGBA8888,
                                   Rect<uint32_t>(left, top, right - left, bottom - top));
    if(!line.metadata)
    {
      return {};
    }
    const Rect<uint32_t> crop(rectangle.x - sourceBounds.x, rectangle.y - sourceBounds.y,
                              rectangle.width, rectangle.height);
    line.foreground = CropRuntimeRevealBlurPixels(line.foreground, crop);
    if(!line.foreground)
    {
      return {};
    }
    if(line.mask)
    {
      line.mask = CropRuntimeRevealBlurPixels(line.mask, crop);
      if(!line.mask)
      {
        return {};
      }
    }
    line.metadata = CropRuntimeRevealBlurPixels(line.metadata,
                                                Rect<uint32_t>(rectangle.x - left, rectangle.y - top, rectangle.width, rectangle.height));
    if(!line.metadata)
    {
      return {};
    }
    sequence.textureRect = Vector4(static_cast<float>(rectangle.x) / static_cast<float>(width), static_cast<float>(rectangle.y) / static_cast<float>(height),
                                   static_cast<float>(rectangle.width) / static_cast<float>(width), static_cast<float>(rectangle.height) / static_cast<float>(height));
    result->lines.push_back(std::move(line));
  }
  result->metadata = typesetter.RenderTextRevealMetadata(size, options.textDirection, plan, result->fadeDuration);
  if(!result->metadata || !PrepareRevealBlurSourceAtlases(*result))
  {
    return {};
  }
  return result;
}
} //namespace DALI_NAMESPACE::Ui::Internal
