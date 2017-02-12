#ifndef HITTOP_URI_BASIC_URI_H
#define HITTOP_URI_BASIC_URI_H

#include <iterator>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "boost/optional.hpp"
#include "boost/range/iterator_range.hpp"
#include "boost/utility/typed_in_place_factory.hpp"

#include "third_party/short_alloc/short_alloc.h"

namespace hittop {
namespace uri {

constexpr std::size_t DEFAULT_ARENA_SIZE = 4096;

namespace internal {

template <typename T, typename Arena> struct TypedArenaFactoryBuilder {
  template <typename... Args> auto in_place(Args &&... args) const {
    return boost::in_place<T>(std::forward<Args>(args)..., arena_);
  }

  Arena arena_;
};

template <typename Iterator, typename Arena>
struct TypedArenaFactoryBuilder<boost::iterator_range<Iterator>, Arena> {
  template <typename... Args> auto in_place(Args &&... args) const {
    return boost::in_place<boost::iterator_range<Iterator>>(
        std::forward<Args>(args)...);
  }

  Arena arena_;
};

template <std::size_t kSize> class ArenaFactoryBuilder {
public:
  using Arena = ::short_alloc::arena<kSize>;

  template <typename T, typename... Args> auto in_place(Args &&... args) const {
    return TypedArenaFactoryBuilder<T, Arena &>{arena_}.in_place(
        std::forward<Args>(args)...);
  }

private:
  Arena arena_;
};

} // namespace internal

template <
    typename Range,
    typename SubRange =
        boost::iterator_range<decltype(std::begin(std::declval<Range>()))>,
    typename SubRangeSequence = std::vector<
        SubRange, ::short_alloc::short_alloc<SubRange, DEFAULT_ARENA_SIZE>>,
    typename SubRangeMap = std::unordered_map<
        SubRange, SubRange, std::hash<SubRange>, std::equal_to<SubRange>,
        ::short_alloc<SubRange, DEFAULT_ARENA_SIZE>>,
    typename InPlaceFactoryBuilder = internal::ArenaFactory<DEFAULT_ARENA_SIZE>>
class BasicUri {
public:
  Uri() : uri_() {}

  Uri &operator=(const Range &range) {
    uri_ = range;
    return *this;
  }

  Uri &operator(Range &&range) {
    uri_ = std::move(range);
    return *this;
  }

  template <typename... Args> void assign(Args &&... args) {
    uri_ = builder_.in_place(std::forward<Args>(args)...);
  }

  auto begin() { return std::begin(*range_); }

  auto begin() const { return std::begin(*range_); }

  auto cbegin() const { return std::cbegin(*range_); }

  auto end() { return std::begin(*range_); }

  auto end() const { return std::begin(*range_); }

  auto cend() const { return std::cbegin(*range_); }

  const boost::optional<SubRange> &scheme() const { return scheme_; }

  template <typename... Args> void assign_scheme(Args &&... args) {
    scheme_ = builder_.in_place(std::forward<Args>(args)...);
  }

  const boost::optional<SubRange> &username() const { return username_; }

  template <typename... Args> void assign_username(Args &&... args) {
    username_ = builder_.in_place(std::forward<Args>(args)...);
  }

  const boost::optional<SubRange> &host() const { return host_; }

  template <typename... Args> void assign_host(Args &&... args) {
    host_ = builder_.in_place(std::forward<Args>(args)...);
  }

  const boost::optional<unsigned> &port() const { return port_; }

  void assign_port(unsigned p){port_ = p};

  const boost::optional<SubRange> &path() const { return path_; }

  template <typename... Args> void assign_path(Args &&... args) {
    path_ = builder_.in_place(std::forward<Args>(args)...);
  }

  const boost::optional<SubRange> &query() const { return query_; }

  template <typename... Args> void assign_query(Args &&... args) {
    query_ = builder_.in_place(std::forward<Args>(args)...);
  }

  const boost::optional<SubRange> &fragment() const { return query_; }

  template <typename... Args> void assign_fragment(Args &&... args) {
    fragment_ = builder_.in_place(std::forward<Args>(args)...);
  }

  const boost::optional<SubRangeSequence> &path_segments() const {
    return path_segments_;
  }

  SubRangeSequence *mutable_path_segments() {
    if (!path_segments_) {
      path_segments_ = builder_.in_place();
    }
    return path_segments_.get_ptr();
  }

  const boost::optional<SubRangeMap> &query_params() const {
    return query_params_;
  }

  SubRangeSequence *mutable_query_params() {
    if (!query_params_) {
      query_params_ = builder_.in_place();
    }
    return query_params_.get_ptr();
  }

private:
  InPlaceFactory builder_;
  boost::optional<Range> uri_;
  boost::optional<SubRange> scheme_;
  boost::optional<SubRange> username_;
  boost::optional<SubRange> host_;
  boost::optional<unsigned> port_;
  boost::optional<SubRange> path_;
  boost::optional<SubRange> query_;
  boost::optional<SubRange> fragment_;
  boost::optional<SubRangeSequence> path_segments_;
  boost::optional<SubRangeMap> query_params_;
};

} // namespace uri
} // namespace hittop

#endif // HITTOP_URI_BASIC_URI_H
