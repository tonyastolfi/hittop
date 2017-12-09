#ifndef HITTOP_HTTP_BASIC_REQUEST_H
#define HITTOP_HTTP_BASIC_REQUEST_H

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "boost/range/iterator_range.hpp"
#include "boost/variant.hpp"

#include "third_party/short_alloc/short_alloc.h"

#include "hittop/http/basic_header.h"
#include "hittop/http/http_method.h"
#include "hittop/http/http_version.h"
#include "hittop/http/mutable_request.h"
#include "hittop/uri/basic_uri.h"
#include "hittop/util/boost_iterator_range_helper.h"
#include "hittop/util/in_place_alloc_factory.h"

namespace hittop {
namespace http {

constexpr std::size_t DEFAULT_ARENA_SIZE = 4096;

template <typename Range>
using DefaultSubRange =
    boost::iterator_range<decltype(std::cbegin(std::declval<Range>()))>;

template <typename T>
using DefaultArenaAllocator = ::short_alloc::short_alloc<T, DEFAULT_ARENA_SIZE>;

template <typename T>
using DefaultArenaVector = std::vector<T, DefaultArenaAllocator<T>>;

template <typename K, typename V>
using DefaultArenaMap =
    std::unordered_map<K, V, std::hash<K>,
                       DefaultArenaAllocator<std::pair<const K, V>>>;

using DefaultInPlaceFactoryBuilder = util::AllocFactoryBuilder<
    std::tuple<::short_alloc::arena<DEFAULT_ARENA_SIZE>>>;

template <typename Range>
using GenericMethod = boost::variant<HttpMethod, Range>;

struct HttpMethodVisitor : boost::static_visitor<HttpMethod> {
  HttpMethod operator()(const HttpMethod &m) const { return m; }

  template <typename Range> HttpMethod operator()(const Range &) const {
    return HttpMethod::UNKNOWN;
  }
};

struct MethodNameVisitor : boost::static_visitor<std::string> {
  template <typename Range> std::string operator()(const Range &r) const {
    return {std::begin(r), std::end(r)};
  }

  std::string operator()(const HttpMethod m) const {
    static const std::string names[] = {
        "UNKNOWN", "CONNECT", "DELETE", "GET", "HEAD", "POST", "PUT", "TRACE"};
    const std::size_t index = static_cast<std::size_t>(m);
    return names[index];
  }
};

template <typename Range, //
          typename SubRange = DefaultSubRange<Range>,
          template <typename> class Sequence = DefaultArenaVector,
          typename InPlaceFactoryBuilder = DefaultInPlaceFactoryBuilder>
class BasicRequest
    : public MutableRequest<
          BasicRequest<Range, SubRange, Sequence, InPlaceFactoryBuilder>> {
public:
  using FieldName = SubRange;
  using FieldValue = SubRange;

  using Uri =
      uri::BasicUri<SubRange, SubRange, DefaultArenaVector<SubRange>,
                    DefaultArenaMap<SubRange, SubRange>, InPlaceFactoryBuilder>;

  template <typename... Args> void assign(Args &&... args) {
    range_ = builder_.template in_place<Range>(std::forward<Args>(args)...);
  }

  auto begin() { return std::begin(range_); }

  auto end() { return std::end(range_); }

  auto begin() const { return std::begin(range_); }

  auto end() const { return std::end(range_); }

  auto cbegin() const { return std::begin(range_); }

  auto cend() const { return std::end(range_); }

  void set_extension_method(const SubRange &extension_method) {
    method_ = extension_method;
  }

  void set_http_method(const HttpMethod http_method) { method_ = http_method; }

  bool is_extension_method() const { return method_.which() == 1; }

  auto &method() const { return method_; }

  HttpMethod http_method() const {
    return boost::apply_visitor(HttpMethodVisitor{}, method_);
  }

  auto method_name() const {
    return boost::apply_visitor(MethodNameVisitor{}, method_);
  }

  void set_major_version(int major) { version_.major = major; }

  void set_minor_version(int minor) { version_.minor = minor; }

  void set_http_version(const HttpVersion &version) { version_ = version; }

  auto &version() const { return version_; }

  // TODO: set_uri - parse a range into the uri_ field

  auto mutable_uri() { return &uri_; }

  auto &uri() const { return uri_; }

  auto mutable_headers() { return &headers_; }

  auto &headers() const { return headers_; }

  auto &header(std::size_t index) const { return headers_[index]; }

private:
  using Headers = Sequence<BasicHeader<SubRange>>;

  InPlaceFactoryBuilder builder_;
  boost::optional<Range> range_;
  GenericMethod<SubRange> method_;
  Uri uri_;
  HttpVersion version_;
  boost::optional<Headers> opt_headers_{builder_.template in_place<Headers>()};
  Headers &headers_ = *opt_headers_;
};

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_BASIC_REQUEST_H
