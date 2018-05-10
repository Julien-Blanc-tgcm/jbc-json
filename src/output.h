//          Copyright Julien Blanc 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef JBC_JSON_OUTPUT_H
#define JBC_JSON_OUTPUT_H

#include <DBC/contracts.h>
#include <array>
#include <stdlib.h>
#include <memory>
#include "output_utilities.h"
#include "basic_item.h"

namespace jbc
{
namespace json
{

struct locator
{
    /**
     * @brief position is the position inside the item
     */
    int position = 0;
    /**
     * @brief sub_position is a sub position, used when the item needs transformation.
     * For example, when outputting utf8 characters, position means the position inside the string,
     * and subposition means the position inside the serialization
     *
     */
    int sub_position = 0;
    /**
     * @brief position_in_subitem is the position in the sub item. for example, in case of an object, position
     * means the nth item, and position_in_subitem will means the position where we stopped inside this item.
     * depth is not limited
     */
    std::unique_ptr<locator> position_in_subitem;
    void reset() {
        position = 0;
        sub_position = 0;
        position_in_subitem = nullptr;
    }
};


template <typename char_type>
class output
{
private:
    static bool is_basic_printed(char_type c);

    static std::uint32_t char_value(char_type c)
    {
        typename unsigned_char<char_type>::char_type c2 = c;
        return static_cast<std::uint32_t>(c2);
    }

    /**
     * writes a char character to the buffer buf, taken from the string val.
     * it may require to read additional characters from the string, this will
     * be reported in the return value
     * @return a pair, containing :
     *   @li first : the size of the data
     *   @li second : the number of lookahead characters
     */
    template<typename string_type, typename buffer>
    static std::pair<int,int> char_to_tmp(string_type const& val, buffer& buf, locator& loc);

    template<typename string_type>
    static void append_single_codepoint(string_type& val, std::uint32_t codepoint);
public:

    /**
     * Outputs a single string delimiter, aka « " ».
     * @return true if can be written
     */
    template<typename buffer>
    static bool string_delimiter(buffer& buf, int& offset);

    /**
     * Outputs a single number into the buffer. If it does not fit into buffer, will output what it can, and
     * set the locator accordingly.
     * @return true if written completely, false otherwise
     * @remark When returning true, the locator is reset.
     */
    template<typename buffer>
    static bool number(double number, int precision, locator& loc, buffer& buf, int& offset);

    /**
     * Outputs a single boolean into the buffer. If it does not fit into buffer, will output what it can, and
     * set the locator accordingly.
     * @return true if written completely, false otherwise
     * @remark When returning true, the locator is reset.
     */
    template<typename buffer>
    static bool boolean(bool value, locator& loc, buffer& buf, int& offset);

    /**
     * Outputs a single null value into the buffer. If it does not fit into buffer, will output what it can, and
     * set the locator accordingly.
     * @return true if written completely, false otherwise
     * @remark When returning true, the locator is reset.
     */
    template<typename buffer>
    static bool null(locator& loc, buffer& buf, int& offset);

    template<typename buffer, typename string_type>
    static bool string(string_type const& value, locator& loc, buffer& buf, int& offset);

    /**
     * Outputs the object start, aka « { »
     */
    template<typename buffer>
    static bool object_start(buffer& buf, int& offset);

    /**
     * Outputs the object key value separator, aka « : »
     */
    template<typename buffer>
    static bool object_keyvalueseparator(buffer& buf, int& offset);

    /**
     * Outputs the object separator, aka « , »
     */
    template<typename buffer>
    static bool object_separator(buffer& buf, int& offset);

    /**
     * Outputs the object end, aka « } ».
     */
    template<typename buffer>
    static bool object_end(buffer& buf, int& offset);

    /**
     * outputs a whole object into the buffer
     */
    template<typename buffer, typename item>
    static bool object(
            typename item::traits::object_const_iterator begin,
            typename item::traits::object_const_iterator end,
            locator& loc, buffer& buf, int& offset);

    template<typename buffer>
    static bool array_start(buffer& buf, int& offset);

    template<typename buffer>
    static bool array_separator(buffer& buf, int& offset);

    template<typename buffer>
    static bool array_end(buffer& buf, int& offset);

