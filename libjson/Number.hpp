//
// Created by Deondre English on 10/27/26.
//

#ifndef JSON_NUMBER_H
#define JSON_NUMBER_H
#include "Data.hpp"
#include "jsonmanip.hpp"

namespace json {

class Number : public Data
{
    double val;
public:
    Number(double v = 0.0) : val{v} {}

    bool isNumber() const override { return true; }
    double& getNumber() override { return val; }
    const double& getNumber() const override { return val;}
    
    void print(std::ostream& os) const override { os << json_number(val); }

    bool equals(const Data& other) const override {
        return other.isNumber() && val == other.getNumber();
    }

    std::unique_ptr<Data> clone() const override { return std::make_unique<Number>(val); }
    std::unique_ptr<Data> eject() override { return clone(); }
};

}



#endif //JSON_NUMBER_H
