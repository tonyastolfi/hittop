#include "hittop/util/is_callable.h"
#include "hittop/util/is_callable.h"

#include <string>

using hittop::util::IsCallable;

namespace {
using fun0 = void (&)();
using fun2 = void (&)(int, int);
using fun3 = void (&)(char *, int, std::string);
}

static_assert(IsCallable<fun0>::value, "nullary is callable with no args");

static_assert(!IsCallable<fun0, int>::value,
              "nullary is not callable with >1 arg");

static_assert(IsCallable<fun2, int, int>::value, "callable with exact types");

static_assert(IsCallable<fun2, char, short>::value,
              "callable with implicit up-converts");

static_assert(!IsCallable<fun2, int>::value, "not callable -- too few");

static_assert(!IsCallable<fun2, int, int, int>::value,
              "not callable -- too many");

static_assert(!IsCallable<fun2, int, std::string>::value,
              "not callable -- non-convertible types");

namespace {
struct Five {
  operator int() const { return 5; }
};
} // namespace

static_assert(IsCallable<fun3, decltype(nullptr), Five, const char *>::value,
              "callable -- user defined and polymorphic conversions");
