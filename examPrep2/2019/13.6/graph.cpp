#include <iostream>
#include <map>
#include <set>
#include <cassert>
#include <queue>
#include <sstream>

using namespace std;

class CTrain
{
private:
    map<string, set<string>> m_graph;

public:
    CTrain(void) {}

    ~CTrain(void) {}

    void Add(istream &is)
    {
        string station, stop;
        getline(is, station);

        while (getline(is, stop))
        {
            m_graph[station].insert(stop);
            m_graph[stop].insert(station);
            station = stop;
        }
    }
    set<string> Dest(const vector<string> &stations, int &cost)
    {
        map<string, int> costs;
        set<string> result;
        set<string> global_visited;

        for (const auto &station : stations)
        {
            if (global_visited.contains(station))
                continue;

            if (!m_graph.contains(station))
            {
                cost = 0;
                return set<string>({stations.begin(), stations.end()});
            }

            queue<pair<string, int>> q({make_pair(station, 0)});
            costs.insert({station, 0});
            set<string> visited{station};
            global_visited.insert(station);

            while (!q.empty())
            {
                string station = q.front().first;
                int total = q.front().second;
                q.pop();

                for (const auto &stop : m_graph.at(station))
                {
                    if (!visited.contains(stop))
                    {
                        visited.insert(stop);
                        q.push(make_pair(stop, total + 1));

                        if (costs.contains(stop))
                            costs[stop] += total + 1;

                        else
                            costs[stop] = total + 1;
                    }
                }
            }

            for (const auto &station : stations)
                if (!visited.contains(station))
                    return {};
        }

        cost = min_element(costs.begin(), costs.end(), [](const auto &l, const auto &r)
                           { return l.second < r.second; })
                   ->second;

        for (const auto &[station, total] : costs)
            if (total == cost)
                result.insert(station);

        return result;
    }

    friend ostream &operator<<(ostream &os, const CTrain &src)
    {
        for (const auto &[station, stops] : src.m_graph)
        {
            os << station << " : ";
            for (const auto &stop : stops)
                os << stop << " ";
            os << endl;
        }
        return os;
    }
};

int main(void)
{
    CTrain t0;
    istringstream iss;

    iss.clear();
    iss.str(
        "1\n"
        "2\n"
        "3\n"
        "4\n"
        "5\n"
        "6\n"
        "7\n"
        "8\n");
    t0.Add(iss);
    iss.clear();
    iss.str(
        "12\n"
        "11\n"
        "4\n"
        "9\n"
        "10\n");
    t0.Add(iss);
    iss.clear();
    iss.str(
        "15\n"
        "11\n"
        "13\n"
        "14\n");
    t0.Add(iss);
    iss.clear();
    iss.str(
        "7\n"
        "16\n"
        "17\n");
    t0.Add(iss);
    iss.clear();
    iss.str(
        "20\n"
        "19\n"
        "17\n"
        "18\n");
    t0.Add(iss);
    iss.clear();
    iss.str(
        "21\n"
        "22\n"
        "23\n"
        "25\n"
        "24\n"
        "21\n");
    t0.Add(iss);
    iss.clear();
    iss.str(
        "26\n"
        "27\n"
        "28\n");
    t0.Add(iss);
    iss.clear();
    iss.str(
        "27\n"
        "29\n"
        "30\n");
    t0.Add(iss);

    int cost = 5381;
    assert(t0.Dest({"2", "5", "11"}, cost) == set<string>({"4"}) && cost == 4);
    assert(t0.Dest({"12", "10"}, cost) == set<string>({"10", "9", "4", "11", "12"}) && cost == 4);
    assert(t0.Dest({"2", "9", "19"}, cost) == set<string>({"4"}) && cost == 9);
    assert(t0.Dest({"15", "12", "13"}, cost) == set<string>({"11"}) && cost == 3);
    cost = 5381;
    assert(t0.Dest({"21", "26"}, cost) == set<string>() && cost == 5381);
    cost = 1256;
    assert(t0.Dest({"10", "28"}, cost) == set<string>() && cost == 1256);
    assert(t0.Dest({"21", "25"}, cost) == set<string>({"21", "24", "25"}) && cost == 2);
    assert(t0.Dest({"21", "21"}, cost) == set<string>({"21"}) && cost == 0);
    assert(t0.Dest({"23", "21"}, cost) == set<string>({"21", "22", "23"}) && cost == 2);
    assert(t0.Dest({"12", "20"}, cost) == set<string>({"12", "11", "4", "5", "6", "7", "16", "17", "19", "20"}) && cost == 9);
    assert(t0.Dest({"50"}, cost) == set<string>({"50"}) && cost == 0);
    assert(t0.Dest({"13", "10"}, cost) == set<string>({"10", "11", "13", "4", "9"}) && cost == 4);
    assert(t0.Dest({"17", "19"}, cost) == set<string>({"17", "19"}) && cost == 1);
    cost = 1999;
    assert(t0.Dest({"29", "25"}, cost) == set<string>() && cost == 1999);
    assert(t0.Dest({"8", "5"}, cost) == set<string>({"5", "6", "7", "8"}) && cost == 3);
    assert(t0.Dest({"4", "4"}, cost) == set<string>({"4"}) && cost == 0);

    return 0;
}