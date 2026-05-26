## UiLocalizationManager

`UiLocalizationManager`는 dali-ui에서 다국어 문자열을 조회하고 `View`/`BaseHandle`에 등록된 binding을 갱신하는 manager입니다.

기본 모델:

~~~text
resourceId + domain -> localized string
~~~

localized string은 기본적으로 UI 문자열이며, Direct Binding을 응용하면 resource path나 image URL 같은 locale-dependent string에도 사용할 수 있습니다.

샘플: [text-localization-po-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-localization-po-example.cpp)

<br/>

## 기본 사용 흐름

1. `.po` 파일 작성
2. 빌드 과정에서 `.po` 파일을 `.mo` 파일로 변환
3. 생성된 `.mo` 파일을 gettext runtime layout에 맞게 설치/패키징
4. `UiLocalizationManager::RegisterDomain()` 호출
5. `SetDefaultDomain()`으로 default domain 설정
6. `Label::SetTranslatableText()`, `InputField::SetTranslatablePlaceholder()`, `InputEditor::SetTranslatablePlaceholder()` 또는 `SetBindingResource()` 사용

~~~cpp
constexpr const char* TEXT_DOMAIN     = "myapp";
constexpr const char* TEXT_LOCALE_DIR = RESOURCES_DIR "locale";

UiLocalizationManager manager = UiLocalizationManager::Get();

manager.RegisterDomain(TEXT_DOMAIN, TEXT_LOCALE_DIR);
manager.SetDefaultDomain(TEXT_DOMAIN);

Label title = Label::New()
  .SetTranslatableText("IDS_TITLE");
~~~

> [!NOTE]
> `.po` 파일 이름은 domain을 결정하지 않고, runtime에 생성되는 `<domain>.mo` 파일 이름이 gettext domain과 일치해야 합니다.

<br/>

## PO/MO 리소스 구조

`.po` 파일은 번역 source 파일이고, `.mo` 파일은 runtime lookup에 사용되는 gettext binary catalog입니다.

예를 들어 source PO 파일은 locale별로 다음처럼 둘 수 있습니다:

~~~text
res/po/default/ko_KR.po
res/po/default/en_US.po
res/po/default/ar_AE.po
~~~

빌드 과정에서는 각 `.po` 파일을 gettext runtime layout에 맞는 `.mo` 파일로 변환합니다.

~~~text
<localePath>/<locale>/LC_MESSAGES/<domain>.mo
~~~

예:

~~~text
res/
 ├─ po/
 │   └─ default/
 │       ├─ ko_KR.po
 │       └─ en_US.po
 │
 └─ locale/
     └─ default/   <- RegisterDomain("text-localization-po", "res/locale/default")
         ├─ ko_KR/
         │   └─ LC_MESSAGES/
         │       └─ text-localization-po.mo
         └─ en_US/
             └─ LC_MESSAGES/
                 └─ text-localization-po.mo
~~~

이 경우 domain은 `text-localization-po`이고, `localePath`는 `res/locale/default`입니다.

~~~cpp
UiLocalizationManager::Get().RegisterDomain(
  "text-localization-po",
  RESOURCES_DIR "locale/default");
~~~

`msgfmt` 예시:

~~~bash
msgfmt -o res/locale/default/ko_KR/LC_MESSAGES/text-localization-po.mo res/po/default/ko_KR.po
~~~

샘플 CMake는 `samples/text/CMakeLists.txt`를 참고하세요.

<br/>

## PO 파일 예시

~~~po
msgid ""
msgstr ""
"Content-Type: text/plain; charset=UTF-8\n"
"Content-Transfer-Encoding: 8bit\n"

msgid "IDS_TITLE"
msgstr "Hello"

msgid "IDS_DESCRIPTION"
msgstr "This is a localized description."
~~~

PO 파일은 runtime lookup에 직접 사용되지 않고, gettext는 compile된 `.mo` 파일을 사용합니다.

<br/>

## Domain

### RegisterDomain

