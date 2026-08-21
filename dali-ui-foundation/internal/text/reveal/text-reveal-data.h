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
#include <dali/public-api/object/property.h>
#include <cstdint>
#include <memory>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/text/style/reveal.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace Text
{

/**
 * @brief Stores Label-side reveal state after Reveal is first used.
 *
 * The attachment is retained after disabling Reveal so that progress, the
 * registered scene property index, and the async revision remain stable when
 * Reveal is enabled again.
 */
struct TextRevealData
{
  Ui::Text::Reveal::Unit unit{Ui::Text::Reveal::Unit::CHARACTER};
  float                  fadeDurationRatio{Ui::Text::Reveal::AUTO_FADE_DURATION_RATIO};
  Property::Index        progressPropertyIndex{Property::INVALID_INDEX};
  uint64_t               revision{0u};
  float                  progress{0.0f};
  bool                   enabled{false};
};

using TextRevealDataPtr = std::unique_ptr<TextRevealData>;

inline TextRevealData* GetTextRevealData(TextRevealDataPtr& data)
{
  return data.get();
}

inline const TextRevealData* GetTextRevealData(const TextRevealDataPtr& data)
{
  return data.get();
}

inline TextRevealData& GetOrCreateTextRevealData(TextRevealDataPtr& data)
{
  if(!data)
  {
    data = std::make_unique<TextRevealData>();
  }

  return *data;
}

} // namespace Text
} // namespace Internal
} // namespace Ui
} // namespace Dali
