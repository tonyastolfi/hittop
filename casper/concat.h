#ifndef CASPER_CONCAT_H
#define CASPER_CONCAT_H

namespace casper {

template <typename First, typename Second> struct Concat {};

template <typename First, typename Second> class Parser<Concat<First, Second>> {
  template <typename Range>
  auto operator()(const Range &input) -> Fallible<decltype(std::begin(input))> {
    First first;
    auto first_result = first(input);
    if (first_result.error()) {
      return first_result;
    }
    Second second;
    return {}
  }
};

} // namespace casper

#endif // CASPER_CONCAT_H
