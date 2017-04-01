#ifndef HITTOP_URI_BASIC_URI_H
#define HITTOP_URI_BASIC_URI_H

#include <iterator>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "boost/optional.hpp"
#include "boost/range/iterator_range.hpp"
#include "boost/utility/typed_in_place_factory.hpp"

#include "third_party/short_alloc/short_alloc.h"

#include "hittop/util/in_place_alloc_factory.h"

namespace hittop {
namespace uri {

constexpr std::size_t DEFAULT_ARENA_SIZE = 4096;

template <typename T>
using DefaultArenaAllocator = ::short_alloc::short_alloc<T, DEFAULT_ARENA_SIZE>;

template <typename Range, //
          typename SubRange = boost::iterator_range<decltype(
              std::cbegin(std::declval<Range>()))>,
          typename SubRangeSequence =
              std::vector<SubRange, DefaultArenaAllocator<SubRange>>,
          typename SubRangeMap = std::unordered_map<
              SubRange, SubRange, std::hash<SubRange>, std::equal_to<SubRange>,
              DefaultArenaAllocator<std::pair<const SubRange, SubRange>>>,
          typename InPlaceFactoryBuilder = util::AllocFactoryBuilder<
              std::tuple<::short_alloc::arena<DEFAULT_ARENA_SIZE>>>>
class BasicUri {
public:
  using part_type = SubRange;
  using sequence_type = SubRangeSequence;
  using map_type = SubRangeMap;

  BasicUri() : uri_() {}

  BasicUri(const BasicUri &) = delete;

  BasicUri &operator=(const BasicUri &) = delete;

  BasicUri &operator=(const Range &range) {
    uri_ = range;
    return *this;
  }

  BasicUri &operator=(Range &&range) {
    uri_ = std::move(range);
    return *this;
  }

  template <typename... Args> void assign(Args &&... args) {
    uri_ = builder_.template in_place<Range>(std::forward<Args>(args)...);
  }

  auto begin() const { return std::begin(*uri_); }

  auto cbegin() const { return std::cbegin(*uri_); }

  auto end() const { return std::end(*uri_); }

  auto cend() const { return std::cend(*uri_); }

  const boost::optional<SubRange> &scheme() const { return scheme_; }

  template <typename... Args> void assign_scheme(Args &&... args) {
    scheme_ = builder_.template in_place<SubRange>(std::forward<Args>(args)...);
  }

  const boost::optional<SubRange> &user() const { return user_; }

  template <typename... Args> void assign_user(Args &&... args) {
    user_ = builder_.template in_place<SubRange>(std::forward<Args>(args)...);
  }

  void reset_user() { user_ = boost::none; }

  const boost::optional<SubRange> &host() const { return host_; }

  template <typename... Args> void assign_host(Args &&... args) {
    host_ = builder_.template in_place<SubRange>(std::forward<Args>(args)...);
  }

  const boost::optional<unsigned> &port() const { return port_; }

  void assign_port(unsigned p) { port_ = p; }

  const boost::optional<SubRange> &path() const { return path_; }

  template <typename... Args> void assign_path(Args &&... args) {
    path_ = builder_.template in_place<SubRange>(std::forward<Args>(args)...);
  }

  const boost::optional<SubRange> &query() const { return query_; }

  template <typename... Args> void assign_query(Args &&... args) {
    query_ = builder_.template in_place<SubRange>(std::forward<Args>(args)...);
  }

  const boost::optional<SubRange> &fragment() const { return fragment_; }

  template <typename... Args> void assign_fragment(Args &&... args) {
    fragment_ =
        builder_.template in_place<SubRange>(std::forward<Args>(args)...);
  }

  const boost::optional<SubRangeSequence> &path_segments() const {
    return path_segments_;
  }

  SubRangeSequence *mutable_path_segments() {
    if (!path_segments_) {
      path_segments_ = builder_.template in_place<SubRangeSequence>();
    }
    return path_segments_.get_ptr();
  }

  const boost::optional<SubRangeMap> &query_params() const {
    return query_params_;
  }

  SubRangeMap *mutable_query_params() {
    if (!query_params_) {
      query_params_ = builder_.template in_place<SubRangeMap>();
    }
    return query_params_.get_ptr();
  }

private:
  InPlaceFactoryBuilder builder_;
  boost::optional<Range> uri_;
  boost::optional<SubRange> scheme_;
  boost::optional<SubRange> user_;
  boost::optional<SubRange> host_;
  boost::optional<unsigned> port_;
  boost::optional<SubRange> path_;
  boost::optional<SubRange> query_;
  boost::optional<SubRange> fragment_;
  boost::optional<SubRangeSequence> path_segments_;
  boost::optional<SubRangeMap> query_params_;
};

using Uri = BasicUri<std::string, std::string>;

using FastUri = BasicUri<std::string>;

template <typename Iterator>
using ZeroCopyUri = BasicUri<boost::iterator_range<Iterator>>;

} // namespace uri
} // namespace hittop

#endif // HITTOP_URI_BASIC_URI_H
