#ifndef JBC_JSON_UTF8_PRINTER
#define JBC_JSON_UTF8_PRINTER

#include <iostream>
#include "output_utilities.h"

namespace jbc
{
namespace json
{

template<typename stream, typename char_type, int>
struct multibyte_handler
{
};

template<typename stream, typename char_type>
struct multibyte_handler<stream, char_type, 1> {
    static void handle(stream& o, char_type c, int v, int& codepoint, int& nbdigits, int& currentdigit)
    {
        if(v < 0xA0)
            o << c;
        else if((v & 0xE0) == 0xC0) // 2 bytes unicode code point
        {
            o << "\\u";
            codepoint = (v - 0xC0) << 6;
            nbdigits = 2;
            currentdigit = 1;
        }
        else if((v & 0xF0) == 0xE0) // 3 bytes unicode code point
        {
            o << "\\u";
            nbdigits = 3;
            currentdigit = 1;
            codepoint = (v & 0xF) << 12;
        }
        else if((v & 0xF8) == 0xF0)// 4 bytes unicode code point
        {
            o << "\\u";
            nbdigits = 4;
            currentdigit = 1;
            codepoint = (v & 0x7) << 18;
        }
        else
        {
            std::cerr << "oops";
        }
    }
};

template<typename string, typename char_type = typename string::char_type, typename stream = std::ostream>
struct utf8_printer
{

    static void print_single_codepoint(std::uint32_t codepoint, stream& o)
    {
        o << hexchar<char_type>(codepoint >> 12);
        o << hexchar<char_type>((codepoint & 0xF00)>> 8);
        o << hexchar<char_type>((codepoint &0xF0) >> 4);
        o << hexchar<char_type>(codepoint & 0xF);
    }

    static std::uint32_t char_value(char_type c)
    {
        typename unsigned_char<char_type>::char_type c2 = c;
        return (std::uint32_t) c2;
    }

    static void print_str(string const& val, stream& o)
    {
        int codepoint = 0;
        int nbdigits = 0;
        int currentdigit = 0;
        for(char_type c : val)
        {
            if(nbdigits == 2)
            {
                unsigned int v = char_value(c);
                codepoint += v - 0x80;
                print_single_codepoint(codepoint, o);
                nbdigits = 0;
                currentdigit = 0;
                codepoint = 0;
            }
            else if(nbdigits == 3)
            {
                unsigned int v = char_value(c);
                codepoint += (v & 0x3F) << ((currentdigit == 1)?6:0);
                if(currentdigit == 2)
                {
                    print_single_codepoint(codepoint, o);
                    nbdigits = 0;
                    currentdigit = 0;
                    codepoint = 0;
                }
                else
                    currentdigit = 2;
            }
            else if(nbdigits == 4)
            {
                unsigned int v = char_value(c);
                codepoint += (v & 0x3F) << ((nbdigits - currentdigit - 1)*6);
                if(currentdigit == 3) // last digit
                {
                    int highorder = ((codepoint & 0xFFFFFC00) - 0x10000) >> 10;
                    int firstnumber = (highorder) + 0xD800;
                    int secondnumber = (codepoint & 0x03FF) + 0xDC00;
                    print_single_codepoint(firstnumber, o);
                    o << "\\u";
                    print_single_codepoint(secondnumber, o);
                    nbdigits = 0;
                    currentdigit = 0;
                    codepoint = 0;
                }
                else
                    currentdigit += 1;
            }
            else
            {
                if(c == '\n')
                    o << "\\n";
                else if(c == '\r')
                    o << "\\r";
                else if(c == '\t')
                    o << "\\t";
                else if(c == '\\')
                    o << "\\\\";
                else if(c == '/')
                    o << "\\/";
                else if(c == '"')
                    o << "\\\"";
                else if(c == '\f')
                    o << "\\f";
                else if(c == '\b')
                    o << "\\b";
                else
                {
                    unsigned int v = char_value(c);
                    if(v < 0x20)
                    {
                        o << "\\u00";
                        if(v > 16)
                        {
                            o << hexchar<char_type>(v >> 4);
                            v = v & 0xF;
                        }
                        else
                            o << '0';
                        o << hexchar<char_type>(v);
                    }
                    else
                    {
                        multibyte_handler<stream, char_type, sizeof(char_type)>::handle(o, c, v, codepoint, nbdigits, currentdigit);
                    }
                }
            }
        }
    }
};

}
}

#endif // JBC_JSON_UTF8_PRINTER
