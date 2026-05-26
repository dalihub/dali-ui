## What is View?

`View`는 `dali-ui`의 기본이 되는 UI building block 이며 주요 기능은 다음과 같습니다.

* Shadow, Background, Content, Border 등 다층 비주얼 레이어 구성
* 레아이웃에 의해 배치되는 대상
* FOCUSED, DISABLED 등 상태 관리
* 접근성 기능 제공

## Data attachment

DALi 대부분의 객체는 p-impl 구조로 정의되어 있고 `View`도 마찬가지 입니다. NUI나 FLUX를 사용하여 앱을 작성할 때, View 나 Layout 또는 Button등의 컴포넌트를 단순 상속받아 앱이 원하는 데이터를 추가하는 형태의 코딩 패턴은 네이티브에서 유효하지 않습니다. (p-impl 의 핸들에 데이터를 추가하면 upcast 과정에서 소실되기 때문)


이에 대한 대안으로 view 에 데이터를 붙일 수 있는 API가 제공되며, 붙인 데이터는 view 인스턴스가 소멸될 때 함께 소멸되도록 관리됩니다.



<ins>Define data class</ins>
```cpp
class MyData
{
  /* ... */
};
```

<ins>Create a data and attach to a view</ins>
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

<br/>

## View Architecture (Handle / Impl Pattern)

`View`는 DALi의 **p-impl(pointer-to-implementation) 패턴**을 기반으로 설계되어 있습니다. Framework 개발자는 이 패턴을 이해해야 새로운 컴포넌트를 올바르게 정의할 수 있습니다.

![view-pimpl-class](https://github.sec.samsung.net/NUI/dali-ui/assets/1000/45850c41-206f-4c15-8c67-a77e93f2345d)

<br/>

### Handle 클래스: `View`

`View`는 `Dali::CustomActor`를 상속한 **handle 클래스**입니다. 실제 구현 데이터를 직접 보유하지 않고, `ViewImpl` 인스턴스에 대한 ref-counted 포인터를 통해 구현체를 참조합니다.

- `View`를 복사하면 동일한 `ViewImpl`을 가리키는 새 handle이 만들어집니다.
- 모든 `View` handle이 소멸되면 `ViewImpl`이 자동으로 해제됩니다.

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

### 구현 클래스: `ViewImpl`

`ViewImpl`은 `Dali::CustomActorImpl`을 상속한 **구현 클래스**입니다. 실제 상태와 로직이 여기에 있습니다.

```cpp
// public-api/view-impl.h
class ViewImpl : public CustomActorImpl, public ConnectionTrackerInterface
{
public:
  static ViewImplPtr New();

protected:
  virtual MeasuredSize OnMeasure(float widthConstraint, float heightConstraint);
  virtual MeasuredSize OnArrange(const LayoutRect& bounds);
};
```

<br/>

### Handle에서 Impl 접근: `GetImpl()`

`View` 클래스는 `GetImpl()` 헬퍼로 handle에서 impl 참조를 얻습니다.

```cpp
// public-api/view-impl.h 에 정의
ViewImpl& GetImpl(View& view);
const ViewImpl& GetImpl(const View& view);
```
```cpp
// 사용 예
void MyFunction(View& view)
{
  ViewImpl& impl = GetImpl(view);
  // ...
}
```

<br/>

## 커스텀 컴포넌트 정의하기

Framework 개발자가 새 컴포넌트를 만들거나 앱이 커스텀 컴포넌트를 만드고 싶을때는 **handle 클래스**와 **impl 클래스**를 함께 정의합니다.

### 1. Impl 클래스 정의 (View 상속)

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

### 2. Handle 클래스 정의 (ViewImpl 상속)

```cpp
// my-view.h
class MyView : public View
{
public:
  static MyView New();

  // DownCast: View::DownCast<T, I>() 템플릿 사용
  static MyView DownCast(BaseHandle handle)
  {
    return View::DownCast<MyView, Integration::MyViewImpl>(handle);
  }

  void DoSomething();

  ~MyView(); // non-virtual
};
```

### 3. `New()` 구현

```cpp
// my-view.cpp
MyView MyView::New()
{
  IntrusivePtr<Integration::MyViewImpl> impl = MyViewImpl::New();
  MyView handle(*impl);   // handle이 impl의 소유권을 획득
  impl->Initialize();
  return handle;
}
```

<br/>

### 주의사항

| 항목 | 규칙 |
|------|------|
| Handle 소멸자 | `~MyView()` 는 반드시 **non-virtual** |
| Handle 데이터 | Handle 클래스에 멤버 변수를 추가하지 않는다 |
| 로직 위치 | 모든 상태와 로직은 Impl에 위치해야 한다 |

<br/>

## FAQ

### Q. View 이외에 다른 클래스를 상속 받고 싶다면?

`View`만 impl 클래스가 public-api로 노출되어 있기 때문에, 앱이 `View` 이외의 클래스를 상속받는것은 불가능합니다.

커스텀 컴포넌트의 보편적으로 

``


<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#documents)
