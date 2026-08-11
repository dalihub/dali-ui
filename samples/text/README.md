# Text sample

This sample demonstrates DALi UI text features.

## Features

- Basic Label text rendering
- Layout direction
- Markup text
- Marquee
- Text style, bevel, and cutout mask
- TextGradient visual playground
- Text fit and fit candidate
- Font variation
- Emoji rendering
- Render scale
- InputField and InputEditor
- Typing style
- Localization with override callback
- Localization with PO/MO resources
- Application-side formatting of localized PO/MO strings
- Custom component localization
- Localized TextGradient markup ranges

## Examples

| Executable | Description |
| --- | --- |
| `text.example` | Basic text sample |
| `text-layout-direction.example` | Text layout direction sample |
| `text-markup.example` | Markup text sample |
| `text-marquee.example` | Marquee text sample |
| `text-style.example` | Text style sample |
| `text-style-bevel.example` | Text bevel style sample |
| `text-fit.example` | Text fit sample |
| `text-fit-candidate.example` | Text fit candidate sample |
| `text-scale.example` | Text scale sample |
| `text-font-variation.example` | Font variation sample |
| `text-cutout-mask.example` | Text cutout mask sample |
| `text-gradient.example` | Label TextGradient visual playground |
| `text-gradient-simple.example` | Minimal Label TextGradient animation sample |
| `text-emoji.example` | Emoji rendering sample |
| `text-render-scale.example` | Text render scale sample |
| `text-input-field.example` | InputField sample |
| `text-clipboard.example` | Clipboard public API sample |
| `text-input-editor.example` | InputEditor sample |
| `text-typing-style.example` | Typing style sample |
| `text-localization.example` | Localization sample using override callback |
| `text-localization-po.example` | Localization sample using PO/MO resources |
| `text-formatted-localization.example` | Positional printf formatting and gettext plural lookup using localized PO/MO strings |
| `text-localization-custom-component.example` | Custom component localization sample |
| `text-gradient-localization.example` | Localized markup ranges with TextGradient |

## Localization resources

The PO source files are stored by locale under `res/po`.

~~~text
res/po/default/en_US.po
res/po/default/ko_KR.po
res/po/default/ar_AE.po

res/po/alternate/en_US.po
res/po/alternate/ko_KR.po
res/po/alternate/ar_AE.po

res/po/formatted/en_US.po
res/po/formatted/ko_KR.po
res/po/formatted/pl_PL.po

res/po/gradient/en_US.po
res/po/gradient/ko_KR.po
res/po/gradient/ar_AE.po
~~~

During build, CMake uses `msgfmt` to generate MO files into the gettext runtime layout.

~~~text
res/locale/default/en_US/LC_MESSAGES/text-localization-po.mo
res/locale/default/ko_KR/LC_MESSAGES/text-localization-po.mo
res/locale/default/ar_AE/LC_MESSAGES/text-localization-po.mo

res/locale/alternate/en_US/LC_MESSAGES/text-localization-po-alt.mo
res/locale/alternate/ko_KR/LC_MESSAGES/text-localization-po-alt.mo
res/locale/alternate/ar_AE/LC_MESSAGES/text-localization-po-alt.mo

res/locale/formatted/en_US/LC_MESSAGES/text-formatted-localization.mo
res/locale/formatted/ko_KR/LC_MESSAGES/text-formatted-localization.mo
res/locale/formatted/pl_PL/LC_MESSAGES/text-formatted-localization.mo

res/locale/gradient/en_US/LC_MESSAGES/text-gradient-localization-po.mo
res/locale/gradient/ko_KR/LC_MESSAGES/text-gradient-localization-po.mo
res/locale/gradient/ar_AE/LC_MESSAGES/text-gradient-localization-po.mo
~~~

The generated `.mo` files are build artifacts and are not intended to be tracked in git.

## Build

### Ubuntu

Requires DALi environment to be set up first.

~~~bash
# From dali-ui root
cd samples/text
cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX
make -j4
~~~

Run:

~~~bash
./bin/text.example
./bin/text-gradient.example
./bin/text-gradient-simple.example
~~~

Run localization samples:

~~~bash
./bin/text-localization.example
./bin/text-localization-po.example
./bin/text-formatted-localization.example
./bin/text-localization-custom-component.example
./bin/text-gradient-localization.example
~~~

### Windows / MSVC

Use an x64 Visual Studio Developer PowerShell after installing the DALi
dependencies. The separate tools vcpkg supplies `msgfmt.exe`; the DALi vcpkg
supplies the runtime `libintl.dll`.

~~~powershell
$Msgfmt = "C:\Tools\DALI_VCPKG_TOOLS\vcpkg\installed\x64-windows\tools\gettext\bin\msgfmt.exe"

cmake -S .\samples -B C:\work\DALi\out\dali-ui-samples `
  -DDALI_UI_SAMPLE_LIST=text `
  "-DMSGFMT_EXECUTABLE=$Msgfmt"

cmake --build C:\work\DALi\out\dali-ui-samples --target `
  text-localization-po.example `
  text-formatted-localization.example `
  text-localization-custom-component.example `
  text-gradient-localization.example

$env:PATH = "C:\work\DALi\dali-env\bin;C:\Tools\DALI_VCPKG\vcpkg\installed\x64-windows\bin;$env:PATH"
.\samples\text\bin\text-localization-po.example.exe
.\samples\text\bin\text-formatted-localization.example.exe
~~~

For host-side manual testing, the Windows samples update gettext's `LANGUAGE`
environment variable. The formatted-localization sample uses `1`, `2`, and `3`
for `en_US`, `ko_KR`, and `pl_PL`; samples that include `ar_AE` use `3`. This
key-driven locale setup is not the locale-management pattern for Tizen devices.

### GBS build (Tizen)

~~~bash
# From dali-ui root
gbs build -A armv7l --include-all --packaging-dir samples/text/packaging
~~~

Output:

~~~text
com.samsung.dali.text-2.0.0-1.armv7l.rpm
~~~

## Controls

Common controls:

- **Escape** or **Back**: Quit the application.

Localization samples also provide additional key controls such as locale switching, domain switching, bypass mode, refresh, and manual text update. See each sample source file for details.
