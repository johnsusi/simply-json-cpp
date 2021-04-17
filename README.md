# simply-json-cpp

## Introduction

This is a small and compliant library to read and write JSON data in C++.

## Usage

Simply copy [json.h](src/json.h) and [json.cpp](src/json.cpp) into your project or install one of the provided libraries.

```cpp

#include <simply/json.h>

auto source = R("
{
    "foo": [
        "bar",
        { "baz": 42 }
    ]
}
");

if (auto json = simply::parse_json("[1, 2, 3]"); json)
{
    std::cout << json["foo"][1]["baz"] << std::endl;
}

// Outputs: 42
```


## 3rd-party software used

  - https://github.com/catchorg/Catch2
  - https://github.com/nst/JSONTestSuite
