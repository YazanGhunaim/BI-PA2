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
  ~Employee(void) {}
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
  const string &get_name() const
  {
    return m_name;
  }
  const string &get_surname() const
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
  vector<Employee *> db_sorted_by_names;
  vector<Employee *> db_sorted_by_emails;
  vector<unsigned> db_sorted_by_salary;

public:
  CPersonalAgenda(void) {}

  ~CPersonalAgenda(void)
  {
    for (auto ptr : db_sorted_by_names)
    {
      delete ptr;
    }
  }

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

  friend std::ostream &operator<<(std::ostream &stream, const CPersonalAgenda &db);

private:
  int binarySearchNewName(const string &name, const string &surname) const;
  int binarySearchNewEmail(const string &email) const;
  int binarySearchNewSalary(unsigned salary) const;
  bool binarySearchByName(const string &name, const string &sur_name) const;
  bool binarySearchByEmail(const string &email) const;
  bool unique_credentials(const string &email) const;
  bool unique_credentials(const string &name, const string &sur_name) const;
};

// ---- private methods ---- //

// returns the appropriate index to insert a new employee to maintain a by name sort
int CPersonalAgenda::binarySearchNewName(const string &name, const string &surname) const
{
  Employee target{name, surname, "", 0};
  auto it = std::lower_bound(db_sorted_by_names.begin(), db_sorted_by_names.end(), &target, [](const Employee *a, const Employee *b)
                             { return std::tie(a->get_surname(), a->get_name()) < std::tie(b->get_surname(), b->get_name()); });
  return it - db_sorted_by_names.begin();
}

// returns the appropriate index to insert a new employee to maintain a by email sort
int CPersonalAgenda::binarySearchNewEmail(const string &email) const
{
  Employee target{"", "", email, 0};
  auto it = std::lower_bound(db_sorted_by_emails.begin(), db_sorted_by_emails.end(), &target, [](const Employee *a, const Employee *b)
                             { return a->get_email() < b->get_email(); });
  return it - db_sorted_by_emails.begin();
}

int CPersonalAgenda::binarySearchNewSalary(unsigned salary) const
{
  auto it = std::lower_bound(db_sorted_by_salary.begin(), db_sorted_by_salary.end(), salary, [](const unsigned &a, const unsigned &b)
                             { return a < b; });
  return it - db_sorted_by_salary.begin();
}

// Employee exists or not check
bool CPersonalAgenda::binarySearchByName(const string &name, const string &sur_name) const
{
  Employee target{name, sur_name, "", 0};
  return std::binary_search(db_sorted_by_names.begin(), db_sorted_by_names.end(), &target, [](const Employee *a, const Employee *b)
                            { return std::tie(a->get_surname(), a->get_name()) < std::tie(b->get_surname(), b->get_name()); });
}

// Employee exists or not check
bool CPersonalAgenda::binarySearchByEmail(const string &email) const
{
  Employee target{"", "", email, 0};
  return std::binary_search(db_sorted_by_emails.begin(), db_sorted_by_emails.end(), &target, [](const Employee *a, const Employee *b)
                            { return a->get_email() < b->get_email(); });
}

bool CPersonalAgenda::unique_credentials(const string &email) const
{
  if (db_sorted_by_emails.size() == 0)
  {
    return true;
  }
  return !binarySearchByEmail(email);
}

bool CPersonalAgenda::unique_credentials(const string &name, const string &sur_name) const
{
  if (db_sorted_by_names.size() == 0)
  {
    return true;
  }
  return !binarySearchByName(name, sur_name);
}

// ---- public methods ---- //

bool CPersonalAgenda::add(const string &name, const string &surname, const string &email, unsigned int salary)
{
  if (!unique_credentials(name, surname) || !unique_credentials(email))
  {
    return false;
  }

  Employee *emp = new Employee(name, surname, email, salary);
  if (db_sorted_by_names.size() == 0)
  {
    db_sorted_by_names.push_back(emp);
    db_sorted_by_emails.push_back(emp);
    db_sorted_by_salary.push_back(salary);
    return true;
  }

  auto it1 = db_sorted_by_names.begin();
  auto it2 = db_sorted_by_emails.begin();
  auto it3 = db_sorted_by_salary.begin();

  int pos_name = binarySearchNewName(name, surname);
  int pos_email = binarySearchNewEmail(email);
  int pos_salary = binarySearchNewSalary(salary);

  db_sorted_by_names.insert(it1 + pos_name, emp);
  db_sorted_by_emails.insert(it2 + pos_email, emp);
  db_sorted_by_salary.insert(it3 + pos_salary, salary);
  return true;
}

