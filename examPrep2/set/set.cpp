#include <iostream>
#include <cassert>
#include <compare>
#include <memory>
#include <algorithm>

template <typename T>
class CSet
{
private:
    struct CNode
    {
        T m_value;
        CNode *m_left;
        CNode *m_right;

        CNode(const T &val, CNode *left = nullptr, CNode *right = nullptr)
            : m_value(val), m_left(left), m_right(right) {}

        CNode(const CNode &src)
            : m_value(src.m_value),
              m_left(src.m_left ? new CNode(*src.m_left) : nullptr),
              m_right(src.m_right ? new CNode(*src.m_right) : nullptr) {}

        CNode &operator=(CNode other)
        {
            std::swap(m_value, other.m_value);
            std::swap(m_left, other.m_left);
            std::swap(m_right, other.m_right);
            return *this;
        }

        CNode(CNode &&src) noexcept
            : m_value(std::move(src.m_value)), m_left(std::move(src.m_left)), m_right(std::move(src.m_right))
        {
            m_left = m_right = nullptr; // valid but unspecified state
        }

        ~CNode()
        {
            delete m_left;
            delete m_right;
        }
    };

    CNode *m_root;
    size_t m_size;

private:
    CNode *deepcopy(CNode *node)
    {
        if (!node)
            return nullptr;

        CNode *newNode = new CNode(node->m_value);

        newNode->m_left = deepcopy(node->m_left);
        newNode->m_right = deepcopy(node->m_right);

        return newNode;
    }

public:
    CSet()
        : m_root(nullptr), m_size(0) {}

    CSet(const CSet &src)
    {
        m_root = deepcopy(src.m_root);
        m_size = src.m_size;
    }

    CSet &operator=(CSet other)
    {
        std::swap(m_root, other.m_root);
        std::swap(m_size, other.m_size);
        return *this;
    }

    CSet(CSet &&src) noexcept
        : m_root(std::move(src.m_root)), m_size(std::move(src.m_size))
    {
        m_root = nullptr;
        m_size = 0;
    }

    ~CSet()
    {
        delete m_root;
    }

    bool insert(const T &val)
    {
        CNode **node = &m_root;

        while (*node)
        {
            if (val == (*node)->m_value)
                return false; // no dupes allowed

            if (val < (*node)->m_value)
                node = &(*node)->m_left;

            else
                node = &(*node)->m_right;
        }

        *node = new CNode(val);
        ++m_size;
        return true;
    }

    bool erase(const T &val)
    {
        CNode **node = &m_root;

        while ((*node))
        {
            if (val < (*node)->m_value)
                node = &(*node)->m_left;
            else if (val > (*node)->m_value)
                node = &(*node)->m_right;
            else
            {
                CNode *toDel = *node;
                if ((*node)->m_left && (*node)->m_right)
                {
                    toDel = toDel->m_left;

                    while (toDel)
                        toDel = toDel->m_right;

                    (*node)->m_value = toDel->m_value;
                }
                if ((*node)->m_left)
                    *node = toDel->m_left;
                else
                    *node = toDel->m_right;

                toDel->m_left = toDel->m_right = nullptr;
                delete toDel;

                --m_size;
                return true;
            }
        }
        return false;
    }

    bool contains(const T &val) const
    {
        CNode *node = m_root;

        while (node)
        {
            if (val == node->m_value)
                return true;

            if (val < node->m_value)
                node = node->m_left;

            else
                node = node->m_right;
        }

        return false;
    }

    void clear()
    {
        CSet newSet;
        *this = newSet;
        m_size = 0;
    }

    size_t size() const
    {
        return m_size;
    }

    bool empty() const
    {
        return m_size == 0;
    }
};

int main()
{
    CSet<int> mySet;

    // Test insertion
    assert(mySet.insert(5));  // Insert should return true
    assert(mySet.insert(10)); // Insert should return true
    assert(mySet.insert(15)); // Insert should return true
    assert(!mySet.insert(5)); // Duplicate insert should return false
    assert(mySet.contains(5));
    assert(mySet.contains(10));
    assert(mySet.contains(15));
    assert(!mySet.contains(20));
    assert(mySet.size() == 3);
    assert(!mySet.empty());

    // Test deletion
    mySet.erase(10);
    assert(!mySet.contains(10));
    assert(mySet.contains(5));
    assert(mySet.contains(15));
    assert(mySet.size() == 2);

    // Test clear
    mySet.clear();
    assert(mySet.empty());
    assert(mySet.size() == 0);

    // Test re-insertion after clear
    assert(mySet.insert(20)); // Insert should return true
    assert(mySet.contains(20));
    assert(mySet.size() == 1);

    // Test removal of last element
    mySet.erase(20);
    assert(!mySet.contains(20));
    assert(mySet.empty());
    assert(mySet.size() == 0);

    // Test insertion of multiple elements and for-each loop
    assert(mySet.insert(1));
    assert(mySet.insert(2));
    assert(mySet.insert(3));
    assert(mySet.insert(4));
    assert(mySet.insert(5));
    assert(mySet.size() == 5);

    std::cout << "All assertions passed!" << std::endl;
    return 0;
}
