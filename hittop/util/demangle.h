#ifndef HITTOP_UTIL_DEMANGLE_H
#define HITTOP_UTIL_DEMANGLE_H

#include <cxxabi.h>

#include <memory>

#include "boost/optional.hpp"

namespace hittop {
namespace util {
namespace abi {

struct free_delete {
  void operator()(void *ptr) const { free(ptr); }
};

inline boost::optional<std::string> demangle(const char *mangled_name) {
  std::unique_ptr<char, free_delete> output(
      ::abi::__cxa_demangle(mangled_name, NULL, NULL, NULL));
  if (output) {
    return {{output.get()}};
  } else {
    return boost::none;
  }
}

inline boost::optional<std::string> demangle(const std::type_info &t) {
  return demangle(t.name());
}

template <typename T> inline boost::optional<std::string> demangle() {
  return demangle(typeid(T));
}

} // namespace abi
} // namespace util
} // namespace hittop

#endif // HITTOP_UTIL_DEMANGLE_H
