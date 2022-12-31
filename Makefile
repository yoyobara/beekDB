CC = g++

SOURCE_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include

SOURCES := $(wildcard $(SOURCE_DIR)/*.cpp)
OBJECTS := $(patsubst $(SOURCE_DIR)/%.cpp,$(BUILD_DIR)/%.o, $(SOURCES))

GTEST_LINK = -lgtest -lgtest_main

EXECUTABLE = beekDB

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $(EXECUTABLE) $(OBJECTS)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CC) -I $(INCLUDE_DIR) -c $< -o $@


clean:
	rm -rf $(BUILD_DIR) $(EXECUTABLE)
