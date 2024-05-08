#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>

using namespace std;

class CAnimal;
using AnimalPtr = std::unique_ptr<CAnimal>;

class CAnimal
{
protected:
    string m_name;
    double m_weight;

    string nutrition() const
    {
        return "water: " + to_string(water() * m_weight) + " meat: " + to_string(meat()) + " hay: " + to_string(hay());
    }

public:
    CAnimal(const string &name, double weight)
        : m_name(name), m_weight(weight) {}

    virtual ~CAnimal() noexcept = default;

    virtual AnimalPtr clone() const = 0;

    virtual double meat() const
    {
        return 0;
    }

    virtual double hay() const
    {
        return 0;
    }

    virtual double water() const
    {
        return 0;
    }

    virtual ostream &print(ostream &os) const = 0;
};

class CPredator : public CAnimal
{
public:
    CPredator(const string &name, double weight)
        : CAnimal(name, weight) {}

    virtual AnimalPtr clone() const override = 0;

    double water() const override
    {
        return 0.01;
    }

    virtual ostream &print(ostream &os) const override = 0;
};

class CHerbivore : public CAnimal
{
public:
    CHerbivore(const string &name, double weight)
        : CAnimal(name, weight) {}

    virtual AnimalPtr clone() const override = 0;

    double water() const override
    {
        return 0.02;
    }

    virtual ostream &print(ostream &os) const override = 0;
};

class CLion : public CPredator
{
public:
    CLion(const string &name, double weight)
        : CPredator(name, weight) {}

    AnimalPtr clone() const override
    {
        return std::make_unique<CLion>(*this);
    }

    double meat() const override
    {
        return 3;
    }

    ostream &print(ostream &os) const override
    {
        return os << m_name << "(" << m_weight << "): " << nutrition();
    }
};

class CTiger : public CPredator
{
public:
    CTiger(const string &name, double weight)
        : CPredator(name, weight) {}

    AnimalPtr clone() const override
    {
        return std::make_unique<CTiger>(*this);
    }

    double meat() const override
    {
        return 2;
    }

    ostream &print(ostream &os) const override
    {
        return os << m_name << "(" << m_weight << "): " << nutrition();
    }
};

class CElephant : public CHerbivore
{
public:
    CElephant(const string &name, double weight)
        : CHerbivore(name, weight) {}

    AnimalPtr clone() const override
    {
        return std::make_unique<CElephant>(*this);
    }

    double hay() const override
    {
        return 20;
    }

    ostream &print(ostream &os) const override
    {
        return os << m_name << "(" << m_weight << "): " << nutrition();
    }
};

class CZebra : public CHerbivore
{
public:
    CZebra(const string &name, double weight)
        : CHerbivore(name, weight) {}

    AnimalPtr clone() const override
    {
        return std::make_unique<CZebra>(*this);
    }

    double hay() const override
    {
        return 3;
    }

    ostream &print(ostream &os) const override
    {
        return os << m_name << "(" << m_weight << "): " << nutrition();
    }
};

ostream &operator<<(ostream &os, const CAnimal &animal)
{
    return animal.print(os);
}

class CZoo
{
private:
    vector<AnimalPtr> m_db;

public:
    CZoo() = default;

    CZoo(const CZoo &other)
    {
        for (const auto &ptr : other.m_db)
            m_db.push_back(ptr->clone());
    }

    CZoo(CZoo &&other)
        : m_db(std::move(other.m_db)) {}

    CZoo &operator=(CZoo other)
    {
        swap(m_db, other.m_db);
        return *this;
    }

    ~CZoo() = default;

    void push(const CAnimal &animal)
    {
        m_db.push_back(animal.clone());
    }

    friend ostream &operator<<(ostream &os, const CZoo &src)
    {
        for (const auto &animal : src.m_db)
            os << *animal << endl;
        return os;
    }
};

int main()
{
    CZoo zoo;
    zoo.push(CLion("Laith", 50));
    zoo.push(CTiger("Caramel", 80));
    zoo.push(CLion("Yazan", 100));
    zoo.push(CElephant("Louai", 60));
    zoo.push(CZebra("Mohannad", 120));
    zoo.push(CZebra("Saif", 500));

    cout << zoo;
    return EXIT_SUCCESS;
}