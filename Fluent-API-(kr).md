> [!CAUTION]
> This page is no longer valid (2026.06.08~)

<br/>

# Fluent API

View 계열 fluent API는 제거되었습니다.

`View`, `Label`, `InputField`, `InputEditor`, `ScrollView`, layout class 등 앱 또는
프레임워크 개발자가 상속할 수 있는 public API는 더 이상 아래와 같은 메소드 체인을
지원하지 않습니다.

```cpp
View::New()
  .SetRequestedWidth(100.0f)
  .SetRequestedHeight(100.0f);
```

대신 일반 statement 형태로 작성합니다:

```cpp
View view = View::New();
view.SetRequestedWidth(100.0f);
view.SetRequestedHeight(100.0f);
```

`As`, `With`도 View API에서 제거되었습니다. handle 캡처와 커스텀 초기화는 별도
statement로 작성하세요.

Visual, AnimationSpec/AnimationBridge, LayoutParams, Text style value object처럼
상속 문제가 같은 방식으로 발생하지 않는 builder 성격의 API는 fluent chaining을
유지할 수 있습니다.

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki/Home-(kr)#development-guides)
