## Features

- phong shading
- scene graph
- raycasting
- shadow mapping


![screenshot](/docs/screenshot.png "Screenshot")

## Building the main app with CMake

touch build && cd build
cmake ..
cmake --build .
./native

## Building the tests with CMake

cd tests
touch build && cd build
cmake ..
cmake --build .
./tests


## setup emscripten (not currently used but keeping around in case it makes a comeback)

- First, ensure that the emscripten sdk is checkout out by running `git submodule update --init --recursive`
- Run `here=$(pwd) && cd ../emsdk && ./emsdk install 3.1.54 && ./emsdk activate 3.1.54 && source ./emsdk_env.sh && cd $here`

## Platform support

Currently, the native code is only tested with Cmake on linux. The cmake config expects to find 
SDL2 dev folders locally so you will need SDL2 dev stuff installed locally to build it.

