#ifndef __PROGTEST__
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <string>
#include <iostream>
#include <stdexcept>
#include <list>
#include <vector>
#include <algorithm>
using namespace std;
#endif /* __PROGTEST__ */

using coords = pair<size_t, size_t>;

template <typename T>
class CSparseMatrix
{
private:
    vector<T> m_data;
    vector<coords> m_coordinates;

    size_t m_rows;
    size_t m_cols;

    T m_default;

public:
    CSparseMatrix(size_t rows, size_t cols)
        : m_rows(rows), m_cols(cols), m_default() {}

    ~CSparseMatrix() {}

    const T &operator()(size_t row, size_t col) const
    {
        if (row >= m_rows || col >= m_cols)
            throw out_of_range("Index error");

        auto coordinate = make_pair(row, col);
        auto it = std::lower_bound(m_coordinates.begin(), m_coordinates.end(), coordinate);

        if (it == m_coordinates.end())
            return m_default;

        // coord doesnt exist
        size_t index = it - m_coordinates.begin();
        return m_data[index];
    }

    T &operator()(size_t row, size_t col)
    {
        if (row >= m_rows || col >= m_cols)
            throw out_of_range("Index error");

        auto coordinate = make_pair(row, col);
        auto it = std::lower_bound(m_coordinates.begin(), m_coordinates.end(), coordinate);

        // if coord exists
        if (it != m_coordinates.end() && *it == coordinate)
            return m_data[it - m_coordinates.begin()];

        // coord doesnt exist
        size_t index = it - m_coordinates.begin();
        m_coordinates.insert(m_coordinates.begin() + index, coordinate);
        m_data.emplace(m_data.begin() + index, T{});
        return m_data[index];
    }

    bool contains(size_t row, size_t col) const
    {
        auto coordinate = make_pair(row, col);
        auto it = std::lower_bound(m_coordinates.begin(), m_coordinates.end(), coordinate);
        return it != m_coordinates.end();
    }
};

#ifndef __PROGTEST__
int main(void)
{
    CSparseMatrix<int> m1(300, 700);

    assert(!m1.contains(0, 0));
    m1(0, 0) = 123;
    m1(1, 1) = 321;
    m1(2, 2) = m1(1, 1) + m1(0, 0);
    assert(m1.contains(0, 0) && m1(0, 0) == 123);
    assert(m1.contains(1, 1) && m1(1, 1) == 321);
    assert(m1.contains(2, 2) && m1(2, 2) == 444);

    for (int i = 0; i < 300; ++i)
        for (int j = 0; j < 300; ++j)
            m1(i, j) = i * j;

    for (int i = 0; i < 300; ++i)
        for (int j = 0; j < 300; ++j)
            assert(m1.contains(i, j) && m1(i, j) == i * j);

    try
    {
        m1(300, 300) = 666;
        assert(false);
    }
    catch (out_of_range &o)
    {
        assert(o.what() == "Index error"s);
    }
    catch (...)
    {
        assert(false);
    }

    try
    {
        m1(299, 300) = 322;
    }
    catch (...)
    {
        assert(false);
    }

    CSparseMatrix m2 = m1;

    for (int i = 0; i < 300; ++i)
        for (int j = 0; j < 300; ++j)
            assert(m2.contains(i, j) && m2(i, j) == i * j);
    m1(0, 0) = 15;

    assert(m2(0, 0) == 0);
    assert(m1(0, 0) == 15);

    CSparseMatrix<int> m3(1000, 1000);
    assert(!m3.contains(666, 666));
    m3(666, 666) = 666;
    assert(m3.contains(666, 666) && m3(666, 666) == 666);
    m3 = m1;
    assert(!m3.contains(666, 666));
    assert(m3.contains(0, 0) && m3(0, 0) == 15);

    for (int i = 1; i < 300; ++i)
        for (int j = 1; j < 300; ++j)
            assert(m3.contains(i, j) && m3(i, j) == i * j);

    CSparseMatrix<string> m4(1000000, 1000000);
    assert(!m4.contains(0, 0));
    assert(!m4.contains(999999, 999999));
    m4(0, 0) = "P";
    m4(10, 40) = "r";
    m4(55, 50000) = "o";
    m4(400000, 11) = "g";
    m4(999999, 1221) = "t";
    m4(121, 169) = "e";
    m4(66, 6) = "s";
    m4(6, 66) = "t";
    assert(m4.contains(0, 0) && m4(0, 0) == "P"s);
    assert(m4.contains(10, 40) && m4(10, 40) == "r"s);
    assert(m4.contains(55, 50000) && m4(55, 50000) == "o"s);
    assert(m4.contains(400000, 11) && m4(400000, 11) == "g"s);
    assert(m4.contains(999999, 1221) && m4(999999, 1221) == "t"s);
    assert(m4.contains(121, 169) && m4(121, 169) == "e"s);
    assert(m4.contains(66, 6) && m4(66, 6) == "s"s);
    assert(m4.contains(6, 66) && m4(6, 66) == "t"s);
    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */