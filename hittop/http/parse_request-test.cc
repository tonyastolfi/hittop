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
  EXPECT_EQ(request.headers().size(), 7U);
  EXPECT_EQ(RangeToString(request.headers()[0].name), "Host");
  EXPECT_EQ(RangeToString(request.headers()[0].value), "localhost:9999");
  EXPECT_EQ(RangeToString(request.headers()[1].name), "Connection");
  EXPECT_EQ(RangeToString(request.headers()[1].value), "keep-alive");
  EXPECT_EQ(RangeToString(request.headers()[2].name),
            "Upgrade-Insecure-Requests");
  EXPECT_EQ(RangeToString(request.headers()[2].value), "1");
  EXPECT_EQ(RangeToString(request.headers()[3].name), "User-Agent");
  EXPECT_EQ(RangeToString(request.headers()[3].value),
            "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_6) "
            "AppleWebKit/537.36 (KHTML, like Gecko) Chrome/56.0.2924.87 "
            "Safari/537.36");
  EXPECT_EQ(RangeToString(request.headers()[4].name), "Accept");
  EXPECT_EQ(RangeToString(request.headers()[4].value),
            "text/html,application/xhtml+xml,application/xml;q=0.9,image/"
            "webp,*/*;q=0.8");
  EXPECT_EQ(RangeToString(request.headers()[5].name), "Accept-Encoding");
  EXPECT_EQ(RangeToString(request.headers()[5].value),
            "gzip, deflate, sdch, br");
  EXPECT_EQ(RangeToString(request.headers()[6].name), "Accept-Language");
  EXPECT_EQ(RangeToString(request.headers()[6].value), "en-US,en;q=0.8");
  EXPECT_EQ(RangeToString(request.header(0).name), "Host");
  EXPECT_EQ(RangeToString(request.header(0).value), "localhost:9999");
  EXPECT_EQ(RangeToString(request.header(1).name), "Connection");
  EXPECT_EQ(RangeToString(request.header(1).value), "keep-alive");
  EXPECT_EQ(RangeToString(request.header(2).name), "Upgrade-Insecure-Requests");
  EXPECT_EQ(RangeToString(request.header(2).value), "1");
  EXPECT_EQ(RangeToString(request.header(3).name), "User-Agent");
  EXPECT_EQ(RangeToString(request.header(3).value),
            "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_6) "
            "AppleWebKit/537.36 (KHTML, like Gecko) Chrome/56.0.2924.87 "
            "Safari/537.36");
  EXPECT_EQ(RangeToString(request.header(4).name), "Accept");
  EXPECT_EQ(RangeToString(request.header(4).value),
            "text/html,application/xhtml+xml,application/xml;q=0.9,image/"
            "webp,*/*;q=0.8");
  EXPECT_EQ(RangeToString(request.header(5).name), "Accept-Encoding");
  EXPECT_EQ(RangeToString(request.header(5).value), "gzip, deflate, sdch, br");
  EXPECT_EQ(RangeToString(request.header(6).name), "Accept-Language");
  EXPECT_EQ(RangeToString(request.header(6).value), "en-US,en;q=0.8");
}

} // namespace
