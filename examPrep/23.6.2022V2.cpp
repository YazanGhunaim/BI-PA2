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
    ~CDelivery() {}
    CDelivery &addConn(const string &a, const string &b);
    vector<int> findCoverage(const set<string> &depots) const;

private:
    std::map<std::string, std::set<std::string>> m_map;
};

CDelivery &CDelivery::addConn(const string &a, const string &b)
{
    m_map[a].insert(b);
    m_map[b].insert(a);
    return *this;
}

vector<int> CDelivery::findCoverage(const set<string> &depots) const
{
    std::vector<int> result;
    std::queue<std::string> q;
    std::set<std::string> visited;
    int prev = depots.size();
    int next = 0;

    for (const auto &i : depots)
    {
        if (m_map.find(i) == m_map.end())
            throw invalid_argument("");
        q.emplace(std::move(i));
        visited.emplace(std::move(i));
    }

    while (!q.empty())
    {
        if (next == 0)
        {
            if (result.empty())
                result.push_back(prev);
            else
                result.push_back(prev + result.back());
            next = prev;
            prev = 0;
        }

        for (const auto &i : m_map.at(q.front()))
        {
            if (visited.count(i) != 0)
                continue;
            visited.emplace(std::move(i));
            q.emplace(std::move(i));
            prev++;
        }
        next--;
        q.pop();
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