#include <cstdio>
#include <cstdarg>
#include <string>
#include <limits>
#include <typeinfo>
#include <cerrno>
#include <sstream>
#include <vector>
#include "tinyxml.h"

//region LogDefines
// This group of macros is used for log.
// Use your log tool instead here.
#ifdef MERROR
#error MERROR is already defined.
#else
#define MERROR(format, ...) fprintf (stderr, "[%s][%s](line %d): " format "\n", __FILE__, __func__,  __LINE__, ##__VA_ARGS__)
#endif

#ifdef MDEBUG
#error MDEBUG is already defined.
#else
#ifdef DEBUG
#define MDEBUG(format, ...) fprintf (stdout, "[%s][%s](line %d): " format "\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#else
#define MDEBUG(format, ...) fprintf (stdout, "[%s][%s](line %d): " format "\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#endif
#endif

#ifdef AUTOXML_MERROR
#error AUTOXML_MERROR is already defined.
#else
#define AUTOXML_MERROR(format, ...) MERROR(format "->[%s]:line %d", ##__VA_ARGS__, m_strCurFile, m_sizeCurLine)
#endif

#ifdef AUTOXML_MDEBUG
#error AUTOXML_MDEBUG is already defined.
#else
#define AUTOXML_MDEBUG(format, ...) MDEBUG(format "->[%s]:line %d", ##__VA_ARGS__, m_strCurFile, m_sizeCurLine)
#endif
//endregion

//region ArgsNumDefines
// This group of macons is used to get the length of the parameter
// https://stackoverflow.com/questions/2308243/macro-returning-the-number-of-arguments-it-is-given-in-c
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
#define ARG_NUM_N(\
    _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#endif
#ifdef ARG_SEQ_N
#error ARG_SEQ_N is already defined.
#else
#define ARG_SEQ_N() \
    9,8,7,6,5,4,3,2,1,0
#endif
//endregion

#ifdef AUTO_XML
#error AUTO_XML is already defined.
#else
#define AUTO_XML(filename, root) AutoXML_NS::AutoXML autoxml(filename, root, __FILE__, __LINE__);
#endif

