#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;
#endif /* __PROGTEST__ */

class OutOfBoundsException : public std::exception
{
public:
  virtual const char *what() const noexcept override
  {
    return "index out of bounds";
  }
};

class CString
{
private:
  char *m_buffer;
  size_t m_length;

public:
  CString() : m_buffer(nullptr), m_length(0) {}
  CString(const char *string) : m_length(strlen(string))
  {
    m_buffer = new char[m_length + 1];
    strcpy(m_buffer, string);
  }
  CString(const CString &other) : m_length(other.m_length)
  {
    m_buffer = new char[m_length + 1];
    strcpy(m_buffer, other.m_buffer);
  }
  CString &operator=(const CString &src)
  {
    if (this != &src)
    {
      delete[] m_buffer;
      m_length = src.m_length;
      m_buffer = new char[m_length + 1];
      strcpy(m_buffer, src.m_buffer);
    }
    return *this;
  }
  ~CString()
  {
    delete[] m_buffer;
    m_buffer = nullptr;
    m_length = 0;
  }
  size_t length() const { return m_length; }
  char &operator[](size_t index)
  {
    if (index >= m_length)
    {
      throw OutOfBoundsException();
    }
    return m_buffer[index];
  }
  const char &operator[](size_t index) const
  {
    if (index >= m_length)
    {
      throw OutOfBoundsException();
    }
    return m_buffer[index];
  }
  bool operator==(const CString &other) const
  {
    if (m_length != other.m_length)
    {
      return false;
    }
    return strcmp(m_buffer, other.m_buffer) == 0;
  }
  bool operator!=(const CString &other) const { return !(*this == other); }
  friend std::ostream &operator<<(std::ostream &os, const CString &str);
  bool operator<(const CString &other) const { return strcmp(m_buffer, other.m_buffer); }
};

std::ostream &operator<<(std::ostream &oss, const CString &str)
{
  return oss << str.m_buffer;
}

template <typename T>
class CArray
{
private:
  T *m_data;
  size_t m_size;
  size_t m_capacity;

public:
  CArray() : m_data(), m_size(0), m_capacity(0) {}
  CArray(size_t size) : m_data(new T[size]), m_size(size), m_capacity(size) {}
  CArray(const CArray &other) : m_data(new T[other.m_capacity]), m_size(other.m_size), m_capacity(other.m_capacity)
  {
    for (size_t i = 0; i < other.m_size; ++i)
    {
      m_data[i] = other.m_data[i];
    }
  }
  CArray &operator=(const CArray &src)
  {
    if (this != &src)
    {
      delete[] m_data;
      m_data = new T[src.m_capacity];
      m_size = src.m_size;
      m_capacity = src.m_capacity;
      for (size_t i = 0; i < m_size; ++i)
      {
        m_data[i] = src.m_data[i];
      }
    }
    return *this;
  }
  ~CArray()
  {
    delete[] m_data;
    m_data = nullptr;
    m_size = m_capacity = 0;
  }
  void reserve(size_t capacity)
  {
    if (capacity > m_capacity)
    {
      T *new_data = new T[capacity];
      for (size_t i = 0; i < m_size; i++)
      {
        new_data[i] = m_data[i];
      }
      delete[] m_data;
      m_data = new_data;
      m_capacity = capacity;
    }
  }
  void push_back(const T &value)
  {
    if (m_capacity == m_size)
    {
      reserve(m_capacity * 2 + 1);
    }
    m_data[m_size++] = value;
  }
  T &operator[](size_t index)
  {
    if (index >= m_size)
    {
      throw OutOfBoundsException();
    }
    return m_data[index];
  }
  const T &operator[](size_t index) const
  {
    if (index >= m_size)
    {
      throw OutOfBoundsException();
    }
    return m_data[index];
  }
  size_t size() const
  {
    return m_size;
  }
  size_t capacity() const
  {
    return m_capacity;
  }
  bool empty() const
  {
    return m_size == 0;
  }
  T *begin()
  {
    return m_data;
  }

  const T *begin() const
  {
    return m_data;
  }

  T *end()
  {
    return m_data + m_size;
  }

  const T *end() const
  {
    return m_data + m_size;
  }

  void insert(T *pos, const T &value)
  {
    size_t index = pos - m_data;
    if (m_size == m_capacity)
    {
      reserve(m_capacity * 2 + 1);
    }
    for (size_t i = m_size; i > index; i--)
    {
      m_data[i] = m_data[i - 1];
    }
    m_data[index] = value;
    m_size++;
  }
};

