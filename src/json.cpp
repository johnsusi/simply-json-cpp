#include "json.h"

#include <sstream>

JsonParser::JsonParser(std::string json)
    : _source(json), _it(_source.begin()), _end(_source.end()), _depth(0)
{
}

auto JsonParser::syntax_error(std::string what)
{
    return std::runtime_error(what + " ... '" +
                              std::string{_it, _end}.substr(10) + "'");
}

std::optional<JsonParser::Value> JsonParser::json()
{
    if (Value result; is_value(result))
    {
        skip_ws();
        if (_it != _end)
            throw syntax_error("Trailing characters");
        return {std::move(result)};
    }
    return {};
}

bool JsonParser::is_json(Value &result)
{
    return is_value(result);
}

bool JsonParser::is_value(Value &result)
{
    skip_ws();
    if (_depth > 100)
        return false;
    ++_depth;
    auto ok = is_object(result) || is_array(result) || is_string(result) ||
              is_true(result) || is_false(result) || is_null(result) ||
              is_number(result);
    --_depth;
    return ok;
}

bool JsonParser::is_true(Value &result)
{
    using namespace std::string_literals;
    if (std::string s; is_match("true"s, s))
    {
        result = true;
        return true;
    }
    return false;
}

bool JsonParser::is_false(Value &result)
{
    using namespace std::string_literals;
    if (std::string s; is_match("false"s, s))
    {
        result = false;
        return true;
    }
    return false;
}

bool JsonParser::is_null(Value &result)
{
    using namespace std::string_literals;
    if (std::string s; is_match("null"s, s))
    {
        result = nullptr;
        return true;
    }
    return false;
}

bool JsonParser::is_object(Value &result)
{
    if (char c; is_match('{', c))
    {
        is_members(result.emplace<Value::Object>());
        if (!is_match('}', c))
            throw syntax_error("Expected '}'");
        return true;
    }
    return false;
}

bool JsonParser::is_members(Value::Object &result)
{
    if (is_member(result))
    {
        for (char c; is_match(',', c);)
        {
            if (!is_member(result))
                throw syntax_error("Expected <member>");
        }
        return true;
    }
    return false;
}

bool JsonParser::is_member(Value::Object &result)
{
    std::string ignored;
    if (std::string key; is_ws(ignored) && is_string(key) && is_ws(ignored))
    {
        if (char c; !is_match(':', c))
            throw syntax_error("Expected ':'");
        Value v;
        if (!is_element(v))
            throw syntax_error("Expected <element>");
        result[key] = std::move(v);
        return true;
    }
    return false;
}

bool JsonParser::is_array(Value &result)
{
    if (char c; is_match('[', c))
    {
        auto &a = result.emplace<Value::Array>();
        is_elements(a);
        if (!is_match(']', c))
            throw syntax_error("Expected ']'");
        return true;
    }
    return false;
}

bool JsonParser::is_elements(Value::Array &result)
{
    if (Value v; is_element(v))
    {
        result.emplace_back(std::move(v));
        for (char c; is_match(',', c);)
        {
            if (Value v; is_element(v))
                result.emplace_back(std::move(v));
            else
                throw syntax_error("Expected <element>");
        }
        return true;
    }
    return false;
}

bool JsonParser::is_element(Value &result)
{
    std::string ignored;
    return is_ws(ignored) && is_value(result) && is_ws(ignored);
}

bool JsonParser::is_string(Value &result)
{
    if (std::string s; is_string(s))
    {
        result = std::move(s);
        return true;
    }
    return false;
}

bool JsonParser::is_string(std::string &result)
{
    if (char c; is_match('"', c))
    {
        is_characters(result);
        if (!is_match('"', c))
            throw syntax_error("Excepted '\"'");
        return true;
    }
    return false;
}

bool JsonParser::is_characters(std::string &result)
{

    while (is_character(result))
        ;
    return true;
}

bool JsonParser::is_character(std::string &result)
{
    if (is_match([](auto x) { return x != '\\' && x != '"' && x >= 20; },
                 result))
    {
        return true;
    }
    else if (char c; is_match('\\', c))
    {
        if (!is_escape(c))
            throw syntax_error("Expected <escape>");
        result += c;
        return true;
    }
    return false;
}

bool JsonParser::is_escape(char &result)
{
    if (char c; is_match('u', c))
    {
        int c1, c2, c3, c4;
        if (!is_hex(c1) || !is_hex(c2) || !is_hex(c3) || !is_hex(c4))
            throw syntax_error("Expected <hex>");

        result = (c1 << 12) | (c2 << 8) | (c3 << 4) | c4;
        return true;
    }

    return is_match(
        [](char x) {
            switch (x)
            {
            case '"':
                return true;
            case '\\':
                return true;
            case '/':
                return true;
            case 'b':
                return true;
            case 'f':
                return true;
            case 'n':
                return true;
            case 'r':
                return true;
            case 't':
                return true;
            default:
                return false;
            }
        },
        result);
}

