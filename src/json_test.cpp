#include "json.h"

#include <string>
#include <tuple>

#include "catch.hpp"

TEST_CASE("null")
{
    std::string expected = "null";
    auto actual = JsonParser(expected).json();
    REQUIRE(actual);
    REQUIRE(expected == actual->to_string());
}

TEST_CASE("true")
{
    std::string expected = "true";
    auto actual = JsonParser(expected).json();
    REQUIRE(actual);
    REQUIRE(expected == actual->to_string());
}

TEST_CASE("false")
{
    std::string expected = "false";
    auto actual = JsonParser(expected).json();
    REQUIRE(actual);
    REQUIRE(expected == actual->to_string());
}

TEST_CASE("number")
{
    auto [source, expected] =
        GENERATE(table<std::string, std::string>({{"3", "3"},
                                                  {"3.0", "3"},
                                                  {"1e9", "1e+09"},
                                                  {"1e+9", "1e+09"},
                                                  {"1e-9", "1e-09"},
                                                  {"-1e9", "-1e+09"},
                                                  {"-1.1e-9", "-1.1e-09"},
                                                  {"0", "0"}}));
    auto actual = JsonParser(source).json();
    REQUIRE(actual);
    REQUIRE(expected == actual->to_string());
}

TEST_CASE("string")
{
    auto [source, expected] =
        GENERATE(table<std::string, std::string>({{"\"\"", "\"\""}}));
    auto actual = JsonParser(source).json();
    REQUIRE(actual);
    REQUIRE(expected == actual->to_string());
}

TEST_CASE("array")
{
    auto [source, expected] = GENERATE(table<std::string, std::string>({
        {"[1, 2, 3]", "[1,2,3]"},
        {"[\n1\t,\n 2, 3\n\r]", "[1,2,3]"},
    }));
    auto actual = JsonParser(source).json();
    REQUIRE(actual);
    REQUIRE(expected == actual->to_string());
}

TEST_CASE("object")
{
    auto [source, expected] = GENERATE(table<std::string, std::string>({
        {"{ \"foo\": [1, 2, 3]}", "{\"foo\":[1,2,3]}"},
    }));
    auto actual = JsonParser(source).json();
    REQUIRE(actual);
    REQUIRE(expected == actual->to_string());
}

// TEST_CASE("complex")
// {
//   std::string source = R"(
//     {
//       "null": null,
//       "true": true,
//       "false": false,
//       "3.14": 3.14,
//       "1e10": 1e10,
//       "1.23e45": 1.23e45,
//       "-1.23e-45": -1.23e-45,
//       "1.23e+45": 1.23e+45,
//       "array": [
//         null,
//         true,
//         false,
//         3.14,
//         1e10,
//         1.23e45,
//         -1.23e-45,
//         1.23e+45,
//         [1, 2, 3]
//       ],
//       "object": {
//         "null": null,
//         "true": true,
//         "false": false,
//         "3.14": 3.14,
//         "1e10": 1e10,
//         "1.23e45": 1.23e45,
//         "-1.23e-45": -1.23e-45,
//         "1.23e+45": 1.23e+45,
//         "array": [
//           null,
//           true,
//           false,
//           3.14,
//           1e10,
//           1.23e45,
//           -1.23e-45,
//           1.23e+45,
//           [1, 2, 3]
//         ]
//       }
//     }
//   )";
//   std::string expected =
//     "{\"null\":null,\"true\":true,\"false\":false,\"3.14\":3.14,\"1e10\":1e10,\"1.23e45\":1.23e45,\"-1.23e-45\":-1.23e-45,\"1.23e+45\":1.23e+45,\"array\":[null,true,false,3.14,1e10,1.23e45,-1.23e-45,1.23e+45,[1,2,3]],\"object\":{\"null\":null,\"true\":true,\"false\":false,\"3.14\":3.14,\"1e10\":1e10,\"1.23e45\":1.23e45,\"-1.23e-45\":-1.23e-45,\"1.23e+45\":1.23e+45,\"array\":[null,true,false,3.14,1e10,1.23e45,-1.23e-45,1.23e+45,[1,2,3]]}}";
//   auto actual = JsonParser(source).json();
//   REQUIRE(actual);
//   REQUIRE(expected == actual->to_string());
// }
