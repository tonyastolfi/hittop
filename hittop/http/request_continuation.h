#ifndef HITTOP_HTTP_REQUEST_CONTINUATION_H
#define HITTOP_HTTP_REQUEST_CONTINUATION_H

namespace hittop {
namespace http {

template <typename T> struct RequestContinuation {
  RequestContinuation() {
    // must be callable like:
    //  void(AsyncConstBufferStream*, ResponseConsumer, CompletionHandler)
  }
};

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_REQUEST_CONTINUATION_H
