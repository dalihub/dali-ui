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

// CLASS HEADER
#include <dali-ui-foundation/internal/state-effects/overlay-effect-impl.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <cmath>
#include <iterator>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/animation/duration.h>
#include <dali-ui-foundation/public-api/animation/view-animation-bridge.autogen.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

namespace
{

UiColor GetOverlayColorWithActiveCount(const UiColor& color, uint32_t activeCount)
{
  if(activeCount <= 1u)
  {
    return color;
  }

  Vector4 rgba = color.GetRgba();
  rgba.a       = 1.0f - std::pow(1.0f - rgba.a, static_cast<float>(activeCount));
  return UiColor(rgba);
}

const AttachmentId OVERLAY_EFFECT_DATA_ATTACHMENT_ID = AttachmentId::Alloc();

constexpr float    DEFAULT_RECOIL_SCALE_FACTOR       = 0.96f;
constexpr float    RECOIL_ANIMATION_SECONDS          = 0.1f;
constexpr float    DISABLED_OPACITY_FACTOR           = 0.4f;
constexpr uint32_t OVERLAY_EFFECT_DATA_POOL_MAX_SIZE = 8u;
constexpr float    PROPERTY_EPSILON                  = 0.001f;

bool ApproximatelyEqual(float lhs, float rhs)
{
  return std::fabs(lhs - rhs) <= PROPERTY_EPSILON;
}

bool ApproximatelyEqual(const Vector3& lhs, const Vector3& rhs)
{
  return ApproximatelyEqual(lhs.x, rhs.x) && ApproximatelyEqual(lhs.y, rhs.y) && ApproximatelyEqual(lhs.z, rhs.z);
}

Vector2 GetBaseScale(View view)
{
  return Vector2(view.GetScaleX(), view.GetScaleY());
}

void AddScaleAnimation(Animation animation, View view, const Vector2& targetScale)
{
  ViewAnimationBridge bridge = view.Animate(animation);
  bridge.ScaleX(targetScale.x, Duration(RECOIL_ANIMATION_SECONDS), AlphaFunction(AlphaFunction::LINEAR));
  bridge.ScaleY(targetScale.y, Duration(RECOIL_ANIMATION_SECONDS), AlphaFunction(AlphaFunction::LINEAR));
}

bool ComputeParentCenterPivot(View parent, View child, Vector3& pivot)
{
  const Vector3 parentSize = parent.GetCurrentSize();
  const Vector3 childSize  = child.GetCurrentSize();

  if(childSize.width <= PROPERTY_EPSILON || childSize.height <= PROPERTY_EPSILON)
  {
    return false;
  }

  const Vector3 originalPivot = child.GetPivot();
  pivot.x                     = (parentSize.width * 0.5f - child.GetPositionX()) / childSize.width;
  pivot.y                     = (parentSize.height * 0.5f - child.GetPositionY()) / childSize.height;
  pivot.z                     = originalPivot.z;
  return true;
}

bool IsOverlayActive(const ViewState& state)
{
  return state.Contains(ViewState::PRESSED) || state.Contains(ViewState::FOCUS_INDICATED);
}

OverlayEffectData* GetOverlayEffectData(View owner)
{
  if(!owner)
  {
    return nullptr;
  }

  return owner.GetAttachment<OverlayEffectData>(OVERLAY_EFFECT_DATA_ATTACHMENT_ID);
}

OverlayEffectData* GetOverlayEffectData(ViewImpl& viewImpl)
{
  UniqueAny*                          attachment = viewImpl.GetAttachment(OVERLAY_EFFECT_DATA_ATTACHMENT_ID);
  Dali::UniquePtr<OverlayEffectData>* data       = attachment ? attachment->Get<Dali::UniquePtr<OverlayEffectData>>() : nullptr;
  return data ? data->Get() : nullptr;
}

std::vector<Dali::UniquePtr<OverlayEffectData>>& GetOverlayEffectDataPool()
{
  static std::vector<Dali::UniquePtr<OverlayEffectData>> pool;
  return pool;
}

std::vector<Dali::UniquePtr<OverlayEffectData>>& GetPendingReleaseOverlayEffectData()
{
  static std::vector<Dali::UniquePtr<OverlayEffectData>> pendingReleaseData;
  return pendingReleaseData;
}

} // namespace

