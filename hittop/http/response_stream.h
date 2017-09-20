#ifndef HITTOP_HTTP_RESPONSE_STREAM_H
#define HITTOP_HTTP_RESPONSE_STREAM_H

#include <algorithm>
#include <cinttypes>
#include <cstring>

#include "boost/range/algorithm/equal.hpp"
#include "boost/range/size.hpp"

#include "hittop/http/errors.h"
#include "hittop/util/construct_from_tuple.h"
#include "hittop/util/static_storage.h"

namespace hittop {
namespace http {

template <typeanme AsyncWriteStream> class ResponseStream {
public:
  ResponseStream(const ResponseStream &) = delete;
  ResponseStream &operator=(const ResponseStream &) = delete;

  template <typename AsyncWriteStreamTuple>
  explicit ResponseStream(AsyncWriteStreamTuple &&awst)
      : sink_(std::forward<AsyncWriteStreamTuple>(awst)) {
    status_line_buffer_.resize()
  }

  template <typename ApplyCompletionHandler>
  ResponseHandlerImpl<ApplyCompletionHandler> *
  StartResponse(ApplyCompletionHandler apply_completion_handler) {
    return &response_handler_.init<ResponseHandlerImpl<ApplyCompletionHandler>>(
        this, std::move(apply_completion_handler));
  }

  template <typename AsyncConstBufferStream, typename CompletionHandler>
  void WriteBody(AsyncConstBufferStream *content, CompletionHandler complete) {}

  template <typename ApplyCompletionHandler>
  class ResponseHandlerImpl : public ResponseHandler<ResponseHandlerImpl> {
  public:
    using mutable_headers_type = ResponseHandlerImpl;

    ResponseHandlerImpl(ResponseStream *that,
                        ApplyCompletionHandler &&apply_completion_handler)
        : that_(that),
          apply_completion_handler_(std::move(apply_completion_handler)) {
      set_major_version(1);
      set_minor_version(1);
      set_status_code(200);
    }

    void set_major_version(int major) {
      major = std::max(0, std::min(major, 9));
      major_version_ = major;
      that_->status_line_buffer_[5] = '0' + major;
    }

    void set_minor_version(int minor) {
      minor = std::max(0, std::min(minor, 9));
      minor_version_ = minor;
      that_->status_line_buffer_[7] = '0' + minor;
    }

    void set_http_version(const HttpVersion &version) {
      set_major_version(version.major);
      set_minor_version(version.minor);
    }

    void set_status_code(int code) {
      code = std::max(100, std::min(code, 599));
      that_->status_line_buffer_[9] = '0' + (code / 100);
      that_->status_line_buffer_[10] = '0' + (code / 10) % 10;
      that_->status_line_buffer_[11] = '0' + code % 10;
    }

    template <typename CharRange>
    void set_reason_phrase(const CharRange &reason) {
      const std::size_t pos = status_line_form().length();
      that_->status_line_buffer_.resize(pos + boost::size(reason));
      std::copy(std::begin(reason), std::end(reason),
                that_->status_line_buffer_.begin() + pos);
    }

    void set_connection_disposition(ConnectionDisposition connection) {
      static const std::string connection_name = "Connection" connection_ =
          connection;
      switch (connection_) {
      case ConnectionDisposition::KEEP_ALIVE:
        mutable_headers()->emplace_back(connection_name, "keep-alive");
        break;
      case ConnectionDisposition::CLOSE:
        mutable_headers()->emplace_back(connection_name, "close");
        break;
      case ConnectionDisposition::DEFAULT:
      default:
        break;
      };
    }

    void set_content_length(std::uint64_t length) { content_length_ = length; }

    template <typename CharRange>
    void set_transfer_encoding(CharRange &&transfer_encoding) {
      const std::string chunked = "chunked";
      const std::string transfer_encoding_name = "Transfer-Encoding";
      chunked_encoding_ = boost::equal(chunked, transfer_encoding);
      mutable_headers()->emplace_back(transfer_encoding_name,
                                      transfer_encoding);
    }

    auto *mutable_headers() { return this; }

    template <typename NameRange, typename ValueRange>
    void emplace_back(NameRange &&name, ValueRange &&value) {
      static const std::string colon = ": ";
      static const std::string crlf = "\r\n";
      const std::size_t insert_pos = that_->headers_buffer_.size();

      that_->headers_buffer_.resize(insert_pos + boost::size(name) +
                                    boost::size(colon) + boost::size(value) +
                                    boost::size(crlf));

      const auto name_pos = that_->headers_buffer_.begin() + insert_pos;

      const auto colon_pos =
          std::copy(std::begin(name), std::end(name), name_pos);

      const auto value_pos =
          std::copy(std::begin(colon), std::end(colon), colon_pos);

      const auto crlf_pos =
          std::copy(std::begin(value), std::end(value), value_pos);

      std::copy(std::begin(crlf), std::end(crlf), crlf_pos);
    }

    template <typename CharRange>
    void push_back(const BasicHeader<CharRange> &header) {
      emplace_back(header.name, header.value);
    }

    template <typename ValidationHandler>
    void async_validate(ValidationHandler &&handler) {
      // ...check stuff here...
      // We must be able to tell the extent of the response body.  We can tell
      // that length if one of the following is true:
      //  - HTTP version is <= 1.0
      //  - Content-Length is given
      //  - Transfer-Encoding is "chunked"
      //  - Connection is "close"
      //
      // If all these are false, then signal error to the caller.
      //
      if (HttpVersion{major_version_, minor_version_} > HttpVersion{1, 0} &&
          !content_length_ && !chunked_encoding_ &&
          connection_ != ConnectionDisposition::CLOSE) {
        apply_handler(handler, errors::content_not_bounded, [](auto...) {});
        return;
      }

      handler({}, [apply_handler = std::move(apply_completion_handler_)](
                      auto *body_buffer_stream, auto &&body_written) {
        // TODO - wrap the response_buffer_ in an AsyncConstBufferStream and
        // prepend to body_buffer_stream to produce a new AsyncConstBufferStream
        // to pass to the writer.
        writer_
            .init<
                AsyncWriter<decltype(*body_buffer_stream), AsyncWriteStream &>>(
                std::make_tuple(*body_buffer_stream),
                std::make_tuple(std::ref(sink_)))
            .AsyncRun([
              body_written = std::move(body_written),
              apply_handler = std::move(apply_handler)
            ](const io::error_code &ec) { apply_handler(body_written, ec); });
        // TBD - we're lazily destorying the writer task; is this ok?
      });
    }

  private:
    ResponseStream *const that_;
    int major_version_;
    int minor_version_;
    boost::optional<std::uint64_t> content_length_;
    bool chunked_encoding_ = false;
    ConnectionDisposition connection_ = ConnectionDisposition::DEFAULT;
    ApplyCompletionHandler apply_completion_handler_;
  };

private:
  const std::string &status_line_form() {
    static const std::string s = "HTTP/1.1 200 ";
    return s;
  }

  util::StaticStorage<64> response_handler_;
  util::ConstructFromTuple<AsyncWriteStream> sink_;
  std::vector<char> status_line_buffer_;
  std::vector<char> headers_buffer_;
  util::StaticStorage<512> writer_;
};

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_RESPONSE_STREAM_H
