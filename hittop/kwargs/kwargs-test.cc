#include "hittop/kwargs/kwargs.h"
#include "hittop/kwargs/kwargs.h"

#include "gtest/gtest.h"

#include <sstream>
#include <string>

namespace {

DEFINE_KEYWORD(name, std::string);
DEFINE_KEYWORD(age, int);
DEFINE_KEYWORD(phone, unsigned long long);
DEFINE_KEYWORD(unused, void *);

template <typename... Args>
std::string FormatRecordWithDefaults(Args &&... kwargs) {
  std::ostringstream oss;
  oss << "{name: " << Get(name, kwargs...).value_or("Bob")
      << ", age: " << Get(age, kwargs...).value_or(47)
      << ", phone: " << Get(phone, kwargs...).value_or(7816651027ULL) << "}";
  return oss.str();
}

const char expected[] = "{name: Bob, age: 47, phone: 7816651027}";

TEST(KeywordArgsTest, TestFoundAt0) {
  boost::optional<std::string> v =
      Get(name, name = "Bob", age = 47, phone = 7816651027ULL);
  EXPECT_FALSE(!v);
  EXPECT_EQ(*v, "Bob");
}

TEST(KeywordArgsTest, TestFoundAt1) {
  boost::optional<int> v =
      Get(age, name = "Bob", age = 47, phone = 7816651027ULL);
  EXPECT_FALSE(!v);
  EXPECT_EQ(*v, 47);
}

TEST(KeywordArgsTest, TestFoundAt2) {
  boost::optional<unsigned long long> v =
      Get(phone, name = "Bob", age = 47, phone = 7816651027ULL);
  EXPECT_FALSE(!v);
  EXPECT_EQ(*v, 7816651027ULL);
}

TEST(KeywordArgsTest, TestOrder012) {
  EXPECT_EQ(expected, FormatRecordWithDefaults( //
                          name = "Bob",         //
                          age = 47,             //
                          phone = 7816651027ULL //
                          ));
}

TEST(KeywordArgsTest, TestWithUnused) {
  EXPECT_EQ(expected, FormatRecordWithDefaults( //
                          name = "Bob",         //
                          age = 47,             //
                          unused = nullptr,     //
                          phone = 7816651027ULL //
                          ));
}

TEST(KeywordArgsTest, TestOrder021) {
  EXPECT_EQ(expected, FormatRecordWithDefaults(  //
                          name = "Bob",          //
                          phone = 7816651027ULL, //
                          age = 47               //
                          ));
}

TEST(KeywordArgsTest, TestOrder102) {
  EXPECT_EQ(expected, FormatRecordWithDefaults(  //
                          phone = 7816651027ULL, //
                          name = "Bob",          //
                          age = 47               //
                          ));
}

TEST(KeywordArgsTest, TestOrder120) {
  EXPECT_EQ(expected, FormatRecordWithDefaults(  //
                          phone = 7816651027ULL, //
                          age = 47,              //
                          name = "Bob"           //
                          ));
}

TEST(KeywordArgsTest, TestOrder201) {
  EXPECT_EQ(expected, FormatRecordWithDefaults( //
                          age = 47,             //
                          name = "Bob",         //
                          phone = 7816651027ULL //
                          ));
}

TEST(KeywordArgsTest, TestOrder210) {
  EXPECT_EQ(expected, FormatRecordWithDefaults(  //
                          age = 47,              //
                          phone = 7816651027ULL, //
                          name = "Bob"           //
                          ));
}

TEST(KeywordArgsTest, TestMissing0) {
  EXPECT_EQ(expected, FormatRecordWithDefaults( //
                          age = 47,             //
                          phone = 7816651027ULL //
                          ));
}

TEST(KeywordArgsTest, TestMissing1) {
  EXPECT_EQ(expected, FormatRecordWithDefaults( //
                          name = "Bob",         //
                          phone = 7816651027ULL //
                          ));
}

TEST(KeywordArgsTest, TestMissing2) {
  EXPECT_EQ(expected, FormatRecordWithDefaults( //
                          name = "Bob",         //
                          age = 47              //
                          ));
}

TEST(KeywordArgsTest, TestUniquePtr) {
  std::unique_ptr<int> a(new int{5});
  boost::optional<std::unique_ptr<int>> b = Consume(age, age = std::move(a));
  EXPECT_EQ(a.get(), nullptr);
  EXPECT_EQ(**b, 5);
}

} // namespace
