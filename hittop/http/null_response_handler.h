#ifndef HITTOP_HTTP_NULL_RESPONSE_HANDLER_H
#define HITTOP_HTTP_NULL_RESPONSE_HANDLER_H

#include "hittop/http/response_handler.h"

namespace hittop {
namespace http {

class NullResponseHandler : public ResponseHandler<NullResponseHandler> {
public:
};

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_NULL_RESPONSE_HANDLER_H
