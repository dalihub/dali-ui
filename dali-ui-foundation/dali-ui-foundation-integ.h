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

#include <dali-ui-foundation/dali-ui-foundation-extension.h>
#include <dali-ui-foundation/integration-api/reserved-trait-id.h>
#include <dali-ui-foundation/integration-api/view-integ.h>

// Integration APIs
#include <dali-ui-foundation/integration-api/asset-manager/asset-manager.h>
#include <dali-ui-foundation/integration-api/builder/builder.h>
#include <dali-ui-foundation/integration-api/builder/json-parser.h>
#include <dali-ui-foundation/integration-api/builder/tree-node.h>
#include <dali-ui-foundation/integration-api/direction-enums.h>
#include <dali-ui-foundation/integration-api/drag-drop-detector/drag-and-drop-detector.h>
#include <dali-ui-foundation/integration-api/focus-indication-policy.h>
#include <dali-ui-foundation/integration-api/image-loader/async-image-loader-integ.h>
#include <dali-ui-foundation/integration-api/image-loader/texture-manager.h>
#include <dali-ui-foundation/integration-api/property-bridge/property-bridge.h>
#include <dali-ui-foundation/integration-api/shader-effects/alpha-discard-effect.h>
#include <dali-ui-foundation/integration-api/shader-effects/dissolve-effect.h>
#include <dali-ui-foundation/integration-api/shader-effects/distance-field-effect.h>
#include <dali-ui-foundation/integration-api/shader-effects/image-region-effect.h>
#include <dali-ui-foundation/integration-api/shader-effects/motion-blur-effect.h>
#include <dali-ui-foundation/integration-api/shader-effects/motion-stretch-effect.h>
#include <dali-ui-foundation/integration-api/ui-action-index-ranges.h>
#include <dali-ui-foundation/integration-api/ui-config-integ.h>
#include <dali-ui-foundation/integration-api/ui-foundation-pre-initialize.h>
#include <dali-ui-foundation/integration-api/ui-property-index-ranges.h>
#include <dali-ui-foundation/integration-api/utility/npatch-helper.h>
#include <dali-ui-foundation/integration-api/utility/npatch-utilities.h>
#include <dali-ui-foundation/integration-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/integration-api/visual-factory/precompile-shader-option.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/integration-api/visuals/animated-image-visual-actions-integ.h>
#include <dali-ui-foundation/integration-api/visuals/animated-image-visual-signals-integ.h>
#include <dali-ui-foundation/integration-api/visuals/animated-vector-image-visual-actions-integ.h>
#include <dali-ui-foundation/integration-api/visuals/animated-vector-image-visual-signals-integ.h>
#include <dali-ui-foundation/integration-api/visuals/arc-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/image-visual-actions-integ.h>
#include <dali-ui-foundation/integration-api/visuals/mesh-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/primitive-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-actions-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-base-impl.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-transform.h>
#include <dali-ui-foundation/integration-api/visuals/visuals-container.h>

// Impl
#include <dali-ui-foundation/extension-api/ui-config-impl.h>
#include <dali-ui-foundation/integration-api/state-effect-impl.h>

// Layouts
#include <dali-ui-foundation/integration-api/layouts/absolute-layout-impl.h>
#include <dali-ui-foundation/integration-api/layouts/flex-layout-impl.h>
#include <dali-ui-foundation/integration-api/layouts/grid-layout-impl.h>
#include <dali-ui-foundation/integration-api/layouts/layout-impl.h>
#include <dali-ui-foundation/integration-api/layouts/stack-layout-impl.h>
