#ifndef __PROGTEST__
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <climits>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <array>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <functional>
#include <iterator>
#include <compare>

class CDate
{
public:
  CDate(int y, int m, int d) : m_Y(y), m_M(m), m_D(d)
  {
  }

  std::strong_ordering operator<=>(const CDate &other) const = default;

  friend std::ostream &operator<<(std::ostream &os, const CDate &d)
  {
    return os << d.m_Y << '-' << d.m_M << '-' << d.m_D;
  }

private:
  int m_Y;
  int m_M;
  int m_D;
};

enum class ESortKey
{
  NAME,
  BIRTH_DATE,
  ENROLL_YEAR
};

#endif /* __PROGTEST__ */

class CStudent
{
private:
  std::string m_name;
  CDate m_dob;
  int m_enrollment_year;

  std::multiset<std::string> m_names_set;
  std::vector<std::string> m_sorted_names_vec;

  void splitNameIntoWords()
  {
    std::string name;
    for (char c : m_name)
    {
      if (c == ' ')
      {
        if (!name.empty())
        {
          m_names_set.insert(name);
          m_sorted_names_vec.push_back(name);
          name.clear();
        }
      }
      else
      {
        name += std::tolower(c);
      }
    }
    if (!name.empty())
    {
      m_names_set.insert(name);
      m_sorted_names_vec.push_back(name);
    }
  }

public:
  CStudent(const std::string &name, const CDate &born, int enrolled)
      : m_name(name), m_dob(born), m_enrollment_year(enrolled)
  {
    splitNameIntoWords();
    std::sort(m_sorted_names_vec.begin(), m_sorted_names_vec.end());
  }

  std::strong_ordering operator<=>(const CStudent &other) const = default;

  bool passesNameFilter(const std::string &name) const
  {
    return m_names_set.contains(name);
  }

  std::string getName() const
  {
    return m_name;
  }

  CDate getDOB() const
  {
    return m_dob;
  }

  int getEnrollment() const
  {
    return m_enrollment_year;
  }

  auto getNamesSet() const
  {
    return m_names_set;
  }

  auto getNamesVec() const
  {
    return m_sorted_names_vec;
  }
};

class CFilter
{
private:
  std::set<std::vector<std::string>> m_accepted_names;

  CDate m_bornBefore = CDate(0, 0, 0);
  CDate m_bornAfter = CDate(0, 0, 0);

  int m_enrolledBefore;
  int m_enrolledAfter;

  std::array<bool, 5> m_filter_list{false};

  bool m_empty;

  void splitNameIntoWords(const std::string &full_name, std::vector<std::string> &vec) const
  {
    std::string name;
    for (char c : full_name)
    {
      if (c == ' ')
      {
        if (!name.empty())
        {
          vec.push_back(name);
          name.clear();
        }
      }
      else
      {
        name += std::tolower(c);
      }
    }
    if (!name.empty())
    {
      vec.push_back(name);
    }
  }

public:
  CFilter() : m_empty(true) {}

  CFilter &name(const std::string &name)
  {
    std::vector<std::string> vec;
    std::string nameLower = name;

    splitNameIntoWords(nameLower, vec);
    std::sort(vec.begin(), vec.end());
    m_accepted_names.insert(vec);

    m_filter_list[0] = true;
    m_empty = false;
    return *this;
  }

  CFilter &bornBefore(const CDate &date)
  {
    m_bornBefore = date;
    m_filter_list[1] = true;
    m_empty = false;
    return *this;
  }

  CFilter &bornAfter(const CDate &date)
  {
    m_bornAfter = date;
    m_filter_list[2] = true;
    m_empty = false;
    return *this;
  }

  CFilter &enrolledBefore(int year)
  {
    m_enrolledBefore = year;
    m_filter_list[3] = true;
    m_empty = false;
    return *this;
  }

  CFilter &enrolledAfter(int year)
  {
    m_enrolledAfter = year;
    m_filter_list[4] = true;
    m_empty = false;
    return *this;
  }

