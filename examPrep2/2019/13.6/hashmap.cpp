#include <iostream>
#include <cassert>
#include <functional>
#include <sstream>
using namespace std;

class CHash
{
private:
    struct TItem
    {
        TItem(const string &key,
              const string &val)
            : m_Key(key),
              m_Val(val),
              m_NextHash(NULL),
              m_NextOrder(NULL),
              m_PrevOrder(NULL)
        {
        }

        string m_Key;
        string m_Val;
        TItem *m_NextHash;
        TItem *m_NextOrder;
        TItem *m_PrevOrder;
    };

    TItem **m_Table;
    size_t m_Size;
    TItem *m_FirstOrder;
    TItem *m_LastOrder;

    friend int main(void);

    size_t hashFn(const string &x, size_t modulus) const
    {
        size_t h = 5381;
        for (string::size_type c : x)
            h += (h << 5) + c;
        return h % modulus;
    }

public:
    CHash(size_t size)
        : m_Table(new TItem *[size] {}), m_Size(size), m_FirstOrder(nullptr), m_LastOrder(nullptr) {}

    CHash(const CHash &src)
        : m_Table(new TItem *[src.m_Size] {}), m_Size(src.m_Size), m_FirstOrder(nullptr), m_LastOrder(nullptr)
    {
        for (TItem *head = src.m_FirstOrder; head; head = head->m_NextOrder)
            Ins(head->m_Key, head->m_Val);
    }

    CHash &operator=(CHash src)
    {
        std::swap(m_Table, src.m_Table);
        std::swap(m_Size, src.m_Size);
        std::swap(m_FirstOrder, src.m_FirstOrder);
        std::swap(m_LastOrder, src.m_LastOrder);
        return *this;
    }

    ~CHash(void)
    {
        for (size_t i = 0; i < m_Size; ++i)
        {
            TItem *head = m_Table[i];
            while (head)
            {
                TItem *temp = head;
                head = head->m_NextHash;
                delete temp;
            }
        }

        delete[] m_Table;
    }

    bool Ins(const string &key, const string &val)
    {
        auto hash = hashFn(key, m_Size);

        for (TItem *list = m_Table[hash]; list; list = list->m_NextHash)
            if (list->m_Key == key)
                return false;

        TItem *new_node = new TItem(key, val);
        new_node->m_NextHash = m_Table[hash];
        new_node->m_PrevOrder = m_LastOrder;

        if (m_LastOrder)
            m_LastOrder->m_NextOrder = new_node;
        if (!m_FirstOrder)
            m_FirstOrder = new_node;

        m_LastOrder = new_node;

        m_Table[hash] = new_node;

        return true;
    }

    bool Del(const string &key)
    {
        auto hash = hashFn(key, m_Size);
        TItem *head = m_Table[hash];
        TItem *prev = nullptr;

        // Find the node to delete
        while (head)
        {
            if (head->m_Key == key)
                break;

            prev = head;
            head = head->m_NextHash;
        }

        // If node not found, return false
        if (!head)
            return false;

        // Update pointers to remove the node
        if (!prev)
        {
            m_Table[hash] = head->m_NextHash;
        }
        else
        {
            prev->m_NextHash = head->m_NextHash;
        }

        if (m_FirstOrder == head)
        {
            m_FirstOrder = head->m_NextOrder;
        }
        if (m_LastOrder == head)
        {
            m_LastOrder = head->m_PrevOrder;
        }
        if (head->m_NextOrder)
        {
            head->m_NextOrder->m_PrevOrder = head->m_PrevOrder;
        }
        if (head->m_PrevOrder)
        {
            head->m_PrevOrder->m_NextOrder = head->m_NextOrder;
        }

        delete head;

        return true;
    }

    bool IsSet(const string &key) const
    {
        auto hash = hashFn(key, m_Size);

        for (TItem *list = m_Table[hash]; list; list = list->m_NextHash)
            if (list->m_Key == key)
                return true;

        return false;
    }

    void ForEach(const auto &f) const
    {
        for (TItem *head = m_FirstOrder; head; head = head->m_NextOrder)
            f(head->m_Val);
    }
};

