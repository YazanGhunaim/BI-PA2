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

        auto newNode = new CNode(key);
        addToList(newNode);
        *temp = newNode;
        return true;
    }
    bool erase(string key)
    {
        CNode **trav = &m_Root;

        while (*trav)
        {
            if (key == (*trav)->m_Key)
            {
                // two children
                if ((*trav)->m_L && (*trav)->m_R)
                {
                    CNode *overWrite = *trav;

                    trav = &((*trav)->m_R);
                    while ((*trav)->m_L)
                        trav = &((*trav)->m_L);

                    overWrite->m_Key = (*trav)->m_Key;
                }

                CNode *toDelete = *trav;
                if (toDelete->m_L)
                    *trav = toDelete->m_L;
                else
                    *trav = toDelete->m_R;

                toDelete->m_L = toDelete->m_R = nullptr;

                removeFromList(toDelete);
                return true;
            }
            else if (key < (*trav)->m_Key)
                trav = &((*trav)->m_L);
            else
                trav = &((*trav)->m_R);
        }
        return false;
    }
    void print() const
    {
        CNode *temp = m_First;
        bool first = true;
        while (temp)
        {
            if (first)
            {
                cout << "first :";
                first = false;
            }
            cout << temp->m_Key;
            if (temp->m_PrevOrder)
                cout << " prev : " << temp->m_PrevOrder->m_Key;

            cout << endl;
            temp = temp->m_NextOrder;
        }

        cout << "first : " << m_First->m_Key << " last : " << m_Last->m_Key << endl;
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

private:
    void addToList(CNode *newNode)
    {
        if (!m_First)
            m_First = newNode;
        else
        {
            CNode *temp = m_First;
            while (temp && temp->m_NextOrder)
                temp = temp->m_NextOrder;

            CNode *prev = temp;
            temp->m_NextOrder = newNode;
            temp = temp->m_NextOrder;
            temp->m_PrevOrder = prev;

            m_Last = newNode;
        }
    }

    void removeFromList(CNode *to_delete)
    {
        if (m_First == to_delete)
            m_First = to_delete->m_NextOrder;

        if (m_Last == to_delete)
            m_Last = to_delete->m_PrevOrder;

        if (to_delete->m_PrevOrder)
            to_delete->m_PrevOrder->m_NextOrder = to_delete->m_NextOrder;

        if (to_delete->m_NextOrder)
            to_delete->m_NextOrder->m_PrevOrder = to_delete->m_PrevOrder;

        delete to_delete;
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