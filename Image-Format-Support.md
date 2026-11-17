# DALi UI Foundation - Image Format Support

[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Image-Format-Support-(kr))

The tables list supported variants and the recommended View for each format. Use `ImageView` for still images, `AnimatedImageView` for animated images and frame sequences, and `LottieAnimationView` for Lottie animations.

## Still Images

| Format | Recommended View | Supported variants |
|---|---|---|
| PNG (`.png`) | `ImageView` | Grayscale, Grayscale+Alpha, RGB, RGBA, Palette |
| JPEG (`.jpg`, `.jpeg`) | `ImageView` | Baseline, Progressive / Grayscale, RGB, YCbCr, CMYK, YCCK |
| BMP (`.bmp`) | `ImageView` | Uncompressed 1/4/8/16/24/32bpp, RLE4, RLE8, some 16/32bpp BITFIELDS |
| GIF (`.gif`) | `ImageView` | Palettes, interlace, first animation frame |
| WebP (`.webp`) | `ImageView` | Lossy, Lossless, Alpha, first animation frame |
| ICO (`.ico`) | `ImageView` | DIB-based 1/4/8/24/32bpp, Alpha, AND mask |
| WBMP (`.wbmp`) | `ImageView` | Type 0 monochrome images |
| SVG (`.svg`) | `ImageView` | Static SVG |

- PNG 16-bit channels are reduced to 8-bit; JPEG CMYK/YCCK is converted to RGB.
- Uncompressed 32bpp BMP alpha and static GIF transparency are not applied. PNG-embedded ICO is unsupported.

## Animations and Frame Sequences

| Format / input | Recommended View | Supported variants |
|---|---|---|
| Animated GIF (`.gif`) | `AnimatedImageView` | Frame animation, transparency |
| Animated WebP (`.webp`) | `AnimatedImageView` | Frame animation, Alpha |
| Bitmap image URL list | `AnimatedImageView` | Frame sequences of separate PNG, JPEG, and other bitmap images |
| Lottie JSON (`.json`) | `LottieAnimationView` | Lottie vector animation |
| Animated SVG | — | SVG-internal animation playback is unsupported |
| APNG | — | Animation playback is unsupported; the PNG default image can be displayed as a still |

Set frame sequences with `SetResourceUrlList()`. SVG/Lottie URL lists are unsupported.

## Compressed Textures

| Format | Recommended View | Supported variants |
|---|---|---|
| KTX 1 (`.ktx`) | `ImageView` | ETC1, ETC2/EAC, PVRTC1 RGB 4bpp, ASTC, Alpha8 |
| ASTC (`.astc`) | `ImageView` | 2D linear RGBA ASTC with 14 supported block sizes |
| PKM (`.pkm`) | `ImageView` | Specified PKM 1/2 ETC1, ETC2, and EAC formats |

The GPU and graphics backend must also support the compressed pixel format. KTX supports a single 2D image; KTX 2, multiple mipmap levels, multiple array layers, cubemaps, and 3D textures are unsupported.

## Availability

- WebP requires a build with WebP decoding; Animated WebP additionally requires animation decoding.
- GIF/WebP animation URLs must end in `.gif`/`.GIF`/`.webp`/`.WEBP`. Automatic animation detection does not support mixed-case extensions such as `.Gif` or a query string appended after the extension.
- Supported SVG and Lottie features depend on the renderer and its version.
- These tables cover the image Views' built-in loaders. `CanvasView` and `WebView` follow their respective engines' capabilities.

_Based on development sources as of 2026-11-16._

## Related Documentation

- [ImageView](https://github.sec.samsung.net/NUI/dali-ui/wiki/ImageView)
- [AnimatedImageView API](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1AnimatedImageView.html)
- [LottieAnimationView API](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1LottieAnimationView.html)
