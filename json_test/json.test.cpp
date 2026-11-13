//
// Created by Deondre English on 10/27/26.
//

#include <gtest/gtest.h>
#include <json.hpp>

using namespace json;

TEST(JsonScalarTest, DefaultConstruction) {
    Null n;
    Boolean b;
    Number num;
    String s;

    EXPECT_TRUE(n.isNull());
    EXPECT_FALSE(b.getBoolean());
    EXPECT_DOUBLE_EQ(num.getNumber(), 0.0);
    EXPECT_EQ(s.getString(), "");
}

TEST(JsonScalarTest, ImplicitInitialization) {
    Boolean b = true;
    Number n = 42.5;
    String s1 = std::string("test");
    String s2 = "literal";

    EXPECT_TRUE(b.getBoolean());
    EXPECT_DOUBLE_EQ(n.getNumber(), 42.5);
    EXPECT_EQ(s1.getString(), "test");
    EXPECT_EQ(s2.getString(), "literal");
}

TEST(JsonDataTest, TypeIdentification) {
    std::unique_ptr<Data> data = std::make_unique<Number>(10.0);
    
    EXPECT_TRUE(data->isNumber());
    EXPECT_FALSE(data->isNull());
    EXPECT_FALSE(data->isBoolean());
    EXPECT_FALSE(data->isString());
}

TEST(JsonDataTest, TypeErrorHandling) {
    Number n(5.0);
    Data& d = n;

    EXPECT_THROW(d.getBoolean(), TypeError);
    EXPECT_THROW(d.getString(), TypeError);
    
    // Check error message content
    try {
        d.getString();
    } catch (const TypeError& e) {
        EXPECT_STREQ(e.what(), "Not a string");
    }
}

TEST(JsonDataTest, SetValueViaReference) {
    String s("old");
    Data& d = s;
    
    d.getString() = "new";
    EXPECT_EQ(s.getString(), "new");
}


TEST(JsonIOTest, Printing) {
    Null n;
    Boolean b(true);
    Number num(1.23);
    String s("hello");

    std::stringstream ss;
    
    ss << n;
    EXPECT_EQ(ss.str(), "null");
    ss.str("");

    ss << b;
    EXPECT_EQ(ss.str(), "true");
    ss.str("");

    ss << num;
    EXPECT_FALSE(ss.str().empty()); 
    ss.str("");

    ss << s;
    EXPECT_EQ(ss.str(), "\"hello\""); 
}

TEST(JsonLogicTest, Equality) {
    Number n1(10.0);
    Number n2(10.0);
    Number n3(20.0);
    Boolean b(true);

    EXPECT_TRUE(n1 == n2);
    EXPECT_FALSE(n1 == n3);
    EXPECT_FALSE(n1 == b); // Different types
}

TEST(JsonLogicTest, Clone) {
    String original("copy me");
    auto cloned = original.clone();

    EXPECT_NE(cloned.get(), &original); // Different memory addresses
    EXPECT_TRUE(*cloned == original);
    EXPECT_EQ(original.getString(), "copy me"); // Original unchanged
}

TEST(JsonLogicTest, Eject) {
    String original("move me");
    auto ejected = original.eject();

    EXPECT_TRUE(ejected->isString());
    EXPECT_EQ(ejected->getString(), "move me");
    EXPECT_EQ(original.getString(), ""); // String was moved/cleared
}

TEST(JsonLogicTest, EjectScalarDoesNotClear) {
    Number original(3.14);
    auto ejected = original.eject();

    EXPECT_DOUBLE_EQ(ejected->getNumber(), 3.14);
    EXPECT_DOUBLE_EQ(original.getNumber(), 3.14); // Scalars aren't cleared
}

TEST(VarPhase2, NullEfficiency) {
    json::Var v;
    EXPECT_TRUE(v->isNull());

    json::Var v2 = std::move(v);
    EXPECT_TRUE(v->isNull()); // Source remains null/nullptr
    EXPECT_TRUE(v2->isNull());
}

TEST(VarPhase2, ArithmeticTemplate) {
    json::Var v = 42; // int -> double via template
    EXPECT_TRUE(v->isNumber());
    EXPECT_DOUBLE_EQ(v->getNumber(), 42.0);
}