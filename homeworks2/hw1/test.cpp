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
#endif /* __PROGTEST__ */

struct Landlot
{
  std::string city;
  std::string addr;
  std::string region;
  size_t id;
  std::string owner;
  std::string owner_lower = "";
};

class CIterator
{
private:
  const std::vector<Landlot *> m_landLots;
  size_t m_index;

public:
  CIterator(const std::vector<Landlot *> &landLots) : m_landLots(landLots), m_index(0) {}

  bool atEnd() const
  {
    return m_index >= m_landLots.size();
  }

  void next()
  {
    ++m_index;
  }

  std::string city() const
  {
    return m_landLots[m_index]->city;
  }

  std::string addr() const
  {
    return m_landLots[m_index]->addr;
  }

  std::string region() const
  {
    return m_landLots[m_index]->region;
  }

  size_t id() const
  {
    return m_landLots[m_index]->id;
  }

  std::string owner() const
  {
    return m_landLots[m_index]->owner;
  }
};

class CLandRegister
{
private:
  std::vector<std::pair<std::string, std::vector<Landlot *>>> m_ownerDB;
  std::vector<Landlot *> m_landLots_by_cityAddr;
  std::vector<Landlot *> m_landLots_by_regionId;

public:
  CLandRegister();

  ~CLandRegister();

  std::vector<Landlot *> getLandlotsByCityAddr() const;

  std::vector<Landlot *> getLandlotsByRegId() const;

  bool add(const std::string &city, const std::string &addr, const std::string &region, size_t id);

  bool del(const std::string &city, const std::string &addr);

  bool del(const std::string &region, size_t id);

  bool getOwner(const std::string &city, const std::string &addr, std::string &owner) const;

  bool getOwner(const std::string &region, size_t id, std::string &owner) const;

  bool newOwner(const std::string &city, const std::string &addr, const std::string &owner);

  bool newOwner(const std::string &region, size_t id, const std::string &owner);

  size_t count(const std::string &owner) const;

  CIterator listByAddr() const;

  CIterator listByOwner(const std::string &owner) const;

private:
  int binarySearchNewByCityAddr(const std::string &city, const std::string &addr) const;
  int binarySearchNewByRegId(const std::string &region, size_t id) const;
  bool binarySearchByCityAddr(const std::string &city, const std::string &addr) const;
  bool binarySearchByRegionID(const std::string &region, size_t id) const;
  bool unique_land(const std::string &city, const std::string &addr) const;
  bool unique_land(const std::string &region, size_t id) const;
  void modifyOwnerDB(const std::string &owner, Landlot *Landlot, bool operation);
  void delByRegOwnerDB(const std::string &owner, const std::string &region, size_t id);
  void delByCityOwnerDB(const std::string &owner, const std::string &city, const std::string &addr);
};

// -------------------------------------------------- //

/// @brief Returns the appropriate position which a landlot should be inserted to maintain sorting
int CLandRegister::binarySearchNewByCityAddr(const std::string &city, const std::string &addr) const
{
  Landlot landlot{city, addr, "", 0, ""};
  auto it = std::lower_bound(m_landLots_by_cityAddr.begin(), m_landLots_by_cityAddr.end(), &landlot,
                             [](const Landlot *a, const Landlot *b)
                             { return std::tie(a->city, a->addr) < std::tie(b->city, b->addr); });
  return it - m_landLots_by_cityAddr.begin();
}

/// @brief Returns the appropriate position which a landlot should be inserted to maintain sorting
int CLandRegister::binarySearchNewByRegId(const std::string &region, size_t id) const
{
  Landlot landlot{"", "", region, id, ""};
  auto it = std::lower_bound(m_landLots_by_regionId.begin(), m_landLots_by_regionId.end(), &landlot,
                             [](const Landlot *a, const Landlot *b)
                             { return std::tie(a->region, a->id) < std::tie(b->region, b->id); });
  return it - m_landLots_by_regionId.begin();
}

