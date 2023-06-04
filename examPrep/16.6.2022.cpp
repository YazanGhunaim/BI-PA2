/**
 * Implement the method insert (insertion), erase (deleting elements
 * from the tree and at the same time it is necessary to keep the correct order according to
 * the date of insertion) and a custom destructor, the other methods are
 * implemented. Both copy constructor and assignment operator
 * disabled. You are not allowed to add any member variables
 * but you can implement your own helper methods.
 *
 * I am not responsible for the correctness of the template :-).
 */

#include <string>
#include <cassert>
#include <iostream>

using namespace std;

class CTree
{
public:
    CTree() = default;
    ~CTree()
    {
        delete m_Root;
    }
    CTree(const CTree &src) = delete;
    bool operator=(const CTree &other) = delete;
    bool isSet(string key)
    {
        CNode *n = m_Root;
        while (n)
        {
            if (key == n->m_Key)
                return true;
            else if (key > n->m_Key)
                n = n->m_R;
            else
                n = n->m_L;
        }
        return false;
    }
    bool insert(string key)
    {
        CNode **temp = &m_Root;
        while (*temp)
        {
            if (key == (*temp)->m_Key)
                return false;
            else if (key < (*temp)->m_Key)
                temp = &((*temp)->m_L);
            else
                temp = &((*temp)->m_R);
        }

        auto newNode = new CNode{key};
        newNode->m_PrevOrder = m_Last;

        if (!m_Last)
            m_First = newNode;
        else
            m_Last->m_NextOrder = newNode;

        m_Last = newNode;
        *temp = newNode;
        return true;
    }
    bool erase(string key)
    {
        CNode **temp = &m_Root;

        while (*temp)
        {
            if (key == (*temp)->m_Key)
            {
                if ((*temp)->m_L && (*temp)->m_R)
                {
                    CNode *overwrite = *temp;
                    temp = &((*temp)->m_R);

                    while ((*temp)->m_L)
                        temp = &((*temp)->m_L);

                    overwrite->m_Key = (*temp)->m_Key;
                }

                CNode *toDelete = *temp;

                if (toDelete->m_L)
                    *temp = toDelete->m_L;
                else
                    *temp = toDelete->m_R;

                toDelete->m_L = toDelete->m_R = nullptr;

                if (toDelete->m_PrevOrder)
                    toDelete->m_PrevOrder->m_NextOrder = toDelete->m_NextOrder;
                else
                    m_First = toDelete->m_NextOrder;

                if (toDelete->m_NextOrder)
                    toDelete->m_NextOrder->m_PrevOrder = toDelete->m_PrevOrder;
                else
                    m_Last = toDelete->m_PrevOrder;

                delete toDelete;
                return true;
            }
            else if (key < (*temp)->m_Key)
                temp = &((*temp)->m_L);
            else
                temp = &((*temp)->m_R);
        }
        return false;
    }

protected:
    class CNode
    {
    public:
        CNode(string key) : m_Key(key) {}
        ~CNode()
        {
            delete m_L;
            delete m_R;
        }
        string m_Key;
        CNode *m_L = nullptr;
        CNode *m_R = nullptr;
        CNode *m_PrevOrder = nullptr;
        CNode *m_NextOrder = nullptr;
    };
    CNode *m_Root = nullptr;
    CNode *m_First = nullptr;
    CNode *m_Last = nullptr;
};