//====================================================================//
class CMail
{
private:
  CString m_from;
  CString m_to;
  CString m_body;

public:
  CMail() : m_from(), m_to(), m_body() {}
  CMail(const char *from, const char *to, const char *body) : m_from(from), m_to(to), m_body(body) {}
  bool operator==(const CMail &x) const
  {
    return m_from == x.m_from && m_to == x.m_to && m_body == x.m_body;
  }
  CString from() const { return m_from; }
  CString to() const { return m_to; }
  CString body() const { return m_body; }
  friend ostream &operator<<(ostream &os, const CMail &m);
  bool operator<(const CMail &other) const
  {
    if (m_from < other.m_from)
    {
      return true;
    }
    else if (m_from == other.m_from)
    {
      if (m_to < other.m_to)
      {
        return true;
      }
      else if (m_to == other.m_to)
      {
        return m_body < other.m_body;
      }
    }
    return false;
  }
};

std::ostream &operator<<(std::ostream &os, const CMail &m)
{
  return os << "From: " << m.m_from << ", "
            << "To: " << m.m_to << ", "
            << "Body: " << m.m_body;
}

class CMailIterator
{
private:
  CArray<CMail> m_mailbox;
  size_t m_index;

public:
  CMailIterator() : m_mailbox(), m_index(0) {}
  CMailIterator(const CArray<CMail> &mailbox) : m_mailbox(mailbox), m_index(0) {}
  bool operator!() const { return m_index >= m_mailbox.size(); }
  operator bool() const { return m_index < m_mailbox.size(); }
  const CMail &operator*() const { return m_mailbox[m_index]; }
  CMailIterator &operator++()
  {
    m_index++;
    return *this;
  }
};

class CMailServer
{
private:
  struct Mailbox
  {
    CString m_user;
    CArray<CMail> m_inbox;
    CArray<CMail> m_outbox;
  };
  CArray<Mailbox> m_server;

public:
  CMailServer(void) {}
  CMailServer(const CMailServer &src) : m_server(src.m_server) {}
  CMailServer &operator=(const CMailServer &src)
  {
    if (this != &src)
    {
      m_server = src.m_server;
    }
    return *this;
  }
  ~CMailServer(void) {}
  void sendMail(const CMail &m)
  {
    // Find the sender mailbox
    Mailbox *sender = nullptr;
    for (size_t i = 0; i < m_server.size(); i++)
    {
      if (m_server[i].m_user == m.from())
      {
        sender = &m_server[i];
        break;
      }
    }

    // Create the sender mailbox if it doesn't exist
    if (sender == nullptr)
    {
      Mailbox newMailbox;
      newMailbox.m_user = m.from();
      m_server.push_back(newMailbox);
      sender = &m_server[m_server.size() - 1];
    }

    // Add the mail to the sender's outbox in sorted order
    auto it = std::lower_bound(sender->m_outbox.begin(), sender->m_outbox.end(), m);
    sender->m_outbox.insert(it, m);

    // Find the receiver mailbox
    Mailbox *receiver = nullptr;
    for (size_t i = 0; i < m_server.size(); i++)
    {
      if (m_server[i].m_user == m.to())
      {
        receiver = &m_server[i];
        break;
      }
    }

    // Create the receiver mailbox if it doesn't exist
    if (receiver == nullptr)
    {
      Mailbox newMailbox;
      newMailbox.m_user = m.to();
      m_server.push_back(newMailbox);
      receiver = &m_server[m_server.size() - 1];
    }

    // Add the mail to the receiver's inbox in sorted order
    it = std::lower_bound(receiver->m_inbox.begin(), receiver->m_inbox.end(), m);
    receiver->m_inbox.insert(it, m);
  }
  CMailIterator outbox(const char *email) const
  {
    // Find the mailbox for the given email address
    const Mailbox *mailbox = nullptr;
    for (size_t i = 0; i < m_server.size(); i++)
    {
      if (m_server[i].m_user == email)
      {
        mailbox = &m_server[i];
        break;
      }
    }

    // If the mailbox doesn't exist, return an invalid iterator
    if (mailbox == nullptr)
    {
      return CMailIterator();
    }

    // Otherwise, return an iterator to the outbox of the mailbox
    return CMailIterator(mailbox->m_outbox);
  }
  CMailIterator inbox(const char *email) const
  {
    // Find the mailbox for the given email address
    const Mailbox *mailbox = nullptr;
    for (size_t i = 0; i < m_server.size(); i++)
    {
      if (m_server[i].m_user == email)
      {
        mailbox = &m_server[i];
        break;
      }
    }

    // If the mailbox doesn't exist, return an invalid iterator
    if (mailbox == nullptr)
    {
      return CMailIterator();
    }

    // Otherwise, return an iterator to the inbox of the mailbox
    return CMailIterator(mailbox->m_inbox);
  }
};

