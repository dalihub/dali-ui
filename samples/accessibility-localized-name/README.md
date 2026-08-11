# Accessibility Localized Name

`accessibility-localized-name.example` is a Screen Reader sample for comparing
two ways to build a localized accessibility name:

1. Resolve `IDS_WIFI`, `IDS_CONNECT`, and `IDS_COMPLETE` separately and compose
   them in three different orders with `SetAccessibilityRequestNameCallback()`.
2. Bind the complete sentence `IDS_WIFI_CONNECT_COMPLETE` to the last button with
   `SetTranslatableAccessibilityName()`.

The sample uses real gettext PO/MO resources for `en_US`, `ko_KR`, and `ar_AE`.
It does not use a localization override.

Buttons `5` and `6` select the English or Korean gettext locale inside this
sample process. They do not change the device's system language. The same
actions are available from the `5` and `6` keyboard/remote keys.

## Expected names

| Locale | 1: Wi-Fi + connect + complete | 2: Complete + connect + Wi-Fi | 3: Connect + Wi-Fi + complete | 4: Complete sentence |
| --- | --- | --- | --- | --- |
| `en_US` | `Wi-Fi connection complete` | `complete connection Wi-Fi` | `connection Wi-Fi complete` | `Wi-Fi connection is complete` |
| `ko_KR` | `와이파이 연결 완료` | `완료 연결 와이파이` | `연결 와이파이 완료` | `와이파이 연결이 완료되었습니다` |
| `ar_AE` | `واي فاي اتصال مكتمل` | `مكتمل اتصال واي فاي` | `اتصال واي فاي مكتمل` | `اكتمل اتصال واي فاي` |

The fragment result may sound acceptable in some languages, but the complete
sentence gives translators control over word order, particles, inflection, and
politeness.

## Target verification

1. Build and install the sample package.
2. Enable the platform Screen Reader.
3. Launch **Accessibility localized name**.
4. Select English with button/key `5`, or Korean with button/key `6`.
5. Swipe between the four test buttons, or press `1` through `4` on a
   keyboard/remote.
6. Compare the announced names. The Screen Reader may append the localized
   role, such as "button".
7. Change the system language while the application is running.
8. Return to the application and press `R` to refresh localization. Move key
   focus away and back to hear the refreshed value.

The complete-sentence button is refreshed by `UiLocalizationManager` when the
platform locale-changed signal arrives. The three fragment buttons resolve all
three IDs on every accessibility name request, so the next announcement uses
the current locale as well. `R` also calls `RefreshBindings()` explicitly, which
is useful on test images where locale notification delivery is being diagnosed.
The sample never calls `GrabAccessibilityHighlight()`; the platform follows key
focus with accessibility highlight automatically.

The in-app locale buttons set the process message locale and call
`RefreshBindings()` immediately. If the requested POSIX locale is unavailable
on the target, the sample also uses `LANGUAGE` to select the requested gettext
catalog.

## Runtime name diagnostics

The sample writes release logs with `DALI_LOG_RELEASE_INFO()` to distinguish
localization from the value exposed to the Screen Reader:

- `[LOCALIZATION DOMAIN]`: registered gettext domain and locale path.
- `[LOCALIZATION LOOKUP]`: active locale, `LANGUAGE`, and all four resource
  lookup results.
- `[A11Y NAME REQUEST]`: exact Name returned by the accessibility-name callback
  when the accessibility framework requests it.
- `[LOCALE SWITCH]`: result of selecting English or Korean inside the sample.
- `[KEY FOCUS]`: result of moving key focus to a test button.

The Screen Reader may append a localized role such as "button" after receiving
this name. That final role composition and TTS processing occur outside the
sample application.

If the status shows resource IDs instead of translations, verify that `msgfmt`
was found during the build and that the generated `.mo` files were installed
under:

```text
res/locale/<locale>/LC_MESSAGES/accessibility-localized-name.mo
```

## Desktop build

From the `dali-ui` root:

```sh
cmake -S samples -B build/samples-a11y-name \
  -DDALI_UI_SAMPLE_LIST=accessibility-localized-name \
  -DCMAKE_INSTALL_PREFIX="$DESKTOP_PREFIX"
cmake --build build/samples-a11y-name \
  --target accessibility-localized-name.example -j8
DISPLAY=:0 samples/accessibility-localized-name/bin/accessibility-localized-name.example
```

## Tizen GBS package

```sh
gbs build -A armv7l --include-all \
  --packaging-dir samples/accessibility-localized-name/packaging
```

The package requires `gettext-tools` at build time so CMake can generate the
runtime message catalogs.
