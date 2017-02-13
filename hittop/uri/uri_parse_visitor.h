#ifndef HITTOP_URI_URI_PARSE_VISITOR_H
#define HITTOP_URI_URI_PARSE_VISITOR_H

#include <string>

#include "hittop/util/range_to_string.h"

#include "hittop/uri/grammar.h"

namespace hittop {
namespace uri {

template <typename Uri> class UriFieldParseVisitor {
public:
  explicit UriFieldParseVisitor(Uri *uri) : uri_(uri) {}

  template <typename F> void operator()(grammar::scheme, F &&run_parser) const {
    auto result = run_parser();
    if (!result.error()) {
      uri_->assign_scheme(std::begin(result.get()), std::end(result.get()));
    }
  }

  template <typename F>
  void operator()(grammar::userinfo_at, F &&run_parser) const {
    auto result = run_parser([this](grammar::userinfo, auto &&run_parser) {
      auto result = run_parser();
      if (!result.error()) {
        uri_->assign_user(std::begin(result.get()), std::end(result.get()));
      }
    });
    if (result.error()) {
      uri_->reset_user();
    }
  }

  template <typename F> void operator()(grammar::host, F &&run_parser) const {
    auto result = run_parser();
    if (!result.error()) {
      uri_->assign_host(std::begin(result.get()), std::end(result.get()));
    }
  }

  template <typename F> void operator()(grammar::port, F &&run_parser) const {
    auto result = run_parser();
    if (!result.error()) {
      uri_->assign_port(std::stoul(util::RangeToString(result.get())));
    }
  }

  template <typename F> void operator()(grammar::path, F &&run_parser) const {
    auto *segments = uri_->mutable_path_segments();
    auto result = run_parser([segments](grammar::segment, auto &&run_parser) {
      auto result = run_parser();
      if (!result.error()) {
        segments->emplace_back(std::begin(result.get()),
                               std::end(result.get()));
      }
    });
    if (!result.error()) {
      uri_->assign_path(std::begin(result.get()), std::end(result.get()));
    }
  }

  template <typename F> void operator()(grammar::query, F &&run_parser) const {
    // TDOD
    //    auto * params = uri_->mutable_query_params();
    //    auto result = run_parser([params](grammar::);
    auto result = run_parser();
    if (!result.error()) {
      uri_->assign_query(std::begin(result.get()), std::end(result.get()));
    }
  }

  template <typename F>
  void operator()(grammar::fragment, F &&run_parser) const {
    auto result = run_parser();
    if (!result.error()) {
      uri_->assign_fragment(std::begin(result.get()), std::end(result.get()));
    }
  }

private:
  Uri *uri_;
};

template <typename T> auto MakeUriParseVisitor(T *uri) {
  return [uri](grammar::URI_reference, auto &&run_parser) {
    auto result = run_parser(UriFieldParseVisitor<T>{uri});
    if (!result.error()) {
      uri->assign(std::begin(result.get()), std::end(result.get()));
    }
  };
}

} // namespace uri
} // namespace hittop

#endif // HITTOP_URI_URI_PARSE_VISITOR_H
