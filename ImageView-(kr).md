# DALi UI Foundation - ImageView

[→ English Documentation](https://github.sec.samsung.net/NUI/dali-ui/wiki/ImageView)

`ImageView`는 이미지 리소스를 표시하는 뷰입니다. 다양한 이미지 포맷을 지원하며, 이미지 맞춤(Fitting), 샘플링, 마스킹, 로딩 정책, N-patch 테두리 등을 제어할 수 있습니다.

---

## 목차

1. [기본 설정](#1-기본-설정)
2. [Fitting Mode (이미지 맞춤 모드)](#2-fitting-mode-이미지-맞춤-모드)
3. [Sampling Mode (샘플링 모드)](#3-sampling-mode-샘플링-모드)
4. [주요 프로퍼티와 메서드](#4-주요-프로퍼티와-메서드)
   - [이미지 리소스](#41-이미지-리소스)
   - [플레이스홀더 이미지](#42-플레이스홀더-이미지)
   - [이미지 색상](#43-이미지-색상)
   - [Pixel Area (부분 영역)](#44-pixel-area-부분-영역)
   - [Desired Size](#45-desired-size)
   - [Image Load With View Size](#46-image-load-with-view-size)
5. [고급 렌더링 및 마스킹](#5-고급-렌더링-및-마스킹)
   - [알파 마스크](#51-알파-마스크)
   - [마스킹 모드](#52-마스킹-모드)
   - [Pre-multiplied Alpha](#53-pre-multiplied-alpha)
6. [로딩 동작](#6-로딩-동작)
   - [동기 로딩](#61-동기-로딩)
   - [Release Policy](#62-release-policy)
   - [Fast-track Uploading](#63-fast-track-uploading)
   - [방향 보정](#64-방향-보정)
7. [N-Patch 테두리](#7-n-patch-테두리)
8. [로딩 상태 및 시그널](#8-로딩-상태-및-시그널)
9. [메서드 체이닝](#9-메서드-체이닝)
10. [기본값](#10-기본값)
11. [주의사항](#11-주의사항)

---

## 1. 기본 설정

정적 팩토리 메서드 `New()`를 사용하여 `ImageView`를 생성합니다. 이미지는 기본적으로 비동기로 로딩됩니다.

```cpp
#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali::Ui;

// 빈 ImageView 생성
ImageView imageView = ImageView::New();

// 이미지 URL과 함께 ImageView 생성
ImageView imageView = ImageView::New("image.png");

// 크기 설정
imageView.SetRequestedWidth(200.0f);
imageView.SetRequestedHeight(200.0f);

// 씬에 추가
window.Add(imageView);
```

---

## 2. Fitting Mode (이미지 맞춤 모드)

`FittingMode`는 이미지가 뷰 영역에 맞춰지는 방식을 제어합니다.

```cpp
// 가로세로 비율을 유지하며 맞춤 (기본값)
imageView.SetFittingMode(Image::FittingMode::FIT_KEEP_ASPECT_RATIO);

// 뷰 영역을 꽉 채우도록 늘림 (왜곡 발생 가능)
imageView.SetFittingMode(Image::FittingMode::FILL);

// 가로세로 비율을 유지하며 채우고, 넘치는 부분 잘림
imageView.SetFittingMode(Image::FittingMode::OVER_FIT_KEEP_ASPECT_RATIO);

// 원본 이미지 크기 유지, 중앙 정렬
imageView.SetFittingMode(Image::FittingMode::CENTER);
```

| 값 | 동작 |
|---|---|
| `FIT_KEEP_ASPECT_RATIO` | 가로세로 비율을 유지하며 뷰 영역에 맞춤. 기본값. |
| `FILL` | 뷰 영역을 꽉 채우도록 늘림. 왜곡 발생 가능. |
| `OVER_FIT_KEEP_ASPECT_RATIO` | 가로세로 비율을 유지하며 채우고, 넘치는 부분은 잘림. |
| `CENTER` | 원본 크기 그대로 표시, 뷰 중앙에 위치. |
| `FIT_HEIGHT` | 높이 기준 비율 맞춤 (deprecated). |
| `FIT_WIDTH` | 너비 기준 비율 맞춤 (deprecated). |
| `DONT_CARE` | 특정 맞춤 모드 없음. |

### 시각적 예시

가정: **이미지 600x400**을 **View 800x800**에 표시 (View가 이미지보다 큼)

**원본 이미지 (600x400):**

![원본 이미지](./assets/ImageView/fm_original.svg)

| 모드 | 시각적 결과 |
|:---:|:---|
| **FIT_KEEP_ASPECT_RATIO** | ![FIT_KEEP_ASPECT_RATIO](./assets/ImageView/fm_fit.svg) |
| **FILL** | ![FILL](./assets/ImageView/fm_fill.svg) |
| **OVER_FIT_KEEP_ASPECT_RATIO** | ![OVER_FIT_KEEP_ASPECT_RATIO](./assets/ImageView/fm_cover.svg) |
| **CENTER** | ![CENTER](./assets/ImageView/fm_center.svg) |

**간단 비교:**

| 상황 | 추천 모드 |
|:---|:---|
| 사진, 아바타 (왜곡 없음) | `FIT_KEEP_ASPECT_RATIO` |
| 배경 이미지 (영역 꽉 채움) | `FILL` 또는 `OVER_FIT_KEEP_ASPECT_RATIO` |
| 아이콘, 썸네일 (원본 크기) | `CENTER` |

---

## 3. Sampling Mode (샘플링 모드)

`SamplingMode`는 이미지 스케일링 시 적용되는 필터를 제어합니다. `dali-ui-foundation/public-api/image/image-enumerations.h`에서 `Dali::SamplingMode`의 alias로 정의되어 있습니다.

```cpp
#include <dali-ui-foundation/public-api/image/image-enumerations.h>

using namespace Dali::Ui;

// 최근접 이웃 (픽셀화, 빠름)
imageView.SetSamplingMode(Image::SamplingMode::NEAREST);

// 선형 필터링 (부드러움, 기본값)
imageView.SetSamplingMode(Image::SamplingMode::LINEAR);

// 박스 필터링 (축소 시 고품질)
imageView.SetSamplingMode(Image::SamplingMode::BOX);

// 필터링 없음
imageView.SetSamplingMode(Image::SamplingMode::DONT_CARE);
```

| 값 | 용도 |
|---|---|
| `NEAREST` | 픽셀 아트, 날카로운 경계, 빠른 렌더링 |
| `LINEAR` | 사진, 부드러운 스케일링 (기본값) |
| `BOX` | 고품질 축소 |
| `DONT_CARE` | 시스템이 결정 |

---

## 4. 주요 프로퍼티와 메서드

### 4.1 이미지 리소스

뷰에 표시할 이미지를 설정하거나 변경합니다.

```cpp
// 이미지 URL 설정
imageView.SetResourceUrl("images/photo.jpg");

// 현재 URL 확인
Dali::String url = imageView.GetResourceUrl();

// 동일한 URL에서 이미지 다시 로드
imageView.Reload();
```

지원하는 URL 형식:
- 로컬 파일 경로: `"images/photo.jpg"`
- HTTP/HTTPS: `"https://example.com/image.png"`

---

### 4.2 플레이스홀더 이미지

메인 이미지가 로딩되는 동안 표시할 플레이스홀더 이미지를 설정합니다.

```cpp
imageView.SetPlaceholderUrl("images/placeholder.png");

Dali::String placeholderUrl = imageView.GetPlaceholderUrl();
```

---

### 4.3 이미지 색상

이미지에 색상 승수를 적용합니다. 직접 RGBA 값 또는 테마 색상 토큰을 사용할 수 있습니다.

```cpp
// 16진수 색상 사용
imageView.SetImageColor(UiColor(0xFF0000FF));  // 빨간색 틴트

// RGBA 값 사용
imageView.SetImageColor(UiColor(1.0f, 0.5f, 0.0f, 1.0f));  // 주황색 틴트

// 현재 색상 확인
UiColor color = imageView.GetImageColor();
```

---

### 4.4 Pixel Area (부분 영역)

이미지의 일부 영역만 표시합니다. 좌표는 정규화된 값(0.0 ~ 1.0)을 사용합니다.

```cpp
// 이미지의 왼쪽 상단 1/4만 표시
imageView.SetPixelArea(Vector4(0.0f, 0.0f, 0.5f, 0.5f));

// 중앙 영역 표시
imageView.SetPixelArea(Vector4(0.25f, 0.25f, 0.5f, 0.5f));

// 전체 이미지 표시로 리셋
imageView.SetPixelArea(Vector4(0.0f, 0.0f, 1.0f, 1.0f));

Vector4 area = imageView.GetPixelArea();
```

`Vector4`는 정규화된 좌표로 `(x, y, width, height)`를 나타냅니다.

---

### 4.5 Desired Size

이미지 로더에 원하는 크기를 힌트로 제공합니다.

```cpp
imageView.SetDesiredSize(ImageDimensions(800, 600));

ImageDimensions size = imageView.GetDesiredSize();
```

메모리 절약을 위해 저해상도 버전의 이미지를 로드할 때 사용할 수 있습니다.

---

### 4.6 Image Load With View Size

현재 뷰 크기로 이미지를 로드하여 메모리와 디코딩 시간을 절약합니다.

```cpp
imageView.SetImageLoadWithViewSize(true);

bool enabled = imageView.GetImageLoadWithViewSize();
```

활성화 시:
- 뷰의 결정된 레이아웃 크기로 이미지가 (재)로드됨
- 작은 화면에 표시할 때 전체 해상도 이미지를 로드하지 않아도 됨
- 방향: 뷰 크기 → 이미지 로드 크기

---

## 5. 고급 렌더링 및 마스킹

### 5.1 알파 마스크

메인 이미지에 알파 마스크 이미지를 적용합니다.

```cpp
imageView.SetAlphaMaskUrl("images/mask.png");

// 이미지를 마스크 영역에 맞춰 자르기
imageView.SetCropToMask(true);

Dali::String maskUrl = imageView.GetAlphaMaskUrl();
bool cropToMask = imageView.GetCropToMask();
```

---

### 5.2 마스킹 모드

마스킹이 적용되는 시점을 제어합니다.

```cpp
// 렌더링 단계에서 마스킹 적용 (기본값)
imageView.SetMaskingMode(Image::MaskingType::MASKING_ON_RENDERING);

// 이미지 로딩 단계에서 마스킹 적용
imageView.SetMaskingMode(Image::MaskingType::MASKING_ON_LOADING);

Image::MaskingType mode = imageView.GetMaskingMode();
```

| 값 | 동작 |
|---|---|
| `MASKING_ON_RENDERING` | 렌더링 단계에서 마스킹 적용. 기본값. |
| `MASKING_ON_LOADING` | 로딩 단계에서 마스킹 적용 (더 효율적). |

---

### 5.3 Pre-multiplied Alpha

이미지가 미리 곱해진 알파(pre-multiplied alpha)를 사용하는지 제어합니다.

```cpp
imageView.SetPreMultipliedAlpha(true);

bool preMultiplied = imageView.GetPreMultipliedAlpha();
```

---

## 6. 로딩 동작

### 6.1 동기 로딩

메인 스레드에서 동기적으로 이미지를 로드합니다 (로딩 완료까지 블록됨).

```cpp
imageView.SetSynchronousLoading(true);

bool sync = imageView.GetSynchronousLoading();
```

> **참고:** 동기 로딩은 UI 버벅임을 유발할 수 있습니다. 작은 이미지나 불가피한 경우에만 사용하세요.

---

### 6.2 Release Policy

이미지 텍스처가 메모리에서 해제되는 시점을 제어합니다.

```cpp
// 비주얼이 씬에서 분리될 때 해제 (기본값)
imageView.SetReleasePolicy(Image::ReleasePolicy::DETACHED);

// 비주얼이 소멸될 때 해제
imageView.SetReleasePolicy(Image::ReleasePolicy::DESTROYED);

// 캐시에서 해제하지 않음
imageView.SetReleasePolicy(Image::ReleasePolicy::NEVER);

Image::ReleasePolicy policy = imageView.GetReleasePolicy();
```

| 값 | 동작 |
|---|---|
| `DETACHED` | 비주얼이 씬에서 분리될 때 해제. 기본값. |
| `DESTROYED` | 비주얼이 소멸될 때 해제. |
| `NEVER` | 캐시에 계속 유지. 주의해서 사용. |

---

### 6.3 Fast-track Uploading

메인 스레드 지연을 줄이기 위해 백그라운드 스레드에서 GPU에 이미지를 업로드합니다.

```cpp
imageView.SetFastTrackUploading(true);

bool enabled = imageView.GetFastTrackUploading();
```

---

### 6.4 방향 보정

EXIF 방향 메타데이터를 자동으로 적용합니다.

```cpp
imageView.SetOrientationCorrection(true);

bool enabled = imageView.GetOrientationCorrection();
```

---

## 7. N-Patch 테두리

N-patch (9-patch) 이미지는 정의된 테두리 영역만 늘어나고, 모서리와 가장자리는 그대로 유지됩니다.

```cpp
// 테두리 inset 설정 (왼쪽, 위, 오른쪽, 아래)
imageView.SetNPatchBorder(Vector4(10.0f, 10.0f, 10.0f, 10.0f));

// 테두리만 렌더링, 중앙은 렌더링하지 않음
imageView.SetNPatchBorderOnly(true);

Vector4 border = imageView.GetNPatchBorder();
bool borderOnly = imageView.GetNPatchBorderOnly();
```

### N-Patch 시각적 예시

**9-Patch 영역 구조:**

![NPatch Grid](./assets/ImageView/np_grid.svg)

**비교: 일반 스케일링 vs N-Patch:**

| 원본 이미지 | 일반 이미지 스케일링 (왜곡됨) | N-Patch 적용 |
|:---:|:---:|:---:|
| ![Original](./assets/ImageView/np_original.svg) | ![Stretched](./assets/ImageView/np_stretched.svg) | ![NPatch Applied](./assets/ImageView/np_applied.svg) |

**핵심 포인트:**
- 🔳 **모서리**는 원래 크기 그대로 유지
- ↔️ **위/아래 가장자리**는 가로로만 늘어남
- ↕️ **왼쪽/오른쪽 가장자리**는 세로로만 늘어남
- 🟧 **중앙**은 가로세로 모두 늘어남

---

## 8. 로딩 상태 및 시그널

### 로딩 상태 확인

```cpp
Visual::ResourceStatus status = imageView.GetLoadingStatus();

if(status == Visual::ResourceStatus::READY)
{
    // 이미지가 로드되어 준비됨
}
else if(status == Visual::ResourceStatus::LOADING)
{
    // 이미지 로딩 중
}
else if(status == Visual::ResourceStatus::FAILED)
{
    // 이미지 로드 실패
}
```

### Resource Ready 시그널

이미지 로딩이 완료되면 알림을 받습니다.

```cpp
class MyImageHandler : public Dali::ConnectionTracker
{
public:
    void OnImageReady(ImageView imageView)
    {
        // 이미지 로딩 완료, 표시 준비됨
        DALI_LOG_RELEASE_INFO("이미지 로드 완료!\n");
    }
};

MyImageHandler handler;
imageView.ResourceReadySignal().Connect(&handler, &MyImageHandler::OnImageReady);
```

---

## 9. 메서드 체이닝

모든 setter는 `ImageView&`를 반환하여 유창한 설정이 가능합니다:

```cpp
ImageView imageView = ImageView::New("photo.jpg");

imageView
    .SetFittingMode(Image::FittingMode::FIT_KEEP_ASPECT_RATIO)
    .SetSamplingMode(Image::SamplingMode::LINEAR)
    .SetImageColor(UiColor(0xFFFFFFFF))
    .SetSynchronousLoading(false)
    .SetFastTrackUploading(true)
    .SetOrientationCorrection(true)
    .SetReleasePolicy(Image::ReleasePolicy::DETACHED);
```

---

## 10. 기본값

| 프로퍼티 | 기본값 |
|---|---|
| `FittingMode` | `FIT_KEEP_ASPECT_RATIO` |
| `SamplingMode` | `LINEAR` |
| `ImageColor` | 흰색 `(1.0, 1.0, 1.0, 1.0)` |
| `PixelArea` | `(0.0, 0.0, 1.0, 1.0)` (전체 이미지) |
| `ImageLoadWithViewSize` | `false` |
| `SynchronousLoading` | `false` |
| `FastTrackUploading` | `false` |
| `OrientationCorrection` | `true` |
| `ReleasePolicy` | `DETACHED` |
| `PreMultipliedAlpha` | `false` |
| `CropToMask` | `false` |
| `MaskingMode` | `MASKING_ON_RENDERING` |
| `NPatchBorder` | `(0.0, 0.0, 0.0, 0.0)` |
| `NPatchBorderOnly` | `false` |

---

## 11. 주의사항

- **기본적으로 비동기 로딩.** 로딩 완료 시점을 알려면 `ResourceReadySignal()`을 사용하세요.

- **메모리 관리.** 텍스처 해제 시점을 제어하려면 `ReleasePolicy`를 사용하세요. `NEVER`는 메모리 누수를 유발할 수 있으므로 주의해서 사용해야 합니다.

- **N-patch 이미지.** 0이 아닌 `SetNPatchBorder()`를 설정하면 자동으로 N-patch 렌더링이 활성화됩니다.

- **Pixel Area는 정규화된 값.** `SetPixelArea()`는 픽셀 값이 아닌 [0, 1] 범위의 좌표를 사용합니다.

- **URL 변경 시 자동 리로드.** 새 URL로 `SetResourceUrl()`을 호출하면 자동으로 로딩이 트리거됩니다. 동일한 URL을 다시 로드하려면 `Reload()`를 사용하세요.

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki/Home-(kr)#development-guides)