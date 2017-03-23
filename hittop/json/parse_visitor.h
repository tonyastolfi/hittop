#ifndef HITTOP_JSON_PARSE_VISITOR_H
#define HITTOP_JSON_PARSE_VISITOR_H

#include <iterator>
#include <sstream>
#include <string>

#include "hittop/util/first_match.h"
#include "hittop/util/range_to_string.h"

#include "hittop/json/grammar.h"
#include "hittop/json/types.h"

namespace hittop {
namespace json {

namespace internal {

inline unsigned long HexValue(int x) {
  if (x <= '9') {
    return x - '0';
  } else if (x >= 'a') {
    return x - 'a' + 10;
  } else {
    return x - 'A' + 10;
  }
}

/* Returns the given character range as a string with all JSON escaped chars
 * replaced by their UTF-8 equivalent.  This function is unsafe to call on
 * ranges that are not valid JSON string content sequences (e.g., it does not
 * make sure that the character after a '\' is inside the range).
 */
template <typename Range> inline std::string UnescapeUnsafe(const Range &in) {
  std::ostringstream out;
  auto next = std::begin(in);
  auto last = std::end(in);
  for (; next != last; ++next) {
    switch (*next) {
    case '\\':
      ++next;
      switch (*next) {
      case '"':
        out << char('"');
        break;
      case '\\':
        out << char('\\');
        break;
      case '/':
        out << char('/');
        break;
      case 'b':
        out << char('\b');
        break;
      case 'f':
        out << char('\f');
        break;
      case 'n':
        out << char('\n');
        break;
      case 'r':
        out << char('\r');
        break;
      case 't':
        out << char('\t');
        break;
      case 'u': {
        const unsigned int u16 = (HexValue(*std::next(next, 1)) << 12) | //
                                 (HexValue(*std::next(next, 2)) << 8) |  //
                                 (HexValue(*std::next(next, 3)) << 4) |  //
                                 (HexValue(*std::next(next, 4)));
        std::advance(next, 3);
        if (u16 < 0x80) {
          out << char(u16);
        } else {
          if (u16 < 0x800) {
            out << char(0xc0 | ((u16 >> 6) & 0x1f))
                << char(0x80 | ((u16 >> 0) & 0x3f));
          } else {
            out << char(0xe0 | ((u16 >> 12) & 0x0f))
                << char(0x80 | ((u16 >> 6) & 0x3f))
                << char(0x80 | ((u16 >> 0) & 0x3f));
          }
        }
        break;
      }
      default:
        throw std::runtime_error("bad json escape sequence");
      }
      break;
    default:
      out << char(*next);
    }
  }
  return out.str();
}

} // namespace internal

class ValueParseVisitor {
private:
  Value *const output_;

public:
  explicit ValueParseVisitor(Value *output) : output_(output) {}

  ValueParseVisitor(const ValueParseVisitor &) = delete;
  ValueParseVisitor &operator=(const ValueParseVisitor &) = delete;

  template <typename F>
  void operator()(grammar::StringContents, F &&run_parser) const {
    auto result = run_parser();
    if (result.ok()) {
      *output_ = internal::UnescapeUnsafe(result.get());
    }
  }

  template <typename F>
  void operator()(grammar::Boolean, F &&run_parser) const {
    auto result = run_parser();
    if (result.ok()) {
      *output_ = Boolean{*std::begin(result.get()) == 't'};
    }
  }

  template <typename F> void operator()(grammar::Number, F &&run_parser) const {
    auto result = run_parser();
    if (result.ok()) {
      *output_ = std::stod(util::RangeToString(result.get()));
    }
  }

  template <typename F> void operator()(grammar::Null, F &&run_parser) const {
    auto result = run_parser();
    if (result.ok()) {
      *output_ = Null{};
    }
  }

  template <typename F> void operator()(grammar::Array, F &&run_parser) const {
    Array items;
    auto result = run_parser([&items](grammar::Value, auto get_item_result) {
      Value next_item;
      ValueParseVisitor item_visitor{&next_item};
      auto item_result = get_item_result(item_visitor);
      if (item_result.ok()) {
        items.emplace_back(std::move(next_item));
      }
    });
    if (result.ok()) {
      *output_ = std::move(items);
    }
  }

  template <typename F> void operator()(grammar::Object, F &&run_parser) const {
    Object object;
    auto result =
        run_parser([&object](grammar::Property, auto get_property_result) {
          std::string name;
          get_property_result(util::FirstMatch(
              [&name](grammar::StringContents, auto get_name_result) {
                auto name_result = get_name_result();
                if (name_result.ok()) {
                  name = internal::UnescapeUnsafe(name_result.get());
                }
              },
              [&object, &name](grammar::Value, auto get_value_result) {
                Value property;
                ValueParseVisitor property_visitor{&property};
                auto value_result = get_value_result(property_visitor);
                if (value_result.ok()) {
                  object.emplace(
                      std::make_pair(std::move(name), std::move(property)));
                }
              }));
        });
    if (result.ok()) {
      *output_ = std::move(object);
    }
  }
};

} // namespace json
} // namespace hittop

#endif // HITTOP_JSON_PARSE_VISITOR_H
