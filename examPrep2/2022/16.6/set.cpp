#include <string>
#include <cassert>

using namespace std;

class CTree
{
protected:
    class CNode
    {
    public:
        CNode(string key) : m_Key(key) {}
        string m_Key;
        CNode *m_L = nullptr;
        CNode *m_R = nullptr;
        CNode *m_PrevOrder = nullptr;
        CNode *m_NextOrder = nullptr;

        ~CNode()
        {
            delete m_L;
            delete m_R;
        }
    };
    CNode *m_Root = nullptr;
    CNode *m_First = nullptr;
    CNode *m_Last = nullptr;

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
        CNode **node = &m_Root;

        while (*node)
        {
            if (key < (*node)->m_Key)
                node = &(*node)->m_L;
            else if (key > (*node)->m_Key)
                node = &(*node)->m_R;
            else
                return false; // key already exists
        }

        CNode *newNode = new CNode(key);
        newNode->m_PrevOrder = m_Last;

        if (m_Last)
            m_Last->m_NextOrder = newNode;
        else
            m_First = newNode;

        m_Last = newNode;

        *node = newNode;
        return true;
    }

    bool erase(string key)
    {
        CNode **node = &m_Root;

        while (*node)
        {
            if (key < (*node)->m_Key)
                node = &(*node)->m_L;
            else if (key > (*node)->m_Key)
                node = &(*node)->m_R;
            else
            {
                CNode *toDel = *node;

                if (toDel->m_L && toDel->m_R)
                {
                    node = &(*node)->m_R;

                    while ((*node)->m_L)
                        node = &(*node)->m_L;

                    toDel->m_Key = (*node)->m_Key;
                    toDel = *node;
                }

                if (toDel->m_L)
                    *node = toDel->m_L;
                else
                    *node = toDel->m_R;

                if (toDel->m_PrevOrder)
                    toDel->m_PrevOrder->m_NextOrder = toDel->m_NextOrder;
                else
                    m_First = toDel->m_NextOrder;

                if (toDel->m_NextOrder)
                    toDel->m_NextOrder->m_PrevOrder = toDel->m_PrevOrder;
                else
                    m_Last = toDel->m_PrevOrder;

                toDel->m_R = toDel->m_L = nullptr;
                delete toDel;
                return true;
            }
        }

        return false;
    }
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