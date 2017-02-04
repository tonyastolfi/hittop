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
DEFINE_TOKEN(end);
} // tokens

using BadBacktrackGrammar =
    Concat<Repeat<CharFilter<&std::isalpha>>, tokens::end>;

using CorrectBackgrackGrammar =
    RepeatAndThen<CharFilter<&std::isalpha>, tokens::end>;

std::string backtrack_good_input() { return "abcdefgend"; }
std::string backtrack_bad_input() { return "abcdefg123"; }
//
} // namespace

TEST(RepeatAndThenTest, ConcatRepeatFails) {
  std::string in = "a1b2c3d";
  auto result = Parse<BadGrammar>(in);
  EXPECT_FALSE(!result.error());
  EXPECT_EQ(in.end(), result.get());
}

TEST(RepeatAndThenTest, Ok) {
  std::string in = "a1b2c3d";
  auto result = Parse<CorrectGrammar>(in);
  EXPECT_FALSE(result.error());
  EXPECT_EQ(in.end(), result.get());
}

TEST(RepeatAndThenTest, OkBacktrack1) {
  std::string in = "a1b2c3d4";
  auto result = Parse<CorrectGrammar>(in);
  EXPECT_FALSE(result.error());
  EXPECT_EQ(std::prev(in.end()), result.get());
}

TEST(RepeatAndThenTest, BacktrackConcatRepeatFails) {
  auto in = backtrack_good_input();
  auto result = Parse<BacktrackBadGrammar>(in);
  EXPECT_FALSE(!result.error());
  EXPECT_EQ(in.end(), result.get());
}

TEST(RepeatAndThenTest, BacktrackOk) {
  auto in = backtrack_good_input();
  auto result = Parse<BacktrackCorrectGrammar>(in);
  EXPECT_FALSE(result.error());
  EXPECT_EQ(in.end(), result.get());
}

TEST(RepeatAndThenTest, BacktrackFail) {
  auto in = backtrack_bad_input();
  auto result = Parse<BacktrackCorrectGrammar>(in);
  EXPECT_FALSE(!result.error());
  EXPECT_EQ(in.begin(), result.get());
}
