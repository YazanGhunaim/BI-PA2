#include <string>
#include <iostream>

using namespace std;

struct TItem
{
    TItem(string key, string val, TItem *nextHash, TItem *nextOrd, TItem *prevOrd)
        : m_Key(key), m_Val(val), m_NextHash(nextHash), m_NextOrder(nextOrd), m_PrevOrder(prevOrd) {}

    string m_Key, m_Val;
    TItem *m_NextHash, *m_NextOrder, *m_PrevOrder;
};

class CHash
{
public:
    CHash(int m) : m_Table(NULL), m_Size(m), m_FirstOrder(NULL), m_LastOrder(NULL)
    {
        m_Table = new TItem *[m];
        for (int i = 0; i < m; i++)
            m_Table[i] = NULL;
    }

    ~CHash()
    {
        for (int i = 0; i < m_Size; i++)
        {
            TItem *save, *it = m_Table[i];
            while (it)
            {
                save = it;
                it = it->m_NextHash;
                delete save;
            }
        }
        delete[] m_Table;
    }

    CHash(CHash &) = delete;

    CHash &operator=(CHash &) = delete;

    bool Ins(string key, string val)
    {
        unsigned int hash = hashFn(key);
        TItem *ptr = m_Table[hash];

        while (ptr)
        {
            if (ptr->m_Key == key)
                return false;

            ptr = ptr->m_NextHash;
        }

        ptr = new TItem(key, val, m_Table[hash], nullptr, m_LastOrder);
        m_Table[hash] = ptr;
        
        if (m_FirstOrder == nullptr)
            m_FirstOrder = ptr;
        else
            m_LastOrder->m_NextOrder = ptr;
        m_LastOrder = ptr;

        return true;
    }

    bool Del(string key)
    {
        unsigned int hash = hashFn(key);

        TItem *ptr = m_Table[hash];
        if (ptr == nullptr)
            return false;

        TItem *ptr1 = m_Table[hash];
        while (true)
        {
            if (ptr->m_Key == key)
            {
                if (m_Table[hash] == ptr)
                    m_Table[hash] = ptr->m_NextHash;
                else
                    ptr1->m_NextHash = ptr->m_NextHash;
                if (ptr->m_PrevOrder == nullptr)
                {
                    m_FirstOrder = ptr->m_NextOrder;
                    m_FirstOrder->m_PrevOrder = nullptr;
                }
                else
                    ptr->m_PrevOrder->m_NextOrder = ptr->m_NextOrder;
                if (ptr->m_NextOrder == nullptr)
                {
                    m_LastOrder = ptr->m_PrevOrder;
                    m_LastOrder->m_NextOrder = nullptr;
                }
                else
                    ptr->m_NextOrder->m_PrevOrder = ptr->m_PrevOrder;
                delete ptr;
                return true;
            }
            ptr1 = ptr;
            ptr = ptr->m_NextHash;
            if (ptr == nullptr)
                return false;
        }
    }

    bool IsSet(string key)
    {
        TItem *tmp = m_Table[hashFn(key)];
        while (tmp != NULL && tmp->m_Key != key)
            tmp = tmp->m_NextHash;
        if (tmp == NULL)
            return false;
        return true;
    }

    template <typename func>
    void ForEach(func f)
    {
        TItem *tmp = m_FirstOrder;
        while (tmp != NULL)
        {
            f(tmp);
            tmp = tmp->m_NextOrder;
        }
    }

private:
    TItem **m_Table;
    unsigned int m_Size;
    TItem *m_FirstOrder, *m_LastOrder;
    unsigned int hashFn(string &str)
    {
        std::hash<std::string> hash_fn;
        return hash_fn(str) % m_Size;
    }
};

int main(int argc, char **argv)
{
    CHash hashtable(100);
    hashtable.Ins("h1", "car");
    hashtable.Ins("h1", "phone");
    hashtable.Ins("h2", "field");
    hashtable.Ins("h3", "house");
    hashtable.Ins("h4", "tree");

    hashtable.ForEach([](TItem *it)
                      { cout << it->m_Key << " - " << it->m_Val << endl; });

    hashtable.Del("h4");
    cout << endl;

    hashtable.ForEach([](TItem *it)
                      { cout << it->m_Key << " - " << it->m_Val << endl; });

    return 0;
}