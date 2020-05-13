#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include <array>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <unistd.h>
using namespace std;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1 << 16
#endif

#ifndef NO_AUTO_FLUSH
#define NO_AUTO_FLUSH 1
#endif

struct Ofast {
	char buffer[BUFFER_SIZE];
	size_t idx = 0;
	const int fd;

	Ofast(int fd) : fd(fd) {}
	Ofast(FILE* f) : fd(fileno(f)) {}

#if NO_AUTO_BUFFER < 1
	~Ofast() {
		flush();
		close(fd);
	}
#endif

	void flush() noexcept {
		[[maybe_unused]] ssize_t rc = write(fd, buffer, idx);
		assert(rc >= 0);
		idx = 0;
	}

	void flush_if(size_t x) noexcept {
#if NO_AUTO_FLUSH < 2
		if (sizeof(buffer) - idx < x)
			flush();
#endif
	}

	Ofast& operator<< (const char c) noexcept {
		flush_if(1);
		buffer[idx++] = c;
		return *this;
	}

	template <class T, class = enable_if_t<is_integral<T>::value>,
		class unsT = typename make_unsigned<T>::type>
	Ofast& operator<< (const T a) noexcept {
		char d[(long)(sizeof(T) * log10(256)) + 1];
		uint8_t i = sizeof(d);

		static_assert(sizeof(d) <= 256);

		unsT u = a;

		if constexpr (is_signed<T>::value)
			if (signbit(a)) {
				*this << '-';
				u = -u;
			}

		do {
			d[--i] = u % 10 + '0';
			u /= 10;
		} while (u);

		flush_if(sizeof(d) - i);
		memcpy(buffer + idx, d + i, sizeof(d) - i);
		idx += sizeof(d) - i;
		return *this;
	}

	Ofast& operator<< (const char* const s) noexcept {
		size_t len = strlen(s);
		flush_if(len);
		memcpy(buffer + idx, s, len);
		idx += len;
		return *this;
	}

	Ofast& operator<< (const string_view s) noexcept {
		flush_if(s.size());
		memcpy(buffer + idx, s.data(), s.size());
		idx += s.size();
		return *this;
	}

	Ofast& operator<< (const string& s) noexcept {
		flush_if(s.size());
		memcpy(buffer + idx, s.data(), s.size());
		idx += s.size();
		return *this;
	}

	Ofast& operator<< (const double a) noexcept {
		flush_if(32);
		idx += sprintf(buffer + idx, "%g", a);
		return *this;
	}

	template <class T>
	void helper2(const size_t i, const T& v, const size_t j) noexcept {
		if (i == j) *this << v;
	}

	template <class ... T, size_t ... Is>
	void helper(const size_t i, const T& ... v, const index_sequence<Is...>) noexcept {
		((helper2(i, v, Is)), ...);
	}

	template <class ... T>
	void operator() (const char* s, const T& ... v) noexcept {
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

[[nodiscard]] constexpr array<bool, 256> digits() noexcept {
	array<bool, 256> table {};
	for (char c = '0'; c <= '9'; c++)
		table[c] = true;
	return table;
}
constexpr const array<bool, 256> is_digit = digits();

struct Ifast {
	char buffer[BUFFER_SIZE];
	size_t idx = 0, size = 0;
	const int fd;

	Ifast(int fd) : fd(fd) {}
	Ifast(FILE* f) : fd(fileno(f)) {}

	void flush() noexcept {
#if NO_AUTO_FLUSH < 2
		if (idx == size) {
			ssize_t s = read(fd, buffer, sizeof(buffer));
			assert(s >= 0);
			size = s;
			idx = 0;
		}
#endif
	}

	Ifast& operator>> (char& c) noexcept {
		flush();
		c = buffer[idx++];
		return *this;
	}

	template <class T, class = enable_if_t<is_integral<T>::value>>
	Ifast& operator>> (T& i) noexcept {
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
