//          Copyright Julien Blanc 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <qt_json.h>

#include <iostream>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE json_conformance
#include <boost/test/unit_test.hpp>

namespace utf = boost::unit_test;

BOOST_AUTO_TEST_CASE(numberoutput, *utf::description("Output a single number"))
{
    double d = 3.38778;
    jbc::json::output<QChar, jbc::json::basic_locator> o;
    std::array<QChar, 60> buf;
    jbc::json::basic_locator loc;
    int offset = 0;
    bool res = o.number(d, 6, loc, buf, offset);
    BOOST_TEST(offset == 7);
    BOOST_TEST(res);
    QString str{buf.data(), 7};
    bool testpassed = str == "3.38778";
    BOOST_TEST(testpassed);
}

BOOST_AUTO_TEST_CASE(numberoutput2, *utf::description("Output a single number at specific position"))
{
    double d = 3.38778;
    jbc::json::output<QChar, jbc::json::basic_locator> o;
    std::array<QChar, 60> buf;
    jbc::json::basic_locator loc;
    int offset = 10;
    bool res = o.number(d, 6, loc, buf, offset);
    BOOST_TEST(offset == 17);
    BOOST_TEST(res);
    QString str{buf.data() + 10, 7};
    bool testpassed = str == "3.38778";
    BOOST_TEST(testpassed);
}

BOOST_AUTO_TEST_CASE(numberoutput3, *utf::description("Output a number that does not fit"))
{
    double d = 3.38778;
    jbc::json::output<QChar> o;
    std::array<QChar, 5> buf;
    jbc::json::basic_locator loc;
    int offset = 1;
    bool res = o.number(d, 6, loc, buf, offset);
    BOOST_TEST(offset == 5);
    BOOST_TEST(loc.position == 4);
    BOOST_TEST(!res);
    QString str{buf.data() + 1, 4};
    bool testpassed = str == "3.38";
    BOOST_TEST(testpassed);
}

BOOST_AUTO_TEST_CASE(numberoutput4, *utf::description("Output second part of a number that does not fit"))
{
    double d = 3.38778;
    jbc::json::output<QChar> o;
    std::array<QChar, 5> buf;
    jbc::json::basic_locator loc;
    loc.position = 4;
    int offset = 1;
    bool res = o.number(d, 6, loc, buf, offset);
    BOOST_TEST(offset == 4);
    BOOST_TEST(loc.position == 0); // written, so reset
    BOOST_TEST(res);
    QString str{buf.data() + 1, 3};
    bool testpassed = str == "778";
    BOOST_TEST(testpassed);
}

BOOST_AUTO_TEST_CASE(booleanoutput, *utf::description("Output a single boolean (true)"))
{
    bool v = true;
    jbc::json::output<QChar> o;
    std::array<QChar, 6> buf;
    jbc::json::basic_locator loc;
    int offset = 0;
    bool res = o.boolean(v, loc, buf, offset);
    BOOST_TEST(offset == 4);
    BOOST_TEST(loc.position == 0);
    BOOST_TEST(res);
    QString str{buf.data(), 4};
    bool testpassed = str == "true";
    BOOST_TEST(testpassed);
}

BOOST_AUTO_TEST_CASE(booleanoutput2, *utf::description("Output a single boolean (false)"))
{
    bool v = false;
    jbc::json::output<QChar> o;
    std::array<QChar, 6> buf;
    jbc::json::basic_locator loc;
    int offset = 0;
    bool res = o.boolean(v, loc, buf, offset);
    BOOST_TEST(offset == 5);
    BOOST_TEST(loc.position == 0);
    BOOST_TEST(res);
    QString str{buf.data(), 5};
    bool testpassed = str == "false";
    BOOST_TEST(testpassed);
}

BOOST_AUTO_TEST_CASE(booleanoutput3, *utf::description("Output a partial boolean"))
{
    bool v = true;
    jbc::json::output<QChar> o;
    std::array<QChar, 2> buf;
    jbc::json::basic_locator loc;
    int offset = 0;
    bool res = o.boolean(v, loc, buf, offset);
    BOOST_TEST(offset == 2);
    BOOST_TEST(loc.position == 2);
    BOOST_TEST(!res);
    QString str{buf.data(), 2};
    bool testpassed = str == "tr";
    BOOST_TEST(testpassed);
}

