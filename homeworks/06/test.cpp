#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <typeinfo>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <list>
#include <string>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <memory>
#include <algorithm>
#include <type_traits>
using namespace std;

class CRect
{
public:
  CRect(double x,
        double y,
        double w,
        double h)
      : m_X(x),
        m_Y(y),
        m_W(w),
        m_H(h)
  {
  }
  friend ostream &operator<<(ostream &os,
                             const CRect &x)
  {
    return os << '(' << x.m_X << ',' << x.m_Y << ',' << x.m_W << ',' << x.m_H << ')';
  }
  double m_X;
  double m_Y;
  double m_W;
  double m_H;
};
#endif /* __PROGTEST__ */

class CComponent
{
protected:
  int m_id;
  CRect m_pos;
  CRect m_originalPos;

public:
  CComponent(int id, const CRect &pos) : m_id(id), m_pos(pos), m_originalPos(pos) {}
  CComponent(const CComponent &other) : m_id(other.m_id), m_pos(other.m_pos), m_originalPos(other.m_originalPos) {}
  bool equalID(int id) const { return m_id == id; }
  virtual ~CComponent() {}
  virtual CComponent *clone() const = 0; // uses copy constructor

  virtual void recalculatePosition(const CRect &parentPos)
  {
    m_pos.m_X = (m_originalPos.m_X * parentPos.m_W) + parentPos.m_X;
    m_pos.m_Y = (m_originalPos.m_Y * parentPos.m_H) + parentPos.m_Y;
    m_pos.m_W = (m_originalPos.m_W * parentPos.m_W);
    m_pos.m_H = (m_originalPos.m_H * parentPos.m_H);
  }

  virtual operator CComponent *() = 0;

  virtual std::ostream &print(std::ostream &os) const = 0;
  virtual std::ostream &print_layer(std::ostream &os, bool prefix = false) const
  {
    return print(os);
  }

  friend std::ostream &operator<<(std::ostream &os, const CComponent &cmp) { return cmp.print(os); }
};

class CButton : public CComponent
{
private:
  std::string m_name;

public:
  CButton(int id, const CRect &relPos, const string &name)
      : CComponent(id, relPos), m_name(name) {}

  CButton(const CButton &other) // cpy constructor
      : CComponent(other), m_name(other.m_name)
  {
  }

  ~CButton() override {}

  CComponent *clone() const override
  {
    return new CButton(*this);
  }

  operator CComponent *() override { return this->clone(); }

  std::ostream &print(std::ostream &os) const override
  {
    return os << "[" << m_id << "] "
              << "Button "
              << "\"" << m_name << "\" " << m_pos;
  }
};

class CInput : public CComponent
{
private:
  std::string m_value;

public:
  CInput(int id, const CRect &relPos, const string &value)
      : CComponent(id, relPos), m_value(value) {}

  CInput(const CInput &other)
      : CComponent(other), m_value(other.m_value) {}

  ~CInput() override {}

  CComponent *clone() const override
  {
    return new CInput(*this);
  }
  operator CComponent *() override { return this->clone(); }

  void setValue(const std::string &val) { m_value = val; }

  std::string getValue() const { return m_value; }

  std::ostream &print(std::ostream &os) const override
  {
    return os << "[" << m_id << "] "
              << "Input "
              << "\"" << m_value << "\" " << m_pos;
  }
};

class CLabel : public CComponent
{
private:
  std::string m_label;

public:
  CLabel(int id, const CRect &relPos, const string &label)
      : CComponent(id, relPos), m_label(label) {}

  CLabel(const CLabel &other)
      : CComponent(other), m_label(other.m_label) {}

  ~CLabel() override {}

  operator CComponent *() override { return new CLabel(*this); }

  CComponent *clone() const override
  {
    return new CLabel(*this);
  }

  std::ostream &print(std::ostream &os) const override
  {
    return os << "[" << m_id << "] "
              << "Label "
              << "\"" << m_label << "\" " << m_pos;
  }
};

