#include "autoxml.h"
#include <cstdio>
#include <string>
using namespace AutoXML_NS;

struct STAutoXMLDesc {
    std::string m_strName;
    std::string m_strVersion;
    std::string m_strAuthor;
    int m_iTest;
    double m_dTest;
};

void Display(const STAutoXMLDesc &desc)
{
    printf("Name: %s\n", desc.m_strName.c_str());
    printf("Version: %s\n", desc.m_strVersion.c_str());
    printf("Author: %s\n", desc.m_strAuthor.c_str());
    printf("Test int: %d\n", desc.m_iTest);
    printf("Test double: %lf\n", desc.m_dTest);
}

bool LoadCfg() {
    STAutoXMLDesc desc;
    AUTO_XML("xml4test.xml", "document");
    BIND_XML(&desc.m_strName, "TheName");
    BIND_XML(&desc.m_strVersion, "Version", "Value");
    BIND_XML(&desc.m_strAuthor, "Version", "Author");
    BIND_XML(&desc.m_iTest, "Test", "Int");
    BIND_XML(&desc.m_dTest, "Test", "Double");
    Display(desc);
    return true;
}

void run() {
    LoadCfg();
}

int main() {
    run();
    return 0;
}
