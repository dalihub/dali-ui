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

#include <dali-ui-test-suite-utils.h>
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/integration-api/pixel-data-integ.h>
#include <algorithm>
#include <array>
#include <cstdlib>

using namespace Dali;

void utc_dali_text_abstraction_ownership_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_text_abstraction_ownership_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliTextAbstractionGlyphBufferOwnershipP(void)
{
  UiTestApplication           application;
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();

  // Exercise RAII and explicit release under memory checking. The latter is
  // used by Typesetter when reusing its per-glyph state across a raster.
  for(bool explicitRelease : {false, true})
  {
    TextAbstraction::GlyphBufferData bitmap;
    fontClient.CreateBitmap(1u, 1u, false, false, bitmap, 0);
    DALI_TEST_CHECK(bitmap.isBufferOwned && bitmap.buffer);
    if(explicitRelease)
    {
      free(bitmap.buffer);
      bitmap.isBufferOwned = false;
    }
  }

  // A borrowed buffer must remain valid after the wrapper is destroyed.
  std::array<uint8_t, 4u> borrowed{{1u, 2u, 3u, 4u}};
  {
    TextAbstraction::GlyphBufferData bitmap;
    bitmap.buffer = borrowed.data();
    DALI_TEST_CHECK(!bitmap.isBufferOwned);
  }
  DALI_TEST_EQUALS(borrowed.front(), static_cast<uint8_t>(1u), TEST_LOCATION);
  DALI_TEST_EQUALS(borrowed.back(), static_cast<uint8_t>(4u), TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextAbstractionBitmapPixelDataOwnershipP(void)
{
  UiTestApplication           application;
  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  PixelData                   pixels     = fontClient.CreateBitmap(1u, 1u, 0);
  DALI_TEST_CHECK(pixels);
  const auto buffer = Dali::Integration::GetPixelDataBuffer(pixels);
  DALI_TEST_CHECK(buffer.buffer && buffer.bufferSize > 0u);
  // The mock produces opaque glyphs. Its local GlyphBufferData has already
  // been destroyed; ownership must have transferred to PixelData exactly once.
  DALI_TEST_CHECK(std::all_of(buffer.buffer, buffer.buffer + buffer.bufferSize, [](uint8_t coverage)
  {
    return coverage == 255u;
  }));
  pixels.Reset();
  END_TEST;
}