bool CPersonalAgenda::getFirst(string &outName, string &outSurname) const
{
  if (db_sorted_by_names.size() == 0)
  {
    return false;
  }
  outName = db_sorted_by_names[0]->get_name();
  outSurname = db_sorted_by_names[0]->get_surname();
  return true;
}

bool CPersonalAgenda::getNext(const string &name, const string &surname, string &outName, string &outSurname) const
{
  int target_index = binarySearchNewName(name, surname);
  if (!binarySearchByName(name, surname) || target_index == (int)db_sorted_by_names.size() - 1)
  {
    return false;
  }

  outName = db_sorted_by_names[target_index + 1]->get_name();
  outSurname = db_sorted_by_names[target_index + 1]->get_surname();
  return true;
}

bool CPersonalAgenda::setSalary(const string &name, const string &surname, unsigned int salary)
{
  if (!binarySearchByName(name, surname))
  {
    return false;
  }

  int target_index = binarySearchNewName(name, surname);
  int delete_index = binarySearchNewSalary(db_sorted_by_names[target_index]->get_salary());

  db_sorted_by_salary.erase(db_sorted_by_salary.begin() + delete_index);

  int insert_index = binarySearchNewSalary(salary);
  db_sorted_by_salary.insert(db_sorted_by_salary.begin() + insert_index, salary);
  db_sorted_by_names[target_index]->set_salary(salary);
  return true;
}

bool CPersonalAgenda::setSalary(const string &email, unsigned int salary)
{
  if (!binarySearchByEmail(email))
  {
    return false;
  }
  int target_index = binarySearchNewEmail(email);
  int delete_index = binarySearchNewSalary(db_sorted_by_emails[target_index]->get_salary());

  db_sorted_by_salary.erase(db_sorted_by_salary.begin() + delete_index);

  int insert_index = binarySearchNewSalary(salary);
  db_sorted_by_salary.insert(db_sorted_by_salary.begin() + insert_index, salary);
  db_sorted_by_emails[target_index]->set_salary(salary);
  return true;
}

unsigned int CPersonalAgenda::getSalary(const string &name, const string &surname) const
{
  if (!binarySearchByName(name, surname))
  {
    return 0;
  }
  int target_index = binarySearchNewName(name, surname);
  return db_sorted_by_names[target_index]->get_salary();
}

unsigned int CPersonalAgenda::getSalary(const string &email) const
{
  if (!binarySearchByEmail(email))
  {
    return 0;
  }
  int target_index = binarySearchNewEmail(email);
  return db_sorted_by_emails[target_index]->get_salary();
}

bool CPersonalAgenda::getRank(const string &name, const string &surname, int &rankMin, int &rankMax) const
{
  if (!binarySearchByName(name, surname))
  {
    return false;
  }

  int index = binarySearchNewName(name, surname);
  unsigned salary = db_sorted_by_names[index]->get_salary();

  int less_count = binarySearchNewSalary(salary);
  int equal_count = std::upper_bound(db_sorted_by_salary.begin(), db_sorted_by_salary.end(), salary, [](const unsigned &a, const unsigned &b)
                                     { return a < b; }) -
                    db_sorted_by_salary.begin();

  equal_count = equal_count - less_count;
  rankMin = less_count;
  rankMax = less_count + equal_count - 1;
  return true;
}

bool CPersonalAgenda::getRank(const string &email, int &rankMin, int &rankMax) const
{
  if (!binarySearchByEmail(email))
  {
    return false;
  }

  int index = binarySearchNewEmail(email);
  unsigned salary = db_sorted_by_emails[index]->get_salary();

  int less_count = binarySearchNewSalary(salary);
  int equal_count = std::upper_bound(db_sorted_by_salary.begin(), db_sorted_by_salary.end(), salary, [](const unsigned &a, const unsigned &b)
                                     { return a < b; }) -
                    db_sorted_by_salary.begin();

  equal_count = equal_count - less_count;
  rankMin = less_count;
  rankMax = less_count + equal_count - 1;
  return true;
}