class CComboBox : public CComponent
{
private:
  std::vector<std::string> m_options;
  int m_selected;

public:
  CComboBox(int id, const CRect &relPos)
      : CComponent(id, relPos), m_selected(0) {}

  CComboBox(const CComboBox &other)
      : CComponent(other), m_options(other.m_options), m_selected(other.m_selected) {}

  ~CComboBox() override
  {
  }

  CComponent *clone() const override
  {
    return new CComboBox(*this);
  }

  operator CComponent *() override { return this->clone(); }

  CComboBox &add(const std::string &x)
  {
    m_options.push_back(x);
    return *this;
  }

  void setSelected(int x) { m_selected = x; }

  int getSelected() const { return m_selected; }

  std::vector<std::string> get_options() const { return m_options; }

  std::ostream &print_layer(std::ostream &os, bool prefix = false) const override
  {
    os << "[" << m_id << "] "
       << "ComboBox " << m_pos << std::endl;

    std::stringstream ss;
    for (auto item : m_options)
    {
      if (prefix)
      {
        if (item == m_options[m_selected])
          ss << "|  +->" << item << "<"
             << "\n";
        else
          ss << "|  +- " << item << "\n";
      }
      else
      {
        if (item == m_options[m_selected])
          ss << "   +->" << item << "<"
             << "\n";
        else
          ss << "   +- " << item << "\n";
      }
    }
    std::string line;
    while (std::getline(ss, line))
    {
      if (line.empty())
        continue;
      os << line << std::endl;
    }
    return os;
  }

  std::ostream &print(std::ostream &os) const override
  {
    os << "[" << m_id << "] "
       << "ComboBox " << m_pos << std::endl;

    std::stringstream ss;
    for (auto item : m_options)
    {
      if (item == m_options[m_selected])
        ss << "+->" << item << "<"
           << "\n";
      else
        ss << "+- " << item << "\n";
    }

    std::string line;
    while (std::getline(ss, line))
    {
      if (line.empty())
        continue;
      os << line << std::endl;
    }
    return os;
  }
};

class CPanel : public CComponent
{
private:
  std::vector<CComponent *> m_controls;

public:
  CPanel(int id, const CRect &relPos) : CComponent(id, relPos) {}
  CPanel(const CPanel &other) : CComponent(other)
  {
    for (const auto &control : other.m_controls)
    {
      m_controls.push_back(control->clone());
    }
  }

  ~CPanel() override
  {
    for (auto &control : m_controls)
    {
      delete control;
    }
  }

  CComponent *clone() const override
  {
    return new CPanel(*this);
  }

  CPanel &operator=(const CPanel &other)
  {
    if (this != &other)
    {
      m_id = other.m_id;
      m_pos = other.m_pos;
      m_originalPos = other.m_originalPos;

      // delete old controls
      for (auto &control : m_controls)
      {
        delete control;
      }
      m_controls.clear();

      // copy new controls
      for (const auto &control : other.m_controls)
      {
        m_controls.push_back(control->clone());
      }
    }
    return *this;
  }
  CPanel &add(CComponent *x)
  {
    x->recalculatePosition(m_pos);
    m_controls.push_back(x);
    return *this;
  }

  operator CComponent *() override { return new CPanel(*this); }

  std::vector<CComponent *> get_controls() const { return m_controls; }

  CRect pos() const { return m_pos; }

  void recalculatePosition(const CRect &parentPos) override
  {
    m_pos.m_X = (m_originalPos.m_X * parentPos.m_W) + parentPos.m_X;
    m_pos.m_Y = (m_originalPos.m_Y * parentPos.m_H) + parentPos.m_Y;
    m_pos.m_W = (m_originalPos.m_W * parentPos.m_W);
    m_pos.m_H = (m_originalPos.m_H * parentPos.m_H);
    for (auto &control : m_controls)
    {
      control->recalculatePosition(m_pos);
    }
  }