class CTester : public CTree
{
public:
    CTester() = default;
    void test()
    {
        CTester t0;
        assert(t0.insert("PA1") == true);
        assert(t0.m_First->m_Key == "PA1" && t0.m_First->m_NextOrder == nullptr);
        assert(t0.isSet("PA1") == true);
        assert(t0.insert("UOS") == true);
        assert(t0.insert("PA2") == true);
        assert(t0.isSet("PA2") == true);
        assert(t0.isSet("PA3") == false);

        assert(t0.insert("PA2") == false);
        assert(t0.insert("CAO") == true);
        assert(t0.insert("LIN") == true);
        assert(t0.insert("AAG") == true);
        assert(t0.insert("AG1") == true);
        assert(t0.insert("ZDM") == true);

        assert(t0.m_First->m_Key == "PA1" && t0.m_First->m_NextOrder->m_Key == "UOS" && t0.m_First->m_NextOrder->m_NextOrder->m_Key == "PA2" && t0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_Key == "CAO" && t0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key == "LIN" && t0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key == "AAG" && t0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key == "AG1" && t0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key == "ZDM");

        assert(t0.m_Last->m_Key == "ZDM" && t0.m_Last->m_PrevOrder->m_Key == "AG1" && t0.m_Last->m_PrevOrder->m_PrevOrder->m_Key == "AAG" && t0.m_Last->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_Key == "LIN" && t0.m_Last->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_Key == "CAO" && t0.m_Last->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_Key == "PA2" && t0.m_Last->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_Key == "UOS" && t0.m_Last->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_Key == "PA1");

        assert(t0.erase("") == false);

        assert(t0.erase("ZDM") == true);
        assert(t0.m_First->m_Key == "PA1" && t0.m_First->m_NextOrder->m_Key == "UOS" && t0.m_First->m_NextOrder->m_NextOrder->m_Key == "PA2" && t0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_Key == "CAO" && t0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key == "LIN" && t0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key == "AAG" && t0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key == "AG1");
        assert(t0.m_Last->m_Key == "AG1" && t0.m_Last->m_PrevOrder->m_Key == "AAG" && t0.m_Last->m_PrevOrder->m_PrevOrder->m_Key == "LIN" && t0.m_Last->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_Key == "CAO" && t0.m_Last->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_Key == "PA2" && t0.m_Last->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_Key == "UOS" && t0.m_Last->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_Key == "PA1");
        assert(t0.isSet("ZDM") == false);

        assert(t0.erase("AAG") == true);
        assert(t0.m_First->m_Key == "PA1" && t0.m_First->m_NextOrder->m_Key == "UOS" && t0.m_First->m_NextOrder->m_NextOrder->m_Key == "PA2" && t0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_Key == "CAO" && t0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key == "LIN" && t0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key == "AG1");
        assert(t0.m_Last->m_Key == "AG1" && t0.m_Last->m_PrevOrder->m_Key == "LIN" && t0.m_Last->m_PrevOrder->m_PrevOrder->m_Key == "CAO" && t0.m_Last->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_Key == "PA2" && t0.m_Last->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_Key == "UOS" && t0.m_Last->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_Key == "PA1");
        assert(t0.isSet("AAG") == false);

        assert(t0.erase("CAO") == true);
        assert(t0.m_First->m_Key == "PA1" && t0.m_First->m_NextOrder->m_Key == "UOS" && t0.m_First->m_NextOrder->m_NextOrder->m_Key == "PA2" && t0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_Key == "LIN" && t0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_NextOrder->m_Key == "AG1");
        assert(t0.m_Last->m_Key == "AG1" && t0.m_Last->m_PrevOrder->m_Key == "LIN" && t0.m_Last->m_PrevOrder->m_PrevOrder->m_Key == "PA2" && t0.m_Last->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_Key == "UOS" && t0.m_Last->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_Key == "PA1");
        assert(t0.isSet("CAO") == false);

        assert(t0.erase("UOS") == true);
        assert(t0.m_First->m_Key == "PA1" && t0.m_First->m_NextOrder->m_Key == "PA2" && t0.m_First->m_NextOrder->m_NextOrder->m_Key == "LIN" && t0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_Key == "AG1");
        assert(t0.m_Last->m_Key == "AG1" && t0.m_Last->m_PrevOrder->m_Key == "LIN" && t0.m_Last->m_PrevOrder->m_PrevOrder->m_Key == "PA2" && t0.m_Last->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_Key == "PA1");
        assert(t0.isSet("UOS") == false);

        assert(t0.erase("UOS") == false);
        assert(t0.m_First->m_Key == "PA1" && t0.m_First->m_NextOrder->m_Key == "PA2" && t0.m_First->m_NextOrder->m_NextOrder->m_Key == "LIN" && t0.m_First->m_NextOrder->m_NextOrder->m_NextOrder->m_Key == "AG1");
        assert(t0.m_Last->m_Key == "AG1" && t0.m_Last->m_PrevOrder->m_Key == "LIN" && t0.m_Last->m_PrevOrder->m_PrevOrder->m_Key == "PA2" && t0.m_Last->m_PrevOrder->m_PrevOrder->m_PrevOrder->m_Key == "PA1");
        assert(t0.isSet("UOS") == false);

        assert(t0.erase("LIN") == true);
        assert(t0.m_First->m_Key == "PA1" && t0.m_First->m_NextOrder->m_Key == "PA2" && t0.m_First->m_NextOrder->m_NextOrder->m_Key == "AG1");
        assert(t0.m_Last->m_Key == "AG1" && t0.m_Last->m_PrevOrder->m_Key == "PA2" && t0.m_Last->m_PrevOrder->m_PrevOrder->m_Key == "PA1");
        assert(t0.isSet("LIN") == false);

        assert(t0.erase("PA1") == true);
        assert(t0.m_First->m_Key == "PA2" && t0.m_First->m_NextOrder->m_Key == "AG1");
        assert(t0.m_Last->m_Key == "AG1" && t0.m_Last->m_PrevOrder->m_Key == "PA2");
        assert(t0.isSet("PA1") == false);

        assert(t0.erase("PA2") == true);
        assert(t0.m_First->m_Key == "AG1");
        assert(t0.m_Last->m_Key == "AG1");
        assert(t0.isSet("PA2") == false);

        assert(t0.erase("AG1") == true);
        assert(t0.m_First == t0.m_Last);
        assert(t0.isSet("AG1") == false);
    }
};

int main()
{
    CTester t;
    t.test();

    return EXIT_SUCCESS;
}