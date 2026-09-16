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
 */

#include <dali-ui-foundation/internal/text/async-text/async-text-loader.h>
#include <dali-ui-foundation/internal/text/marquee/marquee-start-geometry.h>
#include <dali-ui-test-suite-utils.h>
#include <dali/integration-api/pixel-data-integ.h>
#include <algorithm>
#include <cstring>

using namespace Dali;

namespace
{
namespace Text = Dali::Ui::Text;

Text::AsyncTextParameters Parameters(Text::Alignment alignment)
{
  Text::AsyncTextParameters parameters;
  parameters.text                = "Async marquee coordinate regression";
  parameters.fontSize            = 20.0f;
  parameters.textWidth           = 200.0f;
  parameters.textHeight          = 80.0f;
  parameters.maxTextureSize      = 2048;
  parameters.requestType         = Ui::Integration::Text::Async::RENDER_FIXED_SIZE;
  parameters.isMarqueeEnabled    = true;
  parameters.marqueeGap          = 40;
  parameters.marqueeOrientation  = Text::MarqueeOrientation::HORIZONTAL;
  parameters.horizontalAlignment = alignment;
  return parameters;
}

bool HasInk(PixelData pixels)
{
  if(!pixels)
  {
    return false;
  }
  const uint8_t* bytes = Integration::GetPixelDataBuffer(pixels).buffer;
  const uint32_t stride = Pixel::GetBytesPerPixel(pixels.GetPixelFormat());
  for(uint32_t index = stride - 1u; index < pixels.GetWidth() * pixels.GetHeight() * stride; index += stride)
  {
    if(bytes[index] != 0u)
    {
      return true;
    }
  }
  return false;
}

void ExpectSamePixels(PixelData actual, PixelData expected)
{
  DALI_TEST_CHECK(actual && expected);
  if(!actual || !expected)
  {
    return;
  }
  DALI_TEST_EQUALS(actual.GetWidth(), expected.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(actual.GetHeight(), expected.GetHeight(), TEST_LOCATION);
  DALI_TEST_EQUALS(actual.GetPixelFormat(), expected.GetPixelFormat(), TEST_LOCATION);
  const size_t actualSize = static_cast<size_t>(actual.GetWidth()) * actual.GetHeight() * Pixel::GetBytesPerPixel(actual.GetPixelFormat());
  const size_t expectedSize = static_cast<size_t>(expected.GetWidth()) * expected.GetHeight() * Pixel::GetBytesPerPixel(expected.GetPixelFormat());
  DALI_TEST_CHECK(actualSize == expectedSize && std::memcmp(Integration::GetPixelDataBuffer(actual).buffer, Integration::GetPixelDataBuffer(expected).buffer, actualSize) == 0);
}
void ExpectSameCoverage(PixelData actual, PixelData expected)
{
  DALI_TEST_CHECK(actual && expected);
  if(!actual || !expected)
  {
    return;
  }
  DALI_TEST_EQUALS(actual.GetWidth(), expected.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(actual.GetHeight(), expected.GetHeight(), TEST_LOCATION);
  const auto a = Integration::GetPixelDataBuffer(actual);
  const auto b = Integration::GetPixelDataBuffer(expected);
  const uint32_t ap = Pixel::GetBytesPerPixel(actual.GetPixelFormat());
  const uint32_t bp = Pixel::GetBytesPerPixel(expected.GetPixelFormat());
  for(uint32_t y = 0u; y < actual.GetHeight(); ++y)
  {
    for(uint32_t x = 0u; x < actual.GetWidth(); ++x)
    {
      DALI_TEST_EQUALS(a.buffer[y * actual.GetStrideBytes() + x * ap + ap - 1u],
                        b.buffer[y * expected.GetStrideBytes() + x * bp + bp - 1u], TEST_LOCATION);
    }
  }
}

void SetStyle(Text::AsyncTextParameters& p, int style)
{
  p.isTextGradientRequested = true;
  p.isUnderlineEnabled = style == 1 || style == 4;
  p.underlineHeight = 1.0f;
  p.isShadowEnabled = style == 2 || style == 4;
  p.shadowOffset = Vector2(2.0f, 2.0f);
  p.isOutlineEnabled = style == 3 || style == 4;
  p.outlineWidth = p.isOutlineEnabled ? 1u : 0u;
  p.isStrikethroughEnabled = style == 5;
  p.strikethroughHeight = 1.0f;
  p.isTextBackgroundEnabled = style == 6;
  p.textBackgroundColor = Color::BLUE;
  if(style == 7 || style == 8)
  {
    p.hasStyledTextStyleSnapshot = true;
    if(style == 7)
    {
      auto gradient = std::make_shared<Text::Internal::StyledTextGradientSnapshotData>();
      Text::Internal::StyledTextGradientRunSnapshot run;
      run.numberOfCharacters = 3u;
      run.style.enabled = true;
      run.style.linearStart = Vector2::ZERO;
      run.style.linearEnd = Vector2::ONE;
      run.style.stops.PushBack({0.0f, Color::RED});
      run.style.stops.PushBack({1.0f, Color::BLUE});
      run.boundsMode = Text::GradientSpan::BoundsMode::CONTENT_BOUND;
      gradient->runs.push_back(run);
      p.styledTextStyleSnapshot.gradientData = gradient;
    }
    else
    {
      p.styledTextStyleSnapshot.foregroundColorRuns.push_back({0u, 3u, Color::RED, 0u});
      p.styledTextStyleSnapshot.backgroundColorRuns.push_back({0u, 3u, Color::BLUE, 0u});
    }
  }
}

} // namespace

int UtcDaliAsyncMarqueeCoordinatesNaturalMeasurementP(void)
{
  TestApplication application;
  Text::AsyncTextLoader loader = Text::AsyncTextLoader::New();
  auto start = Parameters(Text::Alignment::START);
  const auto reference = loader.RenderMarquee(start, false, Size::ZERO);
  DALI_TEST_CHECK(HasInk(reference.textPixelData));
  DALI_TEST_CHECK(HasInk(reference.marqueePixelData));
  for(auto alignment : {Text::Alignment::CENTER, Text::Alignment::END})
  {
    auto parameters = Parameters(alignment);
    const auto result = loader.RenderMarquee(parameters, false, Size::ZERO);
    DALI_TEST_CHECK(HasInk(result.textPixelData));
    DALI_TEST_CHECK(HasInk(result.marqueePixelData));
    ExpectSamePixels(result.textPixelData, reference.textPixelData);
    ExpectSamePixels(result.marqueePixelData, reference.marqueePixelData);
    DALI_TEST_EQUALS(parameters.textWidth, 200.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(parameters.textHeight, 80.0f, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliAsyncMarqueePlainFittingAndOverflowP(void)
{
  TestApplication application;
  Text::AsyncTextLoader loader = Text::AsyncTextLoader::New();
  // Alternate fit/overflow and END/START on the same worker, without SetStyle().
  for(float width : {500.0f, 40.0f, 500.0f})
  {
    auto start = Parameters(Text::Alignment::START);
    start.text = "Short";
    start.textWidth = width;
    const auto reference = loader.RenderMarquee(start, false, Size::ZERO);
    DALI_TEST_EQUALS(reference.isMarqueeContentOverflow, width == 40.0f, TEST_LOCATION);
    for(auto alignment : {Text::Alignment::START, Text::Alignment::CENTER, Text::Alignment::END, Text::Alignment::START})
    {
      auto parameters = start;
      parameters.horizontalAlignment = alignment;
      DALI_TEST_CHECK(!parameters.isTextGradientRequested && !parameters.isUnderlineEnabled &&
                      !parameters.isShadowEnabled && !parameters.isOutlineEnabled &&
                      !parameters.isTextBackgroundEnabled && !parameters.hasStyledTextStyleSnapshot);
      const auto result = loader.RenderMarquee(parameters, false, Size::ZERO);
      DALI_TEST_CHECK(HasInk(result.textPixelData) && HasInk(result.marqueePixelData));
      ExpectSamePixels(result.textPixelData, reference.textPixelData);
      ExpectSamePixels(result.marqueePixelData, reference.marqueePixelData);
      ExpectSameCoverage(result.textPixelData, result.marqueePixelData);
      DALI_TEST_CHECK(!result.stylePixelData && !result.overlayStylePixelData &&
                      !result.textGradientMaskPixelData && !result.textGradientPreservedPixelData);
      // The texture stays local. The scroller shader supplies the final fit alignment.
      const float factor = Text::ResolveHorizontalMarqueeAlignment(result.isMarqueeContentOverflow,
                                                                    result.isTextDirectionRTL, alignment);
      const float origin = Text::ResolveLegacyHorizontalMarqueeViewportOrigin(factor,
                             result.size.width, result.controlSize.width, result.marqueeWrapGap);
      const float freeSpace = result.controlSize.width - (result.size.width - result.marqueeWrapGap);
      const float expected = result.isMarqueeContentOverflow || alignment == Text::Alignment::START ? 0.0f
                               : (alignment == Text::Alignment::CENTER ? freeSpace * 0.5f : freeSpace);
      DALI_TEST_EQUALS(-origin, expected, 0.001f, TEST_LOCATION);
    }
  }
  END_TEST;
}

int UtcDaliAsyncMarqueeCoordinatesStylesAndBidiP(void)
{
  TestApplication application;
  Text::AsyncTextLoader loader = Text::AsyncTextLoader::New();
  for(const char* text : {"Async marquee", "אבגדה וזחטי", "English אבג trailing"})
  {
    for(float width : {60.0f, 400.0f})
    {
      for(auto alignment : {Text::Alignment::START, Text::Alignment::CENTER, Text::Alignment::END})
      {
        for(int style = 0; style <= 8; ++style)
        {
          auto p = Parameters(alignment);
          p.text = text;
          p.textWidth = width;
          SetStyle(p, style);
          const auto result = loader.RenderMarquee(p, false, Size::ZERO);
          DALI_TEST_CHECK(HasInk(result.textPixelData));
          DALI_TEST_CHECK(HasInk(result.marqueePixelData));
          if(result.marqueeFillPixelData)
          {
            ExpectSameCoverage(result.textPixelData, result.marqueeFillPixelData);
          }
          if(result.marqueeStylePixelData)
          {
            DALI_TEST_CHECK(HasInk(result.stylePixelData));
            ExpectSamePixels(result.stylePixelData, result.marqueeStylePixelData);
          }
          if(result.marqueeOverlayStylePixelData)
          {
            DALI_TEST_CHECK(HasInk(result.overlayStylePixelData));
            ExpectSamePixels(result.overlayStylePixelData, result.marqueeOverlayStylePixelData);
          }
          if(style == 0)
          {
            ExpectSameCoverage(result.textPixelData, result.marqueePixelData);
          }
          if(style == 7 || style == 8)
          {
            DALI_TEST_CHECK(HasInk(result.textGradientPreservedPixelData));
            DALI_TEST_CHECK(HasInk(result.textGradientMaskPixelData));
          }
        }
      }
    }
  }
  END_TEST;
}

int UtcDaliAsyncMarqueeCoordinatesCacheAndTransitionsP(void)
{
  TestApplication application;
  Text::AsyncTextLoader loader = Text::AsyncTextLoader::New();
  for(auto alignment : {Text::Alignment::START, Text::Alignment::CENTER, Text::Alignment::END,
                        Text::Alignment::CENTER, Text::Alignment::START})
  {
    auto p = Parameters(alignment);
    const auto naturalSize = loader.ComputeNaturalSize(p);
    const auto cached = loader.RenderMarquee(p, true, naturalSize);
    const auto uncached = loader.RenderMarquee(p, false, Size::ZERO);
    ExpectSamePixels(cached.textPixelData, uncached.textPixelData);
    ExpectSamePixels(cached.marqueePixelData, uncached.marqueePixelData);
    loader.ClearModule();
    loader.SetLocale("ar");
    const auto cleared = loader.RenderMarquee(p, false, Size::ZERO);
    ExpectSamePixels(cleared.marqueePixelData, uncached.marqueePixelData);
    // Reuse the worker for normal ellipsis, a new font/size/text and marquee restart.
    p.isMarqueeEnabled = false;
    p.textWidth = 120.0f;
    const auto normal = loader.RenderText(p, false, Size::ZERO);
    DALI_TEST_CHECK(HasInk(normal.textPixelData));
    DALI_TEST_CHECK(!normal.marqueePixelData);
    p.text = "Changed content";
    p.fontSize = 24.0f;
    p.textWidth = 180.0f;
    p.isMarqueeEnabled = true;
    const auto restarted = loader.RenderMarquee(p, false, Size::ZERO);
    DALI_TEST_CHECK(HasInk(restarted.textPixelData));
    DALI_TEST_CHECK(HasInk(restarted.marqueePixelData));
    // Vertical marquee must still apply normal horizontal alignment.
    p.marqueeOrientation = Text::MarqueeOrientation::VERTICAL;
    p.isMultiLine = true;
    p.text = "two lines\nmore text";
    const auto vertical = loader.RenderMarquee(p, false, Size::ZERO);
    DALI_TEST_CHECK(HasInk(vertical.textPixelData));
    DALI_TEST_CHECK(HasInk(vertical.marqueePixelData));
    ExpectSameCoverage(vertical.textPixelData, vertical.marqueePixelData);
  }
  END_TEST;
}
