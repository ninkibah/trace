#include <iostream>
#include "trace.h"

auto inc = [](int x) {
  LoggedTrace trace(__FILE__, "inc(int) lambda", __LINE__, "x", x);
  throw 1;
  return x + 1;
};

auto dec = [](int x) {
  TRACE(x);
  return x + 1;
};

template<typename T>
T plus2(T x) {
  TRACE(x);
  return x + 2;
}
// Some functions that call one another
int hitchhiker() {
  TRACE();
  plus2(3.0);
  plus2(4);
  return inc(42);
}

struct Foo {
  Foo(std::string const& s) {
    TRACE(s);
  }
};

void f() {
  TRACE();
  hitchhiker();
}

int main() {
  TRACE_TO(std::cerr);
  TRACE();
  try {
    Foo foo1("FUBAR");
    Foo foo2("A very long string of characters which should be chopped in trace output");
    f();
  } catch(...) {
    std::cout << "Finished unwinding\n";
  }
  return 0;
}