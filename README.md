# fastio
Modern c++ fastio

# Usage
```c++
#include "fastio.h"

Ifast fastin(stdin);
Ofast fastout(stdout);

int main() {
  int x;
  fastin >> x;
  
  fastout << "X: " << x << '\n';
  fastout("X: %\n", x);
}
