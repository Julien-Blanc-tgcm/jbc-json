#ifndef JBC_JSON_PRINTER_H
#define JBC_JSON_PRINTER_H

#include "basic_item.h"
#include "utf8_printer.h"
#include "object_iterator.h"
#include <cassert>

namespace jbc
{
namespace json
{

/**
 * The printer class is used to output item into a « stream »
 */
template<typename Item_, typename stream = std::ostream>
class printer
{
private:
    static void print(Item_ const& item, stream& o, int double_precision);
    static typename Item_::traits::char_type hexchar(unsigned int val);
public:
    /**
     * @brief print an item to the stream
     * @param item item to print
     * @param pretty whether to use pretty (indented) printing
     * @param o output stream
     */
    static void print(Item_ const& item, bool pretty, stream& o, int double_precision = 10);
};

template<typename Item_, typename stream>
typename Item_::traits::char_type printer<Item_, stream>::hexchar(unsigned int val)
{
    if(val >= 10 && val < 16)
        return 'A' + val - 10;
    else
        return '0' + val;
    return '?';
}

template<typename Item_,typename stream>
class item_print_visitor : public boost::static_visitor<>
{
private:
    stream& stream_;
    int dprecision_;
public:
    item_print_visitor(stream& s, int double_precision) : stream_(s), dprecision_(double_precision) {}

    void operator()(bool v)const {
        stream_ << (v?"true":"false");
    }
    void operator()(double v)const {
        stream_.precision(dprecision_);
        stream_ << v;
    }
    void operator()(std::int64_t v)const {
        stream_ << v;
    }
    void operator()(boost::blank)const { stream_ << "null"; }
    void operator()(typename Item_::traits::string_type const& str)const {
            stream_ << "\"";
            utf8_printer<typename Item_::traits::string_type,
                         typename Item_::traits::char_type,stream>::
                             print_str(str, stream_);
            stream_ << "\"";
    }

    void operator()(typename Item_::traits::array_type const& arr)const {
        bool first = true;
        stream_ << '[';
        for(auto it = arr.begin(); it != arr.end(); ++it)
        {
            if(first)
                first = false;
            else
                stream_ << ',';
            boost::apply_visitor(*this, it->data_);
//            boost::apply_visitor(item_print_visitor<Item_,stream>{}, it->data_);
        }
        stream_ << ']';
    }
    void operator()(typename Item_::traits::object_type const& obj)const {
        stream_ << "{";
        bool first = true;
        for(auto const& item : obj)
        {
            if(first)
                first = false;
            else
                stream_ << ",";
            stream_ << "\"";
            auto const& val = item.first;
            utf8_printer<typename Item_::traits::string_type,
                         typename Item_::traits::char_type,stream>::
                         print_str(val, stream_);

            stream_ << "\":";
            boost::apply_visitor(*this, item.second.data_);
            //boost::apply_visitor(item_print_visitor<Item_,stream>{}, item.second.data_);
        }
        stream_ << "}";
    }
};

template<typename Item_, typename stream>
void printer<Item_, stream>::print(Item_ const& item, stream& o, int double_precision)
{
    boost::apply_visitor(item_print_visitor<Item_,stream>{o, double_precision}, item.data_);
}

template<typename Item_, typename stream>
void printer<Item_, stream>::print(Item_ const& item, bool /*pretty*/, stream& o, int double_precision)
{
    print(item, o, double_precision);
}

}
}

#endif // JBC_JSON_PRINTER_H
