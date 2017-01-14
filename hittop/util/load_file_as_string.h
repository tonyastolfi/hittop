#ifndef HITTOP_UTIL_LOAD_FILE_AS_STRING_H
#define HITTOP_UTIL_LOAD_FILE_AS_STRING_H

#include "hittop/util/fallible.h"

#include <string>

namespace hittop {
namespace util {

Fallible<std::string> LoadFileAsString(const char *path);

} // namespace
} // namespace hittop

#endif // HITTOP_UTIL_LOAD_FILE_AS_STRING_H
