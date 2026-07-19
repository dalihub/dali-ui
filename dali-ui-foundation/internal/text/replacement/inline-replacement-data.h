#ifndef DALI_UI_INLINE_REPLACEMENT_DATA_H
#define DALI_UI_INLINE_REPLACEMENT_DATA_H

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
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/replacement/inline-replacement-manager.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali::Ui
{
class ViewImpl;
}

namespace Dali::Ui::Internal::Text
{
/**
 * @brief Stores on-demand inline replacement data for a Label.
 */
struct InlineReplacementData
{
  /**
   * @brief Creates inline replacement data for a visual owner.
   *
   * @param[in] owner The visual owner.
   */
  explicit InlineReplacementData(Ui::View owner);

  InlineReplacementViewHost host;
  InlineReplacementManager  manager;
  uint64_t                  requestGeneration{0u};
  uint64_t                  lastRenderGeneration{0u};
  bool                      resourceReadyConnected{false};
};

/**
 * @brief Gets inline replacement data.
 *
 * @param[in] owner The visual owner.
 * @return The data, or nullptr.
 */
InlineReplacementData* GetInlineReplacementData(Ui::View owner);

/**
 * @brief Gets inline replacement data from a visual owner implementation.
 *
 * @param[in] owner The visual owner implementation.
 * @return The data, or nullptr.
 */
InlineReplacementData* GetInlineReplacementData(Ui::ViewImpl& owner);

/**
 * @brief Gets or creates inline replacement data.
 *
 * @param[in] owner The visual owner.
 * @return The data.
 */
InlineReplacementData& GetOrCreateInlineReplacementData(Ui::View owner);

/**
 * @brief Removes inline replacement data.
 *
 * @param[in] owner The visual owner.
 */
void RemoveInlineReplacementData(Ui::View owner);

/**
 * @brief Removes inline replacement data from a visual owner implementation.
 *
 * @param[in] owner The visual owner implementation.
 */
void RemoveInlineReplacementData(Ui::ViewImpl& owner);

} // namespace Dali::Ui::Internal::Text

#endif // DALI_UI_INLINE_REPLACEMENT_DATA_H
