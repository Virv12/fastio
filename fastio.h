#include <cmath>
#include <cstdio>
#include <cstdint>
#include <cstring>

#include <array>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <unistd.h>
using namespace std;

struct Ofast {
	char buffer[1 << 16];
	size_t idx = 0;
	int fd;

	Ofast(int fd) : fd(fd) {}
	Ofast(FILE* f) : fd(fileno(f)) {}

	~Ofast() {
		flush();
		close(fd);
	}

	void flush() {
		write(fd, buffer, idx);
		idx = 0;
	}

	void flush_if(size_t x) {
		if (sizeof(buffer) - idx < x)
			flush();
	}

	Ofast& operator<< (const char c) {
		flush_if(1);
		buffer[idx++] = c;
		return *this;
	}

	template <class T, class = enable_if_t<is_integral<T>::value>>
	Ofast& operator<< (T a) {
		char d[(long)(sizeof(T) * log10(256)) + 1];
		uint8_t i = sizeof(d);

		static_assert(sizeof(d) <= 256);

		if constexpr (is_signed<T>::value)
			if (signbit(a)) {
				*this << '-';
				a *= -1;
			}

		do {
			d[--i] = a % 10 + '0';
			a /= 10;
		} while (a);

		flush_if(sizeof(d) - i);
		memcpy(buffer + idx, d + i, sizeof(d) - i);
		idx += sizeof(d) - i;
		return *this;
	}

	Ofast& operator<< (const char* s) {
		size_t len = strlen(s);
		flush_if(len);
		memcpy(buffer + idx, s, len);
		idx += len;
		return *this;
	}

	Ofast& operator<< (const string_view s) {
		flush_if(s.size());
		memcpy(buffer + idx, s.data(), s.size());
		idx += s.size();
		return *this;
	}

	Ofast& operator<< (const string& s) {
		flush_if(s.size());
		memcpy(buffer + idx, s.data(), s.size());
		idx += s.size();
		return *this;
	}

	Ofast& operator<< (const double a) {
		flush_if(32);
		idx += sprintf(buffer + idx, "%g", a);
		return *this;
	}

	template <class T>
	void helper2(size_t i, const T& v, size_t j) {
		if (i == j) *this << v;
	}

	template <class ... T, size_t ... Is>
	void helper(size_t i, const T& ... v, index_sequence<Is...>) {
		((helper2(i, v, Is)), ...);
	}

	template <class ... T>
	void operator() (const char* s, const T& ... v) {
		size_t i = 0;
		while (*s) {
			if (*s == '%')
				helper<T ...>(i++, v ..., make_index_sequence<sizeof...(T)>());
			else
				*this << *s;
			s++;
		}
	}
};

constexpr array<bool, 256> digits() {
    array<bool, 256> table {};
    for (char c = '0'; c <= '9'; c++)
        table[c] = true;
    return table;
}
constexpr const array<bool, 256> is_digit = digits();

struct Ifast {
	char buffer[1 << 16];
	size_t idx = 0, size = 0;
	int fd;

	Ifast(int fd) : fd(fd) {}
	Ifast(FILE* f) : fd(fileno(f)) {}

	void flush() {
		if (idx == size) {
			size = read(fd, buffer, sizeof(buffer));
			idx = 0;
		}
	}

	Ifast& operator>> (char& c) {
		flush();
		c = buffer[idx++];
		return *this;
	}

	template <class T, class = enable_if_t<is_integral<T>::value>>
	Ifast& operator>> (T& i) {
		while (flush(), buffer[idx] <= 32) idx ++;

		bool sign = false;
		if constexpr (is_signed<T>::value)
			if (buffer[idx] == '-') {
				sign = 1;
				idx++;
			}

		i = 0;
		while (flush(), is_digit[buffer[idx]])
			i = 10 * i + buffer[idx++] - '0';

		if constexpr (is_signed<T>::value)
			if (sign) i *= -1;

		return *this;
	}
};
