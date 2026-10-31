# VisualBase sample

This sample demonstrates the use of **VisualBase**.

## Features


## Build

### Ubuntu

Requires DALi environment to be set up first.

```bash
# From dali-ui root
cd samples/visual-base
cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX
make -j4
```

Run:

```bash
./bin/color-visual.example
```

### GBS build (Tizen)

```bash
# From dali-ui root
gbs build -A armv7l --include-all --packaging-dir samples/visual-base/packaging
```

Output: `com.samsung.dali.visual-base-2.0.0-1.armv7l.rpm`

## Controls

- **Escape** or **Back**: Quit the application.
- **q** / **w** / **e** / **r** / **t** (gradient-visual, image-visual): UI scale 0.8 / 1.0 / 1.2 / 1.5 / 2.0.
- The root is as large as the window at UI scale 1.0 and grows with the UI scale (gradient-visual, image-visual),
  so above 1.0 it extends past the right and bottom window edges.
- **4** (gradient-visual): a `USER_SPACE` background ramp over the first 200 units of the view.
