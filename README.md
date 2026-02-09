## Features

- phong shading
- scene graph
- raycasting
- shadow mapping


![screenshot](/docs/screenshot.png "Screenshot")

## Building the main app with CMake (for dev)

mkdir Debug && cd Debug  
cmake -DCMAKE_BUILD_TYPE=Debug ..  
cmake --build .  
./native  

## Building the main app with CMake (for release)

mkdir Release && cd Release  
cmake -DCMAKE_BUILD_TYPE=Release ..  
cmake --build .  
./native  

## Building the tests with CMake (for dev)

cd tests  
mkdir Debug && cd Debug  
cmake -DCMAKE_BUILD_TYPE=Debug ..  
cmake --build .  
./tests 

## Building the tests with CMake (for ci)

cd tests  
mkdir Release && cd Release  
cmake -DCMAKE_BUILD_TYPE=Release ..  
cmake --build .  
./tests  


## Platform support

Currently, the native code is only tested with Cmake on linux. The cmake config uses sdl3 in the
submodule and links it statically so there should be no requirement to install sdl to 
build or run the app

## copying shaders

the cmake script for copying shaders only works when
cmake see a change to a cpp or header file, so there's a copy_shaders.sh script 
as a workaround for when you've only changed the shaders

## profiling with tracy

While the main project cmake takes care of most of the tracy setup, building the profiler
is done by cd-ing into third_party/tracy and running. 

`cmake -B profiler/build -S profiler -DCMAKE_BUILD_TYPE=Release` or, since Tracy assumes you're on wayland, but you can tell it to use X11 by setting LEGACY=1
`cmake -B profiler/build -S profiler -DCMAKE_BUILD_TYPE=Release -DLEGACY=1` if you're on X11

to generate the build files, followed by 

`cmake --build profiler/build --config Release --parallel`

to build, and then, finally

`./profiler/build/tracy-profiler` to run