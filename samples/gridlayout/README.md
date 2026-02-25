# GridLayout sample

This sample demonstrates the use of **GridLayout** to arrange child views in rows and columns.

## Features

- **Fixed grid**: Rows and columns with absolute pixel sizes.
- **Star sizing**: Proportional distribution of remaining space using `GridLength::Star()`.
- **Row/column span**: Children that span multiple rows or columns.

## Build

### Ubuntu

Requires DALi environment to be set up first.

```bash
# From dali-ui root
cd samples/gridlayout
cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX
make -j
```

Run:

```bash
./bin/gridlayout.example
```

**Star and auto sizing sample** (Star 비례 크기 및 고정/유연 행·열 혼합):

```bash
./bin/gridlayout-star-auto.example
```

- Star(2) 열은 Star(1) 열보다 2배 넓은 공간 차지.
- 고정 헤더/푸터 + 유연한 중간 영역 레이아웃.
- ColumnSpan으로 헤더/푸터가 전체 열을 차지.

**Row and column span sample** (RowSpan/ColumnSpan으로 셀 병합):

```bash
./bin/gridlayout-span.example
```

- 3x3 그리드에서 셀이 여러 행/열에 걸쳐 확장.
- ColumnSpan(2): 한 셀이 2열을 차지.
- RowSpan(2) + ColumnSpan(2): 한 셀이 2행×2열 영역을 차지.

### GBS build (Tizen)

```bash
# From dali-ui root
gbs build -A armv7l --include-all --packaging-dir samples/gridlayout/packaging
```

Output: `com.samsung.dali.gridlayout-2.0.0-1.armv7l.rpm`

## Controls

- **Escape** or **Back**: Quit the application.
