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
    // connection details of teleport
    struct Connection
    {
        std::string m_to;
        unsigned m_departure;
        unsigned m_arrival;

        Connection() = default;
        ~Connection() = default;

        Connection(const std::string &to, unsigned dep, unsigned arr)
            : m_to(std::move(to)), m_departure(dep), m_arrival(arr)
        {
        }

        bool operator<(const Connection &rhs) const
        {
            return std::tie(m_to, m_departure, m_arrival) < std::tie(rhs.m_to, rhs.m_departure, rhs.m_arrival);
        }

        void print(std::ostream &os) const
        {
            os << "to :" << m_to << " dep :" << m_departure << " arrival :" << m_arrival;
        }
    };

    // set of connections
    struct Schedule
    {
        std::set<Connection> m_connections;

        Schedule() = default;
        ~Schedule() = default;

        Schedule &insert(const Connection &other)
        {
            m_connections.emplace(std::move(other));
            return *this;
        }

        std::set<Connection> &getSchedule()
        {
            return m_connections;
        }
    };

    // city and corresponding teleport schedule
    std::unordered_map<std::string, Schedule> m_teleports;

public:
    CTeleport() = default;
    ~CTeleport() = default;

    CTeleport &Add(const string &from, const string &to, unsigned fromTime, unsigned toTime)
    {
        m_teleports.emplace(std::move(from), Schedule());
        auto it = m_teleports.find(from);
        (it->second).insert(Connection(to, fromTime, toTime));
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

        // storing best arrival time for each city
        std::map<std::string, unsigned> best_arrivals;
        std::queue<std::pair<std::string, unsigned>> queue;

        queue.push(std::make_pair(from, time));
        best_arrivals.emplace(from, time);

        while (!queue.empty())
        {
            auto depCity{std::move(queue.front())};
            queue.pop();

            std::string cityName = depCity.first;
            unsigned actualTime = depCity.second;

            for (const auto &connection : m_teleports[cityName].getSchedule())
            {
                // check if we can travel
                if (actualTime <= connection.m_departure)
                {
                    auto check_arrival = best_arrivals.find(connection.m_to);
                    if (check_arrival == best_arrivals.end())
                    {
                        best_arrivals.insert({connection.m_to, connection.m_arrival});
                    }
                    
                    if (check_arrival->second > connection.m_arrival)
                    {
                        queue.push(make_pair(connection.m_to, connection.m_arrival));
                        best_arrivals[connection.m_to] = connection.m_arrival;
                    }
                }
            }
        }

        auto res = best_arrivals.find(to);
        if (res == best_arrivals.end())
            throw invalid_argument("No connection was provided.");
        return res->second;
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