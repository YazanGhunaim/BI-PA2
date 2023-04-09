#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <deque>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <iterator>
#include <functional>
using namespace std;

class CDate
{
public:
  //---------------------------------------------------------------------------------------------
  CDate(int y, int m, int d) : m_Year(y), m_Month(m), m_Day(d)
  {
  }
  //---------------------------------------------------------------------------------------------
  int compare(const CDate &x) const
  {
    if (m_Year != x.m_Year)
      return m_Year - x.m_Year;
    if (m_Month != x.m_Month)
      return m_Month - x.m_Month;
    return m_Day - x.m_Day;
  }
  //---------------------------------------------------------------------------------------------
  int year(void) const
  {
    return m_Year;
  }
  //---------------------------------------------------------------------------------------------
  int month(void) const
  {
    return m_Month;
  }
  //---------------------------------------------------------------------------------------------
  int day(void) const
  {
    return m_Day;
  }
  //---------------------------------------------------------------------------------------------
  friend ostream &operator<<(ostream &os, const CDate &x)
  {
    char oldFill = os.fill();
    return os << setfill('0') << setw(4) << x.m_Year << "-"
              << setw(2) << static_cast<int>(x.m_Month) << "-"
              << setw(2) << static_cast<int>(x.m_Day)
              << setfill(oldFill);
  }
  //---------------------------------------------------------------------------------------------
private:
  int16_t m_Year;
  int8_t m_Month;
  int8_t m_Day;
};
#endif /* __PROGTEST__ */

class CInvoice
{
private:
  struct invoice
  {
    CDate m_date;
    string m_seller;
    string m_buyer;
    unsigned m_amout;
    double m_vat;
  };
  invoice m_invoice;

public:
  CInvoice(const CDate &date, const string &seller, const string &buyer, unsigned int amount, double vat) : m_invoice{date, seller, buyer, amount, vat} {}
  CDate date(void) const { return m_invoice.m_date; }
  string buyer(void) const { return m_invoice.m_buyer; }
  string seller(void) const { return m_invoice.m_seller; }
  unsigned int amount(void) const { return m_invoice.m_amout; }
  double vat(void) const { return m_invoice.m_vat; }
  bool operator<(const CInvoice &other) const
  {
    if (m_invoice.m_seller < other.seller())
    {
      return true;
    }
    else if (m_invoice.m_buyer < other.buyer())
    {
      return true;
    }
    else if (m_invoice.m_amout < other.amount())
    {
      return true;
    }
    else if (m_invoice.m_vat < other.vat())
    {
      return true;
    }
    return m_invoice.m_date.compare(other.date());
  }
  bool operator==(const CInvoice &other) const
  {
    if (m_invoice.m_date.compare(other.date()) != 0)
    {
      return false;
    }
    return m_invoice.m_seller == other.seller() && m_invoice.m_buyer == other.buyer() && m_invoice.m_amout == other.amount() && m_invoice.m_vat == other.vat();
  }
};

class CSortOpt
{
private:
  std::deque<std::pair<int, bool>> m_sortCriteria;

public:
  static const int BY_DATE = 0;
  static const int BY_BUYER = 1;
  static const int BY_SELLER = 2;
  static const int BY_AMOUNT = 3;
  static const int BY_VAT = 4;
  CSortOpt(void){};
  CSortOpt &addKey(int key, bool ascending = true);
  std::deque<std::pair<int, bool>> get_keys() const { return m_sortCriteria; }
};

/* ============ Public Methods ============ */
CSortOpt &CSortOpt::addKey(int key, bool ascending)
{
  m_sortCriteria.push_back(std::make_pair(key, ascending));
  return *this;
}

class CVATRegister
{
private:
  // private methods
  static string remove_space(const string &str)
  {
    int n = str.length();
    int i = 0;
    int j = n - 1;

    while (i < n && str[i] == ' ')
      ++i;
    while (j >= 0 && str[j] == ' ')
      --j;

    string result = "";
    bool space = false;
    for (int k = i; k <= j; k++)
    {
      if (str[k] == ' ')
      {
        if (!space)
        {
          result += ' ';
          space = true;
        }
      }
      else
      {
        result += str[k];
        space = false;
      }
    }

    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
  }
  bool addInvoiceCheck(const CInvoice &x) const
  {
    string seller = remove_space(x.seller());
    string buyer = remove_space(x.buyer());
    if (m_issuedInvoiceMap.find(buyer) == m_issuedInvoiceMap.end() || m_issuedInvoiceMap.find(seller) == m_issuedInvoiceMap.end() || seller == buyer)
    {
      return false;
    }
    return m_issuedInvoiceMap.find(seller)->second.find(x) == m_issuedInvoiceMap.find(seller)->second.end();
  }

