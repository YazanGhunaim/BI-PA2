#ifndef __PROGTEST__
#include <cassert>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <memory>
#include <functional>
#endif /* __PROGTEST__ */

class CComponent;
class CComputer;
class CNetwork;

using ComponentPtr = std::shared_ptr<CComponent>;
using ComputerPtr = std::shared_ptr<CComputer>;
using NetworkPtr = std::shared_ptr<CNetwork>;

class CComponent
{
public:
  virtual ~CComponent() = default;
  virtual ComponentPtr clone() const = 0;
  virtual std::ostream &print(std::ostream &os) const = 0;
  virtual std::ostream &print_layer(std::ostream &os, bool prefix) const
  {
    return print(os);
  }
  virtual bool isNetwork() const
  {
    return false;
  }
  virtual std::string getName() const
  {
    return "";
  }
};

class CCPU : public CComponent
{
private:
  int m_cores;
  int m_frequency;

public:
  CCPU(int cores, int frequency) : m_cores(cores), m_frequency(frequency) {}

  ComponentPtr clone() const override
  {
    return std::make_shared<CCPU>(*this);
  }

  std::ostream &print(std::ostream &os) const override
  {
    return os << "CPU, " << std::to_string(m_cores) << " cores @ " << std::to_string(m_frequency) << "MHz\n";
  }
};

class CMemory : public CComponent
{
private:
  int m_size;

public:
  CMemory(int size) : m_size(size) {}

  ComponentPtr clone() const override
  {
    return std::make_shared<CMemory>(*this);
  }

  std::ostream &print(std::ostream &os) const override
  {
    return os << "Memory, " << std::to_string(m_size) << " MiB\n";
  }
};

class CDisk : public CComponent
{
public:
  enum DiskType
  {
    SSD,
    MAGNETIC
  };

  struct Partition
  {
    int m_size;
    std::string m_desc;

    Partition(int size, std::string desc) : m_size(size), m_desc(desc) {}

    std::ostream &print(std::ostream &os, size_t count, bool prefix) const
    {
      if (prefix)
        return os << "+-[" << std::to_string(count) << "]: " << std::to_string(m_size) << " GiB, " << m_desc;
      else
        return os << "[" << std::to_string(count) << "]: " << std::to_string(m_size) << " GiB, " << m_desc;
    }
  };

private:
  DiskType m_disk_type;
  int m_size;
  std::vector<Partition> m_paritions;

public:
  CDisk(DiskType type, int size) : m_disk_type(type), m_size(size) {}

  ComponentPtr clone() const override
  {
    return std::make_shared<CDisk>(*this);
  }

  CDisk &addPartition(int size, const std::string &desc)
  {
    m_paritions.push_back(Partition(size, desc));
    return *this;
  }

  std::ostream &print_layer(std::ostream &os, bool prefix) const override
  {
    os << (m_disk_type == SSD ? "SSD" : "HDD") << ", " << std::to_string(m_size) << " GiB\n";

    std::stringstream ss;
    size_t count = 0;
    for (const auto &partition : m_paritions)
    {
      bool last = (count == m_paritions.size() - 1);
      if (last)
      {
        ss << "\\-";
        partition.print(ss, count++, !last);
      }
      else
      {
        ss << "+-";
        partition.print(ss, count++, last);
      }
      ss << "\n";
    }

    std::string line;
    while (getline(ss, line))
    {
      if (line.empty())
        continue;

      if (prefix)
        os << "| " << line << "\n";
      else
        os << "  " << line << "\n";
    }

    return os;
  }

  std::ostream &print(std::ostream &os) const override
  {

    os << (m_disk_type == SSD ? "SSD" : "HDD") << ", " << std::to_string(m_size) << " GiB\n";

    size_t count = 0;
    for (const auto &partition : m_paritions)
    {
      if (count == m_paritions.size() - 1)
        os << "\\-";

      partition.print(os, count++, false);
    }

    return os;
  }
};

class CComputer
{
private:
  std::string m_name;
  std::vector<std::string> m_addresses;
  std::vector<ComponentPtr> m_components;

public:
  CComputer(std::string name) : m_name(name) {}

  CComputer(const CComputer &other) : m_name(other.m_name), m_addresses(other.m_addresses)
  {
    for (const auto &component : other.m_components)
    {
      m_components.push_back(component->clone());
    }
  }

  CComputer &operator=(const CComputer &other)
  {
    if (this != &other)
    {
      m_name = other.m_name;
      m_addresses = other.m_addresses;

      m_components.clear();
      for (const auto &component : other.m_components)
      {
        m_components.push_back(component->clone());
      }
    }
    return *this;
  }

  ComputerPtr clone() const
  {
    return std::make_shared<CComputer>(*this);
  }

  CComputer &addComponent(const CComponent &component)
  {
    m_components.push_back(component.clone());
    return *this;
  }

  CComputer &addAddress(const std::string &address)
  {
    m_addresses.push_back(address);
    return *this;
  }

  ComputerPtr findComputer(const std::string &name) const;

  NetworkPtr findNetwork(const std::string &name) const
  {
    for (auto &component : m_components)
    {
      if (component->isNetwork())
      {
        if (component->getName() == name)
        {
          auto network = std::dynamic_pointer_cast<CNetwork>(component);
          return network;
        }
      }
    }

    return nullptr;
  }

