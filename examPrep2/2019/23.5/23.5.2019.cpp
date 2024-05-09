#include <sstream>
#include <string>
#include <algorithm>
#include <map>
#include <set>
#include <list>
#include <deque>
#include <queue>
#include <stack>
#include <cassert>
#include <iostream>
using namespace std;

class CMHD
{
private:
    map<string, set<string>> m_graph;

public:
    void Add(istringstream &is)
    {
        set<string> line;
        string stop;
        while (getline(is, stop))
            line.insert(stop);

        for (const auto &station : line)
            for (const auto &stop : line)
                m_graph[station].insert(stop);
    }

    set<string> Dest(const string &from, int maxCost) const
    {
        queue<pair<string, int>> queue;
        set<string> result({from});

        if (!m_graph.contains(from))
            return result;

        queue.push(make_pair(from, 0));

        while (!queue.empty())
        {
            auto station = queue.front().first;
            auto cost = queue.front().second;
            queue.pop();

            if (cost > maxCost)
                continue;

            for (const auto &neighbor : m_graph.at(station))
            {
                if (!result.contains(neighbor))
                {
                    queue.emplace(neighbor, cost + 1);
                    result.insert(neighbor);
                }
            }
        }

        return result;
    }

    friend ostream &operator<<(ostream &os, const CMHD &src)
    {
        for (const auto &[from, toSet] : src.m_graph)
        {
            os << from << " : ";
            for (const auto &to : toSet)
                os << to << ", ";
            os << endl;
        }

        return os;
    }
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

    cout << city;

    assert(city.Dest("S", 0) == set<string>({"S", "N", "R", "Q", "P"}));

    assert(city.Dest("N", 0) == set<string>({"S", "N", "R", "Q", "P",
                                             "O", "M", "L",
                                             "K", "J", "I", "G", "F"}));

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

    // speed test
    CMHD circleCity;
    iss.clear();
    iss.str("A\nB\nC\n");
    circleCity.Add(iss);

    iss.clear();
    iss.str("C\nD\nA\n");
    circleCity.Add(iss);

    assert(circleCity.Dest("A", 1000) == set<string>({"A", "B", "C", "D"}));

    cout << "All asserts passed." << endl;
    return 0;
}