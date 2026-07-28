# Devel API to Integration API Migration History

이 문서는 `dali-ui-foundation/devel-api`에 있던 파일을 `dali-ui-foundation/integration-api`로 이동하면서 발생한 파일명, include 경로, namespace 변경 이력을 기록한다.

## 목적

`devel-api`에서 `integration-api`로 이동한 이력을 남겨 이후 코드 리뷰, 마이그레이션, 후속 정리 작업에서 어떤 API가 어떤 이름으로 이동되었는지 추적할 수 있게 한다.

이동된 API는 더 이상 `Dali::Ui` 바로 아래의 devel 성격 API가 아니라 `Dali::Ui::Integration` 아래의 integration API로 취급한다.

## 기본 변경 규칙

| 항목 | Before | After |
|---|---|---|
| API directory | `dali-ui-foundation/devel-api/...` | `dali-ui-foundation/integration-api/...` |
| Include prefix | `#include <dali-ui-foundation/devel-api/...>` | `#include <dali-ui-foundation/integration-api/...>` |
| Devel suffix | `*-devel.h`, `*-devel.cpp` | `*-integ.h`, `*-integ.cpp` |
| API namespace | `Dali::Ui` | `Dali::Ui::Integration` |
| Umbrella header | 개별 include 또는 devel 경로 include | `dali-ui-foundation/dali-ui-foundation-integ.h`에서 include |

외부 DALi dependency의 `<dali/devel-api/...>` include는 이 이동의 대상이 아니다. 해당 include는 `dali-core` 또는 `dali-adaptor` API를 참조하므로 그대로 유지한다.

일부 파일은 include 경로만 `integration-api`로 이동하고 C linkage 또는 internal namespace를 유지한다. 자세한 내용은 "Namespace 유지 예외" 섹션에 기록한다.

## 파일명 변경

아래 파일은 `devel-api`에서 `integration-api`로 이동하면서 파일명도 `-devel`에서 `-integ`로 변경되었다.

| Before | After |
|---|---|
| `devel-api/image-loader/async-image-loader-devel.h` | `integration-api/image-loader/async-image-loader-integ.h` |
| `devel-api/image-loader/async-image-loader-devel.cpp` | `integration-api/image-loader/async-image-loader-integ.cpp` |
| `devel-api/visuals/animated-image-visual-actions-devel.h` | `integration-api/visuals/animated-image-visual-actions-integ.h` |
| `devel-api/visuals/animated-image-visual-signals-devel.h` | `integration-api/visuals/animated-image-visual-signals-integ.h` |
| `devel-api/visuals/animated-vector-image-visual-actions-devel.h` | `integration-api/visuals/animated-vector-image-visual-actions-integ.h` |
| `devel-api/visuals/animated-vector-image-visual-signals-devel.h` | `integration-api/visuals/animated-vector-image-visual-signals-integ.h` |
| `devel-api/visuals/arc-visual-properties-devel.h` | `integration-api/visuals/arc-visual-properties-integ.h` |
| `devel-api/visuals/image-visual-actions-devel.h` | `integration-api/visuals/image-visual-actions-integ.h` |
| `devel-api/visuals/mesh-visual-properties-devel.h` | `integration-api/visuals/mesh-visual-properties-integ.h` |
| `devel-api/visuals/primitive-visual-properties-devel.h` | `integration-api/visuals/primitive-visual-properties-integ.h` |
| `devel-api/visuals/visual-actions-devel.h` | `integration-api/visuals/visual-actions-integ.h` |
| `devel-api/visuals/visual-properties-devel.h` | `integration-api/visuals/visual-properties-integ.h` |

## 위치만 변경된 파일

아래 파일은 파일명은 유지하고 directory만 `devel-api`에서 `integration-api`로 이동되었다.

