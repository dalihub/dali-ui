#ifndef DALI_UI_TEXT_VISUALIZER_LAYOUT_ENGINE_H
#define DALI_UI_TEXT_VISUALIZER_LAYOUT_ENGINE_H

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
#include <dali-ui-foundation/internal/text/text-visualizer/exclusion-layout-cache.h>
#include <dali-ui-foundation/internal/text/text-visualizer/layout-types.h>
#include <dali-ui-foundation/internal/text/text-visualizer/prepared-text.h>
#include <dali/public-api/math/rect.h>

namespace Dali::Ui::Internal::TextVisualizer
{
class LayoutEngine
{
public:
  static float GetPlaceholderClusterAdvance(const PreparedText& preparedText);

  static float GetPlaceholderLineHeight(const PreparedText& preparedText);

  static Dali::Vector<AvailableInterval> BuildAvailableIntervals(float                            layoutWidth,
                                                                 float                            lineY,
                                                                 float                            lineHeight,
                                                                 const Dali::Vector<Rect<float>>& exclusionRegions);

  static Dali::Vector<AvailableInterval> BuildAvailableIntervals(float                       layoutWidth,
                                                                 float                       lineY,
                                                                 float                       lineHeight,
                                                                 const ExclusionLayoutCache& exclusionCache);

  static void LayoutPlaceholder(const PreparedText&              preparedText,
                                float                            layoutWidth,
                                float                            lineHeight,
                                const Dali::Vector<Rect<float>>& exclusionRegions,
                                LayoutResult&                    result);

  static void LayoutPlaceholder(const PreparedText&         preparedText,
                                float                       layoutWidth,
                                float                       lineHeight,
                                const ExclusionLayoutCache& exclusionCache,
                                LayoutResult&               result);

  static void LayoutGlyphs(const PreparedText&              preparedText,
                           float                            layoutWidth,
                           float                            lineHeight,
                           const Dali::Vector<Rect<float>>& exclusionRegions,
                           LayoutResult&                    result);

  static void LayoutGlyphs(const PreparedText&         preparedText,
                           float                       layoutWidth,
                           float                       lineHeight,
                           const ExclusionLayoutCache& exclusionCache,
                           LayoutResult&               result);
};

} // namespace Dali::Ui::Internal::TextVisualizer

#endif // DALI_UI_TEXT_VISUALIZER_LAYOUT_ENGINE_H
