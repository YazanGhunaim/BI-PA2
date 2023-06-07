#include <iostream>
#include <vector>
#include <cassert>
using namespace std;

class CNetwork
{
public:
    class CPerson
    {
    public:
        string m_Name;
        CPerson *m_Next;
        vector<CPerson *> m_Friends;

        CPerson(const string &name) : m_Name(name), m_Next(nullptr) {}
    };
    CPerson **m_Table;
    size_t m_Size;

    size_t hashFn(const string &s) const;
    CNetwork(size_t size);
    ~CNetwork();
    CNetwork(const CNetwork &other);
    CNetwork &operator=(const CNetwork &) = delete;
    bool Ins(const string &name);
    CPerson *Find(const string &name) const;
    bool Friends(const string &name1, const string &name2);
};

size_t CNetwork::hashFn(const string &s) const
{
    hash<string> h;
    return h(s) % m_Size;
}

CNetwork::CNetwork(size_t size) : m_Size(size)
{
    m_Table = new CPerson *[m_Size];
    for (size_t i = 0; i < m_Size; i++)
        m_Table[i] = nullptr;
}

CNetwork::~CNetwork()
{
    for (size_t i = 0; i < m_Size; ++i)
    {
        CPerson *head = m_Table[i];
        while (head)
        {
            CPerson *tmp = head->m_Next;
            delete head;
            head = tmp;
        }
    }
    delete[] m_Table;
}

CNetwork::CNetwork(const CNetwork &other)
    : m_Size(other.m_Size)
{
    m_Table = new CPerson *[m_Size];
    for (size_t i = 0; i < m_Size; ++i)
    {
        m_Table[i] = nullptr;
        CPerson *head = other.m_Table[i];
        while (head)
        {
            CPerson *p = new CPerson(head->m_Name);
            p->m_Next = nullptr;
            CPerson *back = m_Table[i];
            if (!back)
                m_Table[i] = p;
            else
            {
                while (back->m_Next)
                    back = back->m_Next;
                back->m_Next = p;
            }
            head = head->m_Next;
        }
    }

    for (size_t i = 0; i < m_Size; ++i)
    {
        CPerson *head = other.m_Table[i];
        while (head)
        {
            CPerson *p = Find(head->m_Name);
            for (CPerson *it : head->m_Friends)
            {
                p->m_Friends.push_back(Find(it->m_Name));
            }
            head = head->m_Next;
        }
    }
}

bool CNetwork::Ins(const string &name)
{
    size_t h = hashFn(name);
    CPerson *p = m_Table[h];
    while (p != nullptr)
    {
        if (p->m_Name == name)
            return false;
        p = p->m_Next;
    }
    p = new CPerson(name);
    p->m_Next = m_Table[h];
    m_Table[h] = p;
    return true;
}

CNetwork::CPerson *CNetwork::Find(const string &name) const
{
    size_t h = hashFn(name);
    CPerson *p = m_Table[h];
    while (p != nullptr)
    {
        if (p->m_Name == name)
            return p;
        p = p->m_Next;
    }
    return nullptr;
}

bool CNetwork::Friends(const string &name1, const string &name2)
{
    CPerson *p1 = Find(name1);
    CPerson *p2 = Find(name2);
    if (p1 == nullptr || p2 == nullptr)
        return false;
    p1->m_Friends.push_back(p2);
    p2->m_Friends.push_back(p1);
    return true;
}

