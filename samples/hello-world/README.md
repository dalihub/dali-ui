## How to build

### Ubuntu
:warning: Need to set up DALi env first.
```
# dali-ui root
cd samples/hello-world
cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX
make -j
```
```
$./bin/hello-world.example
```

### GBS build (Tizen)
```
# dali-ui root
gbs build -A armv7l --include-all --packaging-dir samples/hello-world/packaging
```
Check `com.samsung.dali.hello-world-2.0.0-1.armv7l.rpm` generated.