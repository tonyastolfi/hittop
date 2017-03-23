#include "hittop/uri/grammar.h"
#include "hittop/uri/grammar.h"

#include "gtest/gtest.h"

#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>

#include "hittop/parser/trace_visitor.h"
#include "hittop/util/first_match.h"
#include "hittop/util/range_to_string.h"

using ::hittop::parser::Parse;
using ::hittop::parser::TraceVisitor;
using ::hittop::util::FirstMatch;
using ::hittop::util::RangeToString;

namespace uri = ::hittop::uri::grammar;

namespace {

class UriGrammarTest : public ::testing::Test {
protected:
};

TEST_F(UriGrammarTest, Example1) {
  std::string input =
      "https://example.org/absolute/URI/with/absolute/path/to/resource.txt\n";

  TraceVisitor v;
  v.InvokeWithTypeRegistry(
      [](auto &m) { ::hittop::uri::grammar::RegisterRuleNames(m); });

  auto result = Parse<uri::URI_reference>(input, v);
  EXPECT_EQ(result.get(), std::prev(input.end()));
  EXPECT_TRUE(result.ok()) << make_error_condition(result.error()).message()
                           << " at '" << std::string(result.get(), input.cend())
                           << "'";
}

} // namespace
