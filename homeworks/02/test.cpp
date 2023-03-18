#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <memory>
using namespace std;
#endif /* __PROGTEST__ */

class Employee
{
private:
  string m_name;
  string m_surname;
  string m_email;
  unsigned m_salary;

public:
  // constructor
  Employee(string name, string surname, string email, unsigned salary) : m_name(move(name)), m_surname(move(surname)), m_email(move(email)), m_salary(salary) {}
  void change_name(const string &new_name, const string &new_surname)
  {
    m_name = new_name;
    m_surname = new_surname;
  }
  void change_email(const string &new_email)
  {
    m_email = new_email;
  }
  void set_salary(unsigned salary)
  {
    m_salary = salary;
  }
  unsigned get_salary() const
  {
    return m_salary;
  }
  string get_name() const
  {
    return m_name;
  }
  string get_surname() const
  {
    return m_surname;
  }
  string get_email() const
  {
    return m_email;
  }
  bool compare_names(const string &name, const string &sur_name) const
  {
    return m_name == name && m_surname == sur_name;
  }
};

class CPersonalAgenda
{
private:
  vector<Employee> db_sorted_by_names;
  vector<Employee> db_sorted_by_emails;

public:
  CPersonalAgenda(void) {}

  ~CPersonalAgenda(void) {}

  bool add(const string &name, const string &surname, const string &email, unsigned int salary);

  bool del(const string &name, const string &surname);

  bool del(const string &email);

  bool changeName(const string &email, const string &newName, const string &newSurname);

  bool changeEmail(const string &name, const string &surname, const string &newEmail);

  bool setSalary(const string &name, const string &surname, unsigned int salary);

  bool setSalary(const string &email, unsigned int salary);

  unsigned int getSalary(const string &name, const string &surname) const;

  unsigned int getSalary(const string &email) const;

  bool getRank(const string &name, const string &surname, int &rankMin, int &rankMax) const;

  bool getRank(const string &email, int &rankMin, int &rankMax) const;

  bool getFirst(string &outName, string &outSurname) const;

  bool getNext(const string &name, const string &surname, string &outName, string &outSurname) const;

private:
  bool binarySearchByName(const vector<Employee> &database, const string &targetName, const string &targetSurname) const;
  int binarySearchNewName(const string &name, const string &sur_name) const;
  int binarySearchNewEmail(const string &email) const;
  bool unique_credentials(const string &email) const;
  bool unique_credentials(const string &name, const string &sur_name) const;
};

// private methods
bool CPersonalAgenda::binarySearchByName(const vector<Employee> &database, const string &targetName, const string &targetSurname) const
{
  int left = 0;
  int right = database.size() - 1;

  while (left <= right)
  {
    int mid = (left + right) / 2;

    if (database[mid].get_name() == targetName && database[mid].get_surname() == targetSurname)
    {
      return true;
    }
    else if (database[mid].get_name() < targetName || (database[mid].get_name() == targetName && database[mid].get_surname() < targetSurname))
    {
      // Employee is in the right half of the array
      left = mid + 1;
    }
    else
    {
      // Employee is in the left half of the array
      right = mid - 1;
    }
  }

  // Employee not found
  return false;
}

// to get appropriate index to insert while maintaining order
int CPersonalAgenda::binarySearchNewName(const string &name, const string &sur_name) const
{
  int first = 0;
  int last = db_sorted_by_names.size() - 1;
  int middle;

  while (first <= last)
  {
    middle = first + (last - first) / 2;
    if (db_sorted_by_names[middle].compare_names(name, sur_name))
    {
      return -1;
    }
    else if (db_sorted_by_names[middle].get_name() > name || (db_sorted_by_names[middle].get_name() == name && db_sorted_by_names[middle].get_surname() > sur_name))
    {
      last = middle - 1;
    }
    else
    {
      first = middle + 1;
    }
  }
  return first;
}