  bool passesFilter(const CStudent &student) const
  {
    // Check if student's name matches any accepted names
    if (m_filter_list[0] && !m_accepted_names.empty())
      if (!m_accepted_names.contains(student.getNamesVec()))
        return false;

    // Check if student's date of birth is after the specified date
    if (m_filter_list[1] && student.getDOB() >= m_bornBefore)
      return false;

    // // Check if student's date of birth is before the specified date
    if (m_filter_list[2] && student.getDOB() <= m_bornAfter)
      return false;

    // Check if student's enrollment year is after the specified year
    if (m_filter_list[3] && student.getEnrollment() >= m_enrolledBefore)
      return false;

    // Check if student's enrollment year is before the specified year
    if (m_filter_list[4] && student.getEnrollment() <= m_enrolledAfter)
      return false;

    // All filters passed
    return true;
  }

  bool isEmpty() const
  {
    return m_empty;
  }
};

class CSort
{
private:
  std::vector<std::pair<ESortKey, bool>> m_key_list;

public:
  CSort() {}

  CSort &addKey(ESortKey key, bool ascending)
  {
    m_key_list.push_back({key, ascending});
    return *this;
  }

  auto getKeyList() const
  {
    return m_key_list;
  }
};

// functor to custom sort CStudyDept m_db
class CStudentDeptComparator
{
private:
  CSort m_sort_keys;

public:
  CStudentDeptComparator(const CSort &sort_keys) : m_sort_keys(sort_keys) {}

  bool operator()(const CStudent &lhs, const CStudent &rhs)
  {
    for (const auto &[key, ascending] : m_sort_keys.getKeyList())
    {
      switch (key)
      {
      case ESortKey::NAME:
        if (ascending ? (lhs.getName() < rhs.getName()) : (rhs.getName() < lhs.getName()))
        {
          return true;
        }

        if (lhs.getName() == rhs.getName())
        {
          continue;
        }

        break;

      case ESortKey::BIRTH_DATE:
        if (ascending ? (lhs.getDOB() < rhs.getDOB()) : (rhs.getDOB() < lhs.getDOB()))
        {
          return true;
        }

        if (lhs.getDOB() == rhs.getDOB())
        {
          continue;
        }
        break;

      case ESortKey::ENROLL_YEAR:
        if (ascending ? (lhs.getEnrollment() < rhs.getEnrollment()) : (rhs.getEnrollment() < lhs.getEnrollment()))
        {
          return true;
        }

        if (lhs.getEnrollment() == rhs.getEnrollment())
        {
          continue;
        }
        break;

      default:
        break;
      }

      return false;
    }

    // nothing required ( registration order )
    return false;
  }
};

class CStudyDept
{
private:
  std::map<CStudent, std::list<CStudent>::iterator> m_db;
  std::list<CStudent> m_db_list;

  void splitNameIntoWords(const std::string &full_name, std::vector<std::string> &vec) const
  {
    std::string name;
    for (char c : full_name)
    {
      if (c == ' ')
      {
        if (!name.empty())
        {
          vec.push_back(name);
          name.clear();
        }
      }
      else
      {
        name += std::tolower(c);
      }
    }
    if (!name.empty())
    {
      vec.push_back(name);
    }
  }

public:
  CStudyDept() {}

  bool addStudent(const CStudent &x)
  {
    auto [it, inserted] = m_db.insert({x, m_db_list.end()});

    if (inserted)
    {
      m_db_list.push_back(x);
      it->second = std::prev(m_db_list.end());
      return true;
    }
    return false;
  }

  bool delStudent(const CStudent &x)
  {
    // Find and erase the student from the main database
    auto it = m_db.find(x);
    if (it != m_db.end())
    {
      m_db_list.erase(it->second);
      m_db.erase(it);
      return true;
    }
    return false;
  }

  std::list<CStudent> search(const CFilter &flt, const CSort &sortOpt) const
  {
    std::list<CStudent> result;

    for (const auto &student : m_db_list)
    {
      if (flt.isEmpty() || flt.passesFilter(student))
      {
        result.push_back(student);
      }
    }

    result.sort(CStudentDeptComparator(sortOpt));
    return result;
  }

