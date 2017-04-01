#include "hittop/parser/repeat_and_then.h"
#include "hittop/parser/repeat_and_then.h"

#include "gtest/gtest.h"

#include <cctype>
#include <iterator>
#include <string>

#include "hittop/parser/char_filter.h"
#include "hittop/parser/concat.h"
#include "hittop/parser/repeat.h"
#include "hittop/parser/token.h"

using ::hittop::parser::Parse;
using ::hittop::parser::ParseError;
using ::hittop::parser::CharFilter;
using ::hittop::parser::Concat;
using ::hittop::parser::Repeat;
using ::hittop::parser::RepeatAndThen;

namespace {
using BadGrammar =
    Concat<Repeat<CharFilter<&std::isalnum>>, CharFilter<&std::isalpha>>;

using CorrectGrammar =
    RepeatAndThen<CharFilter<&std::isalnum>, CharFilter<&isalpha>>;

namespace tokens {
DEFINE_TOKEN(eoi);
} // tokens

using BadBacktrackGrammar =
    Concat<Repeat<CharFilter<&std::isalpha>>, tokens::eoi>;

using CorrectBacktrackGrammar =
    RepeatAndThen<CharFilter<&std::isalpha>, tokens::eoi>;
//
} // namespace

TEST(RepeatAndThenTest, ConcatRepeatFails) {
  std::string in = "a1b2c3d!";
  auto result = Parse<BadGrammar>(in);
  EXPECT_EQ(result.error(), ParseError::BAD_CHAR);
  EXPECT_EQ(std::prev(in.end()), result.get());
}

TEST(RepeatAndThenTest, Ok) {
  std::string in = "a1b2c3d!";
  auto result = Parse<CorrectGrammar>(in);
  EXPECT_TRUE(result.ok());
  EXPECT_EQ(std::prev(in.end()), result.get());
}

TEST(RepeatAndThenTest, OkBacktrack1) {
  std::string in = "a1b2c3d4!";
  auto result = Parse<CorrectGrammar>(in);
  EXPECT_TRUE(result.ok());
  EXPECT_EQ(std::prev(in.end(), 2), result.get());
}

TEST(RepeatAndThenTest, BacktrackConcatRepeatFails) {
  std::string in = "abcdefgeoi!";
  auto result = Parse<BadBacktrackGrammar>(in);
  EXPECT_FALSE(result.ok());
  EXPECT_EQ(std::prev(in.end()), result.get());
}

TEST(RepeatAndThenTest, BacktrackOk) {
  std::string in = "abcdefgeoi!";
  auto result = Parse<CorrectBacktrackGrammar>(in);
  EXPECT_TRUE(result.ok());
  EXPECT_EQ(std::prev(in.end()), result.get());
}

TEST(RepeatAndThenTest, BacktrackFail) {
  std::string in = "abcdefgfoo!";
  auto result = Parse<CorrectBacktrackGrammar>(in);
  EXPECT_EQ(result.error(), ParseError::BAD_CHAR);
  EXPECT_EQ(in.begin(), result.get());
}
