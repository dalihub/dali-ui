# Text sample

This sample demonstrates DALi UI text features.

## Features

- Basic Label text rendering
- Layout direction
- Markup text
- Marquee
- Text style, bevel, and cutout mask
- Text fit and fit candidate
- Font variation
- Emoji rendering
- Render scale
- InputField and InputEditor
- Typing style
- Localization with override callback
- Localization with PO/MO resources
- Custom component localization

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
| `text-emoji.example` | Emoji rendering sample |
| `text-render-scale.example` | Text render scale sample |
| `text-input-field.example` | InputField sample |
| `text-input-editor.example` | InputEditor sample |
| `text-typing-style.example` | Typing style sample |
| `text-localization.example` | Localization sample using override callback |
| `text-localization-po.example` | Localization sample using PO/MO resources |
| `text-localization-custom-component.example` | Custom component localization sample |

## Localization resources

The PO source files are stored by locale under `res/po`.

~~~text
res/po/default/en_US.po
res/po/default/ko_KR.po
res/po/default/ar_AE.po

res/po/alternate/en_US.po
res/po/alternate/ko_KR.po
res/po/alternate/ar_AE.po
~~~

During build, CMake uses `msgfmt` to generate MO files into the gettext runtime layout.

~~~text
res/locale/default/en_US/LC_MESSAGES/text-localization-po.mo
res/locale/default/ko_KR/LC_MESSAGES/text-localization-po.mo
res/locale/default/ar_AE/LC_MESSAGES/text-localization-po.mo

res/locale/alternate/en_US/LC_MESSAGES/text-localization-po-alt.mo
res/locale/alternate/ko_KR/LC_MESSAGES/text-localization-po-alt.mo
res/locale/alternate/ar_AE/LC_MESSAGES/text-localization-po-alt.mo
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
~~~

Run localization samples:

~~~bash
./bin/text-localization.example
./bin/text-localization-po.example
./bin/text-localization-custom-component.example
~~~

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