| Before | After |
|---|---|
| `devel-api/asset-manager/asset-manager.h` | `integration-api/asset-manager/asset-manager.h` |
| `devel-api/asset-manager/asset-manager.cpp` | `integration-api/asset-manager/asset-manager.cpp` |
| `devel-api/builder/builder.h` | `integration-api/builder/builder.h` |
| `devel-api/builder/builder.cpp` | `integration-api/builder/builder.cpp` |
| `devel-api/builder/json-parser.h` | `integration-api/builder/json-parser.h` |
| `devel-api/builder/json-parser.cpp` | `integration-api/builder/json-parser.cpp` |
| `devel-api/builder/tree-node.h` | `integration-api/builder/tree-node.h` |
| `devel-api/builder/tree-node.cpp` | `integration-api/builder/tree-node.cpp` |
| `devel-api/direction-enums.h` | `integration-api/direction-enums.h` |
| `devel-api/drag-drop-detector/drag-and-drop-detector.h` | `integration-api/drag-drop-detector/drag-and-drop-detector.h` |
| `devel-api/drag-drop-detector/drag-and-drop-detector.cpp` | `integration-api/drag-drop-detector/drag-and-drop-detector.cpp` |
| `devel-api/image-loader/texture-manager.h` | `integration-api/image-loader/texture-manager.h` |
| `devel-api/image-loader/texture-manager.cpp` | `integration-api/image-loader/texture-manager.cpp` |
| `devel-api/property-bridge/property-bridge.h` | `integration-api/property-bridge/property-bridge.h` |
| `devel-api/property-bridge/property-bridge.cpp` | `integration-api/property-bridge/property-bridge.cpp` |
| `devel-api/shader-effects/alpha-discard-effect.h` | `integration-api/shader-effects/alpha-discard-effect.h` |
| `devel-api/shader-effects/alpha-discard-effect.cpp` | `integration-api/shader-effects/alpha-discard-effect.cpp` |
| `devel-api/shader-effects/dissolve-effect.h` | `integration-api/shader-effects/dissolve-effect.h` |
| `devel-api/shader-effects/dissolve-effect.cpp` | `integration-api/shader-effects/dissolve-effect.cpp` |
| `devel-api/shader-effects/distance-field-effect.h` | `integration-api/shader-effects/distance-field-effect.h` |
| `devel-api/shader-effects/distance-field-effect.cpp` | `integration-api/shader-effects/distance-field-effect.cpp` |
| `devel-api/shader-effects/image-region-effect.h` | `integration-api/shader-effects/image-region-effect.h` |
| `devel-api/shader-effects/image-region-effect.cpp` | `integration-api/shader-effects/image-region-effect.cpp` |
| `devel-api/shader-effects/motion-blur-effect.h` | `integration-api/shader-effects/motion-blur-effect.h` |
| `devel-api/shader-effects/motion-blur-effect.cpp` | `integration-api/shader-effects/motion-blur-effect.cpp` |
| `devel-api/shader-effects/motion-stretch-effect.h` | `integration-api/shader-effects/motion-stretch-effect.h` |
| `devel-api/shader-effects/motion-stretch-effect.cpp` | `integration-api/shader-effects/motion-stretch-effect.cpp` |
| `devel-api/ui-action-index-ranges.h` | `integration-api/ui-action-index-ranges.h` |
| `devel-api/ui-foundation-pre-initialize.h` | `integration-api/ui-foundation-pre-initialize.h` |
| `devel-api/ui-foundation-pre-initialize.cpp` | `integration-api/ui-foundation-pre-initialize.cpp` |
| `devel-api/ui-property-index-ranges.h` | `integration-api/ui-property-index-ranges.h` |
| `devel-api/utility/npatch-helper.h` | `integration-api/utility/npatch-helper.h` |
| `devel-api/utility/npatch-helper.cpp` | `integration-api/utility/npatch-helper.cpp` |
| `devel-api/utility/npatch-utilities.h` | `integration-api/utility/npatch-utilities.h` |
| `devel-api/utility/npatch-utilities.cpp` | `integration-api/utility/npatch-utilities.cpp` |
| `devel-api/view-depth-index-ranges.h` | `integration-api/view-depth-index-ranges.h` |
| `devel-api/visual-factory/precompile-shader-option.h` | `integration-api/visual-factory/precompile-shader-option.h` |
| `devel-api/visual-factory/precompile-shader-option.cpp` | `integration-api/visual-factory/precompile-shader-option.cpp` |
| `devel-api/visual-factory/visual-base.h` | `integration-api/visual-factory/visual-base.h` |
| `devel-api/visual-factory/visual-base.cpp` | `integration-api/visual-factory/visual-base.cpp` |
| `devel-api/visual-factory/visual-factory.h` | `integration-api/visual-factory/visual-factory.h` |
| `devel-api/visual-factory/visual-factory.cpp` | `integration-api/visual-factory/visual-factory.cpp` |
| `devel-api/visuals/visual-base-impl.h` | `integration-api/visuals/visual-base-impl.h` |
| `devel-api/visuals/visual-base-impl.cpp` | `integration-api/visuals/visual-base-impl.cpp` |
| `devel-api/visuals/visual-transform.h` | `integration-api/visuals/visual-transform.h` |
| `devel-api/visuals/visual-transform.cpp` | `integration-api/visuals/visual-transform.cpp` |
| `devel-api/visuals/visuals-container.h` | `integration-api/visuals/visuals-container.h` |
| `devel-api/visuals/visuals-container.cpp` | `integration-api/visuals/visuals-container.cpp` |

