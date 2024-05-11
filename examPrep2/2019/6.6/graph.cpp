#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>
#include <set>
#include <vector>
#include <list>
#include <deque>
#include <queue>
#include <stack>
#include <cassert>

using namespace std;

class CTrain
{
public:
    void Add(istringstream &is);

    int Count(void) const;

    bool dfs(const string &station, set<string> &visited) const
    {
        if (visited.contains(station))
            return false;

        visited.insert(station);

        for (const auto &neighbor : m_graph.at(station))
        {
            dfs(neighbor, visited);
        }

        return true;
    }

    friend ostream &operator<<(ostream &os, const CTrain &src)
    {
        for (const auto &[station, line] : src.m_graph)
        {
            os << station << " : ";
            for (const auto &stop : line)
                os << stop << " ";
            os << endl;
        }
        return os;
    }

private:
    map<string, set<string>> m_graph;
    set<string> m_staions;
};

void CTrain::Add(istringstream &is)
{
    vector<string> line;
    string station;
    while (getline(is, station))
    {
        if (!station.empty())
        {
            m_staions.insert(station);
            line.push_back(station);
        }
    }

    for (const auto &station : line)
        for (const auto &stop : line)
            m_graph[station].insert(stop);
}

int CTrain::Count(void) const
{
    set<string> visited;
    int count = 0;
    for (const auto &station : m_staions)
    {
        if (dfs(station, visited))
            ++count;
    }
    return count;
}

int main()
{
    CTrain t0;
    istringstream iss;
    assert(t0.Count() == 0);

    iss.clear();
    iss.str("Newton\n");
    t0.Add(iss);
    assert(t0.Count() == 1);

    iss.clear();
    iss.str("Newton\n");
    t0.Add(iss);
    assert(t0.Count() == 1);

    iss.clear();
    iss.str("\n");
    t0.Add(iss);
    assert(t0.Count() == 1);
    iss.clear();
    iss.str("\n\nNewton\n\nNewton\n\nNewton\n\n");
    t0.Add(iss);
    assert(t0.Count() == 1);

    iss.clear();
    iss.str("Black Hill\n");
    t0.Add(iss);
    assert(t0.Count() == 2);

    iss.clear();
    iss.str("Wood Side\n");
    t0.Add(iss);
    assert(t0.Count() == 3);

    iss.clear();
    iss.str("Green Hill\n");
    t0.Add(iss);
    assert(t0.Count() == 4);

    iss.clear();
    iss.str("Lakeside\n");
    t0.Add(iss);
    assert(t0.Count() == 5);

    iss.clear();
    iss.str("Newton\nBlack Hill\nWood Side\nGreen Hill\nLakeside\n");
    t0.Add(iss);
    assert(t0.Count() == 1);

    iss.clear();
    iss.str("Little Newton\nLittle Burnside\nCastle Hill\nNewton Crossroad\nLakeside Central\n");
    t0.Add(iss);
    assert(t0.Count() == 2);

    iss.clear();
    iss.str("Waterton West\nWaterton Central\nWaterton East\nWaterton Woods\nLittle Waterton\nWaterton West\n");
    t0.Add(iss);
    assert(t0.Count() == 3);

    iss.clear();
    iss.str("Little Waterton\nLakeside Central\n");
    t0.Add(iss);
    assert(t0.Count() == 2);

    iss.clear();
    iss.str("Great Newton\nLittle Burnside\nGreen Hill\nWood Side\n");
    t0.Add(iss);
    assert(t0.Count() == 1);

    iss.clear();
    iss.str("Sodor Ironworks\nSodor Steamworks\nKnapford\nMaron\n");
    t0.Add(iss);
    assert(t0.Count() == 2);

    iss.clear();
    iss.str("Tidmouth\nGordons Hill\nSuderry\nKnapford\nGreat Waterton\nBrendam Docks\n");
    t0.Add(iss);
    assert(t0.Count() == 2);

    iss.clear();
    iss.str("Newton\nNewton Crossroad\nTidmouth\nBrendam Docks\n");
    t0.Add(iss);
    assert(t0.Count() == 1);
    return 0;
}