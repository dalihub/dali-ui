[→ English](https://github.sec.samsung.net/NUI/dali-ui/wiki/Render-Effect)

# Render Effect

`RenderEffect`는 View에 블러나 마스킹 같은 시각 효과를 적용하기 위한 인터페이스입니다. 사용할 효과를 생성한 뒤 `View::SetRenderEffect()`로 View에 지정합니다.

dali-ui는 다음 효과를 제공합니다.

| 효과 | 적용 대상 |
| --- | --- |
| `GaussianBlurEffect` | View 자신과 자식을 흐리게 표시합니다. |
| `BackgroundBlurEffect` | View의 배경을 흐리게 표시합니다. |
| `MaskEffect` | 다른 View를 마스크 소스로 사용해 View를 마스킹합니다. |

구체적인 효과 객체는 각 클래스의 `New()`로 생성합니다. `RenderEffect`를 기본 생성하는 것만으로는 사용할 수 있는 효과가 만들어지지 않습니다.

<br/>

## 기본 사용법

### 효과 생성과 적용

다음 예제는 화면에 표시할 콘텐츠가 준비된 `view`에 Gaussian Blur를 적용합니다.

```cpp
#include <dali-ui-foundation/public-api/render-effects/gaussian-blur-effect.h>
#include <dali-ui-foundation/public-api/views/view.h>

using namespace Dali::Ui;

// view는 콘텐츠가 준비된 기존 View입니다.
GaussianBlurEffect effect = GaussianBlurEffect::New();
view.SetRenderEffect(effect);
```

`SetRenderEffect()`는 효과의 활성화도 처리하므로 별도로 `Activate()`를 호출할 필요가 없습니다. 다만 View가 scene에 없거나, 보이지 않거나, 크기가 0이면 활성화가 보류됩니다. View가 표시 가능한 상태가 되면 자동으로 활성화됩니다.

<br/>

### 효과 제거

View에서 효과를 제거하려면 `ClearRenderEffect()`를 호출합니다. 제거 과정에서 효과도 비활성화됩니다.

```cpp
view.ClearRenderEffect();
```

효과를 View에 지정한 채 잠시 끄려면 `Deactivate()`를 사용하고, 다시 켜려면 `Activate()`를 사용합니다.

```cpp
effect.Deactivate();

// 필요할 때 같은 View에서 다시 활성화합니다.
effect.Activate();
```

`Activate()`를 사용하려면 효과가 View에 지정되어 있어야 합니다. `ClearRenderEffect()`로 제거한 뒤 다시 사용하려면 `SetRenderEffect()`로 지정합니다.

<br/>

### 효과의 소유 View

각 효과 인스턴스는 한 번에 하나의 View에만 지정할 수 있습니다. 같은 효과를 다른 View에 지정하면 효과가 새 View로 이동하고, 이전 View에는 효과가 남지 않습니다.

```cpp
GaussianBlurEffect effect = GaussianBlurEffect::New();

firstView.SetRenderEffect(effect);
secondView.SetRenderEffect(effect); // firstView에서 secondView로 이동합니다.
```

여러 View에 동시에 효과를 적용하려면 View마다 별도의 효과 인스턴스를 생성합니다. 핸들을 복사해도 같은 실제 객체를 가리키므로 별도 인스턴스가 만들어지지 않습니다.

<br/>

## Gaussian Blur

`GaussianBlurEffect`는 효과를 지정한 View와 그 자식을 함께 흐리게 표시합니다. 이미지뿐 아니라 여러 자식을 포함한 View에도 적용할 수 있습니다.

```cpp
// gaussian-blur-effect.h를 포함합니다.
// contentView는 화면에 표시할 콘텐츠가 준비된 View입니다.
GaussianBlurEffect blur = GaussianBlurEffect::New(40u);
blur.SetBlurOnce(false);
contentView.SetRenderEffect(blur);
```

`New(40u)`는 blur radius를 40으로 지정합니다. 인수 없이 `New()`를 호출해도 기본 radius는 40입니다. `SetBlurOnce(false)`는 콘텐츠가 변할 때 블러 결과도 매 프레임 갱신하도록 합니다.

<!-- 이미지 추가: assets/RenderEffect/gaussian-blur-comparison.png
같은 콘텐츠를 효과 적용 전/후로 나란히 촬영합니다. 자식 텍스트나 도형도 포함해
View와 자식이 함께 흐려지는 모습을 보여주세요. radius 40, 매 프레임 갱신 조건입니다.
-->
<img src="assets/RenderEffect/gaussian-blur-comparison.png" alt="동일한 View의 Gaussian Blur 적용 전후 비교" width="720" />

*그림 1. Gaussian Blur 적용 전(왼쪽)과 적용 후(오른쪽). View의 콘텐츠와 자식이 함께 흐려집니다.*

<br/>

## Background Blur

`BackgroundBlurEffect`는 효과를 지정한 View의 배경을 흐리게 표시합니다. Gaussian Blur가 View 자신과 자식을 대상으로 한다면, Background Blur는 View 뒤의 콘텐츠를 대상으로 합니다.

```cpp
#include <dali-ui-foundation/public-api/render-effects/background-blur-effect.h>

// blurPane은 배경 콘텐츠 앞에 배치한 View입니다.
BackgroundBlurEffect blur = BackgroundBlurEffect::New(40u);
blur.SetBlurOnce(false);
blurPane.SetRenderEffect(blur);
```

배경에 어떤 콘텐츠가 포함되는지는 View 트리 구조의 영향을 받습니다. 캡처 범위를 명시하려면 `SetSourceView()`와 `SetStopperView()`를 사용합니다.

```cpp
// backgroundRoot는 blurPane의 조상 View입니다.
// stopperView는 캡처를 끝낼 위치를 나타내는 기존 View입니다.
blur.SetSourceView(backgroundRoot);
blur.SetStopperView(stopperView);
```

| 설정 | 동작 |
| --- | --- |
| `SetSourceView(view)` | 지정한 조상의 하위 트리로 배경 캡처 범위를 제한합니다. 소유 View의 조상이 아니면 무시됩니다. |
| `SetStopperView(view)` | 순회 중 지정한 View에 도달하면 캡처를 멈춥니다. 해당 View와 그 이후에 그려지는 콘텐츠는 제외됩니다. |
| 빈 `View()` 전달 | 해당 제한을 해제하고 기본 캡처 범위로 돌아갑니다. |

Source를 지정하지 않으면 기본적으로 소유 View 위의 전체 조상 체인을 배경 캡처 범위로 사용합니다. 조상에 자체 forward offscreen effect가 있으면 탐색이 그 지점에서 끝나므로 source 설정이 적용되지 않을 수 있습니다. Source와 stopper는 약한 참조로 저장되므로, 사용하는 동안 해당 View가 유지되도록 해야 합니다.

`samples/blur-effects-grid/blur-effects-grid-example.cpp`는 공통 root 아래에 배경 이미지들을 추가하고, stopper 다음에 blur pane들을 추가하는 구성을 보여줍니다.

<!-- 이미지 추가: assets/RenderEffect/background-blur-comparison.png
동일한 배경, 패널, 패널 안의 텍스트를 사용해 효과 없음/Gaussian/Background 세 장면을
나란히 촬영합니다. 텍스트를 각 효과의 소유 View 아래에 두어 적용 대상 차이를 보여주세요.
-->
<img src="assets/RenderEffect/background-blur-comparison.png" alt="효과 없음, Gaussian Blur, Background Blur의 적용 대상 비교" width="720" />

*그림 2. 효과 없음(왼쪽), Gaussian Blur(가운데), Background Blur(오른쪽). Gaussian Blur는 패널 콘텐츠를, Background Blur는 패널 뒤의 배경을 흐리게 표시합니다.*

<br/>

## Mask Effect

`MaskEffect`는 다른 View를 마스크 소스로 사용합니다. 아래 예제의 `targetView`는 마스킹할 콘텐츠이고, `maskView`는 마스크로 사용할 콘텐츠가 준비된 View입니다.

```cpp
#include <dali-ui-foundation/public-api/render-effects/mask-effect.h>

MaskEffect mask = MaskEffect::New(maskView);
targetView.SetRenderEffect(mask);
```

### 마스크 모드와 위치

| 모드 | 마스크 값 |
| --- | --- |
| `MaskEffect::MaskMode::ALPHA` | 마스크 텍스처의 알파 채널을 사용합니다. 기본 모드입니다. |
| `MaskEffect::MaskMode::LUMINANCE` | RGB를 회색조로 변환한 밝기를 사용합니다. |

모드와 위치, 크기를 지정하려면 다음 생성 함수를 사용합니다.

```cpp
MaskEffect mask = MaskEffect::New(
  maskView,
  MaskEffect::MaskMode::LUMINANCE,
  Dali::Vector2(20.0f, 0.0f),
  Dali::Vector2(0.8f, 0.8f));
targetView.SetRenderEffect(mask);
```

위치는 소유 View에 대한 픽셀 단위 offset입니다. Scale `(1, 1)`은 마스크 소스 자체 크기를 유지합니다. 간단한 `New(maskView)`의 기본 위치는 `(0, 0)`, 기본 scale은 `(1, 1)`입니다. Scale의 성분을 0으로 지정해도 효과가 꺼지지는 않으며, 내부적으로 작은 0이 아닌 값으로 보정됩니다.

<!-- 이미지 추가: assets/RenderEffect/mask-comparison.png
위 행은 ALPHA용 투명 배경 도형, 아래 행은 LUMINANCE용 흑백 그라데이션을 사용합니다.
각 행에 원본/마스크 소스/결과를 배치하고 투명 영역은 체크무늬로 구분해주세요.
두 행 모두 offset (0, 0), scale (1, 1)로 촬영합니다.
-->
<img src="assets/RenderEffect/mask-comparison.png" alt="ALPHA와 LUMINANCE 모드의 원본, 마스크 소스, 결과 비교" width="720" />

*그림 3. 원본(왼쪽), 마스크 소스(가운데), 마스킹 결과(오른쪽). 위 행은 ALPHA 모드, 아래 행은 LUMINANCE 모드입니다.*

<br/>

### 소스와 대상의 갱신

마스크의 대상 View와 소스 View를 각각 한 번만 캡처할지, 매 프레임 캡처할지 지정할 수 있습니다.

```cpp
mask.SetTargetRenderOnce(false); // 대상 콘텐츠를 매 프레임 캡처합니다.
mask.SetSourceRenderOnce(true);  // 마스크 소스는 한 번만 캡처합니다.
```

위 조합은 대상 콘텐츠가 변하고 마스크 소스는 고정된 경우에 사용할 수 있습니다. 마스크 소스도 변한다면 `SetSourceRenderOnce(false)`를 사용합니다. 두 옵션은 입력의 캡처 주기만 제어하며, 마스킹 자체는 매 프레임 적용됩니다.

<br/>

## 블러 옵션과 애니메이션

이 절의 API는 `GaussianBlurEffect`와 `BackgroundBlurEffect` 모두에서 사용할 수 있습니다.

### Radius와 downscale

```cpp
blur.SetBlurRadius(40u);
blur.SetBlurDownscaleFactor(0.5f);
```

`SetBlurDownscaleFactor()`는 입력 텍스처의 축소 비율을 지정하며, 값은 `[0.25, 1.0]` 범위로 보정됩니다. `1.0`은 원래 해상도를 사용하고, 작은 값은 축소한 입력을 사용합니다.

Downscale과 커널 최적화 때문에 실제 적용되는 radius는 `2 / downscaleFactor` 픽셀 단위로 변합니다. 예를 들어 factor가 `0.25`이면 8픽셀 간격이므로, radius를 조금씩 바꾸어도 화면 차이가 없을 수 있습니다.

<br/>

### 한 번 렌더링과 매 프레임 갱신

| 설정 | 사용 상황 |
| --- | --- |
| `SetBlurOnce(true)` | 고정된 콘텐츠의 블러를 한 번 렌더링합니다. 캡처 전에 콘텐츠가 준비되어 있어야 합니다. |
| `SetBlurOnce(false)` | 움직이거나 바뀌는 콘텐츠의 블러를 매 프레임 갱신합니다. |

`FinishedSignal()`은 blur-once가 켜져 있고 효과가 활성화된 상태에서 블러 렌더링이 완료되면 발생합니다. 콜백 인수는 없으며, signal 발생 직후 offscreen 리소스가 해제됩니다. 매 프레임 갱신 모드에서는 완료 signal이 발생하지 않습니다.

한 번 렌더링하는 이미지 예제는 `samples/blur-effects-grid/blur-effects-grid-example.cpp`처럼 `ResourceReadySignal()`에서 효과를 적용하는 구성을 참고할 수 있습니다.

<br/>

### Dithering

`SetDitherNoiseStrength()`는 최종 블러 출력에 추가되는 dithering noise의 강도를 지정합니다. 범위는 `[0, 1]`, 기본값은 `0.1`이며, `0`은 noise를 끕니다.

```cpp
blur.SetDitherNoiseStrength(0.1f);
```

<!-- 이미지 추가: assets/RenderEffect/blur-dithering-comparison.png
samples/background-blur-dither를 이용해 같은 그라데이션 영역을 noise 0.0/0.1로 촬영합니다.
Radius, downscale, 배율을 동일하게 유지하고 차이가 보이는 영역을 확대해 배치해주세요.
-->
<img src="assets/RenderEffect/blur-dithering-comparison.png" alt="동일한 블러 출력에서 dithering noise 강도 0과 0.1 비교" width="720" />

*그림 4. Dithering noise 강도 0(왼쪽)과 기본값 0.1(오른쪽)의 동일 영역 확대 비교.*

<br/>

### 블러 강도 애니메이션

`AddBlurStrengthAnimation()`은 지정한 `Animation`에 블러 강도 애니메이션을 추가합니다. 시작값과 끝값은 `[0, 1]` 범위이며, 아래 예제는 선명한 상태에서 블러 상태로 전환합니다. `blur`는 View에 적용된 효과입니다.

```cpp
Dali::Animation animation = Dali::Animation::New(1.0f);
blur.AddBlurStrengthAnimation(
  animation,
  Dali::AlphaFunction(Dali::AlphaFunction::BuiltinFunction::EASE_IN_OUT_SINE),
  Dali::TimePeriod(0.0f, 1.0f),
  0.0f,
  1.0f);
animation.Play();
```

시작값을 `1.0`, 끝값을 `0.0`으로 지정하면 반대 방향으로 전환합니다. `TimePeriod`도 명시적으로 전달합니다. Gaussian 곡선 자체가 비선형이므로 alpha function을 선택한 뒤 실제 전환 모습을 확인하세요.

강도 애니메이션 중에는 낮은 강도에서 축소 텍스처의 확대가 드러나지 않도록 원래 해상도의 버퍼를 사용합니다. 강도 `1`로 종료되면 설정한 downscale factor로 돌아가고, 강도 `0`으로 종료되면 다음 강도 애니메이션을 추가할 때까지 블러 렌더링을 우회합니다.

`AddBlurOpacityAnimation()`도 같은 형태의 인수를 받으며 블러 opacity 애니메이션을 추가합니다. 강도 변화 예제는 `AddBlurStrengthAnimation()`을 사용합니다.

<!-- 이미지 추가: assets/RenderEffect/blur-strength-animation.gif
samples/blur-strength-animation에서 Gaussian/Background 두 패널과 강도 표시를 함께 녹화합니다.
선명함 → 블러 → 선명함의 한 주기를 포함해주세요. 정지 화면으로 대체한다면
strength 0/0.5/1을 나란히 담은 PNG를 추가하고 아래 경로와 캡션을 바꾸면 됩니다.
-->
<img src="assets/RenderEffect/blur-strength-animation.gif" alt="Gaussian Blur와 Background Blur의 강도 0에서 1로, 다시 0으로 전환" width="720" />

*그림 5. Gaussian Blur와 Background Blur의 강도 애니메이션. 강도 0에서 선명한 화면을 보여주고, 1로 갈수록 블러가 적용됩니다.*

<br/>

## 생명주기와 주의사항

| API | 동작 |
| --- | --- |
| `view.SetRenderEffect(effect)` | View에 효과를 지정하고 활성화를 처리합니다. View는 한 번에 하나의 render effect를 가집니다. |
| `effect.Deactivate()` | 효과를 비활성화하되 View에 지정된 상태는 유지합니다. |
| `effect.Activate()` | 소유 View가 있고 활성화 조건을 만족할 때 효과를 켭니다. |
| `effect.IsActivated()` | 효과가 현재 활성화되어 있는지 확인합니다. |
| `effect.Refresh()` | 소유 View 크기를 다시 읽고, 조건에 따라 활성화·비활성화하거나 현재 offscreen 렌더링을 갱신합니다. |
| `view.ClearRenderEffect()` | View에서 효과를 제거하고 비활성화합니다. |

`Activate()`와 `Refresh()`는 소유 View가 없으면 아무 동작도 하지 않습니다. View가 scene에 없거나, 보이지 않거나, 크기가 0이면 활성화되지 않습니다.

View가 재배치되면 framework가 `Refresh()`를 호출하므로, 일반적인 크기 변경에 대해 직접 호출할 필요는 거의 없습니다. 효과 인스턴스를 여러 View에 동시에 쓰려면 각각 새로 생성해야 한다는 점도 함께 확인하세요.

<br/>

## 관련 샘플

| 경로 (`samples/` 아래) | 참고할 내용 |
| --- | --- |
| `blur-effects-grid/blur-effects-grid-example.cpp` | 두 블러 효과의 once/always 갱신 비교, 배경 source와 stopper 구성 |
| `blur-strength-animation/blur-strength-animation-example.cpp` | 두 블러 효과의 강도 애니메이션 |
| `background-blur-dither/background-blur-dither-example.cpp` | Background Blur의 radius와 dithering noise 조절 |

예제 코드 조각은 레이아웃과 콘텐츠가 준비된 View를 전제로 합니다. 캡처할 이미지가 비동기로 로드된다면, 리소스가 준비된 시점에 효과를 적용하도록 구성하세요.

<br/>

## API Reference

[RenderEffect API Reference](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1RenderEffect.html)  
[BackgroundBlurEffect API Reference](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1BackgroundBlurEffect.html)  
[GaussianBlurEffect API Reference](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1GaussianBlurEffect.html)  
[MaskEffect API Reference](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1MaskEffect.html)  

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki/Home-(kr)#development-guides)
