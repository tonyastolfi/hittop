// Translation of the JavaScript Object Notation grammar from http://json.org.
//
#ifndef HITTOP_JSON_GRAMMAR_H
#define HITTOP_JSON_GRAMMAR_H

#include <cctype>

#include "hittop/parser/any_char.h"
#include "hittop/parser/at_least.h"
#include "hittop/parser/char_filter.h"
#include "hittop/parser/concat.h"
#include "hittop/parser/either.h"
#include "hittop/parser/exactly.h"
#include "hittop/parser/forward_ref.h"
#include "hittop/parser/inter.h"
#include "hittop/parser/literal.h"
#include "hittop/parser/opt.h"
#include "hittop/parser/repeat.h"
#include "hittop/parser/token.h"
#include "hittop/parser/trim.h"
#include "hittop/parser/unless.h"

namespace hittop {
namespace json {
namespace grammar {

using namespace ::hittop::parser;

using digit = CharFilter<std::isdigit>;

using digit_1_9 = Unless<Literal<'0'>, digit>;

using hex_digit = CharFilter<std::isxdigit>;

using control_char = CharFilter<std::iscntrl>;

struct Value_;

using Value = ForwardRef<Value_>;

using StringContents = Repeat<Either<                         //
    Unless<Either<Literal<'"'>, Literal<'\\'>, control_char>, //
           AnyChar>,                                          //
    Concat<Literal<'\\'>,                                     //
           Either<                                            //
               Literal<'"'>,                                  //
               Literal<'\\'>,                                 //
               Literal<'/'>,                                  //
               Literal<'b'>,                                  //
               Literal<'f'>,                                  //
               Literal<'n'>,                                  //
               Literal<'r'>,                                  //
               Literal<'t'>,                                  //
               Concat<Literal<'u'>, Exactly<4, hex_digit>>>>>>;

using String =              //
    Trim<                   //
        Concat<             //
            Literal<'"'>,   //
            StringContents, //
            Literal<'"'>>>;

using Number =                                               //
    Trim<                                                    //
        Concat<                                              //
            Opt<Literal<'-'>>,                               //
            Either<                                          //
                Literal<'0'>,                                //
                Concat<digit_1_9, Repeat<digit>>>,           //
            Opt<Concat<Literal<'.'>, AtLeast<1, digit>>>,    //
            Opt<                                             //
                Concat<                                      //
                    Either<Literal<'e'>, Literal<'E'>>,      //
                    Opt<Either<Literal<'+'>, Literal<'-'>>>, //
                    AtLeast<1, digit>>>>>;

namespace tokens {

DEFINE_NAMED_TOKEN(True, "true");
DEFINE_NAMED_TOKEN(False, "false");
DEFINE_NAMED_TOKEN(Null, "null");

} // namespace token

using Null = tokens::Null;

using Property = Concat<Trim<String>, Literal<':'>, Trim<Value>>;

using Object =                             //
    Trim<                                  //
        Concat<                            //
            Literal<'{'>,                  //
            Inter<Property, Literal<','>>, //
            Literal<'}'>>>;

using Array =                                 //
    Trim<                                     //
        Concat<                               //
            Literal<'['>,                     //
            Inter<Trim<Value>, Literal<','>>, //
            Literal<']'>>>;

using Boolean = Trim<Either<tokens::True, tokens::False>>;

struct Value_ {
  using type = Either< //
      String,          //
      Number,          //
      Object,          //
      Array,           //
      Boolean,         //
      Null>;
};

} // grammar
} // namespace json
} // namespace hittop

#endif // HITTOP_JSON_GRAMMAR_H
