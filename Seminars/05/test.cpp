#ifndef __PROGTEST__
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <numeric>
#include <vector>
#include <array>
#include <set>
#include <map>
#include <deque>
#include <queue>
#include <stack>
#include <cassert>
using namespace std;
#endif /* __PROGTEST__ */

class CTimeStamp
{
private:
  struct date
  {
    int m_year;
    int m_month;
    int m_day;
    int m_hour;
    int m_min;
    int m_sec;
  };
  
  date m_date;

public:
  CTimeStamp(int year, int month, int day, int hour, int min, int sec) : m_date{year, month, day, hour, min, sec} {}
  int get_year() const
  {
    return m_date.m_year;
  }
  // true if target within time_stamps one and two / beginning end
  bool compare(const CTimeStamp &one, const CTimeStamp &two) const
  {
    if (m_date.m_year < one.m_date.m_year || m_date.m_year > two.m_date.m_year)
    {
      return false;
    }
    if (m_date.m_year == one.m_date.m_year && m_date.m_month < one.m_date.m_month)
    {
      return false;
    }
    if (m_date.m_year == two.m_date.m_year && m_date.m_month > two.m_date.m_month)
    {
      return false;
    }
    if (m_date.m_year == one.m_date.m_year && m_date.m_month == one.m_date.m_month && m_date.m_day < one.m_date.m_day)
    {
      return false;
    }
    if (m_date.m_year == two.m_date.m_year && m_date.m_month == two.m_date.m_month && m_date.m_day > two.m_date.m_day)
    {
      return false;
    }
    if (m_date.m_year == one.m_date.m_year && m_date.m_month == one.m_date.m_month && m_date.m_day == one.m_date.m_day &&
        m_date.m_hour < one.m_date.m_hour)
    {
      return false;
    }
    if (m_date.m_year == two.m_date.m_year && m_date.m_month == two.m_date.m_month && m_date.m_day == two.m_date.m_day &&
        m_date.m_hour > two.m_date.m_hour)
    {
      return false;
    }
    if (m_date.m_year == one.m_date.m_year && m_date.m_month == one.m_date.m_month && m_date.m_day == one.m_date.m_day &&
        m_date.m_hour == one.m_date.m_hour && m_date.m_min < one.m_date.m_min)
    {
      return false;
    }
    if (m_date.m_year == two.m_date.m_year && m_date.m_month == two.m_date.m_month && m_date.m_day == two.m_date.m_day &&
        m_date.m_hour == two.m_date.m_hour && m_date.m_min > two.m_date.m_min)
    {
      return false;
    }
    if (m_date.m_year == one.m_date.m_year && m_date.m_month == one.m_date.m_month && m_date.m_day == one.m_date.m_day &&
        m_date.m_hour == one.m_date.m_hour && m_date.m_min == one.m_date.m_min && m_date.m_sec < one.m_date.m_sec)
    {
      return false;
    }
    if (m_date.m_year == two.m_date.m_year && m_date.m_month == two.m_date.m_month && m_date.m_day == two.m_date.m_day &&
        m_date.m_hour == two.m_date.m_hour && m_date.m_min == two.m_date.m_min && m_date.m_sec > two.m_date.m_sec)
    {
      return false;
    }
    return true;
  }
};
class CContact
{
private:
  CTimeStamp m_time_stamp;
  int m_phone_number1;
  int m_phone_number2;

public:
  CContact(const CTimeStamp &time_stamp, int phone_number1, int phone_number2) : m_time_stamp(time_stamp), m_phone_number1(phone_number1), m_phone_number2(phone_number2) {}
  CTimeStamp get_timestamp() const
  {
    return m_time_stamp;
  }
  int get_contact1() const
  {
    return m_phone_number1;
  }
  int get_contact2() const
  {
    return m_phone_number2;
  }
};

class CEFaceMask
{
private:
  std::vector<CContact> vec;

public:
  // default constructor
  CEFaceMask(){};
  // addContact ( contact )
  CEFaceMask &addContact(const CContact &contanct);
  // getSuperSpreaders ( from, to )
  std::vector<int> getSuperSpreaders(const CTimeStamp &from, const CTimeStamp &to) const;
  // TODO
};

