#include <climits>

#include <string>
#include <string_view>

#include "fastio.h"
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

#define SPEED_TEST(n, fn, end) { size_t i = n; long __time = gettime(); while (i--) fn; end; __time = gettime() - __time; fasterr("%:\t%ns\n", #fn, (double)__time / n); }

const char* long_c_string = "sfhsfhgalsfhglsvgeshrbabashrlahrbagrvbiagrviagilvsgiawurhlvbishfgsdfrgviaywrvtia";
string long_string = long_c_string;

const char* short_c_string = "";
string short_string = short_c_string;

int main() {
	fasterr("int: %\n", -45);
	fasterr("char*: %\n", "test");
	fasterr << '\n';

	SPEED_TEST(1e6, fastout << 0   , fastout.flush());
	SPEED_TEST(1e6, printf("%d", 0), fflush(stdout));
	fasterr << '\n';

	SPEED_TEST(1e6, fastout << ULONG_MAX    , fastout.flush());
	SPEED_TEST(1e6, printf("%ld", ULONG_MAX), fflush(stdout));
	fasterr << '\n';

	SPEED_TEST(1e6, fastout << long_c_string             , fastout.flush());
	SPEED_TEST(1e6, fastout << (string_view)long_c_string, fastout.flush());
	SPEED_TEST(1e6, printf("%s", long_c_string)          , fflush(stdout));
	SPEED_TEST(1e6, puts(long_c_string)                  , fflush(stdout));
	fasterr << '\n';

	SPEED_TEST(1e6, fastout << long_string             , fastout.flush());
	SPEED_TEST(1e6, fastout << long_string.c_str()     , fastout.flush());
	SPEED_TEST(1e6, fastout << (string_view)long_string, fastout.flush());
	SPEED_TEST(1e6, printf("%s", long_string.data())   , fflush(stdout));
	fasterr << '\n';

	SPEED_TEST(1e6, fastout << short_c_string             , fastout.flush());
	SPEED_TEST(1e6, fastout << (string_view)short_c_string, fastout.flush());
	SPEED_TEST(1e6, printf("%s", short_c_string)          , fflush(stdout));
	SPEED_TEST(1e6, puts(short_c_string)                  , fflush(stdout));
	fasterr << '\n';

	SPEED_TEST(1e6, fastout << short_string             , fastout.flush());
	SPEED_TEST(1e6, fastout << short_string.c_str()     , fastout.flush());
	SPEED_TEST(1e6, fastout << (string_view)short_string, fastout.flush());
	SPEED_TEST(1e6, printf("%s", short_string.data())   , fflush(stdout));
	fasterr << '\n';

	SPEED_TEST(1e6, fastout(long_c_string)  , fastout.flush());
	SPEED_TEST(1e6, fastout << long_c_string, fastout.flush());
	SPEED_TEST(1e6, printf(long_c_string)   , fflush(stdout));
	fasterr << '\n';

	SPEED_TEST(1e6, fastout("%%%%%%%%%%", 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j')         , fastout.flush());
	SPEED_TEST(1e6, printf("%c%c%c%c%c%c%c%c%c%c", 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'), fflush(stdout));
	fasterr << '\n';
	fasterr.flush();
	
	{
		Ofast fout("tmp.txt");
		for (size_t i = 0; i < 1e6; i++)
			fout("% ", INT_MIN);
		fout.flush();
	}

	{
		Ifast fin("tmp.txt");
		int x;
		SPEED_TEST(1e6, fin >> x, );
	}

	{
		int x;
		FILE* fin = fopen("tmp.txt", "rt");
		SPEED_TEST(1e6, fscanf(fin, "%d", &x), );
	}
}
