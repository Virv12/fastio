#ifndef FASTIO
#define FASTIO

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>

#include <array>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <unistd.h>
#include <fcntl.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1 << 16
#endif

namespace IOfast {
	template <std::size_t N>
	struct fixed_string {
		std::array<char, N> buf;

		constexpr fixed_string(const char s[]) noexcept {
			for (size_t i = 0; i < N; i++)
				buf[i] = s[i];
		}

		constexpr auto begin() noexcept { return buf.begin(); }
		constexpr auto end()   noexcept { return buf.end();   }
		constexpr auto begin() const noexcept { return buf.begin(); }
		constexpr auto end()   const noexcept { return buf.end();   }
	};

	template <size_t N> fixed_string(const char (&)[N]) -> fixed_string<N-1>;

	struct Ofast {
		std::array<char, BUFFER_SIZE> buffer;
		size_t idx = 0;
		const int fd;

		[[nodiscard]] explicit Ofast(const int fd) noexcept : fd(fd) {}
		[[nodiscard]] explicit Ofast(const char f[]) noexcept : fd(open(f, O_WRONLY | O_CREAT)) {}
		Ofast(const Ofast&) = delete;
		Ofast& operator= (const Ofast&) = delete;

		~Ofast() {
			flush();
			close(fd);
		}

		void flush() noexcept {
			[[maybe_unused]] ssize_t rc = write(fd, buffer.data(), idx);
			assert(rc >= 0);
			idx = 0;
		}

		void flush_if(size_t x) noexcept {
#ifndef NO_AUTO_FLUSH
			if (buffer.size() - idx < x)
				flush();
#endif
		}

		Ofast& operator<< (const char c) noexcept {
			flush_if(1);
			buffer[idx++] = c;
			return *this;
		}

		template <class T, class = std::enable_if_t<std::is_integral<T>::value>,
			class unsT = typename std::make_unsigned<T>::type>
		Ofast& operator<< (const T a) noexcept {
			std::array<char, (long)(sizeof(T) * log10(256)) + 1 + std::is_signed<T>::value> d;
			uint8_t i = d.size();

			static_assert(d.size() <= 256);

			unsT u = a;

			if constexpr (std::is_signed<T>::value)
				if (std::signbit(a))
					u = -u;

			do {
				d[--i] = u % 10 + '0';
				u /= 10;
			} while (u);

			if constexpr (std::is_signed<T>::value)
				if (std::signbit(a))
					d[--i] = '-';

			flush_if(d.size() - i);
			memcpy(&buffer[idx], &d[i], d.size() - i);
			idx += d.size() - i;
			return *this;
		}

		Ofast& operator<< (const char s[]) noexcept {
			size_t len = strlen(s);
			flush_if(len);
			memcpy(&buffer[idx], s, len);
			idx += len;
			return *this;
		}

		Ofast& operator<< (const std::string_view s) noexcept {
			flush_if(s.size());
			memcpy(&buffer[idx], s.data(), s.size());
			idx += s.size();
			return *this;
		}

		Ofast& operator<< (const std::string& s) noexcept {
			flush_if(s.size());
			memcpy(&buffer[idx], s.data(), s.size());
			idx += s.size();
			return *this;
		}

		template <class T, class = std::enable_if_t<std::is_floating_point<T>::value>>
		Ofast& operator<< (T f) noexcept {
			constexpr size_t precision = 6;

			if constexpr (std::is_signed<T>::value)
				if (std::signbit(f)) {
					*this << '-';
					f *= -1;
				}

			int e = 0;
			while (f < 1) {
				f *= 10;
				e -= 1;
			}
			while (f >= 10) {
				f /= 10;
				e += 1;
			}

			if (e <= -5 || e >= 6) {
				flush_if(precision + 2);
				for (size_t i = 0; i < precision; i++) {
					if (i == 1) buffer[idx++] = '.';

					buffer[idx++] = int(f) + '0';
					f -= int(f);
					f *= 10;
				}

				buffer[idx++] = 'e';
				return *this << e;
			} else {
				f *= pow(10, e);
				return *this << (unsigned)f << '.' << (unsigned)(pow(10, precision - e - 1) * (f - (unsigned)f));
			}
		}

		Ofast& operator<< (void* p) noexcept {
			constexpr const char* digits = "0123456789abcdef";
			flush_if(2 * sizeof(void*) + 2);

			buffer[idx++] = '0';
			buffer[idx++] = 'x';

			for (ssize_t mask = 8 * sizeof(void*) - 4; mask >= 0; mask -= 4)
				buffer[idx++] = digits[(long)p >> mask & 0xF];

			return *this;
		}

		template <fixed_string s, class ... T>
		void fmt(const T& ... v) noexcept {
			static_assert(std::count(s.begin(), s.end(), '%') == sizeof...(T), "Number of parameters does not match format string");

			auto pos = s.begin();

			[[maybe_unused]] const auto helper = [this, &pos](const auto& v) {
				auto npos = std::find(pos, s.end(), '%');
				*this << std::string_view(pos, npos);
				*this << v;
				pos = npos + 1;
			};

			(helper(v), ...);
			*this << std::string_view(pos, s.end());
		}
	};

	constexpr auto is_digit = [] {
		std::array<bool, 256> is_digit {};
		for (char c = '0'; c <= '9'; c++)
			is_digit[c] = true;
		return is_digit;
	} ();

	struct Ifast {
		std::array<char, BUFFER_SIZE> buffer;
		size_t idx = 0, size = 0;
		const int fd;

		[[nodiscard]] explicit Ifast(const int fd) noexcept : fd(fd) {}
		[[nodiscard]] explicit Ifast(const char* const f) noexcept : fd(open(f, O_RDONLY)) {}
		Ifast(const Ifast&) = delete;
		Ifast& operator= (const Ifast&) = delete;

		~Ifast() {
			close(fd);
		}

		void flush() noexcept {
#ifndef NO_AUTO_FLUSH
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

		template <class T, class = std::enable_if_t<std::is_integral<T>::value>>
		Ifast& operator>> (T& i) noexcept {
			while (flush(), buffer[idx] <= 32) idx ++;

			bool sign = false;
			if constexpr (std::is_signed<T>::value)
				if (buffer[idx] == '-') {
					sign = 1;
					idx++;
				}

			i = 0;
			while (flush(), is_digit[buffer[idx]])
				i = 10 * i + buffer[idx++] - '0';

			if constexpr (std::is_signed<T>::value)
				if (sign) i *= -1;

			return *this;
		}
	};
}

#endif
