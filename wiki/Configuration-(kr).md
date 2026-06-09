# Configuration

dali-ui를 사용하려면 앱 시작 시 반드시 Config 객체를 생성하고 `Apply()`를 호출해야 합니다.
`Apply()` 호출 전까지 dali-ui의 단위 리터럴(`_spx`, `_dp`, `_sdp` 등)을 포함한 주요 기능들이
올바르게 동작하지 않습니다. `Apply()` 이후에는 설정이 고정되며, 런타임 중 변경은 불가능합니다.

<br/>

## 어떤 Config를 사용해야 하나요?

사용하는 라이브러리에 따라 Config 클래스가 달라집니다.

| 사용 라이브러리 | Config 클래스 | 헤더 |
|---|---|---|
| `dali-ui-foundation` | `UiConfig` | `<dali-ui-foundation/public-api/ui-config.h>` |
| `dali-ui-components` | `UiComponentConfig` | `<dali-ui-components/public-api/ui-component-config.h>` |

`UiComponentConfig`는 `UiConfig`를 상속하므로 동일한 설정 항목을 모두 사용할 수 있습니다.
`dali-ui-components`를 사용하는 경우 반드시 `UiComponentConfig`를 사용하세요.

<br/>

## 기본 사용법

<ins>예제</ins>

```cpp
int main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);

  // Apply()는 application.MainLoop() 호출 전에 호출합니다.
  UiConfig config = UiConfig::New();
  config.SetDpi(320);
  config.SetScalingFactor(1.5f);
  config.Apply();

  MyAppController controller(application);
  application.MainLoop();
  return 0;
}
```

`dali-ui-components`를 사용하는 경우:

```cpp
UiComponentConfig config = UiComponentConfig::New();
config.SetDpi(320);
config.SetScalingFactor(1.5f);
config.Apply();
```

전체 예제는 [hello-world sample](https://github.com/dalihub/dali-ui/tree/main/samples/hello-world)을 참고하세요.

<br/>

## 설정 항목

아래는 몇가지 주요 항목에 대한 설명입니다. 전체 API는 다음 링크에서 확인할 수 있습니다: [UiConfig](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1UiConfig.html), [UiComponentConfig](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1UiComponentConfig.html)

| 항목 | 메서드 | 기본값 | 설명 |
|---|---|---|---|
| Scaling Factor | `SetScalingFactor(float)` | `1.0f` | `spx`, `sdp` 단위에 곱해지는 스케일 배율 |
| DPI | `SetDpi(int)` | `160` | 디스플레이의 실제 DPI |
| Baseline DPI | `SetBaselineDpi(int)` | `160` | `dp` 계산의 기준 DPI |
| Default Font Size | `SetDefaultFontSize(float)` | `16.0f` | 텍스트 요소의 기본 폰트 크기 (pt) |
| Default Text Color | `SetDefaultTextColor(Vector4)` | `Color::BLACK` | 텍스트 요소의 기본 색상 |
| Key Click Policy | `SetKeyClickPolicy(KeyClickPolicy)` | — | Clicked 이벤트 발생이 key press 시점인지 release 시점인지 설정 |
| Execution Key Predicate | `SetExecutionKeyPredicate(fn)` | `"Return"` | 클릭 실행으로 인식할 키를 결정하는 함수 |
| Min Long Press Key Count | `SetKeyLongPressThreshold(uint32_t)` | — | 롱프레스로 인식할 최소 키 반복 횟수 |
| Tap Recognizer Time | `SetTapRecognizerTime(uint32_t)` | — | 탭으로 인식할 시간 제한 (ms) |
| Broken Image URL | `SetBrokenImageUrl(type, url)` | — | 이미지 로딩 실패 시 표시할 이미지 (SMALL/NORMAL/LARGE) |
| Focus Clear on Escape | `EnableFocusClearOnEscape(bool)` | — | Escape 키 입력 시 포커스 해제 여부 |
| Always Show Focus | `SetAlwaysShowFocus(bool)` | — | 포커스 인디케이터 항상 표시 여부 |

<br/>

## Framework 개발자를 위한 Config 확장

특정 디바이스나 플랫폼 전용 Framework를 개발하는 경우, `UiConfig` 또는 `UiComponentConfig`를
상속하여 플랫폼에 맞는 설정이 미리 적용된 Config 클래스를 정의하고 앱 개발자에게 배포할 수 있습니다.

```cpp
// TVConfig.h (Framework 개발자가 정의 및 배포)
class TVConfig : public Dali::Ui::UiComponentConfig
{
public:
  explicit TVConfig(const Dali::Ui::UiComponentConfig& config)
  : Dali::Ui::UiComponentConfig(config)
  {
  }

  static TVConfig New()
  {
    TVConfig config(Dali::Ui::UiComponentConfig::New());
    config.SetDpi(72);
    config.SetScalingFactor(2.0f);
    config.SetDefaultFontSize(28.0f);
    return config;
  }
};
```

앱 개발자는 별도의 설정 없이 아래 한 줄로 플랫폼 설정을 적용할 수 있습니다:

```cpp
TVConfig::New().Apply();
```

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki/Home-(kr)#development-guides)