BOOST_AUTO_TEST_CASE(booleanoutput4, *utf::description("Output a partial boolean"))
{
    bool v = false;
    jbc::json::output<QChar> o;
    std::array<QChar, 2> buf;
    jbc::json::basic_locator loc;
    loc.position = 1;
    int offset = 0;
    bool res = o.boolean(v, loc, buf, offset);
    BOOST_TEST(offset == 2);
    BOOST_TEST(loc.position == 3);
    BOOST_TEST(!res);
    QString str{buf.data(), 2};
    bool testpassed = str == "al";
    BOOST_TEST(testpassed);
}

BOOST_AUTO_TEST_CASE(booleanoutput5, *utf::description("Output the end of a boolean, with offset"))
{
    bool v = false;
    jbc::json::output<QChar> o;
    std::array<QChar, 10> buf;
    jbc::json::basic_locator loc;
    loc.position = 1;
    int offset = 3;
    bool res = o.boolean(v, loc, buf, offset);
    BOOST_TEST(offset == 7);
    BOOST_TEST(loc.position == 0);
    BOOST_TEST(res);
    QString str{buf.data() + 3, 4};
    bool testpassed = str == "alse";
    BOOST_TEST(testpassed);
}

BOOST_AUTO_TEST_CASE(nulloutput, *utf::description("Output a null value"))
{
    jbc::json::output<QChar> o;
    std::array<QChar, 10> buf;
    jbc::json::basic_locator loc;
    int offset = 0;
    bool res = o.null(loc, buf, offset);
    BOOST_TEST(offset == 4);
    BOOST_TEST(loc.position == 0);
    BOOST_TEST(res);
    QString str{buf.data(), 4};
    bool testpassed = str == "null";
    BOOST_TEST(testpassed);
}

BOOST_AUTO_TEST_CASE(nulloutput2, *utf::description("Output a partial null value"))
{
    jbc::json::output<QChar> o;
    std::array<QChar, 3> buf;
    jbc::json::basic_locator loc;
    int offset = 0;
    bool res = o.null(loc, buf, offset);
    BOOST_TEST(offset == 3);
    BOOST_TEST(loc.position == 3);
    BOOST_TEST(!res);
    QString str{buf.data(), 3};
    bool testpassed = str == "nul";
    BOOST_TEST(testpassed);
}

BOOST_AUTO_TEST_CASE(nulloutput3, *utf::description("Output end of a partial null value, with offset"))
{
    jbc::json::output<QChar> o;
    std::array<QChar, 10> buf;
    jbc::json::basic_locator loc;
    loc.position = 2;
    int offset = 3;
    bool res = o.null(loc, buf, offset);
    BOOST_TEST(offset == 5);
    BOOST_TEST(loc.position == 0);
    BOOST_TEST(res);
    QString str{buf.data() + 3, 2};
    bool testpassed = str == "ll";
    BOOST_TEST(testpassed);
}

BOOST_AUTO_TEST_CASE(stringoutput1, *utf::description("Complete output of a string with no escaped characters"))
{
    jbc::json::output<char> o;
    std::array<char, 50> buf;
    QString v{"A sample string"};
    jbc::json::basic_locator loc;
    loc.position = 0;
    int offset = 0;
    bool res = o.string<jbc::json::qt_types>(v, loc, buf, offset);
    BOOST_TEST(offset == v.size() + 2);
    BOOST_TEST(loc.position == 0);
    BOOST_TEST(res);
    std::string str{buf.data(), static_cast<unsigned int>(v.size()) + 2};
    bool testpassed = str == "\"A sample string\"";
    BOOST_TEST(testpassed);
}

BOOST_AUTO_TEST_CASE(stringoutput2, *utf::description("Incomplete output of a string with no escaped characters"))
{
    jbc::json::output<QChar> o;
    std::array<QChar, 10> buf;
    QString v{"A sample string"};
    jbc::json::basic_locator loc;
    loc.position = 0;
    int offset = 0;
    bool res = o.string<jbc::json::qt_types>(v, loc, buf, offset);
    BOOST_TEST(offset == 10);
    BOOST_TEST(loc.position == 10);
    BOOST_TEST(!res);
    QString str{buf.data(), 10};
    bool testpassed = str == "\"A sample ";
    BOOST_TEST(testpassed);
}

