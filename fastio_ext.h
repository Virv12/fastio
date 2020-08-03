#include "fastio.h"
#include <array>
#include <set>
#include <utility>
#include <vector>

namespace IOfast {
	template <class T> Ofast &print_iters(Ofast &out, T begin, T end) noexcept {
		out << '{';
		while (begin != end) {
			out << *begin;
			begin++;
			if (begin != end)
				out << ", ";
			else
				out << '}';
		}
		return out;
	}

	template <class T, class A> Ofast &operator<<(Ofast &out, std::vector<T, A> const &v) noexcept {
		out << "std::vector";
		print_iters(out, v.begin(), v.end());
		return out;
	}

	template <class T, size_t N> Ofast &operator<<(Ofast &out, std::array<T, N> const &a) noexcept {
		out << "std::array";
		print_iters(out, a.begin(), a.end());
		return out;
	}

	template <class T, class C, class A> Ofast &operator<<(Ofast &out, std::set<T, C, A> const &s) noexcept {
		out << "std::set";
		print_iters(out, s.begin(), s.end());
		return out;
	}

	template <class T1, class T2> Ofast &operator<<(Ofast &out, std::pair<T1, T2> const &p) noexcept {
		return out << "std::pair{" << p.first << ", " << p.second < '}';
	}
} // namespace IOfast