Dali::UniquePtr<OverlayEffectData> AcquireOverlayEffectData();
void                               RecycleOverlayEffectData(Dali::UniquePtr<OverlayEffectData> data);
void                               RecyclePendingOverlayEffectData(OverlayEffectData& data);

OverlayEffectImpl::OverlayEffectImpl()
: OverlayEffectImpl(UiColor(DEFAULT_OVERLAY_COLOR_RGB, DEFAULT_OVERLAY_COLOR_ALPHA),
                    Insets(),
                    Vector4::ZERO,
                    CornerRadiusPolicy::ABSOLUTE,
                    true,
                    RecoilScope::OVERLAY_TARGET,
                    DEFAULT_RECOIL_SCALE_FACTOR)
{
}

OverlayEffectImpl::OverlayEffectImpl(const UiColor& overlayColor, const Insets& overlayPadding, const Vector4& cornerRadius, CornerRadiusPolicy cornerRadiusPolicy, bool useTargetCornerRadius, RecoilScope recoilScope, float recoilScaleFactor)
: Integration::StateEffectImpl(),
  mOverlayColor(overlayColor),
  mOverlayPadding(overlayPadding),
  mCornerRadius(cornerRadius),
  mCornerRadiusPolicy(cornerRadiusPolicy),
  mRecoilScope(recoilScope),
  mRecoilScaleFactor(recoilScaleFactor),
  mUseTargetCornerRadius(useTargetCornerRadius)
{
}

OverlayEffectImpl::OverlayEffectImpl(const OverlayEffectImpl& other)
: OverlayEffectImpl(other.mOverlayColor, other.mOverlayPadding, other.mCornerRadius, other.mCornerRadiusPolicy, other.mUseTargetCornerRadius, other.mRecoilScope, other.mRecoilScaleFactor)
{
}

OverlayEffectImpl::~OverlayEffectImpl() = default;

void OverlayEffectImpl::SetOverlayColor(const UiColor& color)
{
  mOverlayColor = color;
}

UiColor OverlayEffectImpl::GetOverlayColor() const
{
  return mOverlayColor;
}

void OverlayEffectImpl::SetOverlayPadding(const Insets& padding)
{
  DALI_ASSERT_ALWAYS(std::isfinite(padding.start) && padding.start >= 0.0f &&
                     std::isfinite(padding.end) && padding.end >= 0.0f &&
                     std::isfinite(padding.top) && padding.top >= 0.0f &&
                     std::isfinite(padding.bottom) && padding.bottom >= 0.0f &&
                     "OverlayEffect padding must be finite and non-negative");
  mOverlayPadding = padding;
}

Insets OverlayEffectImpl::GetOverlayPadding() const
{
  return mOverlayPadding;
}

void OverlayEffectImpl::SetCornerRadius(const Vector4& radius)
{
  mCornerRadius = radius;
}

Vector4 OverlayEffectImpl::GetCornerRadius() const
{
  return mCornerRadius;
}

void OverlayEffectImpl::SetCornerRadiusPolicy(CornerRadiusPolicy policy)
{
  mCornerRadiusPolicy = policy;
}

CornerRadiusPolicy OverlayEffectImpl::GetCornerRadiusPolicy() const
{
  return mCornerRadiusPolicy;
}

void OverlayEffectImpl::SetUseTargetCornerRadius(bool useTargetCornerRadius)
{
  mUseTargetCornerRadius = useTargetCornerRadius;
}