BOOST_AUTO_TEST_CASE(stringoutput3, *utf::description("End of output of a string with no escaped characters, with offset"))
{
    jbc::json::output<char> o;
    std::array<char, 10> buf;
    QString v{"A sample string"};
    jbc::json::basic_locator loc;
    loc.position = 10;
    int offset = 2;
    bool res = o.string<jbc::json::qt_types>(v, loc, buf, offset);
    BOOST_TEST(offset == 9);
    BOOST_TEST(loc.position == 0);
    BOOST_TEST(res);
    std::string str{buf.data() + 2, 7};
    bool testpassed = str == "string\"";
    BOOST_TEST(testpassed);
}

BOOST_AUTO_TEST_CASE(stringoutput4, *utf::description("Output of a complete string with special character"))
{
    jbc::json::output<char> o;
    std::array<char, 50> buf;
    QString v{"A sample \"string\""};
    jbc::json::basic_locator loc;
    loc.position = 0;
    int offset = 0;
    bool res = o.string<jbc::json::qt_types>(v, loc, buf, offset);
    BOOST_TEST(offset == 21);
    BOOST_TEST(loc.position == 0);
    BOOST_TEST(res);
    std::string str{buf.data(), static_cast<unsigned int>(offset)};
    bool testpassed = str == "\"A sample \\\"string\\\"\"";
    BOOST_TEST(testpassed);
}

BOOST_AUTO_TEST_CASE(stringoutput5, *utf::description("Output of a incomplete string with special character"))
{
    jbc::json::output<char> o;
    std::array<QChar, 15> buf;
    QString v{"A sample \"string\""};
    jbc::json::basic_locator loc;
    loc.position = 0;
    int offset = 0;
    bool res = o.string<jbc::json::qt_types>(v, loc, buf, offset);
    BOOST_TEST(offset == 15);
    BOOST_TEST(loc.position == 14);
    BOOST_TEST(!res);
    QString str{buf.data(), offset};
    bool testpassed = str == "\"A sample \\\"str";
    BOOST_TEST(testpassed);
}

BOOST_AUTO_TEST_CASE(stringoutput6, *utf::description("Output of the end of an incomplete string with special character"))
{
    jbc::json::output<QChar> o;
    std::array<QChar, 15> buf;
    QString v{"A sample \"string\""};
    jbc::json::basic_locator loc;
    loc.position = 14;
    int offset = 0;
    bool res = o.string<jbc::json::qt_types>(v, loc, buf, offset);
    BOOST_TEST(offset == 6);
    BOOST_TEST(loc.position == 0);
    BOOST_TEST(res);
    QString str{buf.data(), offset};
    bool testpassed = str == "ing\\\"\"";
    BOOST_TEST(testpassed);
}

BOOST_AUTO_TEST_CASE(stringoutput7, *utf::description("Output of a string, cut in the middle of a special character"))
{
    jbc::json::output<QChar> o;
    std::array<QChar, 12> buf;
    QString v{"A sample \x12 string\""};
    jbc::json::basic_locator loc;
    loc.position = 0;
    int offset = 0;
    bool res = o.string<jbc::json::qt_types>(v, loc, buf, offset);
    BOOST_TEST(offset == 12);
    BOOST_TEST(loc.position == 10);
    BOOST_TEST(loc.sub_position == 2);
    BOOST_TEST(!res);
    QString str{buf.data(), offset};
    bool testpassed = str == "\"A sample \\u";
    BOOST_TEST(testpassed);
}

BOOST_AUTO_TEST_CASE(stringoutput8, *utf::description("Output of the end of a string, cut in the middle of a special character"))
{
    jbc::json::output<QChar> o;
    std::array<QChar, 20> buf;
    QString v{"A sample \x12 string\""};
    jbc::json::basic_locator loc;
    loc.position = 10;
    loc.sub_position = 2;
    int offset = 3;
    bool res = o.string<jbc::json::qt_types>(v, loc, buf, offset);
    BOOST_TEST(offset == 17);
    BOOST_TEST(loc.position == 0);
    BOOST_TEST(loc.sub_position == 0);
    BOOST_TEST(res);
    QString str{buf.data() + 3, offset - 3};
    bool testpassed = str == "0012 string\\\"\"";
    BOOST_TEST(testpassed);
}

BOOST_AUTO_TEST_CASE(stringoutput9, *utf::description("Output of a small string portion with special character"))
{
    jbc::json::output<QChar> o;
    std::array<QChar, 4> buf;
    QString v = QString::fromUtf8("\x12");
    jbc::json::basic_locator loc;
    loc.position = 1;
    loc.sub_position = 2;
    int offset = 2;
    bool res = o.string<jbc::json::qt_types>(v, loc, buf, offset);
    BOOST_TEST(offset == 4);
    BOOST_TEST(loc.position == 1);
    BOOST_TEST(loc.sub_position == 4);
    BOOST_TEST(!res);
    QString str{buf.data() + 2, 2};
    bool testpassed = str == "00";
    BOOST_TEST(testpassed);
}

