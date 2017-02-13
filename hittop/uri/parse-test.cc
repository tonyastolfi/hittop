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

  EXPECT_FALSE(result.error());
  EXPECT_FALSE(!uri.scheme());
  EXPECT_FALSE(!uri.user());
  EXPECT_FALSE(!uri.host());
  EXPECT_FALSE(!uri.port());
  EXPECT_FALSE(!uri.path());
  EXPECT_FALSE(!uri.fragment());
  EXPECT_FALSE(!uri.query());
}

} // namespace
