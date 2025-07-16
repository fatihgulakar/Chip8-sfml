CXX = g++
LIBSFML_HOME=/opt/homebrew/Cellar/sfml/3.0.0/lib
SFML_INCLUDE = /opt/homebrew/Include

SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
TARGET_DIR = $(BUILD_DIR)/bin
BUILD_LOG = $(BUILD_DIR)/build.log

CXXFLAGS = -Wall -std=c++17 -MMD -MP
CXXFLAGS += -I$(INC_DIR)
CXXFLAGS += -I$(SFML_INCLUDE)

LDFLAGS += -L$(LIBSFML_HOME)
LDFLAGS += -lsfml-graphics -lsfml-window -lsfml-system

TARGET = $(TARGET_DIR)/chip8

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
DEPS = $(OBJS:.o=.d)

# Create necessary directories
$(shell mkdir -p $(OBJ_DIR) $(TARGET_DIR))

# Default target
all: $(BUILD_LOG) $(TARGET)

# Clear log at start of each build
$(BUILD_LOG):
	@echo "[Build started at $$(date)]" > $(BUILD_LOG)

# Linking
$(TARGET): $(OBJS)
	@echo "[Linking] $@" | tee -a $(BUILD_LOG)
	@$(CXX) $^ -o $@ $(LDFLAGS) >> $(BUILD_LOG) 2>&1

# Compilation + dependency generation
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "[Compiling] $<" | tee -a $(BUILD_LOG)
	@$(CXX) $(CXXFLAGS) -c $< -o $@ >> $(BUILD_LOG) 2>&1

# Clean
clean:
	@rm -rf $(BUILD_DIR)

# Include auto-generated dependency files
-include $(DEPS)

.PHONY: all clean