## UiColor

`UiColor`는 색상 값을 두 가지 방식으로 표현합니다.

**직접 RGBA 지정:**
```cpp
UiColor red(1.0f, 0.0f, 0.0f);          // RGB, alpha = 1.0
UiColor semi(1.0f, 0.0f, 0.0f, 0.5f);   // RGBA
UiColor hex(0xFF5733);                   // 0xRRGGBB, alpha = 1.0
UiColor hex2(0xFF5733, 0.8f);            // 0xRRGGBB + alpha
```

**Token 기반 (테마에서 런타임 해석):**
```cpp
UiColor primary("Primary");  // 테마 컬러 테이블에서 lookup
```

Token 기반 `UiColor`를 `SetBackgroundColor()` 등에 전달하면, 테마가 변경될 때 자동으로 색상이 갱신됩니다.

<br/>

## Predefined Color IDs

dali-ui는 플랫폼에 공통적인 몇 가지 color ID를 미리 정의합니다:

```cpp
view.SetBackgroundColor(UiColor::PRIMARY);     // "Primary"
view.SetBackgroundColor(UiColor::BACKGROUND);  // "Background"
view.SetBackgroundColor(UiColor::OUTLINE);     // "Outline"
```

이 외의 color ID는 플랫폼/디바이스별 확장 라이브러리에서 추가로 정의하는 것을 권장합니다.
자세한 내용은 [Framework 개발자: Color ID 확장](https://github.sec.samsung.net/NUI/dali-ui/wiki/Color-&-Theme-(kr)#color-id-%ED%99%95%EC%9E%A5-for-framework-developers) 섹션을 참고하세요.

<br/>

## Alpha 조정: ScaleAlpha / WithAlpha

Token 색상에 대해 alpha 값을 조정할 수 있습니다.
동일한 색상을 투명도에 따라 여러 token으로 나누어 정의하는 대신, 이 메서드를 활용하세요.

```cpp
// "Primary" 색상의 alpha를 50%로 낮춤
view.SetBackgroundColor(UiColor::PRIMARY.ScaleAlpha(0.5f));

// "Primary" 색상의 alpha를 0.3으로 고정
view.SetBackgroundColor(UiColor::PRIMARY.WithAlpha(0.3f));
```

> [!WARNING]
> **권장**: 색상 테이블이 커질수록 lookup 비용이 증가합니다.
> 같은 색상을 alpha 값만 달리하여 여러 token으로 등록하는 것은 지양하세요.
> `ScaleAlpha` / `WithAlpha`를 활용하면 token 수를 최소화할 수 있습니다.

<br/>

## 테마 변경 감지

dali-ui는 테마를 직접 변경하는 API를 제공하지 않습니다.
테마 변경은 `ThemeLoaderInterface` 구현이 시스템 이벤트를 받아 처리합니다.

앱에서 테마 변경 시점을 감지하려면 `UiThemeManager`의 signal을 사용하세요:

```cpp
UiThemeManager::Get().ThemeChangedSignal().Connect(this, &MyApp::OnThemeChanged);

void MyApp::OnThemeChanged()
{
  auto themeId = UiThemeManager::Get().GetCurrentThemeId();
  // 테마에 따른 추가 처리
}
```

Token 기반 `UiColor`로 색상을 설정한 View는 테마 변경 시 자동으로 색상이 갱신되므로
별도의 signal 처리 없이도 동작합니다.

<br/>

## 커스텀 테마 로더 구현 (for Framework Developers)

특정 디바이스 전용의 테마 모듈을 구현하려면 `ThemeLoaderInterface`를 구현하고,
`UiConfigImpl::CreateThemeLoader()`를 오버라이드하여 연결합니다.

아래는 설명을 위한 간단한 예시입니다. 동작하는 샘플은 여기를 참고하세요: [samples/color-controls](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/color-controls)

**1. ThemeLoaderInterface 구현 예시**

```cpp
// tv-theme-loader.h
class TvThemeLoader : public Dali::Ui::ThemeLoaderInterface
{
public:
  TvThemeLoader()
  {
    Vconf.NotifyKeyChanged("db/menu/system/accessibility/highcontrast", &HighContrastChangedCallback);
  }

  bool GetColor(Dali::StringView colorId, Dali::Vector4& outColor) override
  {
    // 예: vconf, DB, 파일 또는 data 영역 등에서 테이블을 읽어 와 컬러를 조회
    return mColorTable.Lookup(colorId, outColor);
  }

  Dali::String GetCurrentThemeId() const override
  {
    return mCurrentThemeId;
  }

  ThemeChangedSignalType& ThemeChangedSignal() override
  {
    return mThemeChangedSignal;
  }

  void HighContrastChangedCallback(const Dali::String& newThemeId)
  {
    mCurrentThemeId = newThemeId;
    // 색상 테이블 갱신 후 signal 발행
    mThemeChangedSignal.Emit();
  }

private:
  Dali::String             mCurrentThemeId;
  ThemeChangedSignalType   mThemeChangedSignal;
  // ...
};
```

**2. UiConfigImpl을 상속하여 CreateThemeLoader() 오버라이드**

```cpp
// tv-config-impl.h (integration-api 레벨)
class TvConfigImpl : public Dali::Ui::Integration::UiConfigImpl
{
public:
  static Dali::IntrusivePtr<TvConfigImpl> New() { return new TvConfigImpl(); }

  Dali::Ui::ThemeLoaderInterface* CreateThemeLoader() override
  {
    return new TvThemeLoader();
  }
};
```

**3. TVConfig에 TvConfigImpl 연결**

[Configuration](링크)에서 소개한 `TVConfig` 패턴과 결합합니다:

```cpp
// tv-config.h (public-api 레벨)
class TVConfig : public Dali::Ui::UiComponentConfig
{
public:
  static TVConfig New()
  {
    auto* impl = TvConfigImpl::New().Get();
    TVConfig config(impl);
    return config
      .SetDpi(72)
      .SetScalingFactor(2.0f);
  }

private:
  explicit TVConfig(Dali::Ui::Integration::UiConfigImpl* impl)
  : UiComponentConfig(/* ... */) {}
};
```

앱 개발자는 동일하게 한 줄로 사용합니다:
```cpp
TVConfig::New().Apply();
```

<br/>

## Color ID 확장 (for Framework Developers)

플랫폼 전용 color ID는 확장 라이브러리에서 정의하고 배포하는 것을 권장합니다:

```cpp
// tv-colors.h
struct TVColors
{
  static const Dali::Ui::UiColor SURFACE_ON_FOCUSED;
  static const Dali::Ui::UiColor SURFACE_ON_SELECTED;
  // ...
};

// tv-colors.cpp
const Dali::Ui::UiColor TVColors::SURFACE_ON_FOCUSED(Dali::String("SurfaceOnFocused"));
const Dali::Ui::UiColor TVColors::SURFACE_ON_SELECTED(Dali::String("SurfaceOnSelected"));
```

앱 개발자는 다음과 같이 사용합니다:
```cpp
view.SetBackgroundColor(TVColors::SURFACE_ON_FOCUSED);
view.SetBackgroundColor(TVColors::SURFACE_ON_FOCUSED.ScaleAlpha(0.5f));
```

> [!WARNING]
> **주의**: color token은 매 색상 적용 시 테이블 lookup이 발생합니다.
> 동일한 색상을 alpha 값만 달리하여 여러 token으로 등록하는 것은 지양하고,
> `ScaleAlpha` / `WithAlpha`를 활용하여 token 수를 최소화하세요.

<br/>

## Color Override (for App Developers)

특정 상황에서 테마와 무관하게 일부 color를 재정의해야 하는 경우,
`UiColorManager::SetColorOverride()`를 사용합니다:

```cpp
bool MyOverride(Dali::StringView id, Dali::Vector4& out)
{
  if(id == "Primary") { out = Dali::Vector4(1, 0, 0, 1); return true; }
  return false;  // 다른 color는 테마에서 정상 조회
}

UiColorManager::Get().SetColorOverride(MyOverride);

// 해제
UiColorManager::Get().ClearColorOverride();
```

Override가 설정되면 기존 View 바인딩이 즉시 갱신됩니다.

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#documents)
