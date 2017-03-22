#ifndef HITTOP_HTTP_REQUEST_PARSE_VISITOR_H
#define HITTOP_HTTP_REQUEST_PARSE_VISITOR_H

#include "hittop/http/basic_request.h"
#include "hittop/http/grammar.h"

#include "hittop/parser/integer_parse_visitor.h"
#include "hittop/uri/uri_parse_visitor.h"
#include "hittop/util/first_match.h"

namespace hittop {
namespace http {

template <typename Request> class RequestParseVisitor {
public:
  explicit RequestParseVisitor(Request *request) : request_(request) {}

  template <typename F>
  void operator()(grammar::HTTP_major_version, F &&run_parser) const {
    run_parser(parser::MakeIntegerParseVisitor(
        [this](int n) { request_->set_major_version(n); }));
  }

  template <typename F>
  void operator()(grammar::HTTP_minor_version, F &&run_parser) const {
    run_parser(parser::MakeIntegerParseVisitor(
        [this](int n) { request_->set_minor_version(n); }));
  }

  template <typename F>
  void operator()(grammar::Request_URI, F &&run_parser) const {
    auto *uri = request_->mutable_uri();
    uri::UriFieldParseVisitor<std::decay_t<decltype(*uri)>> uri_visitor(uri);
    auto result = run_parser(uri_visitor);
    if (result.ok()) {
      uri->assign(std::begin(result.get()), std::end(result.get()));
    }
  }

  template <typename F>
  void operator()(grammar::HttpMethod, F &&run_parser) const {
    auto result = run_parser();
    if (result.ok()) {
      auto first = std::begin(result.get());
      auto last = std::end(result.get());
      if (first != last) {
        switch (*first) {
        case 'C':
          request_->set_http_method(HttpMethod::CONNECT);
          break;
        case 'D':
          request_->set_http_method(HttpMethod::DELETE);
          break;
        case 'G':
          request_->set_http_method(HttpMethod::GET);
          break;
        case 'H':
          request_->set_http_method(HttpMethod::HEAD);
          break;
        case 'P':
          ++first;
          if (first != last) {
            if (*first == 'U') {
              request_->set_http_method(HttpMethod::PUT);
            } else {
              request_->set_http_method(HttpMethod::POST);
            }
          }
          break;
        case 'T':
          request_->set_http_method(HttpMethod::TRACE);
          break;
        default:
          break;
        }
      }
    }
  }

  template <typename F>
  void operator()(grammar::message_header, F &&run_parser) const {
    typename Request::FieldName name;
    run_parser(util::FirstMatchRef(
        [&](grammar::field_name, auto &&run_parser) {
          auto result = run_parser();
          if (result.ok()) {
            name = typename Request::FieldName(std::begin(result.get()),
                                               std::end(result.get()));
          }
        },
        [&](grammar::field_value, auto &&run_parser) {
          auto result = run_parser();
          if (result.ok()) {
            typename Request::FieldValue value(std::begin(result.get()),
                                               std::end(result.get()));
            request_->mutable_headers()->emplace_back(std::move(name),
                                                      std::move(value));
          }
        }));
  }

private:
  Request *request_;
};

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_REQUEST_PARSE_VISITOR_H
