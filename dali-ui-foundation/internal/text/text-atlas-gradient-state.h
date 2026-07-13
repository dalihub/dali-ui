/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef DALI_UI_FOUNDATION_INTERNAL_TEXT_ATLAS_GRADIENT_STATE_H
#define DALI_UI_FOUNDATION_INTERNAL_TEXT_ATLAS_GRADIENT_STATE_H

#include <dali/public-api/rendering/sampler.h>
#include <dali/public-api/rendering/texture.h>
#include <cstdint>

#include <dali-ui-foundation/internal/text/text-gradient-style.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>

namespace Dali::Ui::Text::Internal::Gradient
{
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
 * @brief Stores the renderer-facing values for one prepared atlas resource.
 *
 * The style is copied only when the resource version changes. Texture and
 * Sampler handles retain their underlying resources independently of the
 * control lifetime.
 */
struct AtlasRendererState
{
  Style    style;
  Texture  lookupTexture;
  Sampler  lookupSampler;
  uint64_t resourceId{0u};
  uint64_t styleRevision{0u};
  uint64_t lookupRevision{0u};

  /**
   * @brief Returns whether this state contains a renderable resource.
   *
   * @return True if the style and lookup resources are valid.
   */
  bool IsEnabled() const;
};

/**
 * @brief Stores per-frame atlas data used while scrolling or resizing.
 */
struct AtlasFrameState
{
  bool                     enabled{false};
  Text::GradientBoundsMode boundsMode{Text::GradientBoundsMode::CONTENT_BOUND};
};

/**
 * @brief Tracks which atlas resource version is applied to one renderer instance.
 */
struct AtlasApplyState
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

/**
 * @brief Stores the prepared atlas resource for one authored gradient.
 *
 * The LUT cache key is intentionally the stop list only. Geometry, units,
 * spread, bounds, and scrolling do not recreate the lookup texture.
 */
class AtlasResource
{
public:
  AtlasResource();

  AtlasResource(const AtlasResource&)            = delete;
  AtlasResource(AtlasResource&&)                 = delete;
  AtlasResource& operator=(const AtlasResource&) = delete;
  AtlasResource& operator=(AtlasResource&&)      = delete;

  bool Set(const Dali::Ui::Gradient::Base& gradient);

  const Dali::Ui::Gradient::Base& Get() const;
  bool                            IsEnabled() const;
  uint64_t                        GetStyleRevision() const;
  uint64_t                        GetLookupRevision() const;
  uint64_t                        GetLookupGenerationCount() const;
  const AtlasRendererState&       GetRendererState() const;

private:
  Dali::Ui::Gradient::Base mAuthored;
  AtlasRendererState       mRendererState;
  uint64_t                 mLookupGenerationCount{0u};
};

/**
 * @brief Stores prepared normal and placeholder atlas resources with one shared
 * bounds policy.
 */
class EditableAtlasResources
{
public:
  bool SetTextGradient(const Dali::Ui::Gradient::Base& gradient);
  bool SetPlaceholderGradient(const Dali::Ui::Gradient::Base& gradient);
  bool SetBoundsMode(Text::GradientBoundsMode mode);

  const Dali::Ui::Gradient::Base& GetTextGradient() const;
  const Dali::Ui::Gradient::Base& GetPlaceholderGradient() const;
  Text::GradientBoundsMode        GetBoundsMode() const;
  const AtlasRendererState&       GetRendererState(bool placeholder) const;
  AtlasFrameState                 GetFrameState(bool placeholder) const;
  AtlasFrameState                 GetFrameState(bool placeholder, const AtlasApplyState& applied) const;

private:
  AtlasResource            mText;
  AtlasResource            mPlaceholder;
  Text::GradientBoundsMode mBoundsMode{Text::GradientBoundsMode::CONTENT_BOUND};
};

} // namespace Dali::Ui::Text::Internal::Gradient

#endif // DALI_UI_FOUNDATION_INTERNAL_TEXT_ATLAS_GRADIENT_STATE_H
