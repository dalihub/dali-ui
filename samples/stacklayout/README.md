# StackLayout sample

This sample demonstrates the use of **StackLayout** to arrange child views in a single column (vertical stack).

## Features

- **Root**: A vertical `StackLayout` that fills the window, with padding and spacing between children.
- **Top bar**: Fixed-height red view (80 px).
- **Middle**: Green view with layout weight 1, taking the remaining space.
- **Bottom bar**: Fixed-height blue view (60 px).

## Build

### Ubuntu

Requires DALi environment to be set up first.

```bash
# From dali-ui root
cd samples/stacklayout
cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX
make -j
```

Run:

```bash
./bin/stacklayout.example
```

**Cross-axis alignment sample** (StackLayout 자식의 cross-axis alignment 테스트):

```bash
./bin/stacklayout-alignment.example
```

- 세로 스택에서 좁은 박스 4개: Start(왼쪽), Center(가운데), End(오른쪽), Fill(전체 너비).

**Margin, padding, nested StackLayout sample** (사용성: margin / padding / StackLayout in StackLayout):

```bash
./bin/stacklayout-margin-padding-nested.example
```

- **Padding**: 루트 스택에 `SetViewPadding`으로 창 가장자리와의 여백.
- **Margin**: 자식 View에 `SetViewMargin`으로 서로 다른 여백(좌우/하단) 적용.
- **Nested**: 세로 스택 안에 가로 `StackLayout` 한 행(여러 박스)을 자식으로 추가.

### GBS build (Tizen)

```bash
# From dali-ui root
gbs build -A armv7l --include-all --packaging-dir samples/stacklayout/packaging
```

Output: `com.samsung.dali.stacklayout-2.0.0-1.armv7l.rpm`

## Controls

- **Escape** or **Back**: Quit the application.
