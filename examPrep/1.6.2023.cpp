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

template <typename T_>
class CSparseMatrix
{
private:
    size_t m_Rows;
    size_t m_Cols;
    vector<pair<size_t, vector<pair<size_t, T_>>>> m_Vec;

public:
    CSparseMatrix(size_t rows, size_t cols)
        : m_Rows(rows), m_Cols(cols)
    {
    }

    T_ &operator()(size_t row, size_t col)
    {
        if (row >= m_Rows || col >= m_Cols)
            throw std::out_of_range("Index error");

        auto it1 = lower_bound(m_Vec.begin(), m_Vec.end(),
                               make_pair(row, vector<pair<size_t, T_>>{}),
                               [](const pair<size_t, vector<pair<size_t, T_>>> &a,
                                  const pair<size_t, vector<pair<size_t, T_>>> &b)
                               {
                                   return a.first < b.first;
                               });

        if (it1 == m_Vec.end() || (*it1).first != row)
            m_Vec.insert(it1, make_pair(row, vector<pair<size_t, T_>>{}));

        auto it2 = lower_bound(m_Vec.begin(), m_Vec.end(),
                               make_pair(row, vector<pair<size_t, T_>>{}),
                               [](const pair<size_t, vector<pair<size_t, T_>>> &a,
                                  const pair<size_t, vector<pair<size_t, T_>>> &b)
                               {
                                   return a.first < b.first;
                               });

        auto it3 = lower_bound((*it2).second.begin(), (*it2).second.end(),
                               make_pair(col, T_()),
                               [](const pair<size_t, T_> &a,
                                  const pair<size_t, T_> &b)
                               {
                                   return a.first < b.first;
                               });

        if (it3 == (*it2).second.end() || (*it3).first != col)
            (*it2).second.insert(it3, make_pair(col, T_()));

        auto it4 = lower_bound((*it2).second.begin(), (*it2).second.end(),
                               make_pair(col, T_()),
                               [](const pair<size_t, T_> &a,
                                  const pair<size_t, T_> &b)
                               {
                                   return a.first < b.first;
                               });

        return (*it4).second;
    }

    bool contains(size_t row,
                  size_t col)
    {
        //! Using lower_bound to find a row we need
        auto it1 = lower_bound(m_Vec.begin(), m_Vec.end(),
                               make_pair(row, vector<pair<size_t, T_>>{}),
                               [](const pair<size_t, vector<pair<size_t, T_>>> &a,
                                  const pair<size_t, vector<pair<size_t, T_>>> &b)
                               {
                                   return a.first < b.first;
                               });

        //! If there is ho such row return false
        if (it1 == m_Vec.end() || (*it1).first != row)
            return false;

        //! Using lower_bound to find a column we need
        auto it2 = lower_bound((*it1).second.begin(), (*it1).second.end(),
                               make_pair(col, T_()),
                               [](const pair<size_t, T_> &a,
                                  const pair<size_t, T_> &b)
                               {
                                   return a.first < b.first;
                               });

        //! If there is ho such column return false
        if (it2 == (*it1).second.end() || (*it2).first != col)
            return false;

        //! If we are here, there is an element of this poisition so return true
        return true;
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