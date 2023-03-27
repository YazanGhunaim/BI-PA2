#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <ctime>
#include <climits>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <memory>
using namespace std;
#endif /* __PROGTEST__ */

// uncomment if your code implements initializer lists
// #define EXTENDED_SYNTAX

class CRange
{
private:
  long long m_lo;
  long long m_hi;

  bool boundary_check(long long lo, long long hi)
  {
    return lo <= hi;
  }

public:
  CRange(long long lo) : m_lo(lo), m_hi(lo) {}
  CRange(long long lo, long long hi)
  {
    if (!boundary_check(lo, hi))
    {
      throw std::logic_error("invalid boundaries.");
    }
    m_lo = lo;
    m_hi = hi;
  }
  bool in_interval(const long long val) const
  {
    return val <= m_hi && val >= m_lo;
  }
  bool equal_boundary() const
  {
    return m_lo == m_hi;
  }
  bool overlapping_boundaries(const CRange &target_interval) const
  {
    return std::max(m_lo, target_interval.m_lo) <= std::min(m_hi, target_interval.m_hi);
  }
  long long get_lo() const
  {
    return m_lo;
  }
  long long get_hi() const
  {
    return m_hi;
  }
  bool can_be_merged(const CRange &target_interval) const
  {
    return std::max(m_lo, target_interval.m_lo) <= std::min(m_hi, target_interval.m_hi) || std::max(m_lo, target_interval.m_lo) <= std::min(m_hi, target_interval.m_hi) + 1;
  }
};

class CRangeList
{
private:
  vector<CRange> list_of_intervals;

public:
  // constructor
  CRangeList() {}
  // includes long long / range
  bool includes(const long long val) const;
  bool includes(const CRange &interval) const;
  // += range / range list
  CRangeList &operator+=(const CRange &list)
  {
    bool merged = false;
    for (const auto &x : list_of_intervals)
    {
      if (x.can_be_merged(list))
      {
        merged = true;
      }
    }
    if (!merged)
    {
      list_of_intervals.push_back(list);
    }
    return *this;
  }
  CRangeList &operator+=(const CRangeList &list)
  {
  }
  // -= range / range list
  // = range / range list
  CRangeList &operator=(const CRange &other)
  {
    if (list_of_intervals.size() != 0)
    {
      list_of_intervals.clear();
    }
    list_of_intervals.push_back(other);
    return *this;
  }
  CRangeList &operator=(const CRangeList &other)
  {
    list_of_intervals = other.list_of_intervals;
    return *this;
  }
  // operator ==
  // operator !=
  // operator <<
};

bool CRangeList::includes(const long long val) const
{
  for (const auto &x : list_of_intervals)
  {
    if (x.in_interval(val))
    {
      return true;
    }
  }
  return false;
}
// TBC
bool CRangeList::includes(const CRange &target_interval) const
{
  if (target_interval.equal_boundary())
  {
    return includes(target_interval.get_lo());
  }

  vector<CRange> combined_intervals;
  for (const auto &x : list_of_intervals)
  {
    // if intervals overlap
    if (x.overlapping_boundaries(target_interval))
    {
      CRange temp{std::max(x.get_lo(), target_interval.get_lo()), std::min(x.get_hi(), target_interval.get_hi())};
      combined_intervals.push_back(temp);
    }
  }
  return true;
}

#ifndef __PROGTEST__
string toString(const CRangeList &x)
{
  ostringstream oss;
  oss << x;
  return oss.str();
}

