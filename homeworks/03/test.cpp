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

public:
  CRange(long long lo, long long hi) : m_lo(lo), m_hi(hi)
  {
    if (lo > hi)
    {
      throw std::logic_error("Invalid boundaries");
    }
  }
  // getters
  long long get_low() const { return m_lo; }
  long long get_hi() const { return m_hi; }
  bool overlap(const CRange &other) const { return std::max(m_lo, other.m_lo) <= std::min(m_hi, other.m_hi) + 1; }
  bool single_integer() const { return m_lo == m_hi; };
  bool equal_range(const CRange &other) const { return m_lo == other.m_lo && m_hi == other.m_hi; }
  bool complete_containment(const CRange &other) const { return m_lo <= other.m_lo && m_hi >= other.m_hi; }
  bool right_side_engulf(const CRange &other) const { return other.m_lo <= m_hi && other.m_hi >= m_hi; }
  bool left_side_engulf(const CRange &other) const { return other.m_lo <= m_lo && other.m_hi >= m_lo; }
  // merging two intervals, modifying current instance
  CRange &merge(const CRange &other);
  friend std::ostream &operator<<(std::ostream &os, const CRange &range);
};

// public methods
CRange &CRange::merge(const CRange &other)
{
  this->m_lo = std::min(m_lo, other.m_lo);
  this->m_hi = std::max(m_hi, other.m_hi);
  return *this;
}
// friend functions
std::ostream &operator<<(std::ostream &os, const CRange &range)
{
  os << range.get_low() << ".." << range.get_hi() << endl;
  return os;
}

class CRangeList
{
private:
  vector<CRange> list_intervals;

  // methods
  int binary_search_interval(const CRange &interval) const;
  int binary_search_interval_hi(const CRange &interval) const;

public:
  // constructor initializes empty list of intervals
  CRangeList() {}
  // includes long long / range
  bool includes(long long val) const;
  bool includes(const CRange &interval) const;
  // += range / range list
  CRangeList operator+(const CRange &other) const;
  CRangeList &operator+=(const CRange &other);
  CRangeList &operator+=(const CRangeList &other);
  friend CRangeList operator+(const CRange &lhs, const CRange &rhs);
  // -= range / range list
  CRangeList operator-(const CRange &other) const;
  CRangeList &operator-=(const CRange &other);
  CRangeList &operator-=(const CRangeList &other);
  friend CRangeList operator-=(const CRange &lhs, const CRange &rhs);
  // = range / range list
  CRangeList &operator=(const CRange &other);
  CRangeList &operator=(const CRangeList &other);
  // operator ==
  bool operator==(const CRangeList &other) const;
  // operator !=
  bool operator!=(const CRangeList &other) const;
  // operator <<
  friend std::ostream &operator<<(std::ostream &os, const CRangeList &list);
};

// private methods
int CRangeList::binary_search_interval(const CRange &interval) const
{
  auto it = std::lower_bound(list_intervals.begin(), list_intervals.end(), interval, [](const CRange &a, const CRange &b)
                             { return a.get_low() < b.get_low(); });
  return it - list_intervals.begin();
}
int CRangeList::binary_search_interval_hi(const CRange &interval) const
{
  auto it = std::lower_bound(list_intervals.begin(), list_intervals.end(), interval, [](const CRange &a, const CRange &b)
                             { return a.get_hi() < b.get_hi(); });
  return it - list_intervals.begin();
}

// public methods
bool CRangeList::includes(long long val) const
{
  CRange temp{val, val};
  for (unsigned i = 0; i < list_intervals.size(); ++i)
  {
    if (list_intervals[i].overlap(temp))
    {
      return true;
    }
  }
  return false;
}
bool CRangeList::includes(const CRange &interval) const
{
  if (interval.single_integer())
  {
    return includes(interval.get_low());
  }
  CRangeList tmp1 = *this;
  tmp1 -= interval;
  tmp1 += interval;
  return tmp1 == *this;
  return false;
}

CRangeList &CRangeList::operator=(const CRange &other)
{
  if (list_intervals.size() == 0)
    list_intervals.clear();
  list_intervals.push_back(other);
  return *this;
}
CRangeList &CRangeList::operator=(const CRangeList &other)
{
  list_intervals = other.list_intervals;
  return *this;
}

