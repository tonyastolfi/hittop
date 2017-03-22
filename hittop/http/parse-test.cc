#include "gtest/gtest.h"

#include <iterator>

#include "hittop/http/grammar.h"
#include "hittop/parser/parser.h"
#include "hittop/parser/trace_visitor.h"
#include "hittop/util/first_match.h"
#include "hittop/util/test_data.h"

namespace {

using ::hittop::util::FirstMatch;
using ::hittop::util::LoadTestData;
using ::hittop::parser::Parse;
using ::hittop::parser::TraceVisitor;

namespace http = ::hittop::http::grammar;

class HttpParseTest : public ::testing::Test {
protected:
  void SetUp() {
    tracer_.InvokeWithTypeRegistry(
        [](auto &m) { ::hittop::http::grammar::RegisterRuleNames(m); });
  }

  template <typename Assoc> auto CaptureHeaders(Assoc *headers) {
    return [headers](http::message_header, auto &&run_parser) {
      typename Assoc::key_type name;
      typename Assoc::key_type value;
      if (!run_parser(FirstMatch(
                          [&name](http::field_name, auto &&run_parser) {
                            auto result = run_parser();
                            if (result.ok()) {
                              name.assign(std::begin(result.get()),
                                          std::end(result.get()));
                            }
                          },
                          [&value](http::field_value, auto &&run_parser) {
                            auto result = run_parser();
                            if (result.ok()) {
                              value.assign(std::begin(result.get()),
                                           std::end(result.get()));
                            }
                          }))
               .error()) {
        headers->emplace(std::move(name), std::move(value));
      }
    };
  }

  std::unordered_map<std::string, std::string>
  RunParseTest(const char *input_file, std::size_t expected_size) {
    std::unordered_map<std::string, std::string> headers;
    const std::string input = LoadTestData(input_file);
    auto result = Parse<http::HTTP_message>(input, CaptureHeaders(&headers));
    EXPECT_FALSE(result.error()) << result.error().message();
    EXPECT_EQ(std::distance(input.cbegin(), result.get()), expected_size);
    return headers;
  }

  TraceVisitor tracer_;
};

TEST_F(HttpParseTest, ParseGoogleResponse) {
  auto headers = RunParseTest("/hittop/http/google_response.bin", 656);
  EXPECT_EQ(headers.size(), 12U);
  EXPECT_EQ(headers["Date"], "Tue, 21 Feb 2017 12:54:01 GMT");
  EXPECT_EQ(headers["Expires"], "-1");
  EXPECT_EQ(headers["Cache-Control"], "private, max-age=0");
  EXPECT_EQ(headers["Content-Type"], "text/html; charset=ISO-8859-1");
  EXPECT_EQ(headers["P3P"],
            "CP=\"This is not a P3P policy! See https://www.google.com/support/"
            "accounts/answer/151657?hl=en for more info.\"");
  EXPECT_EQ(headers["Server"], "gws");
  EXPECT_EQ(headers["X-XSS-Protection"], "1; mode=block");
  EXPECT_EQ(headers["X-Frame-Options"], "SAMEORIGIN");
  EXPECT_EQ(headers["Set-Cookie"],
            "NID=97=jSX643F0LHdoYA7xxzDHpC8eZGPEZRBeEqs5zCAhk9hhGV0fk0yR7ZIHEaR"
            "qMnjxgmkC2aK9RoO5Lp1KQarotd3jOAm6S3qL9LgGx7YBOo4LAsRapqHplwAs5T3O6"
            "DuYBJbfoPeVl5rLbegHGw; expires=Wed, 23-Aug-2017 12:54:01 GMT; path"
            "=/; domain=.google.com; HttpOnly");
  EXPECT_EQ(headers["Accept-Ranges"], "none");
  EXPECT_EQ(headers["Vary"], "Accept-Encoding");
  EXPECT_EQ(headers["Transfer-Encoding"], "chunked");
}

TEST_F(HttpParseTest, ParseChromeRequest) {
  auto headers = RunParseTest("/hittop/http/curl_request.bin", 78);
  EXPECT_EQ(headers.size(), 3);
  EXPECT_EQ(headers["Host"], "localhost:9999");
  EXPECT_EQ(headers["User-Agent"], "curl/7.43.0");
  EXPECT_EQ(headers["Accept"], "*/*");
}
TEST_F(HttpParseTest, ParseCurlRequest) {
  auto headers = RunParseTest("/hittop/http/chrome_request.bin", 387);
  EXPECT_EQ(headers.size(), 7U);
  EXPECT_EQ(headers["Host"], "localhost:9999");
  EXPECT_EQ(headers["Connection"], "keep-alive");
  EXPECT_EQ(headers["Upgrade-Insecure-Requests"], "1");
  EXPECT_EQ(headers["User-Agent"],
            "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_6) AppleWebKit/537.36"
            " (KHTML, like Gecko) Chrome/56.0.2924.87 Safari/537.36");
  EXPECT_EQ(headers["Accept"],
            "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*"
            "/*;q=0.8");
  EXPECT_EQ(headers["Accept-Encoding"], "gzip, deflate, sdch, br");
  EXPECT_EQ(headers["Accept-Language"], "en-US,en;q=0.8");
}

} // namespace
