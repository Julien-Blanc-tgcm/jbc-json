#ifndef NMC_LIBJSON_QT_JSON_H
#define NMC_LIBJSON_QT_JSON_H

#include <QPair>
#include <QString>
#include <QVector>
#include <QTextStream>

#define COPYABLEITEM
#include "libjson.h"

#undef COPYABLEITEM

namespace jbc
{
namespace json
{

struct qt_types
{
    using string_type = QString;
    using string_view = QString;
    typedef QChar char_type;
    using buffer_type=QVector<QChar>;
    typedef QVector<basic_item<qt_types> > array_type;
    typedef QVector<QPair<QString, basic_item<qt_types> > > object_type;
    typedef array_type::iterator array_iterator;
    typedef object_type::iterator object_iterator;
    typedef array_type::const_iterator array_const_iterator;
    typedef object_type::const_iterator object_const_iterator;
    template<typename... args>
    static void array_emplace_back(array_type& container, args... arg)
    {
        container.push_back(basic_item<qt_types>(arg...));
    }
    template<typename... args>
    static void object_emplace_back(object_type& container, args... arg)
    {
        container.push_back(QPair<QString, basic_item<qt_types> >(arg...));
    }
    static QString make_string(char const* str) {
        return QString::fromUtf8(str);
    }
    static QString make_string(QChar const* begin, QChar const* end)
    {
        return QString(begin, end - begin);
    }
    static int char_value(QChar c)
    {
        return c.unicode();
    }
    template<typename buffer>
    static int char_to_tmp(string_type const& value, buffer& buf, basic_locator& loc);
    static bool is_start_of_two_words_code_point(int value);
    static constexpr const bool is_utf8 = false;
    static void copy_basic_data(QChar const* first, QChar const* last, char* dest);
    static void copy_basic_data(QChar const* first, QChar const* last, QChar* dest);
};

template<>
struct helper_functions<QVector<QChar>, QChar>
{
/*    static ParserCharType token(QChar val)
    {
        return helper_functions<std::string>::token(val.toLatin1());
    } */

    static void append(QVector<QChar>& ref, QChar val) {
        ref.push_back(val);
    }

    static void append(QVector<QChar>& ref, char val) {
        ref.push_back(QChar::fromLatin1(val));
    }

    template<typename Iterator>
    static void append(QString& ref, Iterator beginit, Iterator endit)
    {
        for(auto it = beginit; it != endit; ++it)
        {
            ref.push_back(*it);
        }
    }

    static void append_code_point(QVector<QChar>& ref, uint32_t val)
    {
        ref.append(QChar(val));
    }

    static void append_special_char(QVector<QChar>& ref, SpecialChar char_)
    {
        switch(char_)
        {
            case SpecialChar::Backspace:
                ref.push_back(QChar::fromLatin1('\b'));
                break;
            case SpecialChar::CarriageReturn:
                ref.push_back(QChar::fromLatin1('\r'));
                break;
            case SpecialChar::FormFeed:
                ref.push_back(QChar::fromLatin1('\f'));
                break;
            case SpecialChar::NewLine:
                ref.push_back(QChar::fromLatin1('\n'));
                break;
            case SpecialChar::Tab:
                ref.push_back(QChar::fromLatin1('\t'));
                break;
        }
    }
    static void truncate(QVector<QChar>& ref) { ref.clear();ref.reserve(20); }
    static int8_t hexdigit_val(QChar char_) {
        char c = char_.toLatin1();
        if(c >= '0' && c <= '9')
            return c - '0';
        else if(c >= 'a' && c <= 'f')
            return c - 'a' + 10;
        else if(c >= 'A' && c <= 'F')
            return c - 'A' + 10;
        else
            return -1;
    }

