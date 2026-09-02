#pragma once

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

#include <cstdint>

namespace Dali::TextAbstraction::Test
{
// Reserved by the test FontClient for deterministic half-coverage bitmaps.
// Keep this distinct from UINT32_MAX, which text code uses as an invalid or synthetic glyph sentinel.
constexpr uint32_t HALF_COVERAGE_GLYPH_INDEX = 0xF0000001u;
} // namespace Dali::TextAbstraction::Test