bool CLandRegister::binarySearchByCityAddr(const std::string &city, const std::string &addr) const
{
  Landlot landlot{city, addr, "", 0, ""};
  return std::binary_search(m_landLots_by_cityAddr.begin(), m_landLots_by_cityAddr.end(), &landlot,
                            [](const Landlot *a, const Landlot *b)
                            { return std::tie(a->city, a->addr) < std::tie(b->city, b->addr); });
}

bool CLandRegister::binarySearchByRegionID(const std::string &region, size_t id) const
{
  Landlot landlot{"", "", region, id, ""};
  return std::binary_search(m_landLots_by_regionId.begin(), m_landLots_by_regionId.end(), &landlot,
                            [](const Landlot *a, const Landlot *b)
                            { return std::tie(a->region, a->id) < std::tie(b->region, b->id); });
}

bool CLandRegister::unique_land(const std::string &city, const std::string &addr) const
{
  return m_landLots_by_cityAddr.size() == 0 ? true : !binarySearchByCityAddr(city, addr);
}

bool CLandRegister::unique_land(const std::string &region, size_t id) const
{
  return m_landLots_by_regionId.size() == 0 ? true : !binarySearchByRegionID(region, id);
}

/// @brief Manages the m_OwnerDB vector where it maintains and keeps track of what owners acquire what lands, and stores them in order of aquisition
/// @param operation Represents what operation should be done to the owners land vector:
///                  true -> insert land
///                  false -> remove land
void CLandRegister::modifyOwnerDB(const std::string &owner, Landlot *landlot, bool operation)
{
  auto pair = std::make_pair(owner, std::vector<Landlot *>{});
  auto it = std::lower_bound(m_ownerDB.begin(), m_ownerDB.end(), pair,
                             [](const auto &a, const auto &b)
                             { return a.first < b.first; });

  // Check if owner was found and then modify accordingly
  if (it != m_ownerDB.end() && it->first == owner)
  {
    if (operation)
      it->second.push_back(landlot);
    else
    {
      it->second.erase(std::remove_if(it->second.begin(), it->second.end(),
                                      [&](Landlot *l)
                                      { return l->owner != it->first; }),
                       it->second.end());
    }
    return;
  }

  // Owner not found
  int index = it - m_ownerDB.begin();
  m_ownerDB.insert(m_ownerDB.begin() + index, std::make_pair(owner, std::vector{landlot}));
  return;
}

/// @brief delOwnerDB functions provide the functionality to erase a specific land from owners list, depending on which parameters are given
void CLandRegister::delByRegOwnerDB(const std::string &owner, const std::string &region, size_t id)
{

  for (auto &[owner_, lands] : m_ownerDB)
  {
    if (owner_ == owner)
    {
      lands.erase(std::remove_if(lands.begin(), lands.end(),
                                 [&](Landlot *l)
                                 { return l->region == region && l->id == id; }),
                  lands.end());
    }
  }
}

void CLandRegister::delByCityOwnerDB(const std::string &owner, const std::string &city, const std::string &addr)
{

  for (auto &[owner_, lands] : m_ownerDB)
  {
    if (owner_ == owner)
    {
      lands.erase(std::remove_if(lands.begin(), lands.end(),
                                 [&](Landlot *l)
                                 { return l->city == city && l->addr == addr; }),
                  lands.end());
    }
  }
}

// -------------------------------------------------- //

CLandRegister::CLandRegister() {}

CLandRegister::~CLandRegister()
{
  for (auto ptr : m_landLots_by_cityAddr)
  {
    delete ptr;
  }
}

std::vector<Landlot *> CLandRegister::getLandlotsByCityAddr() const
{
  return m_landLots_by_cityAddr;
}

std::vector<Landlot *> CLandRegister::getLandlotsByRegId() const
{
  return m_landLots_by_regionId;
}

