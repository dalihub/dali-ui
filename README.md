![Potential Defects](https://code.sec.samsung.net/crb/image/api/badge/31457/analysis-hub-service) ![CQM](https://code.sec.samsung.net/crb/image/api/badge/31457/analysis-hub-service?show=cqm) ![SAM](https://code.sec.samsung.net/crb/image/api/badge/31457/analysis-hub-service?show=sam) ![TEM](https://code.sec.samsung.net/crb/image/api/badge/31457/test-hub-service?show=tem) ![CVG](https://code.sec.samsung.net/crb/image/api/badge/31457/test-hub-service)

# DALi UI

## Libraries & Architecture

dali-ui는 유연성과 확장성을 위해 기능별로 분리된 세 개의 핵심 라이브러리로 구성됩니다. 각 라이브러리는 하위 계층의 기능을 기반으로 빌드되는 계층적 구조를 가집니다.

| Library | Primary Responsibility | Components |
| :--- | :--- | :--- |
| dali-ui-foundation | Core Abstractions | Layout, Traits, Theme, etc. |
| dali-ui-elements | Atomic UI Blocks | ImageView, TextLabel, etc. |
| dali-ui-components | High-level UI Controls | Button, Slider, etc. |


#### 1. dali-ui-foundation
프레임워크의 최하위 계층으로, UI 시스템 구축에 필요한 핵심 추상화와 유틸리티를 제공합니다. Trait 시스템의 베이스 클래스, 메모리 관리 모델, 공통 데이터 타입들 그리고 레이아웃 엔진의 기초가 여기에 포함되어 있습니다.

#### 2. dali-ui-elements
dali-ui-foundation을 기반으로 구축되며, 프레임워크의 기본 빌딩 블록들을 포함합니다. 더 복잡한 컴포넌트로 조합되기 전의 원자적인(Atomic) UI 요소들 중 앱 개발에 필수적인 Text 와 Image, Scroll 등의 항목들을 제공합니다.

#### 3. dali-ui-components
실제 애플리케이션 개발에 즉시 사용 가능한 고수준 UI 컨트롤(Button, List, Slider 등)을 제공합니다. elements를 조합하고 foundation의 Trait 시스템을 활용하여 풍부한 로직을 구현합니다.

</br>

## How to build

### Ubuntu

:warning: DALi env set up required. [→Link](https://github.com/dalihub/dali-core/blob/master/README.md)

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