bool OverlayEffectImpl::IsUsingTargetCornerRadius() const
{
  return mUseTargetCornerRadius;
}

void OverlayEffectImpl::SetRecoilScope(RecoilScope scope)
{
  mRecoilScope = scope;
}

RecoilScope OverlayEffectImpl::GetRecoilScope() const
{
  return mRecoilScope;
}

void OverlayEffectImpl::SetRecoilScaleFactor(float factor)
{
  DALI_ASSERT_ALWAYS(std::isfinite(factor) && factor >= 0.0f && "OverlayEffect recoil scale factor must be finite and non-negative");
  mRecoilScaleFactor = factor;
}

float OverlayEffectImpl::GetRecoilScaleFactor() const
{
  return mRecoilScaleFactor;
}

void OverlayEffectImpl::HandleStateChanged(OverlayEffectData& data, View owner, const ViewState& state, RecoilRestoreMode restoreMode)
{
  data.SetState(state);
  if(state.IsAnyDisabled())
  {
    data.ApplyDisabledOpacity(owner);
  }
  else
  {
    data.RestoreDisabledOpacity(owner);
  }

  uint32_t activeCount = 0u;
  if(data.IsPressed())
  {
    ++activeCount;
  }
  if(data.IsFocusIndicated())
  {
    ++activeCount;
  }

  if(activeCount == 0u)
  {
    if(state.IsAnyDisabled())
    {
      CleanupOverlay(data, restoreMode);
    }
    else
    {
      ReleaseOverlayEffectData(owner, restoreMode);
    }
    return;
  }

  View target = ResolveTarget(owner);
  if(!target)
  {
    data.RestoreRecoil(restoreMode);
    return;
  }

  if(data.IsPressed())
  {
    data.ApplyRecoil(target, mRecoilScope, mRecoilScaleFactor);
  }
  else
  {
    data.RestoreRecoil(restoreMode);
  }

  ColorVisual overlay = data.GetActiveOverlay();
  if(!overlay)
  {
    // If overlay ColorVisual creation becomes a measured hot path, consider
    // applying an object pool at this creation point.
    overlay = ColorVisual::New();
    target.AddVisual(overlay, Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);
    data.SetActiveOverlay(overlay);
    data.SetActiveTarget(target);
  }
  else if(data.GetActiveTarget() != target)
  {
    overlay.Detach();
    target.AddVisual(overlay, Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);
    data.SetActiveTarget(target);
  }

  ApplyOverlayProperties(overlay, target);
  overlay.SetColor(GetOverlayColorWithActiveCount(mOverlayColor, activeCount));
}

void OverlayEffectImpl::CleanupOverlay(OverlayEffectData& data, RecoilRestoreMode restoreMode)
{
  data.RestoreRecoil(restoreMode);

  ColorVisual overlay = data.GetActiveOverlay();
  data.ClearActiveOverlay();
  data.ClearActiveTarget();
  if(overlay)
  {
    overlay.Detach();
  }
}

void OverlayEffectImpl::OnAttached(TraitId /*id*/, View& /*view*/)
{
}

void OverlayEffectImpl::OnDetaching(TraitId /*id*/, View& view)
{
  ReleaseOverlayEffectData(view, RecoilRestoreMode::IMMEDIATE);
}

void OverlayEffectImpl::OnViewDestroying(ViewImpl* viewImpl)
{
  OverlayEffectData* data = viewImpl ? GetOverlayEffectData(*viewImpl) : nullptr;
  if(data)
  {
    data->DetachEffect(RecoilRestoreMode::IMMEDIATE);
  }
}

