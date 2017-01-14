#include "hittop/util/load_file_as_string.h"

#include <fstream>
#include <sstream>

namespace hittop {
namespace util {

Fallible<std::string> LoadFileAsString(const char *path) {
  std::ostringstream oss;
  try {
    std::ifstream ifs;
    ifs.exceptions(ifs.exceptions() | std::ios::failbit);
    ifs.open(path);
    oss << ifs.rdbuf();
  } catch (std::system_error &e) {
    return {"", e.code().default_error_condition()};
  }
  return oss.str();
}

} // namespace hittop
} // namespace hittop
