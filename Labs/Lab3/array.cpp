#include <iostream>
#include <iomanip>
#include <cassert>

/*
Create an Array class representing a dynamically allocated array of integers.
The class shall support at least insertion of a new element at any position,
erasure of any element at any position,
access to any element given its index (to change the element or to just read it in case of a constant object),
and a method to get the number of elements in the container.
*/
class array
{
private:
    int *m_array = nullptr;
    size_t m_size = 0;
    size_t m_capacity = 0;

    void ensure(size_t requested)
    {
        if (requested <= m_capacity)
            return;

        m_capacity = m_capacity * 2 + 1;
        m_capacity = std::max(m_capacity, requested);

        int *temp = new int[m_capacity];
        memcpy(temp, m_array, sizeof(*m_array) * m_size);
        delete[] m_array;
        m_array = temp;
    }

public:
    // destructor
    ~array()
    {
        delete[] m_array;
    }

    void insert(int data, size_t pos)
    {
        if (pos > m_size)
            throw std::invalid_argument("Position out of bounds");

        ensure(m_size + 1);

        for (size_t i = m_size; i > pos; --i)
        {
            m_array[i] = m_array[i - 1];
        }

        m_array[pos] = data;
        m_size++;
    }

    void erase(size_t pos)
    {
        if (pos >= m_size)
            throw std::invalid_argument("Position out of bounds");

        for (size_t i = pos + 1; i < m_size; ++i)
        {
            m_array[i - 1] = m_array[i];
        }

        m_size--;
    }

    int &access(size_t index)
    {
        if (index >= m_size)
            throw std::invalid_argument("Index out of bounds");

        return m_array[index];
    }

    int access(size_t index) const
    {
        if (index >= m_size)
            throw std::invalid_argument("Index out of bounds");

        return m_array[index];
    }

    size_t size() const
    {
        return m_size;
    }

    friend std::ostream &operator<<(std::ostream &os, const array &inst);
};

std::ostream &operator<<(std::ostream &os, const array &inst)
{
    os << '[';
    for (size_t i = 0; i < inst.m_size; ++i)
    {
        if (i != 0)
            os << ", ";
        os << inst.m_array[i];
    }

    return os << ']';
}

int main()
{
    array a;
    a.insert(10, 0);
    a.insert(5, 1);
    a.insert(15, 0);

    std::cout << a << std::endl;

    assert(a.size() == 3);
    assert(a.access(0) == 15);
    assert(a.access(1) == 10);
    assert(a.access(2) == 5);

    a.erase(1);

    assert(a.size() == 2);
    assert(a.access(0) == 15);
    assert(a.access(1) == 5);

    a.access(0) = 10;
    assert(a.access(0) == 10);

    return 0;
}