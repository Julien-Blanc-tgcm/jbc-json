cmake_minimum_required (VERSION 2.6)
project (libjson)

# The version number.
set (libjson_VERSION_MAJOR 1)
set (libjson_VERSION_MINOR 0)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

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
#    src/printer.h
    src/stl_json.h
#    src/utf8_printer.h
    src/output.h
    src/output_utilities.h
    src/static_json.h
    src/qt_json.h
    DBC/contracts.h
)
include_directories(src)

enable_testing(TRUE)

set(COVERAGE Off CACHE BOOL "Build with coverage enable (gcov)" )

if(COVERAGE)
    SET(CMAKE_COVERAGE_COMMAND FILEPATH /usr/bin/gcov)
    SET(CXX_FLAGS_COVERAGE  -pthread -g -O0 -Wall -W -Wunused-variable
        -Wunused-parameter -Wunused-function -Wunused -Wno-system-headers
        -Wno-deprecated -Wwrite-strings -ftest-coverage
        -Wno-error=uninitialized -fprofile-arcs -Wshadow -Wno-error=type-limits
        -Wno-type-limits -fsanitize=undefined -fno-sanitize-recover -fsanitize=address)
    SET(LD_FLAGS_COVERAGE "-ftest-coverage -pthread -fprofile-arcs -fsanitize=undefined -fsanitize=address")
    message("Compiling with coverage enabled")
    enable_testing()
    include(CTest)
    add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/coverage
        COMMAND mkdir -p ${PROJECT_BINARY_DIR}
        )
    add_custom_target(coverage_html
        DEPENDS ${PROJECT_BINARY_DIR}/coverage
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/coverage
        COMMAND gcovr -r ${CMAKE_SOURCE_DIR} --object-directory ${PROJECT_BINARY_DIR}/CMakeFiles/.
                       --html --html-details -o cov.html
        )
else()
    message("Ignoring coverage : ${COVERAGE}")
    SET(CXX_FLAGS_COVERAGE "")
    SET(LD_FLAGS_COVERAGE "")
endif()

add_executable(jsonprint tools/jsonprint/jsonprint.cpp ${LIB_HEADERS})
add_executable(jsonlint tools/jsonlint/jsonlint.cpp ${LIB_HEADERS})

set_target_properties(jsonprint PROPERTIES LINK_FLAGS -Wl,-Map=jsonprint.map)

add_executable(json_conformance_test tests/json_conformance/json_conformance.cpp ${LIB_HEADERS})
add_executable(json_output_test tests/json_output/json_output.cpp ${LIB_HEADERS})
set_target_properties(json_conformance_test PROPERTIES COMPILE_OPTIONS "${CXX_FLAGS_COVERAGE}" LINK_FLAGS "${LD_FLAGS_COVERAGE}")
set_target_properties(json_output_test PROPERTIES COMPILE_OPTIONS "${CXX_FLAGS_COVERAGE}" LINK_FLAGS "${LD_FLAGS_COVERAGE}")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
find_package(Boost COMPONENTS unit_test_framework REQUIRED)

target_link_libraries(json_conformance_test ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_link_libraries(json_output_test ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test(NAME json_conformance COMMAND json_conformance_test)
add_test(NAME json_output COMMAND json_output_test)

find_package(Qt5Core)
if(Qt5Core_FOUND)
    add_executable(json_conformance_qt_test tests/json_conformance/json_conformance_qt.cpp ${LIB_HEADERS})
    add_executable(json_output_qt_test tests/json_output/json_output_qt.cpp ${LIB_HEADERS})
    set_target_properties(json_conformance_qt_test PROPERTIES COMPILE_OPTIONS "${CXX_FLAGS_COVERAGE}" LINK_FLAGS "${LD_FLAGS_COVERAGE}")
    set_target_properties(json_output_qt_test PROPERTIES COMPILE_OPTIONS "${CXX_FLAGS_COVERAGE}" LINK_FLAGS "${LD_FLAGS_COVERAGE}")

    target_link_libraries(json_conformance_qt_test ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} Qt5::Core)
    target_link_libraries(json_output_qt_test ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} Qt5::Core)

    add_test(NAME json_conformance_qt COMMAND json_conformance_qt_test)
    add_test(NAME json_output_qt COMMAND json_output_qt_test)
endif()

add_definitions(-DCONTRACT_POLICY=CONTRACTS_NO_CHECK)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-long-long -pedantic ")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
