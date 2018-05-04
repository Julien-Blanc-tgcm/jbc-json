#ifndef JBC_JSON_OUTPUTUTILITIES_H
#define JBC_JSON_OUTPUTUTILITIES_H

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
    typedef unsigned char char_type;
};

template<typename T>
struct unsigned_char {
    typedef typename unsigned_char_<sizeof(T)>::char_type char_type;
};

template<typename char_type>
char_type hexchar(std::uint32_t val)
{
    if(val >= 10 && val < 16)
        return static_cast<char_type>('A' + val - 10);
    else if (val < 10)
        return static_cast<char_type>('0' + val);
    return static_cast<char_type>('?');
}


}
}

#endif // JBC_JSON_OUTPUTUTILITIES_H
