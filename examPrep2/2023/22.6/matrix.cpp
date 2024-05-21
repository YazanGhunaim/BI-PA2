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

/**
 * @class CVector
 * @brief Templated class implementing a Vector.
 *
 * This class is a templated class implementing a Vector that supports
 * negative indices and provides memory management, copy semantics and
 * an accessor for its elements.
 */
template <typename T>
class CVector
{
public:
    T *m_data;
    int m_size;
    int m_offset;

    CVector()
        : m_data(nullptr), m_size(0), m_offset(0) {}

    CVector(int lowerBound, int upperBound)
        : m_size(upperBound - lowerBound + 1), m_offset(lowerBound)
    {
        if (m_size < 0)
            throw invalid_argument("Invalid Range");
        m_data = new T[m_size]{};
    }

    CVector(const CVector &src)
        : m_data(new T[src.m_size]), m_size(src.m_size), m_offset(src.m_offset)
    {
        copy(src.m_data, src.m_data + src.m_size, m_data);
    }

    CVector &operator=(CVector src)
    {
        swap(m_data, src.m_data);
        swap(m_size, src.m_size);
        swap(m_offset, src.m_offset);
        return *this;
    }

    ~CVector()
    {
        delete[] m_data;
    }

    T &operator[](int idx)
    {
        idx -= m_offset;

        if (idx >= m_size || idx < 0)
            throw out_of_range("Index out of range");

        return m_data[idx];
    }

    const T &operator[](int idx) const
    {
        idx -= m_offset;

        if (idx >= m_size || idx < 0)
            throw out_of_range("Index out of range");

        return m_data[idx];
    }
};

template <typename T>
class CMatrix2
{
private:
    CVector<CVector<T>> m_matrix;
    int m_lo1, m_hi1;
    int m_lo2, m_hi2;

public:
    CMatrix2(int n1, int n2, int n3, int n4)
        : m_matrix(n1, n2), m_lo1(n1), m_hi1(n2), m_lo2(n3), m_hi2(n4)
    {
        for (int i = n1; i <= n2; ++i)
        {
            m_matrix[i] = CVector<T>(n3, n4);
        }
    }

    ~CMatrix2() = default;

    CVector<T> &operator[](int idx)
    {
        return m_matrix[idx];
    }

    bool operator==(const CMatrix2 &other) const
    {
        if (m_matrix.m_size != other.m_matrix.m_size)
            return false;
        if (m_matrix[0].m_size != other.m_matrix[0].m_size)
            return false;

        for (int i = m_lo1; i < m_hi1; ++i)
        {
            for (int j = m_lo2; j < m_lo2; ++j)
            {
                if (m_matrix[i][j] != other.m_matrix[i][j])
                    return false;
            }
        }

        return true;
    }
};

#ifndef __PROGTEST__
int main()
{

    // Integer test
    CMatrix2<int> m1(-2, 2, -2, 2);
    assert((m1[-2][-2] = 5) == 5);
    assert((m1[2][2] = 10) == 10);

    try
    {
        int x = m1[3][3];
    }
    catch (const std ::out_of_range &e)
    {
        std ::cout << e.what() << std ::endl;
    }

    // Double test
    CMatrix2<double> m2(-2, 2, -2, 2);
    assert((m2[-2][-2] = 5.5) == 5.5);
    assert((m2[2][2] = 10.5) == 10.5);

    try
    {
        double x = m2[3][3];
    }
    catch (const std ::out_of_range &e)
    {
        std ::cout << e.what() << std ::endl;
    }

    // String test
    CMatrix2<std ::string> m3(-2, 2, -2, 2);
    assert((m3[-2][-2] = "test1") == "test1");
    assert((m3[2][2] = "test2") == "test2");

    try
    {
        std ::string x = m3[3][3];
    }
    catch (const std ::out_of_range &e)
    {
        std ::cout << e.what() << std ::endl;
    }

    // Test copy constructor and equality operator
    CMatrix2<int> m4(-2, 2, -2, 2);
    m4[-2][-2] = 5;
    m4[2][2] = 10;
    CMatrix2<int> m5(m4);
    assert(m4 == m5);

    // Test assignment operator and equality operator
    CMatrix2<int> m6(-2, 2, -2, 2);
    m6[-2][-2] = 5;
    m6[2][2] = 10;
    CMatrix2<int> m7 = m6;
    assert(m6 == m7);

    return 0;
}
#endif /* __PROGTEST__ */