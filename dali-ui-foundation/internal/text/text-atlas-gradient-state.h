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
enum class AtlasGradientApplicationStatus : uint8_t
{
  UNINITIALIZED,
  APPLIED,
  SOLID_FALLBACK
};

/**
 * Renderer-owned value snapshot for one prepared gradient resource. Style is
 * copied only when this resource version changes; Texture and Sampler handles
 * retain their underlying resources independently of the control lifetime.
 */
struct AtlasRendererState
{
  Style    style;
  Texture  lookupTexture;
  Sampler  lookupSampler;
  uint64_t resourceId{0u};
  uint64_t styleRevision{0u};
  uint64_t lookupRevision{0u};

  bool IsEnabled() const;
};

/** Small frame-only state used while scrolling or resizing. */
struct AtlasGradientFrameState
{
  bool                     enabled{false};
  Text::GradientBoundsMode boundsMode{Text::GradientBoundsMode::CONTENT_BOUND};
};

/** Tracks which resource version has been applied to one renderer instance. */
struct AppliedAtlasGradientState
{
  AtlasGradientApplicationStatus status{AtlasGradientApplicationStatus::UNINITIALIZED};
  bool                           initialized{false};
  bool                           enabled{false};
  uint64_t                       resourceId{0u};
  uint64_t                       styleRevision{0u};
  uint64_t                       lookupRevision{0u};

  bool Matches(const AtlasRendererState& state) const;
  void Set(const AtlasRendererState& state);
  void SetSolidFallback(const AtlasRendererState& state);
  bool IsGradientApplied() const;
  bool IsSolidFallback() const;
  void Reset();
};

/**
 * Control-owned prepared state for one authored gradient. The LUT cache key is
 * intentionally the stop list only; geometry, units, spread, bounds and scroll
 * never recreate it.
 */
class PreparedAtlasState
{
public:
  PreparedAtlasState();

  PreparedAtlasState(const PreparedAtlasState&)            = delete;
  PreparedAtlasState(PreparedAtlasState&&)                 = delete;
  PreparedAtlasState& operator=(const PreparedAtlasState&) = delete;
  PreparedAtlasState& operator=(PreparedAtlasState&&)      = delete;

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

/** Control-owned normal/placeholder pair with one shared bounds policy. */
class EditableAtlasState
{
public:
  bool SetTextGradient(const Dali::Ui::Gradient::Base& gradient);
  bool SetPlaceholderGradient(const Dali::Ui::Gradient::Base& gradient);
  bool SetBoundsMode(Text::GradientBoundsMode mode);

  const Dali::Ui::Gradient::Base& GetTextGradient() const;
  const Dali::Ui::Gradient::Base& GetPlaceholderGradient() const;
  Text::GradientBoundsMode        GetBoundsMode() const;
  const AtlasRendererState&       GetRendererState(bool placeholder) const;
  AtlasGradientFrameState         GetFrameState(bool placeholder) const;
  AtlasGradientFrameState         GetFrameState(bool placeholder, const AppliedAtlasGradientState& applied) const;

private:
  PreparedAtlasState       mText;
  PreparedAtlasState       mPlaceholder;
  Text::GradientBoundsMode mBoundsMode{Text::GradientBoundsMode::CONTENT_BOUND};
};

} // namespace Dali::Ui::Text::Internal::Gradient

#endif // DALI_UI_FOUNDATION_INTERNAL_TEXT_ATLAS_GRADIENT_STATE_H
