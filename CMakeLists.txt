cmake_minimum_required (VERSION 2.6)
project (libjson)

# The version number.
set (libjson_VERSION_MAJOR 1)
set (libjson_VERSION_MINOR 0)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/src/libjson.h.in"
  "${PROJECT_BINARY_DIR}/src/libjson_version.h"
  )

# add the binary tree to the search path for include files
# so that we will find headers.h
include_directories("${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}")

SET(LIB_HEADERS
    src/basic_item.h
    src/customizable_parser_policy.h
    src/helper_functions.h
    src/item_builder.h
    src/libjson.h
    src/libjson_version.h
    src/object_iterator.h
    src/parser_bits.h
    src/parser.h
    src/printer.h
    src/stl_json.h
    src/utf8_printer.h
    src/output.h
    src/output_utilities.h
    src/static_json.h
    src/qt_json.h
    DBC/contracts.h
)
include_directories(src)

add_executable(jsonprint tools/jsonprint/jsonprint.cpp ${LIB_HEADERS})
add_executable(jsonlint tools/jsonlint/jsonlint.cpp ${LIB_HEADERS})

add_executable(json_conformance_test tests/json_conformance/json_conformance.cpp ${LIB_HEADERS})
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
find_package(Qt5Core)
add_executable(json_conformance_qt_test tests/json_conformance/json_conformance_qt.cpp ${LIB_HEADERS})
add_executable(json_output_test tests/json_output/json_output.cpp ${LIB_HEADERS})
#add_executable(json_output_qt_test tests/json_output/json_output_qt.cpp ${LIB_HEADERS})

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

target_link_libraries(json_conformance_test ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_link_libraries(json_conformance_qt_test ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} Qt5::Core)
target_link_libraries(json_output_test ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
#target_link_libraries(json_output_qt_test ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} Qt5::Core)

add_test(NAME json_conformance COMMAND test_json_conformance)

add_definitions(-DCONTRACT_POLICY=CONTRACTS_NO_CHECK)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Wno-long-long -pedantic")
