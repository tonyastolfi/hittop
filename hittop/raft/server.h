// A Raft server state machine.
//
#ifndef HITTOP_RAFT_SERVER_H
#define HITTOP_RAFT_SERVER_H

#include <cstdint>

#include "boost/optional.hpp"

#include "hittop/concurrent/callback_target.h"

#include "hittop/raft/server_id.h"

namespace hittop {
namespace raft {

template <typename Machine, typename Log, typename Storage, typename Channel,
          typename Timer>
class Server : public concurrent::CallbackTarget<
                   Server<Machine, Log, Storage, Channel, Timer>> {
public:
  using command_type = typename Machine::command_type;

  using AppendEntriesHandler =
      std::function<const io::error_code &, uint64_t, bool>;

  using RequestVoteHandler =
      std::function<const io::error_code &, uint64_t, bool>;

  Server(const Server &) = delete;
  Server &operator=(const Server &) = delete;

  void AppendEntries(std::uint64_t term, const ServerId &leader_id,
                     std::uint64_t prev_log_index, std::uint64_t prev_log_term,
                     std::vector<std::unique_ptr<command_type>> entries,
                     std::uint64_t leader_commit,
                     AppendEntriesHandler handler) {}

  void RequestVote(std::uint64_t term, const ServerId &candidate_id,
                   std::uint64_t last_log_index, std::uint64_t last_log_term,
                   RequestVoteHandler handler) {}

private:
  // all the servers in the cluster of which this server is a member
  std::unordered_set<ServerId> all_servers_;

  // the id for this server
  ServerId id_;

  // the state machine implementation
  Machine machine_;

  // the persistent storage driver
  Storage storage_;

  // the RPC channel used to obtain stubs to communicate with other servers
  Channel channel_;

  // the deadline timer used to trigger elections
  Timer timer_;

  // ========== persistent state ===============================================
  //
  // latest term server has seen
  std::uint64_t current_term_;

  // candidate id that received this server's vote in current term
  boost::optional<ServerId> voted_for_;

  // log entries; each entry contains command for state machine and term when
  // entry was received by leader
  Log log_;

  // ========== volatile state on all servers ==================================
  //
  // index of the highest log entry known to be committed.
  std::uint64_t commit_index_;

  // index of highest log entry applied to state machine
  std::uint64_t last_applied_;

  // ========== volatile state on leaders==== ==================================
  //
  struct LeaderState {
    LeaderState(const ServerId &leader, std::uint64_t leader_last_log_index) {
      for (const ServerId &peer : all_servers_) {
        if (peer == leader) {
          continue;
        }
        next_index_[peer] = leader_last_log_index + 1;
        match_index_[peer] = 0;
      }
    }

    // for each server, index of the next log entry to send to that server
    std::unordered_map<ServerId, std::uint64_t> next_index_;

    // for each server, index of highest log entry known to be replicated on
    // server
    std::unordered_map<ServerId, std::uint64_t> match_index_;
  };
  boost::optional<LeaderState> leader_;
};

} // namespace raft
} // namespace hittop

#endif // HITTOP_RAFT_SERVER_H
