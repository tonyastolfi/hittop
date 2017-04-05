#ifndef HITTOP_UTIL_SWAP_AND_INVOKE_H
#define HITTOP_UTIL_SWAP_AND_INVOKE_H

#include <memory>

namespace hittop {
namespace util {

template <typename F, typename... Args>
auto SwapAndInvoke(F &f, Args &&... args) {
  F local_copy;
  f.swap(local_copy);
  return local_copy(std::forward<Args>(args)...);
}

} // namespace util
} // namespace hittop

#endif // HITTOP_UTIL_SWAP_AND_INVOKE_H
