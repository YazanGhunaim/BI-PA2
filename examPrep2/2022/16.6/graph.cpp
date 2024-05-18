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
private:
    multimap<string, string> m_graph;

    list<string> bfs(const string &customer, const set<string> &depots) const
    {
        queue<string> q({customer});
        map<string, string> paths;

        paths.emplace(customer, customer);

        while (!q.empty())
        {
            string destination = q.front();
            q.pop();

            if (depots.contains(destination))
            {
                list<string> result({destination});

                while (result.back() != customer)
                    result.push_back(paths.at(result.back()));

                return result;
            }

            auto range = m_graph.equal_range(destination);

            for (auto i = range.first; i != range.second; ++i)
            {
                if (paths.emplace(i->second, destination).second)
                    q.push(i->second);
            }
        }

        return {};
    }

public:
    CDelivery() {}

    ~CDelivery() {}

    // add connection from -> to, fluent interface
    CDelivery &addConn(const string &from, const string &to)
    {
        m_graph.emplace(to, from);
        return *this;
    }

    // returns a map containing shortest path to each customer from any of the depots
    // map key is the customer name, value is a list of nodes on the shortest path (depot -> customer)
    // if there is no path, the value is an empty list
    map<string, list<string>> serveCustomers(const set<string> &customers, const set<string> &depots) const
    {
        map<string, list<string>> result;

        for (const auto &customer : customers)
            result.emplace(customer, bfs(customer, depots));

        return result;
    }

    friend ostream &operator<<(ostream &os, const CDelivery &src)
    {
        for (const auto &[to, from] : src.m_graph)
        {
            os << "we can reach " << to << " from: " << from << endl;
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