#include "fastio.h"
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
using namespace std;

void loading_bar(char const str[], int const perc) {
	fprintf(stderr, "%s [", str);
	for (int i = 0; i < 100; i++)
		fputc(i < perc ? '#' : '-', stderr);
	fprintf(stderr, "]%c", perc == 100 ? '\n' : '\r');
	fflush(stderr);
}

IOfast::Ofast fastin(STDIN_FILENO);
IOfast::Ofast fastout(STDOUT_FILENO);

int main() {
	close(STDIN_FILENO);
	close(STDOUT_FILENO);

	int pfds[2];
	pipe(pfds);

	unsigned short i = 0;
	do {
		short a = i;
		fastout << a << '\n';
		fastout.flush();

		short b;
		scanf("%hd", &b);

		if (a != b) {
			fprintf(stderr, "error printing short %hd\n", a);
			exit(1);
		}

		i++;

		if ((i & 0xF) == 0) loading_bar("Ofast << short", i * 100l / (1 << 16));
	} while (i);

	fputs("Ofast << short; success\n", stderr);
}