bool CLandRegister::add(const std::string &city, const std::string &addr, const std::string &region, size_t id)
{
  if (!unique_land(city, addr) || !unique_land(region, id))
    return false;

  Landlot *landlot = new Landlot{city, addr, region, id, ""};
  modifyOwnerDB("", landlot, true);

  if (m_landLots_by_cityAddr.size() == 0)
  {
    m_landLots_by_cityAddr.push_back(landlot);
    m_landLots_by_regionId.push_back(landlot);
    return true;
  }

  int sorted_position_cityAddr = binarySearchNewByCityAddr(city, addr);
  int sorted_position_regionId = binarySearchNewByRegId(region, id);

  auto it1 = m_landLots_by_cityAddr.begin();
  auto it2 = m_landLots_by_regionId.begin();

  m_landLots_by_cityAddr.insert(it1 + sorted_position_cityAddr, landlot);
  m_landLots_by_regionId.insert(it2 + sorted_position_regionId, landlot);

  return true;
}

bool CLandRegister::del(const std::string &city, const std::string &addr)
{
  if (unique_land(city, addr))
    return false;

  int target_index = binarySearchNewByCityAddr(city, addr);
  int target_index_regionId = binarySearchNewByRegId(m_landLots_by_cityAddr[target_index]->region, m_landLots_by_cityAddr[target_index]->id);

  delByCityOwnerDB(m_landLots_by_cityAddr[target_index]->owner_lower, city, addr);

  delete m_landLots_by_cityAddr[target_index];

  m_landLots_by_cityAddr.erase(m_landLots_by_cityAddr.begin() + target_index);
  m_landLots_by_regionId.erase(m_landLots_by_regionId.begin() + target_index_regionId);

  return true;
}

bool CLandRegister::del(const std::string &region, size_t id)
{
  if (unique_land(region, id))
    return false;

  int target_index = binarySearchNewByRegId(region, id);
  int target_index_cityAddr = binarySearchNewByCityAddr(m_landLots_by_regionId[target_index]->city, m_landLots_by_regionId[target_index]->addr);

  delByRegOwnerDB(m_landLots_by_regionId[target_index]->owner_lower, region, id);

  delete m_landLots_by_regionId[target_index];

  m_landLots_by_regionId.erase(m_landLots_by_regionId.begin() + target_index);
  m_landLots_by_cityAddr.erase(m_landLots_by_cityAddr.begin() + target_index_cityAddr);

  return true;
}

bool CLandRegister::getOwner(const std::string &city, const std::string &addr, std::string &owner) const
{
  if (unique_land(city, addr))
    return false;

  int target_index = binarySearchNewByCityAddr(city, addr);
  owner = m_landLots_by_cityAddr[target_index]->owner;
  return true;
}

bool CLandRegister::getOwner(const std::string &region, size_t id, std::string &owner) const
{
  if (unique_land(region, id))
    return false;

  int target_index = binarySearchNewByRegId(region, id);
  owner = m_landLots_by_regionId[target_index]->owner;
  return true;
}

/// @brief the method sets new owner owner for the land lot identified by the parameters.
///        The previous owner information is overwritten.
///        City/address or region/ID may be used to identify the land lot to modify.
/// @return Return value is true to indicate a success, or false to indicate a problem (the land lot was not found in the register,
///         the owner already owns the land lot). If the method fails, the register is not modified in any way
bool CLandRegister::newOwner(const std::string &city, const std::string &addr, const std::string &owner)
{
  if (unique_land(city, addr))
    return false;

  int target_index = binarySearchNewByCityAddr(city, addr);
  std::string prev_owner = m_landLots_by_cityAddr[target_index]->owner_lower;

  std::string owner_lower = owner;
  std::transform(owner_lower.begin(), owner_lower.end(), owner_lower.begin(),
                 [](unsigned char c)
                 { return std::tolower(c); });

  if (prev_owner == owner_lower)
    return false;

  Landlot *land = m_landLots_by_cityAddr[target_index];
  land->owner = owner;
  land->owner_lower = owner_lower;

  modifyOwnerDB(prev_owner, land, false);
  modifyOwnerDB(owner_lower, land, true);
  return true;
}

bool CLandRegister::newOwner(const std::string &region, size_t id, const std::string &owner)
{
  if (unique_land(region, id))
    return false;

  int target_index = binarySearchNewByRegId(region, id);
  std::string prev_owner = m_landLots_by_regionId[target_index]->owner_lower;

  std::string owner_lower = owner;
  std::transform(owner_lower.begin(), owner_lower.end(), owner_lower.begin(),
                 [](unsigned char c)
                 { return std::tolower(c); });

  if (prev_owner == owner_lower)
    return false;

  Landlot *land = m_landLots_by_regionId[target_index];
  land->owner = owner;
  land->owner_lower = owner_lower;

  modifyOwnerDB(prev_owner, land, false);
  modifyOwnerDB(owner_lower, land, true);
  return true;
}

