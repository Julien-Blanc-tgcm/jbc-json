//          Copyright Julien Blanc 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef JBC_JSON_PARSER_BITS_H
#define JBC_JSON_PARSER_BITS_H

#include <cstdint>
#include <functional>
#include "helper_functions.h"

namespace jbc
{
namespace json
{

/**
 * The Parser class is a template class used to parse json data. It uses a callback approach
 */
template<template<class> class container,
            typename parser_callbacks,
            typename buffer_type_,
            typename char_type_>
class parser_bits :
    public parser_callbacks
{

    using consumer_fn = bool(parser_bits::*)(char_type_*);

    container<consumer_fn> consumer_stack_;

    uint32_t lastCodePoint = 0;
#ifdef JSON_USE_LONG_INTEGERS
    bool lastNumIsFloat = false;
#endif
    bool end_ = false;
    bool error_ = false;
    bool begin_ = true;
    bool inside_key_ = false; // need to be stored, because we use the same functions for string and key handling of special characters
    char const* err_ = nullptr;
    /**
     * @brief lastValue_ stores the last value. Is currently still needed only for number parsing
     */
    buffer_type_ lastValue_;
    /**
     * @brief first_ stores the first char of the data currently being processed. Used for strings
     * and keys
     */
    char_type_* first_ = nullptr;
    char_type_* end_buf_ = nullptr;

    void pop_state();

    bool make_error(char const* message);

    bool consume_initial_(char_type_* c);
    bool consume_arraystart_(char_type_* c);
    bool consume_arrayseparator_(char_type_* c);
    bool consume_objectseparator_(char_type_* c);
    bool consume_array_(char_type_* c);
    bool consume_objectstart_(char_type_* c);
    bool consume_objectbetween_(char_type_* c);
    bool consume_objectkey_(char_type_* c);
    bool consume_objectvalue_(char_type_* c);
    bool consume_falsef_(char_type_* c);
    bool consume_falsea_(char_type_* c);
    bool consume_falsel_(char_type_* c);
    bool consume_falses_(char_type_* c);
    bool consume_truet_(char_type_* c);
    bool consume_truer_(char_type_* c);
    bool consume_trueu_(char_type_* c);
    bool consume_nulln_(char_type_* c);
    bool consume_nullu_(char_type_* c);
    bool consume_nulll_(char_type_* c);
    bool consume_stringkey_(char_type_* c);
    bool consume_string_(char_type_* c);
    bool consume_stringescape_(char_type_* c);
    bool consume_stringuvalue_(char_type_* c);
    bool consume_stringuvalue1_(char_type_* c);
    bool consume_stringuvalue2_(char_type_* c);
    bool consume_stringuvalue3_(char_type_* c);
    bool consume_stringuvalue4_(char_type_* c);
    bool consume_stringuvalue5_(char_type_* c);
    bool consume_stringuvalue6_(char_type_* c);
    bool consume_stringuvalue7_(char_type_* c);
    bool consume_stringuvalue8_(char_type_* c);
    bool consume_stringuvalue9_(char_type_* c);
    bool consume_numberstarting0_(char_type_* c);
    bool consume_number_(char_type_* c);
    bool consume_inerror_(char_type_*);
public:
    bool consume_(char_type_* c);
private:
    void push_consumer_(consumer_fn fn)
    {
        consumer_stack_.push_back(fn);
    }
    /** Internals, used for factorization purpose */
   bool consume_array_itembegin_(char_type_* char_);
public:
    using buffer_type = buffer_type_;
    using char_type = char_type_;
    parser_bits() {
        consumer_stack_.push_back(&parser_bits::consume_initial_);
    }

    /**
     * @brief consume reads one character and parses it accordingly
     * Assume non contiguity of chars between calls, so is slow
     * @param char_ the character to read. May be part of a multibyte (utf-8) character
     * @return true if parser did not make any error, false otherwise
     */
    bool consume(char_type_ char_);
    /**
     * @brief consume reads all characters inside the array
     * @param begin start of array
     * @param end end of array
     * @return true if parser did not make any error, false otherwise
     */
    template<typename char_type_iterator>
    bool consume(char_type_iterator begin, char_type_iterator end);

    /**
     * @brief end Ends the current parser. Can be called when reading from an entire stream. Will indicate if parsing
     * was incomplete.
     * @return true if parsing ok, false otherwise
     */
    bool end();

    /**
     * @brief complete_without_error tells whether the parser consumed a complete document / item. Can be used to stop parsing when the item is complete.
     * It is not mandatory to call end().
     * @return true if parser has completed a top level item without error
     */
    bool complete_without_error() const;

    /**
     * @brief error_message returns a pointer to an error message set by the parser. Do not free this message.
     * @return null pointer if no error, pointer to a NULL terminated string otherwise.
     */
    char const* error_message() const;

};

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
void parser_bits<container,parser_callbacks, buffer_type_, char_type_>::pop_state()
{
      consumer_stack_.pop_back();
      if(consumer_stack_.size() == 1) // only the initial consumer is left
        end_ = true;
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_initial_(char_type_ *char_)
{
    if(char_ == nullptr)
        return !error_;
    if(token_helper<char_type_>::is_token_opening_curly_bracket(*char_))
    {
        if(!begin_)
            return make_error("Extra data (object) after document");
        begin_ = false;
        push_consumer_(&parser_bits::consume_objectstart_);
        return parser_callbacks::begin_object_handler() ||
            make_error("begin_object_handler_begin failed");
    }
    if(token_helper<char_type_>::is_token_opening_square_bracket(*char_))
    {
        if(!begin_)
            return make_error("Extra data (array) after document");
        begin_ = false;
        push_consumer_(&parser_bits::consume_arraystart_);
        return parser_callbacks::begin_array_handler() ||
            make_error("begin_array_handler_begin failed");
    }
    if(token_helper<char_type_>::is_token_double_quote(*char_))
    {
        if(!begin_)
        {
            return make_error("Extra data (string) after document");
        }
        begin_ = false;
        first_ = char_ + 1;
        end_buf_ = char_ + 1;
        push_consumer_(&parser_bits::consume_string_);
        return parser_callbacks::begin_string_handler() ||
                make_error("begin_string_handler_begin failed");
    }
    if(token_helper<char_type_>::is_token_space(*char_))
        return true;
    return make_error("Invalid token in Initial");
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_array_itembegin_(char_type_* char_)
{
    if(char_ == nullptr)
        return !error_;
    if(token_helper<char_type_>::is_token_double_quote(*char_))
    {
        push_consumer_(&parser_bits::consume_string_);
        return parser_callbacks::begin_string_handler();
    }
    if(token_helper<char_type_>::is_token_opening_curly_bracket(*char_))
    {
        push_consumer_(&parser_bits::consume_objectstart_);
        return parser_callbacks::begin_object_handler() ||
                make_error("begin_object_handler_array Failed");
    }
    if(token_helper<char_type_>::is_token_opening_square_bracket(*char_))
    {
        push_consumer_(&parser_bits::consume_arraystart_);
        return parser_callbacks::begin_array_handler() ||
            make_error("begin_array_handler_array Failed");
        return true;
    }
    if(token_helper<char_type_>::is_token_minussign(*char_) ||
            token_helper<char_type_>::is_token_digit19(*char_))
    {
        helper_functions<buffer_type_, char_type_>::truncate(lastValue_);
        helper_functions<buffer_type_, char_type_>::append(lastValue_, *char_);
#ifdef JSON_USE_LONG_INTEGERS
        lastNumIsFloat = false;
#endif
        push_consumer_(&parser_bits::consume_number_);
        return true;
    }
    if(token_helper<char_type_>::is_token_zero(*char_))
    {
        helper_functions<buffer_type_, char_type_>::truncate(lastValue_);
        helper_functions<buffer_type_, char_type_>::append(lastValue_, *char_);
#ifdef JSON_USE_LONG_INTEGERS
        lastNumIsFloat = false;
#endif
        push_consumer_(&parser_bits::consume_numberstarting0_);
        return true;
    }
    if(token_helper<char_type_>::is_token_char_n(*char_))
    {
        push_consumer_(&parser_bits::consume_nulln_);
        return true;
    }
    if(token_helper<char_type_>::is_token_char_t(*char_))
    {
        push_consumer_(&parser_bits::consume_truet_);
        return true;
    }
    if(token_helper<char_type_>::is_token_char_f(*char_))
    {
        push_consumer_(&parser_bits::consume_falsef_);
        return true;
    }
    return false;
}


template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_arraystart_(char_type_ * char_)
{
    if(char_ == nullptr)
        return !error_;
    if(token_helper<char_type_>::is_token_space(*char_))
        return true;
    if(token_helper<char_type_>::is_token_closing_square_bracket(*char_))
    {
        pop_state();
        return parser_callbacks::end_array_handler() || make_error("end_array_handler failed");
    }
    consumer_stack_.back() = &parser_bits::consume_array_; // in every other case, consider now to be inside the array
    if(!consume_array_itembegin_(char_))
    {
        if(!err_)
            return make_error("Invalid token in ArraySeparator");
    }
    return true;
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_arrayseparator_(char_type_* char_)
{
    if(char_ == nullptr)
        return !error_;
    if(token_helper<char_type_>::is_token_space(*char_))
        return true;
    consumer_stack_.back() = &parser_bits::consume_array_;
    if(!consume_array_itembegin_(char_))
    {
        if(!err_)
            return make_error("Invalid token in ArraySeparator");
    }
    return true;
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_objectseparator_(char_type_ *char_)
{
    if(char_ == nullptr)
        return !error_;
    if(token_helper<char_type_>::is_token_space(*char_))
        return true;
    consumer_stack_.back() = &parser_bits::consume_objectvalue_;
    if(token_helper<char_type_>::is_token_double_quote(*char_))
    {
        push_consumer_(&parser_bits::consume_string_);
        return parser_callbacks::begin_string_handler();
    }
    if(token_helper<char_type_>::is_token_opening_curly_bracket(*char_))
    {
        push_consumer_(&parser_bits::consume_objectstart_);
        return parser_callbacks::begin_object_handler() ||
            make_error("begin_object_handler_object_value failed");
        return true;
    }
    if(token_helper<char_type_>::is_token_opening_square_bracket(*char_))
    {
        push_consumer_(&parser_bits::consume_arraystart_);
        return parser_callbacks::begin_array_handler() ||
            make_error("begin_array_handler_object_value failed");
        return true;
    }
    if(token_helper<char_type_>::is_token_minussign(*char_) ||
            token_helper<char_type_>::is_token_digit19(*char_))
    {
        helper_functions<buffer_type_, char_type_>::truncate(lastValue_);
        helper_functions<buffer_type_, char_type_>::append(lastValue_, *char_);
#ifdef JSON_USE_LONG_INTEGERS
        lastNumIsFloat = false;
#endif
        push_consumer_(&parser_bits::consume_number_);
        return true;
    }
    if(token_helper<char_type_>::is_token_zero(*char_))
    {
        helper_functions<buffer_type_, char_type_>::truncate(lastValue_);
        helper_functions<buffer_type_, char_type_>::append(lastValue_, *char_);
#ifdef JSON_USE_LONG_INTEGERS
        lastNumIsFloat = false;
#endif
        push_consumer_(&parser_bits::consume_numberstarting0_);
        return true;
    }
    if(token_helper<char_type_>::is_token_char_n(*char_))
    {
        push_consumer_(&parser_bits::consume_nulln_);
        return true;
    }
    if(token_helper<char_type_>::is_token_char_t(*char_))
    {
        push_consumer_(&parser_bits::consume_truet_);
        return true;
    }
    if(token_helper<char_type_>::is_token_char_f(*char_))
    {
        push_consumer_(&parser_bits::consume_falsef_);
        return true;
    }
    return make_error("Unexpected token in ObjectSeparator");
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_array_(char_type_ * char_)
{
    if(char_ == nullptr)
        return !error_;
    if(token_helper<char_type_>::is_token_space(*char_))
        return true;
    if(token_helper<char_type_>::is_token_closing_square_bracket(*char_))
    {
        pop_state();
        return parser_callbacks::end_array_handler() || make_error("end array handler failed");
    }
    if(token_helper<char_type_>::is_token_comma(*char_))
    {
        consumer_stack_.back() = &parser_bits::consume_arrayseparator_;
        return true;
    }
    return make_error("Unexpected token in Array");
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_objectstart_(char_type_* char_)
{
    if(char_ == nullptr)
        return !error_;
    if(token_helper<char_type_>::is_token_space(*char_))
        return true;
    if(token_helper<char_type_>::is_token_closing_curly_bracket(*char_))
    {
        pop_state();
        return parser_callbacks::end_object_handler() || make_error("end_object_handler failed");
    }
    consumer_stack_.back() = &parser_bits::consume_objectkey_;
    if(token_helper<char_type_>::is_token_double_quote(*char_))
    {
        push_consumer_(&parser_bits::consume_stringkey_);
        return parser_callbacks::begin_key_handler();
    }
    return make_error("Unexpected token in ObjectStart");
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_objectbetween_(char_type_* char_)
{
    if(char_ == nullptr)
        return !error_;
    if(token_helper<char_type_>::is_token_space(*char_))
        return true;
    if(token_helper<char_type_>::is_token_double_quote(*char_))
    {
        consumer_stack_.back() = &parser_bits::consume_objectkey_;
        push_consumer_(&parser_bits::consume_stringkey_);
        return parser_callbacks::begin_key_handler();
    }
    return make_error("Unexpected token in ObjectBetween");
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_objectkey_(char_type_* char_)
{
    if(char_ == nullptr)
        return !error_;
    if(token_helper<char_type_>::is_token_space(*char_))
        return true;
    if(token_helper<char_type_>::is_token_colon(*char_))
    {
        consumer_stack_.back() = &parser_bits::consume_objectseparator_;
        return true;
    }
    return make_error("Unexpected token in ObjectKey");
}
 
template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_objectvalue_(char_type_* char_)
{
    if(char_ == nullptr)
        return !error_;
    if(token_helper<char_type_>::is_token_space(*char_))
        return true;
    if(token_helper<char_type_>::is_token_closing_curly_bracket(*char_))
    {
        pop_state();
        return parser_callbacks::end_object_handler() ||
            make_error("end_object_handler failed");
        return true;
    }
    if(token_helper<char_type_>::is_token_comma(*char_))
    {
        consumer_stack_.back() = &parser_bits::consume_objectbetween_;
        return true;
    }
    return make_error("Unexpected token in ObjectValue");
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_falsef_(char_type_* char_)
{
    if(char_ == nullptr)
        return !error_;
    if(token_helper<char_type_>::is_token_char_a(*char_))
    {
        consumer_stack_.back() = &parser_bits::consume_falsea_;
        return true;
    }
    return make_error("Unexpected token in False_F");
}
 
template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_falsea_(char_type_* char_)
{
    if(char_ == nullptr)
        return true;
    if(token_helper<char_type_>::is_token_char_l(*char_))
    {
        consumer_stack_.back() = &parser_bits::consume_falsel_;
        return true;
    }
    return make_error("Unexpected token in False_A");
}
 
template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_falsel_(char_type_* char_)
{
    if(char_ == nullptr)
        return true;
    if(token_helper<char_type_>::is_token_char_s(*char_))
    {
        consumer_stack_.back() = &parser_bits::consume_falses_;
        return true;
    }
    return make_error("Unexpected token in False_L");
}
 
template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_falses_(char_type_* char_)
{
    if(char_ == nullptr)
        return true;
    if(token_helper<char_type_>::is_token_char_e(*char_))
    {
        pop_state();
        return parser_callbacks::boolean_handler(false) || make_error("boolean_handler failed");
    }
    return make_error("Unexpected token in False_S");
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_truet_(char_type_* char_)
{
    if(char_ == nullptr)
        return true;
    if(token_helper<char_type_>::is_token_char_r(*char_))
    {
        consumer_stack_.back() = &parser_bits::consume_truer_;
        return true;
    }
    return make_error("Unexpected t    {""}oken in True_T");
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_truer_(char_type_* char_)
{
    if(char_ == nullptr)
        return true;
    if(token_helper<char_type_>::is_token_char_u(*char_))
    {
        consumer_stack_.back() = &parser_bits::consume_trueu_;
        return true;
    }
    return make_error("Unexpected token in True_R");
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_trueu_(char_type_* char_)
{
    if(char_ == nullptr)
        return true;
    if(token_helper<char_type_>::is_token_char_e(*char_))
    {
        pop_state();
        return parser_callbacks::boolean_handler(true) || make_error("boolean_handler_array failed");
    }
    return make_error("Unexpected token in True_U");
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_nulln_(char_type_* char_)
{
    if(char_ == nullptr)
        return true;
    if(token_helper<char_type_>::is_token_char_u(*char_))
    {
        consumer_stack_.back() = &parser_bits::consume_nullu_;
        return true;
    }
    return make_error("Unexpected token in Null_N");
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_nullu_(char_type_* char_)
{
    if(char_ == nullptr)
        return true;
    if(token_helper<char_type_>::is_token_char_l(*char_))
    {
        consumer_stack_.back() = &parser_bits::consume_nulll_;
        return true;
    }
    return make_error("Unexpected token in Null_U");
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_nulll_(char_type_* char_)
{
    if(char_ == nullptr)
        return true;
    if(token_helper<char_type_>::is_token_char_l(*char_))
    {
        pop_state();
        return parser_callbacks::null_handler() || make_error("null_handler failed");
    }
    return make_error("Unexpected token in Null_L");
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_stringkey_(char_type_* char_)
{
    if(char_ == nullptr)
    {
        if(first_ != nullptr)
        {
            bool res = parser_callbacks::key_content_handler(
                        helper_functions<buffer_type, char_type>::make_string_view(
                            first_, end_buf_ + 1 - first_));
            first_ = nullptr;
            return res;
        }
        return true;
    }
    if(token_helper<char_type_>::is_token_backslash(*char_))
    {
        inside_key_ = true;
        bool res = true;
	if(first_ != nullptr)
	    res = parser_callbacks::key_content_handler(
                        helper_functions<buffer_type, char_type>::make_string_view(
                        first_, char_ - first_));
        first_ = nullptr;
        consumer_stack_.push_back(&parser_bits::consume_stringescape_);
        return res;
    }
    else if(token_helper<char_type_>::is_token_double_quote(*char_))
    {
        pop_state();
        bool res = true;
        if(first_ != nullptr)
        {
            res = parser_callbacks::key_content_handler(
                        helper_functions<buffer_type, char_type>::make_string_view(
                            first_, char_ - first_));
            first_ = nullptr;
        }
        res = res && parser_callbacks::end_key_handler();
        return res || make_error("key handling failure");
    }
    else if(token_helper<char_type_>::is_token_forbidden_in_string(*char_))
        return make_error("Forbidden token in string");
    else
    {
        if(first_ == nullptr)
        {
            first_ = char_;
            end_buf_ = char_;
        }
        else
            ++end_buf_;
    }
    return true;
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container, parser_callbacks, buffer_type_, char_type_>::consume_string_(char_type_* char_)
{
    if(char_ == nullptr)
    {
        bool res = true;
        if(first_ != nullptr)
        {
            res = parser_callbacks::string_content_handler(
                       helper_functions<buffer_type, char_type>::make_string_view(
                           first_, end_buf_ + 1 - first_));
            first_ = nullptr;
        }
        return res;
    }
    if(token_helper<char_type_>::is_token_backslash(*char_))
    {
        inside_key_ = false;
        consumer_stack_.push_back(&parser_bits::consume_stringescape_);
        bool res = true;
        if(first_ != nullptr)
        {
            res = parser_callbacks::string_content_handler(
                        helper_functions<buffer_type, char_type>::make_string_view(first_, char_ - first_));
        }
        first_ = nullptr;
        return res;
    }
    else if(token_helper<char_type_>::is_token_double_quote(*char_))
    {
        pop_state();
        bool res = true;
        if(first_ != nullptr)
        {
            res = parser_callbacks::string_content_handler(
                        helper_functions<buffer_type, char_type>::make_string_view(
                            first_, char_ - first_));
        }
        first_ = nullptr;
        res = res && parser_callbacks::end_string_handler();
        return res || make_error("string handling failure");
    }
    else if(token_helper<char_type_>::is_token_forbidden_in_string(*char_))
        return make_error("Forbidden token in string");
    else
    {
        if(first_ == nullptr)
        {
            first_ = char_;
            end_buf_ = char_;
        }
        else
            ++end_buf_;
    }
    return true;
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_stringescape_(char_type_* char_)
{
    if(char_ == nullptr)
        return !error_;
    if(token_helper<char_type_>::is_token_char_u(*char_))
    {
        consumer_stack_.back() = &parser_bits::consume_stringuvalue_;
        lastCodePoint = 0;
        return true;
    }
    if(token_helper<char_type_>::is_token_backslash(*char_) ||
            token_helper<char_type_>::is_token_slash(*char_) ||
            token_helper<char_type_>::is_token_double_quote(*char_))
    {
        consumer_stack_.pop_back();
        if(inside_key_)
        {
            return parser_callbacks::key_content_handler(
                        helper_functions<buffer_type, char_type>::make_string_view(char_, 1));
        }
        else
        {
            return parser_callbacks::string_content_handler(
                    helper_functions<buffer_type, char_type>::make_string_view(char_, 1));
        }
    }
    if(token_helper<char_type_>::is_token_char_b(*char_))
    {
        consumer_stack_.pop_back();
        char_type c = static_cast<char_type>('\b');
        if(inside_key_)
        {
            return parser_callbacks::key_content_handler(
                        helper_functions<buffer_type, char_type>::make_string_view(&c, 1));
        }
        else
        {
            return parser_callbacks::string_content_handler(
                        helper_functions<buffer_type, char_type>::make_string_view(&c, 1));
        }
    }
    if(token_helper<char_type_>::is_token_char_f(*char_))
    {
        consumer_stack_.pop_back();
        char_type c = static_cast<char_type>('\f');
        if(inside_key_)
        {
            return parser_callbacks::key_content_handler(
                        helper_functions<buffer_type, char_type>::make_string_view(&c, 1));
        }
        else
        {
            return parser_callbacks::string_content_handler(
                        helper_functions<buffer_type, char_type>::make_string_view(&c, 1));
        }
    }
    if(token_helper<char_type_>::is_token_char_n(*char_))
    {
        consumer_stack_.pop_back();
        char_type c = static_cast<char_type>('\n');
        if(inside_key_)
        {
            return parser_callbacks::key_content_handler(
                        helper_functions<buffer_type, char_type>::make_string_view(&c, 1));
        }
        else
        {
            return parser_callbacks::string_content_handler(
                        helper_functions<buffer_type, char_type>::make_string_view(&c, 1));
        }
    }
    if(token_helper<char_type_>::is_token_char_r(*char_))
    {
        consumer_stack_.pop_back();
        char_type c = static_cast<char_type>('\r');
        if(inside_key_)
        {
            return parser_callbacks::key_content_handler(
                        helper_functions<buffer_type, char_type>::make_string_view(&c, 1));
        }
        else
        {
            return parser_callbacks::string_content_handler(
                        helper_functions<buffer_type, char_type>::make_string_view(&c, 1));
        }
    }
    if(token_helper<char_type_>::is_token_char_t(*char_))
    {
        consumer_stack_.pop_back();
        char_type c = static_cast<char_type>('\t');
        if(inside_key_)
        {
            return parser_callbacks::key_content_handler(
                        helper_functions<buffer_type, char_type>::make_string_view(&c, 1));
        }
        else
        {
            return parser_callbacks::string_content_handler(
                        helper_functions<buffer_type, char_type>::make_string_view(&c, 1));
        }
    }
    return make_error("Invalid token in StringEscape");
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_stringuvalue_(char_type_* char_)
{
    if(char_ == nullptr)
        return !error_;
    int8_t val = helper_functions<buffer_type, char_type>::hexdigit_val(*char_);
    if(val != -1)
    {
        lastCodePoint = val << 12;
        consumer_stack_.back() = &parser_bits::consume_stringuvalue1_;
        return true;
    }
    return make_error("Invalid token in unicode 1");
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_stringuvalue1_(char_type_* char_)
{
    if(char_ == nullptr)
        return !error_;
    int8_t val = helper_functions<buffer_type, char_type>::hexdigit_val(*char_);
    if(val != -1)
    {
        lastCodePoint += val << 8;
        consumer_stack_.back() = &parser_bits::consume_stringuvalue2_;
        return true;
    }
    return make_error("Invalid token in unicode 2");
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_stringuvalue2_(char_type_* char_)
{
    if(char_ == nullptr)
        return !error_;
    int8_t val = helper_functions<buffer_type, char_type>::hexdigit_val(*char_);
    if(val != -1)
    {
        lastCodePoint += val << 4;
        consumer_stack_.back() = &parser_bits::consume_stringuvalue3_;
        return true;
    }
    return make_error("Invalid token in unicode 3");
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_stringuvalue3_(char_type_* char_)
{
    if(char_ == nullptr)
        return !error_;
    int8_t val = helper_functions<buffer_type_, char_type_>::hexdigit_val(*char_);
    if(val != -1)
    {
        lastCodePoint += val;
        if(lastCodePoint >= 0xD800 && lastCodePoint < 0xDFFF) // utf16 pair, special care needed
        {
            lastCodePoint = lastCodePoint << 16;
            consumer_stack_.back() = &parser_bits::consume_stringuvalue4_;
            return true;
        }
        consumer_stack_.pop_back();
        helper_functions<buffer_type, char_type_>::truncate(lastValue_);
        helper_functions<buffer_type_,char_type_>::append_code_point(lastValue_, lastCodePoint);
        if(inside_key_)
        {
            return parser_callbacks::key_content_handler(
                        helper_functions<buffer_type, char_type>::make_string_view(lastValue_.data(), lastValue_.size()));
        }
        else
        {
            return parser_callbacks::string_content_handler(
                        helper_functions<buffer_type, char_type>::make_string_view(lastValue_.data(), lastValue_.size()));
        }
    }
    return make_error("Invalid token in StringUValue3");
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_stringuvalue4_(char_type_* char_)
{
    if(char_ == nullptr)
        return !error_;
    if(!token_helper<char_type_>::is_token_backslash(*char_))
        return make_error("Invalid token, expected backslash");
    consumer_stack_.back() = &parser_bits::consume_stringuvalue5_;
    return true;
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_stringuvalue5_(char_type_* char_)
{
    if(char_ == nullptr)
        return !error_;
    if(!token_helper<char_type_>::is_token_char_u(*char_))
        return make_error("Invalid token, expected char_u");
    consumer_stack_.back() = &parser_bits::consume_stringuvalue6_;
    return true;
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_stringuvalue6_(char_type_* char_)
{
    if(char_ == nullptr)
        return !error_;
    int8_t val = helper_functions<buffer_type, char_type>::hexdigit_val(*char_);
    if(val != -1)
    {
        lastCodePoint += val << 12;
        consumer_stack_.back() = &parser_bits::consume_stringuvalue7_;
        return true;
    }
    else
        return make_error("Invalid token in unicode 1");
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_stringuvalue7_(char_type_* char_)
{
    if(char_ == nullptr)
        return !error_;
    int8_t val = helper_functions<buffer_type, char_type>::hexdigit_val(*char_);
    if(val != -1)
    {
        lastCodePoint += val << 8;
        consumer_stack_.back() = &parser_bits::consume_stringuvalue8_;
        return true;
    }
    else
        return make_error("Invalid token in StringUValue7");
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_stringuvalue8_(char_type_* char_)
{
    if(char_ == nullptr)
        return !error_;
    int8_t val = helper_functions<buffer_type, char_type>::hexdigit_val(*char_);
    if(val != -1)
    {
        lastCodePoint += val << 4;
        consumer_stack_.back() = &parser_bits::consume_stringuvalue9_;
        return true;
    }
    else
        return make_error("Invalid token in StringUValue8");
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_stringuvalue9_(char_type_* char_)
{
    if(char_ == nullptr)
        return !error_;
    int8_t val = helper_functions<buffer_type, char_type>::hexdigit_val(*char_);
    if(val != -1)
    {
        lastCodePoint += val;
        consumer_stack_.pop_back();
        uint32_t codePointHigh = ((lastCodePoint >> 16) - 0xD800) << 10;
        uint32_t v = lastCodePoint & 0xFFFF;
        if(v < 0xDC00u)
            return make_error("Invalid unicode code point");
        uint32_t codePointLow = (v - 0xDC00);
        uint32_t val32 = codePointHigh + codePointLow + 0x10000;
        helper_functions<buffer_type_,char_type_>::truncate(lastValue_);
        helper_functions<buffer_type_,char_type_>::append_code_point(lastValue_, val32);
        return parser_callbacks::string_content_handler(
                    helper_functions<buffer_type, char_type>::make_string_view(lastValue_.data(), lastValue_.size()));
        return true;
    }
    else
        return make_error("Invalid token in StringUValue9");
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_numberstarting0_(char_type_* char_)
{
    if(char_ == nullptr)
        return !error_;
    if(token_helper<char_type_>::is_token_point(*char_)) // point ok after leading 0
    {
#ifdef JSON_USE_LONG_INTEGERS
        lastNumIsFloat = true;
#endif
        helper_functions<buffer_type,char_type_>::append(lastValue_, *char_);
        consumer_stack_.back() = &parser_bits::consume_number_;
        return true;
    }
    if(token_helper<char_type_>::is_token_char_E(*char_) ||
            token_helper<char_type_>::is_token_char_e(*char_))
    {
#ifdef JSON_USE_LONG_INTEGERS
        lastNumIsFloat = true;
#endif
        helper_functions<buffer_type,char_type_>::append(lastValue_, *char_);
        consumer_stack_.back() = &parser_bits::consume_number_;
        return true;
    }
    if(token_helper<char_type_>::is_token_digit19(*char_) ||
            token_helper<char_type_>::is_token_minussign(*char_) ||
            token_helper<char_type_>::is_token_plussign(*char_)
            )
    {
        return make_error("Invalid numeric format");
    }
    pop_state(); // char not part of a number, consider number finished
#ifdef JSON_USE_LONG_INTEGERS
    if(lastNumIsFloat)
    {
#endif
        auto val = helper_functions<buffer_type_, char_type_>::string_to_double(lastValue_);
        if(val.first)
        {
            return parser_callbacks::double_handler(val.second) && consume_(char_); // reconsume char, but outside number !
        }
        return make_error("Invalid double value");
#ifdef JSON_USE_LONG_INTEGERS
    }
    else
    {
        std::pair<bool, int64_t> val = helper_functions<buffer_type_, char_type_>::string_to_integer(lastValue);
        if(val.first)
        {
            return parser_callbacks::integer_handler(val.second) && consume(char_);
        }
        auto val2 = helper_functions<buffer_type_, char_type_>::string_to_double(lastValue);
        if(val2.first)
            return parser_callbacks::double_handler(val2.second) && consume(char_); // reconsume char, but outside number !
        return make_error("Invalid integer");
    }
#endif
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_number_(char_type_* char_)
{
#ifdef JSON_USE_LONG_INTEGERS
    if(token_helper<char_type_>::is_token_point(*char_))
        lastNumIsFloat = true;
#endif
    if(char_ == nullptr)
        return !error_;
    if(token_helper<char_type_>::is_token_digit19(*char_) ||
            token_helper<char_type_>::is_token_zero(*char_) ||
            token_helper<char_type_>::is_token_char_E(*char_) ||
            token_helper<char_type_>::is_token_char_e(*char_) ||
            token_helper<char_type_>::is_token_minussign(*char_) ||
            token_helper<char_type_>::is_token_plussign(*char_) ||
            token_helper<char_type_>::is_token_point(*char_))
    {
        helper_functions<buffer_type,char_type>::append(lastValue_, *char_);
        return true;
    }
    pop_state(); // quit number state : char is not part of number
#ifdef JSON_USE_LONG_INTEGERS
    if(lastNumIsFloat)
    {
#endif
        auto val = helper_functions<buffer_type_, char_type_>::string_to_double(lastValue_);
        if(val.first)
        {
            return parser_callbacks::double_handler(val.second) && consume_(char_); // reconsume char, but outside number !
        }
        return make_error("Invalid double value");
#ifdef JSON_USE_LONG_INTEGERS
    }
    else
    {
        auto val = helper_functions<buffer_type_, char_type_>::string_to_integer(lastValue_);
        if(val.first)
        {
            return parser_callbacks::integer_handler(val.second) && consume(*char_);
        }
        else // may be an overflow : try to read as double
        {
            auto val2 = helper_functions<buffer_type_, char_type_>::string_to_double(lastValue_);
            return parser_callbacks::double_handler(val2.second) && consume(char_); // reconsume char, but outside number !
        }
        return make_error("Invalid integer");
    }
#endif
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_inerror_(char_type_*)
{
    return false;
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume_(char_type_* c)
{
    return (this->*consumer_stack_.back())(c);
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume(char_type_ c)
{
    return consume_(&c) &&
           consume_(nullptr);
}

template<template<class> class container,
  typename parser_callbacks, typename buffer_type_, typename char_type_>
template<typename char_type_iterator>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::consume(char_type_iterator begin, char_type_iterator end)
{
    bool good = true;
    while(begin != end && good)
    {
        good = consume_(begin);
        ++begin;
    }
    good = (this->*consumer_stack_.back())(nullptr);
    return good;
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::end()
{
    if(consumer_stack_.size() != 1 || begin_)
        error_ = true;
    end_ = true;
    return !error_;
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container,parser_callbacks, buffer_type_, char_type_>::complete_without_error() const
{
    return end_ && !begin_ && !error_;
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
bool parser_bits<container, parser_callbacks, buffer_type_, char_type_>::make_error(char const* err)
{
    consumer_stack_.clear();
    consumer_stack_.push_back(&parser_bits::consume_inerror_);
    error_ = true;
    err_ = err;
    return false;
}

template<template<class> class container,
typename parser_callbacks,typename buffer_type_, typename char_type_>
char const * parser_bits<container, parser_callbacks, buffer_type_, char_type_>::error_message() const
{
    return err_;
}

}
}


#endif // JBC_JSON_PARSER_BITS_H
