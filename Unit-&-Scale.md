[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Unit-&-Scale-(kr))

# Unit & Scale

dali-ui provides a device-independent unit system to render consistent UI across displays
with varying pixel densities (DPI).
All units are converted to physical pixel values at runtime based on the settings defined
in [Configuration](https://github.sec.samsung.net/NUI/dali-ui/wiki/Configuration#configuration-options).

<br/>

## Unit Types

| Unit | Literal | Conversion | Description |
|---|---|---|---|
| `px` | none (plain `float`) | — | Physical pixels. Maps directly to device resolution |
| `spx` | `100_spx` | `value × ScalingFactor` | Reflects ScalingFactor only. Useful for values that scale with user preferences |
| `dp` | `100_dp` | `value × (DPI / BaselineDPI)` | Density-independent pixels. Reflects screen density |
| `sdp` | `100_sdp` | `value × (DPI / BaselineDPI) × ScalingFactor` | Reflects both screen density and ScalingFactor |

<br/>

## Setting the Scaling Factor

ScalingFactor is configured via [Configuration](https://github.sec.samsung.net/NUI/dali-ui/wiki/Configuration#configuration-options).

```cpp
UiConfig::New()
  .SetScalingFactor(1.5f)
  .SetDpi(320)
  .SetBaselineDpi(160)
  .Apply();
```

With the above configuration, each unit converts as follows:

| Expression | Calculation | Result |
|---|---|---|
| `100_spx` | `100 × 1.5` | `150.0f px` |
| `100_dp` | `100 × (320 / 160)` | `200.0f px` |
| `100_sdp` | `100 × (320 / 160) × 1.5` | `300.0f px` |

<br/>

## Usage Example

```cpp
view.SetRequestedWidth(200_dp)
    .SetRequestedHeight(100_spx)
    .SetPositionX(50_sdp);
```

<br/>

## Cautions

> [!WARNING]
> **Do not use before `Apply()`**
> Unit literals require `UiConfig::Apply()` to have been called first.
> Using them before `Apply()` will trigger an assertion failure.

> [!WARNING]
> **Do not use to initialize static or global variables**
> Never use unit literals as initializers for static or global variables.
> C++ does not guarantee static initialization order across translation units,
> so the unit conversion may execute before `UiConfig::Apply()` has been called.
>
> ```cpp
> // ❌ Wrong
> static float kWidth = 100_dp;
>
> // ✅ Correct: compute at the point of use
> float GetWidth() { return 100_dp; }
> ```

<br/>

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#documents)
