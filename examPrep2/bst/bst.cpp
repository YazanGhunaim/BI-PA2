#include <iostream>
#include <memory>
#include <algorithm>
#include <cassert>
#include <string>

using namespace std;

template <typename T>
class CBST
{
private:
    struct CNode
    {
        T m_val;
        CNode *m_left;
        CNode *m_right;

        CNode(const T &val, CNode *left, CNode *right)
            : m_val(val), m_left(left), m_right(right) {}

        ~CNode()
        {
            delete m_left;
            delete m_right;
        }
    };

    CNode *m_root;

public:
    CBST() : m_root(nullptr)
    {
    }

    CBST(const CBST &other)
    {
        swap(*m_root, *other.m_root);
    }

    CBST(const CBST &&other) : m_root(std::move(other.m_root))
    {
    }

    CBST &operator=(const CBST other)
    {
        swap(*m_root, *other.m_root);
        return *this;
    }

    ~CBST()
    {
        delete m_root;
    }

    bool insert(T val)
    {
        CNode **node = &m_root;

        while (*node)
        {
            if (val < (*node)->m_val)
            {
                node = &(*node)->m_left;
            }
            else if (val > (*node)->m_val)
            {
                node = &(*node)->m_right;
            }
            else
            {
                return false;
            }
        }

        *node = new CNode(val, nullptr, nullptr);
        return true;
    }

    bool search(T val) const
    {
        CNode *node = m_root;

        while (node)
        {
            if (val < node->m_val)
            {
                node = node->m_left;
            }
            else if (val > node->m_val)
            {
                node = node->m_right;
            }
            else
            {
                return true;
            }
        }

        return false;
    }

    bool remove(T val)
    {
        CNode **node = &m_root;

        while (*node)
        {
            if (val < (*node)->m_val)
            {
                node = &(*node)->m_left;
            }
            else if (val > (*node)->m_val)
            {
                node = &(*node)->m_right;
            }
            else
            {
                CNode *toDel = *node;
                // two children
                if ((*node)->m_left && (*node)->m_right)
                {
                    node = &(*node)->m_left;

                    while ((*node)->m_right)
                        node = &(*node)->m_right;

                    toDel->m_val = (*node)->m_val;

                    toDel = *node;
                }

                // one child
                if (toDel->m_left)
                {
                    *node = toDel->m_left;
                }
                // no childern
                else
                {
                    *node = toDel->m_right;
                }

                toDel->m_left = toDel->m_right = nullptr;
                delete toDel;
                return true;
            }
        }

        return false;
    }
};

int main()
{
    CBST<int> tree;
    assert(tree.insert(5));
    assert(tree.insert(1));
    assert(tree.insert(2));
    assert(tree.insert(3));
    assert(tree.insert(10));
    assert(tree.insert(4));
    assert(!tree.insert(3));

    assert(tree.search(5));
    assert(tree.search(10));
    assert(tree.search(4));
    assert(tree.search(3));
    assert(!tree.search(0));
    assert(!tree.search(11));

    assert(tree.remove(5));
    assert(!tree.search(5));

    CBST<string> tree2;
    assert(tree2.insert("yazan"));
    assert(tree2.insert("louai"));

    assert(tree2.search("yazan"));
    assert(tree2.search("louai"));
    assert(!tree2.search("saif"));

    assert(tree2.remove("yazan"));
    assert(!tree2.search("yazan"));

    return EXIT_SUCCESS;
}