  CComponent *search(int id)
  {
    if (m_id == id)
      return this;
    for (const auto &control : m_controls)
    {
      if (control->equalID(id))
      {
        return control;
      }
    }
    return nullptr;
  }
  std::ostream &print_layer(std::ostream &os, bool prefix = false) const override
  {
    os << "[" << m_id << "] "
       << "Panel " << m_pos << std::endl;

    std::stringstream ss;
    for (auto item : m_controls)
    {
      ss << "+- ";

      if (item != m_controls.back())
        item->print_layer(ss, true);
      else
        item->print_layer(ss);

      ss << "\n";
    }
    std::string line;
    while (std::getline(ss, line))
    {
      if (line.empty())
        continue;
      if (prefix)
        os << "|  " << line << "\n";
      else
        os << "   " << line << "\n";
    }
    return os;
  }

  std::ostream &print(std::ostream &os) const override
  {
    os << "[" << m_id << "] "
       << "Panel " << m_pos << std::endl;

    std::stringstream ss;
    for (auto item : m_controls)
    {
      ss << "+- ";
      if (item != m_controls.back())
        item->print_layer(ss, true);
      else
        item->print_layer(ss);
      ss << "\n";
    }
    std::string line;
    while (std::getline(ss, line))
    {
      if (line.empty())
        continue;
      os << line << "\n";
    }
    return os;
  }
};

class CWindow
{
private:
  int m_id;
  std::string m_title;
  CRect m_pos;

  // controls
  std::vector<CComponent *> m_controls;

public:
  CWindow(int id, const string &title, const CRect &absPos)
      : m_id(id), m_title(title), m_pos(absPos) {}

  // copy constructor
  CWindow(const CWindow &other)
      : m_id(other.m_id), m_title(other.m_title), m_pos(other.m_pos)
  {
    // copy controls
    for (const auto &control : other.m_controls)
    {
      m_controls.push_back(control->clone());
    }
  }

  // copy assignment operator
  CWindow &operator=(const CWindow &other)
  {
    if (this != &other)
    {
      // copy id, title, and position
      m_id = other.m_id;
      m_title = other.m_title;
      m_pos = other.m_pos;

      // delete old controls
      for (auto &control : m_controls)
      {
        delete control;
      }
      m_controls.clear();

      // copy new controls
      for (const auto &control : other.m_controls)
      {
        m_controls.push_back(control->clone());
      }
    }
    return *this;
  }

  ~CWindow()
  {
    for (auto &control : m_controls)
    {
      delete control;
    }
  }

  // add
  CWindow &add(CComponent *x)
  {
    x->recalculatePosition(m_pos);
    if (dynamic_cast<CPanel *>(x))
    {
      auto panel = dynamic_cast<CPanel *>(x);
      for (auto &control : panel->get_controls())
      {
        control->recalculatePosition(panel->pos());
      }
    }
    m_controls.push_back(x);
    return *this;
  }

  // search
  CComponent *search(int id)
  {
    for (const auto &component : m_controls)
    {
      if (auto panel = dynamic_cast<CPanel *>(component))
      {
        return panel->search(id);
      }
      if (component->equalID(id))
      {
        return component;
      }
    }
    return nullptr;
  }

  // setPosition
  void setPosition(const CRect &newPos)
  {
    m_pos = newPos;
    for (auto &component : m_controls)
    {
      component->recalculatePosition(m_pos);
    }
  }

  std::ostream &print(std::ostream &os, size_t indent) const
  {
    os << "[" << m_id << "] "
       << "Window "
       << "\"" << m_title << "\" " << m_pos << std::endl;

    std::stringstream ss;
    for (auto item : m_controls)
    {
      ss << "+- ";
      if (item != m_controls.back())
        item->print_layer(ss, true);
      else
        item->print_layer(ss);
      ss << "\n";
    }

    std::string line;
    while (std::getline(ss, line))
    {
      if (line.empty())
        continue;
      else
      {
        os << line << "\n";
      }
    }

    return os;
  }
  friend std::ostream &operator<<(std::ostream &os, const CWindow &window);
};