CRangeList CRangeList::operator+(const CRange &other) const
{
  CRangeList temp = *this;
  return temp += other;
}
CRangeList &CRangeList::operator+=(const CRange &other)
{
  if (list_intervals.size() == 0)
  {
    list_intervals.push_back(other);
  }

  int index = binary_search_interval(other);
  int end_index = binary_search_interval_hi(other);
  // inserting at the beginning
  if (index == 0)
  {
    if (list_intervals[index].overlap(other))
    {
      list_intervals[index].merge(other);

      auto it = list_intervals.begin();
      while (it != list_intervals.end() + end_index)
      {
        auto next_it = std::next(it);
        if (next_it != list_intervals.end())
        {
          if (list_intervals[index].overlap(*next_it))
          {
            list_intervals[index].merge(*next_it);
            next_it = list_intervals.erase(next_it);
          }
          else
          {
            break;
          }
        }
        else
        {
          break;
        }
      }
    }
    else
    {
      list_intervals.insert(list_intervals.begin() + index, other);
    }
  }
  // inserting at the end
  else if (index == (int)list_intervals.size())
  {
    if (list_intervals[index - 1].overlap(other))
    {
      list_intervals[index - 1].merge(other);
    }
    else
    {
      list_intervals.insert(list_intervals.begin() + index, other);
    }
  }
  // inserting in the middle
  else
  {
    if (list_intervals[index - 1].overlap(other))
    {
      list_intervals[index - 1].merge(other);
      auto it = list_intervals.begin() + index;
      while (it != list_intervals.end())
      {
        auto prev_it = std::prev(it);
        if (list_intervals[index].overlap(*prev_it))
        {
          list_intervals[index].merge(*prev_it);
          prev_it = list_intervals.erase(prev_it);
        }
        else
        {
          break;
        }
      }
    }
    else if (index + 1 < (int)list_intervals.size())
    {
      bool merged = false;

      auto it = list_intervals.begin() + index;
      while (it != list_intervals.end() + end_index)
      {
        auto next_it = std::next(it);
        if (list_intervals[index].overlap(*next_it))
        {
          merged = true;
          list_intervals[index].merge(*next_it);
        }
        else
        {
          break;
        }
      }
      if (merged == false)
      {
        list_intervals.insert(list_intervals.begin() + index, other);
      }
    }
    else
    {
      list_intervals.insert(list_intervals.begin() + index, other);
    }
  }
  return *this;
}
CRangeList &CRangeList::operator+=(const CRangeList &other)
{
  for (const auto &interval : other.list_intervals)
  {
    *this += interval;
  }
  return *this;
}

CRangeList CRangeList::operator-(const CRange &other) const
{
  CRangeList temp = *this;
  return temp -= other;
}
CRangeList &CRangeList::operator-=(const CRange &other)
{
  CRangeList temp;
  for (unsigned i = 0; i < list_intervals.size(); ++i)
  {
    temp += list_intervals[i];
    // equal intervals
    if (temp.list_intervals.back().overlap(other))
    {
      // equal intervals
      if (temp.list_intervals.back().equal_range(other) || other.complete_containment(temp.list_intervals.back()))
      {
        temp.list_intervals.pop_back();
      }
      // completely inside interval
      else if (temp.list_intervals.back().complete_containment(other))
      {
        if (other.get_hi() == temp.list_intervals.back().get_hi())
        {
          CRange tmp1{temp.list_intervals.back().get_low(), other.get_low() - 1};
          temp.list_intervals.pop_back();
          temp += tmp1;
        }
        else if (other.get_low() == temp.list_intervals.back().get_low())
        {
          CRange tmp2{other.get_hi() + 1, temp.list_intervals.back().get_hi()};
          temp.list_intervals.pop_back();
          temp += tmp2;
        }
        else
        {
          CRange tmp1{temp.list_intervals.back().get_low(), other.get_low() - 1};
          CRange tmp2{other.get_hi() + 1, temp.list_intervals.back().get_hi()};
          temp.list_intervals.pop_back();
          temp += tmp1;
          temp += tmp2;
        }
      }
      // right side
      else if (temp.list_intervals.back().right_side_engulf(other))
      {
        if (temp.list_intervals.back().single_integer())
        {
          temp.list_intervals.pop_back();
        }
        else
        {
          CRange tmp{temp.list_intervals.back().get_low(), other.get_low() - 1};
          temp.list_intervals.pop_back();
          temp += tmp;
        }
      }
      // left side
      else if (temp.list_intervals.back().left_side_engulf(other))
      {
        if (temp.list_intervals.back().single_integer())
        {
          temp.list_intervals.pop_back();
        }
        else
        {
          CRange tmp{other.get_hi() + 1, temp.list_intervals.back().get_hi()};
          temp.list_intervals.pop_back();
          temp += tmp;
        }
      }
    }
  }
  return *this = temp;
}
CRangeList &CRangeList::operator-=(const CRangeList &other)
{
  for (const auto &interval : other.list_intervals)
  {
    *this -= interval;
  }
  return *this;
}

bool CRangeList::operator==(const CRangeList &other) const
{
  if (list_intervals.size() != other.list_intervals.size())
  {
    return false;
  }
  for (size_t i = 0; i < list_intervals.size(); ++i)
  {
    if (!list_intervals[i].equal_range(other.list_intervals[i]))
    {
      return false;
    }
  }
  return true;
}
bool CRangeList::operator!=(const CRangeList &other) const
{
  return !(*this == other);
}

// friend functions
CRangeList operator+(const CRange &lhs, const CRange &rhs)
{
  CRangeList result;
  result += lhs;
  result += rhs;
  return result;
}
CRangeList operator-(const CRange &lhs, const CRange &rhs)
{
  CRangeList result;
  result += lhs;
  result -= rhs;
  return result;
}
std::ostream &operator<<(std::ostream &os, const CRangeList &list)
{
  os << '{';
  for (unsigned i = 0; i < list.list_intervals.size(); ++i)
  {
    if (i < list.list_intervals.size() - 1)
    {
      if (list.list_intervals[i].single_integer())
      {
        os << list.list_intervals[i].get_low() << ",";
      }
      else
      {
        os << '<' << list.list_intervals[i].get_low() << ".." << list.list_intervals[i].get_hi() << ">,";
      }
    }
    else
    {
      if (list.list_intervals[i].single_integer())
      {
        os << list.list_intervals[i].get_low();
      }
      else
      {
        os << '<' << list.list_intervals[i].get_low() << ".." << list.list_intervals[i].get_hi() << '>';
      }
    }
  }
  os << '}';
  return os;
}
#ifndef __PROGTEST__
string
toString(const CRangeList &x)
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
