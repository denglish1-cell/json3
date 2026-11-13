//
// Created by Deondre English on 10/27/26.
//

#ifndef JSON_BOOLEAN_H
#define JSON_BOOLEAN_H

#include "Data.hpp"

namespace json {

    class Boolean : public Data
    {
    bool val;
    public:
        Boolean(bool v = false) : val(v) {}

        bool isBoolean() const override { return true; }
        bool& getBoolean() override { return val; }
        const bool& getBoolean() const override { return val; } 

        void print(std::ostream& os) const override { os << (val ? "true" : "false"); }

        bool equals(const Data& other) const override {
            return other.isBoolean() && val == other.getBoolean();
        }

        std::unique_ptr<Data> clone() const override { return std::make_unique<Boolean>(val); }
        std::unique_ptr<Data> eject() override { return clone(); }
    };

}



#endif //JSON_BOOLEAN_H
