#ifndef HITTOP_HTTP_REQUEST_H
#define HITTOP_HTTP_REQUEST_H

#include "hittop/http/basic_request.h"

namespace hittop {
namespace http {

template <typename Iterator>
using ZeroCopyRequest = BasicRequest<boost::iterator_range<Iterator>>;

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_REQUEST_H