  std::set<std::string> suggest(const std::string &name) const
  {
    std::set<std::string> res;
    std::vector<std::string> vec;

    // Split the name into individual words
    splitNameIntoWords(name, vec);

    if (vec.empty())
    {
      return res;
    }

    for (const auto &student : m_db_list)
    {
      bool pass = true;
      for (const auto &name : vec)
      {
        if (!student.passesNameFilter(name))
          pass = false;
      }

      if (pass)
        res.insert(student.getName());
    }

    return res;
  }
};

#ifndef __PROGTEST__
int main(void)
{
  CStudyDept x0;
  assert(CStudent("James Bond", CDate(1980, 4, 11), 2010) == CStudent("James Bond", CDate(1980, 4, 11), 2010));
  assert(!(CStudent("James Bond", CDate(1980, 4, 11), 2010) != CStudent("James Bond", CDate(1980, 4, 11), 2010)));
  assert(CStudent("James Bond", CDate(1980, 4, 11), 2010) != CStudent("Peter Peterson", CDate(1980, 4, 11), 2010));
  assert(!(CStudent("James Bond", CDate(1980, 4, 11), 2010) == CStudent("Peter Peterson", CDate(1980, 4, 11), 2010)));
  assert(CStudent("James Bond", CDate(1980, 4, 11), 2010) != CStudent("James Bond", CDate(1997, 6, 17), 2010));
  assert(!(CStudent("James Bond", CDate(1980, 4, 11), 2010) == CStudent("James Bond", CDate(1997, 6, 17), 2010)));
  assert(CStudent("James Bond", CDate(1980, 4, 11), 2010) != CStudent("James Bond", CDate(1980, 4, 11), 2016));
  assert(!(CStudent("James Bond", CDate(1980, 4, 11), 2010) == CStudent("James Bond", CDate(1980, 4, 11), 2016)));
  assert(CStudent("James Bond", CDate(1980, 4, 11), 2010) != CStudent("Peter Peterson", CDate(1980, 4, 11), 2016));
  assert(!(CStudent("James Bond", CDate(1980, 4, 11), 2010) == CStudent("Peter Peterson", CDate(1980, 4, 11), 2016)));
  assert(CStudent("James Bond", CDate(1980, 4, 11), 2010) != CStudent("Peter Peterson", CDate(1997, 6, 17), 2010));
  assert(!(CStudent("James Bond", CDate(1980, 4, 11), 2010) == CStudent("Peter Peterson", CDate(1997, 6, 17), 2010)));
  assert(CStudent("James Bond", CDate(1980, 4, 11), 2010) != CStudent("James Bond", CDate(1997, 6, 17), 2016));
  assert(!(CStudent("James Bond", CDate(1980, 4, 11), 2010) == CStudent("James Bond", CDate(1997, 6, 17), 2016)));
  assert(CStudent("James Bond", CDate(1980, 4, 11), 2010) != CStudent("Peter Peterson", CDate(1997, 6, 17), 2016));
  assert(!(CStudent("James Bond", CDate(1980, 4, 11), 2010) == CStudent("Peter Peterson", CDate(1997, 6, 17), 2016)));
  assert(x0.addStudent(CStudent("John Peter Taylor", CDate(1983, 7, 13), 2014)));
  assert(x0.addStudent(CStudent("John Taylor", CDate(1981, 6, 30), 2012)));
  assert(x0.addStudent(CStudent("Peter Taylor", CDate(1982, 2, 23), 2011)));
  assert(x0.addStudent(CStudent("Peter John Taylor", CDate(1984, 1, 17), 2017)));
  assert(x0.addStudent(CStudent("James Bond", CDate(1981, 7, 16), 2013)));
  assert(x0.addStudent(CStudent("James Bond", CDate(1982, 7, 16), 2013)));
  assert(x0.addStudent(CStudent("James Bond", CDate(1981, 8, 16), 2013)));
  assert(x0.addStudent(CStudent("James Bond", CDate(1981, 7, 17), 2013)));
  assert(x0.addStudent(CStudent("James Bond", CDate(1981, 7, 16), 2012)));
  assert(x0.addStudent(CStudent("Bond James", CDate(1981, 7, 16), 2013)));
  assert(x0.search(CFilter(), CSort()) == (std::list<CStudent>{
                                              CStudent("John Peter Taylor", CDate(1983, 7, 13), 2014),
                                              CStudent("John Taylor", CDate(1981, 6, 30), 2012),
                                              CStudent("Peter Taylor", CDate(1982, 2, 23), 2011),
                                              CStudent("Peter John Taylor", CDate(1984, 1, 17), 2017),
                                              CStudent("James Bond", CDate(1981, 7, 16), 2013),
                                              CStudent("James Bond", CDate(1982, 7, 16), 2013),
                                              CStudent("James Bond", CDate(1981, 8, 16), 2013),
                                              CStudent("James Bond", CDate(1981, 7, 17), 2013),
                                              CStudent("James Bond", CDate(1981, 7, 16), 2012),
                                              CStudent("Bond James", CDate(1981, 7, 16), 2013)}));
  assert(x0.search(CFilter(), CSort().addKey(ESortKey::NAME, true)) == (std::list<CStudent>{
                                                                           CStudent("Bond James", CDate(1981, 7, 16), 2013),
                                                                           CStudent("James Bond", CDate(1981, 7, 16), 2013),
                                                                           CStudent("James Bond", CDate(1982, 7, 16), 2013),
                                                                           CStudent("James Bond", CDate(1981, 8, 16), 2013),
                                                                           CStudent("James Bond", CDate(1981, 7, 17), 2013),
                                                                           CStudent("James Bond", CDate(1981, 7, 16), 2012),
                                                                           CStudent("John Peter Taylor", CDate(1983, 7, 13), 2014),
                                                                           CStudent("John Taylor", CDate(1981, 6, 30), 2012),
                                                                           CStudent("Peter John Taylor", CDate(1984, 1, 17), 2017),
                                                                           CStudent("Peter Taylor", CDate(1982, 2, 23), 2011)}));
  assert(x0.search(CFilter(), CSort().addKey(ESortKey::NAME, false)) == (std::list<CStudent>{
                                                                            CStudent("Peter Taylor", CDate(1982, 2, 23), 2011),
                                                                            CStudent("Peter John Taylor", CDate(1984, 1, 17), 2017),
                                                                            CStudent("John Taylor", CDate(1981, 6, 30), 2012),
                                                                            CStudent("John Peter Taylor", CDate(1983, 7, 13), 2014),
                                                                            CStudent("James Bond", CDate(1981, 7, 16), 2013),
                                                                            CStudent("James Bond", CDate(1982, 7, 16), 2013),
                                                                            CStudent("James Bond", CDate(1981, 8, 16), 2013),
                                                                            CStudent("James Bond", CDate(1981, 7, 17), 2013),
                                                                            CStudent("James Bond", CDate(1981, 7, 16), 2012),
                                                                            CStudent("Bond James", CDate(1981, 7, 16), 2013)}));
  assert(x0.search(CFilter(), CSort().addKey(ESortKey::ENROLL_YEAR, false).addKey(ESortKey::BIRTH_DATE, false).addKey(ESortKey::NAME, true)) == (std::list<CStudent>{
                                                                                                                                                    CStudent("Peter John Taylor", CDate(1984, 1, 17), 2017),
                                                                                                                                                    CStudent("John Peter Taylor", CDate(1983, 7, 13), 2014),
                                                                                                                                                    CStudent("James Bond", CDate(1982, 7, 16), 2013),
                                                                                                                                                    CStudent("James Bond", CDate(1981, 8, 16), 2013),
                                                                                                                                                    CStudent("James Bond", CDate(1981, 7, 17), 2013),
                                                                                                                                                    CStudent("Bond James", CDate(1981, 7, 16), 2013),
                                                                                                                                                    CStudent("James Bond", CDate(1981, 7, 16), 2013),
                                                                                                                                                    CStudent("James Bond", CDate(1981, 7, 16), 2012),
                                                                                                                                                    CStudent("John Taylor", CDate(1981, 6, 30), 2012),
                                                                                                                                                    CStudent("Peter Taylor", CDate(1982, 2, 23), 2011)}));

  assert(x0.search(CFilter().name("james bond"), CSort().addKey(ESortKey::ENROLL_YEAR, false).addKey(ESortKey::BIRTH_DATE, false).addKey(ESortKey::NAME, true)) == (std::list<CStudent>{
                                                                                                                                                                       CStudent("James Bond", CDate(1982, 7, 16), 2013),
                                                                                                                                                                       CStudent("James Bond", CDate(1981, 8, 16), 2013),
                                                                                                                                                                       CStudent("James Bond", CDate(1981, 7, 17), 2013),
                                                                                                                                                                       CStudent("Bond James", CDate(1981, 7, 16), 2013),
                                                                                                                                                                       CStudent("James Bond", CDate(1981, 7, 16), 2013),
                                                                                                                                                                       CStudent("James Bond", CDate(1981, 7, 16), 2012)}));

  assert(x0.search(CFilter().bornAfter(CDate(1980, 4, 11)).bornBefore(CDate(1983, 7, 13)).name("John Taylor").name("james BOND"), CSort().addKey(ESortKey::ENROLL_YEAR, false).addKey(ESortKey::BIRTH_DATE, false).addKey(ESortKey::NAME, true)) == (std::list<CStudent>{
                                                                                                                                                                                                                                                        CStudent("James Bond", CDate(1982, 7, 16), 2013),
                                                                                                                                                                                                                                                        CStudent("James Bond", CDate(1981, 8, 16), 2013),
                                                                                                                                                                                                                                                        CStudent("James Bond", CDate(1981, 7, 17), 2013),
                                                                                                                                                                                                                                                        CStudent("Bond James", CDate(1981, 7, 16), 2013),
                                                                                                                                                                                                                                                        CStudent("James Bond", CDate(1981, 7, 16), 2013),
                                                                                                                                                                                                                                                        CStudent("James Bond", CDate(1981, 7, 16), 2012),
                                                                                                                                                                                                                                                        CStudent("John Taylor", CDate(1981, 6, 30), 2012)}));
  assert(x0.search(CFilter().name("james"), CSort().addKey(ESortKey::NAME, true)) == (std::list<CStudent>{}));
  assert(x0.suggest("peter") == (std::set<std::string>{
                                    "John Peter Taylor",
                                    "Peter John Taylor",
                                    "Peter Taylor"}));
  assert(x0.suggest("bond") == (std::set<std::string>{
                                   "Bond James",
                                   "James Bond"}));
  assert(x0.suggest("peter joHn") == (std::set<std::string>{
                                         "John Peter Taylor",
                                         "Peter John Taylor"}));
  assert(x0.suggest("peter joHn bond") == (std::set<std::string>{}));
  assert(x0.suggest("pete") == (std::set<std::string>{}));
  assert(x0.suggest("peter joHn PETER") == (std::set<std::string>{
                                               "John Peter Taylor",
                                               "Peter John Taylor"}));
                                               
  assert(!x0.addStudent(CStudent("James Bond", CDate(1981, 7, 16), 2013)));
  assert(x0.delStudent(CStudent("James Bond", CDate(1981, 7, 16), 2013)));
  assert(x0.search(CFilter().bornAfter(CDate(1980, 4, 11)).bornBefore(CDate(1983, 7, 13)).name("John Taylor").name("james BOND"), CSort().addKey(ESortKey::ENROLL_YEAR, false).addKey(ESortKey::BIRTH_DATE, false).addKey(ESortKey::NAME, true)) == (std::list<CStudent>{
                                                                                                                                                                                                                                                        CStudent("James Bond", CDate(1982, 7, 16), 2013),
                                                                                                                                                                                                                                                        CStudent("James Bond", CDate(1981, 8, 16), 2013),
                                                                                                                                                                                                                                                        CStudent("James Bond", CDate(1981, 7, 17), 2013),
                                                                                                                                                                                                                                                        CStudent("Bond James", CDate(1981, 7, 16), 2013),
                                                                                                                                                                                                                                                        CStudent("James Bond", CDate(1981, 7, 16), 2012),
                                                                                                                                                                                                                                                        CStudent("John Taylor", CDate(1981, 6, 30), 2012)}));
  assert(!x0.delStudent(CStudent("James Bond", CDate(1981, 7, 16), 2013)));
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
