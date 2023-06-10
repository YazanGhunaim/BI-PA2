#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <stack>
#include <queue>
#include <deque>
#include <algorithm>
#include <memory>
using namespace std;

class CTransport
{
public:
    CTransport() = default;
    ~CTransport() = default;

    void AddLine(istream &is)
    {
        string prev_station, station;
        getline(is, prev_station);
        while (getline(is, station))
        {
            m_Connections[prev_station].insert(station);
            m_Connections[station].insert(prev_station);
            prev_station = station;
        }
    }
    set<string> FindDest(const string &from, int costMax) const
    {
        set<string> result({from});
        if (m_Connections.count(from) == 0)
            return result;

        queue<pair<string, int>> queue;
        queue.push({from, 0});

        while (!queue.empty())
        {
            string station = queue.front().first;
            int cost = queue.front().second;
            queue.pop();

            result.insert(station);

            if (cost >= costMax)
                continue;

            for (const auto &stop : m_Connections.at(station))
            {
                if (result.count(stop) == 0)
                    queue.emplace(make_pair(stop, cost + 1));
            }
        }
        return result;
    }

private:
    map<string, set<string>> m_Connections;
};

int main()
{

    istringstream iss;
    CTransport t0;
    iss.clear();
    iss.str("Newton\n"
            "Black Hill\n"
            "Wood Side\n"
            "Green Hill\n"
            "Lakeside\n");
    t0.AddLine(iss);
    iss.clear();
    iss.str("Great Newton\n"
            "Little Burnside\n"
            "Green Hill\n"
            "Wood Side\n");
    t0.AddLine(iss);
    iss.clear();
    iss.str("Little Newton\n"
            "Little Burnside\n"
            "Castle Hill\n"
            "Newton Crossroad\n"
            "Lakeside Central\n");
    t0.AddLine(iss);
    iss.clear();
    iss.str("Tidmouth\n"
            "Gordon's Hill\n"
            "Suderry\n"
            "Knapford\n"
            "Great Waterton\n"
            "Brendam Docks\n");
    t0.AddLine(iss);
    iss.clear();
    iss.str("Sodor Ironworks\n"
            "Sodor Streamwork\n"
            "Knapford\n"
            "Maron\n");
    t0.AddLine(iss);
    iss.clear();
    iss.str("Lakeside Central\n"
            "Little Waterton\n");
    t0.AddLine(iss);
    iss.clear();
    iss.str("Waterton Central\n"
            "Waterton East\n"
            "Waterton Woods\n"
            "Little Waterton\n"
            "Waterton West\n"
            "Waterton Central\n");
    t0.AddLine(iss);

    assert(t0.FindDest("Maron", 0) == (set<string>{"Maron"}));
    assert(t0.FindDest("Maron", 1) == (set<string>{"Knapford", "Maron"}));
    assert(t0.FindDest("Maron", 2) == (set<string>{"Great Waterton", "Knapford", "Maron", "Sodor Streamwork", "Suderry"}));
    assert(t0.FindDest("Maron", 3) == (set<string>{"Brendam Docks", "Gordon's Hill", "Great Waterton", "Knapford", "Maron",
                                                   "Sodor Ironworks", "Sodor Streamwork", "Suderry"}));
    assert(t0.FindDest("Maron", 4) == (set<string>{"Brendam Docks", "Gordon's Hill", "Great Waterton", "Knapford", "Maron",
                                                   "Sodor Ironworks", "Sodor Streamwork", "Suderry", "Tidmouth"}));
    assert(t0.FindDest("Maron", 5) == (set<string>{"Brendam Docks", "Gordon's Hill", "Great Waterton", "Knapford", "Maron",
                                                   "Sodor Ironworks", "Sodor Streamwork", "Suderry", "Tidmouth"}));

    assert(t0.FindDest("Waterton East", 0) == (set<string>{"Waterton East"}));
    assert(t0.FindDest("Waterton East", 1) == (set<string>{"Waterton Central", "Waterton East", "Waterton Woods"}));
    assert(t0.FindDest("Waterton East", 2) == (set<string>{"Little Waterton", "Waterton Central", "Waterton East",
                                                           "Waterton West", "Waterton Woods"}));
    assert(t0.FindDest("Waterton East", 3) == (set<string>{"Lakeside Central", "Little Waterton", "Waterton Central",
                                                           "Waterton East", "Waterton West", "Waterton Woods"}));
    assert(t0.FindDest("Waterton East", 4) == (set<string>{"Lakeside Central", "Little Waterton", "Newton Crossroad",
                                                           "Waterton Central", "Waterton East", "Waterton West",
                                                           "Waterton Woods"}));
    assert(t0.FindDest("Waterton East", 5) == (set<string>{"Castle Hill", "Lakeside Central", "Little Waterton", "Newton Crossroad",
                                                           "Waterton Central", "Waterton East", "Waterton West", "Waterton Woods"}));
    assert(t0.FindDest("Waterton East", 6) == (set<string>{"Castle Hill", "Lakeside Central", "Little Burnside", "Little Waterton",
                                                           "Newton Crossroad", "Waterton Central", "Waterton East", "Waterton West",
                                                           "Waterton Woods"}));
    assert(t0.FindDest("Waterton East", 9) == (set<string>{"Black Hill", "Castle Hill", "Great Newton", "Green Hill", "Lakeside",
                                                           "Lakeside Central", "Little Burnside", "Little Newton", "Little Waterton", "Newton Crossroad",
                                                           "Waterton Central", "Waterton East", "Waterton West", "Waterton Woods", "Wood Side"}));
    assert(t0.FindDest("Waterton East", 10) == (set<string>{"Black Hill", "Castle Hill", "Great Newton", "Green Hill", "Lakeside",
                                                            "Lakeside Central", "Little Burnside", "Little Newton", "Little Waterton",
                                                            "Newton", "Newton Crossroad", "Waterton Central", "Waterton East",
                                                            "Waterton West", "Waterton Woods", "Wood Side"}));
    assert(t0.FindDest("Waterton East", 11) == (set<string>{"Black Hill", "Castle Hill", "Great Newton", "Green Hill",
                                                            "Lakeside", "Lakeside Central", "Little Burnside",
                                                            "Little Newton", "Little Waterton", "Newton",
                                                            "Newton Crossroad", "Waterton Central", "Waterton East",
                                                            "Waterton West", "Waterton Woods", "Wood Side"}));

    iss.clear();
    iss.str("Newton\n"
            "Newton Crossroad\n"
            "Tidmouth\n"
            "Brendam Docks\n");
    t0.AddLine(iss);

    assert(t0.FindDest("Waterton East", 0) == (set<string>{"Waterton East"}));
    assert(t0.FindDest("Waterton East", 1) == (set<string>{"Waterton Central", "Waterton East", "Waterton Woods"}));
    assert(t0.FindDest("Waterton East", 2) == (set<string>{"Little Waterton", "Waterton Central", "Waterton East",
                                                           "Waterton West", "Waterton Woods"}));
    assert(t0.FindDest("Waterton East", 3) == (set<string>{"Lakeside Central", "Little Waterton", "Waterton Central",
                                                           "Waterton East", "Waterton West", "Waterton Woods"}));
    assert(t0.FindDest("Waterton East", 4) == (set<string>{"Lakeside Central", "Little Waterton", "Newton Crossroad",
                                                           "Waterton Central", "Waterton East", "Waterton West",
                                                           "Waterton Woods"}));
    assert(t0.FindDest("Waterton East", 5) == (set<string>{"Castle Hill", "Lakeside Central", "Little Waterton", "Newton", "Newton Crossroad", "Tidmouth",
                                                           "Waterton Central", "Waterton East", "Waterton West", "Waterton Woods"}));
    assert(t0.FindDest("Waterton East", 6) == (set<string>{"Black Hill", "Brendam Docks", "Castle Hill", "Gordon's Hill", "Lakeside Central", "Little Burnside", "Little Waterton",
                                                           "Newton", "Newton Crossroad", "Tidmouth", "Waterton Central", "Waterton East", "Waterton West",
                                                           "Waterton Woods"}));
    assert(t0.FindDest("Waterton East", 9) == (set<string>{"Black Hill", "Brendam Docks", "Castle Hill", "Gordon's Hill", "Great Newton", "Great Waterton", "Green Hill", "Knapford", "Lakeside",
                                                           "Lakeside Central", "Little Burnside", "Little Newton", "Little Waterton", "Maron", "Newton", "Newton Crossroad",
                                                           "Sodor Streamwork", "Suderry", "Tidmouth", "Waterton Central", "Waterton East", "Waterton West", "Waterton Woods", "Wood Side"}));
    assert(t0.FindDest("Waterton East", 10) == (set<string>{"Black Hill", "Brendam Docks", "Castle Hill", "Gordon's Hill", "Great Newton", "Great Waterton", "Green Hill", "Knapford", "Lakeside",
                                                            "Lakeside Central", "Little Burnside", "Little Newton", "Little Waterton", "Maron",
                                                            "Newton", "Newton Crossroad", "Sodor Ironworks", "Sodor Streamwork", "Suderry", "Tidmouth", "Waterton Central", "Waterton East",
                                                            "Waterton West", "Waterton Woods", "Wood Side"}));
    set<string> test = set<string>{"Black Hill", "Brendam Docks", "Castle Hill", "Gordon's Hill", "Great Newton", "Great Waterton", "Green Hill", "Knapford", "Lakeside",
                                   "Lakeside Central", "Little Burnside", "Little Newton", "Little Waterton", "Maron",
                                   "Newton", "Newton Crossroad", "Sodor Ironworks", "Sodor Streamwork", "Suderry", "Tidmouth", "Waterton Central", "Waterton East",
                                   "Waterton West", "Waterton Woods", "Wood Side"};
    set<string> test2 = t0.FindDest("Waterton East", 11);
    assert(test2 == test);
    assert(t0.FindDest("Waterton East", 1000000) == (set<string>{"Black Hill", "Brendam Docks", "Castle Hill", "Gordon's Hill", "Great Newton", "Great Waterton", "Green Hill", "Knapford", "Lakeside",
                                                                 "Lakeside Central", "Little Burnside", "Little Newton", "Little Waterton", "Maron",
                                                                 "Newton", "Newton Crossroad", "Sodor Ironworks", "Sodor Streamwork", "Suderry", "Tidmouth", "Waterton Central", "Waterton East",
                                                                 "Waterton West", "Waterton Woods", "Wood Side"}));

    assert(t0.FindDest("Suderry Woodside", 0) == (set<string>{"Suderry Woodside"}));
    assert(t0.FindDest("Suderry Woodside", 666) == (set<string>{"Suderry Woodside"}));

    return 0;
}