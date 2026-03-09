# color-controls sample

## 개요 (KOR)

`ThemeLoaderInterface`를 구현하여 자체 색상 테마를 제공하려는 **라이브러리 개발자**를 위한 샘플입니다.

- `UiColor::PRIMARY`, `UiColor("ThemeColor1")` 등 시맨틱 컬러 이름으로 UI 색상을 지정
- `ThemeLoaderInterface`를 구현하여 bright/dark 테마별 컬러 테이블 정의
- 키 입력("1")으로 런타임에 테마를 전환하면, 적용된 모든 `UiColor`가 자동 갱신

## Overview (EN)

A sample for **library developers** who want to implement `ThemeLoaderInterface` to provide custom color themes.

- Assign UI colors with semantic names such as `UiColor::PRIMARY` and `UiColor("ThemeColor1")`
- Implement `ThemeLoaderInterface` to define per-theme color tables (bright / dark)
- Toggle themes at runtime via key input ("1"); all `UiColor` values update automatically

## How to build

### Ubuntu
:warning: Need to set up DALi env first.
```
# dali-ui root
cd samples/theme-controls
cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX
make -j
```
```
$./bin/color-controls.example
```

### GBS build (Tizen)
```
# dali-ui root
gbs build -A armv7l --include-all --packaging-dir samples/theme-controls/packaging
```
Check `com.samsung.dali.color-controls-2.0.0-1.armv7l.rpm` generated.
