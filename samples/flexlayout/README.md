# FlexLayout sample

This sample demonstrates the use of **FlexLayout** (CSS Flexbox model) to arrange child views.

## Features

- **Flex direction**: Row-based layout with flex-grow for distributing space.
- **Justify and align**: SpaceBetween, Center alignment, and per-item AlignSelf override.
- **Wrap and grow**: Wrapping items to new lines with different flex-grow values.
- **Margin and padding**: Padding on the flex container, margin on individual children.

## Build

### Ubuntu

Requires DALi environment to be set up first.

```bash
# From dali-ui root
cd samples/flexlayout
cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX
make -j
```

Run:

```bash
./bin/flexlayout.example
```

**Justify content and align items sample** (JustifyContent, AlignItems, AlignSelf 시연):

```bash
./bin/flexlayout-justify-align.example
```

- SpaceBetween: 아이템 사이 균등 간격.
- AlignItems::Center: 교차축 중앙 정렬.
- AlignSelf::FlexEnd: 개별 아이템의 교차축 정렬 오버라이드.

**Wrap and flex-grow sample** (Wrap 줄바꿈 및 flex-grow 비율 분배):

```bash
./bin/flexlayout-wrap-grow.example
```

- FlexWrap::Wrap: 공간 부족 시 다음 줄로 넘김.
- FlexBasis: 각 아이템의 기본 크기 지정.
- FlexGrow: 남은 공간을 비율에 따라 분배 (grow 2는 grow 1의 2배).

**Margin and padding sample** (Padding, Margin, 중첩 FlexLayout 시연):

```bash
./bin/flexlayout-margin-padding.example
```

- SetViewPadding: 플렉스 컨테이너 안쪽 여백 (자식이 가장자리로부터 밀림).
- SetViewMargin: 자식 뷰 바깥 간격 (없음, 균등 15px, 비대칭 비교).
- 중첩 FlexLayout(Row)에 별도 padding 적용, 내부 자식에 개별 margin.

### GBS build (Tizen)

```bash
# From dali-ui root
gbs build -A armv7l --include-all --packaging-dir samples/flexlayout/packaging
```

Output: `com.samsung.dali.flexlayout-2.0.0-1.armv7l.rpm`

## Controls

- **Escape** or **Back**: Quit the application.
