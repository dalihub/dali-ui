# ScrollView samples

These samples demonstrate the use of **ScrollView** with pan gesture support and interactive child items.

## Samples

### `scrollview.example`

Basic ScrollView with vertical scrolling. Shows how to place a `StackLayout` content view (larger than the viewport) inside a `ScrollView`, with configurable fling physics.

### `scrollview-interactive-children`

Demonstrates that `ScrollView` scrolls correctly even when each child item has an `InteractiveTrait` (click/press handling). Touch disambiguation between scroll gesture and item tap is handled automatically.

Layout:
```
Window (480 x 800)
├── Status bar  View  (0,   0)  480 x 60
└── ScrollView        (0,  60)  480 x 740
    └── Content StackLayout     480 x (ITEM_COUNT * (HEIGHT+SPACING) + SPACING)
        ├── Item 0  (tap to highlight)
        ├── Item 1  (tap to highlight)
        └── …
```

## Build

### Ubuntu

Requires the DALi environment to be set up first.

```bash
# From the dali-ui root
cd samples/scrollview
cmake -B build -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX
cmake --build build -j
```

Binaries are placed in `bin/`:

```bash
./bin/scrollview.example
./bin/scrollview-interactive-children
```

### GBS build (Tizen)

```bash
# From dali-ui root
gbs build -A armv7l --include-all --packaging-dir samples/scrollview/packaging
```

Output: `com.samsung.dali.scrollview-2.0.0-1.armv7l.rpm`

## Controls

- **Scroll**: Pan vertically inside the ScrollView area.
- **Tap item**: Changes the item's background color and updates the status bar label.
- **Escape** or **Back**: Quit the application.
