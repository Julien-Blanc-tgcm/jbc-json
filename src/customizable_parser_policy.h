//          Copyright Julien Blanc 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef JBC_LIBJSON_CUSTOMIZABLE_PARSER_POLICY_H
#define JBC_LIBJSON_CUSTOMIZABLE_PARSER_POLICY_H

#include "libjson.h"

template<typename string_type>
class customizable_parser_policy
{
public:
    std::function<bool()> begin_array_handler;
    std::function<bool()> end_array_handler;

    std::function<bool()> begin_object_handler;
    std::function<bool()> end_object_handler;

    std::function<bool(string_type &&)> string_handler;
    std::function<bool(string_type &&)> key_handler;

    std::function<bool(double)> double_handler;

#ifdef JSON_USE_LONG_INTEGERS
    std::function<bool(int64_t)> integer_handler;
#endif

    std::function<bool(bool)> boolean_handler;

    std::function<bool()> null_handler;
};

#endif // JBC_JSON_CUSTOMIZABLE_PARSER_POLICY_H
