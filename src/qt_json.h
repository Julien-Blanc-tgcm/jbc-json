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
    typedef QString string_type;
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
        return QString::fromRawData(begin, end - begin);
    }
    static int char_value(QChar c)
    {
        return c.unicode();
    }
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

}
}

#endif // NMC_LIBJSON_QT_JSON_H
