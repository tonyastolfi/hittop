#ifndef HITTOP_HTTP_CONNECTION_DISPOSITION_H
#define HITTOP_HTTP_CONNECTION_DISPOSITION_H

namespace hittop {
namespace http {

enum struct ConnectionDisposition { DEFAULT, KEEP_ALIVE, CLOSE };

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_CONNECTION_DISPOSITION_H
