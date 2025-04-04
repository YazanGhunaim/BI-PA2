#ifndef __PROGTEST__
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <set>
#include <list>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <stack>
#include <queue>
#include <deque>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>
#include <iterator>
#include <stdexcept>
using namespace std;
#endif /* __PROGTEST__ */

template <typename M_>
class CContest
{
public:
  struct m_data
  {
    std::string m_contestant1;
    std::string m_contestant2;
    M_ m_result;

    m_data(const std::string &contestant1, const std::string &contestant2, const M_ &result)
        : m_contestant1(std::move(contestant1)), m_contestant2(std::move(contestant2)), m_result(std::move(result)) {}
    ~m_data() {}

    bool operator<(const m_data &rhs) const
    {
      return std::tie(m_contestant1, m_contestant2) < std::tie(rhs.m_contestant1, rhs.m_contestant2);
    }
  };

  std::deque<m_data> m_matches;
  std::unordered_set<std::string> m_contestants;
  mutable std::deque<std::string> m_result;
  mutable std::unordered_map<std::string, std::deque<std::string>> m_graph;

  // priv methods
  bool matchExists(const m_data &match) const
  {
    m_data x{match.m_contestant2, match.m_contestant1, match.m_result};
    for (const auto &m : m_matches)
    {
      if ((!(m < match) && !(match < m)) || (!(m < x) && !(x < m)))
      {
        return true;
      }
    }
    return false;
  }

  bool initializeGraph(const std::function<int(const M_ &)> &comparator) const
  {
    m_graph.clear();
    m_result.clear();
    for (const auto &m : m_matches)
    {
      int result = comparator(m.m_result);
      if (result > 0)
      {
        int count = std::count(m_graph[m.m_contestant1].begin(), m_graph[m.m_contestant1].end(), m.m_contestant2);
        if (count == 0)
        {
          m_graph[m.m_contestant1].push_back(m.m_contestant2);
        }
      }
      else if (result < 0)
      {
        int count = std::count(m_graph[m.m_contestant2].begin(), m_graph[m.m_contestant2].end(), m.m_contestant1);
        if (count == 0)
        {
          m_graph[m.m_contestant2].push_back(m.m_contestant1);
        }
      }
      else
      {
        return false;
      }
    }
    return true;
  }

  bool oneEndNode() const noexcept
  {
    size_t count = 0;
    for (const auto &it : m_contestants)
    {
      if (m_graph.find(it) == m_graph.end())
        count++;
    }
    return count == 1;
  }

  bool topoSort() const noexcept
  {
    std::vector<int> inDegree(m_contestants.size(), 0);
    for (const auto &[str, vec] : m_graph)
    {
      for (const auto &neighbor : vec)
      {
        inDegree[std::distance(m_contestants.begin(), m_contestants.find(neighbor))]++;
      }
    }

    std::queue<std::string> q;
    for (const auto &it : m_contestants)
    {
      if (inDegree[std::distance(m_contestants.begin(), m_contestants.find(it))] == 0)
      {
        q.push(it);
      }
    }

    size_t cnt = 0;
    while (!q.empty())
    {
      std::string u = q.front();
      q.pop();
      for (const auto &it : m_graph[u])
      {
        if (--inDegree[std::distance(m_contestants.begin(), m_contestants.find(it))] == 0)
        {
          q.push(it);
        }
      }
      cnt++;
      m_result.push_back(u);
    }

    return cnt == m_contestants.size();
  }

  bool onePath() const noexcept
  {
    for (size_t i = 1; i < m_result.size(); i++)
    {
      if (std::count(m_graph[m_result[i - 1]].begin(), m_graph[m_result[i - 1]].end(), m_result[i]) == 0)
      {
        return false;
      }
    }
    return true;
  }

public:
  CContest() {}
  ~CContest() {}
  CContest &addMatch(const std::string &contestant1, const std::string &contestant2, const M_ &result)
  {
    m_data match(contestant1, contestant2, result);
    if (matchExists(match))
    {
      throw std::logic_error("Match already exists");
    }
    m_contestants.insert(contestant1);
    m_contestants.insert(contestant2);
    m_matches.push_back(match);
    return *this;
  }
  bool isOrdered(const std::function<int(const M_ &)> &comparator) const noexcept
  {
    if (initializeGraph(comparator))
    {
      return oneEndNode() && topoSort() && onePath();
    }
    return false;
  }
  std::list<std::string> results(const std::function<int(const M_ &)> &comparator) const
  {
    if (!isOrdered(comparator))
    {
      throw std::logic_error("Cannot establish unambiguously");
    }
    std::list<std::string> resultList{m_result.begin(), m_result.end()};
    return resultList;
  }
};

