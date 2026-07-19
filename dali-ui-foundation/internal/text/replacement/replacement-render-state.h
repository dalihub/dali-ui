#ifndef DALI_UI_TEXT_REPLACEMENT_RENDER_STATE_H
#define DALI_UI_TEXT_REPLACEMENT_RENDER_STATE_H

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

// EXTERNAL INCLUDES
#include <dali/devel-api/text-abstraction/bidirectional-support.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/final-elision-result.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-projection.h>
#include <dali-ui-foundation/internal/text/text-model.h>

namespace Dali::Ui::Text
{
/**
 * @brief Stores replacement state for one sync or async request.
 *
 * Processing and final-layout state is owned by one request.
 * The original Model remains the source of public logical text and authored styles.
 */
struct ReplacementRenderState
{
  ReplacementRenderState()                                         = default;
  ReplacementRenderState(const ReplacementRenderState&)            = delete;
  ReplacementRenderState& operator=(const ReplacementRenderState&) = delete;
  ReplacementRenderState(ReplacementRenderState&&)                 = default;
  ReplacementRenderState& operator=(ReplacementRenderState&&)      = default;

  /**
   * @brief Clears the projected model and request-local replacement state.
   *
   * @param[in] bidirectionalSupport The service used to release bidi data.
   */
  void Clear(TextAbstraction::BidirectionalSupport& bidirectionalSupport);

  ModelPtr                     processingModel;
  ReplacementProjection        projection;
  FinalElisionResult           finalElision;
  Vector<ReplacementPlacement> placements;
  Size                         layoutSize{};
  uint64_t                     sourceRevision{0u};
  uint64_t                     layoutGeneration{0u}; ///< UI/request generation used for stale-result rejection.
  bool                         attempted{false};
};

} // namespace Dali::Ui::Text

#endif // DALI_UI_TEXT_REPLACEMENT_RENDER_STATE_H
