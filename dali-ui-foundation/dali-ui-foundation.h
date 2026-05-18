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
#include <dali/dali.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/animation/duration.h>
#include <dali-ui-foundation/public-api/animation/label-animation-bridge.autogen.h>
#include <dali-ui-foundation/public-api/animation/label-animation-spec.autogen.h>
#include <dali-ui-foundation/public-api/animation/view-animation-bridge.autogen.h>
#include <dali-ui-foundation/public-api/animation/view-animation-spec.autogen.h>
#include <dali-ui-foundation/public-api/attachment-id.h>
#include <dali-ui-foundation/public-api/input-editor.h>
#include <dali-ui-foundation/public-api/input-field.h>
#include <dali-ui-foundation/public-api/interactive-view.h>
#include <dali-ui-foundation/public-api/label.h>
#include <dali-ui-foundation/public-api/trait-id.h>
#include <dali-ui-foundation/public-api/trait-object.h>
#include <dali-ui-foundation/public-api/ui-color-manager.h>
#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali-ui-foundation/public-api/ui-config.h>
#include <dali-ui-foundation/public-api/ui-scale-manager.h>
#include <dali-ui-foundation/public-api/ui-scale-policy.h>
#include <dali-ui-foundation/public-api/unique-any.h>
#include <dali-ui-foundation/public-api/unit.h>
#include <dali-ui-foundation/public-api/view-impl.h>
#include <dali-ui-foundation/public-api/view-types.h>
#include <dali-ui-foundation/public-api/view.h>

#include <dali-ui-foundation/public-api/image-view.h>

#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>

#include <dali-ui-foundation/public-api/gradient/gradient-enumerations.h>
#include <dali-ui-foundation/public-api/gradient/gradient-stop-node.h>

#include <dali-ui-foundation/public-api/image/animated-image-enumerations.h>
#include <dali-ui-foundation/public-api/image/image-enumerations.h>
#include <dali-ui-foundation/public-api/image/lottie-animation-enumerations.h>
#include <dali-ui-foundation/public-api/image/lottie-animation-types.h>

#include <dali-ui-foundation/public-api/image-loader/async-image-loader.h>
#include <dali-ui-foundation/public-api/image-loader/image-url-utils.h>
#include <dali-ui-foundation/public-api/image-loader/image-url.h>
#include <dali-ui-foundation/public-api/image-loader/sync-image-loader.h>

#include <dali-ui-foundation/public-api/render-effects/background-blur-effect.h>
#include <dali-ui-foundation/public-api/render-effects/gaussian-blur-effect.h>
#include <dali-ui-foundation/public-api/render-effects/mask-effect.h>
#include <dali-ui-foundation/public-api/render-effects/render-effect.h>

#include <dali-ui-foundation/public-api/text/fit/text-fit-candidate.h>
#include <dali-ui-foundation/public-api/text/fit/text-fit-range.h>
#include <dali-ui-foundation/public-api/text/font-variation/font-variation-axis.h>
#include <dali-ui-foundation/public-api/text/font-variation/font-variation.h>
#include <dali-ui-foundation/public-api/text/input-filter.h>
#include <dali-ui-foundation/public-api/text/style/bevel.h>
#include <dali-ui-foundation/public-api/text/style/line-through.h>
#include <dali-ui-foundation/public-api/text/style/outline.h>
#include <dali-ui-foundation/public-api/text/style/shadow.h>
#include <dali-ui-foundation/public-api/text/style/underline.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>

#include <dali-ui-foundation/public-api/visuals/border-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/color-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/gradient-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/image-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/mesh-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/primitive-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/text-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

#include <dali-ui-foundation/public-api/visuals/animated-image-visual.h>
#include <dali-ui-foundation/public-api/visuals/border-visual.h>
#include <dali-ui-foundation/public-api/visuals/color-visual.h>
#include <dali-ui-foundation/public-api/visuals/gradient-visual.h>
#include <dali-ui-foundation/public-api/visuals/image-visual.h>
#include <dali-ui-foundation/public-api/visuals/lottie-animation-visual.h>
#include <dali-ui-foundation/public-api/visuals/visual-base.h>

#include <dali-ui-foundation/public-api/layouts/absolute-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/absolute-layout.h>
#include <dali-ui-foundation/public-api/layouts/flex-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/flex-layout.h>
#include <dali-ui-foundation/public-api/layouts/grid-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/grid-layout.h>
#include <dali-ui-foundation/public-api/layouts/layout-controller.h>
#include <dali-ui-foundation/public-api/layouts/layout-params.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/layouts/layout.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>

#include <dali-ui-foundation/public-api/align-enumerations.h>
#include <dali-ui-foundation/public-api/ui-property-index-ranges.h>

#include <dali-ui-foundation/public-api/i-scroll-bar.h>
#include <dali-ui-foundation/public-api/scroll-bar.h>
#include <dali-ui-foundation/public-api/scroll-view.h>
#include <dali-ui-foundation/public-api/scrollable-enum.h>
