#include <cstdio>
#include <cstdarg>
#include <string>
#include <limits>
#include <typeinfo>
#include <cerrno>
#include "tinyxml.h"

#ifdef MERROR
#error MERROR is already defined.
#else
#define MERROR(format, ...) fprintf (stderr, "[%s](line %lu): " format "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#ifdef MDEBUG
#error MDEBUG is already defined.
#else
#ifdef DEBUG
#define MDEBUG(format, ...) fprintf (stdout, "[%s](line %lu): " format "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define MDEBUG(format, ...) void(0)
#endif
#endif

#ifdef AUTOXML_MERROR
#error AUTOXML_MERROR is already defined.
#else
#define AUTOXML_MERROR(format, ...) MERROR(format "->[%s]:line %lu", ##__VA_ARGS__, m_strCurFile, m_sizeCurLine)
#endif

#ifdef AUTOXML_MDEBUG
#error AUTOXML_MDEBUG is already defined.
#else
#define AUTOXML_MDEBUG(format, ...) MDEBUG(format "->[%s]:line %lu", ##__VA_ARGS__, m_strCurFile, m_sizeCurLine)
#endif

#ifdef AUTO_XML
#error AUTO_XML is already defined.
#else
#define AUTO_XML(filename, root) AutoXML_NS::AutoXML autoxml(filename, root, __FILE__, __LINE__);
#endif

