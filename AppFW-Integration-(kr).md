# AppFW Integration

`tizen_appfw::UiApplication`을 이용해 dali-ui 애플리케이션을 Tizen Application Framework(AppFW)와 통합하는 방법을 설명합니다.

<br/>

## 1. Application 생성

`tizen_appfw::UiApplication`을 상속받은 후 `OnCreate()`를 구현합니다.

```cpp
#include <tizen_appfw/ui-application.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali/dali.h>

class MyApp : public tizen_appfw::UiApplication, public Dali::ConnectionTracker
{
public:
  MyApp(int argc, char** argv) : UiApplication(argc, argv) {}

protected:
  bool OnCreate() override {
    Dali::Window window = GetDefaultWindow(); // Dali::UiContext::Get().GetDefaultWindow()와 동일
    window.SetBackgroundColor(Dali::Color::WHITE);

    Dali::Ui::Label label = Dali::Ui::Label::New("Hello Tizen C++!");
    window.Add(label);

    return true; // false를 반환하면 앱이 즉시 종료된다
  }
};

int main(int argc, char** argv) {
  MyApp app(argc, argv);
  return app.Run();
}
```

> [!NOTE]
> `OnCreate()`는 순수 가상 함수로, 반드시 오버라이드해야 합니다. 초기화에 성공하면 `true`, 실패하면 `false`를 반환합니다.
> `false`를 반환하면 메인 루프가 시작되지 않고 앱이 바로 종료됩니다.

* `GetDefaultWindow()`는 `OnCreate()`부터 호출 가능합니다.
  * 생성자에서는 default window를 받아올 수 없습니다.
  * DALi 내부에서 만든 default window를 반환합니다.
  * 추가 윈도우 생성을 위해서는 `Dali::Window::New()`를 이용합니다.
* `OnCreate()`부터는 `Dali::UiContext::Get()`도 사용 가능합니다.
  * `Dali::UiContext::Get().GetDefaultWindow()`는 `tizen_appfw::UiApplication::GetDefaultWindow()`와 같은 window를 반환합니다.

<br/>

## 2. Application Lifecycle Callback

`tizen_appfw::UiApplication`은 `OnCreate()`, `OnPause()`, `OnResume()`, `OnTerminate()`, `OnControl()` 콜백을 지원합니다.

* `OnCreate()`를 제외하고는 필요한 것만 오버라이드해서 구현하면 됩니다.
  * `OnCreate()`는 구현이 필수입니다.
  * 나머지는 구현하지 않으면 아무 동작도 하지 않습니다.

<br/>

## 3. Application 종료

`Exit()`을 호출하면 앱이 종료됩니다. `OnTerminate()`가 호출된 뒤 메인 루프가 종료됩니다.

```cpp
void MyApp::OnSomeButtonClicked() {
  Exit();
}
```

<br/>

## 4. 시스템 이벤트 처리

시스템 상태 변화를 처리하려면 해당하는 함수를 오버라이드해서 구현합니다.

`OnLowMemory()`, `OnLowBattery()`, `OnLanguageChanged()`, `OnRegionFormatChanged()`, `OnDeviceOrientationChanged()`, `OnSuspendedStateChanged()`, `OnTimeZoneChanged()` 등

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki/Home-(kr)#development-guides)
