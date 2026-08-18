# Chromium WebView history 및 JavaScript handler 오동작 수정

## 상태와 범위

- 상태: TODO / 확인된 버그, 미수정
- 작업 타입: Chromium backend 버그 수정, adaptor 계약 명확화
- 주요 대상:
  - `dali-adaptor/dali/devel-api/adaptor-framework/web-engine/web-engine-back-forward-list.h`
  - `dali-extension/dali-extension/web-engine-chromium/ecore/tizen-web-engine-back-forward-list.cpp`
  - `dali-extension/dali-extension/web-engine-chromium/tcore/tizen-web-engine-back-forward-list.cpp`
  - `dali-extension/dali-extension/web-engine-chromium/ecore/tizen-web-engine-chromium.cpp`
  - `dali-extension/dali-extension/web-engine-chromium/tcore/tizen-web-engine-chromium.cpp`
  - `dali-ui-foundation/integration-api/web-back-forward-list-impl.cpp`
- 배경: `Add missing WebView APIs` 패치 리뷰에서 Chromium backend의 history item 처리와
  JavaScript message handler routing에 실제 오동작이 확인되었다.

이 문서는 향후 검토할 가능성을 기록하는 설계 제안이 아니라, 재현 가능한 Chromium
backend 버그 두 건과 필수 수정 방향을 기록한다. 실제 버그 수정은 `dali-extension`의
Chromium plugin에서 수행해야 한다. `dali-adaptor`의 계약 문서화는 구현이 따라야 할
항목 부재 의미를 명확히 하기 위한 후속 작업이다.

## 버그 1: 존재하지 않는 history item을 유효한 wrapper로 반환

### 확인된 오동작

`WebEngineBackForwardList`의 단일 항목 조회 API는 다음과 같이
`std::unique_ptr<WebEngineBackForwardListItem>`을 반환한다.

- `GetCurrentItem()`
- `GetPreviousItem()`
- `GetNextItem()`
- `GetItemAtIndex()`

LWE는 항목을 제공할 수 없을 때 `nullptr`를 반환한다. 현재 LWE의 back-forward list는
전체적으로 미구현 상태이므로 count는 0, 목록은 빈 vector를 반환한다.

Chromium은 EWK가 null native item을 반환해도
`TizenWebEngineBackForwardListItem` wrapper를 생성하여 non-null `unique_ptr`로
반환한다. dali-ui는 non-null adaptor item을 유효한 snapshot으로 변환하므로, 빈
history나 범위 밖 조회가 `IsValid() == true`인 빈 항목으로 노출될 수 있다. wrapper의
getter가 null native item을 EWK API에 전달하는 위험도 있다.

### 필수 수정 방향

항목이 존재하지 않을 때는 `nullptr`를 반환한다.

```cpp
// 항목이 존재함
return std::make_unique<TizenWebEngineBackForwardListItem>(nativeItem);

// current/previous/next/index에 해당하는 항목이 없음
return nullptr;
```

`std::unique_ptr`의 null 상태가 이미 항목의 부재를 표현하므로, null native item을 가진
더미 wrapper와 별도 `IsValid()` API를 adaptor item에 추가하지 않는다. 이중 상태인
`item != nullptr && !item->IsValid()`를 만들지 않고, invalid wrapper allocation과
호출자의 반복 검사를 피한다.

현재 dali-ui API는 다음 상태를 모두 invalid `WebBackForwardListItem`으로 축약한다.

- history 경계에서의 정상적인 항목 부재
- 범위 밖 index
- backend 미지원
- backend 오류

애플리케이션이 `NO_ITEM`, `NOT_SUPPORTED`, `BACKEND_ERROR`를 구분해야 하는 요구가
생기면 더미 wrapper가 아니라 명시적인 status/result 타입을 별도 설계한다. 현재
요구사항에서는 이 구분을 추가하지 않는다.

### 수정 작업