size_t CLandRegister::count(const std::string &owner) const
{
  std::string owner_lower = owner;
  std::transform(owner_lower.begin(), owner_lower.end(), owner_lower.begin(),
                 [](unsigned char c)
                 { return std::tolower(c); });

  auto pair = std::make_pair(owner_lower, 0);
  auto it = std::lower_bound(m_ownerDB.begin(), m_ownerDB.end(), pair,
                             [](const auto &a, const auto &b)
                             { return a.first < b.first; });

  if (it != m_ownerDB.end() && it->first == owner_lower)
    return it->second.size();

  // Owner not found
  return 0;
}

/// @brief returns an iterator object (see below), to iterate through the list of all land lots in the register.
///        The returned iterator must list all land lots sorted in an ascending order,
///        the sort key is the name of the city and (if the city is the same) the address.
CIterator CLandRegister::listByAddr() const
{
  return CIterator(m_landLots_by_cityAddr);
}

/// @brief returns an iterator object see (below), to iterate through the list of land lots owned by the owner from the parameter.
///        The iterator must list the land lots in the order the owner bought them (i.e., in the order the owner registered them in our register).
CIterator CLandRegister::listByOwner(const std::string &owner) const
{

  std::string owner_lower = owner;
  std::transform(owner_lower.begin(), owner_lower.end(), owner_lower.begin(),
                 [](unsigned char c)
                 { return std::tolower(c); });

  std::vector<Landlot *> ownerLots;

  for (auto &[owner, lands] : m_ownerDB)
  {
    if (owner == owner_lower)
    {
      ownerLots = lands;
      break;
    }
  }
  return CIterator(ownerLots);
}

