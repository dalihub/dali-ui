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

namespace Dali::Ui::Text::Internal::Gradient
{
struct AtlasRendererState;

/**
 * @brief Enumerates the result of applying an atlas resource to a renderer.
 */
enum class AtlasApplyStatus : uint8_t
{
  UNINITIALIZED,
  APPLIED,
  SOLID_FALLBACK
};

/**
 * @brief Tracks which atlas resource version is applied to one renderer instance.
 */
struct DALI_UI_API AtlasApplyState
{
  AtlasApplyStatus status{AtlasApplyStatus::UNINITIALIZED};
  bool             initialized{false};
  bool             enabled{false};
  uint64_t         resourceId{0u};
  uint64_t         styleRevision{0u};
  uint64_t         lookupRevision{0u};

  bool Matches(const AtlasRendererState& state) const;
  void Set(const AtlasRendererState& state);
  void SetSolidFallback(const AtlasRendererState& state);
  bool IsGradientApplied() const;
  bool IsSolidFallback() const;
  void Reset();
};

} // namespace Dali::Ui::Text::Internal::Gradient

#endif // DALI_UI_INTEGRATION_TEXT_ATLAS_GRADIENT_APPLY_STATE_H
