#include "gtest/gtest.h"

#include "hittop/http/grammar.h"
#include "hittop/http/request.h"
#include "hittop/http/request_parse_visitor.h"
#include "hittop/parser/parser.h"
#include "hittop/util/test_data.h"

namespace {

using ::hittop::util::LoadTestData;

using Request = ::hittop::http::ZeroCopyRequest<std::string::const_iterator>;
using RequestParseVisitor = ::hittop::http::RequestParseVisitor<Request>;
using ::hittop::parser::Parse;
using ::hittop::util::RangeToString;
namespace http = ::hittop::http::grammar;

TEST(ParseRequestTest, ChromeRequest) {
  const std::string input = LoadTestData("/hittop/http/chrome_request2.bin");
  Request request;
  RequestParseVisitor v(&request);
  auto result = Parse<http::Request>(input, v);
  EXPECT_FALSE(result.error());
  EXPECT_EQ(request.http_method(), ::hittop::http::HttpMethod::GET);
  EXPECT_EQ(RangeToString(request.uri()), "/path/to/resource?foo=bar#myfrag");
  EXPECT_FALSE(request.uri().scheme());
  EXPECT_FALSE(request.uri().host());
  EXPECT_FALSE(request.uri().port());
  EXPECT_EQ(RangeToString(*request.uri().path()), "/path/to/resource");
  EXPECT_EQ(RangeToString(*request.uri().query()), "foo=bar");
  EXPECT_EQ(RangeToString(*request.uri().fragment()), "myfrag");
  EXPECT_EQ(request.version().major, 2);
  EXPECT_EQ(request.version().minor, 3);
}

} // namespace
