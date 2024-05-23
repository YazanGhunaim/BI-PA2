#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <memory>
#include <stdexcept>
#endif /* __PROGTEST__ */

class CPatchStr
{
private:
    char *m_Data;

    CPatchStr(const char *data, size_t len)
    {
        m_Data = new char[len + 1];
        strncpy(m_Data, data, len);
        m_Data[len] = '\0';
    }

public:
    CPatchStr()
    {
        m_Data = new char[1];
        m_Data[0] = '\0';
    }

    CPatchStr(const char *str)
    {
        size_t len = strlen(str);
        m_Data = new char[len + 1];
        strcpy(m_Data, str);
    }

    CPatchStr(const CPatchStr &other)
    {
        size_t len = strlen(other.m_Data);
        m_Data = new char[len + 1];
        strcpy(m_Data, other.m_Data);
    }

    ~CPatchStr()
    {
        delete[] m_Data;
    }

    CPatchStr &operator=(const CPatchStr &other)
    {
        if (this != &other)
        {
            delete[] m_Data;
            size_t len = strlen(other.m_Data);
            m_Data = new char[len + 1];
            strcpy(m_Data, other.m_Data);
        }
        return *this;
    }

    CPatchStr subStr(size_t from, size_t len) const
    {
        size_t totalLen = strlen(m_Data);
        if (from >= totalLen || from + len > totalLen)
            throw std::out_of_range("Invalid substring range");
        if (len == 0)
            return {};

        return CPatchStr(m_Data + from, len);
    }

    CPatchStr &append(const CPatchStr &src)
    {
        size_t srcLen = strlen(src.m_Data);
        size_t oldLen = strlen(m_Data);
        char *newData = new char[oldLen + srcLen + 1];
        strcpy(newData, m_Data);
        strcpy(newData + oldLen, src.m_Data);
        delete[] m_Data;
        m_Data = newData;
        return *this;
    }

    CPatchStr &insert(size_t pos, const CPatchStr &src)
    {
        size_t srcLen = strlen(src.m_Data);
        size_t oldLen = strlen(m_Data);
        if (pos >= oldLen)
            throw std::out_of_range("Invalid insert position");

        char *newData = new char[oldLen + srcLen + 1];
        strncpy(newData, m_Data, pos);
        strcpy(newData + pos, src.m_Data);
        strcpy(newData + pos + srcLen, m_Data + pos);
        delete[] m_Data;
        m_Data = newData;
        return *this;
    }

    CPatchStr &remove(size_t from, size_t len)
    {
        size_t totalLen = strlen(m_Data);
        if (from + len > totalLen)
            throw std::out_of_range("Invalid remove range");

        memmove(m_Data + from, m_Data + from + len, totalLen - from - len + 1);
        return *this;
    }

    char *toStr() const
    {
        size_t len = strlen(m_Data);
        char *result = new char[len + 1];
        strcpy(result, m_Data);
        return result;
    }
};

#ifndef __PROGTEST__
bool stringMatch(char *str, const char *expected)
{
    bool res = std::strcmp(str, expected) == 0;
    delete[] str;
    return res;
}

int main()
{
    using namespace std;

    char tmpStr[100];
    CPatchStr a("test");
    assert(stringMatch(a.toStr(), "test"));
    std::strncpy(tmpStr, " da", sizeof(tmpStr) - 1);
    a.append(tmpStr);
    assert(stringMatch(a.toStr(), "test da"));
    std::strncpy(tmpStr, "ta", sizeof(tmpStr) - 1);
    a.append(tmpStr);
    assert(stringMatch(a.toStr(), "test data"));
    std::strncpy(tmpStr, "foo text", sizeof(tmpStr) - 1);
    CPatchStr b(tmpStr);
    assert(stringMatch(b.toStr(), "foo text"));
    CPatchStr c(a);
    assert(stringMatch(c.toStr(), "test data"));
    CPatchStr d(a.subStr(3, 5));
    assert(stringMatch(d.toStr(), "t dat"));
    d.append(b);
    assert(stringMatch(d.toStr(), "t datfoo text"));
    d.append(b.subStr(3, 4));
    assert(stringMatch(d.toStr(), "t datfoo text tex"));
    c.append(d);
    assert(stringMatch(c.toStr(), "test datat datfoo text tex"));
    c.append(c);
    assert(stringMatch(c.toStr(), "test datat datfoo text textest datat datfoo text tex"));
    d.insert(2, c.subStr(6, 9));
    assert(stringMatch(d.toStr(), "t atat datfdatfoo text tex"));
    b = "abcdefgh";
    assert(stringMatch(b.toStr(), "abcdefgh"));
    assert(stringMatch(d.toStr(), "t atat datfdatfoo text tex"));
    assert(stringMatch(d.subStr(4, 8).toStr(), "at datfd"));
    assert(stringMatch(b.subStr(2, 6).toStr(), "cdefgh"));
    try
    {
        b.subStr(2, 7).toStr();
        assert("Exception not thrown" == nullptr);
    }
    catch (const std::out_of_range &e)
    {
    }
    catch (...)
    {
        assert("Invalid exception thrown" == nullptr);
    }
    a.remove(3, 5);
    assert(stringMatch(a.toStr(), "tesa"));
    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
