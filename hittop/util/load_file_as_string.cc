#include "hittop/util/load_file_as_string.h"

#include <fstream>
#include <sstream>

namespace hittop {
namespace util {

Fallible<std::string> LoadFileAsString(const char *path) {
  std::ostringstream oss;
  try {
    std::ifstream ifs;
    const std::ios_base::iostate mask = ifs.exceptions() | std::ios::failbit;
    f.exceptions(mask);
    ifs.open(fileName);
    oss << ifs.rdbuf();
  } catch (std::system_error &e) {
    return {"", e.code()};
  }
  return oss.str();
}

} // namespace hittop
} // namespace hittop
