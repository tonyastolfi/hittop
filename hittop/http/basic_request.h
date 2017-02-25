#ifndef HITTOP_HTTP_BASIC_MESSAGE_H
#define HITTOP_HTTP_BASIC_MESSAGE_H

#include <algorithm>

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

struct HttpVersion {
  int major = 1;
  int minor = 1;
};

enum struct HttpMethod {
  UNKNOWN,
  CONNECT,
  DELETE,
  GET,
  HEAD,
  POST,
  PUT,
  TRACE
};

template <typename Range>
using GenericMethod = boost::variant<HttpMethod, Range>;

struct HttpMethodVisitor : boost::static_visitor<HttpMethod> {
  HttpMethod operator()(const HttpMethod &m) const { return m; }

  template <typename Range> HttpMethod operator()(const Range &) const {
    return HttpMethod::UNKNOWN;
  }
};

template <typename Range>
struct MethodNameVisitor : boost::static_visitor<Range> {
  Range operator()(const Range &r) const { return r; }

  Range operator()(const HttpMethod &m) const {
    static const std::string names[] = {
        "UNKNOWN", "CONNECT", "DELETE", "GET", "HEAD", "POST", "PUT", "TRACE"};
    return Range(n[m].begin(), n[m].end());
  }
};

template <typename Range, //
          typename SubRange = DefaultSubRange<Range>,
          template <typename> class Sequence = DefaultArenaVector,
          typename InPlaceFactoryBuilder = DefaultInPlaceFactoryBuilder>
class BasicRequest {
public:
  using Uri =
      BasicUrl<SubRange, SubRange, DefaultArenaVector<SubRange>,
               DefaultArenaMap<SubRange, SubRange>, InPlaceFactoryBuilder>;

  template <typename... Args> void assign(Args &&... args) {
    range_ = builder_.in_place(std::forward<Args>(args));
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

  SubRange method_name() const {
    return boost::apply_visitor(MethodNameVisitor<SubRange>{});
  }

  void set_major_version(int major) const { version_.major = major; }

  void set_minor_version(int minor) const { version_.minor = minor; }

  void set_http_version(const HttpVersion &version) { version_ = version; }

  auto &version() const { return version_; }

  // TODO: set_uri - parse a range into the uri_ field

  auto mutable_uri() { return &uri_; }

  auto &uri() const { return uri_; }

  auto mutable_headers() { return &headers_; }

  auto &headers() const { return &headers_; }

private:
  InPlaceFactoryBuilder builder_;
  boost::optional<Range> range_;
  GenericMethod<SubRange> method_;
  Uri uri_;
  HttpVersion version_;
  boost::optional<Sequence<BasicHeader<SubRange>>> opt_headers_{
      builder_.in_place()};
  Sequence<BasicHeader<SubRange>> &headers_ = *opt_headers_;
};
g
} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_BASIC_MESSAGE_H