bool JsonParser::is_hex(int &result)
{
    if (char c; is_match([](auto x) { return x >= '0' && x <= '9'; }, c))
    {
        result = c - '0';
        return true;
    }
    else if (char c; is_match([](auto x) { return x >= 'a' && x <= 'f'; }, c))
    {
        result = c - 'a';
        return true;
    }
    if (char c; is_match([](auto x) { return x >= 'A' && x <= 'F'; }, c))
    {
        result = c - 'A';
        return true;
    }
    return false;
}

bool JsonParser::is_number(Value &result)
{
    if (std::string s; is_integer(s) && is_fraction(s) && is_exponent(s))
    {
        result = std::stod(s);
        return true;
    }
    return false;
}

bool JsonParser::is_integer(std::string &result)
{
    if (is_match('-', result))
    {
        if (is_onenine(result))
        {
            is_digits(result);
            return true;
        }
        else if (is_digit(result))
            return true;
        else
            throw syntax_error("Expected <digit>");
    }
    else if (is_onenine(result))
    {
        is_digits(result);
        return true;
    }
    else if (is_digit(result))
        return true;
    return false;
}

bool JsonParser::is_digits(std::string &result)
{
    if (is_digit(result))
    {
        while (is_digit(result))
            ;
        return true;
    }
    return false;
}

bool JsonParser::is_digit(std::string &result)
{
    return is_match([](auto x) { return x >= '0' && x <= '9'; }, result);
}

bool JsonParser::is_onenine(std::string &result)
{
    return is_match([](auto x) { return x >= '1' && x <= '9'; }, result);
}

bool JsonParser::is_fraction(std::string &result)
{
    if (is_match('.', result))
    {
        if (!is_digits(result))
            throw syntax_error("Expected <digits>");
        return true;
    }
    return true;
}

bool JsonParser::is_exponent(std::string &result)
{
    if (is_match([](auto x) { return x == 'e' || x == 'E'; }, result) &&
        is_sign(result))
    {
        if (!is_digits(result))
            throw syntax_error("Expected <digits>");
        return true;
    }
    return true;
}

bool JsonParser::is_sign(std::string &result)
{
    return is_match([](auto x) { return x == '+' || x == '-'; }, result), true;
}

bool JsonParser::is_ws(std::string &result)
{
    while (is_match(
        [](auto x) {
            switch (x)
            {
            case ' ':
                return true;
            case '\t':
                return true;
            case '\r':
                return true;
            case '\n':
                return true;
            default:
                return false;
            }
        },
        result))
        ;
    return true;
}

void JsonParser::skip_ws()
{
    std::string ignored;
    is_ws(ignored);
}

bool JsonParser::is_match(char what, char &result)
{
    if (_it == _end || what != *_it)
        return false;
    result = *_it++;
    return true;
}

bool JsonParser::is_match(char what, std::string &result)
{
    if (_it == _end || what != *_it)
        return false;
    result += *_it++;
    return true;
}

bool JsonParser::is_match(const std::string &what, std::string &result)
{
    auto jt = _it;
    std::string temp;
    for (auto c : what)
    {
        if (jt == _end || c != *jt)
            return false;
        temp += *jt++;
    }
    _it = jt;
    result = temp;
    return true;
}

bool JsonParser::is_match(bool (*what)(char), char &result)
{
    if (_it == _end || !what(*_it))
        return false;
    result = *_it++;
    return true;
}

bool JsonParser::is_match(bool (*what)(char), std::string &result)
{
    if (_it == _end || !what(*_it))
        return false;
    result += *_it++;
    return true;
}

JsonParser::Value &JsonParser::Value::operator[](int index)
{
    return std::get<Array>(_value)[index];
}

JsonParser::Value &JsonParser::Value::operator[](std::string key)
{
    return std::get<Object>(_value)[key];
}

const JsonParser::Value &JsonParser::Value::operator[](int index) const
{
    return std::get<Array>(_value).at(index);
}

const JsonParser::Value &JsonParser::Value::operator[](std::string key) const
{
    return std::get<Object>(_value).at(key);
}

std::string JsonParser::Value::to_string() const
{
    struct visitor
    {

        std::string operator()(const Null &) const
        {
            return "null";
        }

        std::string operator()(const Boolean &value) const
        {
            return value ? "true" : "false";
        }

        std::string operator()(const Number &value) const
        {
            std::stringstream ss;
            ss << value;
            return ss.str();
        }

        std::string operator()(const String &value) const
        {
            return "\"" + value + "\"";
        }

        std::string operator()(const Array &value) const
        {
            std::stringstream ss;
            ss << "[";
            bool first = true;
            for (auto &&element : value)
            {
                if (!first)
                    ss << ",";
                else
                    first = false;
                ss << element.to_string();
            }
            ss << "]";
            return ss.str();
        }

        std::string operator()(const Object &value) const
        {
            std::stringstream ss;
            ss << "{";
            bool first = true;
            for (auto &&[key, val] : value)
            {
                if (!first)
                    ss << ",";
                else
                    first = false;
                ss << "\"" << key << "\":" << val.to_string();
            }
            ss << "}";
            return ss.str();
        }
    };
    return std::visit(visitor{}, _value);
}
