#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cstdint>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <functional>
#include <compare>
#include <stdexcept>
#endif /* __PROGTEST__ */

using namespace std;

class CBigInt
{
public:
  bool neg = false;
  std::string number = "0";

  CBigInt() {}

  CBigInt(const CBigInt &other)
  {
    this->neg = other.neg;
    this->number = other.number;
  }

  CBigInt &operator=(const CBigInt &other)
  {
    if (this != &other)
    {
      this->neg = other.neg;
      this->number = other.number;
    }
    return *this;
  }

  CBigInt(int number)
  {
    if (number < 0)
    {
      this->neg = true;
      number *= -1;
    }

    if (number == 0)
    {
      this->number = "0";
      return;
    }

    this->number = "";

    while (number > 0)
    {
      this->number += (number % 10) + '0';
      number /= 10;
    }
  }

  CBigInt(const char *number)
  {
    std::string number_(number);
    *(this) = number_;
  }

  CBigInt(std::string number)
  {
    if (!isValidNumber(number))
    {
      throw invalid_argument("Invalid number.");
    }

    while (number[0] == '-')
    {
      this->neg = !this->neg;
      number.erase(0, 1);
    }

    reverse(number.begin(), number.end());

    while (number.size() > 1 && number[number.size() - 1] == '0')
      number.erase(number.size() - 1, number.size());

    if (number == "")
      number = "0";

    this->number = number;

    if (number == "0")
      this->neg = false;
  }

  friend bool operator==(CBigInt first, CBigInt second)
  {
    return first.number == second.number && second.neg == first.neg;
  }

  friend bool operator!=(CBigInt first, CBigInt second)
  {
    return !(first == second);
  }

  friend bool operator<(CBigInt first, CBigInt second)
  {
    if (first.neg && !second.neg)
      return true;
    if (!first.neg && second.neg)
      return false;

    if (first.neg && second.neg)
    {
      first.neg = false;
      second.neg = false;

      return first > second;
    }

    if ((first.number).size() < (second.number).size())
      return true;
    if ((first.number).size() > (second.number).size())
      return false;

    std::string temp1 = first.number;
    reverse(temp1.begin(), temp1.end());
    std::string temp2 = second.number;
    reverse(temp2.begin(), temp2.end());

    for (unsigned int i = 0; i < temp1.size(); i++)
      if (temp1[i] < temp2[i])
        return true;
      else if (temp1[i] > temp2[i])
        return false;

    return false;
  }

  friend bool operator<=(CBigInt first, CBigInt second)
  {
    return (first == second) || (first < second);
  }

  friend bool operator>(CBigInt first, CBigInt second)
  {
    if (!first.neg && second.neg)
      return true;
    if (first.neg && !second.neg)
      return false;

    if (first.neg && second.neg)
    {
      first.neg = false;
      second.neg = false;

      return first < second;
    }

    if ((first.number).size() > (second.number).size())
      return true;
    if ((first.number).size() < (second.number).size())
      return false;

    std::string temp1 = first.number;
    reverse(temp1.begin(), temp1.end());
    std::string temp2 = second.number;
    reverse(temp2.begin(), temp2.end());

    for (unsigned int i = 0; i < temp1.size(); i++)
      if (temp1[i] > temp2[i])
        return true;
      else if (temp1[i] < temp2[i])
        return false;

    return false;
  }

  friend bool operator>=(CBigInt first, CBigInt second)
  {
    return (first == second) || (first > second);
  }

  bool isValidNumber(const string &val)
  {
    // Check if the string represents a valid decimal number
    if (val.empty())
      return false;

    size_t pos = 0;

    // Skip sign if present
    if (val[pos] == '-' || val[pos] == '+')
      pos++;

    // Empty string after sign is invalid
    if (pos >= val.size())
      return false;

    // Check each character
    for (; pos < val.size(); pos++)
    {
      if (!std::isdigit(val[pos]))
        return false;
    }

    return true;
  }