  CComputer duplicate(const std::map<std::string, std::string> &remap) const
  {
    CComputer res = *this;

    res.remap(remap);

    return res;
  }

  void remap(const std::map<std::string, std::string> &remap);

  std::string getName() const
  {
    return m_name;
  }

  std::ostream &print_layer(std::ostream &os, bool prefix = false)
  {
    os << "Host: " + m_name + "\n";

    std::stringstream ss;
    for (const auto &address : m_addresses)
    {
      ss << "+-" + address + "\n";
    }

    for (const auto &component : m_components)
    {
      bool last = (component == m_components.back());
      ss << (last ? "\\-" : "+-");
      component->print_layer(ss, !last);
      ss << "\n";
    }

    std::string line;
    while (std::getline(ss, line))
    {
      if (line.empty())
        continue;

      if (prefix)
        os << "| " << line << "\n";
      else
        os << "  " << line << "\n";
    }

    return os;
  }

  std::ostream &print(std::ostream &os) const
  {

    os << "Host: " + m_name << "\n";

    for (const auto &address : m_addresses)
    {
      os << "+-" + address + "\n";
    }

    for (const auto &component : m_components)
    {
      bool last = (component == m_components.back());

      os << (last ? "\\-" : "+-");
      component->print_layer(os, !last);
    }

    return os;
  }

  friend std::ostream &operator<<(std::ostream &os, const CComputer &src)
  {
    return src.print(os);
  }
};

class CNetwork : public CComponent
{
private:
  std::string m_name;
  std::vector<ComputerPtr> m_computers;

public:
  CNetwork(std::string name) : m_name(name) {}

  CNetwork(const CNetwork &other) : m_name(other.m_name)
  {
    for (const auto &computer : other.m_computers)
    {
      m_computers.push_back(computer->clone());
    }
  }

  CNetwork &operator=(const CNetwork &other)
  {
    if (this != &other)
    {
      m_name = other.m_name;

      m_computers.clear();
      for (const auto &computer : other.m_computers)
      {
        m_computers.push_back(computer->clone());
      }
    }
    return *this;
  }

  ComponentPtr clone() const override
  {
    return std::make_shared<CNetwork>(*this);
  }

  CNetwork &addComputer(const CComputer &computer)
  {
    m_computers.push_back(computer.clone());
    return *this;
  }

  ComputerPtr findComputer(const std::string &name) const
  {
    for (auto &computer : m_computers)
    {
      if (computer->getName() == name)
      {
        return computer;
      }

      auto ptr = computer->findComputer(name);
      if (ptr)
        return ptr;
    }
    return nullptr;
  }

  NetworkPtr findNetwork(const std::string &name) const
  {
    for (auto &computer : m_computers)
    {
      auto ptr = computer->findNetwork(name);
      if (ptr)
        return ptr;
    }

    return nullptr;
  }

  void remap(const std::map<std::string, std::string> &remap)
  {
    if (remap.count(m_name) != 0)
    {
      m_name = remap.at(m_name);
    }

    for (auto &computer : m_computers)
    {
      computer->remap(remap);
    }
  }

  std::string getName() const override
  {
    return m_name;
  }

  bool isNetwork() const override
  {
    return true;
  }

  std::ostream &print_layer(std::ostream &os, bool prefix) const override
  {
    os << "Network: " << m_name << "\n";

    std::stringstream ss;
    for (const auto &computer : m_computers)
    {
      bool last = (computer == m_computers.back());
      ss << (last ? "\\-" : "+-");
      computer->print_layer(ss, !last);
    }

    std::string line;
    while (getline(ss, line))
    {

      if (line.empty())
        continue;

      if (prefix)
        os << "| " << line << "\n";
      else
        os << "  " << line << "\n";
    }

    return os;
  }

  std::ostream &print(std::ostream &os) const override
  {

    os << "Network: " << m_name << "\n";

    for (const auto &computer : m_computers)
    {
      bool last = (computer == m_computers.back());
      os << (last ? "\\-" : "+-");
      computer->print_layer(os, !last);
    }

    return os;
  }

  friend std::ostream &operator<<(std::ostream &os, const CNetwork &src)
  {
    return src.print(os);
  }
};

ComputerPtr CComputer::findComputer(const std::string &name) const
{
  for (auto &component : m_components)
  {
    if (component->isNetwork())
    {
      auto network = std::dynamic_pointer_cast<CNetwork>(component);
      auto ptr = network->findComputer(name);
      if (ptr)
        return ptr;
    }
  }

  return nullptr;
}

void CComputer::remap(const std::map<std::string, std::string> &remap)
{
  if (remap.count(m_name) != 0)
  {
    m_name = remap.at(m_name);
  }

  for (auto &component : m_components)
  {
    if (component->isNetwork())
    {
      auto network = std::dynamic_pointer_cast<CNetwork>(component);
      network->remap(remap);
    }
  }
}

#ifndef __PROGTEST__
template <typename T_>
std::string toString(const T_ &x)
{
  std::ostringstream oss;
  oss << x;
  return oss.str();
}

