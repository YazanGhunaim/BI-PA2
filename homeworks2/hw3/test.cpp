#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <memory>
#include <stdexcept>
#endif /* __PROGTEST__ */

class CString
{
private:
  char *m_buffer;
  size_t m_length;

public:
  // constructor
  CString();
  // cstring constructor
  CString(const char *str);
  // copy constructor
  CString(const CString &src);
  // desctructor
  ~CString();
  // assignment operator
  CString &operator=(const CString &src);
  // index [] operator
  char &operator[](size_t index);
  // index [] operator const
  const char &operator[](size_t index) const;
  // length
  size_t length() const;
  // toStr
  const char *toStr() const;
  // << operator
  friend std::ostream &operator<<(std::ostream &os, const CString &src);
};

// -------------------------------- //

CString::CString() {}

CString::CString(const char *str) : m_length(strlen(str))
{
  m_buffer = new char[m_length + 1];
  strcpy(m_buffer, str);
}

CString::CString(const CString &src) : m_length(src.m_length)
{
  m_buffer = new char[m_length + 1];
  strcpy(m_buffer, src.m_buffer);
}

CString::~CString()
{
  delete[] m_buffer;
}

CString &CString::operator=(const CString &src)
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

char &CString::operator[](size_t index)
{
  if (index >= m_length)
  {
    throw std::out_of_range("Index out of range.");
  }
  return m_buffer[index];
}

size_t CString::length() const
{
  return m_length;
}

const char &CString::operator[](size_t index) const
{
  if (index >= m_length)
  {
    throw std::out_of_range("Index out of range.");
  }
  return m_buffer[index];
}

const char *CString::toStr() const
{
  return m_buffer;
}

std::ostream &operator<<(std::ostream &os, const CString &src)
{
  return os << src.m_buffer;
}

// -------------------------------- //

class CPatchStr
{
private:
  struct SPatch
  {
    size_t m_offset;
    size_t m_length;
    std::shared_ptr<CString> m_ptr;

    SPatch() : m_offset(0), m_length(0) {}

    SPatch(size_t offset, size_t length, std::shared_ptr<CString> ptr)
        : m_offset(offset), m_length(length), m_ptr(ptr) {}

    SPatch(const SPatch &other)
        : m_offset(other.m_offset), m_length(other.m_length), m_ptr(other.m_ptr) {}

    SPatch &operator=(const SPatch &other)
    {
      if (this != &other)
      {
        m_offset = other.m_offset;
        m_length = other.m_length;
        m_ptr = other.m_ptr;
      }
      return *this;
    }

    void print() const
    {
      std::cout << "ofs: " << m_offset << " len: " << m_length << " ptr: " << m_ptr << " " << m_ptr->toStr() << std::endl;
    }
  };

  size_t m_size;
  size_t m_capacity;
  size_t m_length;
  SPatch *m_patches;

  void insert_patch(size_t pos, const SPatch &patch)
  {
    size_t new_size = m_size + 1;
    if (new_size > m_capacity)
    {
      m_capacity = std::max(m_capacity * 2, new_size);
      SPatch *new_patches = new SPatch[m_capacity];

      for (size_t i = 0; i < m_size; ++i)
      {
        new_patches[i] = m_patches[i];
      }

      delete[] m_patches;
      m_patches = new_patches;
    }

    for (size_t i = m_size; i >= pos; --i)
    {
      m_patches[i] = m_patches[i - 1];
    }

    m_patches[pos - 1] = patch;

    m_length += 1;
    m_size = new_size;
  }
  void split_patch(size_t pos)
  {
    size_t total_characters = 0;
    size_t i = 0;
    while (i < m_size)
    {
      if (total_characters + m_patches[i].m_length >= pos)
      {
        size_t pivot = pos - total_characters;
        SPatch new_patch = SPatch{pivot, m_patches[i].m_length - pivot, m_patches[i].m_ptr};
        m_patches[i].m_length = pivot;

        insert_patch(i + 2, new_patch);
        break;
      }
      total_characters += m_patches[i++].m_length;
    }
  }

public:
  CPatchStr();
  CPatchStr(const char *str);
  CPatchStr(const SPatch &src);
  CPatchStr(const CPatchStr &src);
  ~CPatchStr();
  // operator =
  CPatchStr &operator=(const CPatchStr &src);
  CPatchStr subStr(size_t from, size_t len) const;
  CPatchStr &append(const CPatchStr &src);

  CPatchStr &insert(size_t pos, const CPatchStr &src);
  CPatchStr &remove(size_t from, size_t len);
  char *toStr() const;

