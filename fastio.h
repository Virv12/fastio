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

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1 << 16
#endif

#ifndef NO_AUTO_FLUSH
#define NO_AUTO_FLUSH 1
#endif

namespace IOfast {
	using namespace std;

	struct Ofast {
		array<char, BUFFER_SIZE> buffer;
		size_t idx = 0;
		const int fd;

		Ofast(int fd) : fd(fd) {}
		Ofast(FILE* f) : fd(fileno(f)) {}

#if NO_AUTO_FLUSH < 1
		~Ofast() {
			flush();
			close(fd);
		}
#endif

		void flush() noexcept {
			[[maybe_unused]] ssize_t rc = write(fd, buffer.data(), idx);
			assert(rc >= 0);
			idx = 0;
		}

		void flush_if(size_t x) noexcept {
#if NO_AUTO_FLUSH < 2
			if (buffer.size() - idx < x)
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
			array<char, (long)(sizeof(T) * log10(256)) + 1 + is_signed<T>::value> d;
			uint8_t i = d.size();

			static_assert(d.size() <= 256);

			unsT u = a;

			if constexpr (is_signed<T>::value)
				if (signbit(a))
					u = -u;

			do {
				d[--i] = u % 10 + '0';
				u /= 10;
			} while (u);

			if constexpr (is_signed<T>::value)
				if (signbit(a))
					d[--i] = '-';

			flush_if(d.size() - i);
			memcpy(&buffer[idx], &d[i], d.size() - i);
			idx += d.size() - i;
			return *this;
		}

		Ofast& operator<< (const char* const s) noexcept {
			size_t len = strlen(s);
			flush_if(len);
			memcpy(&buffer[idx], s, len);
			idx += len;
			return *this;
		}

		Ofast& operator<< (const string_view s) noexcept {
			flush_if(s.size());
			memcpy(&buffer[idx], s.data(), s.size());
			idx += s.size();
			return *this;
		}

		Ofast& operator<< (const string& s) noexcept {
			flush_if(s.size());
			memcpy(&buffer[idx], s.data(), s.size());
			idx += s.size();
			return *this;
		}

		Ofast& operator<< (const double a) noexcept {
			flush_if(32);
			idx += sprintf(&buffer[idx], "%g", a);
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
		array<char, BUFFER_SIZE> buffer;
		size_t idx = 0, size = 0;
		const int fd;

		Ifast(int fd) : fd(fd) {}
		Ifast(FILE* f) : fd(fileno(f)) {}

		void flush() noexcept {
#if NO_AUTO_FLUSH < 2
			if (idx == size) {
				ssize_t s = read(fd, buffer.data(), buffer.size());
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
}
