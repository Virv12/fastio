CC=g++ -std=c++20 -Ofast -Wall -Wextra -Wpedantic -DNDEBUG

run: main
	./main >/dev/null

test: tester
	./tester

tester: tester.cpp fastio.h
	g++ -std=c++20 -Og -Wall -Wextra -Wpedantic -fsanitize=address -D_GLIBCXX_DEBUG -o $@ $<

main: main.cpp fastio.h
	$(CC) -o $@ $<

clean:
	rm -rf main

.PHONY: run test clean
