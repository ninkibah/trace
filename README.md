# trace
A simple C++17 class for logging function entry and exit
## Tracing tools
Sometimes gdb does not work on your code, but you still need to see what is happening.

By using the Trace class, you can log what is happening in your code with
very little effort.

The tracing library supports multi-threaded code using a thread local variable for the
most recent trace object.

### Installation
You need to copy Trace.h and pp_magic.h to your project's include directory.
The trace library is a header only library.

In each source file, you want to trace, you will need to include Trace.h:
````
#include "Trace.h"
````

### Available macros
INCLUDE_IN_BACKTRACE will add the current scope to any backtraces that
are created when throwing an exception. It will not produce any output.

TRACE() will add the current scope to a backtrace, and it will also generate 2
lines of output:
````
Thread(1) ==> /home/joconnor/projects/cpp/talks/trace/main.cpp:17 - void g(int)
Thread(1) <== /home/joconnor/projects/cpp/talks/trace/main.cpp:17 - void g(int)
````
Often you may want to also trace one or more arguments. TRACE(x) generates:
````
Thread(1) ==> /home/joconnor/projects/cpp/talks/trace/main.cpp:17 - void g(int) x => 3
Thread(1) <== /home/joconnor/projects/cpp/talks/trace/main.cpp:17 - void g(int)
````
If the argument is a string, then it will only show the first 10 characters of it.

Sometimes, you need to see what value certain expressions have within the
body of a function. In this case, you can use TRACE_OUT(expr1, expr2, ...).
You can pass any number of expressions to the macro.

TRACE_OUT(x + 2) produces:
````
Thread(1) x + 2 => 5
````

The default output stream is std::cout, but you can change the
output stream on a per thread basis by calling the TRACE_TO(os) macro:
````
TRACE_TO(std::cerr);
// Now all tracing output will go to cerr, instead of cout
````

###Sample program
This sample also shows how you can catch signals. This is very useful in difficult to debug areas.

````
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
````

Generates the following output:
````
Finished unwinding
Thread(1) <== /home/joconnor/projects/cpp/talks/trace/main.cpp:30 - Foo::Foo(const string&)
Thread(1) ==> /home/joconnor/projects/cpp/talks/trace/main.cpp:30 - Foo::Foo(const string&) s => A very lon...
Thread(1) <== /home/joconnor/projects/cpp/talks/trace/main.cpp:30 - Foo::Foo(const string&)
Thread(1) ==> /home/joconnor/projects/cpp/talks/trace/main.cpp:35 - void f()
Thread(1) ==> /home/joconnor/projects/cpp/talks/trace/main.cpp:22 - int hitchhiker()
Thread(1) ==> /home/joconnor/projects/cpp/talks/trace/main.cpp:17 - T plus2(T) [with T = double] x => 3
Thread(1) <== /home/joconnor/projects/cpp/talks/trace/main.cpp:17 - T plus2(T) [with T = double]
Thread(1) ==> /home/joconnor/projects/cpp/talks/trace/main.cpp:17 - T plus2(T) [with T = int] x => 4
Thread(1) <== /home/joconnor/projects/cpp/talks/trace/main.cpp:17 - T plus2(T) [with T = int]
Thread(1) ==> /home/joconnor/projects/cpp/talks/trace/main.cpp:5 - inc(int) lambda x => 42
Thread(1) <== /home/joconnor/projects/cpp/talks/trace/main.cpp:5 - inc(int) lambda
Thread(1) <== /home/joconnor/projects/cpp/talks/trace/main.cpp:22 - int hitchhiker()
Thread(1) <== /home/joconnor/projects/cpp/talks/trace/main.cpp:35 - void f()
Thread(1) <== /home/joconnor/projects/cpp/talks/trace/main.cpp:41 - int main()
````
