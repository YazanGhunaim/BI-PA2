#include <string>
#include <cassert>
#include <iostream>

class CHashSet
{
private:
    /**
     * Tweaking this function may render a considerable portion of the included tests invalid. If you plan to do so,
     * compile the program with @p -D"CUSTOM_HASH" to disable the affected tests.
     */
    [[nodiscard]] size_t hash(const std::string &key) const
    {
        return std::hash<std::string>{}(key) % m_Size;
    }

    class CItem
    {
    public:
        explicit CItem(std::string key)
            : m_Key(std::move(key))
        {
        }

        std::string m_Key;
        CItem *m_NextHash = nullptr;
        CItem *m_NextOrder = nullptr;
        CItem *m_PrevOrder = nullptr;
    };

    size_t m_Size;
    CItem *m_First = nullptr;
    CItem *m_Last = nullptr;
    CItem **m_Table = nullptr;

public:
    explicit CHashSet(size_t size)
        : m_Size(size), m_Table(new CItem *[size]
                                { nullptr })
    {
    }

    ~CHashSet()
    {
        CItem *node = m_First;

        while (node)
        {
            CItem *to_delete = node;
            node = node->m_NextOrder;
            delete to_delete;
        }

        delete[] m_Table;
    }

    CHashSet(const CHashSet &src) = delete;

    CHashSet &operator=(const CHashSet &src) = delete;

    bool insert(const std::string &key)
    {
        if (contains(key))
            return false;

        size_t hashVal = hash(key);

        CItem *newItem = new CItem(key);
        newItem->m_NextHash = m_Table[hashVal];

        if (m_Last)
            m_Last->m_NextOrder = newItem;
        else
            m_First = newItem;

        newItem->m_PrevOrder = m_Last;
        m_Last = newItem;

        m_Table[hashVal] = newItem;
        return true;
    }

    [[nodiscard]] bool contains(const std::string &key) const
    {
        size_t hash_val = hash(key);

        CItem *item = m_Table[hash_val];

        while (item)
        {
            if (item->m_Key == key)
                return true;

            item = item->m_NextHash;
        }

        return false;
    }

    bool remove(const std::string &key)
    {
        size_t hashVal = hash(key);

        CItem **item = &m_Table[hashVal];

        while ((*item))
        {
            if ((*item)->m_Key == key)
            {
                CItem *toDel = *item;

                if (toDel->m_PrevOrder)
                    toDel->m_PrevOrder->m_NextOrder = toDel->m_NextOrder;
                else
                    m_First = toDel->m_NextOrder;

                if (toDel->m_NextOrder)
                    toDel->m_NextOrder->m_PrevOrder = toDel->m_PrevOrder;
                else
                    m_Last = toDel->m_PrevOrder;

                *item = toDel->m_NextHash;

                delete toDel;
                return true;
            }

            item = &(*item)->m_NextHash;
        }

        return false;
    }

    class CIterator
    {
    public:
        CIterator(CItem *item)
            : m_Item(item)
        {
        }

        CIterator &operator++()
        {
            m_Item = m_Item->m_NextOrder;
            return *this;
        }

        CIterator operator++(int)
        {
           auto old = *this;
           operator++();
           return old;
        }

        bool operator==(const CIterator &rhs) const
        {
            return m_Item == rhs.m_Item;
        }

        bool operator!=(const CIterator &rhs) const
        {
            return !(*this == rhs);
        }

        std::string &operator*()
        {
            return m_Item->m_Key;
        }

    private:
        CItem *m_Item = nullptr;

        friend class CHashSetTest; // debug
    };

    [[nodiscard]] CHashSet::CIterator begin() const
    {
        return {m_First};
    }

    [[nodiscard]] static CHashSet::CIterator end()
    {
        return {nullptr};
    }

    friend class CHashSetTest; // debug
};

