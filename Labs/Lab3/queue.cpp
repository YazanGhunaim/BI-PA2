#include <iostream>
#include <iomanip>

class m_Queue
{
private:
    struct node
    {
        int m_val;
        node *m_next;

        node(int val, node *next) : m_val(val), m_next(next) {}
    };
    node *m_head = nullptr;
    node *m_tail = nullptr;

public:
    ~m_Queue()
    {
        while (!empty())
        {
            dequeue();
        }
    }

    void enqueue(int val)
    {
        if (empty())
        {
            m_tail = m_head = new node(val, nullptr);
        }
        else
        {
            m_tail->m_next = new node(val, nullptr);
            m_tail = m_tail->m_next;
        }
    }

    int dequeue()
    {
        int result = top();

        node *temp = m_head;
        m_head = m_head->m_next;

        delete temp;
        if (m_head == nullptr)
        {
            m_tail = nullptr;
        }
        return result;
    }

    int top() const
    {
        if (empty())
        {
            throw std::logic_error("Queue is empty");
        }
        return m_head->m_val;
    }
    bool empty() const
    {
        return m_head == nullptr;
    }
};

int main()
{
    m_Queue ql;
    for (int i = 0; i < 10; ++i)
    {
        ql.enqueue(i);
    }

    assert(!ql.empty());

    for (int i = 0; i < 10; ++i)
    {
        assert(ql.dequeue() == i);
    }

    assert(ql.empty());

    ql.enqueue(1);
    return 0;
}