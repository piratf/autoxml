//
// Created by piratf on 2017/12/10.
//

#include "autoxml.h"
#include "gtest/gtest.h"

TEST(autoxml__Test, read_int)
{
    int i = 0, eq = 1;
    AUTO_XML("xml4test.xml", "document");
    BIND_XML(&i,  "Test", "Int");
    EXPECT_EQ(i, eq);
}

TEST(autoxml__Test, read_double)
{
    double i = 0, eq = 0.03233;
    AUTO_XML("xml4test.xml", "document");
    BIND_XML(&i,  "Test", "Double");
    EXPECT_EQ(i, eq);
}

TEST(autoxml__Test, read_double_negative)
{
    double i = 0, eq = -0.6666666;
    AUTO_XML("xml4test.xml", "document");
    BIND_XML(&i,  "Test", "NegativeDouble");
    EXPECT_EQ(i, eq);
}

TEST(autoxml__Test, read_short)
{
    short i = 0, eq = -1;
    AUTO_XML("xml4test.xml", "document");
    BIND_XML(&i,  "Test", "Short");
    EXPECT_EQ(i, eq);
}

TEST(autoxml__Test, read_ushort)
{
    unsigned short i = 0, eq = 233;
    AUTO_XML("xml4test.xml", "document");
    BIND_XML(&i,  "Test", "UnsignedShort");
    EXPECT_EQ(i, eq);
}

TEST(autoxml__Test, read_cstr)
{
    char str[100] = {};
    char eq[100] = "Test CStr.";
    AUTO_XML("xml4test.xml", "document");
    BIND_XML(&str,  "Test", "CStr");
    EXPECT_STREQ(str, eq);
}