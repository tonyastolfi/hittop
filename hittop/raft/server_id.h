// DESCRIPTION
//
#ifndef HITTOP_RAFT_SERVER_ID_H
#define HITTOP_RAFT_SERVER_ID_H

#include "boost/uuid/uuid.hpp"

namespace hittop {
namespace raft {

using ServerId = boost::uuids::uuid;

} // namespace raft
} // namespace hittop

#endif // HITTOP_RAFT_SERVER_ID_H