int main(void)
{
    CHash x(100);

    assert(sizeof(CHash) == sizeof(CHash::TItem *) * 3 + sizeof(size_t));
    assert(sizeof(CHash::TItem) == sizeof(CHash::TItem *) * 3 + sizeof(string) * 2);
    assert(x.Ins("peg", "foo") && x.m_Table[1]->m_Key == "peg" && x.m_Table[1]->m_Val == "foo" && x.m_FirstOrder == x.m_Table[1] && x.m_FirstOrder->m_NextOrder == NULL && x.m_LastOrder == x.m_Table[1] && x.m_LastOrder->m_PrevOrder == NULL);
    assert(x.Ins("bwc", "bar") && x.m_Table[45]->m_Key == "bwc" && x.m_Table[45]->m_Val == "bar" && x.m_FirstOrder == x.m_Table[1] && x.m_FirstOrder->m_NextOrder == x.m_Table[45] && x.m_FirstOrder->m_NextOrder->m_NextOrder == NULL && x.m_LastOrder == x.m_Table[45] && x.m_LastOrder->m_PrevOrder == x.m_Table[1] && x.m_LastOrder->m_PrevOrder->m_PrevOrder == NULL);
    assert(x.Ins("jdr", "test") && x.m_Table[45]->m_Key == "jdr" && x.m_Table[45]->m_Val == "test" && x.m_Table[45]->m_NextHash->m_Key == "bwc" && x.m_Table[45]->m_NextHash->m_Val == "bar" && x.m_FirstOrder == x.m_Table[1] && x.m_FirstOrder->m_NextOrder == x.m_Table[45]->m_NextHash && x.m_FirstOrder->m_NextOrder->m_NextOrder == x.m_Table[45] && x.m_FirstOrder->m_NextOrder->m_NextOrder->m_NextOrder == NULL && x.m_LastOrder == x.m_Table[45] && x.m_LastOrder->m_PrevOrder == x.m_Table[45]->m_NextHash && x.m_LastOrder->m_PrevOrder->m_PrevOrder == x.m_Table[1] && x.m_LastOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder == NULL);
    assert(x.Ins("peg", "something") == false);
    assert(x.Ins("peg", "foo") == false);
    assert(x.Ins("bwc", "test") == false);
    assert(x.Ins("jdr", "mudr") == false);
    assert(x.Ins("asa", "butterfield") && x.m_Table[22]->m_Key == "asa" && x.m_Table[22]->m_Val == "butterfield" && x.m_FirstOrder == x.m_Table[1] && x.m_FirstOrder->m_NextOrder == x.m_Table[45]->m_NextHash && x.m_FirstOrder->m_NextOrder->m_NextOrder == x.m_Table[45] && x.m_FirstOrder->m_NextOrder->m_NextOrder->m_NextOrder == x.m_Table[22] && x.m_FirstOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder == NULL && x.m_LastOrder == x.m_Table[22] && x.m_LastOrder->m_PrevOrder == x.m_Table[45] && x.m_LastOrder->m_PrevOrder->m_PrevOrder == x.m_Table[45]->m_NextHash && x.m_LastOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder == x.m_Table[1] && x.m_LastOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder == NULL);
    assert(x.IsSet("peg"));
    assert(x.IsSet("asa"));
    assert(x.IsSet("bwc"));
    assert(x.IsSet("jdr"));
    assert(x.IsSet("whf") == false);
    assert(x.IsSet("xns") == false);
    assert(x.IsSet("abc") == false);
    assert(x.IsSet("ooj") == false);
    ostringstream oss;
    x.ForEach([&oss](const string &val)
              { oss << val << endl; });
    assert(oss.str() == "foo\nbar\ntest\nbutterfield\n");
    CHash n(x);
    assert(n.m_Table[1]->m_Key == "peg" && n.m_Table[1]->m_Val == "foo" && n.m_Table[45]->m_Key == "jdr" && n.m_Table[45]->m_Val == "test" && n.m_Table[45]->m_NextHash->m_Key == "bwc" && n.m_Table[45]->m_NextHash->m_Val == "bar" && n.m_Table[22]->m_Key == "asa" && n.m_Table[22]->m_Val == "butterfield" && n.m_FirstOrder == n.m_Table[1] && n.m_FirstOrder->m_NextOrder == n.m_Table[45]->m_NextHash && n.m_FirstOrder->m_NextOrder->m_NextOrder == n.m_Table[45] && n.m_FirstOrder->m_NextOrder->m_NextOrder->m_NextOrder == n.m_Table[22] && n.m_FirstOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder == NULL && n.m_LastOrder == n.m_Table[22] && n.m_LastOrder->m_PrevOrder == n.m_Table[45] && n.m_LastOrder->m_PrevOrder->m_PrevOrder == n.m_Table[45]->m_NextHash && n.m_LastOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder == n.m_Table[1] && n.m_LastOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder == NULL);
    assert(n.Del("jdr") && n.m_Table[45]->m_Key == "bwc" && n.m_Table[45]->m_Val == "bar" && n.m_FirstOrder == n.m_Table[1] && n.m_FirstOrder->m_NextOrder == n.m_Table[45] && n.m_FirstOrder->m_NextOrder->m_NextOrder == n.m_Table[22] && n.m_FirstOrder->m_NextOrder->m_NextOrder->m_NextOrder == NULL && n.m_LastOrder == n.m_Table[22] && n.m_LastOrder->m_PrevOrder == n.m_Table[45] && n.m_LastOrder->m_PrevOrder->m_PrevOrder == n.m_Table[1] && n.m_LastOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder == NULL);
    assert(n.IsSet("jdr") == false);
    assert(x.m_Table[1]->m_Key == "peg" && x.m_Table[1]->m_Val == "foo" && x.m_Table[45]->m_Key == "jdr" && x.m_Table[45]->m_Val == "test" && x.m_Table[45]->m_NextHash->m_Key == "bwc" && x.m_Table[45]->m_NextHash->m_Val == "bar" && x.m_Table[22]->m_Key == "asa" && x.m_Table[22]->m_Val == "butterfield" && x.m_FirstOrder == x.m_Table[1] && x.m_FirstOrder->m_NextOrder == x.m_Table[45]->m_NextHash && x.m_FirstOrder->m_NextOrder->m_NextOrder == x.m_Table[45] && x.m_FirstOrder->m_NextOrder->m_NextOrder->m_NextOrder == x.m_Table[22] && x.m_FirstOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder == NULL && x.m_LastOrder == x.m_Table[22] && x.m_LastOrder->m_PrevOrder == x.m_Table[45] && x.m_LastOrder->m_PrevOrder->m_PrevOrder == x.m_Table[45]->m_NextHash && x.m_LastOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder == x.m_Table[1] && x.m_LastOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder == NULL);
    assert(n.Del("404") == false);
    assert(n.Del("system32") == false);
    assert(n.Del("root") == false);
    n = n;
    assert(n.Ins("xns", "test") && n.m_Table[22]->m_Key == "xns" && n.m_Table[22]->m_Val == "test" && n.m_Table[22]->m_NextHash->m_Key == "asa" && n.m_Table[22]->m_NextHash->m_Val == "butterfield" && n.m_FirstOrder == n.m_Table[1] && n.m_FirstOrder->m_NextOrder == n.m_Table[45] && n.m_FirstOrder->m_NextOrder->m_NextOrder == n.m_Table[22]->m_NextHash && n.m_FirstOrder->m_NextOrder->m_NextOrder->m_NextOrder == n.m_Table[22] && n.m_FirstOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder == NULL && n.m_LastOrder == n.m_Table[22] && n.m_LastOrder->m_PrevOrder == n.m_Table[22]->m_NextHash && n.m_LastOrder->m_PrevOrder->m_PrevOrder == n.m_Table[45] && n.m_LastOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder == n.m_Table[1] && n.m_LastOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder == NULL);
    x = n;
    assert(x.m_Table[22]->m_Key == "xns" && x.m_Table[22]->m_Val == "test" && x.m_Table[22]->m_NextHash->m_Key == "asa" && x.m_Table[22]->m_NextHash->m_Val == "butterfield" && x.m_FirstOrder == x.m_Table[1] && x.m_FirstOrder->m_NextOrder == x.m_Table[45] && x.m_FirstOrder->m_NextOrder->m_NextOrder == x.m_Table[22]->m_NextHash && x.m_FirstOrder->m_NextOrder->m_NextOrder->m_NextOrder == x.m_Table[22] && x.m_FirstOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder == NULL && x.m_LastOrder == x.m_Table[22] && x.m_LastOrder->m_PrevOrder == x.m_Table[22]->m_NextHash && x.m_LastOrder->m_PrevOrder->m_PrevOrder == x.m_Table[45] && x.m_LastOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder == x.m_Table[1] && x.m_LastOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder == NULL);
    assert(x.Ins("whf", "FIT") && x.m_Table[22]->m_Key == "whf" && x.m_Table[22]->m_Val == "FIT" && x.m_Table[22]->m_NextHash->m_Key == "xns" && x.m_Table[22]->m_NextHash->m_Val == "test" && x.m_Table[22]->m_NextHash->m_NextHash->m_Key == "asa" && x.m_Table[22]->m_NextHash->m_NextHash->m_Val == "butterfield" && x.m_FirstOrder == x.m_Table[1] && x.m_FirstOrder->m_NextOrder == x.m_Table[45] && x.m_FirstOrder->m_NextOrder->m_NextOrder == x.m_Table[22]->m_NextHash->m_NextHash && x.m_FirstOrder->m_NextOrder->m_NextOrder->m_NextOrder == x.m_Table[22]->m_NextHash && x.m_FirstOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder == x.m_Table[22] && x.m_FirstOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder == NULL && x.m_LastOrder == x.m_Table[22] && x.m_LastOrder->m_PrevOrder == x.m_Table[22]->m_NextHash && x.m_LastOrder->m_PrevOrder->m_PrevOrder == x.m_Table[22]->m_NextHash->m_NextHash && x.m_LastOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder == x.m_Table[45] && x.m_LastOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder == x.m_Table[1] && x.m_LastOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder == NULL);
    assert(n.m_Table[22]->m_Key == "xns" && n.m_Table[22]->m_Val == "test" && n.m_Table[22]->m_NextHash->m_Key == "asa" && n.m_Table[22]->m_NextHash->m_Val == "butterfield" && n.m_FirstOrder == n.m_Table[1] && n.m_FirstOrder->m_NextOrder == n.m_Table[45] && n.m_FirstOrder->m_NextOrder->m_NextOrder == n.m_Table[22]->m_NextHash && n.m_FirstOrder->m_NextOrder->m_NextOrder->m_NextOrder == n.m_Table[22] && n.m_FirstOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder == NULL && n.m_LastOrder == n.m_Table[22] && n.m_LastOrder->m_PrevOrder == n.m_Table[22]->m_NextHash && n.m_LastOrder->m_PrevOrder->m_PrevOrder == n.m_Table[45] && n.m_LastOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder == n.m_Table[1] && n.m_LastOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder == NULL);
    assert(x.Del("xns") && x.m_Table[22]->m_Key == "whf" && x.m_Table[22]->m_Val == "FIT" && x.m_Table[22]->m_NextHash->m_Key == "asa" && x.m_Table[22]->m_NextHash->m_Val == "butterfield" && x.m_FirstOrder == x.m_Table[1] && x.m_FirstOrder->m_NextOrder == x.m_Table[45] && x.m_FirstOrder->m_NextOrder->m_NextOrder == x.m_Table[22]->m_NextHash && x.m_FirstOrder->m_NextOrder->m_NextOrder->m_NextOrder == x.m_Table[22] && x.m_FirstOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder == NULL && x.m_LastOrder == x.m_Table[22] && x.m_LastOrder->m_PrevOrder == x.m_Table[22]->m_NextHash && x.m_LastOrder->m_PrevOrder->m_PrevOrder == x.m_Table[45] && x.m_LastOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder == x.m_Table[1] && x.m_LastOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder == NULL);
    assert(x.Del("whf") && x.m_Table[22]->m_Key == "asa" && x.m_Table[22]->m_Val == "butterfield" && x.m_FirstOrder == x.m_Table[1] && x.m_FirstOrder->m_NextOrder == x.m_Table[45] && x.m_FirstOrder->m_NextOrder->m_NextOrder == x.m_Table[22] && x.m_FirstOrder->m_NextOrder->m_NextOrder->m_NextOrder == NULL && x.m_LastOrder == x.m_Table[22] && x.m_LastOrder->m_PrevOrder == x.m_Table[45] && x.m_LastOrder->m_PrevOrder->m_PrevOrder == x.m_Table[1] && x.m_LastOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder == NULL);
    assert(x.IsSet("asa"));
    assert(x.IsSet("xns") == false);
    assert(x.IsSet("whf") == false);
    assert(x.Del("asa") && x.m_Table[22] == NULL);
    assert(x.Del("peg") && x.m_Table[1] == NULL);
    assert(x.Del("bwc") && x.m_Table[45] == NULL);
    assert(x.m_FirstOrder == NULL && x.m_LastOrder == NULL);
    return 0;
}