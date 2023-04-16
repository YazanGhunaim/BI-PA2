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
  CRect(double x, double y, double w, double h) : m_X(x), m_Y(y), m_W(w), m_H(h)
  {
  }
  friend ostream &operator<<(ostream &os, const CRect &x)
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

public:
  CComponent(int id, const CRect &pos) : m_id(id), m_pos(pos) {}
  CComponent(const CComponent &other) : m_id(other.m_id), m_pos(other.m_pos) {}
  bool equalID(int id) const { return m_id == id; }
  virtual ~CComponent() {}
  virtual CComponent *clone() const = 0; // uses copy constructor
  virtual void recalculatePosition(const CRect &windowPos)
  {
    m_pos.m_X = (m_pos.m_X * windowPos.m_W) + windowPos.m_X;
    m_pos.m_Y = (m_pos.m_Y * windowPos.m_H) + windowPos.m_Y;
    m_pos.m_W = (m_pos.m_W * windowPos.m_W);
    m_pos.m_H = (m_pos.m_H * windowPos.m_H);
  }

  virtual operator CComponent *() = 0;
  virtual std::ostream &print(std::ostream &os) const = 0;
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

  // << overload
  std::ostream &print(std::ostream &os) const override
  {
    return os << "[" << m_id << "] "
              << "Button "
              << "\"" << m_name << "\" " << m_pos;
  }
  friend std::ostream &operator<<(std::ostream &os, const CButton &button);
};

std::ostream &operator<<(std::ostream &os, const CButton &button)
{
  return button.print(os);
}

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

  // setValue
  void setValue(const std::string &val) { m_value = val; }

  // getValue
  std::string getValue() const { return m_value; }

  std::ostream &print(std::ostream &os) const override
  {
    return os << "[" << m_id << "] "
              << "Input "
              << "\"" << m_value << "\" " << m_pos;
  }
  friend std::ostream &operator<<(std::ostream &os, const CInput &input);
};

std::ostream &operator<<(std::ostream &os, const CInput &input)
{
  return input.print(os);
}

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

  friend std::ostream &operator<<(std::ostream &os, const CLabel &label);
};

std::ostream &operator<<(std::ostream &os, const CLabel &label)
{
  return label.print(os);
}

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
  // add
  CComboBox &add(const std::string &x)
  {
    m_options.push_back(x);
    return *this;
  }
  // setSelected
  void setSelected(int x) { m_selected = x; }
  // getSelected
  std::string getSelected() const { return m_options[m_selected]; }

  std::vector<std::string> get_options() const { return m_options; }
  std::ostream &print(std::ostream &os) const override
  {
    os << "[" << m_id << "] "
       << "ComboBox " << m_pos << std::endl;

    // printing options in box
    for (const auto &option : m_options)
    {
      if (option == m_options[m_selected])
      {
        os << "+->" << option << "<\n";
      }
      else
      {
        os << "+- " << option << "\n";
      }
    }
    return os;
  }

  friend std::ostream &operator<<(std::ostream &os, const CComboBox &box);
};

