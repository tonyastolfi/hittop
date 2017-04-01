#include "hittop/uri/uri_parse_visitor.h"
#include "hittop/uri/uri_parse_visitor.h"

#include "gtest/gtest.h"

#include "hittop/parser/parser.h"
#include "hittop/uri/basic_uri.h"
#include "hittop/uri/grammar.h"

namespace {

using ::hittop::parser::Parse;
using ::hittop::uri::MakeUriParseVisitor;
using ::hittop::uri::Uri;

class UriParseVisitorTest : public ::testing::Test {};

TEST_F(UriParseVisitorTest, Example1) {
  std::string input =
      "https://example.org/absolute/URI/with/absolute/path/to/resource.txt\n";

  Uri uri;
  auto result = Parse<::hittop::uri::grammar::URI_reference>(
      input, MakeUriParseVisitor(&uri));

  EXPECT_TRUE(result.ok());
  ASSERT_FALSE(!uri.scheme());
  EXPECT_EQ(uri.scheme().get(), "https");
  EXPECT_FALSE(uri.user());
  ASSERT_FALSE(!uri.host());
  EXPECT_EQ(uri.host().get(), "example.org");
  EXPECT_FALSE(uri.port());
  ASSERT_FALSE(!uri.path());
  EXPECT_EQ(uri.path().get(),
            "/absolute/URI/with/absolute/path/to/resource.txt");
  EXPECT_FALSE(uri.fragment());
  EXPECT_FALSE(uri.query());
}

} // namespace
