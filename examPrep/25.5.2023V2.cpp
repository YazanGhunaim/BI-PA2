#ifndef __PROGTEST__
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <string>
#include <iostream>
#include <stdexcept>
using namespace std;
#endif /* __PROGTEST__ */

template <typename T>
class Array
{
private:
    T *m_Data;
    size_t m_Size;

public:
    Array()
        : m_Data(nullptr), m_Size(0)
    {
    }

    explicit Array(size_t size)
    {
        m_Data = new T[m_Size = size];
    }

    Array(const Array<T> &src)
    {
        m_Data = new T[m_Size = src.m_Size];
        copy(src.m_Data, src.m_Data + src.m_Size, m_Data);
    }

    ~Array()
    {
        delete[] m_Data;
    }

    T &operator[](size_t index)
    {
        if (index >= m_Size)
            throw std::out_of_range("");
        return m_Data[index];
    }

    Array<T> &operator=(const Array<T> &src)
    {
        if (&src != this)
        {
            delete[] m_Data;
            m_Data = new T[m_Size = src.m_Size];
            copy(src.m_Data, src.m_Data + src.m_Size, m_Data);
        }
        return *this;
    }
};

template <typename T>
class CMatrix3
{
    Array<Array<Array<T>>> m_Data;

public:
    CMatrix3(unsigned n1, unsigned n2, unsigned n3)
        : m_Data{n1}
    {
        for (size_t i = 0; i < n1; ++i)
        {
            m_Data[i] = Array<Array<T>>{n2};
            for (size_t j = 0; j < n2; ++j)
                m_Data[i][j] = Array<T>{n3};
        }
    }

    ~CMatrix3() = default;

    Array<Array<T>> &operator[](size_t index)
    {
        return m_Data[index];
    }
};

#ifndef __PROGTEST__
int main(void)
{
    CMatrix3<int> m(3, 4, 5);
    m[0][0][0] = 128;
    m[2][3][1] = 7;
    assert(m[0][0][0] == 128);
    assert(m[2][3][1] == 7);

    m[1][1][1] = m[0][0][0] + m[2][3][1];
    m[2][3][4] = 135;
    assert(m[1][1][1] == m[2][3][4]);

    CMatrix3<int> copy(m);
    try
    {
        copy[4][1][2];
        assert("Missing exception for access i0 = 4, limit = 2" == nullptr);
    }
    catch (out_of_range &e)
    {
    }
    catch (...)
    {
        assert("Incorrect exception thrown" == nullptr);
    }
    try
    {
        copy[1][4][2];
        assert("Missing exception for access i1 = 4, limit = 3" == nullptr);
    }
    catch (out_of_range &e)
    {
    }
    catch (...)
    {
        assert("Incorrect exception thrown" == nullptr);
    }
    try
    {
        copy[2][1][6];
        assert("Missing exception for access i2 = 6, limit = 4" == nullptr);
    }
    catch (out_of_range &e)
    {
    }
    catch (...)
    {
        assert("Incorrect exception thrown" == nullptr);
    }

    copy[2][3][1] = 42;
    assert(m[2][3][1] == 7);
    assert(copy[2][3][1] == 42);

    CMatrix3<int> n(2, 2, 2);
    n[0][1][0] = 56;
    n[1][0][1] = 42;
    m = n;
    n[0][1][0] = 0;
    assert(m[0][1][0] == 56);
    try
    {
        m[2][1][0];
        assert("Missing exception for access i0 = 2, limit = 2" == nullptr);
    }
    catch (out_of_range &e)
    {
    }
    catch (...)
    {
        assert("Incorrect exception thrown" == nullptr);
    }

    CMatrix3<string> s(2, 1, 1);
    s[0][0][0] = "Hello ";
    s[1][0][0] = "World!";
    assert((s[0][0][0] + s[1][0][0]) == string("Hello World!"));

    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */