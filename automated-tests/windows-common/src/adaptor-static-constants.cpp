/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 */

#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/public-api/math/math-utils.h>

namespace Dali::TextAbstraction
{
const PointSize26Dot6 FontClient::DEFAULT_POINT_SIZE   = 768u;
const float           FontClient::DEFAULT_ITALIC_ANGLE = 12.0f * Dali::Math::PI_OVER_180;

const bool     FontClient::DEFAULT_ATLAS_LIMITATION_ENABLED = true;
const uint32_t FontClient::DEFAULT_TEXT_ATLAS_WIDTH         = 512u;
const uint32_t FontClient::DEFAULT_TEXT_ATLAS_HEIGHT        = 512u;
const Size     FontClient::DEFAULT_TEXT_ATLAS_SIZE(DEFAULT_TEXT_ATLAS_WIDTH, DEFAULT_TEXT_ATLAS_HEIGHT);

const uint32_t FontClient::MAX_TEXT_ATLAS_WIDTH  = 1024u;
const uint32_t FontClient::MAX_TEXT_ATLAS_HEIGHT = 1024u;
const Size     FontClient::MAX_TEXT_ATLAS_SIZE(MAX_TEXT_ATLAS_WIDTH, MAX_TEXT_ATLAS_HEIGHT);

const uint16_t FontClient::PADDING_TEXT_ATLAS_BLOCK = 5u;
const Size     FontClient::MAX_SIZE_FIT_IN_ATLAS(MAX_TEXT_ATLAS_WIDTH - PADDING_TEXT_ATLAS_BLOCK, MAX_TEXT_ATLAS_HEIGHT - PADDING_TEXT_ATLAS_BLOCK);

const uint32_t FontClient::NUMBER_OF_POINTS_PER_ONE_UNIT_OF_POINT_SIZE = 64u;
} // namespace Dali::TextAbstraction