bool CPersonalAgenda::del(const string &name, const string &surname)
{
  if (!binarySearchByName(name, surname))
  {
    return false;
  }

  int index = binarySearchNewName(name, surname);
  int index_emails = binarySearchNewEmail(db_sorted_by_names[index]->get_email());
  int index_salary = binarySearchNewSalary(db_sorted_by_names[index]->get_salary());

  delete db_sorted_by_names[index];

  db_sorted_by_names.erase(db_sorted_by_names.begin() + index);
  db_sorted_by_emails.erase(db_sorted_by_emails.begin() + index_emails);
  db_sorted_by_salary.erase(db_sorted_by_salary.begin() + index_salary);
  return true;
}

bool CPersonalAgenda::del(const string &email)
{
  if (!binarySearchByEmail(email))
  {
    return false;
  }

  int index = binarySearchNewEmail(email);
  int index_names = binarySearchNewName(db_sorted_by_emails[index]->get_name(), db_sorted_by_emails[index]->get_surname());
  int index_salary = binarySearchNewSalary(db_sorted_by_emails[index]->get_salary());

  delete db_sorted_by_emails[index];

  db_sorted_by_emails.erase(db_sorted_by_emails.begin() + index);
  db_sorted_by_names.erase(db_sorted_by_names.begin() + index_names);
  db_sorted_by_salary.erase(db_sorted_by_salary.begin() + index_salary);
  return true;
}

bool CPersonalAgenda::changeName(const string &email, const string &newName, const string &newSurname)
{
  if (!unique_credentials(newName, newSurname) || !binarySearchByEmail(email))
  {
    return false;
  }

  int index = binarySearchNewEmail(email);
  db_sorted_by_emails[index]->change_name(newName, newSurname);
  sort(db_sorted_by_names.begin(), db_sorted_by_names.end(), [](const Employee *a, const Employee *b)
       { return std::tie(a->get_surname(), a->get_name()) < std::tie(b->get_surname(), b->get_name()); });
  return true;
}

bool CPersonalAgenda::changeEmail(const string &name, const string &surname, const string &newEmail)
{
  if (!unique_credentials(newEmail) || !binarySearchByName(name, surname))
  {
    return false;
  }

  int target_index = binarySearchNewName(name, surname);
  db_sorted_by_names[target_index]->change_email(newEmail);
  sort(db_sorted_by_emails.begin(), db_sorted_by_emails.end(), [](const Employee *a, const Employee *b)
       { return a->get_email() < b->get_email(); });
  return true;
}