std::ostream &operator<<(std::ostream &os, const CComboBox &box)
{
  return box.print(os);
}

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

  ~CWindow() {}

  // add
  CWindow &add(CComponent *x)
  {
    x->recalculatePosition(m_pos);
    m_controls.push_back(x);
    return *this;
  }

  // search
  CComponent *search(int id)
  {
    for (const auto &component : m_controls)
    {
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

  friend std::ostream &operator<<(std::ostream &os, const CWindow &window);
};

std::ostream &operator<<(std::ostream &os, const CWindow &window)
{
  os << "[" << window.m_id << "] "
     << "Window "
     << "\"" << window.m_title << "\" " << window.m_pos << std::endl;

  // print controls
  for (const auto &control : window.m_controls)
  {
    // print children
    if (auto comboBox = dynamic_cast<CComboBox *>(control))
    {
      os << "+- ";
      control->print(os);
    }
    else
    {
      os << "+- ";
      control->print(os);
      os << std::endl;
    }
  }
  return os;
}

#ifndef __PROGTEST__
template <typename _T>
string toString(const _T &x)
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
  CWindow a(0, "Sample window", CRect(10, 10, 600, 480));
  a.add(CButton(1, CRect(0.1, 0.8, 0.3, 0.1), "Ok")).add(CButton(2, CRect(0.6, 0.8, 0.3, 0.1), "Cancel"));
  a.add(CLabel(10, CRect(0.1, 0.1, 0.2, 0.1), "Username:"));
  a.add(CInput(11, CRect(0.4, 0.1, 0.5, 0.1), "chucknorris"));
  a.add(CComboBox(20, CRect(0.1, 0.3, 0.8, 0.1)).add("Karate").add("Judo").add("Box").add("Progtest"));

  cout << toString(a);
  // assert(toString(a) ==
  //        "[0] Window \"Sample window\" (10,10,600,480)\n"
  //        "+- [1] Button \"Ok\" (70,394,180,48)\n"
  //        "+- [2] Button \"Cancel\" (370,394,180,48)\n"
  //        "+- [10] Label \"Username:\" (70,58,120,48)\n"
  //        "+- [11] Input \"chucknorris\" (250,58,300,48)\n"
  //        "+- [20] ComboBox (70,154,480,48)\n"
  //        "   +->Karate<\n"
  //        "   +- Judo\n"
  //        "   +- Box\n"
  //        "   +- Progtest\n");

  // CWindow b = a;
  // assert(toString(*b.search(20)) ==
  //        "[20] ComboBox (70,154,480,48)\n"
  //        "+->Karate<\n"
  //        "+- Judo\n"
  //        "+- Box\n"
  //        "+- Progtest\n");
  // assert(dynamic_cast<CComboBox &>(*b.search(20)).getSelected() == 0);
  // dynamic_cast<CComboBox &>(*b.search(20)).setSelected(3);
  // assert(dynamic_cast<CInput &>(*b.search(11)).getValue() == "chucknorris");
  // dynamic_cast<CInput &>(*b.search(11)).setValue("chucknorris@fit.cvut.cz");
  // b.add(CComboBox(21, CRect(0.1, 0.5, 0.8, 0.1)).add("PA2").add("OSY").add("Both"));
  // assert(toString(b) ==
  //        "[0] Window \"Sample window\" (10,10,600,480)\n"
  //        "+- [1] Button \"Ok\" (70,394,180,48)\n"
  //        "+- [2] Button \"Cancel\" (370,394,180,48)\n"
  //        "+- [10] Label \"Username:\" (70,58,120,48)\n"
  //        "+- [11] Input \"chucknorris@fit.cvut.cz\" (250,58,300,48)\n"
  //        "+- [20] ComboBox (70,154,480,48)\n"
  //        "|  +- Karate\n"
  //        "|  +- Judo\n"
  //        "|  +- Box\n"
  //        "|  +->Progtest<\n"
  //        "+- [21] ComboBox (70,250,480,48)\n"
  //        "   +->PA2<\n"
  //        "   +- OSY\n"
  //        "   +- Both\n");
  // assert(toString(a) ==
  //        "[0] Window \"Sample window\" (10,10,600,480)\n"
  //        "+- [1] Button \"Ok\" (70,394,180,48)\n"
  //        "+- [2] Button \"Cancel\" (370,394,180,48)\n"
  //        "+- [10] Label \"Username:\" (70,58,120,48)\n"
  //        "+- [11] Input \"chucknorris\" (250,58,300,48)\n"
  //        "+- [20] ComboBox (70,154,480,48)\n"
  //        "   +->Karate<\n"
  //        "   +- Judo\n"
  //        "   +- Box\n"
  //        "   +- Progtest\n");
  // b.setPosition(CRect(20, 30, 640, 520));
  // assert(toString(b) ==
  //        "[0] Window \"Sample window\" (20,30,640,520)\n"
  //        "+- [1] Button \"Ok\" (84,446,192,52)\n"
  //        "+- [2] Button \"Cancel\" (404,446,192,52)\n"
  //        "+- [10] Label \"Username:\" (84,82,128,52)\n"
  //        "+- [11] Input \"chucknorris@fit.cvut.cz\" (276,82,320,52)\n"
  //        "+- [20] ComboBox (84,186,512,52)\n"
  //        "|  +- Karate\n"
  //        "|  +- Judo\n"
  //        "|  +- Box\n"
  //        "|  +->Progtest<\n"
  //        "+- [21] ComboBox (84,290,512,52)\n"
  //        "   +->PA2<\n"
  //        "   +- OSY\n"
  //        "   +- Both\n");
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
