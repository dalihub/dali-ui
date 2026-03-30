echo "############# cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX -DCMAKE_BUILD_TYPE=Debug . ##############"
CXXFLAGS='-g -O0 --coverage' LDFLAGS='--coverage' cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX -DCMAKE_BUILD_TYPE=Debug .
#CXXFLAGS='-g -O0 --coverage' LDFLAGS='--coverage' cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX -DCMAKE_BUILD_TYPE=Release .

echo "############# make install -j8 ##############"
make install -j8

