# fastio
Modern c++ fastio

# Usage
```c++
#include "fastio.h"

IOfast::Ifast fastin(STDIN_FILENO);
IOfast::Ofast fastout(STDOUT_FILENO);

int main() {
  int x;
  fastin >> x;
  
  fastout << "X: " << x << '\n';
  fastout.fmt<"X: %\n">(x);      // Only with c++20

  fastout.flush();
}
```

```c++
#include "fastio.h"

IOfast::Ofast fastout(STDOUT_FILENO);

struct S {
	int a, b;
};

auto& operator<< (IOfast::Ofast& out, S s) {
	return out << '{' << s.a << ", " << s.b << '}';
}

int main() {
	S s{3, 4};
	fastout << s << '\n';
	fastout.fmt<"Custom type: %\n">(s);  // Only with c++20
	fastout.flush();
}
```