int main()
{
  CNetwork n("FIT network");
  n.addComputer(
       CComputer("progtest.fit.cvut.cz").addAddress("147.32.232.142").addComponent(CCPU(8, 2400)).addComponent(CCPU(8, 1200)).addComponent(CDisk(CDisk::MAGNETIC, 1500).addPartition(50, "/").addPartition(5, "/boot").addPartition(1000, "/var")).addComponent(CDisk(CDisk::SSD, 60).addPartition(60, "/data")).addComponent(CMemory(2000)).addComponent(CMemory(2000)))
      .addComputer(
          CComputer("courses.fit.cvut.cz").addAddress("147.32.232.213").addComponent(CCPU(4, 1600)).addComponent(CMemory(4000)).addComponent(CDisk(CDisk::MAGNETIC, 2000).addPartition(100, "/").addPartition(1900, "/data")))
      .addComputer(
          CComputer("imap.fit.cvut.cz").addAddress("147.32.232.238").addComponent(CCPU(4, 2500)).addAddress("2001:718:2:2901::238").addComponent(CMemory(8000)));
  assert(toString(n) ==
         "Network: FIT network\n"
         "+-Host: progtest.fit.cvut.cz\n"
         "| +-147.32.232.142\n"
         "| +-CPU, 8 cores @ 2400MHz\n"
         "| +-CPU, 8 cores @ 1200MHz\n"
         "| +-HDD, 1500 GiB\n"
         "| | +-[0]: 50 GiB, /\n"
         "| | +-[1]: 5 GiB, /boot\n"
         "| | \\-[2]: 1000 GiB, /var\n"
         "| +-SSD, 60 GiB\n"
         "| | \\-[0]: 60 GiB, /data\n"
         "| +-Memory, 2000 MiB\n"
         "| \\-Memory, 2000 MiB\n"
         "+-Host: courses.fit.cvut.cz\n"
         "| +-147.32.232.213\n"
         "| +-CPU, 4 cores @ 1600MHz\n"
         "| +-Memory, 4000 MiB\n"
         "| \\-HDD, 2000 GiB\n"
         "|   +-[0]: 100 GiB, /\n"
         "|   \\-[1]: 1900 GiB, /data\n"
         "\\-Host: imap.fit.cvut.cz\n"
         "  +-147.32.232.238\n"
         "  +-2001:718:2:2901::238\n"
         "  +-CPU, 4 cores @ 2500MHz\n"
         "  \\-Memory, 8000 MiB\n");
  CNetwork x = n;
  auto c = x.findComputer("imap.fit.cvut.cz");
  assert(toString(*c) ==
         "Host: imap.fit.cvut.cz\n"
         "+-147.32.232.238\n"
         "+-2001:718:2:2901::238\n"
         "+-CPU, 4 cores @ 2500MHz\n"
         "\\-Memory, 8000 MiB\n");
  c->addComponent(CDisk(CDisk::MAGNETIC, 1000).addPartition(100, "system").addPartition(200, "WWW").addPartition(700, "mail"));
  assert(toString(x) ==
         "Network: FIT network\n"
         "+-Host: progtest.fit.cvut.cz\n"
         "| +-147.32.232.142\n"
         "| +-CPU, 8 cores @ 2400MHz\n"
         "| +-CPU, 8 cores @ 1200MHz\n"
         "| +-HDD, 1500 GiB\n"
         "| | +-[0]: 50 GiB, /\n"
         "| | +-[1]: 5 GiB, /boot\n"
         "| | \\-[2]: 1000 GiB, /var\n"
         "| +-SSD, 60 GiB\n"
         "| | \\-[0]: 60 GiB, /data\n"
         "| +-Memory, 2000 MiB\n"
         "| \\-Memory, 2000 MiB\n"
         "+-Host: courses.fit.cvut.cz\n"
         "| +-147.32.232.213\n"
         "| +-CPU, 4 cores @ 1600MHz\n"
         "| +-Memory, 4000 MiB\n"
         "| \\-HDD, 2000 GiB\n"
         "|   +-[0]: 100 GiB, /\n"
         "|   \\-[1]: 1900 GiB, /data\n"
         "\\-Host: imap.fit.cvut.cz\n"
         "  +-147.32.232.238\n"
         "  +-2001:718:2:2901::238\n"
         "  +-CPU, 4 cores @ 2500MHz\n"
         "  +-Memory, 8000 MiB\n"
         "  \\-HDD, 1000 GiB\n"
         "    +-[0]: 100 GiB, system\n"
         "    +-[1]: 200 GiB, WWW\n"
         "    \\-[2]: 700 GiB, mail\n");
  assert(toString(n) ==
         "Network: FIT network\n"
         "+-Host: progtest.fit.cvut.cz\n"
         "| +-147.32.232.142\n"
         "| +-CPU, 8 cores @ 2400MHz\n"
         "| +-CPU, 8 cores @ 1200MHz\n"
         "| +-HDD, 1500 GiB\n"
         "| | +-[0]: 50 GiB, /\n"
         "| | +-[1]: 5 GiB, /boot\n"
         "| | \\-[2]: 1000 GiB, /var\n"
         "| +-SSD, 60 GiB\n"
         "| | \\-[0]: 60 GiB, /data\n"
         "| +-Memory, 2000 MiB\n"
         "| \\-Memory, 2000 MiB\n"
         "+-Host: courses.fit.cvut.cz\n"
         "| +-147.32.232.213\n"
         "| +-CPU, 4 cores @ 1600MHz\n"
         "| +-Memory, 4000 MiB\n"
         "| \\-HDD, 2000 GiB\n"
         "|   +-[0]: 100 GiB, /\n"
         "|   \\-[1]: 1900 GiB, /data\n"
         "\\-Host: imap.fit.cvut.cz\n"
         "  +-147.32.232.238\n"
         "  +-2001:718:2:2901::238\n"
         "  +-CPU, 4 cores @ 2500MHz\n"
         "  \\-Memory, 8000 MiB\n");
  n.addComputer(CComputer("vm01.fit.cvut.cz").addAddress("147.32.232.232").addComponent(CCPU(32, 4000)).addComponent(CMemory(32768)).addComponent(CDisk(CDisk::MAGNETIC, 3000).addPartition(500, "SYSTEM").addPartition(2000, "DATA").addPartition(500, "BACKUP")).addComponent(CNetwork("dummy00").addComputer(CComputer("testing.fit.cvut.cz").addAddress("192.168.1.1").addComponent(CCPU(1, 300)).addComponent(CMemory(256)))).addComponent(CMemory(16384)).addComponent(CNetwork("vnet00")));
  assert(toString(n) ==
         "Network: FIT network\n"
         "+-Host: progtest.fit.cvut.cz\n"
         "| +-147.32.232.142\n"
         "| +-CPU, 8 cores @ 2400MHz\n"
         "| +-CPU, 8 cores @ 1200MHz\n"
         "| +-HDD, 1500 GiB\n"
         "| | +-[0]: 50 GiB, /\n"
         "| | +-[1]: 5 GiB, /boot\n"
         "| | \\-[2]: 1000 GiB, /var\n"
         "| +-SSD, 60 GiB\n"
         "| | \\-[0]: 60 GiB, /data\n"
         "| +-Memory, 2000 MiB\n"
         "| \\-Memory, 2000 MiB\n"
         "+-Host: courses.fit.cvut.cz\n"
         "| +-147.32.232.213\n"
         "| +-CPU, 4 cores @ 1600MHz\n"
         "| +-Memory, 4000 MiB\n"
         "| \\-HDD, 2000 GiB\n"
         "|   +-[0]: 100 GiB, /\n"
         "|   \\-[1]: 1900 GiB, /data\n"
         "+-Host: imap.fit.cvut.cz\n"
         "| +-147.32.232.238\n"
         "| +-2001:718:2:2901::238\n"
         "| +-CPU, 4 cores @ 2500MHz\n"
         "| \\-Memory, 8000 MiB\n"
         "\\-Host: vm01.fit.cvut.cz\n"
         "  +-147.32.232.232\n"
         "  +-CPU, 32 cores @ 4000MHz\n"
         "  +-Memory, 32768 MiB\n"
         "  +-HDD, 3000 GiB\n"
         "  | +-[0]: 500 GiB, SYSTEM\n"
         "  | +-[1]: 2000 GiB, DATA\n"
         "  | \\-[2]: 500 GiB, BACKUP\n"
         "  +-Network: dummy00\n"
         "  | \\-Host: testing.fit.cvut.cz\n"
         "  |   +-192.168.1.1\n"
         "  |   +-CPU, 1 cores @ 300MHz\n"
         "  |   \\-Memory, 256 MiB\n"
         "  +-Memory, 16384 MiB\n"
         "  \\-Network: vnet00\n");
  auto vmnet = n.findNetwork("vnet00");
  vmnet->addComputer(CComputer("monitor1.fit.cvut.cz").addAddress("147.32.232.254").addComponent(CCPU(2, 1500)).addComponent(CMemory(4096)).addComponent(CDisk(CDisk::MAGNETIC, 750).addPartition(100, "root").addPartition(600, "log")));
  assert(toString(*vmnet) ==
         "Network: vnet00\n"
         "\\-Host: monitor1.fit.cvut.cz\n"
         "  +-147.32.232.254\n"
         "  +-CPU, 2 cores @ 1500MHz\n"
         "  +-Memory, 4096 MiB\n"
         "  \\-HDD, 750 GiB\n"
         "    +-[0]: 100 GiB, root\n"
         "    \\-[1]: 600 GiB, log\n");
  assert(toString(n) ==
         "Network: FIT network\n"
         "+-Host: progtest.fit.cvut.cz\n"
         "| +-147.32.232.142\n"
         "| +-CPU, 8 cores @ 2400MHz\n"
         "| +-CPU, 8 cores @ 1200MHz\n"
         "| +-HDD, 1500 GiB\n"
         "| | +-[0]: 50 GiB, /\n"
         "| | +-[1]: 5 GiB, /boot\n"
         "| | \\-[2]: 1000 GiB, /var\n"
         "| +-SSD, 60 GiB\n"
         "| | \\-[0]: 60 GiB, /data\n"
         "| +-Memory, 2000 MiB\n"
         "| \\-Memory, 2000 MiB\n"
         "+-Host: courses.fit.cvut.cz\n"
         "| +-147.32.232.213\n"
         "| +-CPU, 4 cores @ 1600MHz\n"
         "| +-Memory, 4000 MiB\n"
         "| \\-HDD, 2000 GiB\n"
         "|   +-[0]: 100 GiB, /\n"
         "|   \\-[1]: 1900 GiB, /data\n"
         "+-Host: imap.fit.cvut.cz\n"
         "| +-147.32.232.238\n"
         "| +-2001:718:2:2901::238\n"
         "| +-CPU, 4 cores @ 2500MHz\n"
         "| \\-Memory, 8000 MiB\n"
         "\\-Host: vm01.fit.cvut.cz\n"
         "  +-147.32.232.232\n"
         "  +-CPU, 32 cores @ 4000MHz\n"
         "  +-Memory, 32768 MiB\n"
         "  +-HDD, 3000 GiB\n"
         "  | +-[0]: 500 GiB, SYSTEM\n"
         "  | +-[1]: 2000 GiB, DATA\n"
         "  | \\-[2]: 500 GiB, BACKUP\n"
         "  +-Network: dummy00\n"
         "  | \\-Host: testing.fit.cvut.cz\n"
         "  |   +-192.168.1.1\n"
         "  |   +-CPU, 1 cores @ 300MHz\n"
         "  |   \\-Memory, 256 MiB\n"
         "  +-Memory, 16384 MiB\n"
         "  \\-Network: vnet00\n"
         "    \\-Host: monitor1.fit.cvut.cz\n"
         "      +-147.32.232.254\n"
         "      +-CPU, 2 cores @ 1500MHz\n"
         "      +-Memory, 4096 MiB\n"
         "      \\-HDD, 750 GiB\n"
         "        +-[0]: 100 GiB, root\n"
         "        \\-[1]: 600 GiB, log\n");

  vmnet->addComputer(n.findComputer("monitor1.fit.cvut.cz")
                         ->duplicate({std::pair<std::string, std::string>("monitor1.fit.cvut.cz", "monitor2.fit.cvut.cz")}));
  assert(toString(*vmnet) ==
         "Network: vnet00\n"
         "+-Host: monitor1.fit.cvut.cz\n"
         "| +-147.32.232.254\n"
         "| +-CPU, 2 cores @ 1500MHz\n"
         "| +-Memory, 4096 MiB\n"
         "| \\-HDD, 750 GiB\n"
         "|   +-[0]: 100 GiB, root\n"
         "|   \\-[1]: 600 GiB, log\n"
         "\\-Host: monitor2.fit.cvut.cz\n"
         "  +-147.32.232.254\n"
         "  +-CPU, 2 cores @ 1500MHz\n"
         "  +-Memory, 4096 MiB\n"
         "  \\-HDD, 750 GiB\n"
         "    +-[0]: 100 GiB, root\n"
         "    \\-[1]: 600 GiB, log\n");
  n.addComputer(n.findComputer("vm01.fit.cvut.cz")
                    ->duplicate(
                        {std::pair<std::string, std::string>("monitor1.fit.cvut.cz", "monitor3.fit.cvut.cz"),
                         std::pair<std::string, std::string>("monitor2.fit.cvut.cz", "monitor4.fit.cvut.cz"),
                         std::pair<std::string, std::string>("vm01.fit.cvut.cz", "vm02.fit.cvut.cz")}));
  assert(toString(n) ==
         "Network: FIT network\n"
         "+-Host: progtest.fit.cvut.cz\n"
         "| +-147.32.232.142\n"
         "| +-CPU, 8 cores @ 2400MHz\n"
         "| +-CPU, 8 cores @ 1200MHz\n"
         "| +-HDD, 1500 GiB\n"
         "| | +-[0]: 50 GiB, /\n"
         "| | +-[1]: 5 GiB, /boot\n"
         "| | \\-[2]: 1000 GiB, /var\n"
         "| +-SSD, 60 GiB\n"
         "| | \\-[0]: 60 GiB, /data\n"
         "| +-Memory, 2000 MiB\n"
         "| \\-Memory, 2000 MiB\n"
         "+-Host: courses.fit.cvut.cz\n"
         "| +-147.32.232.213\n"
         "| +-CPU, 4 cores @ 1600MHz\n"
         "| +-Memory, 4000 MiB\n"
         "| \\-HDD, 2000 GiB\n"
         "|   +-[0]: 100 GiB, /\n"
         "|   \\-[1]: 1900 GiB, /data\n"
         "+-Host: imap.fit.cvut.cz\n"
         "| +-147.32.232.238\n"
         "| +-2001:718:2:2901::238\n"
         "| +-CPU, 4 cores @ 2500MHz\n"
         "| \\-Memory, 8000 MiB\n"
         "+-Host: vm01.fit.cvut.cz\n"
         "| +-147.32.232.232\n"
         "| +-CPU, 32 cores @ 4000MHz\n"
         "| +-Memory, 32768 MiB\n"
         "| +-HDD, 3000 GiB\n"
         "| | +-[0]: 500 GiB, SYSTEM\n"
         "| | +-[1]: 2000 GiB, DATA\n"
         "| | \\-[2]: 500 GiB, BACKUP\n"
         "| +-Network: dummy00\n"
         "| | \\-Host: testing.fit.cvut.cz\n"
         "| |   +-192.168.1.1\n"
         "| |   +-CPU, 1 cores @ 300MHz\n"
         "| |   \\-Memory, 256 MiB\n"
         "| +-Memory, 16384 MiB\n"
         "| \\-Network: vnet00\n"
         "|   +-Host: monitor1.fit.cvut.cz\n"
         "|   | +-147.32.232.254\n"
         "|   | +-CPU, 2 cores @ 1500MHz\n"
         "|   | +-Memory, 4096 MiB\n"
         "|   | \\-HDD, 750 GiB\n"
         "|   |   +-[0]: 100 GiB, root\n"
         "|   |   \\-[1]: 600 GiB, log\n"
         "|   \\-Host: monitor2.fit.cvut.cz\n"
         "|     +-147.32.232.254\n"
         "|     +-CPU, 2 cores @ 1500MHz\n"
         "|     +-Memory, 4096 MiB\n"
         "|     \\-HDD, 750 GiB\n"
         "|       +-[0]: 100 GiB, root\n"
         "|       \\-[1]: 600 GiB, log\n"
         "\\-Host: vm02.fit.cvut.cz\n"
         "  +-147.32.232.232\n"
         "  +-CPU, 32 cores @ 4000MHz\n"
         "  +-Memory, 32768 MiB\n"
         "  +-HDD, 3000 GiB\n"
         "  | +-[0]: 500 GiB, SYSTEM\n"
         "  | +-[1]: 2000 GiB, DATA\n"
         "  | \\-[2]: 500 GiB, BACKUP\n"
         "  +-Network: dummy00\n"
         "  | \\-Host: testing.fit.cvut.cz\n"
         "  |   +-192.168.1.1\n"
         "  |   +-CPU, 1 cores @ 300MHz\n"
         "  |   \\-Memory, 256 MiB\n"
         "  +-Memory, 16384 MiB\n"
         "  \\-Network: vnet00\n"
         "    +-Host: monitor3.fit.cvut.cz\n"
         "    | +-147.32.232.254\n"
         "    | +-CPU, 2 cores @ 1500MHz\n"
         "    | +-Memory, 4096 MiB\n"
         "    | \\-HDD, 750 GiB\n"
         "    |   +-[0]: 100 GiB, root\n"
         "    |   \\-[1]: 600 GiB, log\n"
         "    \\-Host: monitor4.fit.cvut.cz\n"
         "      +-147.32.232.254\n"
         "      +-CPU, 2 cores @ 1500MHz\n"
         "      +-Memory, 4096 MiB\n"
         "      \\-HDD, 750 GiB\n"
         "        +-[0]: 100 GiB, root\n"
         "        \\-[1]: 600 GiB, log\n");
  vmnet->addComputer(n.findComputer("vm02.fit.cvut.cz")
                         ->duplicate(
                             {std::pair<std::string, std::string>("monitor3.fit.cvut.cz", "monitor5.fit.cvut.cz"),
                              std::pair<std::string, std::string>("monitor4.fit.cvut.cz", "monitor6.fit.cvut.cz"),
                              std::pair<std::string, std::string>("vm02.fit.cvut.cz", "vm03.fit.cvut.cz"),
                              std::pair<std::string, std::string>("vnet00", "vnet01")}));
  assert(toString(*vmnet) ==
         "Network: vnet00\n"
         "+-Host: monitor1.fit.cvut.cz\n"
         "| +-147.32.232.254\n"
         "| +-CPU, 2 cores @ 1500MHz\n"
         "| +-Memory, 4096 MiB\n"
         "| \\-HDD, 750 GiB\n"
         "|   +-[0]: 100 GiB, root\n"
         "|   \\-[1]: 600 GiB, log\n"
         "+-Host: monitor2.fit.cvut.cz\n"
         "| +-147.32.232.254\n"
         "| +-CPU, 2 cores @ 1500MHz\n"
         "| +-Memory, 4096 MiB\n"
         "| \\-HDD, 750 GiB\n"
         "|   +-[0]: 100 GiB, root\n"
         "|   \\-[1]: 600 GiB, log\n"
         "\\-Host: vm03.fit.cvut.cz\n"
         "  +-147.32.232.232\n"
         "  +-CPU, 32 cores @ 4000MHz\n"
         "  +-Memory, 32768 MiB\n"
         "  +-HDD, 3000 GiB\n"
         "  | +-[0]: 500 GiB, SYSTEM\n"
         "  | +-[1]: 2000 GiB, DATA\n"
         "  | \\-[2]: 500 GiB, BACKUP\n"
         "  +-Network: dummy00\n"
         "  | \\-Host: testing.fit.cvut.cz\n"
         "  |   +-192.168.1.1\n"
         "  |   +-CPU, 1 cores @ 300MHz\n"
         "  |   \\-Memory, 256 MiB\n"
         "  +-Memory, 16384 MiB\n"
         "  \\-Network: vnet01\n"
         "    +-Host: monitor5.fit.cvut.cz\n"
         "    | +-147.32.232.254\n"
         "    | +-CPU, 2 cores @ 1500MHz\n"
         "    | +-Memory, 4096 MiB\n"
         "    | \\-HDD, 750 GiB\n"
         "    |   +-[0]: 100 GiB, root\n"
         "    |   \\-[1]: 600 GiB, log\n"
         "    \\-Host: monitor6.fit.cvut.cz\n"
         "      +-147.32.232.254\n"
         "      +-CPU, 2 cores @ 1500MHz\n"
         "      +-Memory, 4096 MiB\n"
         "      \\-HDD, 750 GiB\n"
         "        +-[0]: 100 GiB, root\n"
         "        \\-[1]: 600 GiB, log\n");
  n.findComputer("vm02.fit.cvut.cz")
      ->findNetwork("vnet00")
      ->addComputer(n.findComputer("vm01.fit.cvut.cz")
                        ->duplicate(
                            {std::pair<std::string, std::string>("monitor1.fit.cvut.cz", "monitor11.fit.cvut.cz"),
                             std::pair<std::string, std::string>("monitor2.fit.cvut.cz", "monitor12.fit.cvut.cz"),
                             std::pair<std::string, std::string>("vm01.fit.cvut.cz", "vm11.fit.cvut.cz"),
                             std::pair<std::string, std::string>("vm03.fit.cvut.cz", "vm13.fit.cvut.cz"),
                             std::pair<std::string, std::string>("vnet00", "vnet10"),
                             std::pair<std::string, std::string>("vnet01", "vnet11")}));
  assert(toString(n) ==
         "Network: FIT network\n"
         "+-Host: progtest.fit.cvut.cz\n"
         "| +-147.32.232.142\n"
         "| +-CPU, 8 cores @ 2400MHz\n"
         "| +-CPU, 8 cores @ 1200MHz\n"
         "| +-HDD, 1500 GiB\n"
         "| | +-[0]: 50 GiB, /\n"
         "| | +-[1]: 5 GiB, /boot\n"
         "| | \\-[2]: 1000 GiB, /var\n"
         "| +-SSD, 60 GiB\n"
         "| | \\-[0]: 60 GiB, /data\n"
         "| +-Memory, 2000 MiB\n"
         "| \\-Memory, 2000 MiB\n"
         "+-Host: courses.fit.cvut.cz\n"
         "| +-147.32.232.213\n"
         "| +-CPU, 4 cores @ 1600MHz\n"
         "| +-Memory, 4000 MiB\n"
         "| \\-HDD, 2000 GiB\n"
         "|   +-[0]: 100 GiB, /\n"
         "|   \\-[1]: 1900 GiB, /data\n"
         "+-Host: imap.fit.cvut.cz\n"
         "| +-147.32.232.238\n"
         "| +-2001:718:2:2901::238\n"
         "| +-CPU, 4 cores @ 2500MHz\n"
         "| \\-Memory, 8000 MiB\n"
         "+-Host: vm01.fit.cvut.cz\n"
         "| +-147.32.232.232\n"
         "| +-CPU, 32 cores @ 4000MHz\n"
         "| +-Memory, 32768 MiB\n"
         "| +-HDD, 3000 GiB\n"
         "| | +-[0]: 500 GiB, SYSTEM\n"
         "| | +-[1]: 2000 GiB, DATA\n"
         "| | \\-[2]: 500 GiB, BACKUP\n"
         "| +-Network: dummy00\n"
         "| | \\-Host: testing.fit.cvut.cz\n"
         "| |   +-192.168.1.1\n"
         "| |   +-CPU, 1 cores @ 300MHz\n"
         "| |   \\-Memory, 256 MiB\n"
         "| +-Memory, 16384 MiB\n"
         "| \\-Network: vnet00\n"
         "|   +-Host: monitor1.fit.cvut.cz\n"
         "|   | +-147.32.232.254\n"
         "|   | +-CPU, 2 cores @ 1500MHz\n"
         "|   | +-Memory, 4096 MiB\n"
         "|   | \\-HDD, 750 GiB\n"
         "|   |   +-[0]: 100 GiB, root\n"
         "|   |   \\-[1]: 600 GiB, log\n"
         "|   +-Host: monitor2.fit.cvut.cz\n"
         "|   | +-147.32.232.254\n"
         "|   | +-CPU, 2 cores @ 1500MHz\n"
         "|   | +-Memory, 4096 MiB\n"
         "|   | \\-HDD, 750 GiB\n"
         "|   |   +-[0]: 100 GiB, root\n"
         "|   |   \\-[1]: 600 GiB, log\n"
         "|   \\-Host: vm03.fit.cvut.cz\n"
         "|     +-147.32.232.232\n"
         "|     +-CPU, 32 cores @ 4000MHz\n"
         "|     +-Memory, 32768 MiB\n"
         "|     +-HDD, 3000 GiB\n"
         "|     | +-[0]: 500 GiB, SYSTEM\n"
         "|     | +-[1]: 2000 GiB, DATA\n"
         "|     | \\-[2]: 500 GiB, BACKUP\n"
         "|     +-Network: dummy00\n"
         "|     | \\-Host: testing.fit.cvut.cz\n"
         "|     |   +-192.168.1.1\n"
         "|     |   +-CPU, 1 cores @ 300MHz\n"
         "|     |   \\-Memory, 256 MiB\n"
         "|     +-Memory, 16384 MiB\n"
         "|     \\-Network: vnet01\n"
         "|       +-Host: monitor5.fit.cvut.cz\n"
         "|       | +-147.32.232.254\n"
         "|       | +-CPU, 2 cores @ 1500MHz\n"
         "|       | +-Memory, 4096 MiB\n"
         "|       | \\-HDD, 750 GiB\n"
         "|       |   +-[0]: 100 GiB, root\n"
         "|       |   \\-[1]: 600 GiB, log\n"
         "|       \\-Host: monitor6.fit.cvut.cz\n"
         "|         +-147.32.232.254\n"
         "|         +-CPU, 2 cores @ 1500MHz\n"
         "|         +-Memory, 4096 MiB\n"
         "|         \\-HDD, 750 GiB\n"
         "|           +-[0]: 100 GiB, root\n"
         "|           \\-[1]: 600 GiB, log\n"
         "\\-Host: vm02.fit.cvut.cz\n"
         "  +-147.32.232.232\n"
         "  +-CPU, 32 cores @ 4000MHz\n"
         "  +-Memory, 32768 MiB\n"
         "  +-HDD, 3000 GiB\n"
         "  | +-[0]: 500 GiB, SYSTEM\n"
         "  | +-[1]: 2000 GiB, DATA\n"
         "  | \\-[2]: 500 GiB, BACKUP\n"
         "  +-Network: dummy00\n"
         "  | \\-Host: testing.fit.cvut.cz\n"
         "  |   +-192.168.1.1\n"
         "  |   +-CPU, 1 cores @ 300MHz\n"
         "  |   \\-Memory, 256 MiB\n"
         "  +-Memory, 16384 MiB\n"
         "  \\-Network: vnet00\n"
         "    +-Host: monitor3.fit.cvut.cz\n"
         "    | +-147.32.232.254\n"
         "    | +-CPU, 2 cores @ 1500MHz\n"
         "    | +-Memory, 4096 MiB\n"
         "    | \\-HDD, 750 GiB\n"
         "    |   +-[0]: 100 GiB, root\n"
         "    |   \\-[1]: 600 GiB, log\n"
         "    +-Host: monitor4.fit.cvut.cz\n"
         "    | +-147.32.232.254\n"
         "    | +-CPU, 2 cores @ 1500MHz\n"
         "    | +-Memory, 4096 MiB\n"
         "    | \\-HDD, 750 GiB\n"
         "    |   +-[0]: 100 GiB, root\n"
         "    |   \\-[1]: 600 GiB, log\n"
         "    \\-Host: vm11.fit.cvut.cz\n"
         "      +-147.32.232.232\n"
         "      +-CPU, 32 cores @ 4000MHz\n"
         "      +-Memory, 32768 MiB\n"
         "      +-HDD, 3000 GiB\n"
         "      | +-[0]: 500 GiB, SYSTEM\n"
         "      | +-[1]: 2000 GiB, DATA\n"
         "      | \\-[2]: 500 GiB, BACKUP\n"
         "      +-Network: dummy00\n"
         "      | \\-Host: testing.fit.cvut.cz\n"
         "      |   +-192.168.1.1\n"
         "      |   +-CPU, 1 cores @ 300MHz\n"
         "      |   \\-Memory, 256 MiB\n"
         "      +-Memory, 16384 MiB\n"
         "      \\-Network: vnet10\n"
         "        +-Host: monitor11.fit.cvut.cz\n"
         "        | +-147.32.232.254\n"
         "        | +-CPU, 2 cores @ 1500MHz\n"
         "        | +-Memory, 4096 MiB\n"
         "        | \\-HDD, 750 GiB\n"
         "        |   +-[0]: 100 GiB, root\n"
         "        |   \\-[1]: 600 GiB, log\n"
         "        +-Host: monitor12.fit.cvut.cz\n"
         "        | +-147.32.232.254\n"
         "        | +-CPU, 2 cores @ 1500MHz\n"
         "        | +-Memory, 4096 MiB\n"
         "        | \\-HDD, 750 GiB\n"
         "        |   +-[0]: 100 GiB, root\n"
         "        |   \\-[1]: 600 GiB, log\n"
         "        \\-Host: vm13.fit.cvut.cz\n"
         "          +-147.32.232.232\n"
         "          +-CPU, 32 cores @ 4000MHz\n"
         "          +-Memory, 32768 MiB\n"
         "          +-HDD, 3000 GiB\n"
         "          | +-[0]: 500 GiB, SYSTEM\n"
         "          | +-[1]: 2000 GiB, DATA\n"
         "          | \\-[2]: 500 GiB, BACKUP\n"
         "          +-Network: dummy00\n"
         "          | \\-Host: testing.fit.cvut.cz\n"
         "          |   +-192.168.1.1\n"
         "          |   +-CPU, 1 cores @ 300MHz\n"
         "          |   \\-Memory, 256 MiB\n"
         "          +-Memory, 16384 MiB\n"
         "          \\-Network: vnet11\n"
         "            +-Host: monitor5.fit.cvut.cz\n"
         "            | +-147.32.232.254\n"
         "            | +-CPU, 2 cores @ 1500MHz\n"
         "            | +-Memory, 4096 MiB\n"
         "            | \\-HDD, 750 GiB\n"
         "            |   +-[0]: 100 GiB, root\n"
         "            |   \\-[1]: 600 GiB, log\n"
         "            \\-Host: monitor6.fit.cvut.cz\n"
         "              +-147.32.232.254\n"
         "              +-CPU, 2 cores @ 1500MHz\n"
         "              +-Memory, 4096 MiB\n"
         "              \\-HDD, 750 GiB\n"
         "                +-[0]: 100 GiB, root\n"
         "                \\-[1]: 600 GiB, log\n");
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