BOOST_AUTO_TEST_CASE(stringoutput10, *utf::description("Output of a string, character by character"))
{
    jbc::json::output<QChar> o;
    std::array<QChar, 1> buf;
    QString v{"st\x12\\"};
    jbc::json::basic_locator loc;
    int offset = 0;
    bool res = false;
    QString output;
    while(!res)
    {
        res = o.string<jbc::json::qt_types>(v, loc, buf, offset);
        offset = 0;
        output.push_back(buf[0]);
    }
    BOOST_TEST(loc.position == 0);
    BOOST_TEST(output.toStdString() == "\"st\\u0012\\\\\"");
    BOOST_TEST(res);
}

BOOST_AUTO_TEST_CASE(stringoutput11, *utf::description("Output of a string with large hex characters"))
{
    jbc::json::output<QChar> o;
    std::array<QChar, 30> buf;
    QString v = QString::fromUtf8("st\uCAFEau");
    jbc::json::basic_locator loc;
    int offset = 0;
    bool res = false;
    QString output;
    res = o.string<jbc::json::qt_types>(v, loc, buf, offset);
    buf[offset] = 0;
    BOOST_TEST(res);
    output = QString::fromRawData(buf.data(), offset);
    BOOST_TEST(output.toStdString() == R"json("st\uCAFEau")json");
}

BOOST_AUTO_TEST_CASE(stringoutput13, *utf::description("Output of a 4bytes utf8 character"))
{
    jbc::json::output<QChar> o;
    std::array<QChar, 50> buf;
    QString v = QString::fromUtf8(u8"st💫au");
    jbc::json::basic_locator loc;
    int offset = 0;
    bool res = false;
    res = o.string<jbc::json::qt_types>(v, loc, buf, offset);
    BOOST_TEST(res);
    buf[offset] = 0;
    QString output(buf.data(), offset);
    BOOST_TEST(output.toStdString() == R"json("st\uD83D\uDCABau")json");
}

BOOST_AUTO_TEST_CASE(objectoutput1, *utf::description("Output of a object, in a large enough buffer"))
{
    jbc::json::output<char> o;
    std::array<char, 500> buf;
    jbc::json::QJsonItem obj(jbc::json::ItemType::Object);
    obj.create_property("property1", jbc::json::ItemType::String)->set_string_value("toto");
    obj.create_property("property2", jbc::json::ItemType::String)->set_string_value("tutu");
    obj.create_property("property3", jbc::json::ItemType::String)->set_string_value("tata");
    int offset = 0;
    jbc::json::basic_locator loc;
    bool res = o.object<decltype(buf), jbc::json::QJsonItem>(obj.begin_object(), obj.end_object(), loc, buf, offset);
    buf[offset] = 0;
    BOOST_TEST(res == true);
    BOOST_TEST(memcmp(buf.data(), R"json({"property1":"toto","property2":"tutu","property3":"tata"})json", offset) == 0);
}

BOOST_AUTO_TEST_CASE(objectoutput2, *utf::description("Output of an object, in a very small buffer"))
{
    jbc::json::output<QChar> o;
    std::array<QChar, 5> buf;
    QString str;
    jbc::json::QJsonItem obj(jbc::json::ItemType::Object);
    obj.create_property("property1", jbc::json::ItemType::String)->set_string_value("toto");
    obj.create_property("property2", jbc::json::ItemType::String)->set_string_value("tutu");
    obj.create_property("property3", jbc::json::ItemType::String)->set_string_value("tata");
    int offset = 0;
    jbc::json::basic_locator loc;
    bool res = false;
    while(!res)
    {
        offset = 0;
        res = o.object<decltype(buf), jbc::json::QJsonItem>(obj.begin_object(), obj.end_object(), loc, buf, offset);
        for(int i = 0; i < offset; ++i)
            str.push_back(buf[i]);
    }
    BOOST_TEST(res == true);
    BOOST_TEST(str.toStdString() == R"json({"property1":"toto","property2":"tutu","property3":"tata"})json");
}

