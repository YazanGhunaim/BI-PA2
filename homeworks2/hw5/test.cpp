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

using ComponentPtr = std::shared_ptr<CComponent>;
using ComputerPtr = std::shared_ptr<CComputer>;

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
    os << (m_disk_type == SSD ? "SSD" : "HDD");
    os << ", " << std::to_string(m_size) << " GiB\n";

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

    os << (m_disk_type == SSD ? "SSD" : "HDD");
    os << ", " << std::to_string(m_size) << " GiB\n";

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

  CComputer &addComponent(const CComponent &component)
  {
    m_components.push_back(component.clone());
    return *this;
  }

  ComputerPtr clone() const
  {
    return std::make_shared<CComputer>(*this);
  }

  CComputer &addAddress(const std::string &address)
  {
    m_addresses.push_back(address);
    return *this;
  }

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

class CNetwork
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

  CNetwork &addComputer(const CComputer &computer)
  {
    m_computers.push_back(computer.clone());
    return *this;
  }

  ComputerPtr findComputer(const std::string &name)
  {
    for (auto &computer : m_computers)
    {
      if (computer->getName() == name)
      {
        return computer;
      }
    }

    return nullptr;
  }

  std::ostream &print(std::ostream &os) const
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
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
