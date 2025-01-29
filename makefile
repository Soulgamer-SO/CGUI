CC = gcc -m64 -std=c23
VK_USE_PLATFORM := NONE_PLATFORM
LD_LIBRARY_FLAGS := 
target_bin := cgui-app

#如果是Linux
ifeq ($(shell uname),Linux)
  ifeq ($(shell uname -m),x86_64)
  VK_USE_PLATFORM := VK_USE_PLATFORM_XCB_KHR
  LD_LIBRARY_FLAGS += -ldl -lxcb -lxcb-icccm
  endif
endif

#如果是Windows
ifeq ($(OS),Windows_NT)
VK_USE_PLATFORM := VK_USE_PLATFORM_WIN32_KHR
LD_LIBRARY_FLAGS += -lgdi32
target_bin := cgui-app.exe
endif

#如果是Android
ifeq ($(shell uname),Linux)
  ifeq ($(shell uname -m),aarch64)
  VK_USE_PLATFORM := VK_USE_PLATFORM_ANDROID_KHR
  target_bin := cgui-app
  endif
endif

DEBUG = DEBUG
CFLAGS = -D $(VK_USE_PLATFORM) -D $(DEBUG) -Wall -g -O0
target_path_debug := build/debug/
target_path_release := build/release/
main_src_path := src/main/
functions_src_path := src/functions/
main_src := $(wildcard $(main_src_path)*.c)
main_o := $(patsubst %.c,%.o,$(main_src))
functions_src := $(wildcard $(functions_src_path)*.c)
functions_h := $(patsubst %.c,%.h,$(functions_src))
functions_o := $(patsubst %.c,%.o,$(functions_src))
target_o := $(main_o) $(functions_o)
target_src := $(main_src) $(functions_src) $(functions_h)

# make
all:$(target_src)
	mkdir -p build/release/
	$(CC) -D $(VK_USE_PLATFORM) $(main_src) $(functions_src) -O0 -o $(target_path_release)$(target_bin) $(LD_LIBRARY_FLAGS)

main_build_obj: $(main_src)
	$(CC) $(CFLAGS) $(main_src) -c -o $(main_o)

functions_build_obj: $(functions_src) $(functions_h)
	$(CC) $(CFLAGS) $(functions_src) -c -o $(functions_o)

.PHONY:debug release install clean
debug:$(target_o) $(target_src)
	mkdir -p build/debug/
	$(CC) $(main_o) $(functions_o) -O0 -o $(target_path_debug)$(target_bin) $(LD_LIBRARY_FLAGS)
release:$(target_src)
	mkdir -p build/release/
	$(CC) -D $(VK_USE_PLATFORM) $(main_src) $(functions_src) -O0 -o $(target_path_release)$(target_bin) $(LD_LIBRARY_FLAGS)
install:
	bin/$(target_bin)
clean:
	-rm $(main_o)
	-rm $(functions_o)
	-rm $(target_path_debug)$(target_bin)
	-rm $(target_path_release)$(target_bin)
