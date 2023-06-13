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
    struct Connection
    {
        unsigned m_Departure;
        unsigned m_Arrival;
        string m_Destination;

        Connection() = default;
        ~Connection() = default;

        Connection(unsigned dep, unsigned arr, string des)
            : m_Departure(dep), m_Arrival(arr), m_Destination(std::move(des))
        {
        }

        bool operator<(const Connection &rhs) const
        {
            return tie(m_Departure, m_Arrival, m_Destination) < tie(rhs.m_Departure, rhs.m_Arrival, rhs.m_Destination);
        }
    };

    struct Schedule
    {
        set<Connection> m_Connections;

        Schedule() = default;
        ~Schedule() = default;

        set<Connection> &getConnections()
        {
            return m_Connections;
        }

        auto insert(const Connection &other)
        {
            return m_Connections.insert(std::move(other));
        }
    };

    unordered_map<string, Schedule> m_Table;

public:
    CTeleport() = default;
    ~CTeleport() = default;

    CTeleport &Add(const string &from, const string &to, unsigned fromTime, unsigned toTime)
    {
        auto it = m_Table.find(from);
        if (it == m_Table.end())
            it = m_Table.insert({from, Schedule()}).first;
        it->second.insert(Connection(fromTime, toTime, to));
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

        map<string, unsigned> bestArrivals;
        queue<pair<string, unsigned>> queue;

        bestArrivals.insert({from, time});
        queue.push(make_pair(from, time));

        while (!queue.empty())
        {
            auto departureCity = queue.front();
            queue.pop();

            string cityName = departureCity.first;
            unsigned actualTime = departureCity.second;

            if (m_Table.find(cityName) == m_Table.end())
                continue;

            for (const auto &connection : m_Table.at(cityName).getConnections())
            {
                if (actualTime > connection.m_Departure)
                    continue;

                auto it = bestArrivals.find(connection.m_Destination);
                if (it == bestArrivals.end())
                {
                    bestArrivals.insert({connection.m_Destination, connection.m_Arrival});
                    queue.push(make_pair(connection.m_Destination, connection.m_Arrival));
                }
                else if (connection.m_Arrival < it->second)
                {
                    bestArrivals[connection.m_Destination] = connection.m_Arrival;
                    queue.push(make_pair(connection.m_Destination, connection.m_Arrival));
                }
            }
        }

        auto fastest_arrival = bestArrivals.find(to);
        if (fastest_arrival == bestArrivals.end())
            throw invalid_argument("No connection was provided");
        return fastest_arrival->second;
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