// ---- operator overloading ---- //
std::ostream &
operator<<(std::ostream &stream, const CPersonalAgenda &db)
{
  stream << "-------NAME-SORT--------\n";
  for (const auto &x : db.db_sorted_by_names)
  {
    stream << x->get_name() << " " << x->get_surname() << " " << x->get_email() << " " << x->get_salary() << "\n";
  }
  stream << "-------EMAIL-SORT--------\n";
  for (const auto &x : db.db_sorted_by_emails)
  {
    stream << x->get_name() << " " << x->get_surname() << " " << x->get_email() << " " << x->get_salary() << "\n";
  }
  stream << "-------SALARY-SORT--------\n";
  for (const auto &x : db.db_sorted_by_salary)
  {
    stream << x << "\n";
  }
  stream << endl;
  return stream;
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
  assert(b1.getFirst(outName, outSurname) && outName == "John" && outSurname == "Miller");
  assert(b1.getNext("John", "Miller", outName, outSurname) && outName == "John" && outSurname == "Smith");
  assert(b1.getNext("John", "Smith", outName, outSurname) && outName == "Peter" && outSurname == "Smith");
  assert(!b1.getNext("Peter", "Smith", outName, outSurname));
  assert(b1.setSalary("john", 32000));
  assert(b1.getSalary("john") == 32000);
  assert(b1.getSalary("John", "Smith") == 32000);
  assert(b1.getRank("John", "Smith", lo, hi) && lo == 1 && hi == 1);
  assert(b1.getRank("john", lo, hi) && lo == 1 && hi == 1);
  assert(b1.getRank("peter", lo, hi) && lo == 0 && hi == 0);
  assert(b1.getRank("johnm", lo, hi) && lo == 2 && hi == 2);
  assert(b1.setSalary("John", "Smith", 35000));
  assert(b1.getSalary("John", "Smith") == 35000);
  assert(b1.getSalary("john") == 35000);
  assert(b1.getRank("John", "Smith", lo, hi) && lo == 1 && hi == 2);
  assert(b1.getRank("john", lo, hi) && lo == 1 && hi == 2);
  assert(b1.getRank("peter", lo, hi) && lo == 0 && hi == 0);
  assert(b1.getRank("johnm", lo, hi) && lo == 1 && hi == 2);
  assert(b1.changeName("peter", "James", "Bond"));
  assert(b1.getSalary("peter") == 23000);
  assert(b1.getSalary("James", "Bond") == 23000);
  assert(b1.getSalary("Peter", "Smith") == 0);
  assert(b1.getFirst(outName, outSurname) && outName == "James" && outSurname == "Bond");
  assert(b1.getNext("James", "Bond", outName, outSurname) && outName == "John" && outSurname == "Miller");
  assert(b1.getNext("John", "Miller", outName, outSurname) && outName == "John" && outSurname == "Smith");
  assert(!b1.getNext("John", "Smith", outName, outSurname));
  assert(b1.changeEmail("James", "Bond", "james"));
  assert(b1.getSalary("James", "Bond") == 23000);
  assert(b1.getSalary("james") == 23000);
  assert(b1.getSalary("peter") == 0);
  assert(b1.del("james"));
  assert(b1.getRank("john", lo, hi) && lo == 0 && hi == 1);
  assert(b1.del("John", "Miller"));
  assert(b1.getRank("john", lo, hi) && lo == 0 && hi == 0);
  assert(b1.getFirst(outName, outSurname) && outName == "John" && outSurname == "Smith");
  assert(!b1.getNext("John", "Smith", outName, outSurname));
  assert(b1.del("john"));
  assert(!b1.getFirst(outName, outSurname));
  assert(b1.add("John", "Smith", "john", 31000));
  assert(b1.add("john", "Smith", "joHn", 31000));
  assert(b1.add("John", "smith", "jOhn", 31000));

  CPersonalAgenda b2;
  assert(!b2.getFirst(outName, outSurname));
  assert(b2.add("James", "Bond", "james", 70000));
  assert(b2.add("James", "Smith", "james2", 30000));
  assert(b2.add("Peter", "Smith", "peter", 40000));
  assert(!b2.add("James", "Bond", "james3", 60000));
  assert(!b2.add("Peter", "Bond", "peter", 50000));
  assert(!b2.changeName("joe", "Joe", "Black"));
  assert(!b2.changeEmail("Joe", "Black", "joe"));
  assert(!b2.setSalary("Joe", "Black", 90000));
  assert(!b2.setSalary("joe", 90000));
  assert(b2.getSalary("Joe", "Black") == 0);
  assert(b2.getSalary("joe") == 0);
  assert(!b2.getRank("Joe", "Black", lo, hi));
  assert(!b2.getRank("joe", lo, hi));
  assert(!b2.changeName("joe", "Joe", "Black"));
  assert(!b2.changeEmail("Joe", "Black", "joe"));
  assert(!b2.del("Joe", "Black"));
  assert(!b2.del("joe"));
  assert(!b2.changeName("james2", "James", "Bond"));
  assert(!b2.changeEmail("Peter", "Smith", "james"));
  assert(!b2.changeName("peter", "Peter", "Smith"));
  assert(!b2.changeEmail("Peter", "Smith", "peter"));
  assert(b2.del("Peter", "Smith"));
  assert(!b2.changeEmail("Peter", "Smith", "peter2"));
  assert(!b2.setSalary("Peter", "Smith", 35000));
  assert(b2.getSalary("Peter", "Smith") == 0);
  assert(!b2.getRank("Peter", "Smith", lo, hi));
  assert(!b2.changeName("peter", "Peter", "Falcon"));
  assert(!b2.setSalary("peter", 37000));
  assert(b2.getSalary("peter") == 0);
  assert(!b2.getRank("peter", lo, hi));
  assert(!b2.del("Peter", "Smith"));
  assert(!b2.del("peter"));
  assert(b2.add("Peter", "Smith", "peter", 40000));
  assert(b2.getSalary("peter") == 40000);

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