## 삭제된 파일

아래 focus-manager devel wrapper는 integration API로 유지하지 않고 삭제되었다. 기본 focus algorithm 제어는 internal implementation detail로 남긴다.

| Before | After |
|---|---|
| `devel-api/focus-manager/focus-manager-devel.h` | 삭제 |
| `devel-api/focus-manager/focus-manager-devel.cpp` | 삭제 |

## Class namespace 변경

아래 class는 `Dali::Ui`에서 `Dali::Ui::Integration`으로 이동되었다.

| Symbol | Before | After |
|---|---|---|
| `AssetManager` | `Dali::Ui::AssetManager` | `Dali::Ui::Integration::AssetManager` |
| `Builder` | `Dali::Ui::Builder` | `Dali::Ui::Integration::Builder` |
| `JsonParser` | `Dali::Ui::JsonParser` | `Dali::Ui::Integration::JsonParser` |
| `TreeNode` | `Dali::Ui::TreeNode` | `Dali::Ui::Integration::TreeNode` |
| `TreeNode::ConstIterator` | `Dali::Ui::TreeNode::ConstIterator` | `Dali::Ui::Integration::TreeNode::ConstIterator` |
| `DragAndDropDetector` | `Dali::Ui::DragAndDropDetector` | `Dali::Ui::Integration::DragAndDropDetector` |
| `PropertyBridge` | `Dali::Ui::PropertyBridge` | `Dali::Ui::Integration::PropertyBridge` |
| `PrecompileShaderOption` | `Dali::Ui::PrecompileShaderOption` | `Dali::Ui::Integration::PrecompileShaderOption` |
| `VisualFactory` | `Dali::Ui::VisualFactory` | `Dali::Ui::Integration::VisualFactory` |
| `Visual::Base` | `Dali::Ui::Visual::Base` | `Dali::Ui::Integration::Visual::Base` |
| `VisualsContainer` | `Dali::Ui::VisualsContainer` | `Dali::Ui::Integration::VisualsContainer` |

## Enum, namespace, function namespace 변경

아래 enum namespace 또는 enum class는 `Dali::Ui::Integration` 아래로 이동되었다.