~~~cpp
UiLocalizationManager manager = UiLocalizationManager::Get();
manager.RegisterDomain("myapp", RESOURCES_DIR "locale");
~~~

- `domain`: gettext domain 이름 (`.mo` 파일 이름과 일치해야 함)
- `localePath`: `<locale>/LC_MESSAGES/<domain>.mo` 구조의 상위 디렉토리 경로

`RegisterDomain()`은 내부적으로 gettext의 `bindtextdomain()`을 호출하고 codeset을 UTF-8로 설정합니다.

### Default Domain

~~~cpp
manager.SetDefaultDomain("myapp");

Label title = Label::New()
  .SetTranslatableText("IDS_TITLE");
~~~

default domain은 domain 인자를 생략한 lookup/binding에서 사용할 기준 domain입니다.
예를 들어 `GetLocalizedString("IDS_TITLE")` 또는 `SetTranslatableText("IDS_TITLE")`처럼 domain을 전달하지 않으면 default domain을 사용합니다.
default domain이 변경되면 domain 없이 등록된 binding이 refresh됩니다.

### Explicit Domain

~~~cpp
Label title = Label::New()
  .SetTranslatableText("IDS_TITLE", "myapp-settings");
~~~

explicit domain은 default domain 변경에 영향을 받지 않습니다.

> [!NOTE]
> 같은 프로세스에서 여러 패키지나 가젯이 동작하더라도, 각자 고유한 domain과 locale path를 등록하고 explicit domain을 사용하면 독립적으로 동작할 수 있습니다.

<br/>

## Label 다국어 텍스트

~~~cpp
Label& SetTranslatableText(StringView resourceId);
Label& SetTranslatableText(StringView resourceId, StringView domain);
Dali::String GetTranslatableText() const;
void ClearTranslatableText();
~~~

PO 예:

~~~po
msgid "IDS_TITLE"
msgstr "Title"
~~~

default domain 사용:

~~~cpp
Label title = Label::New()
  .SetTranslatableText("IDS_TITLE");
~~~

explicit domain 사용:

~~~cpp
Label title = Label::New()
  .SetTranslatableText("IDS_TITLE", "myapp-settings");
~~~

> [!WARNING]
> `SetText()`는 translatable text binding을 제거하지 않습니다.
> `SetText()`로 설정한 텍스트를 계속 유지해야 한다면 `ClearTranslatableText()`를 먼저 호출해야 합니다.

<br/>

## InputField / InputEditor Placeholder

~~~cpp
// InputField
InputField& SetTranslatablePlaceholder(StringView resourceId);
InputField& SetTranslatablePlaceholder(StringView resourceId, StringView domain);
Dali::String GetTranslatablePlaceholder() const;
void ClearTranslatablePlaceholder();

// InputEditor
InputEditor& SetTranslatablePlaceholder(StringView resourceId);
InputEditor& SetTranslatablePlaceholder(StringView resourceId, StringView domain);
Dali::String GetTranslatablePlaceholder() const;
void ClearTranslatablePlaceholder();
~~~

~~~cpp
InputField field = InputField::New()
  .SetTranslatablePlaceholder("IDS_SEARCH_PLACEHOLDER");

InputEditor editor = InputEditor::New()
  .SetTranslatablePlaceholder("IDS_COMMENT_PLACEHOLDER");
~~~

<br/>

## Direct Lookup

`GetLocalizedString()`을 사용하면 binding 없이 직접 localized string을 조회할 수 있습니다.

~~~cpp
UiLocalizationManager manager = UiLocalizationManager::Get();

Dali::String title = manager.GetLocalizedString("IDS_TITLE");
Dali::String settingsTitle = manager.GetLocalizedString("IDS_TITLE", "myapp-settings");
~~~

> [!NOTE]
> 번역을 찾지 못하면 기본적으로 resourceId가 반환됩니다.

<br/>

## Direct Binding

`SetBindingResource()`를 사용하면 localized string을 resolve하여 callback으로 전달합니다.
callback은 해당 값을 target에 어떻게 적용할지 결정합니다.

