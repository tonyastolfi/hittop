#ifndef HITTOP_HTTP_HTTP_1_1_BASIC_REQUEST_FILTER_H
#define HITTOP_HTTP_HTTP_1_1_BASIC_REQUEST_FILTER_H

#include "hittop/http/chunk_decoder.h"
#include "hittop/http/request_handler.h"
#include "hittop/io/limited_async_const_buffer_stream.h"
#include "hittop/util/static_storage.h"

namespace hittop {
namespace http {

template <typename DelegateHandler>
class Http_1_1_BasicRequestFilter
    : public RequestHandler<Http_1_1_BasicRequestFilter<DelegateHandler>> {
public:
  Http_1_1_BasicRequestFilter(const Http_1_1_BasicRequestFilter &) = delete;

  Http_1_1_BasicRequestFilter &
  operator=(const Http_1_1_BasicRequestFilter &) = delete;

  template <typename... Args>
  explicit Http_1_1_BasicRequestFilter(Args &&... args)
      : delegate_handler_(std::forward<Args>(args)...) {}

  template <typename ValidateHandler>
  void AsyncValidate(ValidateHandler &&validate_handler) {
    const bool request_delimited = http_version_is_1_0_ || !keep_alive_ ||
                                   content_length_ ||
                                   transfer_encoding_chunked_;

    if (!request_delimited) {
      std::forward<ValidateHandler>(validate_handler)(
          boost::asio::error::invalid_argument, [](auto &&...) {});
    } else {
      delegate_handler_.AsyncValidate([
        this, validate_handler = std::forward<ValidateHandler>(validate_handler)
      ](const io::error_code &ec, auto &&continue_request) {
        if (ec) {
          validate_handler(ec);
          return;
        }

        validate_handler(ec, [this, continue_request](
                                 auto *raw_stream, auto &&response_consumer,
                                 auto &&request_completion_handler) {
          using RawStreamRef = std::decay_t<decltype(*raw_stream)> &;

          if (transfer_encoding_chunked_) {
            // Wrap the stream in a chunk decoder.
            auto &decoder =
                message_body_storage_
                    .construct<http::ChunkDecoder<RawStreamRef>>(*raw_stream);

            continue_request(&decoder, response_consumer,
                             request_completion_handler);

          } else if (!http_version_is_1_0_ && keep_alive_ && content_length_) {
            // Wrap the stream in a limiter.
            auto &limiter =
                message_body_storage_
                    .construct<io::LimitedAsyncConstBufferStream<RawStreamRef>>(
                        *raw_stream, *content_length_);

            continue_request(&limiter, response_consumer,
                             request_completion_handler);
          } else {
            // Pass the raw stream.
            continue_request(raw_stream, response_consumer,
                             request_completion_handler);
          }
        });
      });
    }
  }

private:
  bool http_version_is_1_0_ = false;
  bool keep_alive_ = true;
  boost::optional<std::size_t> content_length_;
  bool transfer_encoding_chunked_ = false;
  DelegateHandler delegate_handler_;
  StaticStorage<512> message_body_storage_;
};

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_HTTP_1_1_BASIC_REQUEST_FILTER_H
