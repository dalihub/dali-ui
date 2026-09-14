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

#include <dali-ui-foundation/integration-api/utility/npatch-utilities.h>
#include <dali-ui-test-suite-utils.h>
#include <cstring>

using namespace Dali;
using namespace Dali::Ui::Integration;

void utc_dali_npatch_utilities_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_npatch_utilities_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliNPatchUtilitiesPixelFormatsP(void)
{
  struct Expected
  {
    Pixel::Format format;
    int offset;
    int mask;
  };
  const Expected cases[] = {
    {Pixel::A8, 0, 0}, {Pixel::L8, 0, 0}, {Pixel::LA88, 0, 0},
    {Pixel::RGB888, 0, 0xff}, {Pixel::RGB8888, 0, 0xff}, {Pixel::RGBA8888, 0, 0xff},
    {Pixel::BGR8888, 2, 0xff}, {Pixel::BGRA8888, 2, 0xff}, {Pixel::RGB565, 0, 0xf8},
    {Pixel::BGR565, 1, 0x1f}, {Pixel::RGBA4444, 0, 0xf0}, {Pixel::BGRA4444, 1, 0xf0},
    {Pixel::RGBA5551, 0, 0xf8}, {Pixel::BGRA5551, 1, 0x1e}, {Pixel::INVALID, 0, 0},
    {Pixel::RGB16F, 0, 0}};

  for(const Expected& expected : cases)
  {
    int offset = -1;
    int mask = -1;
    NPatchUtility::GetRedOffsetAndMask(expected.format, offset, mask);
    DALI_TEST_EQUALS(offset, expected.offset, TEST_LOCATION);
    DALI_TEST_EQUALS(mask, expected.mask, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliNPatchUtilitiesParseBordersP(void)
{
  PixelBuffer buffer = PixelBuffer::New(7u, 7u, Pixel::RGBA8888);
  uint8_t* pixels = buffer.GetBuffer();
  const uint32_t stride = buffer.GetStrideBytes() ? buffer.GetStrideBytes() : 28u;
  std::memset(pixels, 0, stride * buffer.GetHeight());
  pixels[1u * 4u + 3u] = 0xff;
  pixels[2u * 4u + 3u] = 0xff;
  pixels[4u * 4u + 3u] = 0xff;
  pixels[1u * stride + 3u] = 0xff;
  pixels[3u * stride + 3u] = 0xff;
  pixels[4u * stride + 3u] = 0xff;

  NPatchUtility::StretchRanges xRanges;
  NPatchUtility::StretchRanges yRanges;
  DALI_TEST_CHECK(NPatchUtility::ParseBorders(buffer, xRanges, yRanges));
  DALI_TEST_EQUALS(xRanges.Size(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(xRanges[0], Uint16Pair(0u, 2u), TEST_LOCATION);
  DALI_TEST_EQUALS(xRanges[1], Uint16Pair(3u, 4u), TEST_LOCATION);
  DALI_TEST_EQUALS(yRanges.Size(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(yRanges[0], Uint16Pair(0u, 1u), TEST_LOCATION);
  DALI_TEST_EQUALS(yRanges[1], Uint16Pair(2u, 4u), TEST_LOCATION);

  PixelBuffer noMarks = PixelBuffer::New(4u, 5u, Pixel::RGBA8888);
  std::memset(noMarks.GetBuffer(), 0, 4u * 5u * 4u);
  DALI_TEST_CHECK(NPatchUtility::ParseBorders(noMarks, xRanges, yRanges));
  DALI_TEST_EQUALS(xRanges[0], Uint16Pair(0u, 2u), TEST_LOCATION);
  DALI_TEST_EQUALS(yRanges[0], Uint16Pair(0u, 3u), TEST_LOCATION);

  PixelBuffer tooSmall = PixelBuffer::New(2u, 4u, Pixel::RGB888);
  DALI_TEST_CHECK(!NPatchUtility::ParseBorders(tooSmall, xRanges, yRanges));
  DALI_TEST_EQUALS(xRanges.Size(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(yRanges.Size(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliNPatchUtilitiesUrlsAndRangesP(void)
{
  DALI_TEST_CHECK(NPatchUtility::IsNinePatchUrl("image.9.png"));
  DALI_TEST_CHECK(NPatchUtility::IsNinePatchUrl("image.#.png"));
  DALI_TEST_CHECK(!NPatchUtility::IsNinePatchUrl("image.png"));
  DALI_TEST_CHECK(!NPatchUtility::IsNinePatchUrl("image.8.png"));
  DALI_TEST_CHECK(!NPatchUtility::IsNinePatchUrl("image.9-png"));
  DALI_TEST_CHECK(!NPatchUtility::IsNinePatchUrl(""));

  DALI_TEST_EQUALS(NPatchUtility::GetValidStrechPointFromBorder(100u, 10u, 20u), Uint16Pair(10u, 80u), TEST_LOCATION);
  DALI_TEST_EQUALS(NPatchUtility::GetValidStrechPointFromBorder(100u, 80u, 70u), Uint16Pair(53u, 53u), TEST_LOCATION);
  DALI_TEST_EQUALS(NPatchUtility::GetValidStrechPointFromBorder(100000u, 100000u, 100000u), Uint16Pair(32767u, 32767u), TEST_LOCATION);
  END_TEST;
}
