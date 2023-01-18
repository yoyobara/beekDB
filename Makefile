EXECUTABLE = ./beekDB

SRC_DIR = ./src
BUILD_DIR = ./build
HEADERS_DIR = ./include

sources := $(shell find $(SRC_DIR) -name "*.cpp")
objects := $(sources:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
deps := $(objects:.o=.d)

$(EXECUTABLE): $(objects)
	$(CXX) $(objects) -o $@

$(objects): $(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) -I $(HEADERS_DIR) -MMD -MP -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(EXECUTABLE)

-include $(deps)
