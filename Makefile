CC=g++ -std=c++20 -Ofast -Wall -Wextra -Wpedantic -DNDEBUG

run: main
	./main >/dev/null

main: main.cpp fastio.h
	$(CC) -o $@ main.cpp

clean:
	rm -rf main

.PHONY: run clean
