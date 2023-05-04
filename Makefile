EXECUTABLE = beekDB

SRC_DIR = ./src
BUILD_DIR = ./build
HEADERS_DIR = ./include

sources := $(shell find $(SRC_DIR) -name "*.cpp")
objects := $(sources:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
deps := $(objects:.o=.d)

libs := -lsqlparser $(shell pkg-config --libs spdlog) -lcryptopp
libs_cflags := $(shell pkg-config --cflags spdlog)

all: $(EXECUTABLE)

$(EXECUTABLE): $(objects)
	$(CXX) -g -I $(HEADERS_DIR) -std=c++17 $(libs_cflags) $(objects) -o $@ $(libs)

$(objects): $(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) -g $(libs_cflags) -std=c++17 -I $(HEADERS_DIR) -MMD -MP -c $< -o $@ $(libs)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(EXECUTABLE) tmp/* __pycache__/

-include $(deps)
