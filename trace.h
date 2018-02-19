//
// Created by joconnor on 15/02/18.
//

#ifndef TRACE_TRACE_H
#define TRACE_TRACE_H
#include <iostream>
#include <thread>
#include "pp_magic.h"

class Trace {
  static void* operator new(size_t); // Don't implement this, but making it private will prevent heap allocation

  static thread_local inline Trace* mostRecentCaller = nullptr;

  Trace* caller;
protected:
  char const* file;
  char const* func;
  int line;

public:
  Trace(char const* file_, char const* func_, int line_)
      : caller{mostRecentCaller}, file{file_}, func{func_}, line{line_}
  {
    mostRecentCaller = this;
  }

  Trace(Trace const&) = delete;
  Trace& operator=(Trace const&) = delete;
  Trace(Trace&&) = delete;
  Trace& operator=(Trace&&) = delete;

  friend std::ostream& operator<<(std::ostream& os, Trace const& trace) {
    os << "Thread(" << std::this_thread::get_id() << ") " << trace.file << ":" << trace.line << " - " << trace.func;
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

struct LoggedTrace : public Trace {
  // The output stream to write tracing output to
  static thread_local inline std::ostream* pos = &std::cout;

  static void set_trace_stream(std::ostream& os) {
    pos = &os;
  }

  template<typename... Ts>
  LoggedTrace(char const* file, char const* func, int line, Ts...args)
      : Trace{file, func, line} {
    *pos << "Thread(" << std::this_thread::get_id() << ") ==> " << file << ":" << line << " - " << func;
    if constexpr (sizeof...(Ts) > 1) {
      *pos << ' ';
      trace_named_values(args...);
    }
    *pos << std::endl;
  }

  ~LoggedTrace() {
    *pos << "Thread(" << std::this_thread::get_id() << ") <== " << file << ":" << line << " - " << func << "\n";
  }


  template<typename... Ts>
  static void output(Ts... args) {
    *pos << "Thread(" << std::this_thread::get_id() << ") ";
    trace_named_values(args...);
    *pos << std::endl;
  }

private:
  static constexpr int max_trace_strlen = 10;

  template <typename T, typename... Ts>
  static void trace_named_values(char const* name, T arg, Ts... args) {
    *pos << name << " => ";
    trace_value(arg);
    if constexpr (sizeof...(Ts) > 1) {
      *pos << ", ";
      trace_named_values(args...);
    }
  };

  template <typename T>
  static void trace_value(T arg) {
    *pos << arg;
  }

  static void trace_value(std::string arg) {
    *pos << std::string_view{&arg[0], arg.length() > max_trace_strlen ? max_trace_strlen : arg.length()};
    if (arg.length() > max_trace_strlen)
      *pos << "...";
  }
};

#define INCLUDE_IN_BACKTRACE Trace _trace(__FILE__, __PRETTY_FUNCTION__, __LINE__)
#define TRACE(...) LoggedTrace _trace(__FILE__, __PRETTY_FUNCTION__, __LINE__ _PP_COMMA_VA(_PP_NAMED_ARGS(__VA_ARGS__)))
#define TRACE_TO(os) LoggedTrace::set_trace_stream(os)
#define TRACE_OUT(...) LoggedTrace::output(_PP_NAMED_ARGS(__VA_ARGS__))
#endif //TRACE_TRACE_H
