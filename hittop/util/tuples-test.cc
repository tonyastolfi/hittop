#include "hittop/util/tuples.h"
#include "hittop/util/tuples.h"

#include "gtest/gtest.h"

#include <string>
#include <type_traits>

using hittop::util::tuples::FindFirst;
using hittop::util::tuples::NotFound;

////
static_assert(
    std::is_same<int, FindFirst<std::is_integral,
                                std::tuple<void *, std::string, int, char,
                                           char *>>::type>::value,
    "find integral type in the middle");

static_assert(
    FindFirst<std::is_integral,
              std::tuple<void *, std::string, int, char, char *>>::index == 2,
    "find integral type index in the middle");

////
static_assert(
    std::is_same<NotFound,
                 FindFirst<std::is_integral, std::tuple<void *, std::string,
                                                        char *>>::type>::value,
    "fail to find integral type");

static_assert(FindFirst<std::is_integral,
                        std::tuple<void *, std::string, char *>>::index == 3,
              "fail to find integral type index");

////
static_assert(
    std::is_same<void *, FindFirst<std::is_pointer,
                                   std::tuple<void *, std::string, int, char,
                                              char *>>::type>::value,
    "find pointer type at head");

static_assert(
    FindFirst<std::is_pointer,
              std::tuple<void *, std::string, int, char, char *>>::index == 0,
    "find pointer type at index 0");

////
template <typename T> using is_char_pointer = std::is_same<char *, T>;

static_assert(
    std::is_same<char *, FindFirst<is_char_pointer,
                                   std::tuple<void *, std::string, int, char,
                                              char *>>::type>::value,
    "find char pointer at the end");

static_assert(
    FindFirst<is_char_pointer,
              std::tuple<void *, std::string, int, char, char *>>::index == 4,
    "find char pointer at the end");
