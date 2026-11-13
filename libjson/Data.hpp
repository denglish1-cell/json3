//
// Created by Deondre English on 10/27/26.
//

#ifndef JSON_DATA_H
#define JSON_DATA_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <memory>
#include "jsonmanip.hpp"

namespace json {
class Var;
struct TypeError : public std::runtime_error 
{
    explicit TypeError(const std::string& message) : std::runtime_error(message) {}
};

class Data
{
public:
    virtual ~Data() = default;

    virtual bool isNull() const { return false; }
    virtual bool isBoolean() const { return false; }
    virtual bool isNumber() const { return false; }
    virtual bool isString() const { return false; }
    virtual bool isArray() const { return false; }
    virtual bool isObject() const  { return false; }

    virtual bool& getBoolean() { throw TypeError("Not a boolean"); }
    virtual const bool& getBoolean() const { throw TypeError("Not a boolean"); }
    
    virtual double& getNumber() { throw TypeError("Not a number"); }
    virtual const double& getNumber() const { throw TypeError("Not a number"); }
    
    virtual std::string& getString() { throw TypeError("Not a string"); }
    virtual const std::string& getString() const { throw TypeError("Not a string"); }

    virtual std::vector<Var>& getArray() { throw TypeError("Not an array"); }
    virtual const std::vector<Var>& getArray() const {  throw TypeError("Not an array"); }

    virtual std::map<std::string, Var>& getObject() { throw TypeError("Not an object"); }
    virtual const std::map<std::string, Var>& getObject() const { throw TypeError("Not an object"); }

    virtual void print(std::ostream& os) const = 0;
    virtual bool equals(const Data& other) const = 0;
    virtual std::unique_ptr<Data> clone() const = 0;
    virtual std::unique_ptr<Data> eject() = 0;
};

inline
std::ostream& operator<<(std::ostream& os, const Data& data) 
{
    data.print(os);
    return os;
}

inline 
bool operator==(const Data& lhs, const Data& rhs)
{
    return lhs.equals(rhs);
}


}

#endif //JSON_DATA_H
