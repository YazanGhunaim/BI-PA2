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
    T *m_data;
    size_t m_size;

public:
    explicit Array(size_t size)
    {
        m_size = size;
        m_data = new T[size];
    }

    Array()
        : m_data(nullptr), m_size(0) {}

    Array(const Array<T> &src)
    {
        m_data = new T[m_size = src.size()];
        std::copy(src.m_data, src.m_data + src.m_size, m_data);
    }
    ~Array()
    {
        delete[] m_data;
    }

    size_t size() const
    {
        return m_size;
    }

    T &operator[](int idx)
    {
        if (idx >= (int)m_size)
            throw std::out_of_range("");
        return m_data[idx];
    }

    Array<T> &operator=(const Array<T> &src)
    {
        if (&src == this)
            return *this;
        delete[] m_data;
        m_data = new T[m_size = src.m_size];
        std::copy(src.m_data, src.m_data + m_size, m_data);
        return *this;
    }
};

template <typename T_>
class CMatrix3
{
private:
    Array<Array<Array<T_>>> m_Data3D;

public:
    CMatrix3(unsigned n1, unsigned n2, unsigned n3)
        : m_Data3D(n1)
    {
        for (size_t i = 0; i < n1; ++i)
        {
            m_Data3D[i] = Array<Array<T_>>(n2);
            for (size_t j = 0; j < n2; ++j)
                m_Data3D[i][j] = Array<T_>(n3);
        }
    }
    ~CMatrix3() = default;

    Array<Array<T_>> &operator[](unsigned id)
    {
        return m_Data3D[id];
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