| Symbol | Before | After |
|---|---|---|
| `Direction::Type` | `Dali::Ui::Direction::Type` | `Dali::Ui::Integration::Direction::Type` |
| `Visual::Property::Type` | `Dali::Ui::DevelVisual::Property::Type` | `Dali::Ui::Integration::Visual::Property::Type` |
| `Visual::Transform::Property::Type` | `Dali::Ui::DevelVisual::Transform::Property::Type` | `Dali::Ui::Integration::Visual::Transform::Property::Type` |
| `Visual::InternalContainerRangeType` | `Dali::Ui::DevelVisual::InternalContainerRangeType` | `Dali::Ui::Integration::Visual::InternalContainerRangeType` |
| `InternalVisualType` | `Dali::Ui::InternalVisualType` | `Dali::Ui::Integration::InternalVisualType` |
| `AnimatedImageVisual::Action::Type` | `Dali::Ui::DevelAnimatedImageVisual::Action::Type` | `Dali::Ui::Integration::AnimatedImageVisual::Action::Type` |
| `AnimatedImageVisual::Signal::Type` | `Dali::Ui::DevelAnimatedImageVisual::Signal::Type` | `Dali::Ui::Integration::AnimatedImageVisual::Signal::Type` |
| `AnimatedVectorImageVisual::Action::Type` | `Dali::Ui::DevelAnimatedVectorImageVisual::Action::Type` | `Dali::Ui::Integration::AnimatedVectorImageVisual::Action::Type` |
| `AnimatedVectorImageVisual::Signal::Type` | `Dali::Ui::DevelAnimatedVectorImageVisual::Signal::Type` | `Dali::Ui::Integration::AnimatedVectorImageVisual::Signal::Type` |
| `AnimatedVectorImageVisual::Action::DynamicPropertyInfo` | `Dali::Ui::DevelAnimatedVectorImageVisual::Action::DynamicPropertyInfo` | `Dali::Ui::Integration::AnimatedVectorImageVisual::Action::DynamicPropertyInfo` |
| `ImageVisual::Action::Type` | `Dali::Ui::DevelImageVisual::Action::Type` | `Dali::Ui::Integration::ImageVisual::Action::Type` |
| `ArcVisual::Property::Type` | `Dali::Ui::DevelArcVisual::Property::Type` | `Dali::Ui::Integration::ArcVisual::Property::Type` |
| `ArcVisual::Cap::Type` | `Dali::Ui::DevelArcVisual::Cap::Type` | `Dali::Ui::Integration::ArcVisual::Cap::Type` |
| `MeshVisual::Property::Type` | `Dali::Ui::MeshVisual::Property::Type` | `Dali::Ui::Integration::MeshVisual::Property::Type` |
| `MeshVisual::ShadingMode::Value` | `Dali::Ui::MeshVisual::ShadingMode::Value` | `Dali::Ui::Integration::MeshVisual::ShadingMode::Value` |
| `PrimitiveVisual::Property::Type` | `Dali::Ui::PrimitiveVisual::Property::Type` | `Dali::Ui::Integration::PrimitiveVisual::Property::Type` |
| `PrimitiveVisual::Shape::Type` | `Dali::Ui::PrimitiveVisual::Shape::Type` | `Dali::Ui::Integration::PrimitiveVisual::Shape::Type` |
| `Visual::Action::Type` | `Dali::Ui::Visual::Action::Type` | `Dali::Ui::Integration::Visual::Action::Type` |
| `PropertyRanges` | `Dali::Ui::DevelPropertyRanges` | `Dali::Ui::Integration::PropertyRanges` |
| `ActionRanges` | `Dali::Ui` | `Dali::Ui::Integration` |
| `DepthIndex::Ranges` | `Dali::Ui::DepthIndex::Ranges` | `Dali::Ui::Integration::DepthIndex::Ranges` |
| `PrecompileShaderOption::ShaderType` | `Dali::Ui::PrecompileShaderOption::ShaderType` | `Dali::Ui::Integration::PrecompileShaderOption::ShaderType` |
| `PrecompileShaderOption::Flag` | `Dali::Ui::PrecompileShaderOption::Flag` | `Dali::Ui::Integration::PrecompileShaderOption::Flag` |
| `VisualFactory::CreationOptions` | `Dali::Ui::VisualFactory::CreationOptions` | `Dali::Ui::Integration::VisualFactory::CreationOptions` |
| `VisualsContainer::ShadowType` | `Dali::Ui::VisualsContainer::ShadowType` | `Dali::Ui::Integration::VisualsContainer::ShadowType` |
| `PreMultiplyOnLoad` | `Dali::Ui::DevelAsyncImageLoader::PreMultiplyOnLoad` | `Dali::Ui::Integration::PreMultiplyOnLoad` |

Shader effect factory functions also moved into `Dali::Ui::Integration`.

