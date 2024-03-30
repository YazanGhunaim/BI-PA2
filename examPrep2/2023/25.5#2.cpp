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
class CArray
{
private:
    T *m_data;
    size_t m_size;

public:
    CArray() : m_data(nullptr), m_size(0) {}

    explicit CArray(size_t size)
        : m_size(size)
    {
        m_data = new T[m_size];
    }

    explicit CArray(const CArray<T> &src)
    {
        m_data = new T[m_size = src.m_size];
        copy(src.m_data, src.m_data + src.m_size, m_data);
    }

    CArray<T> &operator=(const CArray<T> &other)
    {
        if (this != &other)
        {
            delete[] m_data;
            m_data = new T[m_size = other.m_size];
            copy(other.m_data, other.m_data + other.m_size, m_data);
        }
        return *this;
    }

    T operator[](size_t index) const
    {
        if (index >= m_size)
            throw std::out_of_range("Index out of range.");
        return m_data[index];
    }

    T &operator[](size_t index)
    {
        if (index >= m_size)
            throw std::out_of_range("Index out of range.");
        return m_data[index];
    }

    ~CArray()
    {
        delete[] m_data;
    }
};
template <typename T>
class CMatrix3
{
private:
    CArray<CArray<CArray<T>>> m_data;

public:
    CMatrix3(unsigned n1, unsigned n2, unsigned n3)
        : m_data(n1)
    {
        for (size_t i = 0; i < n1; ++i)
        {
            m_data[i] = CArray<CArray<T>>(n2);
            for (size_t j = 0; j < n2; ++j)
                m_data[i][j] = CArray<T>(n3);
        }
    }

    ~CMatrix3() = default;

    CArray<CArray<T>> &operator[](size_t index)
    {
        return m_data[index];
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