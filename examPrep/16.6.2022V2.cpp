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
        m_connections.emplace(std::move(to), std::move(from));
        return *this;
    }

    map<string, list<string>> serveCustomers(const set<string> &customers, const set<string> &depots) const
    {
        map<string, list<string>> result;

        for (const auto &customer : customers)
            result.emplace(customer, bfs(customer, depots));

        return result;
    }

private:
    multimap<string, string> m_connections;
    list<string> bfs(const string &customer, const set<string> &depots) const
    {
        queue<string> queue({customer});
        map<string, string> arrivalsNav;

        while (!queue.empty())
        {
            string stop({std::move(queue.front())});
            queue.pop();

            auto it = depots.find(stop);
            if (it != depots.end())
            {
                list<string> result{stop};
                while (result.back() != customer)
                {
                    result.push_back(arrivalsNav.at(std::move(result.back())));
                }
                return result;
            }

            auto [lo, hi] = m_connections.equal_range(stop);
            for (auto it = lo; it != hi; ++it)
                if (arrivalsNav.emplace(it->second, stop).second)
                    queue.emplace(it->second);
        }
        return {};
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