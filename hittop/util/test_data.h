#ifndef HITTOP_UTIL_TEST_DATA_H
#define HITTOP_UTIL_TEST_DATA_H

#include "hittop/util/fallible.h"

#include <string>

namespace hittop {
namespace util {

std::string GetTestDataPath(const char *workspace_absolute_path);

std::string LoadTestData(const char *workspace_absolute_path);

} // namespace hittop
} // namespace hittop

#endif // HITTOP_UTIL_TEST_DATA_H