int main(void)
{
  CRangeList a, b;

  assert(sizeof(CRange) <= 2 * sizeof(long long));
  a = CRange(5, 10);
  a += CRange(25, 100);
  assert(toString(a) == "{<5..10>,<25..100>}");
  a += CRange(-5, 0);
  a += CRange(8, 50);
  assert(toString(a) == "{<-5..0>,<5..100>}");
  a += CRange(101, 105) + CRange(120, 150) + CRange(160, 180) + CRange(190, 210);
  assert(toString(a) == "{<-5..0>,<5..105>,<120..150>,<160..180>,<190..210>}");
  a += CRange(106, 119) + CRange(152, 158);
  assert(toString(a) == "{<-5..0>,<5..150>,<152..158>,<160..180>,<190..210>}");
  a += CRange(-3, 170);
  a += CRange(-30, 1000);
  assert(toString(a) == "{<-30..1000>}");
  b = CRange(-500, -300) + CRange(2000, 3000) + CRange(700, 1001);
  a += b;
  assert(toString(a) == "{<-500..-300>,<-30..1001>,<2000..3000>}");
  a -= CRange(-400, -400);
  assert(toString(a) == "{<-500..-401>,<-399..-300>,<-30..1001>,<2000..3000>}");
  a -= CRange(10, 20) + CRange(900, 2500) + CRange(30, 40) + CRange(10000, 20000);
  assert(toString(a) == "{<-500..-401>,<-399..-300>,<-30..9>,<21..29>,<41..899>,<2501..3000>}");
  try
  {
    a += CRange(15, 18) + CRange(10, 0) + CRange(35, 38);
    assert("Exception not thrown" == nullptr);
  }
  catch (const std::logic_error &e)
  {
  }
  catch (...)
  {
    assert("Invalid exception thrown" == nullptr);
  }
  assert(toString(a) == "{<-500..-401>,<-399..-300>,<-30..9>,<21..29>,<41..899>,<2501..3000>}");
  b = a;
  assert(a == b);
  assert(!(a != b));
  b += CRange(2600, 2700);
  assert(toString(b) == "{<-500..-401>,<-399..-300>,<-30..9>,<21..29>,<41..899>,<2501..3000>}");
  assert(a == b);
  assert(!(a != b));
  b += CRange(15, 15);
  assert(toString(b) == "{<-500..-401>,<-399..-300>,<-30..9>,15,<21..29>,<41..899>,<2501..3000>}");
  assert(!(a == b));
  assert(a != b);
  assert(b.includes(15));
  assert(b.includes(2900));
  assert(b.includes(CRange(15, 15)));
  assert(b.includes(CRange(-350, -350)));
  assert(b.includes(CRange(100, 200)));
  assert(!b.includes(CRange(800, 900)));
  assert(!b.includes(CRange(-1000, -450)));
  assert(!b.includes(CRange(0, 500)));
  a += CRange(-10000, 10000) + CRange(10000000, 1000000000);
  assert(toString(a) == "{<-10000..10000>,<10000000..1000000000>}");
  b += a;
  assert(toString(b) == "{<-10000..10000>,<10000000..1000000000>}");
  b -= a;
  assert(toString(b) == "{}");
  b += CRange(0, 100) + CRange(200, 300) - CRange(150, 250) + CRange(160, 180) - CRange(170, 170);
  assert(toString(b) == "{<0..100>,<160..169>,<171..180>,<251..300>}");
  b -= CRange(10, 90) - CRange(20, 30) - CRange(40, 50) - CRange(60, 90) + CRange(70, 80);
  assert(toString(b) == "{<0..9>,<20..30>,<40..50>,<60..69>,<81..100>,<160..169>,<171..180>,<251..300>}");
#ifdef EXTENDED_SYNTAX
  CRangeList x{{5, 20}, {150, 200}, {-9, 12}, {48, 93}};
  assert(toString(x) == "{<-9..20>,<48..93>,<150..200>}");
  ostringstream oss;
  oss << setfill('=') << hex << left;
  for (const auto &v : x + CRange(-100, -100))
    oss << v << endl;
  oss << setw(10) << 1024;
  assert(oss.str() == "-100\n<-9..20>\n<48..93>\n<150..200>\n400=======");
#endif /* EXTENDED_SYNTAX */
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
