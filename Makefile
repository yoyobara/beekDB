EXECUTABLE = beekDB
TEST_EXECUTABLE = test

SRC_DIR = ./src
BUILD_DIR = ./build
HEADERS_DIR = ./include
TESTS_DIR = ./tests

sources := $(shell find $(SRC_DIR) -name "*.cpp")
objects := $(sources:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
deps := $(objects:.o=.d)
tests := $(shell find $(TESTS_DIR) -name "*.cpp")

libs := -lsqlparser $(shell pkg-config --libs spdlog)
libs_cflags := $(shell pkg-config --cflags spdlog)

all: $(EXECUTABLE)

$(EXECUTABLE): $(objects)
	$(CXX) -I $(HEADERS_DIR) -std=c++17 $(libs_cflags) $(objects) -o $@ $(libs)

.PHONY: test
test: $(EXECUTABLE) $(tests)
	@$(CXX) -std=c++17 -I $(HEADERS_DIR) $(tests) $(libs_cflags) $(filter-out ./build/main.o, $(objects)) -o $(TEST_EXECUTABLE) -lgtest -lgtest_main $(libs)
	@./$(TEST_EXECUTABLE)

$(objects): $(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(libs_cflags) -std=c++17 -I $(HEADERS_DIR) -MMD -MP -c $< -o $@ $(libs)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(EXECUTABLE) $(TEST_EXECUTABLE) tmp/*

-include $(deps)
