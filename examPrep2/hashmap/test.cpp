#include <iostream>
#include <functional>
#include <cassert>
#include <sstream>

template <typename TKey, typename TValue, typename Hasher = std::hash<TKey>>
class HashMap
{
private:
    class linked_list
    {
    private:
        struct Node
        {
            TKey m_key;
            TValue m_value;
            Node *m_next;
        };

        Node *m_head = nullptr;

    public:
        bool insert(const TKey &key, const TValue &val)
        {
            if (contains(key))
                return false;

            m_head = new Node{key, val, m_head};
            return true;
        }

        bool remove(const TKey &key)
        {
            Node *head = m_head;
            Node *prev = nullptr;

            while (head)
            {
                if (head->m_key == key)
                    break;

                prev = head;
                head = head->m_next;
            }

            if (!head)
                return false;
            if (!prev)
                m_head = head->m_next;
            else
                prev->m_next = head->m_next;
            delete head;
            return true;
        }

        bool contains(const TKey &key) const
        {
            bool found = false;
            iterate([key, &found](const TKey &node_key, const TValue &)
                    {if (node_key == key) found = true; });
            return found;
        }

        TValue &getValue(const TKey &key)
        {
            const auto &t = *this;
            return const_cast<TValue &>(t.getValue(key));
        }

        const TValue &getValue(const TKey &key) const
        {
            for (Node *head = m_head; head; head = head->m_next)
            {
                if (head->m_key == key)
                    return head->m_value;
            }

            throw std::invalid_argument("key not found");
        }

        void iterate(const auto &func) const
        {
            for (Node *head = m_head; head; head = head->m_next)
                func(head->m_key, head->m_value);
        }
    };

    linked_list *m_table;
    size_t m_capacity;
    size_t m_inserted_count;
    Hasher m_hasher;

private:
    size_t getIndex(const TKey &key) const
    {
        return m_hasher(key) % m_capacity;
    }

    void resize(size_t cap)
    {
        HashMap new_map{cap, Hasher(m_hasher)};
        new_map.clone(*this);
        std::swap(new_map.m_capacity, m_capacity);
        std::swap(new_map.m_table, m_table);
    }

    void clone(const HashMap &other)
    {
        for (size_t i = 0; i < other.m_capacity; ++i)
        {
            other.m_table[i].iterate([&](const TKey &key, const TValue &value)
                                     { insert(key, value); });
        }
    }

public:
    HashMap(size_t capacity, Hasher &&hasher = Hasher{})
        : m_table(new linked_list[capacity]{}),
          m_capacity(capacity),
          m_inserted_count(0),
          m_hasher(std::move(hasher)) {}

    HashMap(const HashMap &other)
        : HashMap(other.m_capacity)
    {
        clone(other);
    }

    ~HashMap()
    {
        delete[] m_table;
    }

    bool contains(const TKey &key) const
    {
        auto idx = getIndex(key);
        return m_table[idx].contains(key);
    }

    bool insert(const TKey &key, const TValue &val)
    {
        if (m_inserted_count >= m_capacity * 3 / 4)
            resize(m_capacity * 3 / 2 + 1);

        auto idx = getIndex(key);
        auto ret_val = m_table[idx].insert(key, val);

        if (ret_val)
            m_inserted_count++;
        return ret_val;
    }

    bool remove(const TKey &key)
    {
        if (!contains(key))
            return false;

        auto idx = getIndex(key);
        auto ret_val = m_table[idx].remove(key);

        if (ret_val)
            m_inserted_count--;

        return ret_val;
    }

    bool setValue(const TKey &key, const TValue &val)
    {
        if (!contains(key))
            return false;

        getValue(key) = val;
        return true;
    }

    const TValue &getValue(const TKey &key) const
    {
        auto idx = getIndex(key);
        return m_table[idx].getValue(key);
    }

    TValue &getValue(const TKey &key)
    {
        const auto &t = *this;
        return const_cast<TValue &>(t.getValue(key));
    }
};

void testInts()
{
    HashMap<int, int, std::hash<int>> map{10};

    assert(map.contains(0) == false);
    assert(map.insert(0, 0));
    assert(map.contains(0));
    assert(map.getValue(0) == 0);

    assert(map.insert(0, 1) == false);
    assert(map.getValue(0) == 0);

    assert(map.remove(0));
    assert(map.remove(0) == false);
    assert(map.insert(0, 1));
    assert(map.getValue(0) == 1);

    assert(map.setValue(0, 0));
    assert(map.getValue(0) == 0);
}

void sieveOfEratosthenesTest()
{
    HashMap<int, bool, std::hash<int>> primes{1};
    int N = 100;
    for (int i = 2; i < N; i++)
    {
        if (primes.contains(i))
            continue;
        primes.insert(i, true);
        for (int k = 2 * i; k < N; k += i)
            primes.insert(k, false);
    }

    std::stringstream ss;
    for (int i = 2; i < N; i++)
        if (primes.contains(i) && primes.getValue(i))
            ss << i << " ";

    assert(ss.str() == "2 3 5 7 11 13 17 19 23 29 31 37 41 43 47 53 59 61 67 71 73 79 83 89 97 ");
}

int main()
{
    testInts();
    sieveOfEratosthenesTest();
}