| Function | Before | After |
|---|---|---|
| `CreateAlphaDiscardEffect()` | `Dali::Ui::CreateAlphaDiscardEffect()` | `Dali::Ui::Integration::CreateAlphaDiscardEffect()` |
| `CreateDissolveEffect()` | `Dali::Ui::CreateDissolveEffect()` | `Dali::Ui::Integration::CreateDissolveEffect()` |
| `CreateDistanceFieldEffect()` | `Dali::Ui::CreateDistanceFieldEffect()` | `Dali::Ui::Integration::CreateDistanceFieldEffect()` |
| `CreateImageRegionEffect()` | `Dali::Ui::CreateImageRegionEffect()` | `Dali::Ui::Integration::CreateImageRegionEffect()` |
| `CreateMotionBlurEffect()` | `Dali::Ui::CreateMotionBlurEffect()` | `Dali::Ui::Integration::CreateMotionBlurEffect()` |
| `CreateMotionStretchEffect()` | `Dali::Ui::CreateMotionStretchEffect()` | `Dali::Ui::Integration::CreateMotionStretchEffect()` |
| `NPatchHelper::CreateGridGeometry()` | `Dali::Ui::NPatchHelper::CreateGridGeometry()` | `Dali::Ui::Integration::NPatchHelper::CreateGridGeometry()` |
| `NPatchHelper::CreateBorderGeometry()` | `Dali::Ui::NPatchHelper::CreateBorderGeometry()` | `Dali::Ui::Integration::NPatchHelper::CreateBorderGeometry()` |
| `NPatchUtility::ParseBorders()` | `Dali::Ui::NPatchUtility::ParseBorders()` | `Dali::Ui::Integration::NPatchUtility::ParseBorders()` |

## Namespace 유지 예외

아래 파일은 `integration-api` 경로 아래로 이동했지만, 실제 타입의 소유권 또는 linkage 특성 때문에 namespace를 `Dali::Ui::Integration`으로 옮기지 않는다.

| File | Namespace | Reason |
|---|---|---|
| `integration-api/ui-foundation-pre-initialize.h` | global `extern "C"` | app launcher에서 C symbol로 호출하는 pre-initialize entry point |
| `integration-api/ui-foundation-pre-initialize.cpp` | global `extern "C"` | app launcher에서 C symbol로 호출하는 pre-initialize entry point |
| `integration-api/visuals/visual-base-impl.h` | `Dali::Ui::Internal` | visual internal implementation class |
| `integration-api/visuals/visual-base-impl.cpp` | `Dali::Ui::Internal` | visual internal implementation implementation |
| `integration-api/visuals/visual-transform.h` | `Dali::Ui::Internal::Visual` | internal visual transform data structure |
| `integration-api/visuals/visual-transform.cpp` | `Dali::Ui::Internal::Visual` | internal visual transform implementation |

## Umbrella header 변경

`dali-ui-foundation/dali-ui-foundation-integ.h`는 이동된 integration API header를 포함한다. 사용자는 필요한 경우 개별 header 대신 umbrella header를 사용할 수 있다.

대표 include 추가 항목:

```cpp
#include <dali-ui-foundation/integration-api/builder/builder.h>
#include <dali-ui-foundation/integration-api/image-loader/async-image-loader-integ.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visuals-container.h>
```

## Migration examples

### Include path

```cpp
// Before
#include <dali-ui-foundation/devel-api/visuals/visual-properties-devel.h>
#include <dali-ui-foundation/devel-api/visual-factory/visual-factory.h>

// After
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
```

### Type and enum names

```cpp
// Before
Dali::Ui::VisualFactory factory;
Dali::Ui::DevelVisual::Property::CORNER_RADIUS;
Dali::Ui::PrimitiveVisual::Property::MIX_COLOR;

// After
Dali::Ui::Integration::VisualFactory factory;
Dali::Ui::Integration::Visual::Property::CORNER_RADIUS;
Dali::Ui::Integration::PrimitiveVisual::Property::MIX_COLOR;
```

### Visual handle

```cpp
// Before
Dali::Ui::Visual::Base visual;

// After
Dali::Ui::Integration::Visual::Base visual;
```

## Build and packaging updates

`build/tizen/dali-ui-foundation/CMakeLists.txt`는 include directory와 install directory를 `integration-api` 기준으로 사용한다.

`packaging/dali-ui.spec`와 `packaging/dali-ui.spec.in`에서는 `dali-ui-foundation/devel-api/*` install entry를 제거하고 `dali-ui-foundation/integration-api/*` entry만 유지한다.

## Validation note

이 변경 이후 `dali-ui/build/tizen`에서 `dali2-ui-foundation`과 `dali2-ui-components` 빌드를 확인했다.

## DragAndDropDetector 후속 public API 이동

`DragAndDropDetector`는 framework 확장용 API가 아니라 애플리케이션이 직접 source,
target, drag preview를 구성하는 기능이다. 따라서 in-scene drag-and-drop 기능을 정리하면서
아래와 같이 public API로 후속 이동했다. 위 표는 최초 devel-to-integration migration
이력으로 유지한다.

