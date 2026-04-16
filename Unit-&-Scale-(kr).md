# Unit & Scale

dali-ui는 다양한 화면 밀도(DPI)를 가진 디바이스에서 일관된 UI를 구현하기 위해 픽셀 독립적인 단위 시스템을 제공합니다.
모든 단위는 [Configuration](https://github.sec.samsung.net/NUI/dali-ui/wiki/Configuration-(kr)#configuration-options)에서 설정한 값을 기반으로 런타임에 픽셀 값으로 변환됩니다.

<br/>

## 단위 종류

| 단위 | 리터럴 | 변환 공식 | 설명 |
|---|---|---|---|
| `px` | 없음 (plain `float`) | — | 물리 픽셀. 디바이스 해상도에 직접 대응 |
| `spx` | `100_spx` | `value × ScalingFactor` | ScalingFactor만 반영. 폰트 크기 등 사용자 설정에 따라 스케일 |
| `dp` | `100_dp` | `value × (DPI / BaselineDPI)` | 화면 밀도를 반영한 밀도 독립 픽셀 |
| `sdp` | `100_sdp` | `value × (DPI / BaselineDPI) × ScalingFactor` | 밀도와 ScalingFactor를 모두 반영 |

<br/>

## Scaling Factor 설정

ScalingFactor는 [Configuration](https://github.sec.samsung.net/NUI/dali-ui/wiki/Configuration-(kr)#configuration-options)에서 설정합니다.

```cpp
UiConfig::New()
  .SetScalingFactor(1.5f)
  .SetDpi(320)
  .SetBaselineDpi(160)
  .Apply();
```

위 설정에서 각 단위의 변환 예시:

| 표현식 | 계산 | 결과 |
|---|---|---|
| `100_spx` | `100 × 1.5` | `150.0f px` |
| `100_dp` | `100 × (320 / 160)` | `200.0f px` |
| `100_sdp` | `100 × (320 / 160) × 1.5` | `300.0f px` |

<br/>

## 사용 예시

```cpp
view.SetRequestedWidth(200_dp)
    .SetRequestedHeight(100_spx)
    .SetPositionX(50_sdp);
```

<br/>

## 주의사항

> [!WARNING]
> **`Apply()` 호출 전 사용 금지**
> 단위 리터럴은 `UiConfig::Apply()` 이후에만 사용할 수 있습니다.
> `Apply()` 전에 사용하면 assertion 실패가 발생합니다.

> [!WARNING]
> **전역/정적 변수 초기화에 사용 금지**
> 단위 리터럴을 전역 또는 정적 변수의 초기값으로 사용하지 마세요.
> C++의 정적 초기화 순서는 translation unit 간에 보장되지 않으므로,
> `UiConfig::Apply()`가 호출되기 전에 단위 변환이 실행될 수 있습니다.
>
> ```cpp
> // ❌ 잘못된 사용
> static float kWidth = 100_dp;
>
> // ✅ 올바른 사용: 필요한 시점에 계산
> float GetWidth() { return 100_dp; }
> ```

<br/>

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#documents)