  // custom hash and equal functors
  struct CaseInsensitiveHash
  {
    size_t operator()(const std::string &str) const
    {
      std::string strModified = remove_space(str);
      size_t hash = 0;
      for (char c : strModified)
      {
        hash = hash * 31 + c;
      }
      return hash;
    }
  };
  struct CaseInsensitiveEqual
  {
    bool operator()(const std::string &str1, const std::string &str2) const
    {
      string str1Modified = remove_space(str1);
      string str2Modified = remove_space(str2);
      return str1Modified.size() == str2Modified.size() && str1Modified == str2Modified;
    }
  };

  // member variables
  std::unordered_map<string, std::set<CInvoice>, CaseInsensitiveHash, CaseInsensitiveEqual> m_issuedInvoiceMap;
  std::unordered_map<string, std::set<CInvoice>, CaseInsensitiveHash, CaseInsensitiveEqual> m_acceptedInvoiceMap;

public:
  CVATRegister(void)
  {
  }
  bool registerCompany(const string &name);
  bool addIssued(const CInvoice &x);
  bool addAccepted(const CInvoice &x);
  bool delIssued(const CInvoice &x);
  bool delAccepted(const CInvoice &x);
  list<CInvoice> unmatched(const string &company, const CSortOpt &sortBy) const;
  friend std::ostream &operator<<(std::ostream &os, const CVATRegister &x);
};

/* ============ Public Methods ============ */
bool CVATRegister::registerCompany(const string &name)
{
  auto res = m_issuedInvoiceMap.emplace(name, std::set<CInvoice>{});
  return res.second;
}

bool CVATRegister::addIssued(const CInvoice &x)
{
  if (!addInvoiceCheck(x))
  {
    return false;
  }
  m_issuedInvoiceMap[x.seller()].insert(x);
  return true;
}

bool CVATRegister::addAccepted(const CInvoice &x)
{
  if (!addInvoiceCheck(x))
  {
    return false;
  }
  m_acceptedInvoiceMap[x.buyer()].insert(x);
  return true;
}

/* ============ Private Methods ============ */

/* ============ Friend Methods ============ */
std::ostream &
operator<<(std::ostream &os, const CVATRegister &x)
{
  cout << "issued" << endl;
  for (const auto &i : x.m_issuedInvoiceMap)
  {
    os << i.first << " : ";
    for (const auto &s : i.second)
    {
      os << s.date() << " " << s.seller() << " " << s.buyer() << " " << s.amount() << " " << s.vat() << endl;
    }
    os << endl;
  }
  cout << "accepted" << endl;
  for (const auto &i : x.m_acceptedInvoiceMap)
  {
    os << i.first << " : ";
    for (const auto &s : i.second)
    {
      os << s.date() << " " << s.seller() << " " << s.buyer() << " " << s.amount() << " " << s.vat() << endl;
    }
    os << endl;
  }
  return os;
}
#ifndef __PROGTEST__
bool equalLists(const list<CInvoice> &a, const list<CInvoice> &b)
{
  if (a.size() != b.size())
  {
    return false;
  }

  auto it1 = a.begin();
  auto it2 = b.begin();
  while (it1 != a.end())
  {
    if (it1->date().compare(it2->date()) != 0 || it1->amount() != it2->amount() || it1->buyer() != it2->buyer() || it1->seller() != it2->seller() || it1->vat() != it2->vat())
    {
      return false;
    }
  }
  return true;
}

