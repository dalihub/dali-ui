#ifndef DALI_UI_TEXT_ELLIPSIS_RESOLVER_H
#define DALI_UI_TEXT_ELLIPSIS_RESOLVER_H

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
#include <dali/public-api/actors/actor-enumerations.h>
// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/final-elision-result.h>
#include <dali-ui-foundation/internal/text/layouts/layout-engine.h>
#include <dali-ui-foundation/internal/text/text-model.h>

namespace Dali::Ui::Text
{

/**
 * @brief Resolves an END-elided line from source-layout topology.
 *
 * A cluster-safe logical prefix is retained. Its source-shaped glyphs and
 * physical order are projected into the final domain, and U+2026 occupies the
 * physical slot of the first removed source cluster with that cluster's
 * resolved direction affinity. No neutral ellipsis character is reintroduced
 * into BiDi resolution. The source model remains immutable; @p result is the
 * sole final-layout authority.
 *
 * The caller supplies a model for which effective elision is enabled and
 * LayoutEngine has either marked an END candidate or produced no visible line.
 *
 * @return true when an authoritative END result, including an intentional omission, was resolved.
 */
bool ResolveEndEllipsis(const Model&                 model,
                        const Size&                  controlSize,
                        TextAbstraction::FontClient& fontClient,
                        FinalElisionResult&          result);

/**
 * @brief Aligns final-domain lines and builds View/atlas coordinates.
 */
void FinalizeEndEllipsisGeometry(const Model&                model,
                                 const Size&                 controlSize,
                                 Dali::LayoutDirection::Type layoutDirection,
                                 bool                        matchLayoutDirection,
                                 Layout::Engine&             layoutEngine,
                                 FinalElisionResult&         result);

} // namespace Dali::Ui::Text

#endif // DALI_UI_TEXT_ELLIPSIS_RESOLVER_H
