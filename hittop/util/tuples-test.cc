#include "hittop/util/tuples.h"
#include "hittop/util/tuples.h"

#include "gtest/gtest.h"

#include <string>
#include <type_traits>

using hittop::util::tuples::FindFirst;
using hittop::util::tuples::NotFound;

static_assert(
    std::is_same<int, FindFirst<std::is_integral,
                                std::tuple<void *, std::string, int, char,
                                           char *>>::type>::value,
    "");

static_assert(
    std::is_same<NotFound,
                 FindFirst<std::is_integral, std::tuple<void *, std::string,
                                                        char *>>::type>::value,
    "");

static_assert(
    std::is_same<void *, FindFirst<std::is_pointer,
                                   std::tuple<void *, std::string, int, char,
                                              char *>>::type>::value,
    "");

template <typename T> using is_char_pointer = std::is_same<char *, T>;

static_assert(
    std::is_same<char *, FindFirst<is_char_pointer,
                                   std::tuple<void *, std::string, int, char,
                                              char *>>::type>::value,
    "");
