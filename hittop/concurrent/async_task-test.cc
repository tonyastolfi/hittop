#include "hittop/concurrent/async_task.h"
#include "hittop/concurrent/async_task.h"

namespace {

using ::hittop::concurrent::AsyncTask;

struct BoringTask : AsyncTask<BoringTask> {};

} // namespace