void testNetwork(const CNetwork &t)
{
    assert(t.m_Size == 4);

    assert(t.m_Table[0]->m_Name == "Fred");
    assert(t.m_Table[0]->m_Next->m_Name == "Dave");
    assert(t.m_Table[0]->m_Next->m_Next->m_Name == "Bob");
    assert(t.m_Table[0]->m_Next->m_Next->m_Next == nullptr);
    assert(t.m_Table[1]->m_Name == "Helen");
    assert(t.m_Table[1]->m_Next->m_Name == "Grace");
    assert(t.m_Table[1]->m_Next->m_Next->m_Name == "Carol");
    assert(t.m_Table[1]->m_Next->m_Next->m_Next->m_Name == "Alice");
    assert(t.m_Table[1]->m_Next->m_Next->m_Next->m_Next == nullptr);
    assert(t.m_Table[2] == nullptr);
    assert(t.m_Table[3]->m_Name == "Evelyn");
    assert(t.m_Table[3]->m_Next == nullptr);

    assert(t.m_Table[0]->m_Friends.size() == 1);
    assert(t.m_Table[0]->m_Friends[0]->m_Name == "Grace");
    assert(t.m_Table[0]->m_Next->m_Friends.size() == 2);
    assert(t.m_Table[0]->m_Next->m_Friends[0]->m_Name == "Evelyn");
    assert(t.m_Table[0]->m_Next->m_Friends[1]->m_Name == "Bob");
    assert(t.m_Table[0]->m_Next->m_Next->m_Friends.size() == 3);
    assert(t.m_Table[0]->m_Next->m_Next->m_Friends[0]->m_Name == "Alice");
    assert(t.m_Table[0]->m_Next->m_Next->m_Friends[1]->m_Name == "Grace");
    assert(t.m_Table[0]->m_Next->m_Next->m_Friends[2]->m_Name == "Dave");
    assert(t.m_Table[1]->m_Friends.size() == 0);
    assert(t.m_Table[1]->m_Next->m_Friends.size() == 2);
    assert(t.m_Table[1]->m_Next->m_Friends[0]->m_Name == "Fred");
    assert(t.m_Table[1]->m_Next->m_Friends[1]->m_Name == "Bob");
    assert(t.m_Table[1]->m_Next->m_Next->m_Friends.size() == 2);
    assert(t.m_Table[1]->m_Next->m_Next->m_Friends[0]->m_Name == "Alice");
    assert(t.m_Table[1]->m_Next->m_Next->m_Friends[1]->m_Name == "Evelyn");
    assert(t.m_Table[1]->m_Next->m_Next->m_Next->m_Friends.size() == 2);
    assert(t.m_Table[1]->m_Next->m_Next->m_Next->m_Friends[0]->m_Name == "Bob");
    assert(t.m_Table[1]->m_Next->m_Next->m_Next->m_Friends[1]->m_Name == "Carol");
    assert(t.m_Table[3]->m_Friends.size() == 2);
    assert(t.m_Table[3]->m_Friends[0]->m_Name == "Dave");
    assert(t.m_Table[3]->m_Friends[1]->m_Name == "Carol");

    assert(t.m_Table[0]->m_Friends[0] == t.m_Table[1]->m_Next);
    assert(t.m_Table[0]->m_Next->m_Friends[0] == t.m_Table[3]);
    assert(t.m_Table[0]->m_Next->m_Friends[1] == t.m_Table[0]->m_Next->m_Next);
    assert(t.m_Table[0]->m_Next->m_Next->m_Friends[0] == t.m_Table[1]->m_Next->m_Next->m_Next);
    assert(t.m_Table[0]->m_Next->m_Next->m_Friends[1] == t.m_Table[1]->m_Next);
    assert(t.m_Table[0]->m_Next->m_Next->m_Friends[2] == t.m_Table[0]->m_Next);
    assert(t.m_Table[1]->m_Next->m_Friends[0] == t.m_Table[0]);
    assert(t.m_Table[1]->m_Next->m_Friends[1] == t.m_Table[0]->m_Next->m_Next);
    assert(t.m_Table[1]->m_Next->m_Next->m_Friends[0] == t.m_Table[1]->m_Next->m_Next->m_Next);
    assert(t.m_Table[1]->m_Next->m_Next->m_Friends[1] == t.m_Table[3]);
    assert(t.m_Table[1]->m_Next->m_Next->m_Next->m_Friends[0] == t.m_Table[0]->m_Next->m_Next);
    assert(t.m_Table[1]->m_Next->m_Next->m_Next->m_Friends[1] == t.m_Table[1]->m_Next->m_Next);
    assert(t.m_Table[3]->m_Friends[0] == t.m_Table[0]->m_Next);
    assert(t.m_Table[3]->m_Friends[1] == t.m_Table[1]->m_Next->m_Next);
}

int main()
{
    CNetwork *t0 = new CNetwork(4);

    assert(t0->Ins("Alice") == true);
    assert(t0->Ins("Bob") == true);
    assert(t0->Ins("Carol") == true);
    assert(t0->Ins("Dave") == true);
    assert(t0->Ins("Evelyn") == true);
    assert(t0->Ins("Fred") == true);
    assert(t0->Ins("Grace") == true);
    assert(t0->Ins("Helen") == true);
    assert(t0->Ins("Alice") == false);

    assert(t0->Friends("Alice", "Bob") == true);
    assert(t0->Friends("Alice", "Carol") == true);
    assert(t0->Friends("Evelyn", "Dave") == true);
    assert(t0->Friends("Carol", "Evelyn") == true);
    assert(t0->Friends("Grace", "Fred") == true);
    assert(t0->Friends("Bob", "Grace") == true);
    assert(t0->Friends("Bob", "Dave") == true);
    assert(t0->Friends("Alice", "Incorrect name") == false);
    assert(t0->Friends("Incorrect name", "Bob") == false);
    assert(t0->Friends("Incorrect name", "Incorrect name") == false);

    testNetwork(*t0);
    cout << "Base network test passed" << endl;

    CNetwork *t1 = new CNetwork(*t0);
    testNetwork(*t1);
    cout << "Copy constructor test passed" << endl;

    delete t0;
    testNetwork(*t1);
    cout << "Deep copy test passed" << endl;

    delete t1;
    cout << "Run with valgrind to check destructor" << endl;

    return EXIT_SUCCESS;
}