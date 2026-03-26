
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
#include <dali-ui-foundation/public-api/render-effects/mask-effect.h>
// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/render-effects/mask-effect-impl.h>
namespace Dali
{
namespace Ui
{
MaskEffect::MaskEffect() = default;
MaskEffect::MaskEffect(const MaskEffect& handle)
: RenderEffect(handle)
{
}

MaskEffect::MaskEffect(Internal::MaskEffectImpl* maskEffectImpl)
: RenderEffect(maskEffectImpl)
{
}

MaskEffect::~MaskEffect() = default;

MaskEffect MaskEffect::New(Ui::View maskView)
{
  Internal::MaskEffectImplPtr internal = Internal::MaskEffectImpl::New(maskView);
  return MaskEffect(internal.Get());
}

void MaskEffect::SetTargetMaskOnce(bool targetMaskOnce)
{
  GetImplementation(*this).SetTargetMaskOnce(targetMaskOnce);
}

bool MaskEffect::GetTargetMaskOnce() const
{
  return GetImplementation(*this).GetTargetMaskOnce();
}

void MaskEffect::SetSourceMaskOnce(bool sourceMaskOnce)
{
  GetImplementation(*this).SetSourceMaskOnce(sourceMaskOnce);
}

bool MaskEffect::GetSourceMaskOnce() const
{
  return GetImplementation(*this).GetSourceMaskOnce();
}

MaskEffect MaskEffect::New(Ui::View maskView, MaskMode maskMode, Vector2 maskPosition, Vector2 maskScale)
{
  Internal::MaskEffectImplPtr internal = Internal::MaskEffectImpl::New(maskView, maskMode, maskPosition, maskScale);
  return MaskEffect(internal.Get());
}
} // namespace Ui
} // namespace Dali
