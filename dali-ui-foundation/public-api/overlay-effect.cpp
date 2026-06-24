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
#include <dali-ui-foundation/public-api/overlay-effect.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

#include <utility>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/state-effects/overlay-effect-impl.h>

namespace Dali
{
namespace Ui
{

namespace
{

OverlayEffect CreatePlainOverlayEffect()
{
  return OverlayEffect::Builder().Build();
}

OverlayEffect CreateRoundOverlayEffect()
{
  return OverlayEffect::Builder()
    .SetCornerRadius(Vector4(0.5f, 0.5f, 0.5f, 0.5f))
    .SetCornerRadiusPolicyRelative()
    .Build();
}

OverlayEffect CreateListItemOverlayEffect()
{
  return OverlayEffect::Builder()
    .SetRecoilScope(RecoilScope::OVERLAY_TARGET_CHILDREN)
    .Build();
}

} // namespace

const OverlayEffect& OverlayEffect::Plain()
{
  static const OverlayEffect effect = CreatePlainOverlayEffect();
  return effect;
}

const OverlayEffect& OverlayEffect::Round()
{
  static const OverlayEffect effect = CreateRoundOverlayEffect();
  return effect;
}

const OverlayEffect& OverlayEffect::ListItem()
{
  static const OverlayEffect effect = CreateListItemOverlayEffect();
  return effect;
}

OverlayEffect OverlayEffect::DownCast(BaseHandle handle)
{
  return OverlayEffect(dynamic_cast<Internal::OverlayEffectImpl*>(handle.GetObjectPtr()));
}

UiColor OverlayEffect::GetOverlayColor() const
{
  return GetImpl(*this).GetOverlayColor();
}

Vector4 OverlayEffect::GetCornerRadius() const
{
  return GetImpl(*this).GetCornerRadius();
}

CornerRadiusPolicy OverlayEffect::GetCornerRadiusPolicy() const
{
  return GetImpl(*this).GetCornerRadiusPolicy();
}

OverlayEffect::Builder OverlayEffect::Configure() const
{
  IntrusivePtr<Internal::OverlayEffectImpl> impl(new Internal::OverlayEffectImpl(GetImpl(*this)));
  return Builder(impl.Get());
}

bool OverlayEffect::IsUsingTargetCornerRadius() const
{
  return GetImpl(*this).IsUsingTargetCornerRadius();
}

RecoilScope OverlayEffect::GetRecoilScope() const
{
  return GetImpl(*this).GetRecoilScope();
}

OverlayEffect::OverlayEffect(Internal::OverlayEffectImpl* impl)
: StateEffect(impl)
{
}

OverlayEffect::Builder::Builder()
: mImpl(new Internal::OverlayEffectImpl())
{
}

OverlayEffect::Builder::Builder(Builder&& rhs) noexcept = default;

OverlayEffect::Builder& OverlayEffect::Builder::operator=(Builder&& rhs) noexcept = default;

OverlayEffect::Builder::~Builder() = default;

OverlayEffect::Builder& OverlayEffect::Builder::SetOverlayColor(const UiColor& color) &
{
  mImpl->SetOverlayColor(color);
  return *this;
}

OverlayEffect::Builder&& OverlayEffect::Builder::SetOverlayColor(const UiColor& color) &&
{
  SetOverlayColor(color);
  return std::move(*this);
}

OverlayEffect::Builder& OverlayEffect::Builder::SetCornerRadius(float radius) &
{
  return SetCornerRadius(Vector4(radius, radius, radius, radius));
}

OverlayEffect::Builder&& OverlayEffect::Builder::SetCornerRadius(float radius) &&
{
  SetCornerRadius(radius);
  return std::move(*this);
}

OverlayEffect::Builder& OverlayEffect::Builder::SetCornerRadius(float topLeft, float topRight, float bottomRight, float bottomLeft) &
{
  return SetCornerRadius(Vector4(topLeft, topRight, bottomRight, bottomLeft));
}

OverlayEffect::Builder&& OverlayEffect::Builder::SetCornerRadius(float topLeft, float topRight, float bottomRight, float bottomLeft) &&
{
  SetCornerRadius(topLeft, topRight, bottomRight, bottomLeft);
  return std::move(*this);
}

OverlayEffect::Builder& OverlayEffect::Builder::SetCornerRadius(const Vector4& radius) &
{
  mImpl->SetCornerRadius(radius);
  mImpl->SetUseTargetCornerRadius(false);
  return *this;
}

OverlayEffect::Builder&& OverlayEffect::Builder::SetCornerRadius(const Vector4& radius) &&
{
  SetCornerRadius(radius);
  return std::move(*this);
}

OverlayEffect::Builder& OverlayEffect::Builder::SetCornerRadiusPolicy(CornerRadiusPolicy policy) &
{
  mImpl->SetCornerRadiusPolicy(policy);
  mImpl->SetUseTargetCornerRadius(false);
  return *this;
}

OverlayEffect::Builder&& OverlayEffect::Builder::SetCornerRadiusPolicy(CornerRadiusPolicy policy) &&
{
  SetCornerRadiusPolicy(policy);
  return std::move(*this);
}

OverlayEffect::Builder& OverlayEffect::Builder::SetCornerRadiusPolicyRelative() &
{
  return SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE);
}

OverlayEffect::Builder&& OverlayEffect::Builder::SetCornerRadiusPolicyRelative() &&
{
  SetCornerRadiusPolicyRelative();
  return std::move(*this);
}

OverlayEffect OverlayEffect::Builder::Build() &&
{
  DALI_ASSERT_ALWAYS(mImpl && "OverlayEffect::Builder has already been consumed");
  OverlayEffect effect(mImpl.Get());
  mImpl.Reset();
  return effect;
}

OverlayEffect::Builder& OverlayEffect::Builder::SetUseTargetCornerRadius(bool useTargetCornerRadius) &
{
  mImpl->SetUseTargetCornerRadius(useTargetCornerRadius);
  return *this;
}

OverlayEffect::Builder&& OverlayEffect::Builder::SetUseTargetCornerRadius(bool useTargetCornerRadius) &&
{
  SetUseTargetCornerRadius(useTargetCornerRadius);
  return std::move(*this);
}

OverlayEffect::Builder& OverlayEffect::Builder::SetRecoilScope(RecoilScope scope) &
{
  mImpl->SetRecoilScope(scope);
  return *this;
}

OverlayEffect::Builder&& OverlayEffect::Builder::SetRecoilScope(RecoilScope scope) &&
{
  SetRecoilScope(scope);
  return std::move(*this);
}

OverlayEffect::Builder::Builder(Internal::OverlayEffectImpl* impl)
: mImpl(impl)
{
}

} // namespace Ui
} // namespace Dali
