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

// EXTERNAL INCLUDES
#include <dali/devel-api/text-abstraction/text-abstraction-definitions.h>

namespace DALI_NAMESPACE::Ui::Text
{
/**
 * @brief Stable source identity and static control geometry retained across
 * the ellipsis-to-marquee transition.
 *
 * The character index plus the glyph's occurrence within that character is
 * stable across the separate static and natural-layout requests used by the
 * async path because both requests shape the same authored source. Font,
 * locale, direction, content, and render changes invalidate the descriptor;
 * request cancellation prevents an older async result from republishing it.
 */
struct MarqueeStartAnchor
{
  TextAbstraction::CharacterIndex characterIndex{0u};
  TextAbstraction::Length         glyphOccurrence{0u};
  float                           staticControlX{0.0f}; ///< Logical pixels in the Label content box.
  bool                            valid{false};
};

/**
 * @brief Effective horizontal translation used by a fitting static render.
 *
 * Fitting single-line marquee keeps the full source topology, so preserving
 * the static renderer's effective line translation is sufficient. This is
 * intentionally separate from MarqueeStartAnchor, which identifies retained
 * source geometry across an END-ellipsis transition.
 */
struct MarqueeFittingStartGeometry
{
  float staticTranslation{0.0f}; ///< Logical pixels in the Label content box.
  bool  valid{false};
};
} // namespace DALI_NAMESPACE::Ui::Text
