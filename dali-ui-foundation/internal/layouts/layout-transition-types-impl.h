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
#include <dali-ui-foundation/public-api/layouts/layout-transition-types.h>

namespace DALI_NAMESPACE
{
namespace Ui
{

struct DALI_INTERNAL LayoutTransitionTiming::Impl
{
  Duration      duration;
  AlphaFunction alpha;
  Duration      delay;
};

struct DALI_INTERNAL LayoutAnimatorTiming::Impl
{
  Duration      duration;
  AlphaFunction alpha;
  Duration      delay;
};

struct DALI_INTERNAL LayoutBoundsEffect::Impl
{
  LayoutTransitionTiming timing;
  bool                   hasOffset{false};
  LayoutBoundsOffset     offset;
  bool                   hasSizeFactor{false};
  float                  sizeFactorX{1.0f};
  float                  sizeFactorY{1.0f};
  float                  anchorX{0.5f};
  float                  anchorY{0.5f};
  LayoutBoundsClipMode   clipMode{LayoutBoundsClipMode::AUTO};
};

/**
 * @brief Storage for a callback invocation or an owned context snapshot.
 *
 * DispatchOneTick keeps this storage on its stack so cancellation or re-entry
 * cannot invalidate the context while the callback is still executing.
 */
struct DALI_INTERNAL LayoutAnimatorContext::Impl
{
  View                 view;
  LayoutTransitionSlot slot{LayoutTransitionSlot::ENTER};
  LayoutChangeCause    changeCause{LayoutChangeCause::OTHER};
  float                progress{0.0f};
  float                rawProgress{0.0f};
  LayoutRect           fromBounds;
  LayoutRect           toBounds;
};

} // namespace Ui
} // namespace DALI_NAMESPACE
