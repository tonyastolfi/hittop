#ifndef HITTOP_UTIL_STATIC_STORAGE_H
#define HITTOP_UTIL_STATIC_STORAGE_H

#include <cstddef>
#include <functional>
#include <type_traits>

namespace hittop {
namespace util {

template <std::size_t Size> class StaticStorage {
public:
  StaticStorage(const StaticStorage &) = delete;
  StaticStorage &operator=(const StaticStorage &) = delete;

  ~StaticStorage() { destroy(); }

  template <typename T, typename... Args> T &init(Args &&args...) {
    destroy();
    if (Size < sizeof(T)) {
      T *const value = new T(std::forward<Args>(args)...);
      delete_ = [value]() { delete value; };
      return *value;
    } else {
      T &value = *(new (&storage_) T(std::forward<Args>(args)...));
      delete_ = [ptr = &value]() { ptr->~T(); };
      return value;
    }
  }

  void destroy() {
    if (delete_) {
      delete_();
      delete_ = nullptr;
    }
  }

private:
  std::aligned_storage_t<Size> storage_;
  std::function<void()> delete_;
};

} // namespace util
} // namespace hittop

#endif // HITTOP_UTIL_STATIC_STORAGE_H
