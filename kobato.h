#include <string>
#include <sstream>
#include <memory>
#include <vector>
#include <unordered_set>
#include <map>
#include <cassert>

namespace kobato {
class Value;

class Null {
} null;

using String = std::string;
using Array = std::vector<Value>;
using Object = std::map<String, Value>;
using Number = double;
using Boolean = bool;

enum Error {
    kOK,
    kInvalidToken,
};

class Value {
public:
    Value() : type_(kTypeUninitialized) {}
    Value(const Value& value)
    {
        Copy(value);
    }
    ~Value()
    {
        if (type_ == kTypeObject)
            object_.~map();
        if (type_ == kTypeArray)
            array_.~vector();
        if (type_ == kTypeString)
            string_.~basic_string();
    }
    Value(bool b) : type_(kTypeBoolean), boolean_(b) {}
    Value(double n) : type_(kTypeNumber), number_(n) {}
    Value(const Null& null) : type_(kTypeNull) {}
    Value(const char* s) : type_(kTypeString) { new(&string_) String(s); }
    Value(const String& s) : type_(kTypeString) { new(&string_) String(s); }
    Value(const Array& a) : type_(kTypeArray) { new(&array_) Array(a.begin(), a.end()); }
    Value(const Object& obj) : type_(kTypeObject) { new(&object_) Object; }
    