  void print() const
  {
    for (size_t i = 0; i < m_size; ++i)
    {
      m_patches[i].print();
    }
  }
};

// -------------------------------- //
CPatchStr::CPatchStr() : m_size(0), m_capacity(1), m_length(0), m_patches(new SPatch[1]) {}

CPatchStr::CPatchStr(const char *str) : m_size(1), m_capacity(1), m_length(strlen(str)), m_patches(new SPatch[1])
{
  m_patches[0].m_offset = 0;
  m_patches[0].m_length = m_length;
  m_patches[0].m_ptr = std::make_shared<CString>(str);
}

CPatchStr::CPatchStr(const SPatch &src) : m_size(1), m_capacity(1), m_length(src.m_length), m_patches(new SPatch[1])
{
  m_patches[0] = src;
}

CPatchStr::CPatchStr(const CPatchStr &src) : m_size(src.m_size), m_capacity(src.m_capacity), m_length(src.m_length), m_patches(new SPatch[src.m_capacity])
{
  for (size_t i = 0; i < m_size; ++i)
  {
    m_patches[i] = src.m_patches[i];
  }
}

CPatchStr::~CPatchStr()
{
  delete[] m_patches;
}

CPatchStr &CPatchStr::operator=(const CPatchStr &src)
{
  if (this != &src)
  {
    delete[] m_patches;
    m_size = src.m_size;
    m_capacity = src.m_capacity;
    m_length = src.m_length;
    m_patches = new SPatch[m_capacity];
    for (size_t i = 0; i < m_size; ++i)
    {
      m_patches[i] = src.m_patches[i];
    }
  }
  return *this;
}

CPatchStr CPatchStr::subStr(size_t from, size_t len) const
{
  if (len == 0)
  {
    return {};
  }

  if (from + len > m_length)
  {
    throw std::out_of_range("from + len greater than this length.");
  }

  CPatchStr result;

  size_t num_of_characters_passed = 0;
  size_t num_of_characters_taken = 0;
  bool first = true;
  for (size_t i = 0; i < m_size; ++i)
  {
    auto patch_data = m_patches[i];
    // if we can start
    if (num_of_characters_passed + (patch_data.m_length) >= from)
    {
      if (num_of_characters_taken == len)
      {
        break;
      }
      if (first)
      {
        size_t offset = from - num_of_characters_passed;
        size_t available_chars = patch_data.m_length - offset;

        if (len >= available_chars)
        {
          result.append(CPatchStr{SPatch{offset, available_chars, patch_data.m_ptr}});
          num_of_characters_taken += available_chars;
        }
        else
        {
          size_t num_chars_needed = len - num_of_characters_taken;
          result.append(CPatchStr{SPatch{offset, num_chars_needed, patch_data.m_ptr}});
          num_of_characters_taken += num_chars_needed;
        }
        first = false;
      }
      else
      {
        size_t available_chars = patch_data.m_length;
        size_t num_chars_needed = len - num_of_characters_taken;
        int num_chars_to_take = num_chars_needed - available_chars;

        if (num_chars_to_take >= 0)
        {
          result.append(CPatchStr{SPatch{patch_data.m_offset, patch_data.m_length, patch_data.m_ptr}});
          num_of_characters_taken += patch_data.m_length;
        }
        else
        {
          result.append(CPatchStr{SPatch{patch_data.m_offset, num_chars_needed, patch_data.m_ptr}});
          num_of_characters_taken += num_chars_needed;
        }
      }
    }
    num_of_characters_passed += patch_data.m_length;
  }

  return result;
}

CPatchStr &CPatchStr::append(const CPatchStr &src)
{
  size_t new_size = m_size + src.m_size;

  if (new_size > m_capacity)
  {
    m_capacity = std::max(m_capacity * 2, new_size);
    SPatch *new_patches = new SPatch[m_capacity];

    for (size_t i = 0; i < m_size; ++i)
    {
      new_patches[i] = m_patches[i];
    }

    delete[] m_patches;
    m_patches = new_patches;
  }

  for (size_t i = 0; i < src.m_size; ++i)
  {
    m_patches[m_size + i] = src.m_patches[i];
  }

  m_length += src.m_length;
  m_size = new_size;
  return *this;
}

CPatchStr &CPatchStr::insert(size_t pos, const CPatchStr &src)
{
  if (pos == m_length)
  {
    return append(src);
  }
  if (pos > m_length)
  {
    throw std::out_of_range("Position is out of range.");
  }

  split_patch(pos);

  pos += 1;
  for (size_t i = 0; i < src.m_size; ++i)
  {
    insert_patch(pos + i, src.m_patches[i]);
  }

  return *this;
}

