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

class record
{
private:
  string name;
  string sur_name;
  string phone_number;

public:
  record(string Name, string SurName, string Number) : name(move(Name)), sur_name(move(SurName)), phone_number(move(Number))
  {
  }
  string getName() const
  {
    return name;
  }
  string getSurname() const
  {
    return sur_name;
  }
  // printing format for one record
  string toString() const;
  bool validityCheck() const;
};

string record::toString() const
{
  ostringstream oss;
  oss << name << " " << sur_name << " " << phone_number;
  return oss.str();
}

bool record::validityCheck() const
{
  if (name.empty() || sur_name.empty() || phone_number.empty() || phone_number.length() != 9 || phone_number[0] == '0')
  {
    return false;
  }
  return true;
}

// displaying all results to the output stream in the correct format
void outputToStream(const string &file_name, ostream &out, vector<string> &targets, vector<record> &records)
{
  int found = 0;
  for (const auto &x : targets)
  {
    int count = 0;
    for (const auto &y : records)
    {
      if (y.getName() == x || y.getSurname() == x)
      {
        found = 1;
        count++;
        out << y.toString() << "\n";
      }
    }
    if (found)
    {
      out << "-> " << count << "\n";
    }
  }
}

bool report(const string &fileName, ostream &out)
{
  // correct format would be -> Name Surname ######### (9 digit phonenumber only digits and doesnt start at 0) \n
  ifstream myFile(fileName);
  vector<record> vec;
  vector<string> queries;
  if (myFile.is_open())
  {
    string line;
    while (getline(myFile, line))
    {
      if (line.empty())
      {
        ifstream queryFile(fileName); // create a new ifstream object to read queries
        while (getline(queryFile, line))
        {
          queries.push_back(line);
        }
        outputToStream(fileName, out, queries, vec);
        return true;
      }
      else
      {
        istringstream ss(line);
        string name, sur_name;
        string phone_number;

        ss >> name >> sur_name >> phone_number;
        record temp(name, sur_name, phone_number);
        if (!temp.validityCheck() || phone_number[9] || stoi(phone_number) < 0)
        {
          return false;
        }
        vec.push_back(temp);
      }
    }
  }

  return false;
}

#ifndef __PROGTEST__
int main()
{
  ostringstream oss;
  oss.str("");
  assert(report("tests/test0_in.txt", oss) == true);
  assert(oss.str() ==
         "John Christescu 258452362\n"
         "John Harmson 861647702\n"
         "-> 2\n"
         "-> 0\n"
         "Josh Dakhov 264112084\n"
         "Dakhov Speechley 865216101\n"
         "-> 2\n"
         "John Harmson 861647702\n"
         "-> 1\n");
  oss.str("");
  assert(report("tests/test1_in.txt", oss) == false);
  oss.str("");
  assert(report("tests/test2_in.txt", oss) == true);
  oss.str("");
  assert(report("tests/test3_in.txt", oss) == false);
  oss.str("");
  assert(report("tests/test4_in.txt", oss) == false);
  return 0;
}
#endif /* __PROGTEST__ */
