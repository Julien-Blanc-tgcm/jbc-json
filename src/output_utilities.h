//          Copyright Julien Blanc 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef JBC_JSON_OUTPUTUTILITIES_H
#define JBC_JSON_OUTPUTUTILITIES_H

#include <cstdint>

namespace jbc
{
namespace json
{

template<int>
struct unsigned_char_
{

};

template<>
struct unsigned_char_<1>
{
    using char_type = unsigned char;
};

template<>
struct unsigned_char_<2>
{
    using char_type=std::uint16_t;
};

template<typename T>
struct unsigned_char {
    using char_type = typename unsigned_char_<sizeof(T)>::char_type;
};

template<typename char_type>
char_type hexchar(std::uint32_t val)
{
    if(val >= 10 && val < 16)
        return static_cast<char_type>('A' + val - 10);
    if (val < 10)
        return static_cast<char_type>('0' + val);
    return static_cast<char_type>('?');
}


}
}

#endif // JBC_JSON_OUTPUTUTILITIES_H
