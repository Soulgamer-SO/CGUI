CC = gcc -m64 -std=c23
PLATFORM := NONE_PLATFORM
PLATFORM_CFLAGS := -DNONE_PLATFORM
VK_PLATFORM_CFLAGS := -DVK_NONE_PLATFORM
DEBUG_CFLAGS := -DDEBUG
POSIX_CFLAGS :=
LD_LIBRARY_FLAGS :=
target_bin := cgui-app

#如果是Linux
ifeq ($(shell uname),Linux)
ifeq ($(shell uname -m),x86_64)
PLATFORM := LINUX
PLATFORM_CFLAGS := -DLINUX
VK_PLATFORM_CFLAGS := -DVK_USE_PLATFORM_XCB_KHR
DEBUG_CFLAGS := -DDEBUG
POSIX_CFLAGS := -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700
LD_LIBRARY_FLAGS += -ldl -lxcb -lxcb-icccm
endif
endif

#如果是Windows
ifeq ($(OS),Windows_NT)
PLATFORM := WINDOWS
PLATFORM_CFLAGS := -DWINDOWS
VK_PLATFORM_CFLAGS := -DVK_USE_PLATFORM_WIN32_KHR
DEBUG_CFLAGS := -DDEBUG
LD_LIBRARY_FLAGS += -lgdi32
target_bin := cgui-app.exe
endif

CFLAGS = $(PLATFORM_CFLAGS) $(VK_PLATFORM_CFLAGS) $(DEBUG_CFLAGS) $(POSIX_CFLAGS) -Wall -g -O0
target_path_debug := build/debug/
target_path_release := build/release/
target_bin_install_path := $(target_path_release)$(target_bin)
main_src_path := src/main/
functions_src_path := src/functions/
main_src := $(wildcard $(main_src_path)*.c)
main_o := $(patsubst %.c,%.o,$(main_src))
functions_src := $(wildcard $(functions_src_path)*.c)
functions_h := $(wildcard $(functions_src_path)*.h)
functions_o := $(patsubst %.c,%.o,$(functions_src))
target_o := $(main_o) $(functions_o)
$(target_o): $(functions_h)
target_src := $(main_src) $(functions_src) $(functions_h)

.PHONY:all debug release clean install

# make
all:$(target_src)
	mkdir -p build/release/
	$(CC) $(PLATFORM_CFLAGS) $(VK_PLATFORM_CFLAGS) $(POSIX_CFLAGS) $(main_src) $(functions_src) -O0 -o $(target_path_release)$(target_bin) $(LD_LIBRARY_FLAGS)

debug:$(target_o) $(target_src)
	mkdir -p build/debug/
	$(CC) $(main_o) $(functions_o) -O0 -o $(target_path_debug)$(target_bin) $(LD_LIBRARY_FLAGS)

release:$(target_src)
	mkdir -p build/release/
	$(CC) $(PLATFORM_CFLAGS) $(VK_PLATFORM_CFLAGS) $(POSIX_CFLAGS) $(main_src) $(functions_src) -O0 -o $(target_path_release)$(target_bin) $(LD_LIBRARY_FLAGS)

install:
ifneq ($(shell test -e '$(target_bin_install_path)' && echo exists),exists)
	@echo "the target bin does not exist!"
else ifeq ($(PLATFORM),LINUX)
	mkdir -p bin/
	@cp $(target_bin_install_path) bin/
else ifeq ($(PLATFORM),WINDOWS)
	mkdir -p bin/
	@cp $(target_bin_install_path) bin/
endif

clean:
	-rm $(main_o)
	-rm $(functions_o)
	-rm $(target_path_debug)$(target_bin)
	-rm $(target_path_release)$(target_bin)
	-rm compile_commands.json
