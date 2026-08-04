#ifndef DALI_UI_INTEGRATION_TEXT_ATLAS_GRADIENT_APPLY_STATE_H
#define DALI_UI_INTEGRATION_TEXT_ATLAS_GRADIENT_APPLY_STATE_H

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
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali::Ui::Integration::Text::Gradient
{
/**
 * @brief Result state for applying an atlas gradient resource to a renderer.
 */
enum class AtlasApplyStatus : uint8_t
{
  UNINITIALIZED, ///< No atlas gradient resource has been applied yet.
  APPLIED,       ///< The atlas gradient resource was applied to the renderer.
  SOLID_FALLBACK ///< The renderer uses solid text as a fallback.
};

/**
 * @brief Stores the atlas gradient resource version currently applied to a renderer.
 */
struct DALI_UI_API AtlasApplyState
{
  AtlasApplyStatus status{AtlasApplyStatus::UNINITIALIZED}; ///< The result of the last apply attempt.
  bool             initialized{false};                      ///< Whether this state has been initialized.
  bool             enabled{false};                          ///< Whether the applied resource is renderable.
  uint64_t         resourceId{0u};                          ///< The atlas resource identifier.
  uint64_t         styleRevision{0u};                       ///< The applied style revision.
  uint64_t         lookupRevision{0u};                      ///< The applied lookup texture revision.

  /**
   * @brief Checks whether this state matches the specified atlas resource version.
   *
   * @param[in] resourceId The atlas resource identifier.
   * @param[in] styleRevision The style revision.
   * @param[in] lookupRevision The lookup texture revision.
   * @return True if all version values match.
   */
  bool Matches(uint64_t resourceId, uint64_t styleRevision, uint64_t lookupRevision) const;

  /**
   * @brief Updates this state after attempting to apply an atlas gradient resource.
   *
   * @param[in] status The result of the apply attempt.
   * @param[in] enabled Whether the resource is renderable.
   * @param[in] resourceId The atlas resource identifier.
   * @param[in] styleRevision The style revision.
   * @param[in] lookupRevision The lookup texture revision.
   */
  void Set(AtlasApplyStatus status, bool enabled, uint64_t resourceId, uint64_t styleRevision, uint64_t lookupRevision);

  /**
   * @brief Checks whether a gradient resource is currently applied.
   *
   * @return True if a renderable atlas gradient is applied.
   */
  bool IsGradientApplied() const;

  /**
   * @brief Checks whether solid text fallback is currently applied.
   *
   * @return True if the last apply attempt used solid text fallback.
   */
  bool IsSolidFallback() const;

  /**
   * @brief Resets this state to the uninitialized state.
   */
  void Reset();
};

} // namespace Dali::Ui::Integration::Text::Gradient

#endif // DALI_UI_INTEGRATION_TEXT_ATLAS_GRADIENT_APPLY_STATE_H
