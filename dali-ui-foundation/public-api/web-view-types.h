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
 *
 */

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{

/**
 * @brief Enumeration for the scroll edge.
 */
enum class WebViewScrollEdge
{
  LEFT,   ///< Left edge reached.
  RIGHT,  ///< Right edge reached.
  TOP,    ///< Top edge reached.
  BOTTOM, ///< Bottom edge reached.
};

/**
 * @brief Enumeration for the over-scroll direction.
 */
enum class WebViewOverScrolled
{
  LEFT,   ///< Left over-scrolled.
  RIGHT,  ///< Right over-scrolled.
  TOP,    ///< Top over-scrolled.
  BOTTOM, ///< Bottom over-scrolled.
};

/**
 * @brief Enumeration for the find-text options (can be combined with bitwise OR).
 */
enum class WebViewFindOption : uint32_t
{
  NONE                               = 0,
  CASE_INSENSITIVE                   = 1 << 0,
  AT_WORD_STARTS                     = 1 << 1,
  TREAT_MEDIAL_CAPITAL_AS_WORD_START = 1 << 2,
  BACKWARDS                          = 1 << 3,
  WRAP_AROUND                        = 1 << 4,
  SHOW_OVERLAY                       = 1 << 5,
  SHOW_FIND_INDICATOR                = 1 << 6,
  SHOW_HIGHLIGHT                     = 1 << 7,
};

} // namespace Ui
} // namespace Dali