// -------------------------------------------------- //
#ifndef __PROGTEST__
static void test0()
{
  CLandRegister x;
  std::string owner;

  assert(x.add("Prague", "Thakurova", "Dejvice", 12345));
  assert(x.add("Prague", "Evropska", "Vokovice", 12345));
  assert(x.add("Prague", "Technicka", "Dejvice", 9873));
  assert(x.add("Plzen", "Evropska", "Plzen mesto", 78901));
  assert(x.add("Liberec", "Evropska", "Librec", 4552));
  CIterator i0 = x.listByAddr();
  assert(!i0.atEnd() && i0.city() == "Liberec" && i0.addr() == "Evropska" && i0.region() == "Librec" && i0.id() == 4552 && i0.owner() == "");
  i0.next();
  assert(!i0.atEnd() && i0.city() == "Plzen" && i0.addr() == "Evropska" && i0.region() == "Plzen mesto" && i0.id() == 78901 && i0.owner() == "");
  i0.next();
  assert(!i0.atEnd() && i0.city() == "Prague" && i0.addr() == "Evropska" && i0.region() == "Vokovice" && i0.id() == 12345 && i0.owner() == "");
  i0.next();
  assert(!i0.atEnd() && i0.city() == "Prague" && i0.addr() == "Technicka" && i0.region() == "Dejvice" && i0.id() == 9873 && i0.owner() == "");
  i0.next();
  assert(!i0.atEnd() && i0.city() == "Prague" && i0.addr() == "Thakurova" && i0.region() == "Dejvice" && i0.id() == 12345 && i0.owner() == "");
  i0.next();
  assert(i0.atEnd());

  assert(x.count("") == 5);
  CIterator i1 = x.listByOwner("");
  assert(!i1.atEnd() && i1.city() == "Prague" && i1.addr() == "Thakurova" && i1.region() == "Dejvice" && i1.id() == 12345 && i1.owner() == "");
  i1.next();
  assert(!i1.atEnd() && i1.city() == "Prague" && i1.addr() == "Evropska" && i1.region() == "Vokovice" && i1.id() == 12345 && i1.owner() == "");
  i1.next();
  assert(!i1.atEnd() && i1.city() == "Prague" && i1.addr() == "Technicka" && i1.region() == "Dejvice" && i1.id() == 9873 && i1.owner() == "");
  i1.next();
  assert(!i1.atEnd() && i1.city() == "Plzen" && i1.addr() == "Evropska" && i1.region() == "Plzen mesto" && i1.id() == 78901 && i1.owner() == "");
  i1.next();
  assert(!i1.atEnd() && i1.city() == "Liberec" && i1.addr() == "Evropska" && i1.region() == "Librec" && i1.id() == 4552 && i1.owner() == "");
  i1.next();
  assert(i1.atEnd());

  assert(x.count("CVUT") == 0);
  CIterator i2 = x.listByOwner("CVUT");
  assert(i2.atEnd());

  assert(x.newOwner("Prague", "Thakurova", "CVUT"));
  assert(x.newOwner("Dejvice", 9873, "CVUT"));
  assert(x.newOwner("Plzen", "Evropska", "Anton Hrabis"));
  assert(x.newOwner("Librec", 4552, "Cvut"));
  assert(x.getOwner("Prague", "Thakurova", owner) && owner == "CVUT");
  assert(x.getOwner("Dejvice", 12345, owner) && owner == "CVUT");
  assert(x.getOwner("Prague", "Evropska", owner) && owner == "");
  assert(x.getOwner("Vokovice", 12345, owner) && owner == "");
  assert(x.getOwner("Prague", "Technicka", owner) && owner == "CVUT");
  assert(x.getOwner("Dejvice", 9873, owner) && owner == "CVUT");
  assert(x.getOwner("Plzen", "Evropska", owner) && owner == "Anton Hrabis");
  assert(x.getOwner("Plzen mesto", 78901, owner) && owner == "Anton Hrabis");
  assert(x.getOwner("Liberec", "Evropska", owner) && owner == "Cvut");
  assert(x.getOwner("Librec", 4552, owner) && owner == "Cvut");
  CIterator i3 = x.listByAddr();
  assert(!i3.atEnd() && i3.city() == "Liberec" && i3.addr() == "Evropska" && i3.region() == "Librec" && i3.id() == 4552 && i3.owner() == "Cvut");
  i3.next();
  assert(!i3.atEnd() && i3.city() == "Plzen" && i3.addr() == "Evropska" && i3.region() == "Plzen mesto" && i3.id() == 78901 && i3.owner() == "Anton Hrabis");
  i3.next();
  assert(!i3.atEnd() && i3.city() == "Prague" && i3.addr() == "Evropska" && i3.region() == "Vokovice" && i3.id() == 12345 && i3.owner() == "");
  i3.next();
  assert(!i3.atEnd() && i3.city() == "Prague" && i3.addr() == "Technicka" && i3.region() == "Dejvice" && i3.id() == 9873 && i3.owner() == "CVUT");
  i3.next();
  assert(!i3.atEnd() && i3.city() == "Prague" && i3.addr() == "Thakurova" && i3.region() == "Dejvice" && i3.id() == 12345 && i3.owner() == "CVUT");
  i3.next();
  assert(i3.atEnd());

  assert(x.count("cvut") == 3);
  CIterator i4 = x.listByOwner("cVuT");
  assert(!i4.atEnd() && i4.city() == "Prague" && i4.addr() == "Thakurova" && i4.region() == "Dejvice" && i4.id() == 12345 && i4.owner() == "CVUT");
  i4.next();
  assert(!i4.atEnd() && i4.city() == "Prague" && i4.addr() == "Technicka" && i4.region() == "Dejvice" && i4.id() == 9873 && i4.owner() == "CVUT");
  i4.next();
  assert(!i4.atEnd() && i4.city() == "Liberec" && i4.addr() == "Evropska" && i4.region() == "Librec" && i4.id() == 4552 && i4.owner() == "Cvut");
  i4.next();
  assert(i4.atEnd());

  assert(x.newOwner("Plzen mesto", 78901, "CVut"));
  assert(x.count("CVUT") == 4);
  CIterator i5 = x.listByOwner("CVUT");
  assert(!i5.atEnd() && i5.city() == "Prague" && i5.addr() == "Thakurova" && i5.region() == "Dejvice" && i5.id() == 12345 && i5.owner() == "CVUT");
  i5.next();
  assert(!i5.atEnd() && i5.city() == "Prague" && i5.addr() == "Technicka" && i5.region() == "Dejvice" && i5.id() == 9873 && i5.owner() == "CVUT");
  i5.next();
  assert(!i5.atEnd() && i5.city() == "Liberec" && i5.addr() == "Evropska" && i5.region() == "Librec" && i5.id() == 4552 && i5.owner() == "Cvut");
  i5.next();
  assert(!i5.atEnd() && i5.city() == "Plzen" && i5.addr() == "Evropska" && i5.region() == "Plzen mesto" && i5.id() == 78901 && i5.owner() == "CVut");
  i5.next();
  assert(i5.atEnd());

  assert(x.del("Liberec", "Evropska"));
  assert(x.del("Plzen mesto", 78901));
  assert(x.count("cvut") == 2);
  CIterator i6 = x.listByOwner("cVuT");
  assert(!i6.atEnd() && i6.city() == "Prague" && i6.addr() == "Thakurova" && i6.region() == "Dejvice" && i6.id() == 12345 && i6.owner() == "CVUT");
  i6.next();
  assert(!i6.atEnd() && i6.city() == "Prague" && i6.addr() == "Technicka" && i6.region() == "Dejvice" && i6.id() == 9873 && i6.owner() == "CVUT");
  i6.next();
  assert(i6.atEnd());

  assert(x.add("Liberec", "Evropska", "Librec", 4552));
}

