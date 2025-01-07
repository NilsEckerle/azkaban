OBJECTS = build/azkaban.o

# This is needed on my mac becaus dah.
LDFLAGS_MAC = -Wl,-rpath,@loader_path/../third/azkaban-core/lib
CFLAGS_MAC = -Ithird/azkaban-core/third/sqlcipher/include
# comment this to build on mac
LDFLAGS_MAC = 
CFLAGS_MAC =

CFLAGS = -Iinclude \
		 -Ithird/azkaban-core/include \
		 -Ithird/sdl2/include \
		 -Ithird/sdl2_image/include \
		 -Ithird/sdl2_ttf/include\
		 $(CFLAGS_MAC)


LDFLAGS = -Lthird/azkaban-core/lib -lazkaban-core \
		  -Lthird/sdl2/lib -lSDL2 \
		  -Lthird/sdl2_image/lib -lSDL2_image \
		  -Lthird/sdl2_ttf/lib -lSDL2_ttf\
          -Wl,-rpath,\$$ORIGIN/../third/azkaban-core/lib\
		  $(LDFLAGS_MAC)

final: setup $(OBJECTS)
	gcc $(OBJECTS) $(LDFLAGS) -o bin/azkaban

clean:
	@if [ -d build ]; then \
		rm -r build; \
	fi
	@if [ -d bin ]; then \
		rm -r bin; \
	fi

setup: clean
	@mkdir build
	@mkdir bin

build/azkaban.o: src/azkaban.c
	gcc $(CFLAGS) -c src/azkaban.c -o build/azkaban.o
