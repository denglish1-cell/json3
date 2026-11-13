#pragma once

#include "Data.hpp"

namespace json
{
    class Array : public Data
    {
        std::vector<Var> _val;
    public:
        Array() = default;
        Array(std::vector<Var> v) : _val(std::move(v)) {}
        Array(std::initializer_list<Var> list) : _val(list) {}

        bool isArray() const override { return true; }
        std::vector<Var>& getArray() override { return _val; }
        const std::vector<Var>& getArray() const override { return _val; }

        void print(std::ostream& os) const override
        {
            os << "[";
            auto& v = getArray();
            for (size_t i = 0; i < v.size(); ++i) {
                os << v[i]; // This uses the Var operator<<
                if (i < v.size() - 1) os << ",";
            }
            os << "]";
        }

        bool equals(const Data& other) const override
        {
            return other.isArray() && _val == other.getArray();
        }

        std::unique_ptr<Data> clone() const override { return std::make_unique<Array>(_val); }
        std::unique_ptr<Data> eject()  override { return std::make_unique<Array>(std::move(_val)); }
    };
}