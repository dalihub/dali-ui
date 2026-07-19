#ifndef DALI_UI_TEXT_REPLACEMENT_LAYOUT_DATA_H
#define DALI_UI_TEXT_REPLACEMENT_LAYOUT_DATA_H

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

#include <dali-ui-foundation/internal/text/replacement/replacement-projection.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>

namespace Dali::Ui::Text
{
/**
 * @brief Stores optional, non-owning replacement data used during layout.
 *
 * A null run pointer selects the ordinary-text layout path.
 */
struct ReplacementLayoutData
{
  const Vector<ProjectedReplacementRun>* runs{nullptr};
  FontId                                 defaultFontId{0u};
  Alignment                              horizontalAlignment{Alignment::START};
  Dali::LayoutDirection::Type            layoutDirection{Dali::LayoutDirection::LEFT_TO_RIGHT};
  bool                                   matchLayoutDirection{false};

  /**
   * @brief Finds the replacement at a projected character index.
   *
   * @param[in] projectedCharacterIndex The character index in the projected text.
   * @return The matching replacement run, or nullptr if none exists.
   */
  const ProjectedReplacementRun* Find(CharacterIndex projectedCharacterIndex) const
  {
    if(runs == nullptr)
    {
      return nullptr;
    }
    if(runs->Count() <= 8u)
    {
      for(const ProjectedReplacementRun& run : *runs)
      {
        if(run.projectedCharacterIndex == projectedCharacterIndex)
        {
          return &run;
        }
        if(run.projectedCharacterIndex > projectedCharacterIndex)
        {
          break;
        }
      }
      return nullptr;
    }

    uint32_t first = 0u;
    uint32_t last  = runs->Count();
    while(first < last)
    {
      const uint32_t                 middle = first + (last - first) / 2u;
      const ProjectedReplacementRun& run    = (*runs)[middle];
      if(projectedCharacterIndex < run.projectedCharacterIndex)
      {
        last = middle;
      }
      else if(projectedCharacterIndex > run.projectedCharacterIndex)
      {
        first = middle + 1u;
      }
      else
      {
        return &run;
      }
    }
    return nullptr;
  }
};

} // namespace Dali::Ui::Text

#endif // DALI_UI_TEXT_REPLACEMENT_LAYOUT_DATA_H
