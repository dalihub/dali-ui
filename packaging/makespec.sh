#!/bin/bash

SCRIPT_FILE=$(readlink -f $0)
SCRIPT_DIR=$(dirname $SCRIPT_FILE)

VERSION_FILE=$SCRIPT_DIR/version.txt
RPMSPEC=$SCRIPT_DIR/dali-ui.spec
RPMSPEC_IN=$RPMSPEC.in
BUILD_ALL=true

source $VERSION_FILE

while getopts ":r:a" opt; do
  case $opt in
    r) RPM_VERSION=$OPTARG ;;
    a) BUILD_ALL=true ;;
    :) echo "Option -$OPTARG requires an argument."; exit 1 ;;
  esac
done

if [ -z "$RPM_VERSION" ]; then
  echo "-r option for rpm version is required."
  echo "eq. makespec.sh -r 0.0.1"
  exit 1
fi

# Update RPM Spec
echo "# Auto-generated from $(basename $RPMSPEC_IN) by makespec.sh" | cat - $RPMSPEC_IN > $RPMSPEC
sed -i -e "s/@rpm_version@/$RPM_VERSION/g" $RPMSPEC
sed -i -e "s/@build_all@/$BUILD_ALL/g" $RPMSPEC