#ifndef __PROGTEST__
bool matchOutput(const CMail &m, const char *str)
{
  ostringstream oss;
  oss << m;
  return oss.str() == str;
}

int main(void)
{
  char from[100], to[100], body[1024];

  assert(CMail("john", "peter", "progtest deadline") == CMail("john", "peter", "progtest deadline"));
  assert(!(CMail("john", "peter", "progtest deadline") == CMail("john", "progtest deadline", "peter")));
  assert(!(CMail("john", "peter", "progtest deadline") == CMail("peter", "john", "progtest deadline")));
  assert(!(CMail("john", "peter", "progtest deadline") == CMail("peter", "progtest deadline", "john")));
  assert(!(CMail("john", "peter", "progtest deadline") == CMail("progtest deadline", "john", "peter")));
  assert(!(CMail("john", "peter", "progtest deadline") == CMail("progtest deadline", "peter", "john")));
  CMailServer s0;
  s0.sendMail(CMail("john", "peter", "some important mail"));
  strncpy(from, "john", sizeof(from));
  strncpy(to, "thomas", sizeof(to));
  strncpy(body, "another important mail", sizeof(body));
  s0.sendMail(CMail(from, to, body));
  strncpy(from, "john", sizeof(from));
  strncpy(to, "alice", sizeof(to));
  strncpy(body, "deadline notice", sizeof(body));
  s0.sendMail(CMail(from, to, body));
  s0.sendMail(CMail("alice", "john", "deadline confirmation"));
  s0.sendMail(CMail("peter", "alice", "PR bullshit"));
  CMailIterator i0 = s0.inbox("alice");
  assert(i0 && *i0 == CMail("john", "alice", "deadline notice"));
  assert(matchOutput(*i0, "From: john, To: alice, Body: deadline notice"));
  assert(++i0 && *i0 == CMail("peter", "alice", "PR bullshit"));
  assert(matchOutput(*i0, "From: peter, To: alice, Body: PR bullshit"));
  assert(!++i0);

  CMailIterator i1 = s0.inbox("john");
  assert(i1 && *i1 == CMail("alice", "john", "deadline confirmation"));
  assert(matchOutput(*i1, "From: alice, To: john, Body: deadline confirmation"));
  assert(!++i1);

  CMailIterator i2 = s0.outbox("john");
  assert(i2 && *i2 == CMail("john", "peter", "some important mail"));
  assert(matchOutput(*i2, "From: john, To: peter, Body: some important mail"));
  assert(++i2 && *i2 == CMail("john", "thomas", "another important mail"));
  assert(matchOutput(*i2, "From: john, To: thomas, Body: another important mail"));
  assert(++i2 && *i2 == CMail("john", "alice", "deadline notice"));
  assert(matchOutput(*i2, "From: john, To: alice, Body: deadline notice"));
  assert(!++i2);

  CMailIterator i3 = s0.outbox("thomas");
  assert(!i3);

  CMailIterator i4 = s0.outbox("steve");
  assert(!i4);

  CMailIterator i5 = s0.outbox("thomas");
  s0.sendMail(CMail("thomas", "boss", "daily report"));
  assert(!i5);

  CMailIterator i6 = s0.outbox("thomas");
  assert(i6 && *i6 == CMail("thomas", "boss", "daily report"));
  assert(matchOutput(*i6, "From: thomas, To: boss, Body: daily report"));
  assert(!++i6);

  CMailIterator i7 = s0.inbox("alice");
  s0.sendMail(CMail("thomas", "alice", "meeting details"));
  assert(i7 && *i7 == CMail("john", "alice", "deadline notice"));
  assert(matchOutput(*i7, "From: john, To: alice, Body: deadline notice"));
  assert(++i7 && *i7 == CMail("peter", "alice", "PR bullshit"));
  assert(matchOutput(*i7, "From: peter, To: alice, Body: PR bullshit"));
  assert(!++i7);

  CMailIterator i8 = s0.inbox("alice");
  assert(i8 && *i8 == CMail("john", "alice", "deadline notice"));
  assert(matchOutput(*i8, "From: john, To: alice, Body: deadline notice"));
  assert(++i8 && *i8 == CMail("peter", "alice", "PR bullshit"));
  assert(matchOutput(*i8, "From: peter, To: alice, Body: PR bullshit"));
  assert(++i8 && *i8 == CMail("thomas", "alice", "meeting details"));
  assert(matchOutput(*i8, "From: thomas, To: alice, Body: meeting details"));
  assert(!++i8);

  CMailServer s1(s0);
  s0.sendMail(CMail("joe", "alice", "delivery details"));
  s1.sendMail(CMail("sam", "alice", "order confirmation"));
  CMailIterator i9 = s0.inbox("alice");
  assert(i9 && *i9 == CMail("john", "alice", "deadline notice"));
  assert(matchOutput(*i9, "From: john, To: alice, Body: deadline notice"));
  assert(++i9 && *i9 == CMail("peter", "alice", "PR bullshit"));
  assert(matchOutput(*i9, "From: peter, To: alice, Body: PR bullshit"));
  assert(++i9 && *i9 == CMail("thomas", "alice", "meeting details"));
  assert(matchOutput(*i9, "From: thomas, To: alice, Body: meeting details"));
  assert(++i9 && *i9 == CMail("joe", "alice", "delivery details"));
  assert(matchOutput(*i9, "From: joe, To: alice, Body: delivery details"));
  assert(!++i9);

  CMailIterator i10 = s1.inbox("alice");
  assert(i10 && *i10 == CMail("john", "alice", "deadline notice"));
  assert(matchOutput(*i10, "From: john, To: alice, Body: deadline notice"));
  assert(++i10 && *i10 == CMail("peter", "alice", "PR bullshit"));
  assert(matchOutput(*i10, "From: peter, To: alice, Body: PR bullshit"));
  assert(++i10 && *i10 == CMail("thomas", "alice", "meeting details"));
  assert(matchOutput(*i10, "From: thomas, To: alice, Body: meeting details"));
  assert(++i10 && *i10 == CMail("sam", "alice", "order confirmation"));
  assert(matchOutput(*i10, "From: sam, To: alice, Body: order confirmation"));
  assert(!++i10);

  CMailServer s2;
  s2.sendMail(CMail("alice", "alice", "mailbox test"));
  CMailIterator i11 = s2.inbox("alice");
  assert(i11 && *i11 == CMail("alice", "alice", "mailbox test"));
  assert(matchOutput(*i11, "From: alice, To: alice, Body: mailbox test"));
  assert(!++i11);

  s2 = s0;
  s0.sendMail(CMail("steve", "alice", "newsletter"));
  s2.sendMail(CMail("paul", "alice", "invalid invoice"));
  CMailIterator i12 = s0.inbox("alice");
  assert(i12 && *i12 == CMail("john", "alice", "deadline notice"));
  assert(matchOutput(*i12, "From: john, To: alice, Body: deadline notice"));
  assert(++i12 && *i12 == CMail("peter", "alice", "PR bullshit"));
  assert(matchOutput(*i12, "From: peter, To: alice, Body: PR bullshit"));
  assert(++i12 && *i12 == CMail("thomas", "alice", "meeting details"));
  assert(matchOutput(*i12, "From: thomas, To: alice, Body: meeting details"));
  assert(++i12 && *i12 == CMail("joe", "alice", "delivery details"));
  assert(matchOutput(*i12, "From: joe, To: alice, Body: delivery details"));
  assert(++i12 && *i12 == CMail("steve", "alice", "newsletter"));
  assert(matchOutput(*i12, "From: steve, To: alice, Body: newsletter"));
  assert(!++i12);

  CMailIterator i13 = s2.inbox("alice");
  assert(i13 && *i13 == CMail("john", "alice", "deadline notice"));
  assert(matchOutput(*i13, "From: john, To: alice, Body: deadline notice"));
  assert(++i13 && *i13 == CMail("peter", "alice", "PR bullshit"));
  assert(matchOutput(*i13, "From: peter, To: alice, Body: PR bullshit"));
  assert(++i13 && *i13 == CMail("thomas", "alice", "meeting details"));
  assert(matchOutput(*i13, "From: thomas, To: alice, Body: meeting details"));
  assert(++i13 && *i13 == CMail("joe", "alice", "delivery details"));
  assert(matchOutput(*i13, "From: joe, To: alice, Body: delivery details"));
  assert(++i13 && *i13 == CMail("paul", "alice", "invalid invoice"));
  assert(matchOutput(*i13, "From: paul, To: alice, Body: invalid invoice"));
  assert(!++i13);

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
