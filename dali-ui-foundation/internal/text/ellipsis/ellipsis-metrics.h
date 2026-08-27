#ifndef DALI_UI_TEXT_ELLIPSIS_METRICS_H
#define DALI_UI_TEXT_ELLIPSIS_METRICS_H

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

// EXTERNAL INCLUDES
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/devel-api/text-abstraction/glyph-info.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/text-definitions.h>

namespace Dali::Ui::Text
{
/**
 * @brief Resolves ellipsis metrics for a font client.
 *
 * @param[in] fontClient The font client used to resolve glyph metrics.
 * @param[in] fontId The preferred font identifier.
 * @param[in] allowDefaultFont Whether the default font may be used.
 * @param[out] metrics The resolved ellipsis metrics.
 * @return true if metrics were resolved.
 */
bool ResolveFontClientEllipsisMetrics(TextAbstraction::FontClient& fontClient,
                                      FontId                       fontId,
                                      bool                         allowDefaultFont,
                                      TextAbstraction::GlyphInfo&  metrics);

} // namespace Dali::Ui::Text

#endif // DALI_UI_TEXT_ELLIPSIS_METRICS_H
