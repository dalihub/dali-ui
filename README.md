![Potential Defects](https://code.sec.samsung.net/crb/image/api/badge/31457/analysis-hub-service) ![CQM](https://code.sec.samsung.net/crb/image/api/badge/31457/analysis-hub-service?show=cqm) ![SAM](https://code.sec.samsung.net/crb/image/api/badge/31457/analysis-hub-service?show=sam) ![TEM](https://code.sec.samsung.net/crb/image/api/badge/31457/test-hub-service?show=tem) ![CVG](https://code.sec.samsung.net/crb/image/api/badge/31457/test-hub-service)

# DALi UI

## Libraries & Architecture

dali-ui는 유연성과 확장성을 위해 기능별로 분리된 두 개의 핵심 라이브러리로 구성됩니다. 각 라이브러리는 하위 계층의 기능을 기반으로 빌드되는 계층적 구조를 가집니다.

| Library | Primary Responsibility | Components |
| :--- | :--- | :--- |
| dali-ui-foundation | Core Abstractions | Layout, Theme, ImageView, TextLabel, etc. |
| dali-ui-components | UI Controls | Button, Slider, etc. |


#### 1. dali-ui-foundation
프레임워크의 최하위 계층으로, UI 시스템 구축에 필요한 핵심 추상화와 유틸리티를 제공합니다. Trait 시스템의 베이스 클래스, 메모리 관리 모델, 공통 데이터 타입들 그리고 레이아웃 엔진의 기초가 여기에 포함되어 있습니다.

#### 2. dali-ui-components
dali-ui-foundation을 기반으로 구축되며, 애플리케이션 개발에 사용 가능한 UI 컨트롤(Button, Slider, ImageView, Label 등)을 제공합니다. foundation의 Trait 시스템을 활용하여 풍부한 로직을 구현합니다.

</br>

## API Reference

https://pages.github.sec.samsung.net/NUI/dali-ui/api/index_classes.html

</br>

## How to build

### Ubuntu

:warning: DALi env set up required. ([→Link](https://github.com/dalihub/dali-core/blob/master/README.md#1-building-for-ubuntu-desktop))

:warning: [dali-core](https://github.com/dalihub/dali-core/blob/master/README.md#1-building-for-ubuntu-desktop) and [dali-adpator](https://github.com/dalihub/dali-adaptor/blob/master/README.md#1-building-for-ubuntu-desktop) should be installed.
```
. setenv
```
```
cd build/tizen
cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX .
make install -j
```

### GBS build (Tizen)

To build dali-ui libraries:

```
gbs build -A armv7l --include-all
```

To build a sample, specify sample app directory:

```
gbs build -A armv7l --include-all --packaging-dir samples/hello-world/packaging
```

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
