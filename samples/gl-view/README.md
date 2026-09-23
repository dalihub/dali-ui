# GlView sample

Draws a spinning cube with the application's own GLES code, inside a `GlView` that is
deliberately inset and off-centre so that placement and GL state bugs are visible.
See the header comment of `gl-view-example.cpp` for the key bindings and what each of
them is meant to show.

Unlike the other samples this one calls GLES directly, so it links `glesv2` itself and
is skipped where pkg-config cannot find it.

## How to build

### Ubuntu
:warning: Need to set up DALi env first.
```
# dali-ui root
cd samples/gl-view
cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX
make -j
```
```
$./bin/gl-view.example
```

### GBS build (Tizen)
```
# dali-ui root
gbs build -A armv7l --include-all --packaging-dir samples/gl-view/packaging
```
Check `com.samsung.dali.gl-view-2.0.0-1.armv7l.rpm` generated.
