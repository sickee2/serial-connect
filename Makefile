# 编译器设置
# CXX = clang++
CXX = g++

INCLUDES = \
-Iinc \

WIN32_FTXUI_INC = -Ilibftxui/include


CXXFLAGS = -O2 -Wall -Wextra -std=c++17 $(INCLUDES)
# CXXFLAGS = -g -Wall -Wextra -std=c++17 $(INCLUDES)

# 目标可执行文件
TARGET = sscom

# 源文件
SRC_DIR = src
SRC = $(SRC_DIR)/main.cpp $(SRC_DIR)/sscom.cpp

# 构建目录
BUILD_DIR = build
OBJ = $(BUILD_DIR)/main.o $(BUILD_DIR)/sscom.o
DEP = $(OBJ:.o=.d)

# 检测操作系统
UNAME_S := $(shell uname -s)

LD_DIR =
WIN32_LD_DIR = -Llibftxui

LDFLAGS = \
-s \
-lserialport \
-lpthread

# 根据操作系统设置链接选项
ifeq ($(UNAME_S), MINGW64_NT-10.0-26100)
	CXXFLAGS += $(WIN32_FTXUI_INC)
  LDFLAGS += -lsetupapi -lCfgMgr32 -l:libftxui-component.a -l:libftxui-dom.a -l:libftxui-screen.a -static $(WIN32_LD_DIR)
else
  LDFLAGS += -lftxui-component -lftxui-dom -lftxui-screen
endif

# 默认目标
all: $(BUILD_DIR) $(TARGET)

# 创建构建目录
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# 生成依赖文件
$(BUILD_DIR)/%.d: $(SRC_DIR)/%.cpp
	@$(CXX) $(CXXFLAGS) -MM -MT $(@:.d=.o) $< > $@

# 包含依赖文件
-include $(DEP)

# 编译
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 链接
$(TARGET): $(OBJ)
	$(CXX) $^ -o $@ $(LD_DIR) $(LDFLAGS)

# 清理
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# 运行
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