void OverlayEffectImpl::OnViewStateChanged(View view, const StateEvent& event)
{
  const bool anyDisabledChanged = event.GetPrev().IsAnyDisabled() != event.GetCurrent().IsAnyDisabled();
  if(!event.Changed(ViewState::PRESSED) && !event.Changed(ViewState::FOCUS_INDICATED) && !anyDisabledChanged)
  {
    return;
  }

  OverlayEffectData* existingData = GetOverlayEffectData(view);
  if(!IsOverlayActive(event.GetCurrent()) && !event.GetCurrent().IsAnyDisabled() && !existingData)
  {
    return;
  }

  OverlayEffectData& data = existingData ? *existingData : GetOrCreateOverlayEffectData(view);

  const bool isCancellation = event.Removed(ViewState::PRESSED) && event.GetCause().IsCancellation();
  HandleStateChanged(data, view, event.GetCurrent(), isCancellation ? RecoilRestoreMode::IMMEDIATE : RecoilRestoreMode::ANIMATE);
}

void OverlayEffectImpl::OnInteractiveAttached(View view)
{
  if(!IsOverlayActive(view.GetState()) && !view.GetState().IsAnyDisabled())
  {
    return;
  }

  OverlayEffectData& data = GetOrCreateOverlayEffectData(view);
  HandleStateChanged(data, view, view.GetState());
}

bool OverlayEffectImpl::ShouldSuppressDefaultFocusIndicator(View /*view*/) const
{
  return true;
}

void OverlayEffectImpl::OnStateEffectTargetsChanged(View view)
{
  OverlayEffectData* data = GetOverlayEffectData(view);
  if(data)
  {
    HandleStateChanged(*data, view, view.GetState());
  }
}

OverlayEffectData& OverlayEffectImpl::GetOrCreateOverlayEffectData(View owner)
{
  OverlayEffectData* data = GetOverlayEffectData(owner);
  if(!data)
  {
    owner.SetAttachment(OVERLAY_EFFECT_DATA_ATTACHMENT_ID, AcquireOverlayEffectData());
    data = GetOverlayEffectData(owner);
  }
  DALI_ASSERT_ALWAYS(data && "OverlayEffectData attachment creation failed");
  return *data;
}

void OverlayEffectImpl::ReleaseOverlayEffectData(View owner, RecoilRestoreMode restoreMode)
{
  if(!owner)
  {
    return;
  }

  Dali::UniquePtr<OverlayEffectData> data = owner.DetachAttachment<OverlayEffectData>(OVERLAY_EFFECT_DATA_ATTACHMENT_ID);
  if(data)
  {
    data->DetachEffect(owner, restoreMode);
    if(restoreMode == RecoilRestoreMode::ANIMATE && data->HasRunningRecoilAnimation())
    {
      data->SetPendingRelease();
      GetPendingReleaseOverlayEffectData().push_back(Dali::Move(data));
    }
    else
    {
      RecycleOverlayEffectData(Dali::Move(data));
    }
  }
}

Dali::UniquePtr<OverlayEffectData> AcquireOverlayEffectData()
{
  auto& pool = GetOverlayEffectDataPool();
  for(auto iter = pool.rbegin(); iter != pool.rend(); ++iter)
  {
    if(*iter && !(*iter)->HasRunningRecoilAnimation())
    {
      Dali::UniquePtr<OverlayEffectData> data = Dali::Move(*iter);
      pool.erase(std::next(iter).base());
      return data;
    }
  }

  return Dali::MakeUnique<OverlayEffectData>();
}

void RecycleOverlayEffectData(Dali::UniquePtr<OverlayEffectData> data)
{
  auto& pool = GetOverlayEffectDataPool();
  if(data && pool.size() < OVERLAY_EFFECT_DATA_POOL_MAX_SIZE)
  {
    pool.push_back(Dali::Move(data));
  }
}

void RecyclePendingOverlayEffectData(OverlayEffectData& data)
{
  auto& pendingReleaseData = GetPendingReleaseOverlayEffectData();
  auto  iter               = std::find_if(pendingReleaseData.begin(),
                                          pendingReleaseData.end(),
                                          [&data](const Dali::UniquePtr<OverlayEffectData>& item)
                 {
    return item.Get() == &data;
  });

  if(iter == pendingReleaseData.end())
  {
    return;
  }

  Dali::UniquePtr<OverlayEffectData> pendingData = Dali::Move(*iter);
  pendingReleaseData.erase(iter);

  pendingData->ClearPendingRelease();
  RecycleOverlayEffectData(Dali::Move(pendingData));
}

