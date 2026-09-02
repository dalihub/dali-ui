# DALi UI Perf Like CSFS

This sample reproduces the original PerfTestLikeCSFS scene using DALi UI
View, ImageView, and Label objects.

## Ubuntu

```sh
cd samples/perf-like-csfs
cmake --fresh -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX .
make -j8
./bin/perf-like-csfs.example
```

## Tizen GBS

```sh
gbs build -A armv7l --include-all --packaging-dir samples/perf-like-csfs/packaging
```
