#ifndef HITTOP_CONCURRENT_PARENT_STARTABLE_H
#define HITTOP_CONCURRENT_PARENT_STARTABLE_H

#include "hittop/concurrent/async_parent_task.h"
#include "hittop/concurrent/startable.h"

namespace hittop {
namespace concurrent {

template <typename Derived, typename Base = AsyncTaskBase>
class ParentStartable : public AsyncParentTask<Derived, Base> {
  friend class AsyncParentTask<Derived, Base>;

public:
  using super_type = AsyncParentTask<Derived, Base>;

  using CompletionHandler = typename super_type::CompletionHandler;

  using StartHandler = Startable::StartHandler;

  virtual ~ParentStartable() {}

protected:
  ParentStartable(boost::asio::io_service &io, StartHandler start_handler)
      : super_type(io), root_(std::move(start_handler), *this) {}

  // This function must be implemented to specify the startup logic for this
  // type; the passed completion handler must be invoked when the object is
  // started (not when it is finished; see Finished below).
  virtual void Start(const CompletionHandler &started) = 0;

  // Derived classes may override this method to provide a way to stop or cancel
  // a running task.  Overrides MUST handle the case where Stop is invoked
  // after the derived class calls Finished.
  virtual void Stop() {}

  // This will cause the root task to complete, possibly terminating the this as
  // well.
  void Finished(const error_code &ec) { root_.Finished(ec); }

  // Returns whether Finished has been called.  This function is not safe to
  // call concurrently with Finished.  It's up to the derived class to implement
  // whatever synchronization strategy is appropriate for Stop, Finished,
  // IsFinished, and any implementation state.
  //
  bool IsFinished() const { return root_.IsFinished(); }

  // NOTE: derived classes may also make use of `Spawn(task, completion)` and
  // `strand_` from AsyncParentTask.

private:
  class RootTask : public Startable {
    friend class ParentStartable;

  public:
    template <typename F>
    RootTask(F &&start_handler, ParentStartable &parent)
        : Startable(std::forward<F>(start_handler)), parent_(parent) {}

  protected:
    void Start(const CompletionHandler &started) { parent_.Start(started); }

    void Stop() { parent_.Stop(); }

  private:
    ParentStartable &parent_;
  };

  void OnRun() {
    this->Spawn(root_, [](auto &&...) {
      // TODO - is there anything we need to do here??
    });
  }

  RootTask root_;
};

} // namespace concurrent
} // namespace hittop

#endif // HITTOP_CONCURRENT_PARENT_STARTABLE_H
