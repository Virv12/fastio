# fastio
Modern c++ fastio

# Usage
```c++
#include "fastio.h"

IOfast::Ifast fastin(stdin);
IOfast::Ofast fastout(stdout);

int main() {
  int x;
  fastin >> x;
  
  fastout << "X: " << x << '\n';
  fastout("X: %\n", x);
  fastout.flush();
}