1. `dali-adaptor` 반환값 문서에 "항목이 없으면 `nullptr`" 계약을 명시한다.
2. Chromium ecore와 tcore 구현에서 EWK native item을 먼저 검사하고, null이면 wrapper를
   생성하지 않는다.
3. dali-ui는 현재의 `nullptr`에서 invalid snapshot으로 변환하는 경계를 유지한다.

## 버그 2: 여러 JavaScript object name이 마지막 callback으로 잘못 routing됨

### 확인된 오동작

dali-ui는 다음 두 callback 형태를 `AddJavaScriptMessageHandler()` overload로 제공한다.

- message body만 전달하는 callback
- exposed object name과 message body를 전달하는 callback

이 overload 자체는 잘못된 routing의 원인이 아니다. 이름 포함 경로가 호출하는
adaptor API도 `(exposedObjectName, handler)`를 받으므로, 각 이름의 등록이 해당
handler와 연결되는 계약을 가진다.

LWE는 각 `AddJavaScriptInterface()` 등록 lambda에 이름과 handler를 개별 캡처하고,
`RemoveJavascriptInterface()`로 해당 이름을 제거한다. 서로 다른 이름의 callback이
마지막 등록 callback으로 합쳐지는 문제는 없다.

Chromium은 이름 포함 callback을 단일
`mJavaScriptEntireMessageReceivedCallback`에 저장한다. A와 B에 서로 다른 callback을
등록하면 B 등록이 A의 callback을 덮어쓰고, 이후 A 메시지도 B callback으로 전달된다.
`RemoveJavaScriptMessageHandler()`도 현재 Chromium에서 no-op이다.

### 필수 수정 방향

이름별 callback registry는 dali-ui가 아니라 Chromium plugin이 소유한다.

adaptor API가 이미 이름과 handler의 연결 계약을 소유하므로, dali-ui에 별도 registry를
두면 상태가 중복되고 모든 backend에 불필요한 lookup과 수명 관리가 추가된다. UI
registry는 adaptor/plugin을 즉시 수정할 수 없는 경우의 임시 compatibility bridge로만
검토한다.

Chromium plugin은 개념적으로 다음 상태를 유지한다.

```cpp
exposedObjectName -> registered callback
```

메시지를 dispatch할 때 registry iterator나 callback reference를 잡은 채 사용자
callback을 실행하지 않는다. callback을 지역 변수로 복사한 뒤 실행하여 callback 안에서
같은 이름을 remove하거나 replace하는 재진입에도 현재 호출의 수명을 보장한다.

`RemoveJavaScriptMessageHandler(name)`은 최소한 registry entry를 제거하여 이후 native
callback이 도착해도 사용자 callback이 실행되지 않게 해야 한다. Chromium native API가
노출된 JavaScript object 자체를 제거할 수 없다면, 물리적 제거 불가와 논리적 callback
비활성화를 구분하여 계약과 주석에 기록한다.

### 수정 작업

1. Chromium ecore와 tcore의 단일 이름 포함 callback을 이름별 registry로 변경한다.
2. 동일 이름 재등록 시 기존 callback을 교체한다.
3. remove와 remove 후 재등록을 지원하고, callback 내부 self-remove/self-replace에서
   현재 호출의 수명을 보장한다.
4. body-only handler와 name-included handler를 동일 이름에 등록할 때의 충돌 정책을
   결정하고 adaptor 계약에 명시한다.
5. callback 등록과 native dispatch가 같은 event thread로 직렬화되는지 확인한다. 다른
   thread에서 접근할 수 있다면 registry 동기화 정책을 추가한다.

## 버그 수정 시 결정할 사항

- adaptor 문서 변경과 dali-extension 구현 변경의 배포 및 최소 버전 의존 관계
- body-only handler와 name-included handler의 동일 이름 등록 정책
- Chromium native API가 handler/object를 실제 제거하지 못할 때 public remove 계약에
  노출할 상세 수준