    template<typename buffer, typename item>
    static bool array(
            typename item::traits::array_const_iterator begin,
            typename item::traits::array_const_iterator end,
            locator& loc, buffer& buf, int& offset);

};

/**
 * @brief output_visitor is used to visit json items to call the print functions according
 * to the item type
 *
 * Outputs an item into the buffer, including all its child items. If it does not fit into buffer,
 * will output what it can, and sets the locator accordingly
 * @return true if written completely, false otherwise when calling visit
 * @remark when returning true, the locator is reset
 */
template<typename buffer, typename item, typename char_type>
class output_visitor : public boost::static_visitor<bool>
{
    buffer& buf_;
    int& offset_;
    jbc::json::locator& loc_;
public:
    output_visitor(buffer& buf, int& offset, jbc::json::locator& loc) :
        buf_{buf},
        offset_{offset},
        loc_{loc}
    {

    }

    bool operator()(double value)
    {
        return output<char_type>::number(value, 10, loc_, buf_, offset_);
    }

    bool operator()(typename item::traits::string_type const& value)
    {
        return output<char_type>::string(value, loc_, buf_, offset_);
    }

    bool operator()(typename item::traits::array_type const& value)
    {
        typename item::traits::array_const_iterator beg = value.cbegin();
        typename item::traits::array_const_iterator end = value.cend();
        return output<char_type>::template array<buffer, item>(beg, end, loc_, buf_, offset_);
    }

    bool operator()(typename item::traits::object_type const& value)
    {
        typename item::traits::object_const_iterator beg = value.cbegin();
        typename item::traits::object_const_iterator end = value.cend();
        return output<char_type>::template object<buffer, item>(beg, end, loc_, buf_, offset_);
    }