~~~cpp
void SetBindingResource(BaseHandle target,
                        StringView bindingId,
                        StringView resourceId,
                        LocalizedStringCallback callback);

void SetBindingResource(BaseHandle target,
                        StringView bindingId,
                        StringView resourceId,
                        StringView domain,
                        LocalizedStringCallback callback);
~~~

`domain` 인자를 생략하면 default domain을 사용하고, explicit domain을 전달하면 해당 domain을 사용합니다.

default domain 사용:

~~~cpp
Label label = Label::New();

UiLocalizationManager::Get().SetBindingResource(
  label,       // target
  "Text",      // binding id
  "IDS_TITLE", // resource id
  LocalizedStringCallback::New(this, &MyApp::ApplyLabelText));
~~~

explicit domain 사용:

~~~cpp
UiLocalizationManager::Get().SetBindingResource(
  label,            // target
  "Text",           // binding id
  "IDS_TITLE",      // resource id
  "myapp-settings", // domain
  LocalizedStringCallback::New(this, &MyApp::ApplyLabelText));
~~~

이 binding은 default domain 변경에 영향을 받지 않고 `myapp-settings` domain을 사용합니다.

~~~cpp
void MyApp::ApplyLabelText(BaseHandle target, const Dali::String& text)
{
  Label label = Label::DownCast(target);
  if(label)
  {
    label.SetText(text);
  }
}
~~~

> [!NOTE]
> binding은 `target + bindingId` 조합으로 식별됩니다.
> 같은 target에 같은 bindingId를 다시 등록하면 기존 binding이 교체됩니다.

> [!NOTE]
> `ClearBinding()`은 binding만 제거하며 현재 적용된 text나 property 값은 변경하지 않습니다.

<br/>

## ImageView URL 바인딩 응용

Direct Binding을 사용하면 text 외의 locale-dependent string에도 응용할 수 있습니다.

PO 예:

~~~po
msgid "IDS_FLAG_IMAGE_URL"
msgstr "flag_kr.png"
~~~

C++ 예:

~~~cpp
ImageView image = ImageView::New();

UiLocalizationManager::Get().SetBindingResource(
  image,
  "ImageUrl",
  "IDS_FLAG_IMAGE_URL",
  "myapp",
  LocalizedStringCallback::New(this, &MyApp::ApplyImageUrl));

void MyApp::ApplyImageUrl(BaseHandle target, const Dali::String& fileName)
{
  ImageView image = ImageView::DownCast(target);
  if(image)
  {
    image.SetResourceUrl(Dali::String(RESOURCES_DIR) + fileName);
  }
}
~~~

<br/>

## Custom Component Localization

custom component 자체를 binding target으로 사용하면, 내부 Label이나 InputField 등을 외부에 노출하거나 별도의 `TranslatableXXX` 프로퍼티를 추가하지 않고도 localization을 적용할 수 있습니다.

아래 예시는 locale 변경에 따라 card 내부 text와 image binding이 함께 갱신되는 흐름을 보여줍니다.

![Custom component localization](./assets/text/localization.gif)

~~~cpp
LocalizedCard card = LocalizedCard::New(); // custom component

UiLocalizationManager::Get().SetBindingResource(
  card,             // target
  "CardTitle",      // binding id
  "IDS_CARD_TITLE", // resource id
  LocalizedStringCallback::New(this, &MyApp::ApplyCardLocalization));
~~~

~~~cpp
void MyApp::ApplyCardLocalization(BaseHandle target, const Dali::String& title)
{
  LocalizedCard card = LocalizedCard::DownCast(target);
  if(!card)
  {
    return;
  }

  UiLocalizationManager manager = UiLocalizationManager::Get();

  card.SetTitle(title);
  card.SetDescription(manager.GetLocalizedString("IDS_CARD_DESCRIPTION"));
  card.SetActionText(manager.GetLocalizedString("IDS_CARD_ACTION"));
  card.SetFlagImageUrl(Dali::String(RESOURCES_DIR) + manager.GetLocalizedString("IDS_FLAG_IMAGE_URL"));
}
~~~

