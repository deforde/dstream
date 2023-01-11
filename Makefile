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

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

LIB_SRCS := $(shell find $(SRC_DIRS) -name 'dstream_*.c')
LIB_OBJS := $(LIB_SRCS:%=$(BUILD_DIR)/%.o)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_DIRS += imgui imgui/backends /usr/include/SDL2 implot

INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CFLAGS := -Wall -Wextra -Wpedantic -Werror $(INC_FLAGS) -MMD -MP
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

target: imgui implot $(TARGET)

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

test: imgui implot
	$(MAKE) -C $(TEST_DIR)

test-compdb: imgui implot
	bear -- $(MAKE) -C $(TEST_DIR) build-only
	mkdir -p build
	mv compile_commands.json build

-include $(DEPS)
