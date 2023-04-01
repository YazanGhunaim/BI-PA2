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
  bool overlap(const CRange &other) const;
  long long get_low() const { return m_lo; }
  long long get_hi() const { return m_hi; }
  void set_low(long long lo) { m_lo = lo; }
  void set_hi(long long hi) { m_hi = hi; }
  bool single_integer() const { return m_lo == m_hi; };
  bool equal_range(const CRange &other) const { return m_lo == other.m_lo && m_hi == other.m_hi; }
  bool complete_containment(const CRange &other) const { return m_lo <= other.m_lo && m_hi >= other.m_hi; }
  bool contains(const CRange &other) const { return m_lo < other.get_low() && m_hi > other.get_hi(); }
  bool right_side_engulf(const CRange &other) const { return m_hi > other.m_hi; }
  bool left_side_engulf(const CRange &other) const { return m_lo < other.m_lo; }
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
bool CRange::overlap(const CRange &other) const
{
  long long min_hi = std::min(m_hi, other.m_hi);
  long long max_lo = std::max(m_lo, other.m_lo);
  if (min_hi == LLONG_MAX)
  {
    return max_lo <= min_hi;
  }
  return max_lo <= min_hi + 1;
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
  friend CRangeList operator-(const CRange &lhs, const CRange &rhs);
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
  CRange target{val, val};

  unsigned x = binary_search_interval_hi(target);
  if (x >= 0 && x < list_intervals.size() && list_intervals[x].complete_containment(target))
    return true;
  return false;
}
bool CRangeList::includes(const CRange &target) const
{
  unsigned x = binary_search_interval(target);
  unsigned y = binary_search_interval_hi(target);
  if (x >= 0 && x < list_intervals.size() && (list_intervals[x].complete_containment(target) || list_intervals[y].complete_containment(target)))
    return true;
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
    return *this;
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
      list_intervals.push_back(other);
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
  auto it = list_intervals.begin();
  while (it != list_intervals.end())
  {
    if (it->overlap(other))
    {
      // modify the current interval in place
      if (it->contains(other))
      {
        long long tmp_hi = it->get_hi();
        it->set_hi(other.get_low() - 1);
        it = list_intervals.insert(it + 1, CRange{other.get_hi() + 1, tmp_hi});
      }
      else if (it->left_side_engulf(other))
      {
        it->set_hi(other.get_low() - 1);
      }
      else if (it->right_side_engulf(other))
      {
        it->set_low(other.get_hi() + 1);
      }
      else
      {
        // Interval overlaps with both sides of 'other', remove it
        it = list_intervals.erase(it);
        continue;
      }
    }
    ++it;
  }
  return *this;
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
  b += CRange(0, 100) + CRange(200, 300) - CRange(150, 250) + CRange(160, 180) - CRange{170, 170};
  assert(toString(b) == "{<0..100>,<160..169>,<171..180>,<251..300>}");
  b -= CRange(10, 90) - CRange(20, 30) - CRange(40, 50) - CRange(60, 90) + CRange(70, 80);
  assert(toString(b) == "{<0..9>,<20..30>,<40..50>,<60..69>,<81..100>,<160..169>,<171..180>,<251..300>}");

  CRangeList c;
  c += CRange(LLONG_MIN, LLONG_MAX);
  assert(toString(c) == "{<-9223372036854775808..9223372036854775807>}");

  CRangeList d;
  d += CRange(-100, 100);
  d += CRange(LLONG_MIN, LLONG_MAX);
  assert(toString(d) == "{<-9223372036854775808..9223372036854775807>}");

  CRangeList e;
  e += CRange(LLONG_MIN, -1);
  e += CRange(-100, 100);
  assert(toString(e) == "{<-9223372036854775808..100>}");

  // Test adding a range that overlaps with LLONG_MAX to a CRangeList object
  CRangeList f;
  f += CRange(1, LLONG_MAX);
  f += CRange(-100, 100);
  assert(toString(f) == "{<-100..9223372036854775807>}");

  // Test removing a range that includes LLONG_MIN and LLONG_MAX from a CRangeList object
  CRangeList g;
  g += CRange(LLONG_MIN, LLONG_MAX);
  g -= CRange(LLONG_MIN, LLONG_MAX);
  assert(toString(g) == "{}");

  // Test removing a range that overlaps with LLONG_MIN from a CRangeList object
  CRangeList h;
  h += CRange(LLONG_MIN, -1);
  h += CRange(-100, 100);
  h -= CRange(LLONG_MIN, 0);
  assert(toString(h) == "{<1..100>}");

  // Test removing a range that overlaps with LLONG_MAX from a CRangeList object
  CRangeList i;
  i += CRange(1, LLONG_MAX);
  i += CRange(-100, 100);
  i -= CRange(LLONG_MAX - 1, LLONG_MAX);
  assert(toString(i) == "{<-100..9223372036854775805>}");

  CRangeList j;
  for (long long i = 0; i < 10000000; i += 50)
  {
    j += CRange(i, i + 99);
  }
  CRangeList k;
  for (long long i = 0; i < 10000000; i += 100)
  {
    k += CRange(i, i + 99);
  }
  for (long long i = 0; i < 10000000; i += 200)
  {
    k -= CRange(i + 50, i + 149);
  }
  // CRangeList k;
  // for (int i = 0; i < INT_MAX; ++i)
  // {
  //   k += CRange(i, i);
  //   k.includes(i);
  // }
  // CRangeList j;
  // j += CRange(INT_MIN, INT_MAX);
  // for (int i = 0; i < INT_MAX; ++i)
  // {
  //   k -= CRange(i, i);
  //   k.includes(i);
  // }
  // CRangeList j;
  // j += CRange(INT_MIN, INT_MAX);
  // for (int i = 0; i < INT_MAX; i += 2)
  // {
  //   j -= CRange(i, i);
  // }
  // for (int i = 0; i < INT_MAX; i += 2)
  // {
  //   j += CRange(i, i);
  // }
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
