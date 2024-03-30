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
        Connection() = default;
        ~Connection() = default;

        Connection(const string &to, unsigned departure, unsigned arrival)
            : m_to(to), m_departure(departure), m_arrival(arrival) {}

        bool operator<(const Connection &other) const
        {
            return tie(m_to, m_departure, m_arrival) < tie(other.m_to, other.m_departure, other.m_arrival);
        }

        string m_to;
        unsigned m_departure;
        unsigned m_arrival;
    };

    struct Schedule
    {
        Schedule() = default;
        ~Schedule() = default;

        auto insert(const Connection &other)
        {
            return m_connections.insert(std::move(other));
        }

        set<Connection> m_connections;
    };

    unordered_map<string, Schedule> m_teleports;

public:
    CTeleport() = default;
    ~CTeleport() = default;

    CTeleport &Add(const string &from, const string &to, unsigned fromTime, unsigned toTime)
    {
        auto departure_city = m_teleports.find(from);

        if (departure_city == m_teleports.end())
        {
            departure_city = m_teleports.insert({from, Schedule{}}).first;
        }

        departure_city->second.insert(Connection{to, fromTime, toTime});
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

        map<string, unsigned> best_arrivals;
        queue<pair<string, unsigned>> departures;

        best_arrivals.insert({from, time});
        departures.push(make_pair(from, time));

        while (!departures.empty())
        {
            auto departure_city = departures.front();
            departures.pop();

            string city_name = departure_city.first;
            unsigned actual_time = departure_city.second;

            for (auto connection : m_teleports[city_name].m_connections)
            {
                if (connection.m_departure >= actual_time)
                {
                    auto check_arrival = best_arrivals.find(connection.m_to);

                    if (check_arrival == best_arrivals.end())
                    {
                        best_arrivals.insert({connection.m_to, connection.m_arrival});
                        departures.push(make_pair(connection.m_to, connection.m_arrival));
                    }

                    else if (check_arrival->second > connection.m_arrival)
                    {
                        departures.push(make_pair(connection.m_to, connection.m_arrival));
                        best_arrivals[connection.m_to] = connection.m_arrival;
                    }
                }
            }
        }
        auto fastest_arrival = best_arrivals.find(to);
        if (fastest_arrival == best_arrivals.end())
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