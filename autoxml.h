#include <cstdio>
#include <cstdarg>
#include <string>
#include "tinyxml.h"

#define MERROR(format, ...) fprintf (stderr, "[%s](line %lu): " format "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define AUTOXML_MERROR(format, ...) MERROR(format "->[%s]:line %lu", ##__VA_ARGS__, m_strCurFile, m_sizeCurLine)
#define AUTOXML_MDEBUG(format, ...) MDEBUG(format "->[%s]:line %lu", ##__VA_ARGS__, m_strCurFile, m_sizeCurLine)
#ifdef DEBUG
#define MDEBUG(format, ...) fprintf (stdout, "[%s](line %lu): " format "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define MDEBUG(format, ...) void(0)
#endif
#define AUTO_XML(filename, root) AutoXML_NS::AutoXML autoxml(filename, root, __FILE__, __LINE__);
#define BIND_XML(address, ...) \
    if (!autoxml.BindXML(address, __FILE__, __LINE__, PP_NARG(address, ##__VA_ARGS__), ##__VA_ARGS__)) { \
        return false; \
    }

#define PP_NARG(...) \
    PP_NARG_(__VA_ARGS__,PP_RSEQ_N())
#define PP_NARG_(...) \
    PP_ARG_N(__VA_ARGS__)
#define PP_ARG_N( \
    _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, N,...) N
#define PP_RSEQ_N() \
    9,8,7,6,5,4,3,2,1,0

namespace AutoXML_NS
{
    template<class T>
        struct is_pointer { static const bool value = false; };

    template<class T>
        struct is_pointer<T*> { static const bool value = true; };

    template<class To, class Src>
        class is_convertible {
            struct OTHER {char data[2];};
            static char Test(To);
            static OTHER Test(...);
            static Src Usage();
        public:
            enum {value = sizeof(Test(Usage())) == sizeof(char)};
        };

    class AutoXML
    {
    public:
        AutoXML (const char* filename, const char *root, const char *cur_file, size_t cur_line):
            m_stDoc(filename), m_pRoot(NULL)
        {
            SetFileLine(cur_file, cur_line);
            if (m_stDoc.LoadFile()) {
                m_pRoot = m_stDoc.FirstChildElement(root);
            }
            else {
                AUTOXML_MERROR("Load File(%s) Error.", filename);
            }
        }

        //Modifier
        void SetFileLine(const char *cur_file, size_t cur_line)
        {
            m_strCurFile = cur_file;
            m_sizeCurLine = cur_line;
        }

        template<class T>
            bool check_type(T *address)
            {
                if (is_pointer<T>::value) {
                    MERROR("address is a pointer, error.");
                    return false;
                } return true;
            }

        template<class T>
            bool GetData(T *address, const char* data)
            {
                if (!address || !data) {
                    AUTOXML_MERROR("pointer is NULL");
                    return false;
                }
                if (is_convertible<T, std::string>::value) {
                    AUTOXML_MDEBUG("Data type is std::string");
                    reinterpret_cast<std::string*>(address)->assign(data);
                }
                else if (is_convertible<T, int>::value) {
                    AUTOXML_MDEBUG("Data type is int");
                    *reinterpret_cast<int*>(address) = atoi(data);
                }
                else if (is_convertible<T, double>::value) {
                    AUTOXML_MDEBUG("Data type is int");
                    *reinterpret_cast<double*>(address) = atof(data);
                }
                else if (is_convertible<T, long>::value) {
                    AUTOXML_MDEBUG("Data type is long");
                    *reinterpret_cast<long*>(address) = atol(data);
                }
                else if (is_convertible<T, long long>::value) {
                    AUTOXML_MDEBUG("Data type is long long");
                    *reinterpret_cast<long*>(address) = atoll(data);
                }
                return true;
            }

        template<class T>
            bool BindXML(T *address, const char* cur_file, size_t cur_line, size_t cnt, ...)
            {
                SetFileLine(cur_file, cur_line);
                bool ret = check_type(address);
                if (!ret) {
                    return false;
                }
                if (!m_pRoot) {
                    MERROR("Load Root Element Failed.");
                    return false;
                }

                MDEBUG("Num Of Args: %d", cnt);

                va_list args;
                va_start(args, cnt);
                TiXmlElement *pElem = m_pRoot;
                for (size_t i = 0; i < cnt; ++i) {
                    const char *name = va_arg(args, const char*);
                    AUTOXML_MDEBUG("Current Name: %s", name);
                    // 如果是最后一个节点，就考虑找 Attr 或者 GetText
                    if (i + 1 == cnt) {
                        AUTOXML_MDEBUG("Attr Name: %s", name);
                        const char* data = pElem->Attribute(name);
                        if (!data) {
                            pElem = pElem->FirstChildElement(name);
                            data = pElem->GetText();
                            if (!data) {
                                AUTOXML_MERROR("Can't Find Attribute(%s)", name);
                                return false;
                            } else {
                                MDEBUG("Get Data is %s", data);
                                return GetData(address, data);
                            }
                        } else {
                            MDEBUG("Get Data is %s", data);
                            return GetData(address, data);
                        }
                    } else {
                        // 否则就向下寻找 Element 节点
                        pElem = pElem->FirstChildElement(name);
                        if (!pElem) {
                            AUTOXML_MERROR("Can't Find Child Element(%s)", name);
                            return false;
                        }
                    }
                }
                va_end(args);
                return true;
            }

    private:
        TiXmlDocument m_stDoc;
        TiXmlElement *m_pRoot;
        const char* m_strCurFile;
        size_t m_sizeCurLine;
    };
}

