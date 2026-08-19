#ifndef DALI_UI_TEXT_ALIGNMENT_H
#define DALI_UI_TEXT_ALIGNMENT_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/math/vector2.h>

#include <dali-ui-foundation/internal/text/layouts/layout-engine.h>
#include <dali-ui-foundation/internal/text/line-run.h>
#include <dali-ui-foundation/internal/text/text-model.h>

namespace Dali::Ui::Text
{
/**
 * @brief Applies controller or bounded-paragraph alignment to an arbitrary line domain.
 *
 * @p lines may be the source VisualModel lines or an authoritative final
 * projection. The same Layout::Engine alignment formula is used in both cases.
 */
void AlignTextLines(Layout::Engine&             layoutEngine,
                    const Size&                 controlSize,
                    CharacterIndex              startIndex,
                    Length                      numberOfCharacters,
                    const Model&                model,
                    Vector<LineRun>&            lines,
                    float&                      minimumLineOffset,
                    Dali::LayoutDirection::Type layoutDirection,
                    bool                        matchLayoutDirection);

} // namespace Dali::Ui::Text

#endif // DALI_UI_TEXT_ALIGNMENT_H
