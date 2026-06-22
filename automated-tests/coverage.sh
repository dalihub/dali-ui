#!/bin/bash

set -euo pipefail

opt_genhtml=true
if [ "${1:-}" == "-n" ] ; then
  opt_genhtml=false
fi

BUILD_DIR_NAME=tizen

BUILDSYSTEM=make

if [ -e ../build/tizen/build.ninja ] ; then
    BUILDSYSTEM=ninja
fi

if [ -z "${DESKTOP_PREFIX:-}" ] ; then
    echo "ERROR: DESKTOP_PREFIX is not set. Source the DALi setenv script before running coverage." >&2
    exit 1
fi

FASTCOV_BIN="${DESKTOP_PREFIX}/bin/fastcov.py"
CMAKE_CACHE="../build/$BUILD_DIR_NAME/CMakeCache.txt"
TOP_LEVEL_INFO="../build/$BUILD_DIR_NAME/dali.info"

if [ ! -x "$FASTCOV_BIN" ] ; then
    echo "ERROR: fastcov.py not found or not executable: $FASTCOV_BIN" >&2
    exit 1
fi

if [ ! -f "$CMAKE_CACHE" ] ; then
    echo "ERROR: CMake cache not found: $CMAKE_CACHE" >&2
    exit 1
fi

CXX_COMPILER=$(sed -n 's/^CMAKE_CXX_COMPILER:FILEPATH=//p' "$CMAKE_CACHE" | head -1)
CXX_COMPILER=${CXX_COMPILER:-${CXX:-c++}}
CXX_VERSION=$("$CXX_COMPILER" -dumpfullversion -dumpversion 2>/dev/null || true)
CXX_MAJOR=${CXX_VERSION%%.*}

if [ -n "$CXX_MAJOR" ] && command -v "gcov-$CXX_MAJOR" >/dev/null 2>&1 ; then
    GCOV_BIN=$(command -v "gcov-$CXX_MAJOR")
elif command -v gcov >/dev/null 2>&1 ; then
    GCOV_BIN=$(command -v gcov)
else
    echo "ERROR: gcov not found. Install the gcov package matching $CXX_COMPILER." >&2
    exit 1
fi

echo "Using gcov for coverage data collection: $GCOV_BIN"

(  cd ../build/$BUILD_DIR_NAME ; $BUILDSYSTEM cov_data )

if [ ! -s "$TOP_LEVEL_INFO" ] ; then
    echo "ERROR: coverage data was not generated: $TOP_LEVEL_INFO" >&2
    echo "Check that dali-ui was configured with coverage flags and that gcov matches the compiler version." >&2
    exit 1
fi

# fastcov.py doesn't require version detection or complex rc options
FASTCOV_OPTS=(--branch-coverage --lcov --gcov "$GCOV_BIN")
echo "Using fastcov.py for coverage data collection"

for i in `find . -name "*.dir"` ; do
    (
        cd $i
        echo `pwd`
        shopt -s nullglob
        covs=( *.gcda )
        shopt -u nullglob
        if [[ ${#covs[@]} -gt 0 ]]
        then
            "$FASTCOV_BIN" "${FASTCOV_OPTS[@]}" \
                -d . \
                --exclude /usr/include automated-tests dali-env \
                -o dali.info
            if [ ! -s dali.info ]
            then
              rm -f dali.info
            fi
        fi
    )
done

(
    if [ $opt_genhtml == true ] ; then
        cd .. ;
        genhtml --branch-coverage -o build/$BUILD_DIR_NAME/doc/coverage `find . -name dali.info`
        echo "Coverage output: ../build/$BUILD_DIR_NAME/doc/coverage/index.html"
    fi
)
