#pragma once

#if __has_include("simply-json_export.h")
#include "simply-json_export.h"
#else
#define SIMPLY_JSON_EXPORT
#endif

#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>

class SIMPLY_JSON_EXPORT JsonParser
{

  public:
    struct Value
    {

        using Array = std::vector<Value>;
        using Object = std::map<std::string, Value>;
        using String = std::string;
        using Number = double;
        using Null = std::nullptr_t;
        using Boolean = bool;

        using type = std::variant<Null, Boolean, Number, String, Array, Object>;
        type _value;

        Value &operator[](int index);
        Value &operator[](std::string key);
        const Value &operator[](int index) const;
        const Value &operator[](std::string key) const;
        std::string to_string() const;

        template <typename T> Value &operator=(T value)
        {
            _value = std::move(value);
            return *this;
        }

        template <class T> T &emplace()
        {
            return _value.emplace<T>();
        }
    };

    JsonParser(std::string json);

    std::optional<Value> json();

  protected:
    bool is_json(Value &result);
    bool is_value(Value &result);
    bool is_true(Value &result);
    bool is_false(Value &result);
    bool is_null(Value &result);
    bool is_object(Value &result);
    bool is_members(Value::Object &result);
    bool is_member(Value::Object &result);
    bool is_array(Value &result);
    bool is_elements(Value::Array &result);
    bool is_element(Value &result);
    bool is_string(Value &result);
    bool is_string(std::string &result);
    bool is_characters(std::string &result);
    bool is_character(std::string &result);
    bool is_escape(char &result);
    bool is_hex(int &result);
    bool is_number(Value &result);
    bool is_integer(std::string &result);
    bool is_digits(std::string &result);
    bool is_digit(std::string &result);
    bool is_onenine(std::string &result);
    bool is_fraction(std::string &result);
    bool is_exponent(std::string &result);
    bool is_sign(std::string &result);
    bool is_ws(std::string &result);
    bool is_match(char what, char &result);
    bool is_match(char what, std::string &result);
    bool is_match(const std::string &what, std::string &result);
    bool is_match(bool (*what)(char), char &result);
    bool is_match(bool (*what)(char), std::string &result);

    void skip_ws();
    auto syntax_error(std::string what);

  private:
    std::string _source;
    std::string::iterator _it, _end;
    int _depth;
};

template <typename T> std::optional<JsonParser::Value> parse_json(T &&source)
{
    return JsonParser(std::forward<T>(source)).json();
}