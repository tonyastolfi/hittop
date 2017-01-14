#ifndef HITTOP_UTIL_SCOPE_EXIT_H
#define HITTOP_UTIL_SCOPE_EXIT_H

#include <functional>

namespace hittop {
namespace util {

class ScopeExit {
public:
  explicit ScopeExit(std::function<void()> f) : f_{std::move(f)} {}

  ScopeExit(const ScopeExit &) = delete;
  ScopeExit &operator=(const ScopeExit &) = delete;

  ~ScopeExit() { f_(); }

  void disarm() {
    f_ = []() {};
  }

private:
  std::function<void()> f_;
};

} // namespace util
} // namespace hittop

#endif // HITTOP_UTIL_SCOPE_EXIT_H