BOOST_AUTO_TEST_CASE(objectoutput3, *utf::description("Output of an object, character by character"))
{
    jbc::json::output<QChar> o;
    std::array<QChar, 1> buf;
    QString str;
    jbc::json::QJsonItem obj(jbc::json::ItemType::Object);
    obj.create_property("property1", jbc::json::ItemType::String)->set_string_value("toto");
    obj.create_property("property2", jbc::json::ItemType::String)->set_string_value("tutu");
    obj.create_property("property3", jbc::json::ItemType::String)->set_string_value("tata");
    int offset = 0;
    jbc::json::basic_locator loc;
    bool res = false;
    while(!res)
    {
        offset = 0;
        res = o.object<decltype(buf), jbc::json::QJsonItem>(obj.begin_object(), obj.end_object(), loc, buf, offset);
        for(int i = 0; i < offset; ++i)
            str.push_back(buf[i]);
    }
    BOOST_TEST(res == true);
    BOOST_TEST(str.toStdString() == R"json({"property1":"toto","property2":"tutu","property3":"tata"})json");
}


BOOST_AUTO_TEST_CASE(arrayoutput, *utf::description("Output of an array, in a large enough buffer"))
{
    jbc::json::output<QChar> o;
    std::array<QChar, 500> buf;
    jbc::json::QJsonItem arr(jbc::json::ItemType::Array);
    arr.create_item(jbc::json::ItemType::String)->set_string_value("sample value");
    arr.create_item(jbc::json::ItemType::Boolean)->set_bool_value(false);
    arr.create_item(jbc::json::ItemType::Null);
    arr.create_item(jbc::json::ItemType::Array);
    int offset = 0;
    jbc::json::basic_locator loc;
    bool res = o.array<decltype(buf), jbc::json::QJsonItem>(arr.begin_array(), arr.end_array(), loc, buf, offset);
    buf[offset] = 0;
    BOOST_TEST(res == true);
    QString str = QString::fromRawData(buf.data(), offset);
    bool equal = str == R"json(["sample value",false,null,[]])json";
    BOOST_TEST(equal);
}

BOOST_AUTO_TEST_CASE(arrayoutput2, *utf::description("Output of an array, in a very small buffer"))
{
    jbc::json::output<QChar> o;
    std::array<QChar, 5> buf;
    jbc::json::QJsonItem arr(jbc::json::ItemType::Array);
    QString str;
    arr.create_item(jbc::json::ItemType::String)->set_string_value("sample value");
    arr.create_item(jbc::json::ItemType::Boolean)->set_bool_value(false);
    arr.create_item(jbc::json::ItemType::Null);
    auto sub = arr.create_item(jbc::json::ItemType::Array);
    sub->create_item(jbc::json::ItemType::Array);
    sub->create_item(jbc::json::ItemType::Null);
    sub->create_item(jbc::json::ItemType::String);
    int offset = 0;
    jbc::json::basic_locator loc;
    bool res = false;
    while(!res)
    {
        offset = 0;
        res = o.array<decltype(buf), jbc::json::QJsonItem>(arr.begin_array(), arr.end_array(), loc, buf, offset);
        for(int i = 0; i < offset; ++i)
            str.push_back(buf[i]);
    }
    BOOST_TEST(res == true);
    BOOST_TEST(str.toStdString() == R"json(["sample value",false,null,[[],null,""]])json");
}

BOOST_AUTO_TEST_CASE(arrayoutput3, *utf::description("Output of an array, char by char"))
{
    jbc::json::output<QChar> o;
    std::array<QChar, 1> buf;
    jbc::json::QJsonItem arr(jbc::json::ItemType::Array);
    QString str;
    arr.create_item(jbc::json::ItemType::String)->set_string_value("sample value");
    arr.create_item(jbc::json::ItemType::Boolean)->set_bool_value(false);
    arr.create_item(jbc::json::ItemType::Null);
    auto sub = arr.create_item(jbc::json::ItemType::Array);
    sub->create_item(jbc::json::ItemType::Array);
    sub->create_item(jbc::json::ItemType::Null);
    sub->create_item(jbc::json::ItemType::String);
    int offset = 0;
    jbc::json::basic_locator loc;
    bool res = false;
    while(!res)
    {
        offset = 0;
        res = o.array<decltype(buf), jbc::json::QJsonItem>(arr.begin_array(), arr.end_array(), loc, buf, offset);
        for(int i = 0; i < offset; ++i)
            str.push_back(buf[i]);
    }
    BOOST_TEST(res == true);
    BOOST_TEST(str.toStdString() == R"json(["sample value",false,null,[[],null,""]])json");
}

