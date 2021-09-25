#include <iostream>
#include <cassert>
#include "kobato.h"

int main()
{
    {
        kobato::Value v;
        assert(kobato::Parse("", &v) == kobato::kOK);
        assert(kobato::Parse("  ", &v) == kobato::kOK);
        assert(kobato::Parse("\n", &v) == kobato::kOK);
        assert(kobato::Parse("\t", &v) == kobato::kOK);
        assert(kobato::Parse("\"hello world\"", &v) == kobato::kOK && v.ToString() == "\"hello world\"");
        assert(kobato::Parse("   \"hello world\"     ", &v) == kobato::kOK && v.ToString() == "\"hello world\"");
        assert(kobato::Parse("\"hello\\nworld\"", &v) == kobato::kOK && v.ToString() == "\"hello\\nworld\"");
        assert(kobato::Parse("\"\\u002F\"", &v) == kobato::kOK && v.ToString() == "\"\\u002F\"");
        assert(kobato::Parse("\"\\u002f\"", &v) == kobato::kOK && v.ToString() == "\"\\u002f\"");
        assert(kobato::Parse("\"", &v) == kobato::kInvalidToken);
        assert(kobato::Parse("'foo'", &v) == kobato::kInvalidToken);
        assert(kobato::Parse("314", &v) == kobato::kOK && v.ToString() == "314");
        assert(kobato::Parse("3.14", &v) == kobato::kOK && v.ToString() == "3.14");
        assert(kobato::Parse("-3.14", &v) == kobato::kOK && v.ToString() == "-3.14");
        assert(kobato::Parse("3.", &v) == kobato::kInvalidToken);
        assert(kobato::Parse("3.14e2", &v) == kobato::kOK && v.ToString() == "314");
        assert(kobato::Parse("3.14e-1", &v) == kobato::kOK && v.ToString() == "0.314");
        assert(kobato::Parse("true", &v) == kobato::kOK && v.ToString() == "true");
        assert(kobato::Parse("false", &v) == kobato::kOK && v.ToString() == "false");
        assert(kobato::Parse("null", &v) == kobato::kOK && v.ToString() == "null");
        assert(kobato::Parse("[true, false, null]", &v) == kobato::kOK && v.ToString() == "[true, false, null]");
        assert(kobato::Parse("[true, false, null,]", &v) == kobato::kOK && v.ToString() == "[true, false, null]");
        assert(kobato::Parse("[]", &v) == kobato::kOK && v.ToString() == "[]");
        assert(kobato::Parse("[,]", &v) == kobato::kInvalidToken);
        assert(kobato::Parse("[true,,]", &v) == kobato::kInvalidToken);
        assert(kobato::Parse("{}", &v) == kobato::kOK && v.ToString() == "{}");
        assert(kobato::Parse("{\"foo\":true}", &v) == kobato::kOK && v.ToString() == "{\"foo\": true}");
        assert(kobato::Parse("{\"foo\":[\"bar\", false, null]}", &v) == kobato::kOK && v.ToString() == "{\"foo\": [\"bar\", false, null]}");
        assert(kobato::Parse("{\"foo\":{\"bar\":\"baz\"}}", &v) == kobato::kOK && v.ToString() == "{\"foo\": {\"bar\": \"baz\"}}");
        assert(kobato::Parse("{true}", &v) == kobato::kInvalidToken);
        assert(kobato::Parse("{314}", &v) == kobato::kInvalidToken);
        assert(kobato::Parse("{foo:false}", &v) == kobato::kInvalidToken);
    }

    {
        kobato::Value v(kobato::Array{});
        auto& a = v.Get<kobato::Array>();
        a.push_back("foo");
        a.push_back(3.14);
        a.push_back(true);
        a.push_back(kobato::null);
        assert(v.ToString() == "[\"foo\", 3.14, true, null]");
    }

    {
        kobato::Value v(kobato::Object{});
        auto& o = v.Get<kobato::Object>();
        o["foo"] = "bar";
        o["baz"] = true;
        o["key"] = kobato::Array{"abc", 3.14, false, "xyz"};
        assert(v.ToString() == "{\"baz\": true, \"foo\": \"bar\", \"key\": [\"abc\", 3.14, false, \"xyz\"]}");
    }

    std::cout << "\e[32mAll test have passed.\e[39m" << std::endl;
}
