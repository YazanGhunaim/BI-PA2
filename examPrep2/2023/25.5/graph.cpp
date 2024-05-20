#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <list>
#include <vector>
#include <stack>
#include <queue>
#include <deque>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <memory>
using namespace std;
#endif /* __PROGTEST__ */

class CTeleport
{
private:
    struct CConnection
    {
        string m_to;
        unsigned m_departure;
        unsigned m_arrival;

        CConnection() {}
        ~CConnection() {}

        CConnection(string to, unsigned departure, unsigned arrival)
            : m_to(to), m_departure(departure), m_arrival(arrival) {}

        bool operator<(const CConnection &rhs) const
        {
            return tie(m_to, m_departure, m_arrival) < tie(rhs.m_to, rhs.m_departure, rhs.m_arrival);
        }
    };

    struct CSchedule
    {
        set<CConnection> m_connections;

        CSchedule() {}

        ~CSchedule() {}
    };

    map<string, CSchedule> m_graph;

public:
    CTeleport() {}

    ~CTeleport() {}

    CTeleport &Add(const string &from, const string &to, unsigned fromTime, unsigned toTime)
    {
        m_graph[from].m_connections.insert({to, fromTime, toTime});
        return *this;
    }

    CTeleport &Optimize(void)
    {
        return *this;
    }

    unsigned FindWay(const string &from, const string &to, unsigned time)
    {
        if (from == to)
            return 0;

        queue<pair<string, unsigned>> q;
        map<string, unsigned> bestArrivals;

        q.emplace(from, time);
        bestArrivals.insert({from, time});

        while (!q.empty())
        {
            auto currentlyAt = q.front().first;
            auto currentTime = q.front().second;
            q.pop();

            if (!m_graph.contains(currentlyAt))
                continue;

            for (const auto &connection : m_graph.at(currentlyAt).m_connections)
            {
                if (connection.m_departure >= currentTime)
                {
                    // we dont have time or better time
                    if (!bestArrivals.contains(connection.m_to) || connection.m_arrival < bestArrivals.at(connection.m_to))
                    {
                        bestArrivals[connection.m_to] = connection.m_arrival;
                        q.emplace(connection.m_to, connection.m_arrival);
                    }
                }
            }
        }

        if (!bestArrivals.contains(to))
            throw invalid_argument("No connection was provided");
        return bestArrivals[to];
    }
};

#ifndef __PROGTEST__
int main(void)
{
    CTeleport t;
    t.Add("Prague", "Vienna", 0, 7)
        .Add("Vienna", "Berlin", 9, 260)
        .Add("Vienna", "London", 8, 120)
        .Add("Vienna", "Chicago", 4, 3)
        .Add("Prague", "Vienna", 10, 10)
        .Optimize();

    assert(t.FindWay("Prague", "Vienna", 0) == 7);
    assert(t.FindWay("Prague", "Vienna", 1) == 10);
    assert(t.FindWay("Prague", "London", 0) == 120);
    assert(t.FindWay("Vienna", "Chicago", 4) == 3);

    try
    {
        t.FindWay("Prague", "London", 2);
        assert("Missing exception" == nullptr);
    }
    catch (const invalid_argument &e)
    {
    }
    catch (...)
    {
        assert("Invalid exception" == nullptr);
    }
    try
    {
        t.FindWay("Prague", "Chicago", 0);
        assert("Missing exception" == nullptr);
    }
    catch (const invalid_argument &e)
    {
    }
    catch (...)
    {
        assert("Invalid exception" == nullptr);
    }

    t.Add("Dallas", "Atlanta", 150, 30)
        .Add("Berlin", "Helsinki", 1080, 2560)
        .Add("Chicago", "Frankfurt", 50, 0)
        .Add("Helsinki", "Vienna", 3200, 3)
        .Add("Chicago", "London", 10, 12)
        .Add("London", "Atlanta", 20, 40)
        .Add("Vienna", "Atlanta", 10, 50)
        .Add("Prague", "Vienna", 1, 6)
        .Add("Berlin", "Helsinki", 265, 265)
        .Add("Berlin", "London", 259, 0)
        .Optimize();

    assert(t.FindWay("Prague", "Frankfurt", 0) == 0);
    assert(t.FindWay("Prague", "Atlanta", 0) == 40);
    assert(t.FindWay("Prague", "Atlanta", 10) == 50);

    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */