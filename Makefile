TARGET_NAME := dstream

BUILD_DIR := build
SRC_DIRS := src

SRCS := $(shell find $(SRC_DIRS) -name '*.c')
SRCS += $(shell find $(SRC_DIRS) -name '*.cpp')
SRCS += imgui/imgui.cpp
SRCS += imgui/imgui_demo.cpp
SRCS += imgui/imgui_draw.cpp
SRCS += imgui/imgui_tables.cpp
SRCS += imgui/imgui_widgets.cpp
SRCS += imgui/backends/imgui_impl_sdl.cpp
SRCS += imgui/backends/imgui_impl_opengl3.cpp
SRCS += implot/implot.cpp
SRCS += implot/implot_items.cpp
SRCS += cjson/cJSON.c

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

LIB_SRCS := $(shell find $(SRC_DIRS) -name 'dstream_*.c')
LIB_OBJS := $(LIB_SRCS:%=$(BUILD_DIR)/%.o)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_DIRS += include imgui imgui/backends /usr/include/SDL2 implot cjson

INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CFLAGS := -Wall -Wextra -Wpedantic -Werror -Wno-overlength-strings $(INC_FLAGS) -MMD -MP -fPIC
LDFLAGS := -lSDL2 -lGL

CXXFLAGS = $(CFLAGS)

TARGET := $(BUILD_DIR)/$(TARGET_NAME)

TEST_DIR := test

all: CFLAGS += -O3 -DNDEBUG
all: target

debug: CFLAGS += -g3 -D_FORTIFY_SOURCE=2
debug: target

san: debug
san: CFLAGS += -fsanitize=address,undefined
san: LDFLAGS += -fsanitize=address,undefined

target: imgui implot cjson $(TARGET)

pkg: all lib shlib
	tar -cJf dstream.tar.xz $(TARGET) $(BUILD_DIR)/lib$(TARGET_NAME).a $(BUILD_DIR)/lib$(TARGET_NAME).so include

install:
	sudo mkdir -p /usr/local/include/dstream
	sudo cp -r include/* /usr/local/include/dstream
	sudo cp $(TARGET) /usr/local/bin
	sudo cp $(BUILD_DIR)/lib$(TARGET_NAME).a $(BUILD_DIR)/lib$(TARGET_NAME).so /usr/local/lib

uninstall:
	sudo rm -rf /usr/local/include/dstream
	sudo rm -f /usr/local/bin/$(TARGET_NAME)
	sudo rm -f /usr/local/lib/lib$(TARGET_NAME).a /usr/local/lib/lib$(TARGET_NAME).so

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

lib: CFLAGS += -O3 -DNDEBUG
lib: $(LIB_OBJS)
	$(AR) -rcs $(BUILD_DIR)/lib$(TARGET_NAME).a $(LIB_OBJS)

shlib: CFLAGS += -O3 -DNDEBUG
shlib: $(LIB_OBJS)
	$(CXX) --shared $(LIB_OBJS) -o $(BUILD_DIR)/lib$(TARGET_NAME).so

.PHONY: clean compdb valgrind run test test-compdb

run: san
	./$(TARGET)

clean:
	@rm -rf $(addprefix $(BUILD_DIR)/,$(filter-out compile_commands.json,$(shell ls $(BUILD_DIR))))

compdb: clean
	@bear -- $(MAKE) san
	@mv compile_commands.json build

valgrind: debug
	@valgrind ./$(TARGET)

imgui:
	mkdir -p imgui && \
	curl -L https://codeload.github.com/ocornut/imgui/tar.gz/refs/tags/v1.89.2 | \
	tar --strip-components=1 -xz -C imgui

implot:
	mkdir -p implot && \
	curl -L https://github.com/epezent/implot/archive/refs/tags/v0.14.tar.gz | \
	tar --strip-components=1 -xz -C implot

cjson:
	mkdir -p cjson && \
	curl -L https://github.com/DaveGamble/cJSON/archive/refs/tags/v1.7.15.tar.gz | \
	tar --strip-components=1 -xz -C cjson

test: imgui implot cjson
	$(MAKE) -C $(TEST_DIR)

test-compdb: imgui implot cjson
	bear -- $(MAKE) -C $(TEST_DIR) build-only
	mkdir -p build
	mv compile_commands.json build

-include $(DEPS)