// to get appropriate index to insert while maintaining order
int CPersonalAgenda::binarySearchNewEmail(const string &email) const
{
  int first = 0;
  int last = db_sorted_by_emails.size() - 1;
  int middle;

  while (first <= last)
  {
    middle = first + (last - first) / 2;
    if (db_sorted_by_emails[middle].get_email() == email)
    {
      return -1;
    }
    else if (db_sorted_by_emails[middle].get_email() > email)
    {
      last = middle - 1;
    }
    else
    {
      first = middle + 1;
    }
  }
  return first;
}

bool CPersonalAgenda::unique_credentials(const string &email) const
{
  if (db_sorted_by_emails.size() == 0)
  {
    return true;
  }
  return binarySearchNewEmail(email) == -1 ? false : true;
}

bool CPersonalAgenda::unique_credentials(const string &name, const string &sur_name) const
{
  if (db_sorted_by_names.size() == 0)
  {
    return true;
  }
  return !binarySearchByName(db_sorted_by_names, name, sur_name);
}

// public methods
bool CPersonalAgenda::add(const string &name, const string &surname, const string &email, unsigned int salary)
{
  bool unique_pair = unique_credentials(name, surname);
  bool unique_email = unique_credentials(email);

  if (!unique_pair || !unique_email)
  {
    cout << boolalpha << unique_pair << unique_email << endl;
    return false;
  }

  if (db_sorted_by_names.size() == 0)
  {
    Employee emp(name, surname, email, salary);
    db_sorted_by_names.push_back(emp);
    db_sorted_by_emails.push_back(emp);
    return true;
  }

  auto it1 = db_sorted_by_names.begin();
  auto it2 = db_sorted_by_emails.begin();
  int posName = binarySearchNewName(name, surname);
  int posEmail = binarySearchNewEmail(email);

  Employee emp(name, surname, email, salary);
  db_sorted_by_names.insert(it1 + posName, emp);
  db_sorted_by_emails.insert(it2 + posEmail, emp);

  return true;
}