View OverlayEffectImpl::ResolveTarget(View owner) const
{
  if(!owner)
  {
    return View();
  }

  View target = owner.GetStateEffectTarget();
  return target ? target : owner;
}

void OverlayEffectImpl::ApplyOverlayProperties(ColorVisual overlay, View target) const
{
  const bool  isRightToLeft = target.GetEffectiveLayoutDirection() == Dali::LayoutDirection::RIGHT_TO_LEFT;
  const float left          = isRightToLeft ? mOverlayPadding.end : mOverlayPadding.start;
  const float right         = isRightToLeft ? mOverlayPadding.start : mOverlayPadding.end;
  const float top           = mOverlayPadding.top;
  const float bottom        = mOverlayPadding.bottom;

  overlay.SetColor(mOverlayColor);
  overlay.SetProportionFlags(Visual::Transform::ProportionFlags::SIZE_PROPORTIONAL);
  overlay.SetOffsetX(-left);
  overlay.SetOffsetY(-top);
  overlay.SetWidth(1.0f);
  overlay.SetHeight(1.0f);
  overlay.SetExtraWidth(left + right);
  overlay.SetExtraHeight(top + bottom);

  if(mUseTargetCornerRadius)
  {
    overlay.SetCornerRadius(target.GetCornerRadius());
    overlay.SetCornerRadiusPolicy(target.GetCornerRadiusPolicy());
  }
  else
  {
    overlay.SetCornerRadius(mCornerRadius);
    overlay.SetCornerRadiusPolicy(mCornerRadiusPolicy);
  }
}

OverlayEffectData::OverlayEffectData()
: mActiveOverlay(),
  mActiveTarget(),
  mRecoilBaseTarget(),
  mRecoilScope(RecoilScope::OVERLAY_TARGET),
  mRecoilTargets(),
  mRecoilAnimation(),
  mRecoilScaleFactor(DEFAULT_RECOIL_SCALE_FACTOR),
  mDisabledOriginalOpacity(INVALID_DISABLED_ORIGINAL_OPACITY),
  mPendingReleaseAnimationId(0u),
  mRecoilTargetCount(0u),
  mPendingReleaseAfterRestore(false),
  mPressed(false),
  mFocusIndicated(false)
{
}

OverlayEffectData::~OverlayEffectData()
{
  DetachEffect();
}

void OverlayEffectData::DetachEffect(RecoilRestoreMode restoreMode)
{
  DetachEffect(View(), restoreMode);
}

void OverlayEffectData::DetachEffect(View owner, RecoilRestoreMode restoreMode)
{
  ClearPendingRelease();

  RestoreRecoil(restoreMode);

  ColorVisual overlay = mActiveOverlay;
  ClearActiveOverlay();
  ClearActiveTarget();
  if(overlay)
  {
    overlay.Detach();
  }

  RestoreDisabledOpacity(owner);
  mPressed        = false;
  mFocusIndicated = false;
}

void OverlayEffectData::SetActiveOverlay(ColorVisual overlay)
{
  mActiveOverlay = overlay;
}

ColorVisual OverlayEffectData::GetActiveOverlay() const
{
  return mActiveOverlay;
}

bool OverlayEffectData::HasActiveOverlay() const
{
  return !!mActiveOverlay;
}

void OverlayEffectData::ClearActiveOverlay()
{
  mActiveOverlay.Reset();
}

void OverlayEffectData::SetActiveTarget(View target)
{
  mActiveTarget = target;
}

View OverlayEffectData::GetActiveTarget() const
{
  return mActiveTarget;
}

