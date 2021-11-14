# Kobato

Kobato is a small JSON parser, consisting of a single header file written in C++.

## Usage

Kobato is made available by simply including the header file in C++ files.

```
#include "kobato.h"
```

## Class

### kobato::Value

`kobato::Value` is an object that is the result of parsing a JSON string.

#### `template<typename T> T& kobato::Value::Get()`

Return a reference to the object that the Value object is wrapping.

#### `template<typename T> kobato::Value& kobato::Value::Set(const T& value)`

Set the specified value to a Value object and return the reference.

## Types

### kobato::String

`kobato::String` is a string type, an alias of `std::string`.

### kobato::Number

`kobato::Number` is a numeric type, an alias of `double`.

### kobato::Boolean

`kobato::Boolean` is a boolean type, an alias of `bool`.

### kobato::Null

`kobato::Null` is a null type.

### kobato::Array

`kobato::Array` is an array type, an alias of `std::vector<kobato::Value>`.

### kobato::Object

`kobato::Object` is a JSON object type (kay-value pairs), an alias of `std::map<kobato::String, kobato::Value>`.

## Functions

### `Error kobato::Parse(const std::string& s, kobato::Value* value_ptr)`

`Parse()` returns a kobato object `value_ptr` obtained as a result of parsing the JSON string `s`.
On success, it returns `kOK`, otherwise it returns `kInvalidToken`.
