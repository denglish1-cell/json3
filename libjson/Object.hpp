#pragma once

#include "Data.hpp"

namespace json
{
    class Object : public Data
    {
        std::map<std::string, Var> _val;
    public:
        Object() = default;
        Object(std::map<std::string, Var> m) : _val(std::move(m)) {}
        Object(std::initializer_list<std::pair<const std::string, Var>> list)
            : _val(list) {}
        bool isObject() const override { return true; }
        std::map<std::string, Var>& getObject() override { return _val; }
        const std::map<std::string, Var>& getObject() const override { return _val; }

        void print(std::ostream& os) const override {
            os << "{";
            auto& m = getObject();
            for (auto it = m.begin(); it != m.end(); ++it) {
                os << json_string(it->first) << ":" << it->second;
                if (std::next(it) != m.end()) os << ",";
            }
            os << "}";
        }

        bool equals(const Data& other) const override {
            return other.isObject() && _val == other.getObject();
        }

        std::unique_ptr<Data> clone() const override { return std::make_unique<Object>(_val); }
        std::unique_ptr<Data> eject() override { return std::make_unique<Object>(std::move(_val)); }
    };
}