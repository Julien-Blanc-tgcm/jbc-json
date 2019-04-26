//          Copyright Julien Blanc 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <qt_json.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE json_conformance
#include <boost/test/unit_test.hpp>

namespace utf = boost::unit_test;

BOOST_AUTO_TEST_CASE(pass_simple1, *utf::description("Very simple document"))
{
    QString str = R"json([                 "JSON Test Pattern pass1","json2"])json";
    jbc::json::QJsonItem i;
    QTextStream s(&str);
    bool res = parse_from_stream(s, i);
    BOOST_TEST(res);
    auto first_string = i.begin_array()->string_value().toStdString();
    BOOST_TEST(first_string == "JSON Test Pattern pass1");
}

BOOST_AUTO_TEST_CASE(pass1, *utf::description("Sample json document"))
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
    BOOST_TEST(res);
    auto first_string = i.begin_array()->string_value().toStdString();
    BOOST_TEST(first_string == "JSON Test Pattern pass1");
}

BOOST_AUTO_TEST_CASE(pass2, *utf::description("Nesting not an issue"))
{
    QString str = R"([[[[[[[[[[[[[[[[[[["Not too deep"]]]]]]]]]]]]]]]]]]])";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(res);
}

BOOST_AUTO_TEST_CASE(pass3, *utf::description("Object outermost value is valid"))
{
    QString str = R"json({
"JSON Test Pattern pass3": {
    "The outermost value": "can be an object.",
   "In this test": "It is an object."
    }
}
)json";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(res);
}

BOOST_AUTO_TEST_CASE(pass4, *utf::description("Single string shall be valid"))
{
    QString str = R"("A single JSON string is a valid document")";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(res);
}

BOOST_AUTO_TEST_CASE(pass5, *utf::description("Array outermost value is valid"))
{
    QString str = R"json([
"JSON Test Pattern pass5",
"The outermost value",
"can be an array.",
"In this test",
"It is an array."
]
)json";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(res);
}

BOOST_AUTO_TEST_CASE(pass6, *utf::description("Extra spaces at end is valid"))
{
    QString str = "{\"obj\":\"value\"}\n\t   \t  \n\r  ";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(res);
}

BOOST_AUTO_TEST_CASE(fail1, *utf::description("empty document not valid"))
{
    QString str;
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail2, *utf::description("Unclosed array"))
{
    QString str=R"(["Unclosed array")";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail3, *utf::description("Unquoted key"))
{
    QString str = R"({unquoted_key: "keys must be quoted"})";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail4, *utf::description("extra comma"))
{
    QString str = R"(["extra comma",])";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}


BOOST_AUTO_TEST_CASE(fail5, *utf::description("double extra comma"))
{
    QString str = R"(["double extra comma",,])";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail6, *utf::description("missing value in array"))
{
    QString str =R"([   , "<-- missing value"])";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail7, *utf::description("comma after close"))
{
    QString str = R"(["Comma after the close"],)";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail8, *utf::description("extra close"))
{
    QString str = R"(["Extra close"]])";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail9, *utf::description("extra comma"))
{
    QString str = R"({"Extra comma": true,})";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail10, *utf::description("extra value after close"))
{
    QString str = R"({"Extra value after close": true} "misplaced quoted value")";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail11, *utf::description("Illegal expression"))
{
    QString str = R"({"Illegal expression": 1 + 2})";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail12, *utf::description("Illegal invocation"))
{
    QString str = R"({"Illegal invocation": alert()})";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail13, *utf::description("Invalid leading 0"))
{
    QString str = R"({"Numbers cannot have leading zeroes": 013})";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail14, *utf::description("Hex number invalid"))
{
    QString str = R"({"Numbers cannot be hex": 0x14})";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail15, *utf::description("Illegal backslash escape"))
{
    QString str = R"(["Illegal backslash escape: \x15"])";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail16, *utf::description("Invalid escape"))
{
    QString str = R"([\naked])";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail17, *utf::description("Illegal backslash escape \\017"))
{
    QString str = R"(["Illegal backslash escape: \017"])";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(notfail18, *utf::description("Depth has no limit"))
{
    QString str = R"([[[[[[[[[[[[[[[[[[[["Too deep"]]]]]]]]]]]]]]]]]]]])";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(res);
}

BOOST_AUTO_TEST_CASE(fail19, *utf::description("Missing colon"))
{
    QString str = R"({"Missing colon" null})";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail20, *utf::description("Double colon"))
{
    QString str = R"({"Double colon":: null})";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail21, *utf::description("Comma instead of colon"))
{
    QString str = R"({"Comma instead of colon", null})";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail22, *utf::description("Colon instead of comma"))
{
    QString str = R"(["Colon instead of comma": false])";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail23, *utf::description("Bad value"))
{
    QString str = R"(["Bad value", truth])";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail24, *utf::description("single quote"))
{
    QString str = "['single quote']";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail25, *utf::description("tab in string"))
{
    QString str = "[\"\ttab\tcharacter\tin\tstring\t\"]";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail26, *utf::description("backslash in string"))
{
    QString str = "[\"backslash\\ character\\\t       in\\  string\\  \"]";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail27, *utf::description("line break in string"))
{
    QString str = "[\"line\nbreak\"]";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail28, *utf::description("line break in string (2)"))
{
    QString str = "[\"line\\\nbreak\"]";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail29, *utf::description("Invalid number format"))
{
    QString str = "[0e]";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail30, *utf::description("Invalid number format (2)"))
{
    QString str = "[0e+]";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail31, *utf::description("Invalid number format (3)"))
{
    QString str = "[0e+-1]";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail32, *utf::description("Comma instead of closing brace"))
{
    QString str = R"("{"Comma instead if closing brace": true,)";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail33, *utf::description("mismatch"))
{
    QString str = R"(["mismatch"})";
    QTextStream s(&str);
    jbc::json::QJsonItem i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}
