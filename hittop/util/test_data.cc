#include "hittop/util/test_data.h"

#include <cstddef>
#include <cstdlib>
#include <exception>

#include "hittop/util/load_file_as_string.h"

namespace hittop {
namespace util {

std::string GetTestDataPath(const char *workspace_absolute_path) {
  const char *const TEST_SRCDIR = std::getenv("TEST_SRCDIR");
  if (!TEST_SRCDIR) {
    throw std::runtime_error("TEST_SRCDIR not defined");
  }

  const char *const TEST_WORKSPACE = std::getenv("TEST_WORKSPACE");
  if (!TEST_WORKSPACE) {
    throw std::runtime_error("TEST_WORKSPACE not defined");
  }

  return std::string(TEST_SRCDIR) + "/" + TEST_WORKSPACE +
         workspace_absolute_path;
}

std::string LoadTestData(const char *workspace_absolute_path) {
  auto maybe =
      LoadFileAsString(GetTestDataPath(workspace_absolute_path).c_str());
  if (maybe.error()) {
    throw std::runtime_error(maybe.error().message());
  }
  return maybe.consume();
}

} // namespace hittop
} // namespace hittop
