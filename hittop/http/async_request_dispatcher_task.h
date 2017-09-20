#ifndef HITTOP_HTTP_ASYNC_REQUEST_DISPATCHER_TASK_H
#define HITTOP_HTTP_ASYNC_REQUEST_DISPATCHER_TASK_H

namespace hittop {
namespace http {

template <typename BufferStream, typename RequestHandlerFactory,
          typename ResponseConsumerFactory>
class AsyncRequestDispatcherTask
    : public concurrent::AsyncTask<AsyncRequestDispatcherTask> {
public:
  static const std::size_t kMaxRequestSize = 1 << 14;

  AsyncRequestDispatcherTask(BufferStream &input_buffer,
                             RequestHandlerFactory &request_handler_factory,
                             ResponseConsumerFactory &response_consumer_factory)
      : stream_(input_buffer), make_request_handler_(request_handler_factory),
        make_response_handler_(response_consumer_factory) {}

  void AsyncRun(CompletionHandler completion_handler_arg) {
    completion_handler_ = std::move(completion_handler_arg);
    ReadNextRequest(1);
  }

private:
  using RequestHandlerType = decltype(make_request_handler_());

  // Pull at least 'min_count' bytes from the buffer stream and try to parse the
  // result as an HTTP request.
  //
  void ReadNextRequest(std::size_t min_count) {
    stream_.async_fetch(min_count, [this](const io::error_code &ec,
                                          const auto &buffers) {
      // Fetch errors are fatal.
      if (ec) {
        Complete(ec);
        return;
      }

      // See how many buffers we have; we always parse from a single contiguous
      // memory region.  If there is more than one buffer then make a local copy
      // of the data.
      //
      const auto first = std::begin(buffers);
      const auto last = std::end(buffers);
      const std::size_t num_buffers = std::distance(first, last);
      if (num_buffers == 1) {
        TryParseRequest(buffer_cast<const char *>(*first), buffer_size(*first));
      } else {
        std::array<char, kMaxRequestSize> local_data;
        auto write_pos = local_data.begin();
        const auto end = local_data.end();
        for (const io::const_buffer &b : buffers) {
          const std::size_t bytes_to_copy =
              std::min(buffer_size(b), std::distance(write_pos, end));
          const char *const b_ptr = buffer_cast<const char *>(b);
          std::copy(b_ptr, b_ptr + bytes_to_copy, write_pos);
        }
        TryParseRequest(local_data.data(),
                        std::distance(local_data.begin(), write_pos));
      }
    });
  }

  void TryParseRequest(const char *const data, const std::size_t size) {
    // Create a new handler for this request.
    request_handler_ = make_request_handler_();

    // TODO - inject a proxy request_handler that intercepts the things we're
    // interested in: Expect:

    // Try to parse the request.
    auto parse_result = parser::Parse<grammar::Request>(
        boost::make_iterator_range(data, data + size),
        RequestParseVisitor<RequestHandlerType>{&request_handler_});

    // Not enough data to complete the parse; fetch more from the buffer stream.
    if (parse_result.error() == parser::ParseError::INCOMPLETE) {
      stream_.consume(0);
      ReadNextRequest(size + 1);
      return;
    }

    // Parse error - this connection is over, man!
    if (!parse_result.ok()) {
      stream_.consume(0);
      Complete(boost::asio::error::invalid_argument);
      return;
    }

    const std::size_t parsed_size = std::distance(data, result.get());

    // Parse complete!
    request_handler_.async_validate(
        [this, parsed_size](const io::error_code &ec, auto &&continue_request) {
          stream_.consume(parsed_size);

          // TODO - if Expect: 100-continue then send the continue response
          // here.

          // Failed to validate!  Return error response to client and close the
          // connection.
          if (ec) {
            // TODO!!
            Complete(ec);
            return;
          }

          continue_request(&stream_, make_response_consumer_(),
                           [this](const io::error_code &ec) {
                             if (ec) {
                               Complete(ec);
                               return;
                             }
                             ReadNextRequest(1);
                           });
        });
  }

  void Complete(const io::error_code &ec) {
    SwapAndInvoke(completion_handler_, ec);
  }

  BufferStream &stream_;
  RequestHandlerFactory &make_request_handler_;
  // void ConsumeResponse(void ProduceResponse(ResponseHandler*))
  ResponseConsumerFactory &make_response_handler_;
  CompletionHandler completion_handler_;
  RequestHandlerType request_handler_;
};

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_ASYNC_REQUEST_DISPATCHER_TASK_H
