![Potential Defects](https://code.sec.samsung.net/crb/image/api/badge/31457/analysis-hub-service) ![CQM](https://code.sec.samsung.net/crb/image/api/badge/31457/analysis-hub-service?show=cqm) ![SAM](https://code.sec.samsung.net/crb/image/api/badge/31457/analysis-hub-service?show=sam) ![TEM](https://code.sec.samsung.net/crb/image/api/badge/31457/test-hub-service?show=tem) ![CVG](https://code.sec.samsung.net/crb/image/api/badge/31457/test-hub-service)

# DALi UI
(TBD)

## How to build

### Ubuntu
:warning: DALi env set up required. ([→Link]())
:warning: [dali-core](https://github.com/dalihub/dali-core/blob/master/README.md#1-building-for-ubuntu-desktop), [dali-adpator](https://github.com/dalihub/dali-adaptor/blob/master/README.md#1-building-for-ubuntu-desktop) and [dali-toolkit](https://github.com/dalihub/dali-toolkit/blob/master/README.md#1-building-for-ubuntu-desktop) should be installed.
```
# . setenv
```
```
cd build/tizen
cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX
make install -j
```

### GBS build (Tizen)
```
gbs build -A armv7l --include-all
```
Check `com.samsung.dali.hello-world-2.0.0-1.armv7l.rpm` generated.

## How to run automated test

Build dali-ui with test options.

```
cd build/tizen
export CC=gcc
export CXX=g++
git clean -fxd . # Only do this in the build folder
CXXFLAGS='-g -O0 --coverage' LDFLAGS='--coverage' cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX -DCMAKE_BUILD_TYPE=Debug
make -j install
```
Then build and run automated test:
```
cd automated-tests
git clean -fxd . # Only do this in the automated-tests folder
./build.sh
./execute.sh
```

Check coverage with the test result:
```
cd automated-tests
./coverage.sh
```