class CHashSetTest
{
public:
    static void test()
    {
        CHashSet hs0(5);

        assert(hs0.insert("PA1"));
        assert(hs0.insert("PA2"));
        assert(hs0.insert("MA1"));
        assert(hs0.insert("MA2"));
        assert(hs0.insert("SAP"));
        assert(hs0.insert("DBS"));

        assert(hs0.contains("PA1"));
        assert(hs0.contains("PA2"));
        assert(hs0.contains("MA1"));
        assert(hs0.contains("MA2"));
        assert(hs0.contains("SAP"));
        assert(hs0.contains("DBS"));

        assert(hs0.m_First->m_Key == "PA1");
        assert(hs0.m_First->m_NextOrder->m_Key == "PA2");
        assert(hs0.m_First->m_NextOrder->m_NextOrder->m_Key == "MA1");
        assert(hs0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_Key == "MA2");
        assert(hs0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key == "SAP");
        assert(hs0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key == "DBS");
        assert(hs0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder == nullptr);

        assert(hs0.m_Last->m_Key == "DBS");
        assert(hs0.m_Last->m_PrevOrder->m_Key == "SAP");
        assert(hs0.m_Last->m_PrevOrder->m_PrevOrder->m_Key == "MA2");
        assert(hs0.m_Last->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_Key == "MA1");
        assert(hs0.m_Last->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_Key == "PA2");
        assert(hs0.m_Last->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_Key == "PA1");
        assert(hs0.m_Last->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder == nullptr);

        assert(hs0.remove("MA1"));

        assert(hs0.contains("PA1"));
        assert(hs0.contains("PA2"));
        assert(!hs0.contains("MA1"));
        assert(hs0.contains("MA2"));
        assert(hs0.contains("SAP"));
        assert(hs0.contains("DBS"));

        assert(hs0.m_First->m_Key == "PA1");
        assert(hs0.m_First->m_NextOrder->m_Key == "PA2");
        assert(hs0.m_First->m_NextOrder->m_NextOrder->m_Key == "MA2");
        assert(hs0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_Key == "SAP");
        assert(hs0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key == "DBS");
        assert(hs0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder == nullptr);

        assert(hs0.m_Last->m_Key == "DBS");
        assert(hs0.m_Last->m_PrevOrder->m_Key == "SAP");
        assert(hs0.m_Last->m_PrevOrder->m_PrevOrder->m_Key == "MA2");
        assert(hs0.m_Last->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_Key == "PA2");
        assert(hs0.m_Last->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_Key == "PA1");
        assert(hs0.m_Last->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder == nullptr);

        assert(hs0.insert("test0"));
        assert(hs0.insert("test1"));
        assert(hs0.insert("test2"));
        assert(hs0.insert("test3"));
        assert(hs0.insert("test4"));
        assert(hs0.insert("test5"));
        assert(hs0.insert("test6"));
        assert(hs0.insert("test7"));

        assert(hs0.contains("test0"));
        assert(hs0.contains("test1"));
        assert(hs0.contains("test2"));
        assert(hs0.contains("test3"));
        assert(hs0.contains("test4"));
        assert(hs0.contains("test5"));
        assert(hs0.contains("test6"));
        assert(hs0.contains("test7"));

        assert(hs0.contains("PA1"));
        assert(hs0.contains("PA2"));
        assert(hs0.contains("MA2"));
        assert(hs0.contains("SAP"));
        assert(hs0.contains("DBS"));

        assert(hs0.remove("test4"));

        assert(hs0.contains("test0"));
        assert(hs0.contains("test1"));
        assert(hs0.contains("test2"));
        assert(hs0.contains("test3"));
        assert(!hs0.contains("test4"));
        assert(hs0.contains("test5"));
        assert(hs0.contains("test6"));
        assert(hs0.contains("test7"));

        assert(hs0.contains("PA1"));
        assert(hs0.contains("PA2"));
        assert(hs0.contains("MA2"));
        assert(hs0.contains("SAP"));
        assert(hs0.contains("DBS"));

        assert(hs0.remove("PA1"));

        assert(hs0.contains("test0"));
        assert(hs0.contains("test1"));
        assert(hs0.contains("test2"));
        assert(hs0.contains("test3"));
        assert(hs0.contains("test5"));
        assert(hs0.contains("test6"));
        assert(hs0.contains("test7"));

        assert(!hs0.contains("PA1"));
        assert(hs0.contains("PA2"));
        assert(hs0.contains("MA2"));
        assert(hs0.contains("SAP"));
        assert(hs0.contains("DBS"));

        assert(hs0.remove("test2"));

        assert(hs0.contains("test0"));
        assert(hs0.contains("test1"));
        assert(!hs0.contains("test2"));
        assert(hs0.contains("test3"));
        assert(hs0.contains("test5"));
        assert(hs0.contains("test6"));
        assert(hs0.contains("test7"));

        assert(hs0.contains("PA2"));
        assert(hs0.contains("MA2"));
        assert(hs0.contains("SAP"));
        assert(hs0.contains("DBS"));

                auto it0 = hs0.begin();

                assert(it0 == hs0.begin());
                assert(it0 != hs0.end());
                assert(it0.m_Item == hs0.m_First);
                assert(*it0 == hs0.m_First->m_Key);
                assert(*(++it0) == hs0.m_First->m_NextOrder->m_Key);
                assert(*(++it0) == hs0.m_First->m_NextOrder->m_NextOrder->m_Key);
                assert(*(++it0) == hs0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_Key);
                assert(*(++it0) == hs0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key);
                assert(*(++it0) == hs0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key);
                assert(*(++it0) == hs0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key);
                assert(*(++it0) == hs0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key);
                assert(*(++it0) == hs0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key);
                assert(*(++it0) == hs0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key);
                assert(++it0 == hs0.end());
                assert(it0.m_Item == nullptr);

                auto it1 = hs0.begin();

                assert(it1 == hs0.begin());
                assert(it1 != hs0.end());
                assert(it1.m_Item == hs0.m_First);
                assert(*it1 == hs0.m_First->m_Key);
                assert(*(it1++) == hs0.m_First->m_Key);
                assert(*it1 == hs0.m_First->m_NextOrder->m_Key);
                assert(*(it1++) == hs0.m_First->m_NextOrder->m_Key);
                assert(*it1 == hs0.m_First->m_NextOrder->m_NextOrder->m_Key);
                assert(*(it1++) == hs0.m_First->m_NextOrder->m_NextOrder->m_Key);
                assert(*it1 == hs0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_Key);
                assert(*(it1++) == hs0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_Key);
                assert(*it1 == hs0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key);
                assert(*(it1++) == hs0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key);
                assert(*it1 == hs0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key);
                assert(*(it1++) == hs0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key);
                assert(*it1 == hs0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key);
                assert(*(it1++) == hs0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key);
                assert(*it1 == hs0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key);
                assert(*(it1++) == hs0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key);
                assert(*it1 == hs0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key);
                assert(*(it1++) == hs0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key);
                assert(*it1 == hs0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key);
                assert(*(it1++) == hs0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key);
                assert(it1 == hs0.end());
                assert(it1.m_Item == nullptr);

                auto it2 = hs0.begin();

                for (size_t i = 0; i < 4; ++i, ++it2)
                    ;

                assert(*it2 == "test0");

                assert(hs0.remove("test5"));

                assert(hs0.contains("test0"));
                assert(hs0.contains("test1"));
                assert(hs0.contains("test3"));
                assert(!hs0.contains("test5"));
                assert(hs0.contains("test6"));
                assert(hs0.contains("test7"));

                assert(hs0.contains("PA2"));
                assert(hs0.contains("MA2"));
                assert(hs0.contains("SAP"));
                assert(hs0.contains("DBS"));

                assert(*it2 == "test0");

                hs0.remove("MA2");

                assert(hs0.contains("test0"));
                assert(hs0.contains("test1"));
                assert(hs0.contains("test3"));
                assert(hs0.contains("test6"));
                assert(hs0.contains("test7"));

                assert(hs0.contains("PA2"));
                assert(!hs0.contains("MA2"));
                assert(hs0.contains("SAP"));
                assert(hs0.contains("DBS"));

                assert(*it2 == "test0");
    }
};

int main()
{
    CHashSetTest::test();

    return EXIT_SUCCESS;
}