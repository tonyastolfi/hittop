#ifndef HITTOP_HTTP_HTTP_REQUEST_PARSE_VISITOR_H
#define HITTOP_HTTP_HTTP_REQUEST_PARSE_VISITOR_H

#include "hittop/http/basic_request.h"
#include "hittop/http/grammar.h"

namespace hittop {
namespace http {

template <typename Request> class RequestParseVisitor {
public:
  explicit RequestParseVisitor(Request *request) : request_(request) {}

  template <typename F>
  void operator()(grammar::HTTP_major_version, F &&run_parser) const {
    run_parser(parser::IntegerParseVisitor(
        [this](int n) { request_->set_major_version(n); }));
  }

  template <typename F>
  void operator()(grammar::HTTP_minor_version, F &&run_parser) const {
    run_parser(parser::IntegerParseVisitor(
        [this](int n) { request_->set_minor_version(n); }));
  }

private:
  Request *request_;
};

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_HTTP_REQUEST_PARSE_VISITOR_H
