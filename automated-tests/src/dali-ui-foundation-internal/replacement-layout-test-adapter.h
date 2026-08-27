#ifndef DALI_UI_REPLACEMENT_LAYOUT_TEST_ADAPTER_H
#define DALI_UI_REPLACEMENT_LAYOUT_TEST_ADAPTER_H

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
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/devel-api/text-abstraction/segmentation.h>
#include <dali/devel-api/text-abstraction/shaping.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/layouts/layout-engine.h>
#include <dali-ui-foundation/internal/text/multi-language-support.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-render-state.h>

namespace Dali::Ui::Text
{
/**
 * @brief Stores real text services used by the replacement layout diagnostics.
 */
struct ReplacementLayoutTestServices
{
  TextAbstraction::Segmentation         segmentation;
  TextAbstraction::BidirectionalSupport bidirectionalSupport;
  TextAbstraction::Shaping              shaping;
  TextAbstraction::FontClient           fontClient;
  MultilanguageSupport                  multilanguageSupport;
};

/**
 * @brief Stores options for the test-only canonical Controller adapter.
 */
struct ReplacementLayoutTestOptions
{
  Size                             contentSize{320.0f, 80.0f};
  Layout::Engine::Type             layoutType{Layout::Engine::SINGLE_LINE_BOX};
  LineWrapMode                     lineWrapMode{LineWrapMode::WORD};
  EllipsisPosition::Type           ellipsisPosition{EllipsisPosition::END};
  TextAbstraction::PointSize26Dot6 fontPointSize{TextAbstraction::FontClient::DEFAULT_POINT_SIZE};
  float                            fontPixelSize{10.0f};
  float                            characterSpacing{0.0f};
  float                            defaultLineSize{0.0f};
  float                            defaultLineSpacing{0.0f};
  float                            relativeLineSize{1.0f};
  Alignment                        horizontalAlignment{Alignment::START};
  Alignment                        verticalAlignment{Alignment::START};
  LayoutDirection::Type            layoutDirection{LayoutDirection::LEFT_TO_RIGHT};
  bool                             matchLayoutDirection{false};
  bool                             elideText{false};
  bool                             marqueeMaxTextureExceeded{false};
  uint64_t                         sourceRevision{0u};
  uint64_t                         layoutGeneration{0u};
};

struct OrdinaryMarqueeTransitionTrace
{
  CharacterIndex anchorCharacter{0u};
  GlyphIndex     staticSourceGlyph{0u};
  GlyphIndex     marqueeTextureGlyph{0u};
  float          staticControlX{0.0f};
  float          marqueeTextureX{0.0f};
  float          naturalContentWidth{0.0f};
  float          sourceToTextureMinimumTranslation{0.0f};
  float          sourceToTextureMaximumTranslation{0.0f};
  bool           directionRightToLeft{false};
  bool           valid{false};
};

bool LayoutReplacementForTest(const Model&                        originalModel,
                              const ReplacementProjection&        projection,
                              ReplacementLayoutTestServices&      services,
                              const ReplacementLayoutTestOptions& options,
                              ReplacementRenderState&             result);

bool LayoutReplacementForTest(const ReplacementProjection&        projection,
                              ReplacementLayoutTestServices&      services,
                              const ReplacementLayoutTestOptions& options,
                              ReplacementRenderState&             result);

bool LayoutOrdinaryForTest(const Model&                        originalModel,
                           const ReplacementLayoutTestOptions& options,
                           ModelPtr&                           result);

OrdinaryMarqueeTransitionTrace TraceOrdinaryMarqueeTransitionForTest(
  const Model&                        originalModel,
  const ReplacementLayoutTestOptions& options);

bool LayoutReplacementSourceForTest(const Model&                        originalModel,
                                    const ReplacementSourceSnapshot&    sourceSnapshot,
                                    ReplacementLayoutTestServices&      services,
                                    const ReplacementLayoutTestOptions& options,
                                    ReplacementRenderState&             result);

} // namespace Dali::Ui::Text

#endif // DALI_UI_REPLACEMENT_LAYOUT_TEST_ADAPTER_H
