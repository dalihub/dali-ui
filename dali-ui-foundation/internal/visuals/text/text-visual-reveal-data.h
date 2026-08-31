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
#include <dali/devel-api/text-abstraction/segmentation.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/object/property.h>
#include <cstdint>
#include <memory>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/reveal/text-reveal.h>
#include <dali-ui-foundation/public-api/text/style/reveal.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

/**
 * @brief Stores TextVisual reveal state after Reveal is first enabled.
 *
 * Label owns the stable progress scene-property identity. TextVisual retains
 * only renderer state, the async publication revision, and optional WORD
 * segmentation. Keeping this attachment after disable preserves revision
 * history; segmentation remains empty until synchronous WORD planning.
 */
struct TextVisualRevealData
{
  std::vector<Constraint>       constraints;
  TextAbstraction::Segmentation segmentation;

  Ui::Text::Internal::Reveal::Unit     unit{Ui::Text::Internal::Reveal::Unit::DISABLED};
  Ui::Text::Internal::Reveal::Sequence sequence{Ui::Text::Internal::Reveal::Sequence::WHOLE_TEXT};
  float                                fadeDurationRatio{Ui::Text::Reveal::AUTO_FADE_DURATION_RATIO};
  float                                fadeDuration{0.0f};
  float                                sequenceStaggerRatio{0.0f};
  Property::Index                      progressPropertyIndex{Property::INVALID_INDEX};
  uint64_t                             revision{0u};
};

using TextVisualRevealDataPtr = std::unique_ptr<TextVisualRevealData>;

inline TextVisualRevealData* GetTextVisualRevealData(TextVisualRevealDataPtr& data)
{
  return data.get();
}

inline const TextVisualRevealData* GetTextVisualRevealData(const TextVisualRevealDataPtr& data)
{
  return data.get();
}

inline TextVisualRevealData& GetOrCreateTextVisualRevealData(TextVisualRevealDataPtr& data)
{
  if(!data)
  {
    data = std::make_unique<TextVisualRevealData>();
  }

  return *data;
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