    template<typename T>
    T& Get();
    template<typename T>
    Value& Set(const T& value);
    Value& operator=(const Value& value)
    {
        if (this != &value) {
            Copy(value);
        }
        return *this;
    }
    std::string ToString()
    {
        if (type_ == kTypeNull)
            return "null";
        if (type_ == kTypeBoolean)
            return boolean_ ? "true" : "false";
        if (type_ == kTypeNumber) {
            std::ostringstream oss;
            oss << number_;
            return oss.str();
        }
        if (type_ == kTypeString)
            return "\"" + string_ + "\"";
        if (type_ == kTypeArray) {
            std::string result;
            result += "[";
            for (size_t i = 0; i < array_.size(); i++) {
                if (i)
                    result += ", ";
                result += array_[i].ToString();
            }
            result += "]";
            return result;
        }
        if (type_ == kTypeObject) {
            std::string result;
            result += "{";
            for (auto it = object_.begin(); it != object_.end(); it++) {
                if (it != object_.begin())
                    result += ", ";
                result += "\"" + it->first + "\"";
                result += ": ";
                result += it->second.ToString();
            }
            result += "}";
            return result;
        }
        return "<undef>";
    }
    
private:
    enum Type {
        kTypeObject,
        kTypeArray,
        kTypeString,
        kTypeNumber,
        kTypeBoolean,
        kTypeNull,
        kTypeUninitialized,
    } type_;
    union {
        Object object_;
        Array array_;
        String string_;
        Number number_;
        Boolean boolean_;
        Null null_;
    };
    void Copy(const Value& value)
    {
        if (value.type_ == kTypeObject) {
            if (type_ == kTypeObject)
                object_ = value.object_;
            else
                new(&object_) Object(value.object_.begin(), value.object_.end());
        } else if (value.type_ == kTypeArray) {
            if (type_ == kTypeArray)
                array_ = value.array_;
            else
                new(&array_) Array(value.array_.begin(), value.array_.end());
        } else if (value.type_ == kTypeString) {
            if (type_ == kTypeString)
                string_ = value.string_;
            else
                new(&string_) String(value.string_);
        } else if (value.type_ == kTypeNumber) {
            number_ = value.number_;
        } else if (value.type_ == kTypeBoolean) {
            boolean_ = value.boolean_;
        } else if (value.type_ == kTypeNull) {
            null_ = value.null_;
        }
        type_ = value.type_;
    }
};

template<>
Object& Value::Get<Object>()
{
    assert(type_ == kTypeObject);
    return object_;
}

template<>
Array& Value::Get<Array>()
{
    assert(type_ == kTypeArray);
    return array_;
}

template<>
String& Value::Get<String>()
{
    assert(type_ == kTypeString);
    return string_;
}

template<>
Number& Value::Get<Number>()
{
    assert(type_ == kTypeNumber);
    return number_;
}

template<>
Boolean& Value::Get<Boolean>()
{
    assert(type_ == kTypeBoolean);
    return boolean_;
}

template<>
Null& Value::Get<Null>()
{
    assert(type_ == kTypeNull);
    return null_;
}

template<>
Value& Value::Set<Object>(const Object& value)
{
    if (type_ == kTypeObject)
        object_.~Object();
    type_ = kTypeObject;
    new(&object_) Object(value.begin(), value.end());
    return *this;
}

template<>
Value& Value::Set<Array>(const Array& value)
{
    if (type_ == kTypeArray)
        array_.~Array();
    type_ = kTypeArray;
    new(&array_) Array(value.begin(), value.end());
    return *this;
}

template<>
Value& Value::Set<String>(const String& value)
{
    if (type_ == kTypeString)
        string_.~String();
    type_ = kTypeString;
    new(&string_) String(value);
    return *this;
}

template<>
Value& Value::Set<Number>(const Number& value)
{
    type_ = kTypeNumber;
    number_ = value;
    return *this;
}

template<>
Value& Value::Set<Boolean>(const Boolean& value)
{
    type_ = kTypeBoolean;
    boolean_ = value;
    return *this;
}

template<>
Value& Value::Set<Null>(const Null& value)
{
    type_ = kTypeNull;
    null_ = value;
    return *this;
}

Error Parse(const std::string& s, Value* value_ptr);

static Error ParseHelper(const std::string& s, size_t* start_ptr, Value* value_ptr);
static void SkipSpace(const std::string& s, size_t* start_ptr);
static Error ParseString(const std::string& s, size_t* start_ptr, Value* value_ptr);
static Error ParseNumber(const std::string& s, size_t* start_ptr, Value* value_ptr);
static Error ParseOthers(const std::string& s, size_t* start_ptr, Value* value_ptr);
static Error ParseArray(const std::string& s, size_t* start_ptr, Value* value_ptr);
static Error ParseObject(const std::string& s, size_t* start_ptr, Value* value_ptr);
static Error ParseHelper(const std::string& s, size_t* start_ptr, Value* value_ptr);

void SkipSpace(const std::string& s, size_t* start_ptr)
{
    size_t& i = *start_ptr;
    while (i < s.size() && isspace(s[i])) i++;
}

Error ParseString(const std::string& s, size_t* start_ptr, Value* value_ptr)
{
    size_t& start = *start_ptr;
    if (s[start] != '"')
        return kInvalidToken;

    std::unordered_set<char> hex_digit {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
    };
    Value& value = *value_ptr;
    value.Set<String>("");

    for (size_t j = start + 1, i = j; i < s.size(); i++) {
        if (s[i] == '"') {
            value.Set<String>(s.substr(j, i - j));
            start = i + 1;
            return kOK;
        } else if (s[i] == '\\') {
            if (i + 1 < s.size()) {
                i++;
                if (s[i] == '"' || s[i] == '\\' || s[i] == '/' || s[i] == 'b' || 
                    s[i] == 'f' || s[i] == 'n' || s[i] == 'r' || s[i] == 't') {
                    continue;
                } else if (s[i] == 'u') {
                    for (int j = 1; j <= 4 && i + j < s.size(); j++) {
                        if (!hex_digit.count(toupper(s[i + j]))) {
                            return kInvalidToken;
                        }
                    }
                    i += 4;
                }
            }
        }
    }
    return kInvalidToken;
}

Error ParseNumber(const std::string& s, size_t* start_ptr, Value* value_ptr)
{
    size_t j = *start_ptr;
    size_t& i = *start_ptr;

    if (s[i] == '-') {
        i++;
        if (i >= s.size())
            return kInvalidToken;
    }
    if (s[i] == '0')
        i++;
    else if (s[i] >= '1' && s[i] <= '9') {
        i++;
        if (i >= s.size())
            return kInvalidToken;
        while (i < s.size() && isdigit(s[i])) i++;
    }
    if (i >= s.size())
        goto Exit;
    if (s[i] == '.') {
        i++;
        if (i >= s.size())
            return kInvalidToken;
        while (i < s.size() && isdigit(s[i])) i++;
        if (i >= s.size())
            goto Exit;
    }
    if (toupper(s[i]) == 'E') {
        i++;
        if (i >= s.size())
            return kInvalidToken;
        if (s[i] == '+' || s[i] == '-') {
            i++;
            if (i >= s.size())
                return kInvalidToken;
        }
        while (i < s.size() && isdigit(s[i])) i++;
    }

Exit:    
    Value& value = *value_ptr;
    value.Set<Number>(stod(s.substr(j, i - j)));

    return kOK;
}

Error ParseOthers(const std::string& s, size_t* start_ptr, Value* value_ptr)
{
    const static std::vector<std::pair<std::string, Value>> token_info{
        {"true", Value(true)},
        {"false", Value(false)},
        {"null", Value(null)},
    };
    size_t& start = *start_ptr;
    Value& value = *value_ptr;

    for (auto& t : token_info) {
        std::string name = t.first;
        int length = name.size();
        if (start + length - 1 < s.size() && !s.compare(start, length, name)) {
            value = t.second;
            start += length;
            return kOK;
        }
    }
    return kInvalidToken;
}

Error ParseArray(const std::string& s, size_t* start_ptr, Value* value_ptr)
{
    size_t& start = *start_ptr;
    if (s[start] != '[')
        return kInvalidToken;
    start++;
    
    Value& value = *value_ptr;
    value.Set<Array>({});
    
    size_t i = start;
    while (i < s.size()) {
        SkipSpace(s, &i);
        if (i >= s.size())
            return kInvalidToken;
        if (s[i] == ']') {
            i++;
            break;
        }
        Value v;
        Error err = ParseHelper(s, &i, &v);
        if (err != kOK)
            return err;
        value.Get<Array>().push_back(v);
        SkipSpace(s, &i);
        if (i >= s.size())
            return kInvalidToken;
        if (s[i] == ',') {
            i++;
        }
    }
    start = i;
    return kOK;
}

Error ParseObject(const std::string& s, size_t* start_ptr, Value* value_ptr)
{
    size_t& start = *start_ptr;
    if (s[start] != '{')
        return kInvalidToken;
    start++;

    Value& value = *value_ptr;
    value.Set<Object>({});

    size_t i = start;
    while (i < s.size()) {
        SkipSpace(s, &i);
        if (i >= s.size())
            return kInvalidToken;
        if (s[i] == '}') {
            i++;
            break;
        }
        Value string;
        Error err = ParseString(s, &i, &string);
        if (err != kOK)
            return err;
        SkipSpace(s, &i);
        if (i >= s.size() || s[i] != ':')
            return kInvalidToken;
        i++;
        SkipSpace(s, &i);
        if (i >= s.size())
            return kInvalidToken;
        Value v;
        err = ParseHelper(s, &i, &v);
        if (err != kOK)
            return err;
        value.Get<Object>()[string.Get<String>()] = v;
        SkipSpace(s, &i);
        if (i >= s.size())
            return kInvalidToken;
        if (s[i] == ',') {
            i++;
        }           
    }
    start = i;
    return kOK;
}

Error ParseHelper(const std::string& s, size_t* start_ptr, Value* value_ptr)
{
    Error err;
    size_t& i = *start_ptr;

    SkipSpace(s, &i);
    if (i >= s.size()) return kOK;

    if (s[i] == '"')
        err = ParseString(s, &i, value_ptr);
    else if (isdigit(s[i]) || s[i] == '-')
        err = ParseNumber(s, &i, value_ptr);
    else if (s[i] == '[')
        err = ParseArray(s, &i, value_ptr);
    else if (s[i] == '{')
        err = ParseObject(s, &i, value_ptr);
    else
        err = ParseOthers(s, &i, value_ptr);
    return err;        
}

Error Parse(const std::string& s, Value* value_ptr)
{
    size_t i = 0;
    return ParseHelper(s, &i, value_ptr);
}
} // namespace