CPatchStr &CPatchStr::remove(size_t from, size_t len)
{
  if (from + len > m_length)
  {
    throw std::out_of_range("Length of characters to remove doesnt fit.");
  }

  if (len == 0)
  {
    return *this;
  }

  size_t total_characters = 0;
  size_t total_deleted = 0;
  size_t i = 0;
  bool first = true;
  while (i < m_size)
  {
    std::cout << "total characters: " << total_characters << std::endl;
    std::cout << "total deleted: " << total_deleted << std::endl;
    if (total_deleted == len)
    {
      break;
    }
    if (total_characters + m_patches[i].m_length >= from)
    {
      if (first)
      {
        size_t pivot = from - total_characters;
        total_deleted += m_patches[i].m_length - pivot;
        m_patches[i].m_length = pivot;
        first = false;
      }
      else
      {
        if (m_patches[i].m_length >= (len - total_deleted))
        {
          m_patches[i].m_offset = (len - total_deleted);
          total_deleted += m_patches[i].m_length;
        }
        else
        {
          total_deleted += m_patches[i].m_length;
          m_patches[i].m_length = 0;
        }
      }
    }
    total_characters += m_patches[i++].m_length;
  }

  return *this;
}

char *CPatchStr::toStr() const
{
  size_t totalLength = 0;
  for (size_t i = 0; i < m_size; ++i)
  {
    totalLength += m_patches[i].m_length;
  }

  char *result = new char[totalLength + 1];

  size_t currentIndex = 0;
  for (size_t i = 0; i < m_size; ++i)
  {
    const CString *patch = m_patches[i].m_ptr.get();
    const char *patchData = patch->toStr() + m_patches[i].m_offset;
    std::memcpy(result + currentIndex, patchData, m_patches[i].m_length);
    currentIndex += m_patches[i].m_length;
  }

  result[currentIndex] = '\0';

  return result;
}
// -------------------------------- //

#ifndef __PROGTEST__
bool stringMatch(char *str, const char *expected)
{
  bool res = std::strcmp(str, expected) == 0;
  delete[] str;
  return res;
}

int main()
{
  using namespace std;

  char tmpStr[100];
  CPatchStr a("test");
  assert(stringMatch(a.toStr(), "test"));
  std::strncpy(tmpStr, " da", sizeof(tmpStr) - 1);
  a.append(tmpStr);
  assert(stringMatch(a.toStr(), "test da"));
  std::strncpy(tmpStr, "ta", sizeof(tmpStr) - 1);
  a.append(tmpStr);
  assert(stringMatch(a.toStr(), "test data"));
  std::strncpy(tmpStr, "foo text", sizeof(tmpStr) - 1);
  CPatchStr b(tmpStr);
  assert(stringMatch(b.toStr(), "foo text"));
  CPatchStr c(a);
  assert(stringMatch(c.toStr(), "test data"));
  CPatchStr d(a.subStr(3, 5));
  assert(stringMatch(d.toStr(), "t dat"));
  d.append(b);
  assert(stringMatch(d.toStr(), "t datfoo text"));
  d.append(b.subStr(3, 4));
  assert(stringMatch(d.toStr(), "t datfoo text tex"));
  c.append(d);
  assert(stringMatch(c.toStr(), "test datat datfoo text tex"));
  c.append(c);
  assert(stringMatch(c.toStr(), "test datat datfoo text textest datat datfoo text tex"));
  d.insert(2, c.subStr(6, 9));
  assert(stringMatch(d.toStr(), "t atat datfdatfoo text tex"));

  CPatchStr x{"test"};
  cout << x.remove(1, 2).toStr() << endl;
  // d.print();
  // cout << d.toStr() << endl;
  // cout << endl;
  // d.remove(2, 9);
  // cout << d.toStr() << endl;

  // b = "abcdefgh";
  // assert(stringMatch(b.toStr(), "abcdefgh"));
  // assert(stringMatch(d.toStr(), "t atat datfdatfoo text tex"));
  // assert(stringMatch(d.subStr(4, 8).toStr(), "at datfd"));
  // assert(stringMatch(b.subStr(2, 6).toStr(), "cdefgh"));
  // try
  // {
  //   b.subStr(2, 7).toStr();
  //   assert("Exception not thrown" == nullptr);
  // }
  // catch (const std::out_of_range &e)
  // {
  // }
  // catch (...)
  // {
  //   assert("Invalid exception thrown" == nullptr);
  // }
  // a.remove(3, 5);
  // assert(stringMatch(a.toStr(), "tesa"));
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