BOOST_AUTO_TEST_CASE(complexdocument, *utf::description("Output of a complex document, in multiple steps"))
{
    QString str = R"json([
                      "JSON Test Pattern pass1",
                      {"object with 1 member":["array with 1 element"]},
                      {},
                      [],
                      -42,
                      true,
                      false,
                      null,
                      {
                          "integer": 1234567890,
                          "real": -9876.543210,
                          "e": 0.123456789e-12,
                          "E": 1.234567890E+34,
                          "":  23456789012E66,
                          "zero": 0,
                          "one": 1,
                          "space": " ",
                          "quote": "\"",
                          "backslash": "\\",
                          "controls": "\b\f\n\r\t",
                          "slash": "/ & \/",
                          "alpha": "abcdefghijklmnopqrstuvwyz",
                          "ALPHA": "ABCDEFGHIJKLMNOPQRSTUVWYZ",
                          "digit": "0123456789",
                          "0123456789": "digit",
                          "special": "`1~!@#$%^&*()_+-={':[,]}|;.</>?",
                          "hex": "\u0123\u4567\u89AB\uCDEF\uabcd\uef4A",
                          "true": true,
                          "false": false,
                          "null": null,
                          "array":[  ],
                          "object":{  },
                          "address": "50 St. James Street",
                          "url": "http://www.JSON.org/",
                          "comment": "// /* <!-- --",
                          "# -- --> */": " ",
                          " s p a c e d " :[1,2 , 3

                  ,

                  4 , 5        ,          6           ,7        ],"compact":[1,2,3,4,5,6,7],
                          "jsontext": "{\"object with 1 member\":[\"array with 1 element\"]}",
                          "quotes": "&#34; \u0022 %22 0x22 034 &#x22;",
                          "\/\\\"\uCAFE\uBABE\uAB98\uFCDE\ubcda\uef4A\b\f\n\r\t`1~!@#$%^&*()_+-=[]{}|;:',./<>?"
                  : "A key can be any string"
                      },
                      0.5 ,98.6
                  ,
                  99.44
                  ,

                  1066,
                  1e1,
                  0.1e1,
                  1e-1,
                  1e00,2e+00,2e-00
                  ,"rosebud"])json";
    jbc::json::QJsonItem i;
    QTextStream s(&str);
    bool res = parse_from_stream(s, i);
    if(res)
    {
        QString output;
        std::array<char, 50> buf;
        jbc::json::basic_locator loc;
        res = false;
        while(!res)
        {
            int offset = 0;
            jbc::json::output_visitor<decltype (buf), jbc::json::QJsonItem, char>
                    v{buf, offset, loc};
            res = i.apply_visitor(v);

//            res = o.sensible(i, loc, buf, offset);
//            output.append(QString::fromRawData(buf.begin(), offset));
            output.append(QString::fromUtf8(buf.begin(), offset));
        }
        BOOST_TEST(res == true);
        BOOST_TEST(output.toStdString() == R"json(["JSON Test Pattern pass1",{"object with 1 member":["array with 1 element"]},{},[],-42,true,false,null,{"integer":1234567890,"real":-9876.54321,"e":1.23456789e-13,"E":1.23456789e+34,"":2.345678901e+76,"zero":0,"one":1,"space":" ","quote":"\"","backslash":"\\","controls":"\b\f\n\r\t","slash":"/ & /","alpha":"abcdefghijklmnopqrstuvwyz","ALPHA":"ABCDEFGHIJKLMNOPQRSTUVWYZ","digit":"0123456789","0123456789":"digit","special":"`1~!@#$%^&*()_+-={':[,]}|;.</>?","hex":"\u0123\u4567\u89AB\uCDEF\uABCD\uEF4A","true":true,"false":false,"null":null,"array":[],"object":{},"address":"50 St. James Street","url":"http://www.JSON.org/","comment":"// /* <!-- --","# -- --> */":" "," s p a c e d ":[1,2,3,4,5,6,7],"compact":[1,2,3,4,5,6,7],"jsontext":"{\"object with 1 member\":[\"array with 1 element\"]}","quotes":"&#34; \" %22 0x22 034 &#x22;","/\\\"\uCAFE\uBABE\uAB98\uFCDE\uBCDA\uEF4A\b\f\n\r\t`1~!@#$%^&*()_+-=[]{}|;:',./<>?":"A key can be any string"},0.5,98.6,99.44,1066,10,1,0.1,1,2,2,"rosebud"])json");
    }
}