void OverlayEffectData::ClearActiveTarget()
{
  mActiveTarget.Reset();
}

bool OverlayEffectData::IsPressed() const
{
  return mPressed;
}

bool OverlayEffectData::IsFocusIndicated() const
{
  return mFocusIndicated;
}

void OverlayEffectData::SetState(const ViewState& state)
{
  mPressed        = state.Contains(ViewState::PRESSED);
  mFocusIndicated = state.Contains(ViewState::FOCUS_INDICATED);
}

void OverlayEffectData::ApplyDisabledOpacity(View owner)
{
  if(!owner || mDisabledOriginalOpacity >= 0.0f)
  {
    return;
  }

  mDisabledOriginalOpacity = owner.GetOpacity();
  owner.SetOpacity(std::clamp(mDisabledOriginalOpacity * DISABLED_OPACITY_FACTOR, 0.0f, 1.0f));
}

void OverlayEffectData::RestoreDisabledOpacity(View owner)
{
  if(mDisabledOriginalOpacity < 0.0f)
  {
    return;
  }

  if(owner)
  {
    const float currentOpacity  = owner.GetOpacity();
    const float dimmedOpacity   = std::clamp(mDisabledOriginalOpacity * DISABLED_OPACITY_FACTOR, 0.0f, 1.0f);
    const float restoredOpacity = std::clamp(mDisabledOriginalOpacity, 0.0f, 1.0f);
    if(ApproximatelyEqual(currentOpacity, dimmedOpacity))
    {
      owner.SetOpacity(restoredOpacity);
    }
  }

  mDisabledOriginalOpacity = INVALID_DISABLED_ORIGINAL_OPACITY;
}

void OverlayEffectData::ApplyRecoil(View overlayTarget, RecoilScope scope, float scaleFactor)
{
  if(!overlayTarget)
  {
    RestoreRecoil();
    return;
  }

  if(mRecoilTargetCount > 0u && mRecoilBaseTarget.GetHandle() == overlayTarget && mRecoilScope == scope && ApproximatelyEqual(mRecoilScaleFactor, scaleFactor))
  {
    return;
  }

  if(mRecoilAnimation && mRecoilTargetCount == 0u)
  {
    ClearPendingRelease();
    mRecoilAnimation.Stop();
    mRecoilAnimation.Reset();
  }

  RestoreRecoil();

  Animation animation = Animation::New(RECOIL_ANIMATION_SECONDS);
  bool      animated  = false;

  auto addRecoilTarget = [&](View target)
  {
    if(!target || mRecoilTargetCount >= MAX_RECOIL_TARGET_COUNT)
    {
      return;
    }

    RecoilTarget item;
    item.view           = target;
    item.originalScale  = GetBaseScale(target);
    Vector2 recoilScale = item.originalScale * scaleFactor;

    if(scope == RecoilScope::OVERLAY_TARGET_CHILDREN)
    {
      Vector3 recoilPivot;
      if(ComputeParentCenterPivot(overlayTarget, target, recoilPivot))
      {
        item.originalPivot   = target.GetPivot();
        item.recoilPivot     = recoilPivot;
        item.usesRecoilPivot = true;
        target.SetPivot(recoilPivot);
      }
    }

    AddScaleAnimation(animation, target, recoilScale);
    mRecoilTargets[mRecoilTargetCount++] = item;
    animated                             = true;
  };

  if(scope == RecoilScope::OVERLAY_TARGET)
  {
    addRecoilTarget(overlayTarget);
  }
  else
  {
    const uint32_t childCount = GetImpl(overlayTarget).GetChildViewCount();
    if(childCount == 0u || childCount > MAX_RECOIL_TARGET_COUNT)
    {
      return;
    }

    for(uint32_t i = 0u; i < childCount; ++i)
    {
      addRecoilTarget(GetImpl(overlayTarget).GetChildViewAt(i));
    }
  }

  if(animated)
  {
    mRecoilBaseTarget  = overlayTarget;
    mRecoilScope       = scope;
    mRecoilScaleFactor = scaleFactor;
    mRecoilAnimation   = animation;
    mRecoilAnimation.Play();
  }
}

