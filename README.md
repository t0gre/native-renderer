## Makefile

Although this part of the project uses a makefile, it doesn't use any actual features of make. It could be just a series of shell scripts, one for each command. See below for setting up emscripten before `make build-en` will work.

## setup emscripten (not currently used but keeping around in case it makes a comeback)

- First, ensure that the emscripten sdk is checkout out by running `git submodule update --init --recursive`
- Run `here=$(pwd) && cd ../emsdk && ./emsdk install 3.1.54 && ./emsdk activate 3.1.54 && source ./emsdk_env.sh && cd $here`

## Compiler

Currently, the native code is only tested with clang on linux. The `make build` command sets the build up for dynamic linking to SDL2, not static, so you will need SDL2 installed locally for it to run.


### to generate debug symbols for the browser
Add `-O0 -g -gsource-map` to the `make build-en` command
