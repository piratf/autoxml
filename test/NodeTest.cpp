//
// Created by piratf on 2017/12/10.
//

#include "autoxml.h"
#include "gtest/gtest.h"

TEST(autoxml__Test, read_Element)
{
    int aiValue[6] = {};
    int eq_aiValue[6] = {1, 2, 3, 4, 5, 6};
    AUTO_XML("xml4test.xml", "document");
    TiXmlElement *pElem = GET_ELEM("TestElem")
    TiXmlElement *cElem = pElem->FirstChildElement();
    for (size_t i = 0; i < 6; ++i) {
        autoxml.GetDataFromElem(&aiValue[i], cElem);
        cElem = cElem->NextSiblingElement();
    }

    for (size_t i = 0; i < 6; ++i) {
        EXPECT_EQ(aiValue[i], eq_aiValue[i]);
    }
}

TEST(autoxml__Test, read_Element_From_Element)
{
    AUTO_XML("xml4test.xml", "document");
    TiXmlElement *pElem = GET_ELEM("Test")
    TiXmlElement *pElemInside = GET_ELEM_FROM_ELEM(pElem, "Inside");
    int iValue = 0, eq_iValue = 233;
    BIND_ELEM(&iValue, pElemInside, "Value")
    EXPECT_EQ(iValue, eq_iValue);
}

TEST(autoxml__Test, read_RelativePath_Back)
{
    AUTO_XML("xml4test.xml", "document");
    TiXmlElement *pElem = GET_ELEM("Test")
    TiXmlElement *pElemInside = GET_ELEM_FROM_ELEM(pElem, "Inside", "..", "Inside");
    int iValue = 0, eq_iValue = 233;
    BIND_ELEM(&iValue, pElemInside, "Value")
    EXPECT_EQ(iValue, eq_iValue);
}