#ifndef __PROGTEST__
struct CMatch
{
public:
  CMatch(int a, int b)
      : m_A(a), m_B(b) {}

  int m_A;
  int m_B;
};

class HigherScoreThreshold
{
public:
  HigherScoreThreshold(int diffAtLeast)
      : m_DiffAtLeast(diffAtLeast)
  {
  }
  int operator()(const CMatch &x) const
  {
    return (x.m_A > x.m_B + m_DiffAtLeast) - (x.m_B > x.m_A + m_DiffAtLeast);
  }

private:
  int m_DiffAtLeast;
};

int HigherScore(const CMatch &x)
{
  return (x.m_A > x.m_B) - (x.m_B > x.m_A);
}

int main(void)
{
  CContest<CMatch> x;

  x.addMatch("C++", "Pascal", CMatch(10, 3))
      .addMatch("C++", "Java", CMatch(8, 1))
      .addMatch("Pascal", "Basic", CMatch(40, 0))
      .addMatch("Java", "PHP", CMatch(6, 2))
      .addMatch("Java", "Pascal", CMatch(7, 3))
      .addMatch("PHP", "Basic", CMatch(10, 0));

  assert(!x.isOrdered(HigherScore));
  try
  {
    list<string> res = x.results(HigherScore);
    assert("Exception missing!" == nullptr);
  }
  catch (const logic_error &e)
  {
  }
  catch (...)
  {
    assert("Invalid exception thrown!" == nullptr);
  }

  x.addMatch("PHP", "Pascal", CMatch(3, 6));

  assert(x.isOrdered(HigherScore));
  try
  {
    list<string> res = x.results(HigherScore);
    assert((res == list<string>{"C++", "Java", "Pascal", "PHP", "Basic"}));
  }
  catch (...)
  {
    assert("Unexpected exception!" == nullptr);
  }

  assert(!x.isOrdered(HigherScoreThreshold(3)));
  try
  {
    list<string> res = x.results(HigherScoreThreshold(3));
    assert("Exception missing!" == nullptr);
  }
  catch (const logic_error &e)
  {
  }
  catch (...)
  {
    assert("Invalid exception thrown!" == nullptr);
  }

  x.isOrdered([](const CMatch &x)
              { return (x.m_A < x.m_B) - (x.m_B < x.m_A); });
  assert(x.isOrdered([](const CMatch &x)
                     { return (x.m_A < x.m_B) - (x.m_B < x.m_A); }));
  try
  {
    list<string> res = x.results([](const CMatch &x)
                                 { return (x.m_A < x.m_B) - (x.m_B < x.m_A); });
    assert((res == list<string>{"Basic", "PHP", "Pascal", "Java", "C++"}));
  }
  catch (...)
  {
    assert("Unexpected exception!" == nullptr);
  }

  CContest<bool> y;

  y.addMatch("Python", "PHP", true)
      .addMatch("PHP", "Perl", true)
      .addMatch("Perl", "Bash", true)
      .addMatch("Bash", "JavaScript", true)
      .addMatch("JavaScript", "VBScript", true);

  assert(y.isOrdered([](bool v)
                     { return v ? 10 : -10; }));
  try
  {
    list<string> res = y.results([](bool v)
                                 { return v ? 10 : -10; });
    assert((res == list<string>{"Python", "PHP", "Perl", "Bash", "JavaScript", "VBScript"}));
  }
  catch (...)
  {
    assert("Unexpected exception!" == nullptr);
  }

  y.addMatch("PHP", "JavaScript", false);
  assert(!y.isOrdered([](bool v)
                      { return v ? 10 : -10; }));
  try
  {
    list<string> res = y.results([](bool v)
                                 { return v ? 10 : -10; });
    assert("Exception missing!" == nullptr);
  }
  catch (const logic_error &e)
  {
  }
  catch (...)
  {
    assert("Invalid exception thrown!" == nullptr);
  }

  try
  {
    y.addMatch("PHP", "JavaScript", false);
    assert("Exception missing!" == nullptr);
  }
  catch (const logic_error &e)
  {
  }
  catch (...)
  {
    assert("Invalid exception thrown!" == nullptr);
  }

  try
  {
    y.addMatch("JavaScript", "PHP", true);
    assert("Exception missing!" == nullptr);
  }
  catch (const logic_error &e)
  {
  }
  catch (...)
  {
    assert("Invalid exception thrown!" == nullptr);
  }
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
