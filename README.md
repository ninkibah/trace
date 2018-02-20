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
Entering: /home/joconnor/projects/cpp/internal/template_magic/tracepointtest.cpp:30 - void g(int)
Exiting: /home/joconnor/projects/cpp/internal/template_magic/tracepointtest.cpp:30 - void g(int)
````
Often you may want to also trace one or more arguments. TRACE(x) generates:
````
Entering: /home/joconnor/projects/cpp/internal/template_magic/tracepointtest.cpp:30 - void g(int) x => 3
Exiting: /home/joconnor/projects/cpp/internal/template_magic/tracepointtest.cpp:30 - void g(int)
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
#include "Trace.h"
#include <iostream>
#include <csignal>

using namespace std;

void signal_handler(int signal) {
  INCLUDE_IN_BACKTRACE;
  cout << "Thread(" << std::this_thread::get_id() << ") Caught signal " << signal << " - Stack trace follows:" << endl << TracePoint::backtrace();
  // Note: You are not allowed to throw an exception from this function.
}

void foo() {
  INCLUDE_IN_BACKTRACE;
  //THROW_WITH_BACKTRACE("Woops something bad happened");
  std::raise(SIGINT);
}

void bar(std::string s) {
  TRACE(s);
  foo();
}

void g(int x) {
  TRACE(x);
  TRACE_OUT(x + 2);
  bar("abc-very long string");
}

int main() {
  INCLUDE_IN_BACKTRACE;
  std::signal(SIGINT, signal_handler);
  try {
    g(3);
  } catch(TracePoint::Exception& exc) {
    cout << exc << endl;
  }
}
````

Generates the following output:
````
Thread(1) ==> /home/joconnor/projects/cpp/internal/template_magic/tracepointtest.cpp:30 - void g(int) x => 3
Thread(1) x + 2 => 5
Thread(1) ==> /home/joconnor/projects/cpp/internal/template_magic/tracepointtest.cpp:25 - void bar(std::__cxx11::string) s => abc-very l...
Thread(1) Caught signal 2 - Stack trace follows:
/home/joconnor/projects/cpp/internal/template_magic/tracepointtest.cpp:11 - void signal_handler(int)
/home/joconnor/projects/cpp/internal/template_magic/tracepointtest.cpp:16 - void foo()
/home/joconnor/projects/cpp/internal/template_magic/tracepointtest.cpp:25 - void bar(std::__cxx11::string)
/home/joconnor/projects/cpp/internal/template_magic/tracepointtest.cpp:30 - void g(int)
/home/joconnor/projects/cpp/internal/template_magic/tracepointtest.cpp:36 - int main()
Thread(1) <== /home/joconnor/projects/cpp/internal/template_magic/tracepointtest.cpp:25 - void bar(std::__cxx11::string)
Thread(1) <== /home/joconnor/projects/cpp/internal/template_magic/tracepointtest.cpp:30 - void g(int)
````