  friend std::istream &operator>>(std::istream &in, CBigInt &CBigInt)
  {
    std::string number;
    in >> number;

    CBigInt.neg = false;

    // while (number[0] == '-')
    // {
    //   CBigInt.neg = !CBigInt.neg;
    //   number.erase(0, 1);
    // }

    bool first = true;
    for (auto it = number.begin(); it != number.end();)
    {
      if (!std::isdigit(*it) && first)
      {
        in.setstate(ios::failbit);
        return in;
      }
      else if (!std::isdigit(*it))
      {
        it = number.erase(it);
      }
      else
      {
        ++it;
      }

      first = false;
    }

    reverse(number.begin(), number.end());

    while (number.size() > 1 && number[number.size() - 1] == '0')
    {
      number.erase(number.size() - 1, number.size());
    }

    CBigInt.number = number;

    if (number == "0")
      CBigInt.neg = false;

    return in;
  }

  friend std::ostream &operator<<(std::ostream &out, const CBigInt &CBigInt)
  {
    std::string number = CBigInt.number;
    reverse(number.begin(), number.end());

    if (CBigInt.neg && CBigInt.number != "0")
      number = '-' + number;

    out << number;

    return out;
  }

  friend void swap(CBigInt &first, CBigInt &second)
  {
    CBigInt temp(first);

    first = second;
    second = temp;
  }

  friend CBigInt abs(CBigInt CBigInt)
  {
    CBigInt.neg = false;

    return CBigInt;
  }

  friend CBigInt operator+(CBigInt first, CBigInt second)
  {
    bool neg = false;

    if (!first.neg && second.neg)
    {
      second.neg = false;
      return first - second;
    }

    if (first.neg && !second.neg)
    {
      first.neg = false;
      return second - first;
    }

    if (first.neg && second.neg)
    {
      neg = true;
      first.neg = second.neg = false;
    }

    int n = first.number.size();
    int m = second.number.size();

    int carry = 0;

    std::string result;

    for (int i = 0; i < std::max(n, m); i++)
    {
      int add = carry;

      if (i < n)
        add += first.number[i] - '0';
      if (i < m)
        add += second.number[i] - '0';

      carry = add / 10;

      result += add % 10 + '0';
    }

    if (carry != 0)
      result += carry + '0';

    reverse(result.begin(), result.end());

    CBigInt result_(result);
    result_.neg = neg;

    return result_;
  }

  friend CBigInt operator+(CBigInt CBigInt)
  {
    return CBigInt;
  }

  friend CBigInt operator-(CBigInt first, CBigInt second)
  {
    if (second.neg)
    {
      second.neg = false;
      return first + second;
    }

    if (first.neg)
    {
      second.neg = true;
      return first + second;
    }

    bool neg = false;

    if (first < abs(second))
    {
      neg = true;

      swap(first, second);
      first = abs(first);
    }

    int n = first.number.size();
    int m = second.number.size();

    int carry = 0;

    std::string result;

    for (int i = 0; i < std::max(n, m); i++)
    {
      int add = carry;

      if (i < n)
        add += first.number[i] - '0';
      if (i < m)
        add -= second.number[i] - '0';

      if (add < 0)
      {
        carry = -1;
        result += add + 10 + '0';
      }

      else
      {
        carry = 0;
        result += add + '0';
      }
    }

    reverse(result.begin(), result.end());

    CBigInt result_(result);

    result_.neg = neg;

    return result_;
  }

  friend CBigInt operator-(CBigInt second)
  {
    CBigInt first("0");
    return first - second;
  }

  friend CBigInt operator*(CBigInt first, CBigInt second)
  {
    bool neg = first.neg != second.neg;

    first.neg = false;
    second.neg = false;

    int n = first.number.size();
    int m = second.number.size();

    CBigInt result_;

    for (int i = 0; i < n; i++)
    {
      int carry = 0;

      std::string result;

      for (int j = 0; j < i; j++)
        result += '0';

      for (int j = 0; j < m; j++)
      {
        int add = carry + (first.number[i] - '0') * (second.number[j] - '0');

        carry = add / 10;

        result += add % 10 + '0';
      }

      if (carry != 0)
        result += carry + '0';

      reverse(result.begin(), result.end());

      CBigInt current(result);

      result_ += current;
    }

    result_.neg = neg;

    return result_;
  }

