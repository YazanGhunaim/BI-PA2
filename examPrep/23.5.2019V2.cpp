#include <sstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include <set>
#include <list>
#include <deque>
#include <queue>
#include <stack>
#include <cassert>
using namespace std;

class CMHD
{
public:
    void Add(istringstream &is)
    {
        string station;
        vector<string> stationsLine;
        while (getline(is, station))
            stationsLine.push_back(station);

        for (const auto &it1 : stationsLine)
            for (const auto &it2 : stationsLine)
                m_Connections[it1].emplace(std::move(it2));
    }

    set<string> Dest(const string &from, int maxCost) const
    {
        queue<string> queue({from});
        set<string> result({from});

        if (m_Connections.find(from) == m_Connections.end())
            return result;

        while (!queue.empty() && maxCost >= 0)
        {
            string station({std::move(queue.front())});
            queue.pop();

            bool newVisit = false;
            for (const auto &x : m_Connections.at(station))
            {
                queue.emplace(std::move(x));
                if (result.emplace(std::move(x)).second)
                    newVisit = true;
            }
            if (newVisit)
                --maxCost;
        }
        return result;
    }

    friend ostream &operator<<(ostream &os, const CMHD &src)
    {
        src.print(os);
        return os;
    }

private:
    void print(ostream &os) const
    {
        for (const auto &[stop, set] : m_Connections)
        {
            os << stop << ": ";
            for (const auto &cnxn : set)
                os << cnxn << " ";
            os << endl;
        }
    }
    map<string, set<string>> m_Connections;
};

int main(void)
{
    CMHD city;
    istringstream iss;
    iss.clear();

    iss.str("A\nB\nC\nD\nE\n");
    city.Add(iss);
    iss.clear();

    iss.str("B\nC\nF\nH\n");
    city.Add(iss);
    iss.clear();

    iss.str("F\nG\nI\nJ\nK\nN\n");
    city.Add(iss);
    iss.clear();

    iss.str("H\nL\n");
    city.Add(iss);
    iss.clear();

    iss.str("L\nM\nN\nO\n");
    city.Add(iss);
    iss.clear();

    iss.str("P\nQ\nR\nN\nS");
    city.Add(iss);
    iss.clear();

    // cout << city.Dest("N", 0) << endl;
    assert(city.Dest("S", 0) == set<string>({"S", "N", "R", "Q", "P"}));

    assert(city.Dest("N", 0) == set<string>({"S", "N", "R", "Q", "P",
                                             "O", "M", "L",
                                             "K", "J", "I", "G", "F"}));
    // cout<<city.Dest("S", 1)<<endl;
    assert(city.Dest("S", 1) == set<string>({"S", "N", "R", "Q", "P",
                                             "O", "M", "L",
                                             "K", "J", "I", "G", "F"}));
    assert(city.Dest("N", 1) == set<string>({"S", "N", "R", "Q", "P",
                                             "O", "M", "L",
                                             "K", "J", "I", "G", "F",
                                             "H", "F", "C", "B"}));

    assert(city.Dest("N", 2) == set<string>({"S", "N", "R", "Q", "P",
                                             "O", "M", "L",
                                             "K", "J", "I", "G", "F",
                                             "H", "F", "C", "B",
                                             "A", "D", "E"}));

    assert(city.Dest("unknown", 0) == set<string>({"unknown"}));
    assert(city.Dest("unknown", 1) == set<string>({"unknown"}));
    assert(city.Dest("unknown", 2) == set<string>({"unknown"}));

    cout << "All asserts passed." << endl;
    return 0;
}