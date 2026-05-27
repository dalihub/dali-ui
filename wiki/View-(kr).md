## View Architecture

`View`는 **p-impl(pointer-to-implementation) 패턴**을 기반으로 설계되어 있습니다. Framework 개발자는 이 패턴을 이해해야 새로운 컴포넌트를 올바르게 정의할 수 있습니다.

<img src="./assets/view-structure.png" style="display:block;margin:0 auto"/>

<br/>

클래스 | 역할 | 설명
--|--|--
`View` | Handle | `ViewImpl` 인스턴스에 대한 포인터만 소유
`ViewImpl` | Implementation | 데이터와 로직을 담당하는 실제 구현체

<br/>

<ins>메모리 관리 구조</ins>
- `View`를 복사하면 동일한 `ViewImpl`을 가리키는 새 핸들이 만들어집니다.
- 모든 `View` 핸들이 소멸되면 `ViewImpl`이 자동으로 해제됩니다.

<br/>

<details>
<summary><ins>상호 접근 방법</ins></summary>

#### View에서 ViewImpl 접근: `GetImpl()`

`View` 클래스는 `GetImpl()` 헬퍼로 handle에서 impl 참조를 얻습니다.

```cpp
// public-api/view-impl.h 에 정의
ViewImpl& GetImpl(View& view);
const ViewImpl& GetImpl(const View& view);
```
```cpp
// 사용 예
ViewImpl& impl = GetImpl(view);
```

<br/>

#### ViewImpl에서 View 접근: `Self()` + `DownCast()`

ViewImpl 인스턴스에서 View의 기능을 사용하려면, 아래와 같이 View를 추가 생성합니다.

```cpp
// 사용 예
View view = View::DownCast(impl.Self());
```

</details>

<br/>

## View Extensibility

이 섹션은 `View`에 유저 데이터나 특정 기능을 붙이는 방법, 그리고 `View`를 상속하여 신규 클래스는 정의하는 방법에 대해 설명하고, 사용자가 목적에 따라 적절한 방법을 선택하여 `View`를 확장할 수 있도록 가이드합니다.

<br/>

### Data Attachment

`dali-ui`는 view 에 유저 데이터를 붙일 수 있는 API를 제공합니다. 유저 데이터는 view 인스턴스가 소멸될 때 함께 소멸되도록 관리됩니다.

<ins>Define data class</ins>
```cpp
class MyData
{
  /* ... */
};
```

<ins>Create a data instance and attach to a view</ins>
```cpp
const AttachmentId MY_DATA_ID = AttachmentId::Alloc();
```
```cpp
view.SetAttachment(MY_DATA_ID, Dali::MakeUnique<MyData>());
```

<ins>Access to data</ins>
```cpp
MyData* data = view.GetAttachment(MY_DATA_ID);
```

단순히 데이터 저장을 위한 목적으로는 상속보다 data attachment를 사용하는 것이 좋습니다.

<br/>

### Layout Attachment

사용자는 `View` 인스턴스에 `LayoutManager`를 붙여 레이아웃 역할을 하도록 만들 수 있습니다. 예를 들어 view에 `GridLayoutManager`를 연결하면 해당 view는 `GridLayout`처럼 자식들을 배치합니다.

<img src="./assets/layout-manager.png"/>


```cpp
// Make view to arrange its children like a GridLayout
view.SetLayoutManager(MakeUnique<GridLayoutManager>());
```

> [!NOTE]
> API 이름과 사용 방식은 변경될 수 있음

이 기능으로 view는 레이아웃 뷰를 child로 붙이지 않고도 레이아웃 역할을 할 수 있게 되므로, 앱이나 컴포넌트를 구현할 때 view의 갯수를 줄이는 데 도움이 됩니다.

<br/>

### Feature Attachment

`dali-ui` 는 view에 붙일 수 있는 몇가지 기능 모듈을 제공합니다.

기능명 | 사용법 | 설명
-- | -- | --
Interaction | `view.AsInteractive()`| View의 interaction을 고수준의 시그널로 변환하여 송출하는 기능. <ul><li>제공하는 시그널: `Clicked`, `LongPressed` 등</li><li>제공하는 상태: `PRESSED`</li></ul>
Selection | `view.AsSelectable()` | View의 clicked interaction을 트리거로 selection 상태를 toggle 하는 기능. <ul><li>제공하는 시그널: `SelectionChanged`</li><li>제공하는 상태: `SELECTED`</li></ul>

```cpp
view.AsInteractive([&](InteractiveTrait trait)
{
  trait.ClickedSignal().Connect(&tracker, [](View v, const InputEvent& e)
  {
    // 클릭됨 (터치 탭 또는 실행키 누름)
  });
});
```

이 기능을 사용하여 사용자는 `Button` 이나 `Switch` 컴포넌트를 상속하지 않고도 view가 해당 기능을 수행하도록 만들 수 있으며, view의 내부를 자유롭게 구성할 수 있습니다.

<br/>

### View Inheritance

Framework 개발자가 새 컴포넌트를 만들거나 앱이 커스텀 컴포넌트를 만드고 싶을때는 `View`의 **handle 클래스**와 **impl 클래스**를 각각 상속합니다.

<img src="./assets/custom-view.png" style="display:block;margin:0 auto"/>

#### 1. Impl 클래스 정의 (`ViewImpl` 상속)

```cpp
// my-view-impl.h

class MyViewImpl : public ViewImpl
{
public:
  static IntrusivePtr<MyViewImpl> New();

private:
  int mMyData0;
  float mMyData1;
};
```

#### 2. Handle 클래스 정의 (`View` 상속)

```cpp
// my-view.h
class MyView : public View
{
public:
  static MyView New();

  // DownCast: View::DownCast<T, I>() 템플릿 사용
  static MyView DownCast(BaseHandle handle)
  {
    return View::DownCast<MyView, MyViewImpl>(handle);
  }

  void DoSomething();

  ~MyView(); // non-virtual
};
```

#### 3. `New()` 구현

```cpp
// my-view.cpp
MyView MyView::New()
{
  IntrusivePtr<MyViewImpl> impl = MyViewImpl::New();
  MyView handle(*impl);   // handle이 impl의 소유권을 획득
  impl->Initialize();
  return handle;
}
```

<br/>

#### 주의사항

| 항목 | 규칙 |
|------|------|
| Handle 소멸자 | `~MyView()` 는 반드시 **non-virtual** |
| Handle 데이터 | Handle 클래스에 멤버 변수를 추가하지 않는다 |
| 로직 위치 | 모든 상태와 로직은 Impl에 위치해야 한다 |

<br/>

### Inheriting from Non-View Classes

앱이 `View` 이외의 클래스를 상속받는 것(예를 들면 `ImageView`나 `Label`)은 약간의 트릭이 필요합니다. 핸들과 impl 클래스가 모두 제공되는 것은 `View` 뿐이기 때문에, 그 이외의 view 클래스들을 상속하려면 data attachment를 이용한 제한적인 방법을 적용해 볼 수 있습니다. 자세한 방법 및 주의 사항은 [#327](https://github.sec.samsung.net/NUI/dali-ui/pull/327)을 참고하세요.

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki/Home-(kr)#view-and-extensibility)
