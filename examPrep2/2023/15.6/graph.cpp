#ifndef __PROGTEST__
#include <algorithm>
#include <assert.h>
#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

class CPos
{
public:
    CPos(int y, int x, int hour)
        : m_hour(hour), m_posY(y), m_posX(x)
    {
    }

    int m_hour;
    int m_posY;
    int m_posX;
};
#endif /* __PROGTEST__ */

class CBattleRoyale
{
private:
    bool isValid(int x, int y) const
    {
        if (y >= m_height || y < 0 || x >= m_width || x < 0)
            return false;
        return true;
    }

public:
    CBattleRoyale(unsigned height, unsigned width, const std::list<CPos> &zones)
        : m_height(height), m_width(width)
    {
        for (const auto &zone : zones)
        {
            auto coord = make_pair(zone.m_posX, zone.m_posY);
            m_banned[coord] = zone.m_hour;
        }
    }

    ~CBattleRoyale() {}

    unsigned findRoute(int ySt, int xSt, int yEn, int xEn) const
    {
        // start is banned
        if (m_banned.contains({xSt, ySt}))
            if (m_banned.at({xSt, ySt}) == 0)
                throw logic_error("");

        // start or end outside map
        if (!isValid(xSt, ySt) || !isValid(xEn, yEn))
            throw logic_error("");

        // start eq end
        if (ySt == yEn && xSt == xEn)
            return 0;

        static const vector<pair<int, int>> moves{{1, 0}, {0, 1}, {0, 0}, {-1, 0}, {0, -1}};

        // <x,y>
        queue<pair<int, int>> q;
        set<pair<int, int>> visited;
        map<pair<int, int>, int> dist;

        q.emplace(xSt, ySt);
        visited.emplace(xSt, ySt);
        dist[make_pair(xSt, ySt)] = 0;

        while (!q.empty())
        {
            auto currentX = q.front().first;
            auto currentY = q.front().second;
            auto currentCoord = make_pair(currentX, currentY);
            q.pop();

            if (currentX == xEn && currentY == yEn)
                break;

            for (const auto &move : moves)
            {
                auto newX = currentX + move.first;
                auto newY = currentY + move.second;
                auto newCoord = make_pair(newX, newY);
                auto newTime = dist[currentCoord] + 1;

                if (!visited.contains(newCoord) && isValid(newCoord.first, newCoord.second) && (!m_banned.contains(newCoord) || m_banned.at(newCoord) > newTime))
                {
                    dist[newCoord] = dist[currentCoord] + 1;
                    visited.insert(newCoord);
                    q.emplace(newCoord);
                }
            }
        }

        auto endCoord = make_pair(xEn, yEn);

        if (!dist.contains(endCoord))
            throw logic_error("");

        return dist[endCoord];
    }

private:
    int m_height;
    int m_width;
    map<pair<int, int>, int> m_banned;
};

#ifndef __PROGTEST__
int main()
{
    CBattleRoyale r1(5, 5, {});
    assert(r1.findRoute(0, 0, 4, 0) == 4);
    assert(r1.findRoute(4, 4, 4, 4) == 0);

    CBattleRoyale r2(6, 7, {CPos(1, 0, 1), CPos(2, 1, 2), CPos(3, 2, 5)});
    assert(r2.findRoute(0, 0, 4, 0) == 10);

    CBattleRoyale r3(8, 8, {CPos(0, 2, 1), CPos(3, 1, 2), CPos(2, 1, 0)});
    try
    {
        r3.findRoute(2, 1, 4, 7);
        assert("Exception missing!" == nullptr);
    }
    catch (const logic_error &e)
    {
    }
    catch (...)
    {
        assert("Invalid exception thrown!" == nullptr);
    }
    assert(r3.findRoute(0, 2, 3, 0) == 5);

    CBattleRoyale b(5, 5, {CPos(0, 1, 1), CPos(1, 1, 0)});
    assert(b.findRoute(0, 0, 2, 2) == 4);
    assert(b.findRoute(0, 0, 0, 2) == 6);
    assert(b.findRoute(3, 3, 3, 3) == 0);
    try
    {
        assert(b.findRoute(1, 1, 2, 1) == 1);
        assert("Kde vyjimka?" == nullptr);
    }
    catch (logic_error &x)
    {
    }
    try
    {
        assert(b.findRoute(1, 1, 1, 1) == 0);
        assert("Kde vyjimka? xd" == nullptr);
    }
    catch (logic_error &x)
    {
    }

    CBattleRoyale b1(5, 5, {CPos(2, 0, 2), CPos(2, 1, 1), CPos(2, 2, 1), CPos(2, 3, 3), CPos(2, 4, 4)});
    try
    {
        b1.findRoute(1, 1, 3, 1);
        assert("Kde vyjimka?" == nullptr);
    }
    catch (logic_error &x)
    {
    }
}
#endif /* __PROGTEST__ */