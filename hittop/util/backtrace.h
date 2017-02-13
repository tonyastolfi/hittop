#ifndef HITTOP_UTIL_BACKTRACE_H
#define HITTOP_UTIL_BACKTRACE_H

#include <execinfo.h>
#include <stdio.h>

namespace hittop {
namespace util {

inline void backtrace() {
  // TODO - run these through demangle
  void *callstack[128];
  int i, frames = ::backtrace(callstack, 128);
  printf("depth=%d\n", frames);
  char **strs = ::backtrace_symbols(callstack, frames);
  for (i = 0; i < frames; ++i) {
    printf("%s\n", strs[i]);
  }
  free(strs);
}

} // namespace util
} // namespace hittop

#endif // HITTOP_UTIL_BACKTRACE_H
