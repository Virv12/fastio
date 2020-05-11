CC=g++ -std=c++17 -Wall -Wextra -Ofast

run: main
	./main >/dev/null

main: main.cpp fastio.h
	$(CC) -o $@ main.cpp

clean:
	rm -rf main

.PHONY: run clean
