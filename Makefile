CC = g++

GTEST_LINK = -lgtest -lgtest_main
INCLUDE = -I include/

objects = build/main.o build/cpp_socket.o

APP = beekDB

beekDB: $(objects)
	$(CC) -o $(APP) $(objects)

build/cpp_socket.o: src/cpp_socket.cpp include/cpp_socket.h
	mkdir -p build
	$(CC) $(INCLUDE) -c src/cpp_socket.cpp -o build/cpp_socket.o

build/main.o: src/main.cpp
	mkdir -p build
	$(CC) $(INCLUDE) -c src/main.cpp -o build/main.o

clean:
	rm -rf build $(APP)
