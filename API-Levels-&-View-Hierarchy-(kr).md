## API Levels

`dali-ui`는 안정성 보장 수준과 대상 독자에 따라 세 가지 API 레벨을 제공합니다.

| 레벨 | 위치 | 대상 | ABI 안정성 |
|------|------|------|------------|
| **Public API** | `public-api/` | 앱 개발자 | **보장** — 릴리즈 간 ABI가 깨져서는 안 됨 |
| **Integration API** | `integration-api/` | Framework 개발자 | 불필요 — dali-ui와 함께 재빌드 가능 |
| **Internal API** | `internal/` | dali-ui 내부 전용 | 불필요 |

**앱 개발자**라면 `public-api/`만 사용하세요. ABI 안정성이 보장되는 유일한 레이어입니다.

**Framework 개발자**라면 `integration-api/`도 사용할 수 있습니다. 다만 dali-ui 버전 간에 변경될 수 있으며, 변경 시 재빌드가 필요합니다.

`internal/`은 라이브러리 외부에서 직접 사용하는 것을 지원하지 않습니다.

<br/>

## View Architecture (Handle / Impl Pattern)

dali-ui의 `View`는 DALi의 **p-impl(pointer-to-implementation) 패턴**을 기반으로 설계되어 있습니다. Framework 개발자는 이 패턴을 이해해야 새로운 컴포넌트를 올바르게 정의할 수 있습니다.

![view-pimpl-class](https://github.sec.samsung.net/NUI/dali-ui/assets/1000/45850c41-206f-4c15-8c67-a77e93f2345d)

<br/>

### Handle 클래스: `View` (public-api)

`View`는 `Dali::CustomActor`를 상속한 **handle 클래스**입니다. 실제 구현 데이터를 직접 보유하지 않고, `ViewImpl` 인스턴스에 대한 ref-counted 포인터를 통해 구현체를 참조합니다.

- `View`를 복사하면 동일한 `ViewImpl`을 가리키는 새 handle이 만들어집니다.
- 모든 `View` handle이 소멸되면 `ViewImpl`이 자동으로 해제됩니다.
- `~View()`는 **non-virtual**입니다. handle 클래스에 가상 함수나 멤버 데이터를 추가해서는 안 됩니다.

```cpp
// public-api/view.h (Dali::Ui 네임스페이스)
class View : public CustomActor
{
public:
  static View New();
  static View DownCast(BaseHandle handle);

  // Handle은 non-virtual 소멸자
  ~View();

  // 데이터나 가상 함수 없음 — 모두 ViewImpl에 위임
};
```

<br/>

### 구현 클래스: `ViewImpl` (integration-api)

`ViewImpl`은 `Dali::CustomActorImpl`을 상속한 **구현 클래스**입니다. 실제 상태와 로직이 여기에 있습니다. `Dali::Ui::Integration` 네임스페이스 안에 위치합니다.

- `OnMeasure()` / `OnArrange()` 등 virtual 메서드를 오버라이드해 동작을 커스터마이즈합니다.
- ref-counting은 `IntrusivePtr<ViewImpl>`(`ViewImplPtr`)로 관리됩니다.

```cpp
// integration-api/view-impl.h (Dali::Ui::Integration 네임스페이스)
class ViewImpl : public CustomActorImpl, public ConnectionTrackerInterface
{
public:
  static ViewImplPtr New();

protected:
  virtual MeasuredSize OnMeasure(float widthConstraint, float heightConstraint);
  virtual MeasuredSize OnArrange(const LayoutRect& bounds);

  // 상태 변경 (traits에서 호출)
  void SetViewState(UiState state, bool on, InputEvent cause = {});
};
```

<br/>

### Handle에서 Impl 접근: `GetImpl()`

integration-api를 사용하는 Framework 개발자는 `GetImpl()` 헬퍼로 handle에서 impl 참조를 얻습니다.

```cpp
// integration-api/view-impl.h 에 정의
Integration::ViewImpl& GetImpl(Ui::View& view);
const Integration::ViewImpl& GetImpl(const Ui::View& view);

// 사용 예
void MyFunction(Ui::View& view)
{
  Integration::ViewImpl& impl = Integration::GetImpl(view);
  impl.SetViewState(UiState::FOCUSED, true);
}
```

<br/>

## 새 컴포넌트 정의하기

Framework 개발자가 새 컴포넌트를 만들 때는 **handle 클래스**와 **impl 클래스**를 함께 정의합니다. `Label` / `LabelImpl`이 그 예입니다.

### 1. Impl 클래스 정의 (integration-api)

```cpp
// my-widget-impl.h
namespace Dali::Ui::Integration {

class MyWidgetImpl : public ViewImpl
{
public:
  static IntrusivePtr<MyWidgetImpl> New();

protected:
  MeasuredSize OnMeasure(float w, float h) override;
  MeasuredSize OnArrange(const LayoutRect& bounds) override;
  void OnInitialize() override;
};

} // namespace Dali::Ui::Integration
```

### 2. Handle 클래스 정의 (public-api)

```cpp
// my-widget.h
namespace Dali::Ui {

class MyWidget : public View
{
public:
  static MyWidget New();

  // DownCast: View::DownCast<T, I>() 템플릿 사용
  static MyWidget DownCast(BaseHandle handle)
  {
    return View::DownCast<MyWidget, Integration::MyWidgetImpl>(handle);
  }

  void DoSomething();

  ~MyWidget(); // non-virtual
};

} // namespace Dali::Ui
```

### 3. `New()` 구현

```cpp
// my-widget.cpp
MyWidget MyWidget::New()
{
  IntrusivePtr<Integration::MyWidgetImpl> impl = Integration::MyWidgetImpl::New();
  MyWidget handle(*impl);   // handle이 impl의 소유권을 획득
  impl->Initialize();
  return handle;
}
```

<br/>

## 주의사항

| 항목 | 규칙 |
|------|------|
| Handle 소멸자 | `~MyWidget()` 은 반드시 **non-virtual** |
| Handle 데이터 | Handle 클래스에 멤버 변수를 추가하지 않는다 |
| Handle 가상 함수 | Handle 클래스에 virtual 함수를 추가하지 않는다 |
| 로직 위치 | 모든 상태와 로직은 Impl에 위치해야 한다 |
| ABI 안정성 | Handle 클래스(`public-api`)의 ABI는 최우선으로 보호해야 한다 |

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#documents)
