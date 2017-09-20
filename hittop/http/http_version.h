#ifndef HITTOP_HTTP_HTTP_VERSION_H
#define HITTOP_HTTP_HTTP_VERSION_H

#include "boost/operators.hpp"

namespace hittop {
namespace http {

struct HttpVersion : boost::less_than_comparable<HttpVersion>,
                     boost::equality_comparable<HttpVersion> {
  int major = 1;
  int minor = 1;

  bool operator<(const HttpVersion &that) const {
    return this->major < that.major ||
           (this->major == that.major && (this->minor < that.minor));
  }

  bool operator==(const HttpVersion &that) const {
    return major = that.major && minor == that.minor;
  }
};

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_HTTP_VERSION_H
