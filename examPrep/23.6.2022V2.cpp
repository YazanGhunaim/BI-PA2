#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stack>
#include <queue>
#include <deque>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <algorithm>
#include <memory>
#include <cassert>
using namespace std;

class CDelivery
{
public:
    CDelivery() = default;
    ~CDelivery() = default;
    CDelivery &addConn(const string &a, const string &b)
    {
        m_Connections[a].insert(b);
        m_Connections[b].insert(a);
        return *this;
    }

    vector<int> findCoverage(const set<string> &depots) const
    {
        vector<int> result;
        set<string> visited;
        queue<string> queue;

        int start = 0;
        int next = depots.size();

        for (const auto &i : depots)
        {
            if (m_Connections.find(i) == m_Connections.end())
                throw invalid_argument("");
            queue.push(i);
            visited.insert(i);
        }

        while (!queue.empty())
        {
            if (start == 0)
            {
                if (result.empty())
                    result.push_back(next);
                else
                    result.push_back(next + result.back());
                start = next;
                next = 0;
            }

            for (const auto &x : m_Connections.at(queue.front()))
            {
                if (visited.count(x) != 0)
                    continue;
                queue.push(x);
                visited.insert(x);
                ++next;
            }
            --start;
            queue.pop();
        }
        return result;
    }

private:
    map<string, set<string>> m_Connections;

    friend ostream &operator<<(ostream &os, const CDelivery &src)
    {
        for (const auto &[stop, cnxns] : src.m_Connections)
        {
            os << stop << " => ";
            for (const auto &x : cnxns)
                os << x << " ";
            os << endl;
        }
        return os;
    }
};

int main()
{
    CDelivery t0;
    vector<int> r;
    t0
        .addConn("Austin", "Berlin")
        .addConn("Chicago", "Berlin")
        .addConn("Chicago", "Dallas")
        .addConn("Dallas", "Essen")
        .addConn("Essen", "Austin")
        .addConn("Frankfurt", "Essen")
        .addConn("Gyor", "Frankfurt")
        .addConn("Helsinki", "Istanbul")
        .addConn("Istanbul", "Jakarta");

    r = t0.findCoverage(set<string>{"Berlin"});
    assert(r == (vector<int>{1, 3, 5, 6, 7}));
    /* result:
     * [0]: Berlin = 1
     * [1]: Austin, Berlin, Chicago = 3
     * [2]: Austin, Berlin, Chicago, Dallas, Essen = 5
     * [3]: Austin, Berlin, Chicago, Dallas, Essen, Frankfurt = 6
     * [4]: Austin, Berlin, Chicago, Dallas, Essen, Frankfurt, Gyor = 7
     */

    r = t0.findCoverage(set<string>{"Berlin", "Essen"});
    assert(r == (vector<int>{2, 6, 7}));

    r = t0.findCoverage(set<string>{"Helsinki"});
    assert(r == (vector<int>{1, 2, 3}));

    r = t0.findCoverage(set<string>{"Istanbul"});
    assert(r == (vector<int>{1, 3}));

    r = t0.findCoverage(set<string>{"Austin", "Jakarta"});
    assert(r == (vector<int>{2, 5, 9, 10}));

    r = t0.findCoverage(set<string>{"Chicago", "Gyor", "Helsinki", "Jakarta"});
    assert(r == (vector<int>{4, 8, 10}));

    try
    {
        r = t0.findCoverage(set<string>{"Incorrect city"});
        assert("No invalid_argument exception caught!" == nullptr);
    }
    catch (const invalid_argument &e)
    {
    }

    return EXIT_SUCCESS;
}