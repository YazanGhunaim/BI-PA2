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
    CDelivery() {}
    ~CDelivery() {}
    CDelivery &addConn(const string &a, const string &b);
    vector<int> findCoverage(const set<string> &depots) const;

private:
    map<string, set<string>> m_graph;
};

CDelivery &CDelivery::addConn(const string &a, const string &b)
{
    m_graph[a].insert(b);
    m_graph[b].insert(a);
    return *this;
}

vector<int> CDelivery::findCoverage(const set<string> &depots) const
{
    queue<string> q;
    set<string> visited;
    vector<int> result;

    int remaining = 0;
    int remainingNext = depots.size();

    for (const auto &depot : depots)
    {
        if (!m_graph.contains(depot))
            throw invalid_argument("");

        q.emplace(depot);
        visited.insert(depot);
    }

    while (!q.empty())
    {
        string current = q.front();
        q.pop();

        if (remaining == 0)
        {
            if (result.empty())
                result.push_back(remainingNext);
            else
                result.push_back(remainingNext + result.back());

            remaining = remainingNext;
            remainingNext = 0;
        }

        visited.insert(current);
        for (const auto &conn : m_graph.at(current))
        {
            if (!visited.contains(conn))
            {
                q.emplace(conn);
                visited.insert(conn);
                ++remainingNext;
            }
        }
        --remaining;
    }

      return result;
}

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