callback에서는 component의 semantic API를 사용해 내부 view를 갱신합니다.

샘플: [text-localization-custom-component-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-localization-custom-component-example.cpp)

<br/>

## Bypass Mode

디버깅 용도로, localized string 대신 resourceId가 그대로 적용되는 mode입니다.
resource id 연결 상태를 확인할 때 사용할 수 있습니다.

~~~cpp
UiLocalizationManager::Get().SetBypassEnabled(true);
UiLocalizationManager::Get().SetBypassEnabled(false);
~~~

![Bypass mode](./assets/text/bypass.gif)

<br/>

## Localization Override

PO/MO가 아닌 별도 문자열 테이블을 사용하는 앱을 위한 기능입니다.

Override는 gettext/dgettext lookup 전에 호출됩니다.
override 함수가 `true`를 반환하면 `out` 값이 최종 localized string으로 사용되고, `false`를 반환하면 기존 gettext lookup으로 fallback됩니다.

~~~text
resourceId
  -> bypass 확인
  -> override lookup (true이면 out 사용)
  -> dgettext lookup
  -> fallback resourceId
~~~

~~~cpp
bool MyOverride(StringView resourceId, StringView domain, Dali::String& out)
{
  if(resourceId == "IDS_TITLE")
  {
    out = "Title from override";
    return true;
  }
  return false;
}

UiLocalizationManager::Get().SetLocalizedStringOverride(&MyOverride);
~~~

> [!WARNING]
> Override는 plain function pointer입니다.
> capturing lambda나 non-static member function은 사용할 수 없습니다.

샘플: [text-localization-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-localization-example.cpp)

<br/>

## 언어 변경 감지

언어 변경이 발생하면 등록된 localization binding이 다시 적용됩니다.

동작 흐름:

~~~text
언어 변경
  -> UiLocalizationManager
  -> RefreshBindings()
  -> 등록된 binding callback 재호출
  -> View text / placeholder / resource URL 갱신
~~~

> [!NOTE]
> TBD: 앱이 언어 변경을 감지하는 방법

<br/>

## Text Layout Direction

다국어 UI에서는 문자열뿐 아니라 layout direction도 함께 고려할 수 있습니다.
기본 text layout direction mode는 `UiConfig`에서 설정할 수 있습니다.

~~~cpp
UiConfig::New()
  .SetTextLayoutDirectionMode(Text::LayoutDirectionMode::INHERIT)
  .Apply();
~~~

각 text view에서도 직접 설정할 수 있습니다:

~~~cpp
Label label = Label::New()
  .SetLayoutDirectionMode(Text::LayoutDirectionMode::CONTENTS);
~~~

Mode 설명:

- `INHERIT`: parent View의 layout direction을 따름
- `CONTENTS`: text 내용으로 direction 판단
- `LOCALE`: system locale 기준

> [!NOTE]
> 일반적인 다국어 UI에서는 상위 View의 layout direction을 따르도록 `INHERIT`를 사용하는 것이 자연스럽습니다.

<br/>

## 빌드 및 패키징 참고

- `.po`는 source translation file, `.mo`는 runtime lookup file
- `msgfmt` 필요
- Tizen spec에서는 `gettext-tools` BuildRequires 필요
- runtime package에는 생성된 `.mo` 포함
- runtime에 필요 없는 `res/po`는 패키지에서 제외 가능

<br/>

## Samples

### Override sample
- [text-localization-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-localization-example.cpp)
- PO/MO 없이 `SetLocalizedStringOverride()`를 사용하는 샘플

### PO/MO sample
- [text-localization-po-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-localization-po-example.cpp)
- 실제 PO/MO 리소스, Label helper, Placeholder helper, Direct Binding, Image URL binding 등을 보여주는 샘플

### Custom component sample
- [text-localization-custom-component-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-localization-custom-component-example.cpp)
- custom component 자체에 binding을 걸고 내부 view를 갱신하는 샘플

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#documents)
