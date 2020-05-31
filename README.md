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
  fastout.fmt<"X: %\n">(x);
}
```