static void test1()
{
  CLandRegister x;
  std::string owner;

  assert(x.add("Prague", "Thakurova", "Dejvice", 12345));
  assert(x.add("Prague", "Evropska", "Vokovice", 12345));
  assert(x.add("Prague", "Technicka", "Dejvice", 9873));
  assert(!x.add("Prague", "Technicka", "Hradcany", 7344));
  assert(!x.add("Brno", "Bozetechova", "Dejvice", 9873));
  assert(!x.getOwner("Prague", "THAKUROVA", owner));
  assert(!x.getOwner("Hradcany", 7343, owner));
  CIterator i0 = x.listByAddr();
  assert(!i0.atEnd() && i0.city() == "Prague" && i0.addr() == "Evropska" && i0.region() == "Vokovice" && i0.id() == 12345 && i0.owner() == "");
  i0.next();
  assert(!i0.atEnd() && i0.city() == "Prague" && i0.addr() == "Technicka" && i0.region() == "Dejvice" && i0.id() == 9873 && i0.owner() == "");
  i0.next();
  assert(!i0.atEnd() && i0.city() == "Prague" && i0.addr() == "Thakurova" && i0.region() == "Dejvice" && i0.id() == 12345 && i0.owner() == "");
  i0.next();
  assert(i0.atEnd());

  assert(x.newOwner("Prague", "Thakurova", "CVUT"));
  assert(!x.newOwner("Prague", "technicka", "CVUT"));
  assert(!x.newOwner("prague", "Technicka", "CVUT"));
  assert(!x.newOwner("dejvice", 9873, "CVUT"));
  assert(!x.newOwner("Dejvice", 9973, "CVUT"));
  assert(!x.newOwner("Dejvice", 12345, "CVUT"));
  assert(x.count("CVUT") == 1);
  CIterator i1 = x.listByOwner("CVUT");
  assert(!i1.atEnd() && i1.city() == "Prague" && i1.addr() == "Thakurova" && i1.region() == "Dejvice" && i1.id() == 12345 && i1.owner() == "CVUT");
  i1.next();
  assert(i1.atEnd());

  assert(!x.del("Brno", "Technicka"));
  assert(!x.del("Karlin", 9873));
  assert(x.del("Prague", "Technicka"));
  assert(!x.del("Prague", "Technicka"));
  assert(!x.del("Dejvice", 9873));
}

int main(void)
{
  test0();
  test1();
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
