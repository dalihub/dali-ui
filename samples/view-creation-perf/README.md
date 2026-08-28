# DALi UI View Creation Performance

Keys:

- `1` / `2`: create 100 / 10000 plain Views
- `3` / `4`: create 100 / 10000 Views with individual Renderers
- `5` / `6`: create 100 / 10000 Views with background colors
- `9`: hide or show the timing labels
- `Escape` / `Back`: quit

The measured interval covers View creation, geometry setup, optional visual or
renderer setup, and adding each View to the scene-connected root. Bulk-created
children use the default layout mode so they do not become independent layout
roots. The timer does not wait for a layout pass or GPU rendering to complete.

## Ubuntu

```sh
cd samples/view-creation-perf
cmake --fresh -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX .
make -j8
./bin/view-creation-perf.example
```

## Tizen GBS

```sh
gbs build -A armv7l --include-all --packaging-dir samples/view-creation-perf/packaging
```