CEFaceMask &CEFaceMask::addContact(const CContact &contact)
{
  vec.push_back(contact);
  return *this;
}

std::vector<int> CEFaceMask::getSuperSpreaders(const CTimeStamp &from, const CTimeStamp &to) const
{
  std::map<int, int> map;
  for (const auto &x : vec)
  {
    if (x.get_contact1() != x.get_contact2())
    {
      if (x.get_timestamp().compare(from, to))
      {
        map[x.get_contact1()]++;
        map[x.get_contact2()]++;
      }
    }
  }
  auto max = std::max_element(map.begin(), map.end(),
                              [](const auto &a, const auto &b)
                              { return a.second < b.second; });

  int max_value = max->second;
  std::vector<int> result;
  for (const auto x : map)
  {
    if (x.second == max_value)
    {
      result.push_back(x.first);
    }
  }
  std::sort(result.begin(), result.end());
  return result;
}

#ifndef __PROGTEST__
int main()
{
  CEFaceMask test;

  test.addContact(CContact(CTimeStamp(2021, 1, 10, 12, 40, 10), 111111111, 222222222));
  test.addContact(CContact(CTimeStamp(2021, 1, 12, 12, 40, 10), 333333333, 222222222));
  test.addContact(CContact(CTimeStamp(2021, 2, 14, 15, 30, 28), 222222222, 444444444));
  test.addContact(CContact(CTimeStamp(2021, 2, 15, 18, 0, 0), 555555555, 444444444));
  assert(test.getSuperSpreaders(CTimeStamp(2021, 1, 1, 0, 0, 0), CTimeStamp(2022, 1, 1, 0, 0, 0)) == (vector<int>{222222222}));
  test.addContact(CContact(CTimeStamp(2021, 3, 20, 18, 0, 0), 444444444, 666666666));
  test.addContact(CContact(CTimeStamp(2021, 3, 25, 0, 0, 0), 111111111, 666666666));
  assert(test.getSuperSpreaders(CTimeStamp(2021, 1, 1, 0, 0, 0), CTimeStamp(2022, 1, 1, 0, 0, 0)) == (vector<int>{222222222, 444444444}));
  assert(test.getSuperSpreaders(CTimeStamp(2025, 1, 1, 0, 0, 0), CTimeStamp(2022, 1, 1, 0, 0, 0)) == (vector<int>{}));
  test.addContact(CContact(CTimeStamp(2021, 1, 10, 12, 40, 10), 111111111, 222222222));
  test.addContact(CContact(CTimeStamp(2021, 1, 12, 12, 40, 10), 333333333, 222222222));
  test.addContact(CContact(CTimeStamp(2021, 2, 14, 15, 30, 28), 222222222, 444444444));
  test.addContact(CContact(CTimeStamp(2021, 2, 15, 18, 0, 0), 555555555, 444444444));
  test.addContact(CContact(CTimeStamp(2021, 3, 20, 18, 0, 0), 444444444, 666666666));
  test.addContact(CContact(CTimeStamp(2021, 3, 25, 0, 0, 0), 111111111, 666666666));
  assert(test.getSuperSpreaders(CTimeStamp(2021, 1, 1, 0, 0, 0), CTimeStamp(2022, 1, 1, 0, 0, 0)) == (vector<int>{222222222, 444444444}));

  CEFaceMask mask;
  mask.addContact(CContact{CTimeStamp{2023, 1, 1, 1, 1, 1}, 0000000, 111111});
  mask.addContact(CContact{CTimeStamp{2023, 1, 1, 1, 1, 1}, 2222222, 111111});
  mask.addContact(CContact{CTimeStamp{2023, 1, 1, 1, 1, 1}, 2222222, 333333});
  assert(mask.getSuperSpreaders(CTimeStamp{2023, 1, 1, 1, 1, 1}, CTimeStamp{2023, 1, 1, 1, 1, 1}) == (vector<int>{111111, 2222222}));
  return 0;
}
#endif /* __PROGTEST__ */
