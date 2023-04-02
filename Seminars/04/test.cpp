#ifndef __PROGTEST__
#include <cstring>
using namespace std;

class CLinkedSetTester;
#endif /* __PROGTEST__ */

class CLinkedSet
{
private:
    struct CNode
    {
        CNode *m_Next;
        char *m_value = nullptr;

        const char *Value() const
        {
            return m_value;
        }
        void set_value(const char *val)
        {
            if (m_value)
            {
                delete[] m_value;
            }
            m_value = new char[strlen(val) + 1];
            strcpy(m_value, val);
        }
        ~CNode()
        {
            delete[] m_value;
        }
    };

    CNode *m_Begin;

public:
    // default constructor
    CLinkedSet() : m_Begin(nullptr) {}

    // copy constructor (deep copy)
    CLinkedSet(const CLinkedSet &other)
    {
        if (other.m_Begin == nullptr)
        {
            m_Begin = nullptr;
        }

        // creating first node
        m_Begin = new CNode;
        m_Begin->m_Next = nullptr;
        const char *other_value = other.m_Begin->Value();
        m_Begin->set_value(other_value);

        // other nodes
        CNode *currentNode = m_Begin;
        CNode *otherNode = other.m_Begin->m_Next;
        while (otherNode != nullptr)
        {
            currentNode->m_Next = new CNode;
            currentNode = currentNode->m_Next;
            currentNode->m_Next = nullptr;
            other_value = otherNode->Value();
            currentNode->set_value(other_value);
            otherNode = otherNode->m_Next;
        }
    }
    // operator=
    CLinkedSet &operator=(const CLinkedSet &other)
    {
        // self assignment check
        if (&other == this)
            return *this;
        this->~CLinkedSet();
        CLinkedSet temp{other};
        m_Begin = temp.m_Begin;
        temp.m_Begin = nullptr;
        return *this;
    }
    // destructor
    ~CLinkedSet()
    {
        while (m_Begin != nullptr)
        {
            CNode *next = m_Begin->m_Next;
            delete m_Begin;
            m_Begin = next;
        }
        m_Begin = nullptr;
    }
    int find_index(const char *value) const
    {
        int index = 0;
        CNode *temp = m_Begin;
        while (temp != nullptr)
        {
            if (strcmp(value, temp->Value()) < 0)
            {
                return index;
            }
            temp = temp->m_Next;
            index++;
        }
        return index;
    }
    bool Insert(const char *value)
    {
        if (!Contains(value))
        {
            size_t index = find_index(value);

            CNode *new_node = new CNode;
            new_node->m_Next = nullptr;
            new_node->set_value(value);
            // inserting at the beginning
            if (index == 0)
            {
                new_node->m_Next = m_Begin;
                m_Begin = new_node;
                return true;
            }
            else
            {
                CNode *temp = m_Begin;
                for (size_t i = 0; i < index - 1; ++i)
                {
                    temp = temp->m_Next;
                }
                new_node->m_Next = temp->m_Next;
                temp->m_Next = new_node;
                return true;
            }
        }
        return false;
    }

    bool Remove(const char *value)
    {
        if (Contains(value))
        {
            CNode *temp = m_Begin;
            while (strcmp(value, temp->m_Next->Value()) != 0 && temp->m_Next)
            {
                temp = temp->m_Next;
            }

            CNode *free_node = temp->m_Next;
            temp->m_Next = temp->m_Next->m_Next;

            delete free_node;
            return true;
        }
        return false;
    }

    bool Empty() const { return m_Begin == nullptr; }

    size_t Size() const
    {
        size_t size = 0;
        CNode *temp = m_Begin;
        while (temp != nullptr)
        {
            size++;
            temp = temp->m_Next;
        }
        return size;
    }

    bool Contains(const char *value) const
    {
        CNode *temp = m_Begin;
        while (temp != nullptr)
        {
            if (strcmp(temp->Value(), value) == 0)
                return true;
            temp = temp->m_Next;
        }
        return false;
    }

    friend class ::CLinkedSetTester;
};

#ifndef __PROGTEST__
#include <cassert>

struct CLinkedSetTester
{
    static void test0()
    {
        CLinkedSet x0;
        assert(x0.Insert("Jerabek Michal"));
        assert(x0.Insert("Pavlik Ales"));
        assert(x0.Insert("Dusek Zikmund"));
        assert(x0.Size() == 3);
        assert(x0.Contains("Dusek Zikmund"));
        assert(!x0.Contains("Pavlik Jan"));
        assert(x0.Remove("Jerabek Michal"));
        assert(!x0.Remove("Pavlik Jan"));
        assert(!x0.Empty());
    }

    static void test1()
    {
        CLinkedSet x0;
        assert(x0.Insert("Jerabek Michal"));
        assert(x0.Insert("Pavlik Ales"));
        assert(x0.Insert("Dusek Zikmund"));
        assert(x0.Size() == 3);
        assert(x0.Contains("Dusek Zikmund"));
        assert(!x0.Contains("Pavlik Jan"));
        assert(x0.Remove("Jerabek Michal"));
        assert(!x0.Remove("Pavlik Jan"));
        CLinkedSet x1(x0);
        assert(x0.Insert("Vodickova Saskie"));
        assert(x1.Insert("Kadlecova Kvetslava"));
        assert(x0.Size() == 3);
        assert(x1.Size() == 3);
        assert(x0.Contains("Vodickova Saskie"));
        assert(!x1.Contains("Vodickova Saskie"));
        assert(!x0.Contains("Kadlecova Kvetslava"));
        assert(x1.Contains("Kadlecova Kvetslava"));
    }

    static void test2()
    {
        CLinkedSet x0;
        CLinkedSet x1;
        assert(x0.Insert("Jerabek Michal"));
        assert(x0.Insert("Pavlik Ales"));
        assert(x0.Insert("Dusek Zikmund"));
        assert(x0.Size() == 3);
        assert(x0.Contains("Dusek Zikmund"));
        assert(!x0.Contains("Pavlik Jan"));
        assert(x0.Remove("Jerabek Michal"));
        assert(!x0.Remove("Pavlik Jan"));
        x1 = x0;
        assert(x0.Insert("Vodickova Saskie"));
        assert(x1.Insert("Kadlecova Kvetslava"));
        assert(x0.Size() == 3);
        assert(x1.Size() == 3);
        assert(x0.Contains("Vodickova Saskie"));
        assert(!x1.Contains("Vodickova Saskie"));
        assert(!x0.Contains("Kadlecova Kvetslava"));
        assert(x1.Contains("Kadlecova Kvetslava"));
    }
};

int main()
{
    CLinkedSetTester::test0();
    CLinkedSetTester::test1();
    CLinkedSetTester::test2();
    return 0;
}
#endif /* __PROGTEST__ */
