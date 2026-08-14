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

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/animation/view-animation-spec-impl.autogen.h>
#include <dali-ui-foundation/public-api/animation/label-animation-spec.autogen.h>

namespace Dali
{
namespace Ui
{

class Label;

namespace Internal
{

class LabelAnimationSpecImpl;
using LabelAnimationSpecImplPtr = IntrusivePtr<LabelAnimationSpecImpl>;

/**
 * @brief Internal implementation of LabelAnimationSpec.
 *
 * Inherits from ViewAnimationSpecImpl; reuses AddAnimateTo/ByEntry, ApplyEntries, ApplyAnimateTo/By.
 */
class DALI_UI_API LabelAnimationSpecImpl : public ViewAnimationSpecImpl
{
public:
  static LabelAnimationSpecImplPtr New();

  static void ApplyTextGradientStartOffsetTo(Animation& animation, Label view, const Entry& entry);
  static void ApplyTextGradientStartOffsetBy(Animation& animation, Label view, const Entry& entry);
  static void ApplyTextGradientOverlayStartOffsetTo(Animation& animation, Label view, const Entry& entry);
  static void ApplyTextGradientOverlayStartOffsetBy(Animation& animation, Label view, const Entry& entry);
  static void ApplyTextRevealProgressTo(Animation& animation, Label view, const Entry& entry);
  static void ApplyTextRevealProgressBy(Animation& animation, Label view, const Entry& entry);
  static void ApplyPixelSnapFactorTo(Animation& animation, Label view, const Entry& entry);
  static void ApplyPixelSnapFactorBy(Animation& animation, Label view, const Entry& entry);

protected:
  LabelAnimationSpecImpl();
  ~LabelAnimationSpecImpl() override;

private:
  LabelAnimationSpecImpl(const LabelAnimationSpecImpl&) = delete;
  LabelAnimationSpecImpl(LabelAnimationSpecImpl&&) = delete;
  LabelAnimationSpecImpl& operator=(const LabelAnimationSpecImpl&) = delete;
  LabelAnimationSpecImpl& operator=(LabelAnimationSpecImpl&&) = delete;
};

inline Internal::LabelAnimationSpecImpl& GetImpl(LabelAnimationSpec& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Internal::LabelAnimationSpecImpl&>(handle);
}

inline const Internal::LabelAnimationSpecImpl& GetImpl(const LabelAnimationSpec& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Internal::LabelAnimationSpecImpl&>(handle);
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
