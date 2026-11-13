//
// Created by Deondre English on 10/27/26.
//

#ifndef JSON_STRING_H
#define JSON_STRING_H

#include "Data.hpp"
#include "jsonmanip.hpp"

namespace json {

class String : public Data
{
    std::string val;
public:
    String() : val{""} {}
    String(const std::string& v) : val{v} {}
    String(std::string&& v) : val{std::move(v)} {}
    String(const char* v) : val{v} {}

    bool isString() const override { return true; }
    std::string& getString() override { return val; }
    const std::string& getString() const override { return val; }

    void print(std::ostream& os) const override { os << json_string(val); }
    bool equals(const Data& other) const override {
        return other.isString() && val == other.getString();
    }
    
    std::unique_ptr<Data> clone() const override { return std::make_unique<String>(val); }
    std::unique_ptr<Data> eject() override { 
        return std::make_unique<String>(std::move(val));
    }
};

}

#endif //JSON_STRING_H
