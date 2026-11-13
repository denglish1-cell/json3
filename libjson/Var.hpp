#ifndef JSON_VAR_H
#define JSON_VAR_H

#include <memory>
#include <type_traits>
#include "Data.hpp"
#include "Null.hpp"
#include "Boolean.hpp"
#include "Number.hpp"
#include "String.hpp"

namespace json
{
    class Array;
    class Object;

    class Var
    {
        std::unique_ptr<Data> _ptr;
    public:
        Var() : _ptr{std::make_unique<Null>()} {}

        // [VarMove]
        Var(Var&& other) noexcept : _ptr{std::move(other._ptr)}
        {
            other._ptr = std::make_unique<Null>();
        }
        Var& operator=(Var&& other) noexcept
        {
            if (this != &other)
            {
                _ptr = std::move(other._ptr);
                other._ptr = std::make_unique<Null>();
            }
            return *this;
        }

        // [VarDeference]
        Data& operator*() { return *_ptr; }
        Data const& operator*() const { return *_ptr; }
        Data* operator->() { return _ptr.get(); }
        Data const* operator->() const { return _ptr.get(); }

        // [VarCopy]
        Var(Var const& other) : _ptr{other._ptr->clone()} {}
        Var& operator=(Var const& other)
        {
            if (this != &other)
            {
                _ptr = other._ptr->clone();
            }
            return *this;
        }


        // [VarConvertVal]
        Var(std::nullptr_t) : _ptr{std::make_unique<Null>()} {}
        Var(bool b) : _ptr{std::make_unique<Boolean>(b)} {}
        Var(double d) : _ptr{std::make_unique<Number>(d)} {}
        Var(std::string s) : _ptr{std::make_unique<String>(std::move(s))} {}
        Var(const char* s) : _ptr{std::make_unique<String>(s)} {}
        Var(std::vector<Var> v);
        Var(std::map<std::string, Var> m);

        template<typename T>
        requires std::is_arithmetic_v<T>
        Var(T src) : Var(static_cast<double>(src)) {}

        // [VarConvertData]
        Var(Data const& d) : _ptr{d.clone()} {}
        Var(Data&& d) : _ptr{d.eject()} {}

        Var(std::unique_ptr<Data> p) : _ptr{std::move(p)}
        {
            if (!_ptr) _ptr = std::make_unique<Null>();
        }

        bool equals(Var const& other) const
        {
            return _ptr->equals(*other._ptr);
        }
    };

    inline std::ostream& operator<<(std::ostream& os, Var const& v)
    {
        v->print(os);
        return os;
    }

    inline bool operator==(Var const& lhs, Var const& rhs)
    {
        return lhs.equals(rhs);
    }

    inline bool operator==(Var const& lhs, Data const& rhs)
    {
        return (*lhs).equals(rhs);
    }

    inline bool operator==(Data const& lhs, Var const& rhs)
    {
        return lhs.equals(*rhs);
    }

    inline bool operator!=(Var const& lhs, Var const& rhs)
    {
        return !(lhs == rhs);
    }
}


#include "Array.hpp"
#include "Object.hpp"

namespace json {
    inline Var::Var(std::vector<Var> v)
        : _ptr{std::make_unique<Array>(std::move(v))} {}

    inline Var::Var(std::map<std::string, Var> m)
        : _ptr{std::make_unique<Object>(std::move(m))} {}
}

#endif