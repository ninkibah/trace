//
// Created by joconnor on 15/02/18.
//

#ifndef TRACE_TRACE_H
#define TRACE_TRACE_H
#include <iostream>

class Trace {
  static inline Trace* mostRecentCaller = nullptr;

  Trace* caller;
  char const* file;
  char const* func;
  int line;

public:
  Trace(char const* file_, char const* func_, int line_)
      : caller{mostRecentCaller}, file{file_}, func{func_}, line{line_}
  {
    mostRecentCaller = this;
  }

  friend std::ostream& operator<<(std::ostream& os, Trace const& trace) {
    os << trace.file << ":" << trace.line << " - " << trace.func;
    return os;
  }

  static void printStackTrace(std::ostream& os) {
    for (Trace const* ptp = mostRecentCaller; ptp != nullptr; ptp = ptp->caller)
      os << *ptp << "\n";
  }

  ~Trace() {
    if (std::uncaught_exceptions() > 0) {
      std::cout << "Exception thrown: " << *this << "\n";
    }
    mostRecentCaller = caller;
  }
};

#define TRACE Trace trace(__FILE__, __PRETTY_FUNCTION__, __LINE__)
#endif //TRACE_TRACE_H
