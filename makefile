CXX      := g++
CXXFLAGS := -Wall -Wextra --std=c++17
LDFLAGS  := -L/usr/lib -lstdc++ -lm -lpthread
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)
TARGET   := stschedule
INCLUDE  := -Iinclude/
SRC      :=                      \
   $(wildcard src/nns/*.cpp)     \
   $(wildcard src/json/*.cpp)    \
   $(wildcard src/*.cpp)         \

OBJECTS  := $(SRC:%.cpp=$(OBJ_DIR)/%.o)
DEPENDENCIES \
         := $(OBJECTS:.o=.d)

all: build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -MMD -o $@

$(APP_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(APP_DIR)/$(TARGET) $^ $(LDFLAGS)

-include $(DEPENDENCIES)

.PHONY: all build clean debug release perf info

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O3
release: all

perf: CXXFLAGS += -g
perf: all

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*

info:
	@echo "[*] Application dir: ${APP_DIR}     "
	@echo "[*] Object dir:      ${OBJ_DIR}     "
	@echo "[*] Sources:         ${SRC}         "
	@echo "[*] Objects:         ${OBJECTS}     "
	@echo "[*] Dependencies:    ${DEPENDENCIES}"

TOOLS_SRC := $(wildcard ir_tools/*.cpp)
JSON_SRC  := $(filter src/json/%.cpp, $(SRC))
JSON_OBJ  := $(JSON_SRC:%.cpp=$(OBJ_DIR)/%.o)
TOOLS     := $(TOOLS_SRC:ir_tools/%.cpp=$(APP_DIR)/%)

tools: build $(TOOLS)

$(TOOLS): $(APP_DIR)/%: $(OBJ_DIR)/ir_tools/%.o $(JSON_OBJ)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	