int main(void)
{
  CVATRegister r;
  assert(r.registerCompany("first Company"));
  assert(r.registerCompany("Second     Company"));
  assert(r.registerCompany("ThirdCompany, Ltd."));
  assert(r.registerCompany("Third Company, Ltd."));
  assert(!r.registerCompany("Third Company, Ltd."));
  assert(!r.registerCompany(" Third  Company,  Ltd.  "));
  assert(r.addIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company ", 100, 20)));
  assert(r.addIssued(CInvoice(CDate(2000, 1, 2), "FirSt Company", "Second Company ", 200, 30)));
  assert(r.addIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company ", 100, 30)));
  assert(r.addIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company ", 300, 30)));
  assert(r.addIssued(CInvoice(CDate(2000, 1, 1), "First Company", " Third  Company,  Ltd.   ", 200, 30)));
  assert(r.addIssued(CInvoice(CDate(2000, 1, 1), "Second Company ", "First Company", 300, 30)));
  assert(r.addIssued(CInvoice(CDate(2000, 1, 1), "Third  Company,  Ltd.", "  Second    COMPANY ", 400, 34)));
  assert(!r.addIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company ", 300, 30)));
  assert(!r.addIssued(CInvoice(CDate(2000, 1, 4), "First Company", "First   Company", 200, 30)));
  assert(!r.addIssued(CInvoice(CDate(2000, 1, 4), "Another Company", "First   Company", 200, 30)));
  // assert(equalLists(r.unmatched("First Company", CSortOpt().addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER, false).addKey(CSortOpt::BY_DATE, false)),
  //                   list<CInvoice>{
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Third Company, Ltd.", 200, 30.000000),
  //                       CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 20.000000),
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 30.000000)}));
  // assert(equalLists(r.unmatched("First Company", CSortOpt().addKey(CSortOpt::BY_DATE, true).addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER, true)),
  //                   list<CInvoice>{
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 20.000000),
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Third Company, Ltd.", 200, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 30.000000),
  //                       CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200, 30.000000)}));
  // assert(equalLists(r.unmatched("First Company", CSortOpt().addKey(CSortOpt::BY_VAT, true).addKey(CSortOpt::BY_AMOUNT, true).addKey(CSortOpt::BY_DATE, true).addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER, true)),
  //                   list<CInvoice>{
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 20.000000),
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Third Company, Ltd.", 200, 30.000000),
  //                       CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 30.000000)}));
  // assert(equalLists(r.unmatched("First Company", CSortOpt()),
  //                   list<CInvoice>{
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 20.000000),
  //                       CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Third Company, Ltd.", 200, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 30.000000)}));
  // assert(equalLists(r.unmatched("second company", CSortOpt().addKey(CSortOpt::BY_DATE, false)),
  //                   list<CInvoice>{
  //                       CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 20.000000),
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "Third Company, Ltd.", "Second     Company", 400, 34.000000)}));
  // assert(equalLists(r.unmatched("last company", CSortOpt().addKey(CSortOpt::BY_VAT, true)),
  //                   list<CInvoice>{}));
  // assert(r.addAccepted(CInvoice(CDate(2000, 1, 2), "First Company", "Second Company ", 200, 30)));
  // assert(r.addAccepted(CInvoice(CDate(2000, 1, 1), "First Company", " Third  Company,  Ltd.   ", 200, 30)));
  // assert(r.addAccepted(CInvoice(CDate(2000, 1, 1), "Second company ", "First Company", 300, 32)));
  // assert(equalLists(r.unmatched("First Company", CSortOpt().addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER, true).addKey(CSortOpt::BY_DATE, true)),
  //                   list<CInvoice>{
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 20.000000),
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 32.000000)}));
  // assert(!r.delIssued(CInvoice(CDate(2001, 1, 1), "First Company", "Second Company ", 200, 30)));
  // assert(!r.delIssued(CInvoice(CDate(2000, 1, 1), "A First Company", "Second Company ", 200, 30)));
  // assert(!r.delIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Hand", 200, 30)));
  // assert(!r.delIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company", 1200, 30)));
  // assert(!r.delIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company", 200, 130)));
  // assert(r.delIssued(CInvoice(CDate(2000, 1, 2), "First Company", "Second Company", 200, 30)));
  // assert(equalLists(r.unmatched("First Company", CSortOpt().addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER, true).addKey(CSortOpt::BY_DATE, true)),
  //                   list<CInvoice>{
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 20.000000),
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300, 30.000000),
  //                       CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 32.000000)}));
  // assert(r.delAccepted(CInvoice(CDate(2000, 1, 1), "First Company", " Third  Company,  Ltd.   ", 200, 30)));
  // assert(equalLists(r.unmatched("First Company", CSortOpt().addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER, true).addKey(CSortOpt::BY_DATE, true)),
  //                   list<CInvoice>{
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 20.000000),
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300, 30.000000),
  //                       CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Third Company, Ltd.", 200, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 32.000000)}));
  // assert(r.delIssued(CInvoice(CDate(2000, 1, 1), "First Company", " Third  Company,  Ltd.   ", 200, 30)));
  // assert(equalLists(r.unmatched("First Company", CSortOpt().addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER, true).addKey(CSortOpt::BY_DATE, true)),
  //                   list<CInvoice>{
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 20.000000),
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300, 30.000000),
  //                       CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 30.000000),
  //                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300, 32.000000)}));
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
