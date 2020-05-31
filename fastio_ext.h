#include <array>
#include <set>
#include <vector>

#include "fastio.h"

namespace IOfast {
	using namespace std;

	template <class T>
	Ofast& print_iters(Ofast& out, T begin, T end) noexcept {
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

	template <class T>
	Ofast& operator<<(Ofast& out, const vector<T>& v) noexcept {
		out << "std::vector";
		print_iters(out, v.begin(), v.end());
		return out;
	}

	template <class T, size_t N>
	Ofast& operator<<(Ofast& out, const array<T, N>& a) noexcept {
		out << "std::array";
		print_iters(out, a.begin(), a.end());
		return out;
	}

	template <class T>
	Ofast& operator<<(Ofast& out, const set<T>& s) noexcept {
		out << "std::set";
		print_iters(out, s.begin(), s.end());
		return out;
	}
}
