# AbsoluteLayout sample

This sample demonstrates the use of **AbsoluteLayout** to position child views at explicit coordinates.

## Features

- **Absolute positioning**: Children placed at pixel coordinates with explicit sizes.
- **Proportional positioning**: Position and/or size specified as 0.0-1.0 proportion of the parent.
- **Overlapping**: Children can freely overlap; later children are drawn on top.
- **Margin and padding**: Padding on the layout container, margin on individual children.

## Build

### Ubuntu

Requires DALi environment to be set up first.

```bash
# From dali-ui root
cd samples/absolutelayout
cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX
make -j
```

Run:

```bash
./bin/absolutelayout.example
```

**Proportional positioning sample** (AbsoluteLayoutFlags를 사용한 비례 위치/크기 지정):

```bash
./bin/absolutelayout-proportional.example
```

- PositionProportional: 부모 영역 대비 비례 위치 (0.0~1.0).
- SizeProportional: 부모 영역 대비 비례 크기.
- All: 위치와 크기 모두 비례.

**Overlapping children sample** (자식 뷰 겹침 및 Z-order 시연):

```bash
./bin/absolutelayout-overlap.example
```

- 자식 뷰가 자유롭게 겹칠 수 있으며, 나중에 추가된 자식이 위에 그려짐.
- 비례 위치와 절대 위치 혼합 사용.

**Margin and padding sample** (Padding, Margin, 중첩 AbsoluteLayout 시연):

```bash
./bin/absolutelayout-margin-padding.example
```

- SetViewPadding: 레이아웃 컨테이너 안쪽 여백 (자식이 가장자리로부터 밀림).
- SetViewMargin: 자식 뷰 바깥 간격 (없음, 균등, 비대칭 비교).
- 중첩 AbsoluteLayout에 별도 padding 적용.

### GBS build (Tizen)

```bash
# From dali-ui root
gbs build -A armv7l --include-all --packaging-dir samples/absolutelayout/packaging
```

Output: `com.samsung.dali.absolutelayout-2.0.0-1.armv7l.rpm`

## Controls

- **Escape** or **Back**: Quit the application.
