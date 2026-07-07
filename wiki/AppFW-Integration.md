[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/AppFW-Integration-(kr))

# AppFW Integration

This guide explains how to build a dali-ui application on top of the Tizen Application Framework (AppFW) using `tizen_appfw::UiApplication`.

<br/>

## 1. Creating an Application

Inherit from `tizen_appfw::UiApplication` and implement `OnCreate()`.

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
    Dali::Window window = GetDefaultWindow(); // Same as Dali::UiContext::Get().GetDefaultWindow()
    window.SetBackgroundColor(Dali::Color::WHITE);

    Dali::Ui::Label label = Dali::Ui::Label::New("Hello Tizen C++!");
    window.Add(label);

    return true; // false terminates the app immediately
  }
};

int main(int argc, char** argv) {
  MyApp app(argc, argv);
  return app.Run();
}
```

> [!NOTE]
> `OnCreate()` is a pure virtual function and must be overridden. Return `true` if initialization succeeds, `false` if it fails.
> Returning `false` terminates the application immediately without starting the main loop.

* `GetDefaultWindow()` can be called starting from `OnCreate()`.
  * The default window is not available yet in the constructor.
  * It returns the default window created internally by DALi.
  * To create additional windows, use `Dali::Window::New()`.
* `Dali::UiContext::Get()` is also available starting from `OnCreate()`.
  * `Dali::UiContext::Get().GetDefaultWindow()` returns the same window as `tizen_appfw::UiApplication::GetDefaultWindow()`.

<br/>

## 2. Application Lifecycle Callbacks

`tizen_appfw::UiApplication` supports the following lifecycle callbacks: `OnCreate()`, `OnPause()`, `OnResume()`, `OnTerminate()`, `OnControl()`.

* Other than `OnCreate()`, override only the callbacks you actually need.
  * `OnCreate()` must be implemented.
  * Any callback that is not overridden simply does nothing.

<br/>

## 3. Terminating the Application

Call `Exit()` to terminate the application. `OnTerminate()` is invoked before the main loop exits.

```cpp
void MyApp::OnSomeButtonClicked() {
  Exit();
}
```

<br/>

## 4. Handling System Events

To react to system state changes, override the corresponding callback:

`OnLowMemory()`, `OnLowBattery()`, `OnLanguageChanged()`, `OnRegionFormatChanged()`, `OnDeviceOrientationChanged()`, `OnSuspendedStateChanged()`, `OnTimeZoneChanged()`, etc.

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#development-guides)
