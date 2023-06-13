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

    // add connection from -> to, fluent interface
    CDelivery &addConn(const string &from, const string &to)
    {
        m_Connections.emplace(to, from);
        return *this;
    }

    map<string, list<string>> serveCustomers(const set<string> &customers, const set<string> &depots) const
    {
        map<string, list<string>> result;

        for (const auto &cust : customers)
            result.emplace(cust, pathBFS(cust, depots));
        return result;
    }

private:
    multimap<string, string> m_Connections;

    list<string> pathBFS(const string &customer, const set<string> &depots) const
    {
        queue<string> queue({customer});
        unordered_map<string, string> arrivalHistory;

        while (!queue.empty())
        {
            string current = queue.front();
            queue.pop();

            auto it = depots.find(current);

            if (it != depots.end())
            {
                list<string> result({current});

                while (result.back() != customer)
                    result.emplace_back(arrivalHistory.at(result.back()));

                return result;
            }
            auto [lo, hi] = m_Connections.equal_range(current);
            for (auto it = lo; it != hi; ++it)
                if (arrivalHistory.emplace(it->second, current).second)
                    queue.emplace(it->second);
        }
        return {};
    }

    friend ostream &operator<<(ostream &os, const CDelivery &src)
    {
        for (const auto &[to, from] : src.m_Connections)
        {
            auto it = src.m_Connections.equal_range(to);
            os << "to: " << to << endl;
            for (auto itr = it.first; itr != it.second; ++itr)
                os << itr->second << endl;
        }
        return os;
    }
};

int main()
{
    CDelivery t0;
    map<string, list<string>> r;
    t0.addConn("Austin", "Berlin");
    t0.addConn("Chicago", "Berlin");
    t0.addConn("Berlin", "Dallas");
    t0.addConn("Dallas", "Essen");
    t0.addConn("Essen", "Austin");
    t0.addConn("Frankfurt", "Gyor");
    t0.addConn("Gyor", "Helsinki");
    t0.addConn("Helsinki", "Frankfurt");

    r = t0.serveCustomers(set<string>{"Berlin", "Gyor"}, set<string>{"Essen", "Helsinki"});
    assert(r == (map<string, list<string>>{{"Berlin", {"Essen", "Austin", "Berlin"}}, {"Gyor", {"Helsinki", "Frankfurt", "Gyor"}}}));

    r = t0.serveCustomers(set<string>{"Austin", "Gyor", "Chicago"}, set<string>{"Austin", "Dallas"});
    assert(r == (map<string, list<string>>{{"Austin", {"Austin"}}, {"Chicago", {}}, {"Gyor", {}}}));
    t0.addConn("Chicago", "Helsinki");

    r = t0.serveCustomers(set<string>{"Austin", "Gyor", "Chicago"}, set<string>{"Austin", "Dallas"});
    assert(r == (map<string, list<string>>{{"Austin", {"Austin"}}, {"Chicago", {}}, {"Gyor", {}}}));
    t0.addConn("Berlin", "Chicago");

    r = t0.serveCustomers(set<string>{"Austin", "Gyor", "Chicago"}, set<string>{"Austin", "Dallas"});
    assert(r == (map<string, list<string>>{{"Austin", {"Austin"}}, {"Chicago", {"Austin", "Berlin", "Chicago"}}, {"Gyor", {"Austin", "Berlin", "Chicago", "Helsinki", "Frankfurt", "Gyor"}}}));
    t0.addConn("Essen", "Frankfurt");

    r = t0.serveCustomers(set<string>{"Austin", "Gyor", "Chicago"}, set<string>{"Austin", "Dallas"});
    assert(r == (map<string, list<string>>{{"Austin", {"Austin"}}, {"Chicago", {"Austin", "Berlin", "Chicago"}}, {"Gyor", {"Dallas", "Essen", "Frankfurt", "Gyor"}}}));

    return EXIT_SUCCESS;
}