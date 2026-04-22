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