    bool operator()(bool value)
    {
        return output<char_type>::boolean(value, loc_, buf_, offset_);
    }
    bool operator()(boost::blank)
    {
        return output<char_type>::null(loc_, buf_, offset_);
    }
};

template<typename char_type>
bool output<char_type>::is_basic_printed(char_type c)
{
    unsigned int v = char_value(c);
    if(c < 0x20) // non printable, includes \t, \n, \r, \f, \b
        return false;
    else if(v >= 0xA0) // not a basic character, needs special care
        return false;
    else if(c == char_type{'\\'})
        return false;
    else if(c == char_type{'"'})
        return false;
    else
        return true;
}

template<typename char_type>
template<typename string_type>
void output<char_type>::append_single_codepoint(string_type& str, std::uint32_t codepoint)
{
    str += hexchar<char_type>(codepoint >> 12);
    str += hexchar<char_type>((codepoint & 0xF00)>> 8);
    str += hexchar<char_type>((codepoint &0xF0) >> 4);
    str += hexchar<char_type>(codepoint & 0xF);
}

template<typename char_type>
template<typename buffer>
bool output<char_type>::number(double number, int precision, locator& loc, buffer& buf, int& offset)
{
    REQUIRE(precision < 40, "Precision cannot be too big");
    std::array<char, 50> chars;
    std::string format{"%."};
    format += std::to_string(precision);
    format += 'g';
    size_t data_size = sprintf(chars.data(), format.c_str(), number);
    using namespace std;
    auto buf_size = buf.size(); // should use std::size
    int writtensize = std::min(data_size - loc.position, buf_size - offset);
    std::copy(chars.data() + loc.position,
              chars.data() + loc.position + writtensize,
              buf.data() + offset);
    if(data_size - loc.position > buf_size - offset)
    {
        offset += writtensize;
        loc.position += writtensize;
        return false;
    }
    offset += writtensize;
    loc.reset();
    return true;
}

template<typename char_type>
template<typename buffer>
bool output<char_type>::boolean(bool value, locator& loc, buffer& buf, int& offset)
{
    char const* val = value ? "true":"false";
    size_t data_size = value ? 4 : 5;
    using namespace std;
    auto buf_size = buf.size();
    int writtensize = std::min(data_size - loc.position, buf_size - offset);
    std::copy(val + loc.position,
              val + loc.position + writtensize,
              buf.data() + offset);
    if(data_size - loc.position > buf_size - offset)
    {
        offset += writtensize;
        loc.position += writtensize;
        return false;
    }
    offset += writtensize;
    loc.reset();
    return true;
}

template<typename char_type>
template<typename buffer>
bool output<char_type>::null(locator& loc, buffer& buf, int& offset)
{
    char const* val = "null";
    size_t data_size = 4;
    using namespace std;
    auto buf_size = buf.size();
    int writtensize = std::min(data_size - loc.position, buf_size - offset);
    std::copy(val + loc.position,
              val + loc.position + writtensize,
              buf.data() + offset);
    if(data_size - loc.position > buf_size - offset)
    {
        offset += writtensize;
        loc.position += writtensize;
        return false;
    }
    offset += writtensize;
    loc.reset();
    return true;
}

template<typename char_type>
template<typename buffer>
bool output<char_type>::string_delimiter(buffer& buf, int& offset)
{
    if(buf.size() - offset > 0)
    {
        buf[offset] = char_type{'"'};
        offset += 1;
        return true;
    }
    else
        return false;
}

template<typename char_type>
template<typename buffer, typename string_type>
bool output<char_type>::string(string_type const& value, locator& loc, buffer& buf, int& offset)
{
    int current = loc.position;
    if(current == 0) // nothing written
    {
        if(string_delimiter(buf, offset))
            loc.position += 1;
        else
            return false;
    }
    int i = 0;
    for(i = loc.position - 1;
        ((i - loc.position + 1) < (static_cast<int>(buf.size()) - offset)) &&
        i < static_cast<long>(value.size()); ++i)
    {
        if(!is_basic_printed(value[i]))
        {
            // copy basic data
            std::copy(value.data() + loc.position - 1,
                      value.data() + i,
                      buf.data() + offset);
            offset += i - (loc.position - 1);
            loc.position = i + 1;
            // copy specific data
            std::array<char_type, 10> tmp;
            auto const size_and_advance = char_to_tmp(value, tmp, loc);
            auto const size = size_and_advance.first;
            auto const advance = size_and_advance.second;
            if(static_cast<int>(buf.size()) - offset >= size) // fits
            {
                std::copy(tmp.data(), tmp.data() + size, buf.data() + offset);
                offset += size;
                loc.position += advance + 1;
                i += advance;
                loc.sub_position = 0;
            }
            else // not fitting
            {
                std::copy(tmp.data(),
                          tmp.data() + buf.size() - offset,
                          buf.data() + offset);
                loc.sub_position += buf.size() - offset;
                offset = buf.size();
                return false; // no need to continue, there’s no room left
            }
        }
    }
    if(i - loc.position + 1 < static_cast<long>(buf.size()) - offset) // fits
    {
        std::copy(value.data() + loc.position - 1,
                  value.data() + i,
                  buf.data() + offset);
        offset += i - (loc.position - 1);
        if(buf.size() - offset >= 1)
        {
            string_delimiter(buf, offset);
            loc.reset();
            return true;
        }
        else
        {
            loc.position = i + 1;
            offset += i;
            return false;
        }
    }
    else
    { // does not fit
        std::copy(value.data() + loc.position - 1,
                  value.data() +i,
                  buf.data() + offset);
        offset += i - (loc.position - 1);
        loc.position = i + 1;
        return false;
    }
}

template<typename char_type>
template<typename string_type, typename buffer>
std::pair<int,int> output<char_type>::char_to_tmp(string_type const& value, buffer& buf, locator& loc)
{
    char_type c = value[loc.position - 1];
    int advance = 0;
    std::string str;
    str.reserve(20);// max size is 20;
    if(c == '\n')
        str = "\\n";
    else if(c == '\r')
        str = "\\r";
    else if(c == '\t')
        str = "\\t";
    else if(c == '\\')
        str = "\\\\";
    else if(c == '"')
        str = "\\\"";
    else if(c == '\f')
        str = "\\f";
    else if(c == '\b')
        str = "\\b";
    else
    {
        int codepoint;
        int v = static_cast<int>(char_value(c));
        if(v < 0x20)
        {
            str = "\\u00";
            if(v > 16)
            {
                str += hexchar<char_type>(v >> 4);
                v = v & 0xF;
            }
            else
                str += '0';
            str += hexchar<char_type>(v);
        }
        else if((c & 0xE0) == 0xC0) // 2 bytes unicode code point
        {
            codepoint = (v - 0xC0) << 6;
            if(value.size() < static_cast<size_t>(loc.position))
            {
                assert(false);// TODO : report error
            }
            v = static_cast<int>(char_value(value[loc.position]));
            codepoint += v - 0x80;
            advance = 1;
            str = "\\u";
            append_single_codepoint(str, codepoint);
        }
        else if((c & 0xF0) == 0xE0) // 3 bytes unicode code point
        {
            if(value.size() < static_cast<size_t>(loc.position + 1))
            {
                assert(false);// TODO : report error
            }
            codepoint = (v&0xF) << 12;
            v = static_cast<int>(char_value(value[loc.position]));
            codepoint += (v & 0x3F) << 6;
            v = static_cast<int>(char_value(value[loc.position + 1]));
            advance = 2;
            codepoint += v & 0x3F;
            str += "\\u";
            append_single_codepoint(str, codepoint);
        }
        else if((c & 0xF8) == 0xF0) // 4 bytes unicode code point
        {
            if(value.size() < static_cast<size_t>(loc.position + 2))
            {
                assert(false);// TODO : report error
            }
            codepoint = (v&0x7) << 18;
            v = static_cast<int>(char_value(value[loc.position]));
            codepoint += (v & 0x3F) << 12;
            v = static_cast<int>(char_value(value[loc.position + 1]));
            codepoint += (v & 0x3F) << 6;
            v = static_cast<int>(char_value(value[loc.position + 2]));
            advance = 3;
            codepoint += v & 0x3F;
            int highorder = ((codepoint & 0xFFFFFC00) - 0x10000) >> 10;
            int firstnumber = (highorder) + 0xD800;
            int secondnumber = (codepoint & 0x03FF) + 0xDC00;
            str += "\\u";
            append_single_codepoint(str, firstnumber);
            str += "\\u";
            append_single_codepoint(str, secondnumber);
        }
        else
        {
            assert(false);// TODO : report error
        }
    }
    std::copy(str.c_str() + loc.sub_position, str.c_str() + str.size(), buf.data());
    return std::make_pair(
            str.size() - loc.sub_position,
            advance);
}

template<typename char_type>
template<typename buffer>
bool output<char_type>::object_start(buffer& buf, int& offset)
{
    if(offset < static_cast<long>(buf.size()))
    {
        buf[offset] = char_type{'{'};
        offset += 1;
        return true;
    }
    else
        return false;
}

template<typename char_type>
template<typename buffer>
bool output<char_type>::object_keyvalueseparator(buffer& buf, int& offset)
{
    if(offset < static_cast<long>(buf.size()))
    {
        buf[offset] = char_type{':'};
        offset += 1;
        return true;
    }
    else
        return false;
}

template<typename char_type>
template<typename buffer>
bool output<char_type>::object_separator(buffer& buf, int& offset)
{
    if(offset < static_cast<long>(buf.size()))
    {
        buf[offset] = char_type{','};
        offset += 1;
        return true;
    }
    else
        return false;
}


template<typename char_type>
template<typename buffer>
bool output<char_type>::object_end(buffer& buf, int& offset)
{
    if(offset < static_cast<long>(buf.size()))
    {
        buf[offset] = char_type{'}'};
        offset += 1;
        return true;
    }
    else
        return false;
}

template<typename char_type>
template<typename buffer, typename item>
bool output<char_type>::object(
        typename item::traits::object_const_iterator begin,
        typename item::traits::object_const_iterator end,
        locator& loc, buffer& buf, int& offset)
{
    REQUIRE(obj.type() == json::ItemType::Object, "obj must be an object");
    REQUIRE(offset >= 0, "offset must be positive or null");
    if(loc.position == 0)
    {
        if(object_start(buf, offset))
            loc.position = 1;
        else
            return false;
    }
    if(static_cast<unsigned int>(offset) < buf.size())
    {
        auto it = begin; //obj.begin_object();
        std::advance(it, loc.position - 1);
        bool res = true;
        while(res && it != end) //obj.end_object())
        {
            if(loc.sub_position == 0)
            {
                locator subitem_location_empty;
                locator& subitem_location = loc.position_in_subitem != nullptr?*loc.position_in_subitem:subitem_location_empty;
                res = string(it->first, subitem_location, buf, offset);
                if(!res)
                {
                    loc.position_in_subitem.reset(new locator{std::move(subitem_location)});
                    return false;
                }
                loc.sub_position = 1;
                loc.position_in_subitem.reset(nullptr);
                if(offset == static_cast<int>(buf.size())) // cannot go further
                    return false;
            }
            if(loc.sub_position == 1)
            {
                object_keyvalueseparator(buf, offset);
                loc.sub_position = 2;
                if(offset == static_cast<int>(buf.size()))
                    return false;
            }
            if(loc.sub_position == 2)
            {
                locator subitem_location_empty;
                locator& subitem_location = loc.position_in_subitem != nullptr?*loc.position_in_subitem:subitem_location_empty;
                output_visitor<buffer, item, char_type> v{buf, offset, subitem_location};
                res = boost::apply_visitor(v, it->second);
                if(!res)
                {
                    loc.position_in_subitem.reset(new locator{std::move(subitem_location)});
                    return false;
                }
                loc.sub_position = 3;
                loc.position_in_subitem.reset(nullptr);
                if(offset == static_cast<int>(buf.size()))
                    return false; // cannot go further
            }
            ++it;
            loc.position += 1;
            loc.sub_position = 0;
            if(it != end) //obj.end_object())
            {
                object_separator(buf, offset);
                if(offset == static_cast<int>(buf.size())) // cannot go further
                    return false;
            }
        }
        if(it == end)
        {
            if(object_end(buf, offset))
            {
                loc.reset();
                return true;
            }
            else
            {
                return false;
            }
        }
        else
            return false;
    }
    else
        return false;
}

template<typename char_type>
template<typename buffer>
bool output<char_type>::array_start(buffer& buf, int& offset)
{
    if(offset < static_cast<long>(buf.size()))
    {
        buf[offset] = char_type{'['};
        offset += 1;
        return true;
    }
    else
        return false;
}

template<typename char_type>
template<typename buffer>
bool output<char_type>::array_separator(buffer& buf, int& offset)
{
    if(offset < static_cast<long>(buf.size()))
    {
        buf[offset] = char_type{','};
        offset += 1;
        return true;
    }
    else
        return false;
}

template<typename char_type>
template<typename buffer>
bool output<char_type>::array_end(buffer& buf, int& offset)
{
    if(offset < static_cast<long>(buf.size()))
    {
        buf[offset] = char_type{']'};
        offset += 1;
        return true;
    }
    else
        return false;
}


template<typename char_type>
template<typename buffer, typename item>
bool output<char_type>::array(
        typename item::traits::array_const_iterator begin,
        typename item::traits::array_const_iterator end,
        locator& loc, buffer& buf, int& offset)
{
    REQUIRE(offset >= 0, "offset must be positive or null");
    if(loc.position == 0)
    {
        if(array_start(buf, offset))
            loc.position = 1;
        else
            return false;
    }
    if(static_cast<unsigned int>(offset) < buf.size())
    {
        auto it = begin; //arr.begin_array();
        std::advance(it, loc.position - 1);
        bool res = true;
        while(res && it != end) //arr.end_array())
        {
            if(loc.sub_position == 0)
            {
                locator subitem_location_empty;
                locator& subitem_location = loc.position_in_subitem != nullptr?*loc.position_in_subitem:subitem_location_empty;
                output_visitor<buffer, item, char_type> v{buf, offset, subitem_location};
                res = boost::apply_visitor(v, *it);
                if(!res)
                {
                    loc.position_in_subitem.reset(new locator{std::move(subitem_location)});
                    return false;
                }
                loc.sub_position = 1;
                loc.position_in_subitem.reset(nullptr);
            }
            auto next = it;
            ++next;
            if(next != end) //arr.end_array())
            {
                if(array_separator(buf, offset))
                {
                    loc.position += 1;
                    loc.sub_position = 0;
                    ++it;
                }
                else
                    return false;
            }
            else
            {
                ++it;
                loc.position += 1;
                loc.sub_position = 0;
            }
        }
        assert(loc.position == std::distance(begin, end) + 1); //arr.child_count() + 1);
        if(array_end(buf, offset))
        {
            loc.reset();
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

template<typename char_type, typename item>
bool output_json(std::ostream& stream, item const& the_item)
{
    std::array<char_type, 32768> buf;
    jbc::json::locator loc;
    int offset = 0;
    output_visitor<decltype(buf), item, char_type> output{buf, offset, loc};
    bool res = false;
    while(!res && stream.good())
    {
        res = boost::apply_visitor(output, the_item);
        stream.write(buf.data(), offset);
        offset = 0;
    }
    return res && stream.good();
}

}
}

#endif
