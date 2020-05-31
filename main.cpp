#include <cfloat>
#include <climits>

#include <iostream>
#include <fstream>
#include <string>
#include <string_view>

#include "fastio.h"
#include "fastio_ext.h"
using namespace std;
using namespace IOfast;

Ifast fastin(STDIN_FILENO);
Ofast fastout(STDOUT_FILENO);
Ofast fasterr(STDERR_FILENO);

long gettime() {
	timespec t;
	clock_gettime(0, &t);
	return t.tv_sec * (long)1e9 + t.tv_nsec;
}

#define SPEED_TEST(n, fn, end) { size_t i = n; long __time = gettime(); while (i--) fn; end; __time = gettime() - __time; fasterr.fmt<"%ns\t%\n">(to_string((double)__time / n), #fn); }

const string long_string = "sfhsfhgalsfhglsvgeshrbabashrlahrbagrvbiagrviagilvsgiawurhlvbishfgsdfrgviaywrvtia";
const string short_string = "";

Ofast& operator<< (Ofast& out, nullptr_t) {
	out.flush();
	return out;
}

int main() {
	fasterr.fmt<"int: %\n">(-45);
	fasterr.fmt<"char*: %\n">("test");
	fasterr.fmt<"double: %\n">(123.456);
	fasterr.fmt<"double: %\n">(DBL_MAX);
	fasterr.fmt<"double: %\n">(DBL_MIN);
	fasterr.fmt<"double: %\n">(-DBL_MAX);
	fasterr.fmt<"float: %\n">(FLT_MAX);
	fasterr.fmt<"vector<int>: %\n">(vector{3, 4, 5});
	fasterr << '\n' << nullptr;

	SPEED_TEST(1e7, fastout << 0   , fastout.flush());
	SPEED_TEST(1e7, printf("%d", 0), fflush(stdout));
	SPEED_TEST(1e7, cout << 0      , cout.flush());
	fasterr << '\n' << nullptr;

	SPEED_TEST(1e7, fastout << ULONG_MAX    , fastout.flush());
	SPEED_TEST(1e7, printf("%ld", ULONG_MAX), fflush(stdout));
	SPEED_TEST(1e7, cout << ULONG_MAX       , cout.flush());
	fasterr << '\n' << nullptr;

	[[maybe_unused]] int i = 0;
	SPEED_TEST(1e7, fastout << &i            , fastout.flush());
	SPEED_TEST(1e7, printf("%p\n", (void*)&i), fflush(stdout));
	SPEED_TEST(1e7, cout << &i               , cout.flush());
	fasterr << '\n' << nullptr;

	SPEED_TEST(1e7, fastout << long_string                       , fastout.flush());
	SPEED_TEST(1e7, fastout << long_string.c_str()               , fastout.flush());
	SPEED_TEST(1e7, fastout << (string_view)long_string          , fastout.flush());
	SPEED_TEST(1e7, fastout << (string_view)(long_string.c_str()), fastout.flush());
	SPEED_TEST(1e7, printf("%s", long_string.data())             , fflush(stdout));
	SPEED_TEST(1e7, puts(long_string.data())                     , fflush(stdout));
	SPEED_TEST(1e7, cout << long_string.data()                   , cout.flush());
	SPEED_TEST(1e7, cout << long_string                          , cout.flush());
	fasterr << '\n' << nullptr;

	SPEED_TEST(1e7, fastout << short_string                       , fastout.flush());
	SPEED_TEST(1e7, fastout << short_string.c_str()               , fastout.flush());
	SPEED_TEST(1e7, fastout << (string_view)short_string          , fastout.flush());
	SPEED_TEST(1e7, fastout << (string_view)(short_string.c_str()), fastout.flush());
	SPEED_TEST(1e7, printf("%s", short_string.data())             , fflush(stdout));
	SPEED_TEST(1e7, puts(short_string.data())                     , fflush(stdout));
	SPEED_TEST(1e7, cout << short_string.data()                   , cout.flush());
	SPEED_TEST(1e7, cout << short_string                          , cout.flush());
	fasterr << '\n' << nullptr;

	SPEED_TEST(1e7, fastout.fmt<"sfhsfhgalsfhglsvgeshrbabashrlahrbagrvbiagrviagilvsgiawurhlvbishfgsdfrgviaywrvtia">(), fastout.flush());
	SPEED_TEST(1e7, printf(long_string.data()) , fflush(stdout));
	fasterr << '\n' << nullptr;

	SPEED_TEST(1e7, fastout.fmt<"%%%%%%%%%%">('a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j')     , fastout.flush());
	SPEED_TEST(1e7, fastout << 'a' << 'b' << 'c' << 'd' << 'e' << 'f' << 'g' << 'h' << 'i' << 'j'   , fastout.flush());
	SPEED_TEST(1e7, printf("%c%c%c%c%c%c%c%c%c%c", 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'), fflush(stdout));
	SPEED_TEST(1e7, cout << 'a' << 'b' << 'c' << 'd' << 'e' << 'f' << 'g' << 'h' << 'i' << 'j'      , cout.flush());
	fasterr << '\n' << nullptr;

	SPEED_TEST(1e7, fastout << 123.456           , fastout.flush());
	SPEED_TEST(1e7, fastout << to_string(123.456), fastout.flush());
	SPEED_TEST(1e7, printf("%f", 123.456)        , fflush(stdout));
	SPEED_TEST(1e7, printf("%g", 123.456)        , fflush(stdout));
	SPEED_TEST(1e7, cout << 123.456              , cout.flush());
	fasterr << '\n' << nullptr;
	
	SPEED_TEST(1e6, fastout << DBL_MAX           , fastout.flush());
	SPEED_TEST(1e6, fastout << (float)FLT_MAX    , fastout.flush());
	SPEED_TEST(1e6, printf("%g", DBL_MAX)        , fflush(stdout));
	SPEED_TEST(1e6, cout << DBL_MAX              , cout.flush());
	fasterr << '\n' << nullptr;
	
	{
		Ofast fout("tmp.txt");
		for (size_t i = 0; i < 1e7; i++)
			fout.fmt<"% ">(INT_MIN);
	}

	{
		Ifast fin("tmp.txt");
		int x;
		SPEED_TEST(1e7, fin >> x, );
	}

	{
		int x;
		FILE* fin = fopen("tmp.txt", "rt");
		SPEED_TEST(1e7, fscanf(fin, "%d", &x), );
	}

	{
		int x;
		ifstream fin("tmp.txt");
		SPEED_TEST(1e7, fin >> x, );
	}
}