#ifdef BIND_XML
#error BIND_XML is already defined.
#else
#define BIND_XML(address, ...) \
    if (!autoxml.BindXML(address, __FILE__, __LINE__, ARG_NUM(address, ##__VA_ARGS__), ##__VA_ARGS__)) { \
        return false; \
    }
#endif

// https://stackoverflow.com/questions/2308243/macro-returning-the-number-of-arguments-it-is-given-in-c
// no more than 64
#ifdef ARG_NUM
#error ARG_NUM is already defined.
#else
#define ARG_NUM(...) \
    ARG_NUM_(__VA_ARGS__, ARG_SEQ_N())
#endif
#ifdef ARG_NUM_
#error ARG_NUM is already defined.
#else
#define ARG_NUM_(...) \
    ARG_NUM_N(__VA_ARGS__)
#endif
#ifdef ARG_NUM_N
#error ARG_NUM_N is already defined.
#else
#define ARG_NUM_N( \
    _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N,...) N
#endif
#ifdef ARG_SEQ_N
#error ARG_SEQ_N is already defined.
#else
#define ARG_SEQ_N() \
    9,8,7,6,5,4,3,2,1,0
#endif

namespace AutoXML_NS
{
    // https://stackoverflow.com/questions/1055452/c-get-name-of-type-in-template
    template<typename T> const char *GetTypeName() { return typeid(T).name(); }

#define DEFINE_TYPE_NAME(type) \
    template<>const char *GetTypeName<type>(){return #type;}

    DEFINE_TYPE_NAME(bool);
    DEFINE_TYPE_NAME(char);
    DEFINE_TYPE_NAME(unsigned char);
    DEFINE_TYPE_NAME(short);
    DEFINE_TYPE_NAME(unsigned short);
    DEFINE_TYPE_NAME(int);
    DEFINE_TYPE_NAME(unsigned int);
    DEFINE_TYPE_NAME(long);
    DEFINE_TYPE_NAME(unsigned long);
    DEFINE_TYPE_NAME(long long);
    DEFINE_TYPE_NAME(unsigned long long);
    DEFINE_TYPE_NAME(float);
    DEFINE_TYPE_NAME(double);
    DEFINE_TYPE_NAME(std::string);
    //================================================================================

    // Type Checkers
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

    template<class To, class Src>
        struct is_same { static const bool value = false; };

    template<class T>
        struct is_same<T, T> { static const bool value = true; };


    // for type id
    enum {
        INT = 1,
        LONG,
        DOUBLE,
        STRING,
        OTHER
    };

    // for src type match base type
    template<class Src>
        class best_match {
            struct Int {char data;};
            struct Long {char data[2];};
            struct Double {char data[3];};
            struct String {char data[4];};
            struct OTHER {char data[5];};
            static Int Test(int);
            static Long Test(long);
            static Double Test(double);
            static String Test(std::string);
            static OTHER Test(...);
            static Src Usage();
        public:
            enum {value = sizeof(Test(Usage())) };
        };

    // for type limit check
    template<class To, class Match, class Cur>
        bool CheckType(Cur data) {
            if (data > std::numeric_limits<Match>::max()) {
                MERROR("value(%ld) greater than the maximum value of MatchType(%s).", data, GetTypeName<Match>());
                errno = ERANGE;
                return false;
            }
            if (data > std::numeric_limits<To>::max()) {
                MERROR("value(%ld) greater than the maximum value of Your Data Type(%s).", data, GetTypeName<To>());
                errno = ERANGE;
                return false;
            }
            if (data < std::numeric_limits<Match>::min()) {
                MERROR("value(%ld) lower than the minimal value of MatchType(%s).", data, GetTypeName<Match>());
                errno = ERANGE;
                return false;
            }
            if (data < std::numeric_limits<To>::min()) {
                MERROR("value(%ld) lower than the minimal value of Your Data Type(%s).", data, GetTypeName<To>());
                errno = ERANGE;
                return false;
            }
            return true;
        }

    // for get data on different base type
    template <int TypeID, class T>
        struct TypeData {
            bool GetData(const char *str, T *address) {
                if (!address) {
                    return false;
                }
                char *end = NULL;
                errno = 0;
                long data = strtol(str, &end, 10);
                if (errno == ERANGE){
                    MERROR("range error, got %ld", data);
                    errno = 0;
                    return false;
                }
                bool ret = CheckType<T, int, long>(data);
                if (ret) {
                    *address = data;
                }
                return ret;
            }
        };

    template <class T>
        struct TypeData<INT, T> {
            bool GetData(const char *str, T *address) {
                if (!address) {
                    return false;
                }
                char *end = NULL;
                errno = 0;
                long data = strtol(str, &end, 10);
                if (errno == ERANGE){
                    MERROR("range error, got %ld", data);
                    errno = 0;
                    return false;
                }
                bool ret = CheckType<T, int, long>(data);
                if (ret) {
                    *address = data;
                }
                return ret;
            }
        };

    template <class T>
        struct TypeData<LONG, T> {
            bool GetData(const char *str, T *address) {
                if (!address) {
                    return false;
                }
                char *end = NULL;
                errno = 0;
                long data = strtol(str, &end, 10);
                if (errno == ERANGE){
                    MERROR("range error, got %ld", data);
                    errno = 0;
                    return false;
                }
                bool ret = CheckType<T, long, long>(data);
                if (ret) {
                    *address = data;
                }
                return ret;
            }
        };

    template <class T>
        struct TypeData<DOUBLE, T> {
            bool GetData(const char *str, T *address) {
                if (!address) {
                    return false;
                }
                char *end = NULL;
                errno = 0;
                double data = strtod(str, &end);
                if (errno == ERANGE){
                    MERROR("range error, got %ld", data);
                    errno = 0;
                    return false;
                }
                bool ret = CheckType<T, long, long>(data);
                if (ret) {
                    *address = data;
                }
                return ret;
            }
        };

    template <class T>
        struct TypeData<STRING, T> {
            bool GetData(const char *str, T *address) {
                if (!address) {
                    return false;
                }
                if (is_same<std::string, T>::value) {
                    address->assign(str);
                    return true;
                }
                MERROR("Only Support std::string for String Type. Current Type(%s)", GetTypeName<T>());
                return false;
            }
        };

    const static std::string types[6] = {
        "Zero", "Int", "Long", "Double", "String", "Other"
    };

    // AutoXML Class Define
    class AutoXML
    {
    public:
        // Load File, Read Root Node
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

        // Bind variable to a path
        template<class T>
            bool BindXML(T *address, const char* cur_file, size_t cur_line, size_t cnt, ...)
            {
                SetFileLine(cur_file, cur_line);
                bool ret = PointerCheck(*address);
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
        // Get Data From XML Node
        template<class T>
            bool GetData(T *address, const char* data)
            {
                if (!address || !data) {
                    AUTOXML_MERROR("pointer is NULL");
                    return false;
                }
                const int type_id = best_match<T>::value;
                AUTOXML_MDEBUG("Best Match ID Is:(%d: %s)", type_id, types[type_id].c_str());
                TypeData<type_id, T> typedata;
                return typedata.GetData(data, address);
            }

        //Modifier
        void SetFileLine(const char *cur_file, size_t cur_line)
        {
            m_strCurFile = cur_file;
            m_sizeCurLine = cur_line;
        }

        template<class T>
            bool PointerCheck(T)
            {
                if (is_pointer<T>::value) {
                    MERROR("address is a pointer, error.");
                    return false;
                } return true;
            }

    private:
        TiXmlDocument m_stDoc;
        TiXmlElement *m_pRoot;
        const char* m_strCurFile;
        size_t m_sizeCurLine;
    };
}

