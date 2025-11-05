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


## Platform support

Currently, the native code is only tested with Cmake on linux. The cmake config uses sdl3 in the
submodule and links it statically so there should be no requirement to install sdl to 
build or run the app