void OverlayEffectData::RestoreRecoil(RecoilRestoreMode restoreMode)
{
  if(mRecoilTargetCount == 0u)
  {
    mRecoilBaseTarget.Reset();
    if(mRecoilAnimation && mRecoilAnimation.GetState() != Animation::PLAYING)
    {
      ClearPendingRelease();
      mRecoilAnimation.Reset();
    }
    return;
  }

  if(mRecoilAnimation)
  {
    ClearPendingRelease();
    mRecoilAnimation.Stop();
    mRecoilAnimation.Reset();
  }

  Animation animation;
  bool      animated = false;
  if(restoreMode == RecoilRestoreMode::ANIMATE)
  {
    animation = Animation::New(RECOIL_ANIMATION_SECONDS);
    animation.SetEndAction(Animation::BAKE_FINAL);
  }

  for(uint32_t i = 0u; i < mRecoilTargetCount; ++i)
  {
    const RecoilTarget& item   = mRecoilTargets[i];
    View                target = item.view.GetHandle();
    if(!target)
    {
      continue;
    }

    if(restoreMode == RecoilRestoreMode::IMMEDIATE)
    {
      target.SetScale(Vector3(item.originalScale.x, item.originalScale.y, 1.0f));
    }
    else
    {
      AddScaleAnimation(animation, target, item.originalScale);
      animated = true;
    }

    if(item.usesRecoilPivot && ApproximatelyEqual(target.GetPivot(), item.recoilPivot))
    {
      // NOTE: Restore the temporary child recoil pivot immediately to keep recoil
      // cleanup simple. Current visual tests do not show artifacts; if pivot jumps
      // become visible, move this restoration to the animation-finished path.
      target.SetPivot(item.originalPivot);
    }
  }

  ClearRecoilTargets();
  mRecoilBaseTarget.Reset();

  if(animated)
  {
    mRecoilAnimation = animation;
    mRecoilAnimation.Play();
  }
}

bool OverlayEffectData::HasRunningRecoilAnimation() const
{
  return mRecoilAnimation && mRecoilAnimation.GetState() == Animation::PLAYING;
}

void OverlayEffectData::SetPendingRelease()
{
  ClearPendingRelease();

  if(!mRecoilAnimation)
  {
    return;
  }

  mPendingReleaseAnimationId  = mRecoilAnimation.GetAnimationId();
  mPendingReleaseAfterRestore = true;
  mRecoilAnimation.FinishedSignal().Connect(this, &OverlayEffectData::OnPendingReleaseAnimationFinished);
}

void OverlayEffectData::ClearPendingRelease()
{
  // Animation::Stop() does not emit FinishedSignal() synchronously, but it may emit
  // it later after update/render notification. Disconnect the pending restore
  // callback before Stop()/Reset() so a cancelled restore cannot release recycled data.
  if(mPendingReleaseAfterRestore && mRecoilAnimation)
  {
    mRecoilAnimation.FinishedSignal().Disconnect(this, &OverlayEffectData::OnPendingReleaseAnimationFinished);
  }

  mPendingReleaseAnimationId  = 0u;
  mPendingReleaseAfterRestore = false;
}

void OverlayEffectData::OnPendingReleaseAnimationFinished(Animation animation)
{
  if(!mPendingReleaseAfterRestore || animation.GetAnimationId() != mPendingReleaseAnimationId)
  {
    return;
  }

  RecyclePendingOverlayEffectData(*this);
}

void OverlayEffectData::ClearRecoilTargets()
{
  for(uint32_t i = 0u; i < mRecoilTargetCount; ++i)
  {
    mRecoilTargets[i] = RecoilTarget();
  }
  mRecoilTargetCount = 0u;
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
