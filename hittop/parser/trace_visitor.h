// DESCRIPTION
//
#ifndef HITTOP_PARSER_TRACE_VISITOR_H
#define HITTOP_PARSER_TRACE_VISITOR_H

#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <type_traits>
#include <unordered_map>

#include "hittop/util/demangle.h"
#include "hittop/util/range_to_string.h"
#include "hittop/util/scope_exit.h"

namespace hittop {
namespace parser {

template <typename BaseRule> struct BasicTraceVisitor {
  std::ostream *out = &std::clog;
  std::shared_ptr<int> nesting = std::make_shared<int>(0);
  std::function<boost::optional<std::string>(const std::type_info &)>
      translate = [](const auto &info) { return util::abi::demangle(info); };

  template <typename F> void InvokeWithTypeRegistry(F &&f) {
    auto rules = std::make_shared<
        std::unordered_map<const std::type_info *, std::string>>();
    f(*rules);
    translate = [rules](const auto &info) -> boost::optional<std::string> {
      if (rules->count(&info)) {
        return (*rules)[&info];
      } else {
        return boost::none;
      }
    };
  }

  template <typename R> BasicTraceVisitor<R> recurse() const {
    BasicTraceVisitor<R> v;
    v.out = this->out;
    v.nesting = this->nesting;
    v.translate = this->translate;
    return v;
  }

  template <typename Rule, typename F,
            typename = std::enable_if_t<!std::is_same<Rule, BaseRule>::value>>
  void operator()(Rule, F &&run_parser) const {
    const std::string indent(*nesting, ' ');
    const auto type_name = translate(typeid(Rule));
    if (type_name) {
      (*out) << indent << type_name.get() << " enter" << std::endl;
      *nesting += 2;
    }
    decltype(run_parser(recurse<Rule>())) result;
    util::ScopeExit exit_guard([&]() {
      if (type_name) {
        std::string fragment;
        if (std::distance(std::begin(result.get()), std::end(result.get())) ==
            1U) {
          std::ostringstream oss;
          oss << " '" << util::RangeToString(result.get()) << "' ("
              << int{*std::begin(result.get())} << ")";
          fragment = oss.str();
        }
        (*out) << indent << type_name.get() << " exit (result="
               << (result.error() ? result.error().message() : "ok") << ")"
               << fragment << std::endl;
        *nesting -= 2;
      }
    });
    result = run_parser(recurse<Rule>());
  }
};

struct NoRulesExcluded {};

using TraceVisitor = BasicTraceVisitor<NoRulesExcluded>;

} // namespace parser
} // namespace hittop

#endif // HITTOP_PARSER_TRACE_VISITOR_H
