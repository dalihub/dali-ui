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
#include <dali/devel-api/common/vector-wrapper.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/signals/connection-tracker.h>
#include <array>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/state-effect-impl.h>
#include <dali-ui-foundation/public-api/input/input-event.h>
#include <dali-ui-foundation/public-api/views/effects/overlay-effect.h>
#include <dali-ui-foundation/public-api/views/state-event.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-foundation/public-api/visuals/color-visual.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

class OverlayEffectData;

enum class RecoilRestoreMode
{
  ANIMATE,
  IMMEDIATE
};

class OverlayEffectImpl : public Integration::StateEffectImpl
{
public:
  static constexpr uint32_t DEFAULT_OVERLAY_COLOR_RGB   = 0x000000u;
  static constexpr float    DEFAULT_OVERLAY_COLOR_ALPHA = 0.1f;

  OverlayEffectImpl();
  OverlayEffectImpl(const UiColor& overlayColor, const Vector4& cornerRadius, CornerRadiusPolicy cornerRadiusPolicy, bool useTargetCornerRadius, RecoilScope recoilScope);
  OverlayEffectImpl(const OverlayEffectImpl& other);

  void    SetOverlayColor(const UiColor& color);
  UiColor GetOverlayColor() const;

  void    SetCornerRadius(const Vector4& radius);
  Vector4 GetCornerRadius() const;

  void               SetCornerRadiusPolicy(CornerRadiusPolicy policy);
  CornerRadiusPolicy GetCornerRadiusPolicy() const;

  void SetUseTargetCornerRadius(bool useTargetCornerRadius);
  bool IsUsingTargetCornerRadius() const;

  void        SetRecoilScope(RecoilScope scope);
  RecoilScope GetRecoilScope() const;

protected:
  ~OverlayEffectImpl() override;

  void OnAttached(TraitId id, View& view) override;
  void OnDetaching(TraitId id, View& view) override;
  void OnViewDestroying(ViewImpl* viewImpl) override;
  void OnViewStateChanged(View view, const StateEvent& event) override;
  void OnInteractiveAttached(View view) override;
  void OnStateEffectTargetsChanged(View view) override;
  bool ShouldSuppressDefaultFocusIndicator(View view) const override;

private:
  OverlayEffectData& GetOrCreateOverlayEffectData(View owner);
  void               ReleaseOverlayEffectData(View owner, RecoilRestoreMode restoreMode = RecoilRestoreMode::IMMEDIATE);
  void               HandleStateChanged(OverlayEffectData& data, View owner, const ViewState& state, RecoilRestoreMode restoreMode = RecoilRestoreMode::ANIMATE);
  void               CleanupOverlay(OverlayEffectData& data, RecoilRestoreMode restoreMode);
  View               ResolveTarget(View owner) const;
  void               ApplyOverlayProperties(ColorVisual overlay, View target) const;

private:
  UiColor            mOverlayColor;
  Vector4            mCornerRadius;
  CornerRadiusPolicy mCornerRadiusPolicy;
  RecoilScope        mRecoilScope;
  bool               mUseTargetCornerRadius;
};

class OverlayEffectData : public ConnectionTracker
{
public:
  OverlayEffectData();
  ~OverlayEffectData();

  void DetachEffect(RecoilRestoreMode restoreMode = RecoilRestoreMode::IMMEDIATE);
  void DetachEffect(View owner, RecoilRestoreMode restoreMode);

  void        SetActiveOverlay(ColorVisual overlay);
  ColorVisual GetActiveOverlay() const;
  bool        HasActiveOverlay() const;
  void        ClearActiveOverlay();
  void        SetActiveTarget(View target);
  View        GetActiveTarget() const;
  void        ClearActiveTarget();
  bool        IsPressed() const;
  bool        IsFocusIndicated() const;
  void        SetState(const ViewState& state);
  void        ApplyDisabledOpacity(View owner);
  void        RestoreDisabledOpacity(View owner);
  void        ApplyRecoil(View overlayTarget, RecoilScope scope);
  void        RestoreRecoil(RecoilRestoreMode restoreMode = RecoilRestoreMode::ANIMATE);
  bool        HasRunningRecoilAnimation() const;
  void        SetPendingRelease();
  void        ClearPendingRelease();

private:
  static constexpr uint32_t MAX_RECOIL_TARGET_COUNT           = 3u;
  static constexpr float    INVALID_DISABLED_ORIGINAL_OPACITY = -1.0f;

  struct RecoilTarget
  {
    WeakHandle<View> view;
    Vector2          originalScale;
    Vector3          originalPivot;
    Vector3          recoilPivot;
    bool             usesRecoilPivot{false};
  };

  void ClearRecoilTargets();
  void OnPendingReleaseAnimationFinished(Animation animation);

  ColorVisual mActiveOverlay;
  // Keep the active target alive until cleanup so the overlay can be detached
  // even if external handles to the target are dropped during feedback.
  View                                              mActiveTarget;
  WeakHandle<View>                                  mRecoilBaseTarget;
  RecoilScope                                       mRecoilScope;
  std::array<RecoilTarget, MAX_RECOIL_TARGET_COUNT> mRecoilTargets;
  Animation                                         mRecoilAnimation;
  float                                             mDisabledOriginalOpacity;
  uint32_t                                          mPendingReleaseAnimationId;
  uint32_t                                          mRecoilTargetCount;
  bool                                              mPendingReleaseAfterRestore;
  bool                                              mPressed;
  bool                                              mFocusIndicated;
};

} // namespace Internal

inline Internal::OverlayEffectImpl& GetImpl(OverlayEffect& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Internal::OverlayEffectImpl&>(handle);
}

inline const Internal::OverlayEffectImpl& GetImpl(const OverlayEffect& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Internal::OverlayEffectImpl&>(handle);
}

} // namespace Ui
} // namespace Dali
