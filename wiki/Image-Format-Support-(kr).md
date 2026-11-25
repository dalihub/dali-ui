# DALi UI Foundation - 이미지 포맷 지원

[→ English Documentation](https://github.sec.samsung.net/NUI/dali-ui/wiki/Image-Format-Support)

포맷별로 지원하는 세부 형식과 권장 View를 안내합니다. 일반 이미지는 `ImageView`, 애니메이션 이미지와 프레임 목록은 `AnimatedImageView`, Lottie는 `LottieAnimationView`를 사용합니다.

## 정적 이미지

| 포맷 | 권장 View | 지원 형식 |
|---|---|---|
| PNG (`.png`) | `ImageView` | Grayscale, Grayscale+Alpha, RGB, RGBA, Palette |
| JPEG (`.jpg`, `.jpeg`) | `ImageView` | Baseline, Progressive / Grayscale, RGB, YCbCr, CMYK, YCCK |
| BMP (`.bmp`) | `ImageView` | 비압축 1/4/8/16/24/32bpp, RLE4, RLE8, 일부 16/32bpp BITFIELDS |
| GIF (`.gif`) | `ImageView` | 팔레트, interlace, 애니메이션의 첫 프레임 표시 |
| WebP (`.webp`) | `ImageView` | Lossy, Lossless, Alpha, 애니메이션의 첫 프레임 표시 |
| ICO (`.ico`) | `ImageView` | DIB 기반 1/4/8/24/32bpp, Alpha, AND mask |
| WBMP (`.wbmp`) | `ImageView` | Type 0 흑백 이미지 |
| SVG (`.svg`) | `ImageView` | 정적 SVG |

- PNG의 16bit 채널은 8bit로, JPEG의 CMYK/YCCK는 RGB로 변환합니다.
- 비압축 32bpp BMP의 알파와 정적 GIF의 투명도는 반영하지 않습니다. PNG 내장 ICO는 지원하지 않습니다.

## 애니메이션 및 프레임 목록

| 포맷 / 입력 | 권장 View | 지원 형식 |
|---|---|---|
| Animated GIF (`.gif`) | `AnimatedImageView` | 프레임 애니메이션, 투명도 |
| Animated WebP (`.webp`) | `AnimatedImageView` | 프레임 애니메이션, Alpha |
| 비트맵 이미지 URL 목록 | `AnimatedImageView` | PNG·JPEG 등 개별 이미지의 프레임 시퀀스 |
| Lottie JSON (`.json`) | `LottieAnimationView` | Lottie 벡터 애니메이션 |
| Animated SVG | — | SVG 내부 애니메이션 재생 미지원 |
| APNG | — | 애니메이션 재생 미지원. PNG 기본 이미지의 정적 표시는 가능 |

프레임 목록은 `SetResourceUrlList()`로 설정합니다. SVG·Lottie URL 목록은 지원하지 않습니다.

## 압축 텍스처

| 포맷 | 권장 View | 지원 형식 |
|---|---|---|
| KTX 1 (`.ktx`) | `ImageView` | ETC1, ETC2/EAC, PVRTC1 RGB 4bpp, ASTC, Alpha8 |
| ASTC (`.astc`) | `ImageView` | 지정된 14종 블록 크기의 2D linear RGBA ASTC |
| PKM (`.pkm`) | `ImageView` | PKM 1/2의 지정된 ETC1, ETC2, EAC 형식 |

해당 압축 픽셀 포맷을 GPU와 그래픽 백엔드도 지원해야 합니다. KTX는 단일 2D 이미지를 지원하며, KTX 2, 다중 mipmap, 다중 배열 레이어, cubemap 및 3D 텍스처는 지원하지 않습니다.

## 지원 조건

- WebP는 WebP 디코더가 포함된 빌드가 필요하며, Animated WebP는 애니메이션 디코더도 필요합니다.
- GIF/WebP 애니메이션 URL은 `.gif`/`.GIF`/`.webp`/`.WEBP`로 끝나야 합니다. `.Gif` 같은 혼합 대소문자 확장자나 확장자 뒤에 query string이 붙는 URL은 자동 애니메이션 판별을 지원하지 않습니다.
- SVG와 Lottie의 세부 기능 지원은 사용하는 렌더러와 버전에 따라 달라집니다.
- 이 표는 이미지 View의 기본 내장 로더 기준입니다. `CanvasView`와 `WebView`의 지원 범위는 사용하는 엔진을 따릅니다.

_2026-11-16 개발 소스 기준._

## 관련 문서

- [ImageView](https://github.sec.samsung.net/NUI/dali-ui/wiki/ImageView-(kr))
- [AnimatedImageView API](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1AnimatedImageView.html)
- [LottieAnimationView API](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1LottieAnimationView.html)