  int size()
  {
    return this->number.size();
  }

  void operator+=(CBigInt CBigInt) { *(this) = *(this) + CBigInt; }
  void operator-=(CBigInt CBigInt) { *(this) = *(this) - CBigInt; }

  void operator*=(CBigInt CBigInt) { *(this) = *(this) * CBigInt; }
};

#ifndef __PROGTEST__
static bool equal(const CBigInt &x, const char val[])
{
  std::ostringstream oss;
  oss << x;
  return oss.str() == val;
}
static bool equalHex(const CBigInt &x, const char val[])
{
  return true; // hex output is needed for bonus tests only
  // std::ostringstream oss;
  // oss << std::hex << x;
  // return oss . str () == val;
}
int main()
{
  CBigInt a, b;
  std::istringstream is;
  a = 10;
  a += 20;
  assert(equal(a, "30"));
  a *= 5;
  assert(equal(a, "150"));
  b = a + 3;
  assert(equal(b, "153"));
  b = a * 7;
  assert(equal(b, "1050"));
  assert(equal(a, "150"));
  assert(equalHex(a, "96"));

  a = 10;
  a += -20;
  assert(equal(a, "-10"));
  a *= 5;
  assert(equal(a, "-50"));
  b = a + 73;
  assert(equal(b, "23"));
  b = a * -7;
  assert(equal(b, "350"));
  assert(equal(a, "-50"));
  assert(equalHex(a, "-32"));

  a = "12345678901234567890";
  a += "-99999999999999999999";
  assert(equal(a, "-87654321098765432109"));
  a *= "54321987654321987654";
  assert(equal(a, "-4761556948575111126880627366067073182286"));
  a *= 0;
  assert(equal(a, "0"));
  a = 10;
  b = a + "400";
  assert(equal(b, "410"));
  b = a * "15";
  assert(equal(b, "150"));
  assert(equal(a, "10"));
  assert(equalHex(a, "a"));

  is.clear();
  is.str(" 1234");
  assert(is >> b);
  assert(equal(b, "1234"));
  is.clear();
  is.str(" 12 34");
  assert(is >> b);
  assert(equal(b, "12"));
  is.clear();
  is.str("999z");
  assert(is >> b);
  assert(equal(b, "999"));
  is.clear();
  is.str("abcd");
  assert(!(is >> b));
  is.clear();
  is.str("- 758");
  assert(!(is >> b));
  a = 42;
  try
  {
    a = "-xyz";
    assert("missing an exception" == nullptr);
  }
  catch (const std::invalid_argument &e)
  {
    assert(equal(a, "42"));
  }

  a = "73786976294838206464";
  assert(equal(a, "73786976294838206464"));
  assert(equalHex(a, "40000000000000000"));
  assert(a < "1361129467683753853853498429727072845824");
  assert(a <= "1361129467683753853853498429727072845824");
  assert(!(a > "1361129467683753853853498429727072845824"));
  assert(!(a >= "1361129467683753853853498429727072845824"));
  assert(!(a == "1361129467683753853853498429727072845824"));
  assert(a != "1361129467683753853853498429727072845824");
  assert(!(a < "73786976294838206464"));
  assert(a <= "73786976294838206464");
  assert(!(a > "73786976294838206464"));
  assert(a >= "73786976294838206464");
  assert(a == "73786976294838206464");
  assert(!(a != "73786976294838206464"));
  assert(a < "73786976294838206465");
  assert(a <= "73786976294838206465");
  assert(!(a > "73786976294838206465"));
  assert(!(a >= "73786976294838206465"));
  assert(!(a == "73786976294838206465"));
  assert(a != "73786976294838206465");
  a = "2147483648";
  assert(!(a < -2147483648));
  assert(!(a <= -2147483648));
  assert(a > -2147483648);
  assert(a >= -2147483648);
  assert(!(a == -2147483648));
  assert(a != -2147483648);
  a = "-12345678";
  assert(!(a < -87654321));
  assert(!(a <= -87654321));
  assert(a > -87654321);
  assert(a >= -87654321);
  assert(!(a == -87654321));
  assert(a != -87654321);

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