std::ostream &operator<<(std::ostream &os, const CWindow &window)
{
  return window.print(os, 0);
}

#ifndef __PROGTEST__
template <typename T_>
string toString(const T_ &x)
{
  ostringstream oss;
  oss << x;
  return oss.str();
}

int main(void)
{
  assert(sizeof(CButton) - sizeof(string) < sizeof(CComboBox) - sizeof(vector<string>));
  assert(sizeof(CInput) - sizeof(string) < sizeof(CComboBox) - sizeof(vector<string>));
  assert(sizeof(CLabel) - sizeof(string) < sizeof(CComboBox) - sizeof(vector<string>));
  assert(sizeof(CButton) - sizeof(string) <= sizeof(CPanel) - sizeof(vector<void *>));
  assert(sizeof(CInput) - sizeof(string) <= sizeof(CPanel) - sizeof(vector<void *>));
  assert(sizeof(CLabel) - sizeof(string) <= sizeof(CPanel) - sizeof(vector<void *>));
  CWindow a(0, "Sample window", CRect(10, 10, 600, 480));
  a.add(CButton(1, CRect(0.1, 0.8, 0.3, 0.1), "Ok")).add(CButton(2, CRect(0.6, 0.8, 0.3, 0.1), "Cancel"));
  a.add(CLabel(10, CRect(0.1, 0.1, 0.2, 0.1), "Username:"));
  a.add(CInput(11, CRect(0.4, 0.1, 0.5, 0.1), "chucknorris"));
  a.add(CPanel(12, CRect(0.1, 0.3, 0.8, 0.7)).add(CComboBox(20, CRect(0.1, 0.3, 0.8, 0.1)).add("Karate").add("Judo").add("Box").add("Progtest")));
  assert(toString(a) ==
         "[0] Window \"Sample window\" (10,10,600,480)\n"
         "+- [1] Button \"Ok\" (70,394,180,48)\n"
         "+- [2] Button \"Cancel\" (370,394,180,48)\n"
         "+- [10] Label \"Username:\" (70,58,120,48)\n"
         "+- [11] Input \"chucknorris\" (250,58,300,48)\n"
         "+- [12] Panel (70,154,480,336)\n"
         "   +- [20] ComboBox (118,254.8,384,33.6)\n"
         "      +->Karate<\n"
         "      +- Judo\n"
         "      +- Box\n"
         "      +- Progtest\n");
  CWindow b = a;
  assert(toString(*b.search(20)) ==
         "[20] ComboBox (118,254.8,384,33.6)\n"
         "+->Karate<\n"
         "+- Judo\n"
         "+- Box\n"
         "+- Progtest\n");
  assert(dynamic_cast<CComboBox &>(*b.search(20)).getSelected() == 0);
  dynamic_cast<CComboBox &>(*b.search(20)).setSelected(3);
  assert(dynamic_cast<CInput &>(*b.search(11)).getValue() == "chucknorris");
  dynamic_cast<CInput &>(*b.search(11)).setValue("chucknorris@fit.cvut.cz");
  CPanel &p = dynamic_cast<CPanel &>(*b.search(12));
  p.add(CComboBox(21, CRect(0.1, 0.5, 0.8, 0.1)).add("PA2").add("OSY").add("Both"));
  assert(toString(b) ==
         "[0] Window \"Sample window\" (10,10,600,480)\n"
         "+- [1] Button \"Ok\" (70,394,180,48)\n"
         "+- [2] Button \"Cancel\" (370,394,180,48)\n"
         "+- [10] Label \"Username:\" (70,58,120,48)\n"
         "+- [11] Input \"chucknorris@fit.cvut.cz\" (250,58,300,48)\n"
         "+- [12] Panel (70,154,480,336)\n"
         "   +- [20] ComboBox (118,254.8,384,33.6)\n"
         "   |  +- Karate\n"
         "   |  +- Judo\n"
         "   |  +- Box\n"
         "   |  +->Progtest<\n"
         "   +- [21] ComboBox (118,322,384,33.6)\n"
         "      +->PA2<\n"
         "      +- OSY\n"
         "      +- Both\n");
  assert(toString(a) ==
         "[0] Window \"Sample window\" (10,10,600,480)\n"
         "+- [1] Button \"Ok\" (70,394,180,48)\n"
         "+- [2] Button \"Cancel\" (370,394,180,48)\n"
         "+- [10] Label \"Username:\" (70,58,120,48)\n"
         "+- [11] Input \"chucknorris\" (250,58,300,48)\n"
         "+- [12] Panel (70,154,480,336)\n"
         "   +- [20] ComboBox (118,254.8,384,33.6)\n"
         "      +->Karate<\n"
         "      +- Judo\n"
         "      +- Box\n"
         "      +- Progtest\n");
  assert(toString(p) ==
         "[12] Panel (70,154,480,336)\n"
         "+- [20] ComboBox (118,254.8,384,33.6)\n"
         "|  +- Karate\n"
         "|  +- Judo\n"
         "|  +- Box\n"
         "|  +->Progtest<\n"
         "+- [21] ComboBox (118,322,384,33.6)\n"
         "   +->PA2<\n"
         "   +- OSY\n"
         "   +- Both\n");
  b.setPosition(CRect(20, 30, 640, 520));
  assert(toString(b) ==
         "[0] Window \"Sample window\" (20,30,640,520)\n"
         "+- [1] Button \"Ok\" (84,446,192,52)\n"
         "+- [2] Button \"Cancel\" (404,446,192,52)\n"
         "+- [10] Label \"Username:\" (84,82,128,52)\n"
         "+- [11] Input \"chucknorris@fit.cvut.cz\" (276,82,320,52)\n"
         "+- [12] Panel (84,186,512,364)\n"
         "   +- [20] ComboBox (135.2,295.2,409.6,36.4)\n"
         "   |  +- Karate\n"
         "   |  +- Judo\n"
         "   |  +- Box\n"
         "   |  +->Progtest<\n"
         "   +- [21] ComboBox (135.2,368,409.6,36.4)\n"
         "      +->PA2<\n"
         "      +- OSY\n"
         "      +- Both\n");
  p.add(p);
  assert(toString(p) ==
         "[12] Panel (84,186,512,364)\n"
         "+- [20] ComboBox (135.2,295.2,409.6,36.4)\n"
         "|  +- Karate\n"
         "|  +- Judo\n"
         "|  +- Box\n"
         "|  +->Progtest<\n"
         "+- [21] ComboBox (135.2,368,409.6,36.4)\n"
         "|  +->PA2<\n"
         "|  +- OSY\n"
         "|  +- Both\n"
         "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
         "   +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
         "   |  +- Karate\n"
         "   |  +- Judo\n"
         "   |  +- Box\n"
         "   |  +->Progtest<\n"
         "   +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
         "      +->PA2<\n"
         "      +- OSY\n"
         "      +- Both\n");
  p.add(p);
  assert(toString(p) ==
         "[12] Panel (84,186,512,364)\n"
         "+- [20] ComboBox (135.2,295.2,409.6,36.4)\n"
         "|  +- Karate\n"
         "|  +- Judo\n"
         "|  +- Box\n"
         "|  +->Progtest<\n"
         "+- [21] ComboBox (135.2,368,409.6,36.4)\n"
         "|  +->PA2<\n"
         "|  +- OSY\n"
         "|  +- Both\n"
         "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
         "|  +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
         "|  |  +- Karate\n"
         "|  |  +- Judo\n"
         "|  |  +- Box\n"
         "|  |  +->Progtest<\n"
         "|  +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
         "|     +->PA2<\n"
         "|     +- OSY\n"
         "|     +- Both\n"
         "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
         "   +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
         "   |  +- Karate\n"
         "   |  +- Judo\n"
         "   |  +- Box\n"
         "   |  +->Progtest<\n"
         "   +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
         "   |  +->PA2<\n"
         "   |  +- OSY\n"
         "   |  +- Both\n"
         "   +- [12] Panel (176.16,371.64,327.68,178.36)\n"
         "      +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
         "      |  +- Karate\n"
         "      |  +- Judo\n"
         "      |  +- Box\n"
         "      |  +->Progtest<\n"
         "      +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
         "         +->PA2<\n"
         "         +- OSY\n"
         "         +- Both\n");
  p.add(p);
  assert(toString(p) ==
         "[12] Panel (84,186,512,364)\n"
         "+- [20] ComboBox (135.2,295.2,409.6,36.4)\n"
         "|  +- Karate\n"
         "|  +- Judo\n"
         "|  +- Box\n"
         "|  +->Progtest<\n"
         "+- [21] ComboBox (135.2,368,409.6,36.4)\n"
         "|  +->PA2<\n"
         "|  +- OSY\n"
         "|  +- Both\n"
         "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
         "|  +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
         "|  |  +- Karate\n"
         "|  |  +- Judo\n"
         "|  |  +- Box\n"
         "|  |  +->Progtest<\n"
         "|  +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
         "|     +->PA2<\n"
         "|     +- OSY\n"
         "|     +- Both\n"
         "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
         "|  +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
         "|  |  +- Karate\n"
         "|  |  +- Judo\n"
         "|  |  +- Box\n"
         "|  |  +->Progtest<\n"
         "|  +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
         "|  |  +->PA2<\n"
         "|  |  +- OSY\n"
         "|  |  +- Both\n"
         "|  +- [12] Panel (176.16,371.64,327.68,178.36)\n"
         "|     +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
         "|     |  +- Karate\n"
         "|     |  +- Judo\n"
         "|     |  +- Box\n"
         "|     |  +->Progtest<\n"
         "|     +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
         "|        +->PA2<\n"
         "|        +- OSY\n"
         "|        +- Both\n"
         "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
         "   +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
         "   |  +- Karate\n"
         "   |  +- Judo\n"
         "   |  +- Box\n"
         "   |  +->Progtest<\n"
         "   +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
         "   |  +->PA2<\n"
         "   |  +- OSY\n"
         "   |  +- Both\n"
         "   +- [12] Panel (176.16,371.64,327.68,178.36)\n"
         "   |  +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
         "   |  |  +- Karate\n"
         "   |  |  +- Judo\n"
         "   |  |  +- Box\n"
         "   |  |  +->Progtest<\n"
         "   |  +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
         "   |     +->PA2<\n"
         "   |     +- OSY\n"
         "   |     +- Both\n"
         "   +- [12] Panel (176.16,371.64,327.68,178.36)\n"
         "      +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
         "      |  +- Karate\n"
         "      |  +- Judo\n"
         "      |  +- Box\n"
         "      |  +->Progtest<\n"
         "      +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
         "      |  +->PA2<\n"
         "      |  +- OSY\n"
         "      |  +- Both\n"
         "      +- [12] Panel (208.928,425.148,262.144,124.852)\n"
         "         +- [20] ComboBox (235.142,462.604,209.715,12.4852)\n"
         "         |  +- Karate\n"
         "         |  +- Judo\n"
         "         |  +- Box\n"
         "         |  +->Progtest<\n"
         "         +- [21] ComboBox (235.142,487.574,209.715,12.4852)\n"
         "            +->PA2<\n"
         "            +- OSY\n"
         "            +- Both\n");
  assert(toString(b) ==
         "[0] Window \"Sample window\" (20,30,640,520)\n"
         "+- [1] Button \"Ok\" (84,446,192,52)\n"
         "+- [2] Button \"Cancel\" (404,446,192,52)\n"
         "+- [10] Label \"Username:\" (84,82,128,52)\n"
         "+- [11] Input \"chucknorris@fit.cvut.cz\" (276,82,320,52)\n"
         "+- [12] Panel (84,186,512,364)\n"
         "   +- [20] ComboBox (135.2,295.2,409.6,36.4)\n"
         "   |  +- Karate\n"
         "   |  +- Judo\n"
         "   |  +- Box\n"
         "   |  +->Progtest<\n"
         "   +- [21] ComboBox (135.2,368,409.6,36.4)\n"
         "   |  +->PA2<\n"
         "   |  +- OSY\n"
         "   |  +- Both\n"
         "   +- [12] Panel (135.2,295.2,409.6,254.8)\n"
         "   |  +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
         "   |  |  +- Karate\n"
         "   |  |  +- Judo\n"
         "   |  |  +- Box\n"
         "   |  |  +->Progtest<\n"
         "   |  +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
         "   |     +->PA2<\n"
         "   |     +- OSY\n"
         "   |     +- Both\n"
         "   +- [12] Panel (135.2,295.2,409.6,254.8)\n"
         "   |  +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
         "   |  |  +- Karate\n"
         "   |  |  +- Judo\n"
         "   |  |  +- Box\n"
         "   |  |  +->Progtest<\n"
         "   |  +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
         "   |  |  +->PA2<\n"
         "   |  |  +- OSY\n"
         "   |  |  +- Both\n"
         "   |  +- [12] Panel (176.16,371.64,327.68,178.36)\n"
         "   |     +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
         "   |     |  +- Karate\n"
         "   |     |  +- Judo\n"
         "   |     |  +- Box\n"
         "   |     |  +->Progtest<\n"
         "   |     +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
         "   |        +->PA2<\n"
         "   |        +- OSY\n"
         "   |        +- Both\n"
         "   +- [12] Panel (135.2,295.2,409.6,254.8)\n"
         "      +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
         "      |  +- Karate\n"
         "      |  +- Judo\n"
         "      |  +- Box\n"
         "      |  +->Progtest<\n"
         "      +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
         "      |  +->PA2<\n"
         "      |  +- OSY\n"
         "      |  +- Both\n"
         "      +- [12] Panel (176.16,371.64,327.68,178.36)\n"
         "      |  +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
         "      |  |  +- Karate\n"
         "      |  |  +- Judo\n"
         "      |  |  +- Box\n"
         "      |  |  +->Progtest<\n"
         "      |  +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
         "      |     +->PA2<\n"
         "      |     +- OSY\n"
         "      |     +- Both\n"
         "      +- [12] Panel (176.16,371.64,327.68,178.36)\n"
         "         +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
         "         |  +- Karate\n"
         "         |  +- Judo\n"
         "         |  +- Box\n"
         "         |  +->Progtest<\n"
         "         +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
         "         |  +->PA2<\n"
         "         |  +- OSY\n"
         "         |  +- Both\n"
         "         +- [12] Panel (208.928,425.148,262.144,124.852)\n"
         "            +- [20] ComboBox (235.142,462.604,209.715,12.4852)\n"
         "            |  +- Karate\n"
         "            |  +- Judo\n"
         "            |  +- Box\n"
         "            |  +->Progtest<\n"
         "            +- [21] ComboBox (235.142,487.574,209.715,12.4852)\n"
         "               +->PA2<\n"
         "               +- OSY\n"
         "               +- Both\n");
  assert(toString(a) ==
         "[0] Window \"Sample window\" (10,10,600,480)\n"
         "+- [1] Button \"Ok\" (70,394,180,48)\n"
         "+- [2] Button \"Cancel\" (370,394,180,48)\n"
         "+- [10] Label \"Username:\" (70,58,120,48)\n"
         "+- [11] Input \"chucknorris\" (250,58,300,48)\n"
         "+- [12] Panel (70,154,480,336)\n"
         "   +- [20] ComboBox (118,254.8,384,33.6)\n"
         "      +->Karate<\n"
         "      +- Judo\n"
         "      +- Box\n"
         "      +- Progtest\n");
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