| 항목 | Integration 단계 | 현재 |
|---|---|---|
| Detector header | `integration-api/drag-drop-detector/drag-and-drop-detector.h` | `public-api/drag-and-drop/drag-and-drop-detector.h` |
| Types header | detector header에 포함 | `public-api/drag-and-drop/drag-and-drop-types.h` |
| Source | `integration-api/drag-drop-detector/drag-and-drop-detector.cpp` | `public-api/drag-and-drop/drag-and-drop-detector.cpp` |
| Namespace | `Dali::Ui::Integration::DragAndDropDetector` | `Dali::Ui::DragAndDropDetector` |
| Umbrella header | `dali-ui-foundation-integ.h` | `dali-ui-foundation.h` |

## DragAndDropDetector 명시적 제어 세션

키보드, 접근성 action, gamepad처럼 포인터 좌표를 직접 제공하지 않는 입력도 동일한
drag lifecycle과 application-provided preview를 재사용할 수 있도록 다음 public API를
추가했다.

| API | 역할 |
|---|---|
| `StartDrag(source, deviceClass)` | 등록된 source의 screen bounds 중앙에서 명시적 세션 시작 |
| `MoveDragTo(target)` | 애플리케이션이 선택한 등록 target으로 이동하고 proposal/feedback 평가 |
| `Drop()` | 승인된 현재 target에 drop하거나 `NO_TARGET`으로 종료 |
| `GetDragSessionOrigin()` | 세션이 `GESTURE` 또는 `EXPLICIT`에서 시작됐는지 조회 |

`DragAndDropEvent::sessionOrigin`은 preview, proposal, feedback, cancel/finalize
callback에서도 같은 세션 출처를 전달한다. Detector는 focus 이동이나 접근성 탐색
순서를 소유하지 않으며, 애플리케이션이 해당 정책을 결정한 뒤 `MoveDragTo()`를
호출한다.

## DragAndDropDetector pre-release API 정리

첫 사용처가 생기기 전에 다른 platform의 drag-and-drop API와 용어 및 event 구조를
맞추기 위해 다음 breaking redesign을 적용했다.

| 항목 | 현재 계약 |
|---|---|
| Payload | `DragPayload`가 우선순위가 있는 복수 representation과 allowed/preferred operation 제공 |
| Dynamic data | `SourcePayloadProvider`가 gesture pickup 시점에 session payload를 한 번 생성 |
| Lifecycle | 모든 signal이 immutable `DragAndDropEvent` snapshot을 전달 |
| Coordinates | screen/source-grab/source-parent/target-local/preview-local 좌표를 명시적으로 구분 |
| Preview | DALi `Visual`과 혼동하지 않도록 `DragPreview` 용어 사용 |
| Target proposal | `DropProposalCallback`이 selected representation과 COPY/MOVE/LINK operation 반환 |
| Explicit control | explicit payload를 받는 `StartDrag()` overload 제공 |

Drop handler는 `DragAndDropEvent::GetSelectedRepresentationData()`를 사용해
proposal의 type을 다시 조회하지 않고 선택된 `Property::Value`를 받을 수 있다.

`EndedSignal`의 event에는 detector query state가 정리된 뒤에도 source, target,
payload, terminal result가 유지된다. 공통 cleanup은 Ended callback에서 수행할 수
있지만, 바깥 detach operation과의 재진입 충돌을 막기 위해 새 session은 callback
반환 뒤 시작해야 한다.

## ScrollView child drag 조정 API

`ScrollView` 안의 child가 drag gesture를 소유하는 동안 일반 pan scrolling을
중지하고 callback-driven edge auto-scroll만 유지할 수 있도록 public API를
추가했다.

| API | 역할 |
|---|---|
| `SetPanScrollEnabled(bool)` | touch/mouse pan의 content 이동과 intercept를 활성화 또는 비활성화 |
| `IsPanScrollEnabled()` | 현재 pan scrolling 활성화 상태 조회 |

pan scrolling을 비활성화해도 `ScrollTo()`를 포함한 programmatic scrolling은
동작한다. 진행 중인 pan을 비활성화하면 fling 없이 interaction을 종료하며 child
touch sequence를 더 이상 intercept하지 않는다.
