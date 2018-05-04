#ifndef JBC_LIBJSON_CUSTOMIZABLE_PARSER_POLICY_H
#define JBC_LIBJSON_CUSTOMIZABLE_PARSER_POLICY_H

#include "libjson.h"

template<typename string_type>
class customizable_parser_policy
{
public:
    std::function<bool()> begin_array_handler_begin;
    std::function<bool()> begin_array_handler_array;
    std::function<bool()> begin_array_handler_object_value;

    std::function<bool()> end_array_handler;
    std::function<bool()> begin_object_handler_begin;
    std::function<bool()> begin_object_handler_array;
    std::function<bool()> begin_object_handler_object_value;
    std::function<bool()> end_object_handler;

    std::function<bool(string_type &&)> string_handler_array;
    std::function<bool(string_type &&)> string_handler_object_value;
    std::function<bool(string_type &&)> string_handler_object_key;

    std::function<bool(double)> double_handler_array;
    std::function<bool(double)> double_handler_object_value;

#ifdef JSON_USE_LONG_INTEGERS
    std::function<bool(int64_t)> integer_handler_array;
    std::function<bool(int64_t)> integer_handler_object_value;
#endif

    std::function<bool(bool)> boolean_handler_array;
    std::function<bool(bool)> boolean_handler_object_value;

    std::function<bool()> null_handler_array;
    std::function<bool()> null_handler_object_value;
};

#endif // JBC_JSON_CUSTOMIZABLE_PARSER_POLICY_H
