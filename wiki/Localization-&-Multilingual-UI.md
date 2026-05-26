[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Localization-&-Multilingual-UI-(kr))

## UiLocalizationManager

`UiLocalizationManager` is the manager that looks up localized strings and refreshes bindings registered on `View`/`BaseHandle` in dali-ui.

Basic model:

~~~text
resourceId + domain -> localized string
~~~

Localized strings are primarily UI strings, but by applying Direct Binding, they can also be used for locale-dependent strings such as resource paths and image URLs.

Sample: [text-localization-po-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-localization-po-example.cpp)

<br/>

## Basic Usage Flow

1. Write `.po` files
2. Convert `.po` files to `.mo` files during the build process
3. Install/package the generated `.mo` files according to the gettext runtime layout
4. Call `UiLocalizationManager::RegisterDomain()`
5. Set the default domain with `SetDefaultDomain()`
6. Use `Label::SetTranslatableText()`, `InputField::SetTranslatablePlaceholder()`, `InputEditor::SetTranslatablePlaceholder()`, or `SetBindingResource()`

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
> The `.po` file name does not determine the domain. The `.mo` file name placed in the runtime gettext layout must match the gettext domain.

<br/>

## PO/MO Resource Structure

A `.po` file is a source translation file, and a `.mo` file is a gettext binary catalog used for runtime lookup.

For example, source PO files can be organized by locale like this:

~~~text
res/po/default/ko_KR.po
res/po/default/en_US.po
res/po/default/ar_AE.po
~~~

During the build process, each `.po` file is converted into a `.mo` file that follows the gettext runtime layout.

~~~text
<localePath>/<locale>/LC_MESSAGES/<domain>.mo
~~~

Example:

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

In this case, the domain is `text-localization-po`, and the `localePath` is `res/locale/default`.

~~~cpp
UiLocalizationManager::Get().RegisterDomain(
  "text-localization-po",
  RESOURCES_DIR "locale/default");
~~~

`msgfmt` example:

~~~bash
msgfmt -o res/locale/default/ko_KR/LC_MESSAGES/text-localization-po.mo res/po/default/ko_KR.po
~~~

See `samples/text/CMakeLists.txt` for the sample CMake.

<br/>

## PO File Example

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

PO files are not used directly for runtime lookup; gettext uses compiled `.mo` files.

<br/>

## Domain

### RegisterDomain

~~~cpp
UiLocalizationManager manager = UiLocalizationManager::Get();
manager.RegisterDomain("myapp", RESOURCES_DIR "locale");
~~~

- `domain`: gettext domain name (must match the `.mo` file name)
- `localePath`: parent directory of the `<locale>/LC_MESSAGES/<domain>.mo` structure

`RegisterDomain()` internally calls gettext's `bindtextdomain()` and sets the codeset to UTF-8.

### Default Domain

~~~cpp
manager.SetDefaultDomain("myapp");

Label title = Label::New()
  .SetTranslatableText("IDS_TITLE");
~~~

The default domain is the reference domain used by lookups and bindings that omit the domain argument.
For example, `GetLocalizedString("IDS_TITLE")` or `SetTranslatableText("IDS_TITLE")` uses the default domain when no domain is provided.
When the default domain changes, bindings registered without a domain are refreshed.

### Explicit Domain

~~~cpp
Label title = Label::New()
  .SetTranslatableText("IDS_TITLE", "myapp-settings");
~~~

Explicit domains are not affected by default domain changes.

> [!NOTE]
> Even when multiple packages or gadgets run in the same process, each can operate independently by registering its own domain and locale path and using explicit domains.

<br/>

## Label Translatable Text

~~~cpp
Label& SetTranslatableText(StringView resourceId);
Label& SetTranslatableText(StringView resourceId, StringView domain);
Dali::String GetTranslatableText() const;
void ClearTranslatableText();
~~~

PO example:

~~~po
msgid "IDS_TITLE"
msgstr "Title"
~~~

Using default domain:

~~~cpp
Label title = Label::New()
  .SetTranslatableText("IDS_TITLE");
~~~

Using explicit domain:

~~~cpp
Label title = Label::New()
  .SetTranslatableText("IDS_TITLE", "myapp-settings");
~~~

> [!WARNING]
> `SetText()` does not remove the translatable text binding.
> If you need to keep the text set by `SetText()`, call `ClearTranslatableText()` first.

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

`GetLocalizedString()` allows you to look up localized strings directly without binding.

~~~cpp
UiLocalizationManager manager = UiLocalizationManager::Get();

Dali::String title = manager.GetLocalizedString("IDS_TITLE");
Dali::String settingsTitle = manager.GetLocalizedString("IDS_TITLE", "myapp-settings");
~~~

