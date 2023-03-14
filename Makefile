EXECUTABLE = ./beekDB

SRC_DIR = ./src
BUILD_DIR = ./build
HEADERS_DIR = ./include
TESTS_DIR = ./tests

sources := $(shell find $(SRC_DIR) -name "*.cpp")
objects := $(sources:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
deps := $(objects:.o=.d)
tests := $(shell find $(TESTS_DIR) -name "*.cpp")

all: $(EXECUTABLE)

.PHONY: test
test: $(EXECUTABLE) $(tests)
	@$(CXX) -std=c++17 -I $(HEADERS_DIR) $(tests) $(filter-out ./build/main.o, $(objects)) -o $@ -lsqlparser
	@./test

$(EXECUTABLE): $(objects)
	$(CXX) -I $(HEADERS_DIR) -std=c++17 $(objects) -o $@ -lsqlparser

$(objects): $(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) -std=c++17 -I $(HEADERS_DIR) -MMD -MP -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(EXECUTABLE) test

-include $(deps)
