#ifndef __PROGTEST__
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <array>
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
  static bool compare(const CTimeStamp &one, const CTimeStamp &two, const CTimeStamp &target)
  {
    if (target.m_date.m_year < one.m_date.m_year || target.m_date.m_year > two.m_date.m_year)
    {
      return false;
    }
    if (target.m_date.m_year == one.m_date.m_year && target.m_date.m_month < one.m_date.m_month)
    {
      return false;
    }
    if (target.m_date.m_year == two.m_date.m_year && target.m_date.m_month > two.m_date.m_month)
    {
      return false;
    }
    if (target.m_date.m_year == one.m_date.m_year && target.m_date.m_month == one.m_date.m_month && target.m_date.m_day < one.m_date.m_day)
    {
      return false;
    }
    if (target.m_date.m_year == two.m_date.m_year && target.m_date.m_month == two.m_date.m_month && target.m_date.m_day > two.m_date.m_day)
    {
      return false;
    }
    if (target.m_date.m_year == one.m_date.m_year && target.m_date.m_month == one.m_date.m_month && target.m_date.m_day == one.m_date.m_day &&
        target.m_date.m_hour < one.m_date.m_hour)
    {
      return false;
    }
    if (target.m_date.m_year == two.m_date.m_year && target.m_date.m_month == two.m_date.m_month && target.m_date.m_day == two.m_date.m_day &&
        target.m_date.m_hour > two.m_date.m_hour)
    {
      return false;
    }
    if (target.m_date.m_year == one.m_date.m_year && target.m_date.m_month == one.m_date.m_month && target.m_date.m_day == one.m_date.m_day &&
        target.m_date.m_hour == one.m_date.m_hour && target.m_date.m_min < one.m_date.m_min)
    {
      return false;
    }
    if (target.m_date.m_year == two.m_date.m_year && target.m_date.m_month == two.m_date.m_month && target.m_date.m_day == two.m_date.m_day &&
        target.m_date.m_hour == two.m_date.m_hour && target.m_date.m_min > two.m_date.m_min)
    {
      return false;
    }
    if (target.m_date.m_year == one.m_date.m_year && target.m_date.m_month == one.m_date.m_month && target.m_date.m_day == one.m_date.m_day &&
        target.m_date.m_hour == one.m_date.m_hour && target.m_date.m_min == one.m_date.m_min && target.m_date.m_sec < one.m_date.m_sec)
    {
      return false;
    }
    if (target.m_date.m_year == two.m_date.m_year && target.m_date.m_month == two.m_date.m_month && target.m_date.m_day == two.m_date.m_day &&
        target.m_date.m_hour == two.m_date.m_hour && target.m_date.m_min == two.m_date.m_min && target.m_date.m_sec > two.m_date.m_sec)
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
  vector<CContact> m_contacts;

public:
  // initialize to empty vector
  CEFaceMask() : m_contacts{} {}
  void addContact(const CContact &contact_details)
  {
    m_contacts.push_back(contact_details);
  }

  bool in_list(const vector<int> &list, int number) const
  {
    for (const auto &x : list)
    {
      if (x == number)
      {
        return true;
      }
    }
    return false;
  }
  vector<int> listContacts(int infected_phone_number, const CTimeStamp &beginning_interval = CTimeStamp(0, 0, 0, 0, 0, 0), const CTimeStamp &end_interval = CTimeStamp(0, 0, 0, 0, 0, 0)) const
  {
    vector<int> result;
    if (beginning_interval.get_year() == 0 && end_interval.get_year() == 0)
    {
      for (const auto &x : m_contacts)
      {
        if (x.get_contact1() == infected_phone_number)
        {
          if (in_list(result, x.get_contact2()) == false && x.get_contact2() != infected_phone_number)
          {
            result.push_back(x.get_contact2());
          }
        }
        else if (x.get_contact2() == infected_phone_number)
        {
          if (in_list(result, x.get_contact1()) == false && x.get_contact1() != infected_phone_number)
          {
            result.push_back(x.get_contact1());
          }
        }
      }
    }
    else
    {
      for (const auto &x : m_contacts)
      {
        if (CTimeStamp::compare(beginning_interval, end_interval, x.get_timestamp()) == true && x.get_contact1() == infected_phone_number)
        {
          if (in_list(result, x.get_contact2()) == false && x.get_contact2() != infected_phone_number)
          {
            result.push_back(x.get_contact2());
          }
        }
        else if (CTimeStamp::compare(beginning_interval, end_interval, x.get_timestamp()) == true && x.get_contact2() == infected_phone_number)
        {
          if (in_list(result, x.get_contact1()) == false && x.get_contact1() != infected_phone_number)
          {
            result.push_back(x.get_contact1());
          }
        }
      }
    }
    return result;
  }
};

#ifndef __PROGTEST__
int main()
{
  CEFaceMask test;

  test.addContact(CContact(CTimeStamp(2021, 1, 10, 12, 40, 10), 123456789, 999888777));
  test.addContact(CContact(CTimeStamp(2021, 1, 12, 12, 40, 10), 123456789, 111222333));
  test.addContact(CContact(CTimeStamp(2021, 2, 5, 15, 30, 28), 999888777, 555000222));
  test.addContact(CContact(CTimeStamp(2021, 2, 21, 18, 0, 0), 123456789, 999888777));
  test.addContact(CContact(CTimeStamp(2021, 1, 5, 18, 0, 0), 123456789, 456456456));
  test.addContact(CContact(CTimeStamp(2021, 2, 1, 0, 0, 0), 123456789, 123456789));
  assert(test.listContacts(123456789) == (vector<int>{999888777, 111222333, 456456456}));
  assert(test.listContacts(999888777) == (vector<int>{123456789, 555000222}));
  assert(test.listContacts(191919191) == (vector<int>{}));
  assert(test.listContacts(123456789, CTimeStamp(2021, 1, 5, 18, 0, 0), CTimeStamp(2021, 2, 21, 18, 0, 0)) == (vector<int>{999888777, 111222333, 456456456}));
  assert(test.listContacts(123456789, CTimeStamp(2021, 1, 5, 18, 0, 1), CTimeStamp(2021, 2, 21, 17, 59, 59)) == (vector<int>{999888777, 111222333}));
  assert(test.listContacts(123456789, CTimeStamp(2021, 1, 10, 12, 41, 9), CTimeStamp(2021, 2, 21, 17, 59, 59)) == (vector<int>{111222333}));
  return 0;
}
#endif /* __PROGTEST__ */