    static std::pair<bool, int64_t> string_to_integer(QString const& ref)
    {
        std::pair<bool, int64_t> result;
        result.second = ref.toLongLong(&result.first);
        return result;
    }
    static QPair<bool, double> string_to_double(QVector<QChar> const& ref)
    {
        QPair<bool, double> result;
        QString s(ref.data(), ref.size());
        result.second = s.toDouble(&result.first);
        return result;
    }
    static QString make_string_view(QChar const* data, int size)
    {
        return QString::fromRawData(data, size);
    }
};

using QJsonItem=basic_item<qt_types>;
using QJsonItemBuilder=item_builder<QVector, QJsonItem>;
using QJsonParser=parser_bits<QVector, QJsonItemBuilder, QVector<QChar>, QChar>;

inline bool parse_from_stream(QTextStream & f, QJsonItem& destination)
{
    QJsonParser parser;
    constexpr const int buffersize = 65536;
    bool good = true;
    if(f.status() != QTextStream::Ok)
        return false;
    while(good && f.status() == QTextStream::Ok && !f.atEnd())
    {
        QString ret = f.read(buffersize);
        good = parser.consume(ret.data(), ret.data() + ret.size());
    }
    if(f.status() == QTextStream::Ok && f.atEnd() && good)
    {
        if(parser.end())
        {
            parser.moveTo(destination);
            return true;
        }
    }
    return false;
}

bool qt_types::is_start_of_two_words_code_point(int value)
{
    return value >= 0xD7FF && value < 0xE000;
}

void qt_types::copy_basic_data(const QChar *first, const QChar *last, char *dest)
{
    auto cur = dest;
    for(auto it = first; it != last; ++it,++cur)
    {
        *cur = static_cast<char>(it->unicode());
    }
}

void qt_types::copy_basic_data(const QChar *first, const QChar *last, QChar *dest)
{
    std::copy(first, last, dest);
}

template<typename buffer>
int qt_types::char_to_tmp(const string_type &value, buffer &buf, basic_locator &loc)
{
    char_type c = value[loc.position - 1];
    if(loc.codepointByte1 == 0) // no previous code point
    {
        QVector<typename buffer::value_type> str;
        if(c == char_type{'\n'})
        {
            helper_functions<decltype(str), typename buffer::value_type>::append(str, typename buffer::value_type{'\\'});
            helper_functions<decltype(str), typename buffer::value_type>::append(str, typename buffer::value_type{'n'});
        }
        else if(c == char_type{'\r'})
        {
            helper_functions<decltype(str), typename buffer::value_type>::append(str, typename buffer::value_type{'\\'});
            helper_functions<decltype(str), typename buffer::value_type>::append(str, typename buffer::value_type{'r'});
        }
        else if(c == char_type{'\t'})
        {
            helper_functions<decltype(str), typename buffer::value_type>::append(str, typename buffer::value_type{'\\'});
            helper_functions<decltype(str), typename buffer::value_type>::append(str, typename buffer::value_type{'t'});
        }
        else if(c == char_type{'\\'})
        {
            helper_functions<decltype(str), typename buffer::value_type>::append(str, typename buffer::value_type{'\\'});
            helper_functions<decltype(str), typename buffer::value_type>::append(str, typename buffer::value_type{'\\'});
        }
        else if(c == char_type{'"'})
        {
            helper_functions<decltype(str), typename buffer::value_type>::append(str, typename buffer::value_type{'\\'});
            helper_functions<decltype(str), typename buffer::value_type>::append(str, typename buffer::value_type{'\"'});
        }
        else if(c == char_type{'\f'})
        {
            helper_functions<decltype(str), typename buffer::value_type>::append(str, typename buffer::value_type{'\\'});
            helper_functions<decltype(str), typename buffer::value_type>::append(str, typename buffer::value_type{'f'});
        }
        else if(c == char_type{'\b'})
        {
            helper_functions<decltype(str), typename buffer::value_type>::append(str, typename buffer::value_type{'\\'});
            helper_functions<decltype(str), typename buffer::value_type>::append(str, typename buffer::value_type{'b'});
        }
        else
        {
            unsigned int v = char_value(c);
            if(v < 0x20u)
            {
                helper_functions<decltype(str), typename buffer::value_type>::append(str, typename buffer::value_type{'\\'});
                helper_functions<decltype(str), typename buffer::value_type>::append(str, typename buffer::value_type{'u'});
                helper_functions<decltype(str), typename buffer::value_type>::append(str, typename buffer::value_type{'0'});
                helper_functions<decltype(str), typename buffer::value_type>::append(str, typename buffer::value_type{'0'});
                if(v > 16u)
                {
                    helper_functions<decltype(str), typename buffer::value_type>::append(str, hexchar<typename buffer::value_type>(v >> 4u));
                    v = v & 0xFu;
                }
                else
                    helper_functions<decltype(str), typename buffer::value_type>::append(str, typename buffer::value_type{'0'});
                helper_functions<decltype(str), typename buffer::value_type>::append(str, hexchar<typename buffer::value_type>(v));
            }
            else if(is_start_of_two_words_code_point(v)) // 2 words utf16 code point
            {
                loc.codepointByte1 = v & 0xFFu;
                loc.codepointByte2 = (v >> 8u) & 0xFFu;
                return 0;
            }
            else
            {
                helper_functions<decltype(str), typename buffer::value_type>::append(str, typename buffer::value_type{'\\'});
                helper_functions<decltype(str), typename buffer::value_type>::append(str, typename buffer::value_type{'u'});
                output<typename buffer::value_type, basic_locator>::append_single_codepoint(str, v);
            }
        }
        std::copy(str.data() + loc.sub_position, str.data() + str.size(), buf.data());
        return str.size() - loc.sub_position;
    }
    else if(loc.codepointByte1 != 0) // a current double word utf16 character is being processed
    {
        auto v = char_value(c);
        auto high = loc.codepointByte1 + (loc.codepointByte2 << 8u);
        QVector<typename buffer::value_type> str;
        helper_functions<decltype(str), typename buffer::value_type>::append(str, typename buffer::value_type{'\\'});
        helper_functions<decltype(str), typename buffer::value_type>::append(str, typename buffer::value_type{'u'});
        output<typename buffer::value_type, basic_locator>::append_single_codepoint(str, high);
        helper_functions<decltype(str), typename buffer::value_type>::append(str, typename buffer::value_type{'\\'});
        helper_functions<decltype(str), typename buffer::value_type>::append(str, typename buffer::value_type{'u'});
        output<typename buffer::value_type, basic_locator>::append_single_codepoint(str, v);
        std::copy(str.data() + loc.sub_position, str.data() + str.size(), buf.data());
        return str.size() - loc.sub_position;
    }
    assert(false);
    return 0;
}

}
}

#endif // NMC_LIBJSON_QT_JSON_H