#ifdef BIND_XML
#error BIND_XML is already defined.
#else
#define BIND_XML(address, ...) \
    autoxml.BindXML(address, __FILE__, __LINE__, ARG_NUM(address, ##__VA_ARGS__), ##__VA_ARGS__);
#endif

//todo: Remove Address
#ifdef GET_ELEM
#error XML_ELEM is already defined
#else
#define GET_ELEM(address, ...) \
    autoxml.GetElemOfPath(__FILE__, __LINE__, ARG_NUM(address, ##__VA_ARGS__), ##__VA_ARGS__);
#endif

//todo: GET_ELEM_FROM_ELEM(pElem, path)

#ifdef BIND_ELEM
#error BIND_ELEM is already defined.
#else
#define BIND_ELEM(address, pElem, strAttr) \
    autoxml.GetDataFromElem(address, pElem, strAttr);
#endif

#ifdef BIND_ELEM_PATH
#error BIND_ELEM_PATH is already defined.
#else
#define BIND_ELEM_PATH(address, pElem, ...) \
    autoxml.GetDataFromElemAndPath(address, pElem, __FILE__, __LINE__, ARG_NUM(address, ##__VA_ARGS__), ##__VA_ARGS__);
#endif

namespace AutoXML_NS
{
// define type name for print
// https://stackoverflow.com/questions/1055452/c-get-name-of-type-in-template
template<typename T>
const char *GetTypeName() { return typeid(T).name(); }
// Custom Type Name Defined Here.
#define DEFINE_TYPE_NAME(type) \
    template<> inline const char *GetTypeName<type>(){return #type;}

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

DEFINE_TYPE_NAME(long double);

DEFINE_TYPE_NAME(std::string);

DEFINE_TYPE_NAME(char *);
//================================================================================

// Check whether the type is a pointer.
template<class T>
struct is_pointer
{
    static const bool value = false;
};

template<class T>
struct is_pointer<T *>
{
    static const bool value = true;
};

// Check whether the two types are the same.
template<class To, class Src>
struct is_same
{
    static const bool value = false;
};

template<class T>
struct is_same<T, T>
{
    static const bool value = true;
};

//================================================================================
// for type to id
enum TypeIDT
{
    OTHER = 1,
    INT,
    LONG,
    DOUBLE,
    STRING,
    CHAR_POINTER,
    CSTRING,
};

//for type to string
const static std::string types[] = {
    "Invalid", "Other", "Int", "Long", "Double", "String", "char *", "Char Array"
};

// for src type match base type
template<class Src>
class best_match
{
    struct OTHER
    {
        char data;
    };
    struct Int
    {
        char data[2];
    };
    struct Long
    {
        char data[3];
    };
    struct Double
    {
        char data[4];
    };
    struct String
    {
        char data[5];
    };
    struct CharPointer
    {
        char data[6];
    };

    static Int Test(int);

    static Long Test(long);

    static Double Test(double);

    static String Test(std::string);

    static CharPointer Test(const char *);

    static OTHER Test(...);

    static Src Usage();

 public:
    enum
    {
        value = sizeof(Test(Usage()))
    };
};

// for char array
// function can't return char array type so previous version can't handle char array.
template<class Src, size_t N>
class best_match<Src[N]>
{
 public:
    enum
    {
        value = CSTRING
    };
};
//================================================================================

// for type limit check
template<class To, class Match, class Cur>
bool CheckBounds(Cur data)
{
    std::ostringstream oss;
    if (data > std::numeric_limits<Match>::max()) {
        oss << "value " << data
            << " greater than the maximum value(" << std::numeric_limits<Match>::max() << ")"
            << " of MatchType(" << GetTypeName<Match>() << ").";
        MERROR("%s", oss.str().c_str());
        errno = ERANGE;
        return false;
    }
    if (data > std::numeric_limits<To>::max()) {
        oss << "value " << data
            << " greater than the maximum value(" << std::numeric_limits<To>::max() << ")"
            << " of Your Data Type(" << GetTypeName<To>() << ").";
        MERROR("%s", oss.str().c_str());
        return false;
    }
    // the minimal value defined for float, double, long double is a POSITIVE value
    // http://en.cppreference.com/w/cpp/types/climits
    Match match_min = std::numeric_limits<Match>::min();
    if (is_same<Match, double>::value) {
        match_min = -std::numeric_limits<Match>::max();
    }
    if (data < match_min) {
        oss << "value " << data
            << " lower than the minimal value(" << std::numeric_limits<Match>::min() << ")"
            << " of MatchType(" << GetTypeName<Match>() << ").";
        MERROR("%s", oss.str().c_str());
        errno = ERANGE;
        return false;
    }
    To to_min = std::numeric_limits<To>::min();
    if (is_same<To, double>::value || is_same<To, float>::value || is_same<To, long double>::value) {
        to_min = -std::numeric_limits<To>::max();
    }
    if (data < to_min) {
        oss << "value " << static_cast<To>(data)
            << " lower than the minimal value(" << std::numeric_limits<To>::min() << ")"
            << " of Your Data Type(" << GetTypeName<To>() << ").";
        MERROR("%s", oss.str().c_str());
        errno = ERANGE;
        return false;
    }
    return true;
}

// for char array type trait
template<class Type>
struct STIsCharArray
{
    enum
    {
        ARR_LEN = 1
    };
    typedef Type EleType;
};

template<class Type, size_t N>
struct STIsCharArray<Type[N]>
{
    enum
    {
        ARR_LEN = N
    };
    typedef Type *EleType;
};

template<class Type>
bool IsCharArray(Type *address)
{
    bool ret = is_same<char *, typename STIsCharArray<Type>::EleType>::value;
    MDEBUG("Type of address(%p) is %s : The extracted type is %s, ret = %d"
           , (void *) address
           , GetTypeName<Type>()
           , GetTypeName<typename STIsCharArray<Type>::EleType>()
           , ret);
    // 如果 C 数组经过萃取，两个类型会不一样
    // 而 raw pointer 在萃取之后两个类型相同
    // raw pointer 比较危险，不建议使用
    if (ret) {
        if (is_same<typename STIsCharArray<Type>::EleType, Type>::value && is_pointer<Type>::value) {
            MDEBUG(
                "Type is a char pointer, not a char array. **I Can't Guarantee Memory Security For Raw Pointer Type**");
            return false;
        } else {
            return true;
        }
    } else {
        return ret;
    }
}

template<class Type>
bool GetCharArrayLen(Type *)
{
    bool ret = is_same<char *, typename STIsCharArray<Type>::EleType>::value;
    MDEBUG("Type of address is %s : The extracted type is %s, ret = %d"
           , GetTypeName<Type>()
           , GetTypeName<typename STIsCharArray<Type>::EleType>()
           , ret);
    return ret;
}

//region TypeDataMagic
// for get data on different base type
// c++98 don't support function template partial specialization, so struct will be create
template<int TypeID, class T>
struct TypeData
{
    bool GetData(const char *str, T *address)
    {
        if (!address) {
            return false;
        }
        MERROR("Undefined TypeID!");
        return false;
    }
};

template<class T>
struct TypeData<INT, T>
{
    bool GetData(const char *str, T *address)
    {
        if (!address) {
            return false;
        }
        char *end = NULL;
        errno = 0;
        long data = strtol(str, &end, 10);
        if (errno == ERANGE) {
            MERROR("range error, got %ld", data);
            errno = 0;
            return false;
        }
        bool ret = CheckBounds<T, int, long>(data);
        if (ret) {
            *address = data;
        }
        return ret;
    }
};

template<class T>
struct TypeData<LONG, T>
{
    bool GetData(const char *str, T *address)
    {
        if (!address) {
            return false;
        }
        char *end = NULL;
        errno = 0;
        long data = strtol(str, &end, 10);
        if (errno == ERANGE) {
            MERROR("range error, got %ld", data);
            errno = 0;
            return false;
        }
        bool ret = CheckBounds<T, long, long>(data);
        if (ret) {
            *address = data;
        }
        return ret;
    }
};

template<class T>
struct TypeData<DOUBLE, T>
{
    bool GetData(const char *str, T *address)
    {
        if (!address) {
            return false;
        }
        char *end = NULL;
        errno = 0;
        double data = strtod(str, &end);
        if (errno == ERANGE) {
            MERROR("range error, got %lf", data);
            errno = 0;
            return false;
        }
        bool ret = CheckBounds<T, double, double>(data);
        if (ret) {
            *address = data;
        }
        return ret;
    }
};

template<class T>
struct TypeData<STRING, T>
{
    bool GetData(const char *str, T *address)
    {
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

template<class T>
struct TypeData<CSTRING, T>
{
    bool GetData(const char *str, T *address)
    {
        if (!address) {
            return false;
        }
        unsigned long len = STIsCharArray<T>::ARR_LEN;
        MDEBUG("Char Array Len Is %lu", len);
        memset(static_cast<void *>(address), 0, len * sizeof(char));
        memcpy(address, str, (len * sizeof(char)) - 1);
        MDEBUG("Char Array Content Is: %s", (const char *) address);
        MDEBUG("Char Array Address(%p)", (void *) address);
        return true;
    }
};
//endregion

// AutoXML Class Define
class AutoXML
{
    enum TypeCheckRet
    {
        TCR_OTHER = 0,
        TCR_POINTER = 1,
        TCR_CHAR_ARRAY = 2,
    };

 public:
    // Load File, Read Root Node
    AutoXML(const char *filename, const char *root, const char *cur_file, size_t cur_line) :
        m_stDoc(filename), m_pRoot(NULL), m_strCurFile(), m_sizeCurLine()
    {
        SetFileLine(cur_file, cur_line);
        if (m_stDoc.LoadFile()) {
            m_pRoot = m_stDoc.FirstChildElement(root);
        } else {
            AUTOXML_MERROR("Load File(%s) Error. ID:(%d), Desc(%s)", filename, m_stDoc.ErrorId(), m_stDoc.ErrorDesc());
        }
    }

    template<class T>
    bool GetDataFromElem(T *address, TiXmlElement *pElem, const char *strName = NULL)
    {
        if (!pElem) {
            AUTOXML_MERROR("pElem Is NULL.");
            return false;
        }
        if (!strName) {
            return GetData(address, pElem->GetText());
        }
        else
        {
            return GetData(address, pElem->Attribute(strName));
        }
    }

    template<class T>
    bool GetDataFromParentElem(T *address, TiXmlElement *pElem, const char *strName)
    {
        if (!pElem) {
            AUTOXML_MERROR("pElem Is NULL.");
            return false;
        }
        AUTOXML_MDEBUG("Target Name: %s", strName);
        const char *data = pElem->Attribute(strName);
        if (!data) {
            pElem = pElem->FirstChildElement(strName);
            data = pElem->GetText();
            if (!data) {
                AUTOXML_MERROR("Can't Find Target Name(%s)", strName);
                return false;
            } else {
                MDEBUG("Get Node Data is %s", data);
                return GetData(address, data);
            }
        } else {
            MDEBUG("Get Attribute Data is %s", data);
            return GetData(address, data);
        }
    }

    template<class T>
    bool GetDataFromElemAndPath(T *address
                                , TiXmlElement *pElem
                                , const char *cur_file
                                , size_t cur_line
                                , size_t cnt
                                , ...)
    {
        va_list args;
        va_start(args, cnt);
        BuildRelativePath(cnt, args);
        va_end(args);
        SetFileLine(cur_file, cur_line);
        TypeCheckRet ret = TypeCheck(address);
        if (ret == TCR_POINTER) {
            return false;
        }
        if (!m_pRoot) {
            MERROR("Load Root Element Failed.");
            return false;
        }
        TiXmlElement *pRElem = GetParentElemOfRelativePath(pElem);
        return GetDataFromParentElem(address, pRElem, m_vecRelativePath[m_vecRelativePath.size() - 1].c_str());
    }

    void BuildPath(size_t cnt, va_list args)
    {
        m_vecPath.clear();
        for (size_t i = 0; i < cnt; ++i) {
            const char *name = va_arg(args, const char*);
            m_vecPath.push_back(name);
        }
        for (size_t i = 0; i < m_vecPath.size(); ++i) {
            MDEBUG("%s", m_vecPath[i].c_str());
        }
    }

    void BuildRelativePath(size_t cnt, va_list args)
    {
        m_vecRelativePath.clear();
        for (size_t i = 0; i < cnt; ++i) {
            const char *name = va_arg(args, const char*);
            m_vecRelativePath.push_back(name);
        }
        for (size_t i = 0; i < m_vecRelativePath.size(); ++i) {
            MDEBUG("%s", m_vecRelativePath[i].c_str());
        }
    }

    TiXmlElement *GetParentElemOfPath()
    {
        if (!m_pRoot) {
            MERROR("Load Root Element Failed.");
            return NULL;
        }

        TiXmlElement *pElem = m_pRoot;
        for (size_t i = 0; i < m_vecPath.size() - 1; ++i) {
            const char *name = m_vecPath[i].c_str();
            AUTOXML_MDEBUG("Current Name: %s", name);
            // 如果是最后一个节点，就考虑找 Attr 或者 GetText
            if (i + 1 == m_vecPath.size()) {
                return pElem;
            } else {
                // 否则就向下寻找 Element 节点
                pElem = pElem->FirstChildElement(name);
                if (!pElem) {
                    AUTOXML_MERROR("Can't Find Child Element(%s)", name);
                    return NULL;
                }
            }
        }
        return pElem;
    }

    TiXmlElement *GetParentElemOfRelativePath(TiXmlElement *pElem)
    {
        if (!m_pRoot) {
            MERROR("Load Root Element Failed.");
            return NULL;
        }

        TiXmlElement *pRElem = pElem;
        for (size_t i = 0; i < m_vecRelativePath.size() - 1; ++i) {
            const char *name = m_vecRelativePath[i].c_str();
            AUTOXML_MDEBUG("Current Name: %s", name);
            pRElem = pRElem->FirstChildElement(name);
            if (!pRElem) {
                AUTOXML_MERROR("Can't Find Child Element(%s)", name);
                return NULL;
            }
        }
        return pRElem;
    }

    TiXmlElement *GetElemOfPath(const char *cur_file, size_t cur_line, size_t cnt, ...)
    {
        va_list args;
        va_start(args, cnt);
        BuildPath(cnt, args);
        va_end(args);
        SetFileLine(cur_file, cur_line);
        if (!m_pRoot) {
            MERROR("Load Root Element Failed.");
            return NULL;
        }

        TiXmlElement *pElem = m_pRoot;
        for (size_t i = 0; i < m_vecPath.size(); ++i) {
            const char *name = m_vecPath[i].c_str();
            AUTOXML_MDEBUG("Current Name: %s", name);
            pElem = pElem->FirstChildElement(name);
            if (!pElem) {
                AUTOXML_MERROR("Can't Find Child Element(%s)", name);
                return NULL;
            }
        }
        return pElem;
    }



    // Bind variable to a path
    template<class T>
    bool BindXML(T *address, const char *cur_file, size_t cur_line, size_t cnt, ...)
    {
        va_list args;
        va_start(args, cnt);
        BuildPath(cnt, args);
        va_end(args);
        SetFileLine(cur_file, cur_line);
        TypeCheckRet ret = TypeCheck(address);
        if (ret == TCR_POINTER) {
            return false;
        }
        if (!m_pRoot) {
            MERROR("Load Root Element Failed.");
            return false;
        }

        TiXmlElement *pElem = GetParentElemOfPath();
        return GetDataFromParentElem(address, pElem, m_vecPath[m_vecPath.size() - 1].c_str());
    }

 private:
    // Get Data From XML Node
    template<class T>
    bool GetData(T *address, const char *data)
    {
        // to deal with error: explicit specialization in non-namespace scope
        // https://stackoverflow.com/questions/3052579/explicit-specialization-in-non-namespace-scope
        AUTOXML_MDEBUG("Read Data.");
        if (!address || !data) {
            MERROR("pointer is NULL");
            return false;
        }
        if (IsCharArray(address)) {
            TypeData<CSTRING, T> typedata;
            return typedata.GetData(data, address);
        } else {
            const int type_id = best_match<T>::value;
            MDEBUG("Best Match ID Is:(%d: %s)", type_id, types[type_id].c_str());
            if (type_id == OTHER) {
                MDEBUG("Type ID Is OTHER");
                return false;
            }
            TypeData<type_id, T> typedata;
            return typedata.GetData(data, address);
        }
    }

    //Modifier
    void SetFileLine(const char *cur_file, size_t cur_line)
    {
        m_strCurFile = cur_file;
        m_sizeCurLine = cur_line;
    }

    template<class T>
    bool IsPointer(T t)
    {
        AUTOXML_MDEBUG("Type T is %s", GetTypeName<T>());
        if (is_pointer<T>::value) {
            MERROR("address is a pointer, error.");
            return true;
        }
        return false;
    }

    template<class T>
    TypeCheckRet TypeCheck(T *address)
    {
        AUTOXML_MDEBUG("Type T is %s", GetTypeName<T>());
        if (IsPointer<T>(*address)) {
            if (IsCharArray(address)) {
                return TCR_CHAR_ARRAY;
            }
            return TCR_POINTER;
        } else {
            return TCR_OTHER;
        }
    }

 private:
    TiXmlDocument m_stDoc;
    TiXmlElement *m_pRoot;
    std::vector<std::string> m_vecPath;
    std::vector<std::string> m_vecRelativePath;

    const char *m_strCurFile;
    size_t m_sizeCurLine;
};
}

