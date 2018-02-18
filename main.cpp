#include <iostream>
#include "trace.h"

// Some functions that call one another
int hitchhiker() {
  TRACE;
  Trace::printStackTrace(std::cout);
  return 42;
}

void f() {
  TRACE;
  hitchhiker();
}

int main() {
  TRACE;
  f();
  return 0;
}