#include <iostream>
#include "trace.h"

auto inc = [](int x) {
  TRACE;
  Trace::printStackTrace(std::cout);
  return x + 1;
};

auto dec = [](int x) {
  TRACE;
  return x + 1;
};

template<typename T>
T plus2(T x) {
  TRACE;
  return x + 2;
}
// Some functions that call one another
int hitchhiker() {
  TRACE;
  plus2(3.0);
  plus2(4);
  return inc(42);
}

struct Foo {
  Foo() {
    TRACE;
    Trace::printStackTrace(std::cout);
  }
};

void f() {
  TRACE;
  hitchhiker();
}

int main() {
  TRACE;
  f();
  Foo foo;
  return 0;
}