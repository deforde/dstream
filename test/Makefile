TARGET_NAME := test

UNITY_ROOT_DIR := Unity
UNITY_LIB := $(UNITY_ROOT_DIR)/build/libunity.a
UNITY_INC_DIR := $(UNITY_ROOT_DIR)/src

BASE_BUILD_DIR := build
BUILD_DIR := $(BASE_BUILD_DIR)/test_build
SRC_DIRS := src ../src

SRCS := $(filter-out ../src/main.c,$(shell find $(SRC_DIRS) -name '*.c'))
SRCS += $(shell find $(SRC_DIRS) -name '*.cpp')
SRCS += ../imgui/imgui.cpp
SRCS += ../imgui/imgui_demo.cpp
SRCS += ../imgui/imgui_draw.cpp
SRCS += ../imgui/imgui_tables.cpp
SRCS += ../imgui/imgui_widgets.cpp
SRCS += ../imgui/backends/imgui_impl_sdl.cpp
SRCS += ../imgui/backends/imgui_impl_opengl3.cpp
SRCS += ../implot/implot.cpp
SRCS += ../implot/implot_items.cpp
SRCS += ../cjson/cJSON.c

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_DIRS += ../include ../imgui ../imgui/backends /usr/include/SDL2 ../implot ../cjson
INC_DIRS += $(UNITY_INC_DIR)

INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CFLAGS := -g3 -Wall -Wextra -Wpedantic -Werror -Wno-overlength-strings $(INC_FLAGS) -MMD -MP -fsanitize=address,undefined
LDFLAGS := $(UNITY_LIB) -lSDL2 -lGL -fsanitize=address,undefined

CXXFLAGS = $(CFLAGS)

TARGET := $(BASE_BUILD_DIR)/$(TARGET_NAME)

.PHONY: all build-only

all: unity ../imgui ../implot ../cjson $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BASE_BUILD_DIR)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

unity: $(UNITY_LIB)

$(UNITY_LIB):
	git clone https://github.com/ThrowTheSwitch/Unity && \
	mkdir -p Unity/build && \
	cd Unity/build && \
	cmake .. && \
	cmake --build . -j 7

build-only: clean unity $(TARGET)

../imgui:
	make -C .. imgui

../implot:
	make -C .. implot

../cjson:
	make -C .. cjson

-include $(DEPS)
