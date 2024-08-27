CFLAGS = -std=c++17 -O2 -g
LDFLAGS = -lglfw -lGL -lpthread -ldl

build/engineRT: src/* src/rtEngine/* include/rtEngine/*
	g++ $(CFLAGS) -Iinclude -o build/engineRT src/*.cpp src/*.c src/rtEngine/* $(LDFLAGS)

.PHONY: test clean

test: build/engineRT
	./build/engineRT

clean:
	rm -rf ./build/
	mkdir ./build/
	mkdir ./build/resources