#ifndef JBC_JSON_HELPER_FUNCTIONS_H
#define JBC_JSON_HELPER_FUNCTIONS_H

#include <string> // for std::strtoll
#include <errno.h> // for errno
#include <iostream>

namespace jbc
{
namespace json
{

enum class SpecialChar
{
    Backspace,
    FormFeed,
    NewLine,
    CarriageReturn,
    Tab
};

/**
 * @brief The json_helper_functions is a class used to provide some static methods that depends on the
 * actual buffer_type and char_type.
 * When using a custom string class, it may be needed to specialize these functions for the parser to be
 * able to work. The default functions are designed for stl string, but should work with other types as
 * well, such as QString
 */
template<typename buffer_type_, typename char_type>
struct helper_functions
{
    /**
     * @brief This function has the same semantics as std::string.push_back()
     */
    static void append(buffer_type_& ref, char_type val) { ref.push_back(val); }
    /**
     * @deprecated
     */
    static void append_special_char(buffer_type_& ref, SpecialChar char_);
    /**
     * @brief This functions appends a char to the string, given its code point. This can result in multiple 
       characters being added to the string
     */
    static void append_code_point(buffer_type_& ref, uint32_t code_point);
    /**
     * @brief Truncates the string, reverting its size to 0;
     */
    static void truncate(buffer_type_& ref) { ref.resize(0);}
    /**
     * @brief returns the value from an hexadecimal char
     */
    static int8_t hexdigit_val(char_type char_);
    /**
     * @brief returns the integer value from the string value
     */
    static std::pair<bool, int64_t> string_to_integer(buffer_type_ /*const*/& ref) {
        std::pair<bool, int64_t> result;
        errno = 0;
        append_code_point(ref, 0);
        result.second = std::strtoll(&ref.front(), nullptr, 10);
        result.first = errno == 0;
        return result;
    }
    static std::pair<bool, double> string_to_double(buffer_type_ /*const*/& ref)
    {
        std::pair<bool, double> result;
        errno = 0;
        char_type* ret = nullptr;
        std::string s{ref.begin(),ref.end()};
        result.second = std::strtod(s.c_str(), &ret);
        result.first = errno == 0 && ret == (s.c_str() + s.size());
        return result;
    }
};

template<typename char_type>
struct token_helper
{
    static bool is_token_space(char_type var);
    static bool is_token_opening_square_bracket(char_type var);
    static bool is_token_closing_square_bracket(char_type var);
    static bool is_token_opening_curly_bracket(char_type var);
    static bool is_token_closing_curly_bracket(char_type var);
    static bool is_token_backslash(char_type var);
    static bool is_token_slash(char_type var);
    static bool is_token_colon(char_type var);
    static bool is_token_comma(char_type var);
    static bool is_token_double_quote(char_type var);
    static bool is_token_plussign(char_type var);
    static bool is_token_minussign(char_type var);
    static bool is_token_zero(char_type var);
    static bool is_token_digit19(char_type var);
    static bool is_token_point(char_type var);
    static bool is_token_char_E(char_type var);
    static bool is_token_char_a(char_type var);
    static bool is_token_char_b(char_type var);
    static bool is_token_char_e(char_type val);
    static bool is_token_char_f(char_type val);
    static bool is_token_char_l(char_type val);
    static bool is_token_char_n(char_type val);
    static bool is_token_char_r(char_type val);
    static bool is_token_char_s(char_type val);
    static bool is_token_char_t(char_type val);
    static bool is_token_char_u(char_type val);
    static bool is_token_other(char_type val);
    static bool is_token_forbidden_in_string(char_type val);
};

template<typename buffer_type, typename char_type>
int8_t helper_functions<buffer_type, char_type>::hexdigit_val(char_type char_)
{
    if(char_ >= '0' && char_ <= '9')
        return char_ - '0';
    else if(char_ >= 'a' && char_ <= 'f')
        return char_ - 'a' + 10;
    else if(char_ >= 'A' && char_ <= 'F')
        return char_ - 'A' + 10;
    else
        return -1;
}

template<typename char_type>
bool token_helper<char_type>::is_token_space(char_type val)
{
    return val == char_type(' ') ||
            val == char_type('\t') ||
            val == char_type('\n') ||
            val == char_type('\r');
}

template<typename char_type>
bool token_helper<char_type>::is_token_opening_square_bracket(char_type val)
{
    return val == char_type('[');
}

template<typename char_type>
bool token_helper<char_type>::is_token_closing_square_bracket(char_type val)
{
    return val == char_type(']');
}

template<typename char_type>
bool token_helper<char_type>::is_token_opening_curly_bracket(char_type val)
{
    return val == char_type('{');
}

template<typename char_type>
bool token_helper<char_type>::is_token_closing_curly_bracket(char_type val)
{
    return val == char_type('}');
}

template<typename char_type>
bool token_helper<char_type>::is_token_backslash(char_type val)
{
    return val == char_type('\\');
}

template<typename char_type>
bool token_helper<char_type>::is_token_slash(char_type val)
{
    return val == char_type('/');
}

template<typename char_type>
bool token_helper<char_type>::is_token_colon(char_type val)
{
    return val == char_type(':');
}

template<typename char_type>
bool token_helper<char_type>::is_token_comma(char_type val)
{
    return val == char_type(',');
}

template<typename char_type>
bool token_helper<char_type>::is_token_double_quote(char_type val)
{
    return val == char_type('"');
}

template<typename char_type>
bool token_helper<char_type>::is_token_plussign(char_type val)
{
    return val == char_type('+');
}

template<typename char_type>
bool token_helper<char_type>::is_token_minussign(char_type val)
{
    return val == char_type('-');
}

template<typename char_type>
bool token_helper<char_type>::is_token_zero(char_type val)
{
    return val == char_type('0');
}

template<typename char_type>
bool token_helper<char_type>::is_token_digit19(char_type val)
{
    return val >= char_type('1') && val <= char_type('9');
}

template<typename char_type>
bool token_helper<char_type>::is_token_point(char_type val)
{
    return val == char_type('.');
}

template<typename char_type>
bool token_helper<char_type>::is_token_char_E(char_type val)
{
    return val == char_type('E');
}

template<typename char_type>
bool token_helper<char_type>::is_token_char_a(char_type val)
{
    return val == char_type('a');
}

template<typename char_type>
bool token_helper<char_type>::is_token_char_b(char_type val)
{
    return val == char_type('b');
}

template<typename char_type>
bool token_helper<char_type>::is_token_char_e(char_type val)
{
    return val == char_type('e');
}

template<typename char_type>
bool token_helper<char_type>::is_token_char_f(char_type val)
{
    return val == char_type('f');
}

template<typename char_type>
bool token_helper<char_type>::is_token_char_l(char_type val)
{
    return val == char_type('l');
}

template<typename char_type>
bool token_helper<char_type>::is_token_char_n(char_type val)
{
    return val == char_type('n');
}

template<typename char_type>
bool token_helper<char_type>::is_token_char_r(char_type val)
{
    return val == char_type('r');
}

template<typename char_type>
bool token_helper<char_type>::is_token_char_s(char_type val)
{
    return val == char_type('s');
}

template<typename char_type>
bool token_helper<char_type>::is_token_char_t(char_type val)
{
    return val == char_type('t');
}

template<typename char_type>
bool token_helper<char_type>::is_token_char_u(char_type val)
{
    return val == char_type('u');
}

template<typename char_type>
bool token_helper<char_type>::is_token_other(char_type /*val*/)
{
    return true;
}


template<typename buffer_type_, typename char_type_>
void helper_functions<buffer_type_, char_type_>::append_special_char(buffer_type_ &ref, SpecialChar char_)
{
    switch(char_)
    {
        case SpecialChar::Backspace:
            ref.push_back((char_type_)'\b');
            break;
        case SpecialChar::CarriageReturn:
            ref.push_back((char_type_)'\r');
            break;
        case SpecialChar::FormFeed:
            ref.push_back((char_type_)'\f');
            break;
        case SpecialChar::NewLine:
            ref.push_back((char_type_)'\n');
            break;
        case SpecialChar::Tab:
            ref.push_back((char_type_)'\t');
            break;
    }
}

template<typename buffer_type_, typename char_type_>
void helper_functions<buffer_type_, char_type_>::append_code_point(buffer_type_ &ref, uint32_t code_point)
{
    switch(sizeof(char_type_))
    {
        case 1: // default implementation assume utf-8 coding
            if(code_point < 0x80u)
                append(ref, code_point);
            else if(code_point < 0x7FF) // two bytes, because less than 11 bits
            {
                uint8_t byte1 = 0xC0;
                uint8_t byte2 = 0x80;
                byte1 += code_point >> 6; // 5 higher bits
                byte2 += code_point & 0x3F; // 3F -> 6 lower bits
                append(ref, (char_type_) byte1);
                append(ref, (char_type_) byte2);
            }
            else if(code_point <= 0xFFFF) // code_point is less than 0xFFFF in current impl : 3 bytes because 16 bits
            {
                uint8_t byte1 = 0xE0;
                uint8_t byte2 = 0x80;
                uint8_t byte3 = 0x80;
                byte1 += code_point >> 12; // 4 higher bits
                byte2 += (code_point & 0xFC0) >> 6; // bits 6-11
                byte3 += code_point & 0x3F; // bits 0-5
                append(ref, (char_type_) byte1);
                append(ref, (char_type_) byte2);
                append(ref, (char_type_) byte3);
            }
            else if(code_point <= 0x200000) // 4 bytes
            {
		uint8_t bytes[5];
                bytes[0] = 0xF0;
                bytes[1] = 0x80;
                bytes[2] = 0x80;
                bytes[3] = 0x80;
                bytes[4] = 0;
                bytes[0] += code_point >> 18;
                bytes[1] += (code_point >> 12) & 0x3F;
                bytes[2] += (code_point >> 6) & 0x3F;
                bytes[3] += code_point & 0x3F;
                ref.insert(ref.end(), bytes, bytes + 4);//& += (char const*) bytes;
            }
            break;
        default:
            ref.push_back((char_type_) code_point);
            break;
    }
}

template<typename char_type>
bool token_helper<char_type>::is_token_forbidden_in_string(char_type val)
{
    return val >= 0 && val < 0x20;
}
  
}
}

#endif // JBC_JSON_HELPER_FUNCTIONS_H
