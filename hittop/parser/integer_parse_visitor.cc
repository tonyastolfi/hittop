#include "hittop/parser/integer_parse_visitor.h"

namespace hittop {
namespace parser {

namespace {
bool initialize_xdigit_values() {
  auto &v = base_traits<16>::values;
  for (int n = 0; n < 256; ++n) {
    if ('0' <= n && n <= '9') {
      v[n] = n - '0';
    } else if ('a' <= n && n <= 'f') {
      v[n] = n - 'a' + 0xa;
    } else if ('A' <= n && n <= 'F') {
      v[n] = n - 'A' + 0xa;
    } else {
      v[n] = 0;
    }
  }
  return true;
}
} // namespace

std::array<int, 256> base_traits<16>::values;
bool base_traits<16>::initialized = initialize_xdigit_values();

} // namespace parser
} // namespace hittop