> [!NOTE]
> If no translation is found, the resourceId is returned by default.

<br/>

## Direct Binding

`SetBindingResource()` resolves a localized string and passes it to a callback.
The callback determines how to apply the value to the target.

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

Omitting the `domain` argument uses the default domain; passing an explicit domain uses that domain.

Using default domain:

~~~cpp
Label label = Label::New();

UiLocalizationManager::Get().SetBindingResource(
  label,       // target
  "Text",      // binding id
  "IDS_TITLE", // resource id
  LocalizedStringCallback::New(this, &MyApp::ApplyLabelText));
~~~

Using explicit domain:

~~~cpp
UiLocalizationManager::Get().SetBindingResource(
  label,            // target
  "Text",           // binding id
  "IDS_TITLE",      // resource id
  "myapp-settings", // domain
  LocalizedStringCallback::New(this, &MyApp::ApplyLabelText));
~~~

This binding uses the `myapp-settings` domain and is not affected by default domain changes.

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
> A binding is identified by the `target + bindingId` combination.
> Registering the same bindingId on the same target replaces the existing binding.

> [!NOTE]
> `ClearBinding()` only removes the binding; it does not change the currently applied text or property value.

<br/>

## ImageView URL Binding Application

Direct Binding can also be applied to locale-dependent strings other than text.

PO example:

~~~po
msgid "IDS_FLAG_IMAGE_URL"
msgstr "flag_kr.png"
~~~

C++ example:

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

Using a custom component itself as a binding target allows you to apply localization without exposing internal Label or InputField instances or adding separate `TranslatableXXX` properties.

The example below shows a localized card updating its internal text and image as the locale changes.

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

The callback uses the component's semantic API to update internal views.

Sample: [text-localization-custom-component-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-localization-custom-component-example.cpp)

<br/>

## Bypass Mode

For debugging purposes, bypass mode applies the resourceId as-is instead of the localized string.
It can be used to verify resource ID connections.

~~~cpp
UiLocalizationManager::Get().SetBypassEnabled(true);
UiLocalizationManager::Get().SetBypassEnabled(false);
~~~

![Bypass mode](./assets/text/bypass.gif)

<br/>

## Localization Override

A feature for apps that use a separate string table instead of PO/MO.

The override is called before gettext/dgettext lookup.
If the override function returns `true`, the `out` value is used as the final localized string; if it returns `false`, the lookup falls back to the existing gettext lookup.

~~~text
resourceId
  -> bypass check
  -> override (if true, use out)
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
> Override is a plain function pointer.
> Capturing lambdas and non-static member functions cannot be used.

Sample: [text-localization-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-localization-example.cpp)

<br/>

## Language Change Detection

When a language change occurs, registered localization bindings are re-applied.

Flow:

~~~text
Language change
  -> UiLocalizationManager
  -> RefreshBindings()
  -> Registered binding callbacks re-invoked
  -> View text / placeholder / resource URL updated
~~~

> [!NOTE]
> TBD: How apps detect language changes

<br/>

## Text Layout Direction

In multilingual UI, layout direction should also be considered along with strings.
The default text layout direction mode can be set via `UiConfig`.

~~~cpp
UiConfig::New()
  .SetTextLayoutDirectionMode(Text::LayoutDirectionMode::INHERIT)
  .Apply();
~~~

It can also be set directly on individual text views:

~~~cpp
Label label = Label::New()
  .SetLayoutDirectionMode(Text::LayoutDirectionMode::CONTENTS);
~~~

Mode descriptions:

- `INHERIT`: Follows the parent View's layout direction
- `CONTENTS`: Determines direction from text content
- `LOCALE`: Based on system locale

> [!NOTE]
> For typical multilingual UI, using `INHERIT` to follow the parent View's layout direction is the most natural choice.

<br/>

## Build and Packaging Notes

- `.po` is the source translation file, `.mo` is the runtime lookup file
- `msgfmt` is required
- Tizen spec files require `gettext-tools` in `BuildRequires`
- Runtime packages must include the generated `.mo` files
- `res/po` not needed at runtime can be excluded from packages

<br/>

## Samples

### Override sample
- [text-localization-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-localization-example.cpp)
- Sample using `SetLocalizedStringOverride()` without PO/MO

### PO/MO sample
- [text-localization-po-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-localization-po-example.cpp)
- Sample demonstrating real PO/MO resources, Label helper, Placeholder helper, Direct Binding, and Image URL binding

### Custom component sample
- [text-localization-custom-component-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-localization-custom-component-example.cpp)
- Sample that registers bindings on a custom component and updates internal views

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#documents)
