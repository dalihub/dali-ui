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

#ifndef DALI_UI_INLINE_REPLACEMENT_REVEAL_BRIDGE_H
#define DALI_UI_INLINE_REPLACEMENT_REVEAL_BRIDGE_H

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/property.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/replacement/replacement-run-snapshot.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali::Ui::Internal
{
bool PublishInlineReplacementRevealTimings(Ui::View                                         owner,
                                           const Vector<Ui::Text::ReplacementRevealTiming>& timings,
                                           uint64_t                                         sourceRevision,
                                           Property::Index                                  progressPropertyIndex);

void ClearInlineReplacementReveal(Ui::View owner);

bool IsCurrentInlineReplacementRender(Ui::View owner, uint64_t layoutGeneration);

} // namespace Dali::Ui::Internal

#endif // DALI_UI_INLINE_REPLACEMENT_REVEAL_BRIDGE_H
