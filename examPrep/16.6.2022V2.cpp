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
    std::multimap<std::string, std::string> m_map;

    [[nodiscard]] std::list<std::string> bfs(const std::string &customer, const std::set<std::string> &depots) const
    {
        std::queue<std::string> queue({customer});
        std::unordered_map<std::string, std::string> path;

        while (!queue.empty())
        {
            std::string stop = std::move(queue.front());
            queue.pop();

            auto stop_it = depots.find(stop);
            if (stop_it != depots.end())
            {
                std::list<std::string> res({stop});
                while (res.back() != customer)
                {
                    res.emplace_back(path.at(res.back()));
                }
                return res;
            }

            auto [lo, hi] = m_map.equal_range(stop);
            for (auto it = lo; it != hi; ++it)
            {
                if (path.emplace(it->second, stop).second)
                    queue.emplace(it->second);
            }
        }

        return {};
    }

public:
    CDelivery() = default;

    ~CDelivery() = default;

    CDelivery &addConn(const string &from, const string &to)
    {
        m_map.emplace(to, from);
        return *this;
    }

    map<string, list<string>> serveCustomers(const set<string> &customers, const set<string> &depots) const
    {
        std::map<std::string, std::list<std::string>> result;
        for (const auto &customer : customers)
            result.emplace(customer, bfs(customer, depots));
        return result;
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