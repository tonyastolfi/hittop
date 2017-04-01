#include "hittop/parser/opt.h"

#include "gtest/gtest.h"

#include "boost/range/as_literal.hpp"

#include "hittop/parser/token.h"

using boost::as_literal;
using hittop::parser::Opt;
using hittop::parser::Parse;
using hittop::parser::ParseError;

namespace {

namespace tokens {

DEFINE_TOKEN(hello);

} // namespace tokens

const char *kEmpty = "";
const char *kHello = "hello";
const char *kHelloHello = "hellohello";
const char *kHell = "hell";
const char *kAloha = "aloha";
const char *kHola = "hola";

auto RunParser(const char *input)
    -> decltype(Parse<Opt<tokens::hello>>(as_literal(input))) {
  return Parse<Opt<tokens::hello>>(as_literal(input));
}

} // namespace

TEST(TestOpt, OkConsumeAll) {
  auto result = RunParser(kHello);
  EXPECT_TRUE(result.ok());
  EXPECT_EQ(result.get(), &kHello[5]);
}

TEST(TestOpt, OkConsumePartial) {
  auto result = RunParser(kHelloHello);
  EXPECT_TRUE(result.ok());
  EXPECT_EQ(result.get(), &kHelloHello[5]);
}

TEST(TestOpt, IncompleteEmpty) {
  auto result = RunParser(kEmpty);
  EXPECT_EQ(result.error(), ParseError::INCOMPLETE);
  EXPECT_EQ(result.get(), &kEmpty[0]);
}

TEST(TestOpt, IncompletePartial) {
  auto result = RunParser(kHell);
  EXPECT_EQ(result.error(), ParseError::INCOMPLETE);
  EXPECT_EQ(result.get(), &kHell[4]);
}

TEST(TestOpt, OkFailFirstChar) {
  auto result = RunParser(kAloha);
  EXPECT_TRUE(result.ok());
  EXPECT_EQ(result.get(), &kAloha[0]);
}

TEST(TestOpt, OkFailMidWay) {
  auto result = RunParser(kHola);
  EXPECT_TRUE(result.ok());
  EXPECT_EQ(result.get(), &kHola[0]);
}
