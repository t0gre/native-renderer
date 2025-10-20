build:
	clang++ -o native -ggdb -std=c++17 index.cpp lib/*.cpp -Ilib/include \
	-I/usr/include/SDL2 \
	-D_REENTRANT \
	-lSDL2 \
	-lGL \
	-lm \
	-Wall \
	-Wno-missing-braces 
	# -Werror 

# build-en:
# 	emcc -std=c++17 index.cpp lib/*.cpp -Ilib/include \
# 	-s MAX_WEBGL_VERSION=2 \
# 	-s USE_SDL=2 \
# 	-s FULL_ES3=1 \
# 	-s WASM=1 \
# 	--preload-file ../public/rainbowtree.obj@rainbowtree.obj \
# 	--preload-file positions.txt \
# 	--preload-file normals.txt \
# 	--preload-file ./lib/shaders/basic.vert@basic.vert \
# 	--preload-file ./lib/shaders/basic.frag@basic.frag \
# 	--preload-file ./lib/shaders/depth-only.vert@depth-only.vert \
# 	--preload-file ./lib/shaders/depth-only.frag@depth-only.frag

# clean:
# 	rm a.out.data a.out.js a.out.wasm
# 	rm a.out.wasm.map

test:
	clang++ -o test-run -ggdb -std=c++17 tests/tests.cpp \
	lib/mat4.cpp \
 	lib/raycast.cpp \
 	lib/vec.cpp \
	lib/scene.cpp \
	lib/camera.cpp \
	-Ilib/include \
	-lm \
	&& ./test-run