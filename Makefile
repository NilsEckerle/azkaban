OBJECTS = build/azkaban.o

CFLAGS = -Iinclude \
		 -Ithird/azkaban-core/include \
		 -Ithird/sdl2/include \
		 -Ithird/sdl2_image/include \
		 -Ithird/sdl2_ttf/include
LDFLAGS = -Lthird/azkaban-core/lib -lazkaban-core \
		  -Lthird/sdl2/lib -lSDL2 \
		  -Lthird/sdl2_image/lib -lSDL2_image \
		  -Lthird/sdl2_ttf/lib -lSDL2_ttf\
          -Wl,-rpath,\$$ORIGIN/../third/azkaban-core/lib


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
