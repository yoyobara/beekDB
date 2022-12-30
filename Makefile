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

$(BUILD_DIR)/main.o: $(SOURCE_DIR)/main.cpp
	@mkdir -p build
	$(CC) -I $(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp $(INCLUDE_DIR)/%.h 
	@mkdir -p build
	$(CC) -I $(INCLUDE_DIR) -c $< -o $@


clean:
	rm -rf $(BUILD_DIR) $(EXECUTABLE)
