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
#include <dali-ui-foundation/public-api/animation/input-editor-animation-bridge.autogen.h>
#include <dali-ui-foundation/public-api/animation/input-editor-animation-spec.autogen.h>
#include <dali-ui-foundation/public-api/animation/input-field-animation-bridge.autogen.h>
#include <dali-ui-foundation/public-api/animation/input-field-animation-spec.autogen.h>
#include <dali-ui-foundation/public-api/animation/label-animation-bridge.autogen.h>
#include <dali-ui-foundation/public-api/animation/label-animation-spec.autogen.h>
#include <dali-ui-foundation/public-api/animation/view-animation-bridge.autogen.h>
#include <dali-ui-foundation/public-api/animation/view-animation-spec.autogen.h>
#include <dali-ui-foundation/public-api/configuration/ui-color-manager.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-foundation/public-api/configuration/ui-localization-manager.h>
#include <dali-ui-foundation/public-api/configuration/ui-scale-manager.h>
#include <dali-ui-foundation/public-api/configuration/ui-scale-policy.h>
#include <dali-ui-foundation/public-api/styles/ui-style-key.h>
#include <dali-ui-foundation/public-api/styles/ui-style-sheet.h>
#include <dali-ui-foundation/public-api/styles/ui-style.h>
#include <dali-ui-foundation/public-api/traits/attachment-id.h>
#include <dali-ui-foundation/public-api/traits/trait-id.h>
#include <dali-ui-foundation/public-api/traits/trait-object.h>
#include <dali-ui-foundation/public-api/types/inner-shadow.h>
#include <dali-ui-foundation/public-api/types/insets.h>
#include <dali-ui-foundation/public-api/types/selectable-lottie-image.h>
#include <dali-ui-foundation/public-api/types/shadow.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/types/unique-any.h>
#include <dali-ui-foundation/public-api/types/unit.h>
#include <dali-ui-foundation/public-api/views/effects/overlay-effect.h>
#include <dali-ui-foundation/public-api/views/effects/shadow-stack.h>
#include <dali-ui-foundation/public-api/views/effects/state-effect.h>
#include <dali-ui-foundation/public-api/views/group-selectable-view.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali-ui-foundation/public-api/views/image/selectable-image-interface.h>
#include <dali-ui-foundation/public-api/views/image/selectable-lottie-animation-view.h>
#include <dali-ui-foundation/public-api/views/interactive-view.h>
#include <dali-ui-foundation/public-api/views/recycler/item-adapter.h>
#include <dali-ui-foundation/public-api/views/recycler/items-layouter.h>
#include <dali-ui-foundation/public-api/views/recycler/linear-items-layouter.h>
#include <dali-ui-foundation/public-api/views/recycler/recycler-view.h>
#include <dali-ui-foundation/public-api/views/selectable-view.h>
#include <dali-ui-foundation/public-api/views/selection-group.h>
#include <dali-ui-foundation/public-api/views/text-controls/input-editor.h>
#include <dali-ui-foundation/public-api/views/text-controls/input-field.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/views/view-types.h>
#include <dali-ui-foundation/public-api/views/view.h>

#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-navigation-callback.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-navigation-context.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-navigation-result.h>

#include <dali-ui-foundation/public-api/drag-and-drop/drag-and-drop-detector.h>
#include <dali-ui-foundation/public-api/drag-and-drop/drag-and-drop-types.h>

#include <dali-ui-foundation/public-api/gradient/conic-gradient.h>
#include <dali-ui-foundation/public-api/gradient/gradient-base.h>
#include <dali-ui-foundation/public-api/gradient/gradient-enumerations.h>
#include <dali-ui-foundation/public-api/gradient/gradient-stop-node.h>
#include <dali-ui-foundation/public-api/gradient/linear-gradient.h>
#include <dali-ui-foundation/public-api/gradient/radial-gradient.h>

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

#include <dali-ui-foundation/public-api/text/fit/text-fit.h>
#include <dali-ui-foundation/public-api/text/font-variation/font-variation-axis.h>
#include <dali-ui-foundation/public-api/text/font-variation/font-variation.h>
#include <dali-ui-foundation/public-api/text/input-filter.h>
#include <dali-ui-foundation/public-api/text/style/anchor-attributes.h>
#include <dali-ui-foundation/public-api/text/style/bevel.h>
#include <dali-ui-foundation/public-api/text/style/font-attributes.h>
#include <dali-ui-foundation/public-api/text/style/image-attributes.h>
#include <dali-ui-foundation/public-api/text/style/line-through.h>
#include <dali-ui-foundation/public-api/text/style/outline.h>
#include <dali-ui-foundation/public-api/text/style/reveal.h>
#include <dali-ui-foundation/public-api/text/style/shadow.h>
#include <dali-ui-foundation/public-api/text/style/underline.h>
#include <dali-ui-foundation/public-api/text/styled-text/anchor-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/annotation-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/background-color-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/font-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/foreground-color-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/image-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/line-through-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/replacement-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/span.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text-builder.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text.h>
#include <dali-ui-foundation/public-api/text/styled-text/underline-span.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>
#include <dali-ui-foundation/public-api/text/text-utils.h>

#include <dali-ui-foundation/public-api/visuals/border-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/color-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/gradient-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/image-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/text-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

#include <dali-ui-foundation/public-api/visuals/animated-image-visual.h>
#include <dali-ui-foundation/public-api/visuals/border-visual.h>
#include <dali-ui-foundation/public-api/visuals/color-visual.h>
#include <dali-ui-foundation/public-api/visuals/gradient-visual.h>
#include <dali-ui-foundation/public-api/visuals/image-visual.h>
#include <dali-ui-foundation/public-api/visuals/lottie-animation-visual.h>
#include <dali-ui-foundation/public-api/visuals/text-visual.h>
#include <dali-ui-foundation/public-api/visuals/visual-base.h>

#include <dali-ui-foundation/public-api/video/video-source.h>
#include <dali-ui-foundation/public-api/video/video-view.h>

#include <dali-ui-foundation/public-api/layouts/absolute-layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/absolute-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/absolute-layout.h>
#include <dali-ui-foundation/public-api/layouts/flex-layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/flex-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/flex-layout.h>
#include <dali-ui-foundation/public-api/layouts/grid-layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/grid-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/grid-layout.h>
#include <dali-ui-foundation/public-api/layouts/layout-bounds-effects.h>
#include <dali-ui-foundation/public-api/layouts/layout-controller.h>
#include <dali-ui-foundation/public-api/layouts/layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/layout-transition-types.h>
#include <dali-ui-foundation/public-api/layouts/layout-transition.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/layouts/layout.h>
#include <dali-ui-foundation/public-api/layouts/scroll-view-layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>

#include <dali-ui-foundation/public-api/types/align-enumerations.h>
#include <dali-ui-foundation/public-api/types/ui-property-index-ranges.h>

#include <dali-ui-foundation/public-api/views/scroll/bounce-edge-effect.h>
#include <dali-ui-foundation/public-api/views/scroll/edge-effect.h>
#include <dali-ui-foundation/public-api/views/scroll/page-scroll-view.h>
#include <dali-ui-foundation/public-api/views/scroll/page-scrollable-interface.h>
#include <dali-ui-foundation/public-api/views/scroll/scroll-bar-interface.h>
#include <dali-ui-foundation/public-api/views/scroll/scroll-bar.h>
#include <dali-ui-foundation/public-api/views/scroll/scroll-view.h>
#include <dali-ui-foundation/public-api/views/scroll/scrollable-enum.h>