#ifndef __PROGTEST__
int main(void)
{
  string outName, outSurname;
  int lo, hi;

  CPersonalAgenda b1;
  assert(b1.add("John", "Smith", "john", 30000));
  assert(b1.add("John", "Miller", "johnm", 35000));
  assert(b1.add("Peter", "Smith", "peter", 23000));
  // assert(b1.getFirst(outName, outSurname) && outName == "John" && outSurname == "Miller");
  // assert(b1.getNext("John", "Miller", outName, outSurname) && outName == "John" && outSurname == "Smith");
  // assert(b1.getNext("John", "Smith", outName, outSurname) && outName == "Peter" && outSurname == "Smith");
  // assert(!b1.getNext("Peter", "Smith", outName, outSurname));
  // assert(b1.setSalary("john", 32000));
  // assert(b1.getSalary("john") == 32000);
  // assert(b1.getSalary("John", "Smith") == 32000);
  // assert(b1.getRank("John", "Smith", lo, hi) && lo == 1 && hi == 1);
  // assert(b1.getRank("john", lo, hi) && lo == 1 && hi == 1);
  // assert(b1.getRank("peter", lo, hi) && lo == 0 && hi == 0);
  // assert(b1.getRank("johnm", lo, hi) && lo == 2 && hi == 2);
  // assert(b1.setSalary("John", "Smith", 35000));
  // assert(b1.getSalary("John", "Smith") == 35000);
  // assert(b1.getSalary("john") == 35000);
  // assert(b1.getRank("John", "Smith", lo, hi) && lo == 1 && hi == 2);
  // assert(b1.getRank("john", lo, hi) && lo == 1 && hi == 2);
  // assert(b1.getRank("peter", lo, hi) && lo == 0 && hi == 0);
  // assert(b1.getRank("johnm", lo, hi) && lo == 1 && hi == 2);
  // assert(b1.changeName("peter", "James", "Bond"));
  // assert(b1.getSalary("peter") == 23000);
  // assert(b1.getSalary("James", "Bond") == 23000);
  // assert(b1.getSalary("Peter", "Smith") == 0);
  // assert(b1.getFirst(outName, outSurname) && outName == "James" && outSurname == "Bond");
  // assert(b1.getNext("James", "Bond", outName, outSurname) && outName == "John" && outSurname == "Miller");
  // assert(b1.getNext("John", "Miller", outName, outSurname) && outName == "John" && outSurname == "Smith");
  // assert(!b1.getNext("John", "Smith", outName, outSurname));
  // assert(b1.changeEmail("James", "Bond", "james"));
  // assert(b1.getSalary("James", "Bond") == 23000);
  // assert(b1.getSalary("james") == 23000);
  // assert(b1.getSalary("peter") == 0);
  // assert(b1.del("james"));
  // assert(b1.getRank("john", lo, hi) && lo == 0 && hi == 1);
  // assert(b1.del("John", "Miller"));
  // assert(b1.getRank("john", lo, hi) && lo == 0 && hi == 0);
  // assert(b1.getFirst(outName, outSurname) && outName == "John" && outSurname == "Smith");
  // assert(!b1.getNext("John", "Smith", outName, outSurname));
  // assert(b1.del("john"));
  // assert(!b1.getFirst(outName, outSurname));
  // assert(b1.add("John", "Smith", "john", 31000));
  // assert(b1.add("john", "Smith", "joHn", 31000));
  // assert(b1.add("John", "smith", "jOhn", 31000));

  // CPersonalAgenda b2;
  // assert(!b2.getFirst(outName, outSurname));
  // assert(b2.add("James", "Bond", "james", 70000));
  // assert(b2.add("James", "Smith", "james2", 30000));
  // assert(b2.add("Peter", "Smith", "peter", 40000));
  // assert(!b2.add("James", "Bond", "james3", 60000));
  // assert(!b2.add("Peter", "Bond", "peter", 50000));
  // assert(!b2.changeName("joe", "Joe", "Black"));
  // assert(!b2.changeEmail("Joe", "Black", "joe"));
  // assert(!b2.setSalary("Joe", "Black", 90000));
  // assert(!b2.setSalary("joe", 90000));
  // assert(b2.getSalary("Joe", "Black") == 0);
  // assert(b2.getSalary("joe") == 0);
  // assert(!b2.getRank("Joe", "Black", lo, hi));
  // assert(!b2.getRank("joe", lo, hi));
  // assert(!b2.changeName("joe", "Joe", "Black"));
  // assert(!b2.changeEmail("Joe", "Black", "joe"));
  // assert(!b2.del("Joe", "Black"));
  // assert(!b2.del("joe"));
  // assert(!b2.changeName("james2", "James", "Bond"));
  // assert(!b2.changeEmail("Peter", "Smith", "james"));
  // assert(!b2.changeName("peter", "Peter", "Smith"));
  // assert(!b2.changeEmail("Peter", "Smith", "peter"));
  // assert(b2.del("Peter", "Smith"));
  // assert(!b2.changeEmail("Peter", "Smith", "peter2"));
  // assert(!b2.setSalary("Peter", "Smith", 35000));
  // assert(b2.getSalary("Peter", "Smith") == 0);
  // assert(!b2.getRank("Peter", "Smith", lo, hi));
  // assert(!b2.changeName("peter", "Peter", "Falcon"));
  // assert(!b2.setSalary("peter", 37000));
  // assert(b2.getSalary("peter") == 0);
  // assert(!b2.getRank("peter", lo, hi));
  // assert(!b2.del("Peter", "Smith"));
  // assert(!b2.del("peter"));
  // assert(b2.add("Peter", "Smith", "peter", 40000));
  // assert(b2.getSalary("peter") == 40000);

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
