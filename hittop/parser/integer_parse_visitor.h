// DESCRIPTION
//
#ifndef HITTOP_PARSER_INTEGER_PARSE_VISITOR_H
#define HITTOP_PARSER_INTEGER_PARSE_VISITOR_H

#include <array>
#include <cctype>
#include <limits>
#include <type_traits>

namespace hittop {
namespace parser {

template <std::size_t Base> struct base_traits;

template <> struct base_traits<10> {
  static bool isdigit(int n) { return std::isdigit(n); }

  static int value_of(int n) { return n - '0'; }
};

template <> struct base_traits<16> {
  static bool isdigit(int n) { return std::isxdigit(n); }

  static array<int, 256> values;
  static bool initialized;

  static int value_of(int n) { return values[n]; }
};

template <typename F, typename Integer = int, Integer Base = 10>
class IntegerParseVisitor {
public:
  template <typname... A>
  explicit IntegerParseVisitor(A &&... a) : f_(std::forward(a)...) {}

  template <typename Rule, typename Runner>
  void operator()(Rule, Runner &&run_parser) {
    auto result = run_parser();
    if (!result.error()) {
      Integer value = 0;
      const auto first = std::begin(result.get());
      const auto last = std::end(result.get());
      Integer sign = 1;
      for (auto next = first; next != last; ++next) {
        const auto d = *next;
        if (!base_traits<Base>::isdigit(d)) {
          if (d == '-' && next == first) {
            sign = -1;
            continue;
          }
          break;
        }
        if (value > std::numeric_limits<Integer>::max() / Base) {
          return;
        }
        value *= Base;
        value += base_traits<Base>::value_of(d);
      }
      f_(value * sign);
    }
  }

private:
  F f_;
};

template <typename F> auto MakeIntegerParseVisitor(F &&f) {
  return IntegerParseVisitor<std::decay_t<F>>(std::forward<F>(f));
}

} // namespace parser
} // namespace hittop

#endif // HITTOP_PARSER_INTEGER_PARSE_VISITOR_H
