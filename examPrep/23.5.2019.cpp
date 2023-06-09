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
        TItem *temp = m_FirstOrder;
        while (temp)
        {
            TItem *temp2 = temp->m_NextOrder;
            delete temp;
            temp = temp2;
        }
        delete[] m_Table;
    }

    CHash(CHash &) = delete;
    CHash &operator=(CHash &) = delete;

    bool Ins(string key, string val)
    {
        unsigned hash = hashFn(key);
        TItem **row = &(m_Table[hash]); // points to correct row, if already in use traverses through the chain
        while (*row)
        {
            if ((*row)->m_Key == key)
                return false;

            row = &((*row)->m_NextHash);
        }

        TItem *newItem = new TItem(key, val, m_Table[hash], nullptr, m_LastOrder);
        if (m_LastOrder)
            m_LastOrder->m_NextOrder = newItem;
        m_Table[hash] = m_LastOrder = newItem;

        if (!m_FirstOrder)
            m_FirstOrder = newItem;
        return true;
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

    return 0;
}