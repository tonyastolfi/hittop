#include "hittop/util/test_data.h"

#include <cstddef>

#include "gtest/gtest.h"

#include "hittop/util/load_file_as_string.h"

namespace hittop {
namespace util {

std::string GetTestDataPath(const char *workspace_absolute_path) {
  const char *const TEST_SRCDIR = std::getenv("TEST_SRCDIR");
  ASSERT_TRUE(TEST_SRCDIR);

  const char *const TEST_WORKSPACE = std::getenv("TEST_WORKSPACE");
  ASSERT_TRUE(TEST_SRCDIR);

  return std::string(TEST_SRCDIR) + "/" + TEST_WORKSPACE +
         workspace_absolute_path;
}

std::string LoadTestData(const char *workspace_absolute_path) {
  auto maybe = LoadFileAsString(GetTestDataPath(workspace_absolute_path));
  ASSERT_FALSE(maybe.error()) << "Failed to load test data file '"
                              << workspace_absolute_path
                              << "'; error:" << maybe.error().message();
  return maybe.get();
}

} // namespace hittop
} // namespace hittop
