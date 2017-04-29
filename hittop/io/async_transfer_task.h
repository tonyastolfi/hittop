#ifndef HITTOP_IO_ASYNC_TRANSFER_TASK_H
#define HITTOP_IO_ASYNC_TRANSFER_TASK_H

#include "hittop/concurrent/async_task.h"
#include "hittop/util/construct_from_tuple.h"

#include "hittop/io/types.h"

namespace hittop {
namespace io {

template <typename Buffer, typename Stream, typename Derived>
class AsyncTransferTask : public concurrent::AsyncTask<Derived> {
public:
  using CompletionHandler = concurrent::AsyncTaskBase::CompletionHandler;

  template <typename BufferArgsTuple, typename StreamArgsTuple>
  AsyncTransferTask(BufferArgsTuple &&buffer_args,
                    StreamArgsTuple &&stream_args)
      : buffer_(std::forward<BufferArgsTuple>(buffer_args)),
        stream_(std::forward<StreamArgsTuple>(stream_args)) {}

  void AsyncRun(CompletionHandler handler) {
    handler_ = std::move(handler);
    TransferNext();
  }

protected:
  void Complete(const error_code &ec) { SwapAndInvoke(handler_, ec); }

  util::ConstructFromTuple<Buffer> buffer_;
  util::ConstructFromTuple<Stream> stream_;

private:
  Derived *derived() { return static_cast<Derived *>(this); }

  void TransferNext() {
    derived()->pre_transfer([this](const error_code &ec, const auto &buffers) {
      if (ec) {
        Complete(ec);
        return;
      }

      derived()->transfer(buffers, [this](const error_code &ec,
                                          const std::size_t bytes_transferred) {
        derived()->post_transfer(bytes_transferred);
        if (ec) {
          Complete(ec);
        } else {
          TransferNext();
        }
      });
    });
  }

  CompletionHandler handler_;
};

} // namespace io
} // namespace hittop

#endif // HITTOP_IO_ASYNC_TRANSFER_TASK_H
