#include <iostream>
#include "trace.h"

// Some functions that call one another
int hitchhiker() {
  Trace trace(__FILE__, __func__, __LINE__);
  Trace::printStackTrace(std::cout);
  return 42;
}

void f() {
  Trace trace(__FILE__, __func__, __LINE__);
  hitchhiker();
}

int main() {
  Trace trace(__FILE__, __func__, __LINE__);
  f();
  return 0;
}