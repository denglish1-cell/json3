//
// Created by Deondre English on 10/27/26.
//

#ifndef JSON_NULL_H
#define JSON_NULL_H
#include "Data.hpp"

namespace json {

    class Null : public Data
    {
    public:
        Null(std::nullptr_t = nullptr) {} // explicit? ??

        bool isNull() const override { return true;}
        void print(std::ostream& os) const override { os << "null"; }

        bool equals(const Data& other) const override { return other.isNull(); }

        std::unique_ptr<Data> clone() const override { return std::make_unique<Null>(); }
        std::unique_ptr<Data> eject() override { return clone(); }
    };

}


#endif //JSON_NULL_H
