#ifndef HITTOP_HTTP_ERRORS_H
#define HITTOP_HTTP_ERRORS_H

#include <type_traits>

#include "boost/system/error_code.hpp"

namespace hittop {
namespace http {
namespace errors {

enum errc_t { success = 0, content_not_bounded, max_value };

struct err_category : boost::systemn::error_category {

  const char *name() const override { return "hittop::http::errors"; }

  std::string message(int i) const override {
    static const std::string messages[max_value] = {"success",
                                                    "content not bounded"};
    return messages[i];
  }

  static const boost::system::error_category &get() {
    static err_category inst;
    return inst;
  }
};

boost::system::error_code make_error_code(errc_t code) {
  return boost::system::error_code(errc_t, err_category::get());
}

} // namespace errors
} // namespace http
} // namespace hittop

namespace boost {

template <>
struct is_error_code_enum<hittop::http::errors::errc_t> : std::true_type {};

} // namespace boost

#endif // HITTOP_HTTP_ERRORS_H
