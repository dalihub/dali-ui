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
#include <dali-ui-foundation/internal/animation/label-animation-spec-impl.autogen.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/label.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{
Dali::TypeRegistration LABEL_ANIMATION_SPEC_TYPE("LabelAnimationSpec", typeid(Dali::Ui::ViewAnimationSpec), nullptr);
Dali::TypeRegistration LABEL_ANIMATION_SPEC_IMPL_TYPE(typeid(Dali::Ui::Internal::LabelAnimationSpecImpl), typeid(Dali::Ui::LabelAnimationSpec), nullptr);
} // namespace

LabelAnimationSpecImpl::LabelAnimationSpecImpl() = default;

LabelAnimationSpecImpl::~LabelAnimationSpecImpl() = default;

LabelAnimationSpecImplPtr LabelAnimationSpecImpl::New()
{
  return LabelAnimationSpecImplPtr(new LabelAnimationSpecImpl());
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
